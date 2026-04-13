// tibetancalendardialog.h
#ifndef TIBETANCALENDARDIALOG_H
#define TIBETANCALENDARDIALOG_H

#include <QDialog>
#include <QCalendarWidget>
#include <QLabel>
#include <QTextEdit>
#include "tibetancalendar.h"

class TibetanCalendarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TibetanCalendarDialog(QWidget *parent = nullptr);
    ~TibetanCalendarDialog();

    void loadCalendarData(const QString &jsonPath);

private slots:
    void onDateSelected(const QDate &date);
    void onDateActivated(const QDate &date);

private:
    void setupUI();
    void updateDateInfo(const QDate &date);
    void applySpecialDayFormatting();

    QCalendarWidget *m_calendar;
    QLabel *m_tibetanDateLabel;
    QLabel *m_specialDayLabel;
    QTextEdit *m_detailsText;
    TibetanCalendar *m_tibetanCalendar;
};

#endif // TIBETANCALENDARDIALOG_H
