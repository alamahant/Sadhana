#include "journalmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include"constants.h"

JournalManager& JournalManager::instance()
{
    static JournalManager manager;
    return manager;
}

JournalManager::JournalManager(QObject* parent)
    : QObject(parent)
{
    load();
}

void JournalManager::addEntry(const QString& moduleName, const QString& stageName, int repetitions, const QString& notes)
{
    JournalEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.moduleName = moduleName;
    entry.stageName = stageName;
    entry.repetitions = repetitions;
    entry.notes = notes;
    
    m_entries.append(entry);
    save();
}

QVector<JournalEntry> JournalManager::getEntries() const
{
    return m_entries;
}

QVector<JournalEntry> JournalManager::getEntriesForDate(const QDate& date) const
{
    QVector<JournalEntry> result;
    for (const JournalEntry& entry : m_entries) {
        if (entry.timestamp.date() == date) {
            result.append(entry);
        }
    }
    return result;
}

void JournalManager::clearAll()
{
    m_entries.clear();
    save();
}

void JournalManager::save()
{
    QJsonArray entriesArray;
    for (const JournalEntry& entry : m_entries) {
        QJsonObject obj;
        obj["timestamp"] = entry.timestamp.toString(Qt::ISODate);
        obj["moduleName"] = entry.moduleName;
        obj["stageName"] = entry.stageName;
        obj["repetitions"] = entry.repetitions;
        obj["notes"] = entry.notes;
        entriesArray.append(obj);
    }
    
    QJsonDocument doc(entriesArray);
    QFile file(getFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void JournalManager::load()
{
    QFile file(getFilePath());
    if (!file.open(QIODevice::ReadOnly)) return;
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isArray()) return;
    
    m_entries.clear();
    for (const QJsonValue& val : doc.array()) {
        QJsonObject obj = val.toObject();
        JournalEntry entry;
        entry.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
        entry.moduleName = obj["moduleName"].toString();
        entry.stageName = obj["stageName"].toString();
        entry.repetitions = obj["repetitions"].toInt();
        entry.notes = obj["notes"].toString();
        m_entries.append(entry);
    }
}

QString JournalManager::getFilePath() const
{
    QString path = Constants::journalPath;
    return path + "/journal.json";
}

void JournalManager::addEntry(const JournalEntry& entry)
{
    m_entries.append(entry);
    save();
}
