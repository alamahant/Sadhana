#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <QObject>
#include <QVector>
#include <QMap>
#include "deitymodule.h"
#include"custommodule.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class ModuleManager : public QObject
{
    Q_OBJECT

public:
    static ModuleManager& instance();

    // Load all modules (defaults + user data)
    void loadModules();

    // Save all modules data (counts, paths, etc.)
    void saveModules();

    // Get modules
    QVector<DeityModule*> getAllModules() const;
    DeityModule* getModule(const QString& id) const;

    // User data operations
    void saveAudioPath(const QString& moduleId, const QString& path);
    void saveImageOverride(const QString& moduleId, const QString& path);
    void saveLifetimeCount(const QString& moduleId, int count);
    void addToLifetimeCount(const QString& moduleId, int count);

    // Journal
    void saveJournalEntry(const QString& moduleId, const QString& date, int count, const QString& notes);
    QVector<QMap<QString, QString>> getJournalEntries(const QString& moduleId) const;
    void savePdfPath(const QString& moduleId, const QString& path);
    void saveCustomModule(CustomModule* module);
    void addCustomModule(CustomModule* module);
    void saveUserData();

private:
    ModuleManager(QObject* parent = nullptr);
    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;

    void createDefaultModules();
    void loadUserData();

    QVector<DeityModule*> m_modules;
    QMap<QString, QVector<QMap<QString, QString>>> m_journalEntries; // moduleId -> list of entries

    QString getDataPath() const;


    //custom modules
private:
    QVector<CustomModule*> m_customModules; // User-created modules
    void loadCustomModules();
public:
    QVector<CustomModule*> getAllCustomModules() const { return m_customModules; };
    QString getCustomModulesPath() const;
    CustomModule* getCustomModuleById(const QString& id) const;
    void deleteCustomModule(const QString& id);
    DeityModule* loadModuleFromJson(const QString& jsonPath);

};

#endif // MODULEMANAGER_H
