#include "modulemanager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDateTime>
#include <QStandardPaths>
#include <QJsonObject>
#include"constants.h"
#include<QCoreApplication>

ModuleManager& ModuleManager::instance()
{
    static ModuleManager manager;
    return manager;
}

ModuleManager::ModuleManager(QObject* parent)
    : QObject(parent)
{
    loadModules();
}

void ModuleManager::loadModules()
{
    createDefaultModules();
    loadCustomModules();
    loadUserData();
}



void ModuleManager::createDefaultModules()
{

    QStringList moduleFiles = {
        ":/modules/chenrezig.json",
        ":/modules/tara.json",
        ":/modules/guru.json",
        ":/modules/manjushri.json"
    };
    for (const QString& filePath : moduleFiles) {
        DeityModule* module = loadModuleFromJson(filePath);
        if (module) {
            m_modules.append(module);
        }
    }
}

DeityModule* ModuleManager::loadModuleFromJson(const QString& jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) return nullptr;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();

    /*
    DeityModule* module = new DeityModule(
        obj["id"].toString(),
        obj["name"].toString(),
        obj["mantraText"].toString(),
        obj["openingText"].toString(),
        obj["closingText"].toString(),
        obj["defaultImagePath"].toString()
    );
*/

    QString imagePath = obj["defaultImagePath"].toString();

    // If it's NOT a Qt resource, make it absolute inside Flatpak
    if (!imagePath.startsWith(":/")) {
    #ifdef FLATPAK_BUILD
        imagePath = QCoreApplication::applicationDirPath() + "/" + imagePath;
    #endif
    }

    DeityModule* module = new DeityModule(
        obj["id"].toString(),
        obj["name"].toString(),
        obj["mantraText"].toString(),
        obj["openingText"].toString(),
        obj["closingText"].toString(),
        imagePath
    );

    module->setDeityInfo(obj["deityInfo"].toString());

    // Load stages from JSON
    QVector<Stage> stages;
    QJsonArray stagesArray = obj["stages"].toArray();
    for (const QJsonValue& val : stagesArray) {
        Stage stage;
        stage.name = val["name"].toString();
        stage.liturgyText = val["liturgyText"].toString();
        stage.mantraText = val["mantraText"].toString();
        stage.showImage = val["showImage"].toBool();
        stage.showMantra = val["showMantra"].toBool();
        stages.append(stage);
    }
    module->setStages(stages);

    return module;
}

void ModuleManager::saveModules()
{
    saveUserData();
}

DeityModule *ModuleManager::getModule(const QString &id) const
{
    for (DeityModule* module : m_modules) {
            if (module->id() == id) {
                return module;
            }
        }
        return nullptr;
}

void ModuleManager::saveUserData()
{
    QString dataPath = getDataPath();
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // Save module data (audio paths, image overrides, lifetime counts)
    QJsonObject rootObject;

    for (DeityModule* module : m_modules) {
        QJsonObject moduleObject;
        moduleObject["audioPath"] = module->audioPath();
        moduleObject["userImagePath"] = module->userImagePath();
        moduleObject["lifetimeCount"] = module->lifetimeCount();
        rootObject[module->id()] = moduleObject;
    }

    // Save journal entries
    QJsonObject journalObject;
    for (auto it = m_journalEntries.begin(); it != m_journalEntries.end(); ++it) {
        QJsonArray entriesArray;
        for (const QMap<QString, QString>& entry : it.value()) {
            QJsonObject entryObject;
            entryObject["date"] = entry["date"];
            entryObject["count"] = entry["count"];
            entryObject["notes"] = entry["notes"];
            entriesArray.append(entryObject);
        }
        journalObject[it.key()] = entriesArray;
    }
    rootObject["journals"] = journalObject;

    // Write to file
    QFile file(dataPath + "puja_data.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(rootObject);
        file.write(doc.toJson());
        file.close();
    }
}

void ModuleManager::loadUserData()
{
    QString dataPath = getDataPath();
    QFile file(dataPath + "puja_data.json");

    if (!file.exists()) {
        return; // No saved data yet
    }

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        QJsonObject rootObject = doc.object();

        // Load module data
        for (DeityModule* module : m_modules) {
            if (rootObject.contains(module->id())) {
                QJsonObject moduleObject = rootObject[module->id()].toObject();

                if (moduleObject.contains("audioPath")) {
                    module->setAudioPath(moduleObject["audioPath"].toString());
                }
                if (moduleObject.contains("userImagePath")) {
                    module->setUserImagePath(moduleObject["userImagePath"].toString());
                }
                if (moduleObject.contains("lifetimeCount")) {
                    module->setLifetimeCount(moduleObject["lifetimeCount"].toInt());
                }
            }
        }

        // Load journal entries
        if (rootObject.contains("journals")) {
            QJsonObject journalObject = rootObject["journals"].toObject();
            for (const QString& moduleId : journalObject.keys()) {
                QJsonArray entriesArray = journalObject[moduleId].toArray();
                QVector<QMap<QString, QString>> entries;

                for (const QJsonValue& val : entriesArray) {
                    QJsonObject entryObject = val.toObject();
                    QMap<QString, QString> entry;
                    entry["date"] = entryObject["date"].toString();
                    entry["count"] = entryObject["count"].toString();
                    entry["notes"] = entryObject["notes"].toString();
                    entries.append(entry);
                }
                m_journalEntries[moduleId] = entries;
            }
        }
    }
}

QString ModuleManager::getDataPath() const
{
    QString dataPath = Constants::defaultModuleDataPath + "/";
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dataPath;
}

