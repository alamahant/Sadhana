// tibetancalendar.h
#ifndef TIBETANCALENDAR_H
#define TIBETANCALENDAR_H

#include <QObject>
#include <QDate>
#include <QMap>
#include <QJsonObject>

class TibetanCalendar : public QObject
{
    Q_OBJECT

public:
    explicit TibetanCalendar(QObject *parent = nullptr);
    
    // Load JSON data from file path
    bool loadData(const QString &filePath);
    
    // Get special day for a given Gregorian date
    QString getSpecialDay(const QDate &date) const;
    
    // Get Tibetan date string (e.g., "4/15")
    QString getTibetanDate(const QDate &date) const;
    
    // Get full observance info for a date
    QJsonObject getDayInfo(const QDate &date) const;
    
    // Get all special days in a month
    QMap<int, QString> getMonthSpecialDays(int year, int month) const;

private:
    QJsonObject m_data;
};

#endif // TIBETANCALENDAR_H