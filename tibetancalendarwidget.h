#ifndef TIBETANCALENDARWIDGET_H
#define TIBETANCALENDARWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QDate>
#include <QMap>

class TibetanCalendarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TibetanCalendarWidget(QWidget *parent = nullptr);

    void setTibetanDate(const QDate &date, const QString &tibetanDate);
    void setSpecialDay(const QDate &date, const QString &specialDay);
    void clearMonthCache(int year, int month);

    int yearShown() const { return m_currentYear; }
    int monthShown() const { return m_currentMonth; }
    QDate selectedDate() const { return m_selectedDate; }

    void showNextMonth();
    void showPreviousMonth();
    void showCurrentMonth();

signals:
    void selectionChanged();
    void currentPageChanged();

private slots:
    void onCellClicked(int row, int column);

private:
    void buildCalendar(int year, int month);
    void updateCell(int row, int col, const QDate &date);
    QString getTibetanDisplay(const QDate &date) const;

    QTableWidget *m_table;
    QLabel *m_monthYearLabel;
    QMap<QDate, QString> m_tibetanDates;
    QMap<QDate, QString> m_specialDays;
    QMap<int, QDate> m_cellToDate;

    int m_currentYear;
    int m_currentMonth;
    QDate m_selectedDate;
};

#endif