void ModuleManager::loadCustomModules()
{
    QString path = getCustomModulesPath();
    QDir dir(path);
    QStringList filters;
    filters << "*.json";

    for (const QString& filename : dir.entryList(filters, QDir::Files)) {
        QFile file(path + filename);
        if (!file.open(QIODevice::ReadOnly)) continue;

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject json = doc.object();

        QString id = json["id"].toString();
        QString name = json["name"].toString();

        CustomModule* module = new CustomModule(id, name);
        module->setUserImagePath(json["userImagePath"].toString());
        module->setAudioPath(json["audioPath"].toString());
        module->setLifetimeCount(json["lifetimeCount"].toInt());

        // Load stages
        QVector<Stage> stages;
        QJsonArray stagesArray = json["stages"].toArray();
        for (const QJsonValue& val : stagesArray) {
            QJsonObject stageObj = val.toObject();
            Stage stage;
            stage.name = stageObj["name"].toString();
            stage.liturgyText = stageObj["liturgyText"].toString();
            stage.mantraText = stageObj["mantraText"].toString();
            stage.imagePath = stageObj["imagePath"].toString();
            stage.audioPath = stageObj["audioPath"].toString();
            stage.pdfPath = stageObj["pdfPath"].toString();
            stage.showImage = stageObj["showImage"].toBool();
            stage.showMantra = stageObj["showMantra"].toBool();
            stage.lifetimeCount = stageObj["lifetimeCount"].toInt();
            stages.append(stage);
        }
        module->setStages(stages);

        m_customModules.append(module);
        file.close();
    }
}

void ModuleManager::saveCustomModule(CustomModule* module)
{
    if (!module) return;

    QString filePath = getCustomModulesPath() + module->id() + ".json";

    QJsonObject json;
    json["id"] = module->id();
    json["name"] = module->name();
    json["userImagePath"] = module->userImagePath();
    json["audioPath"] = module->audioPath();
    json["lifetimeCount"] = module->lifetimeCount();

    // Save stages
    QJsonArray stagesArray;
    for (const Stage& stage : module->stages()) {
        QJsonObject stageObj;
        stageObj["name"] = stage.name;
        stageObj["liturgyText"] = stage.liturgyText;
        stageObj["mantraText"] = stage.mantraText;
        stageObj["imagePath"] = stage.imagePath;
        stageObj["audioPath"] = stage.audioPath;
        stageObj["pdfPath"] = stage.pdfPath;
        stageObj["showImage"] = stage.showImage;
        stageObj["showMantra"] = stage.showMantra;
        stageObj["lifetimeCount"] = stage.lifetimeCount;
        stagesArray.append(stageObj);
    }
    json["stages"] = stagesArray;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(json).toJson());
        file.close();
    }
}

void ModuleManager::deleteCustomModule(const QString& id)
{
    // Remove from vector
    for (int i = 0; i < m_customModules.size(); ++i) {
        if (m_customModules[i]->id() == id) {
            delete m_customModules[i];
            m_customModules.removeAt(i);
            break;
        }
    }

    // Delete JSON file
    QString filePath = getCustomModulesPath() + id + ".json";
    QFile::remove(filePath);

}

void ModuleManager::saveAudioPath(const QString& moduleId, const QString& path)
{
    DeityModule* module = getModule(moduleId);
    if (module) {
        module->setAudioPath(path);
        saveUserData();
    }
}

void ModuleManager::saveImageOverride(const QString& moduleId, const QString& path)
{
    DeityModule* module = getModule(moduleId);
    if (module) {
        module->setUserImagePath(path);
        saveUserData();
    }
}

void ModuleManager::saveLifetimeCount(const QString& moduleId, int count)
{
    DeityModule* module = getModule(moduleId);
    if (module) {
        module->setLifetimeCount(count);
        saveUserData();
    }
}

void ModuleManager::addToLifetimeCount(const QString& moduleId, int count)
{
    DeityModule* module = getModule(moduleId);
    if (module) {
        module->addToLifetimeCount(count);
        saveUserData();
    }
}

void ModuleManager::saveJournalEntry(const QString& moduleId, const QString& date, int count, const QString& notes)
{
    QMap<QString, QString> entry;
    entry["date"] = date;
    entry["count"] = QString::number(count);
    entry["notes"] = notes;

    m_journalEntries[moduleId].append(entry);
    saveUserData();
}

QVector<QMap<QString, QString>> ModuleManager::getJournalEntries(const QString& moduleId) const
{
    return m_journalEntries.value(moduleId);
}

void ModuleManager::savePdfPath(const QString& moduleId, const QString& path)
{
    DeityModule* module = getModule(moduleId);
    if (module) {
        module->setPdfPath(path);
        saveUserData();
    }
}


QString ModuleManager::getCustomModulesPath() const
{
    //QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/CustomModules/";
    QString path = Constants::customModulesPath + "/";

    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return path;
}

void ModuleManager::addCustomModule(CustomModule* module)
{
    if (!module) return;
    m_customModules.append(module);
    saveCustomModule(module);
}

CustomModule* ModuleManager::getCustomModuleById(const QString& id) const
{
    for (CustomModule* module : m_customModules) {
        if (module->id() == id) {
            return module;
        }
    }
    return nullptr;
}

QVector<DeityModule*> ModuleManager::getAllModules() const
{
    QVector<DeityModule*> all;

    // Add default modules
    for (DeityModule* module : m_modules) {
        all.append(module);
    }

    // Add custom modules (as DeityModule*)
    for (CustomModule* custom : m_customModules) {
        all.append(custom);
    }

    return all;
}
