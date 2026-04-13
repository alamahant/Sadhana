// tibetancalendar.cpp
#include "tibetancalendar.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

TibetanCalendar::TibetanCalendar(QObject *parent) : QObject(parent)
{
}

bool TibetanCalendar::loadData(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open Tibetan calendar file:" << filePath;
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        qWarning() << "Invalid JSON in Tibetan calendar file";
        return false;
    }

    m_data = doc.object();
    return true;
}

QString TibetanCalendar::getSpecialDay(const QDate &date) const
{
    QJsonObject dayInfo = getDayInfo(date);
    return dayInfo["observance"].toString();
}

QString TibetanCalendar::getTibetanDate(const QDate &date) const
{
    QJsonObject dayInfo = getDayInfo(date);
    return dayInfo["tibetanDate"].toString();
}

QJsonObject TibetanCalendar::getDayInfo(const QDate &date) const
{
    QString yearStr = QString::number(date.year());
    QString monthStr = QString::number(date.month());
    QString dayStr = QString::number(date.day());

    if (!m_data.contains(yearStr)) return QJsonObject();
    QJsonObject yearObj = m_data[yearStr].toObject();

    if (!yearObj.contains(monthStr)) return QJsonObject();
    QJsonObject monthObj = yearObj[monthStr].toObject();

    if (!monthObj.contains(dayStr)) return QJsonObject();
    return monthObj[dayStr].toObject();
}

QMap<int, QString> TibetanCalendar::getMonthSpecialDays(int year, int month) const
{
    QMap<int, QString> specialDays;
    QString yearStr = QString::number(year);
    QString monthStr = QString::number(month);

    if (!m_data.contains(yearStr)) return specialDays;
    QJsonObject yearObj = m_data[yearStr].toObject();

    if (!yearObj.contains(monthStr)) return specialDays;
    QJsonObject monthObj = yearObj[monthStr].toObject();

    for (const QString &dayStr : monthObj.keys()) {
        QJsonObject dayObj = monthObj[dayStr].toObject();
        QString observance = dayObj["observance"].toString();
        if (!observance.isEmpty()) {
            specialDays[dayStr.toInt()] = observance;
        }
    }

    return specialDays;
}
