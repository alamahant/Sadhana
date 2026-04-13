#include "tibetancalendarwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QLocale>
#include <QDebug>

TibetanCalendarWidget::TibetanCalendarWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentYear(QDate::currentDate().year())
    , m_currentMonth(QDate::currentDate().month())
{

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Header with navigation
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QPushButton *prevButton = new QPushButton("◀", this);
    QPushButton *nextButton = new QPushButton("▶", this);
    QPushButton *todayButton = new QPushButton("Today", this);

    m_monthYearLabel = new QLabel(this);
    m_monthYearLabel->setAlignment(Qt::AlignCenter);
    m_monthYearLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffd700;");

    headerLayout->addWidget(prevButton);
    headerLayout->addWidget(todayButton);
    headerLayout->addWidget(m_monthYearLabel);
    headerLayout->addWidget(nextButton);

    mainLayout->addLayout(headerLayout);

    // Calendar table
    m_table = new QTableWidget(this);
    m_table->setRowCount(6);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"});
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setSelectionBehavior(QAbstractItemView::SelectItems);

    // Style the table
    m_table->setStyleSheet(
        "QTableWidget {"
        "   background-color: #1a1a2e;"
        "   gridline-color: #2a2a3e;"
        "   font-size: 12px;"
        "}"
        "QTableWidget::item {"
        "   padding: 8px;"
        "   min-height: 70px;"
        "   min-width: 80px;"
        "}"
        "QTableWidget::item:selected {"
        "   background-color: #2a5a8a;"
        "   color: #ffd700;"
        "}"
        "QHeaderView::section {"
        "   background-color: #0d0d1a;"
        "   color: #ffd700;"
        "   padding: 5px;"
        "   font-weight: bold;"
        "}"
    );

    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mainLayout->addWidget(m_table);

    // Connect signals
    connect(prevButton, &QPushButton::clicked, this, &TibetanCalendarWidget::showPreviousMonth);
    connect(nextButton, &QPushButton::clicked, this, &TibetanCalendarWidget::showNextMonth);
    connect(todayButton, &QPushButton::clicked, this, &TibetanCalendarWidget::showCurrentMonth);
    connect(m_table, &QTableWidget::cellClicked, this, &TibetanCalendarWidget::onCellClicked);

    buildCalendar(m_currentYear, m_currentMonth);
}

void TibetanCalendarWidget::buildCalendar(int year, int month)
{

    m_currentYear = year;
    m_currentMonth = month;

    // Fix: Use QLocale to get month name
    QLocale locale(QLocale::English);
    QString monthName = locale.monthName(month, QLocale::LongFormat);
    m_monthYearLabel->setText(QString("%1 %2").arg(monthName).arg(year));

    // Clear table
    m_table->clearContents();
    m_cellToDate.clear();

    // Get first day of month
    QDate firstOfMonth(year, month, 1);
    if (!firstOfMonth.isValid()) {
        return;
    }

    int firstDayOfWeek = firstOfMonth.dayOfWeek(); // Monday = 1, Sunday = 7
    int daysInMonth = firstOfMonth.daysInMonth();


    // Adjust for Monday-based week
    int startCol = firstDayOfWeek - 1;

    int row = 0;
    int col = startCol;

    for (int day = 1; day <= daysInMonth; day++) {
        QDate date(year, month, day);

        // Create custom widget for cell
        QWidget *cellWidget = new QWidget();
        QVBoxLayout *cellLayout = new QVBoxLayout(cellWidget);

        // Day number
        QLabel *dayLabel = new QLabel(QString::number(day), cellWidget);
        dayLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        dayLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ccc;");

        // Tibetan date
        QLabel *tibetanLabel = new QLabel("", cellWidget);
        tibetanLabel->setAlignment(Qt::AlignCenter);
        tibetanLabel->setStyleSheet("font-size: 10px; color: #888;");

        // Special day indicator
        QLabel *specialLabel = new QLabel("", cellWidget);
        specialLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);
        specialLabel->setStyleSheet("font-size: 12px; color: #ffd700;");

        cellLayout->addWidget(dayLabel);
        cellLayout->addWidget(tibetanLabel);
        cellLayout->addWidget(specialLabel);
        cellLayout->setContentsMargins(5, 5, 5, 5);
        cellLayout->setSpacing(2);

        // Store data
        if (m_tibetanDates.contains(date)) {
            tibetanLabel->setText(m_tibetanDates[date]);
        }

        if (m_specialDays.contains(date)) {
            specialLabel->setText("★");
            cellWidget->setStyleSheet("background-color: #3a6ea5; border-radius: 4px;");
            dayLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ffd700;");
            tibetanLabel->setStyleSheet("font-size: 10px; color: #ffd700;");
        } else {
            cellWidget->setStyleSheet("background-color: #1a1a2e; border-radius: 4px;");
        }

        m_table->setCellWidget(row, col, cellWidget);
        m_cellToDate[row * 7 + col] = date;

        col++;
        if (col >= 7) {
            col = 0;
            row++;
        }
    }

    emit currentPageChanged();
}

void TibetanCalendarWidget::setTibetanDate(const QDate &date, const QString &tibetanDate)
{
    m_tibetanDates[date] = tibetanDate;
    buildCalendar(m_currentYear, m_currentMonth);
}

void TibetanCalendarWidget::setSpecialDay(const QDate &date, const QString &specialDay)
{
    m_specialDays[date] = specialDay;
    buildCalendar(m_currentYear, m_currentMonth);
}

void TibetanCalendarWidget::clearMonthCache(int year, int month)
{
    QDate firstOfMonth(year, month, 1);
    QDate lastOfMonth = firstOfMonth.addDays(firstOfMonth.daysInMonth() - 1);

    for (QDate d = firstOfMonth; d <= lastOfMonth; d = d.addDays(1)) {
        m_tibetanDates.remove(d);
        m_specialDays.remove(d);
    }
}

void TibetanCalendarWidget::showNextMonth()
{
    QDate nextDate(m_currentYear, m_currentMonth, 1);
    nextDate = nextDate.addMonths(1);
    buildCalendar(nextDate.year(), nextDate.month());
}

void TibetanCalendarWidget::showPreviousMonth()
{
    QDate prevDate(m_currentYear, m_currentMonth, 1);
    prevDate = prevDate.addMonths(-1);
    buildCalendar(prevDate.year(), prevDate.month());
}

void TibetanCalendarWidget::showCurrentMonth()
{
    QDate today = QDate::currentDate();
    buildCalendar(today.year(), today.month());
}

void TibetanCalendarWidget::onCellClicked(int row, int column)
{
    QDate date = m_cellToDate[row * 7 + column];
    if (date.isValid()) {
        m_selectedDate = date;
        emit selectionChanged();
    }
}
