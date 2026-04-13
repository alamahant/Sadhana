#ifndef JOURNALMANAGER_H
#define JOURNALMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QString>

struct JournalEntry {
    QDateTime timestamp;
    QString moduleName;
    QString stageName;
    int repetitions;
    QString notes;
};

class JournalManager : public QObject
{
    Q_OBJECT

public:
    static JournalManager& instance();
    
    void addEntry(const QString& moduleName, const QString& stageName, int repetitions, const QString& notes);
    QVector<JournalEntry> getEntries() const;
    QVector<JournalEntry> getEntriesForDate(const QDate& date) const;
    void clearAll();
    void addEntry(const JournalEntry& entry);
private:
    JournalManager(QObject* parent = nullptr);
    void load();
    void save();
    
    QVector<JournalEntry> m_entries;
    QString getFilePath() const;
};

#endif
