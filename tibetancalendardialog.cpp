// tibetancalendardialog.cpp
#include "tibetancalendardialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCharFormat>
#include <QDebug>
#include<QComboBox>

TibetanCalendarDialog::TibetanCalendarDialog(QWidget *parent)
    : QDialog(parent)
    , m_tibetanCalendar(nullptr)
{
    setupUI();
}

TibetanCalendarDialog::~TibetanCalendarDialog()
{
    delete m_tibetanCalendar;
}

void TibetanCalendarDialog::setupUI()
{
    setWindowTitle("Tibetan Calendar");
    setMinimumSize(600, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);




    // Calendar widget
    m_calendar = new QCalendarWidget(this);
    m_calendar->setGridVisible(true);
    m_calendar->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);

    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *yearLabel = new QLabel("Year:", this);
    QComboBox *yearCombo = new QComboBox(this);

    // Populate years 2025-2049
    for (int y = 2025; y <= 2049; y++) {
        yearCombo->addItem(QString::number(y));
    }

    // Set current year
    yearCombo->setCurrentText(QString::number(m_calendar->yearShown()));

    topLayout->addWidget(yearLabel);
    topLayout->addWidget(yearCombo);
    topLayout->addStretch();

    // Insert at top of mainLayout
    mainLayout->addLayout(topLayout);



    mainLayout->addWidget(m_calendar);

    // Info panel
    QHBoxLayout *infoLayout = new QHBoxLayout();

    QVBoxLayout *leftLayout = new QVBoxLayout();
    m_tibetanDateLabel = new QLabel("Tibetan Date: --", this);
    m_tibetanDateLabel->setStyleSheet("font-size: 14px; color: #ffd700;");
    m_specialDayLabel = new QLabel("Special Day: --", this);
    m_specialDayLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ffd700;");

    leftLayout->addWidget(m_tibetanDateLabel);
    leftLayout->addWidget(m_specialDayLabel);

    m_detailsText = new QTextEdit(this);
    m_detailsText->setMaximumHeight(100);
    m_detailsText->setReadOnly(true);
    m_detailsText->setStyleSheet("background-color: #111; color: #ccc; border: 1px solid #333; border-radius: 4px;");

    infoLayout->addLayout(leftLayout, 1);
    infoLayout->addWidget(m_detailsText, 2);
    mainLayout->addLayout(infoLayout);

    // Connect signals
    connect(m_calendar, &QCalendarWidget::selectionChanged, this, [this]() {
        onDateSelected(m_calendar->selectedDate());
    });
    connect(m_calendar, &QCalendarWidget::activated, this, &TibetanCalendarDialog::onDateActivated);

    connect(m_calendar, &QCalendarWidget::currentPageChanged, this, [this]() {
        applySpecialDayFormatting();
    });

    // Connect the combo box
    connect(yearCombo, &QComboBox::currentTextChanged, this, [this](const QString &yearStr) {
        int year = yearStr.toInt();
        int currentMonth = m_calendar->monthShown();
        QDate newDate(year, currentMonth + 1, 1);
        m_calendar->setSelectedDate(newDate);
        applySpecialDayFormatting();
    });

    setStyleSheet("QDialog { background-color: #1a1a1a; } QLabel { color: #ccc; }");
}

void TibetanCalendarDialog::loadCalendarData(const QString &jsonPath)
{


    m_tibetanCalendar = new TibetanCalendar(this);
    if (m_tibetanCalendar->loadData(jsonPath)) {
        applySpecialDayFormatting();
        // Show today's info
        onDateSelected(QDate::currentDate());
    } else {
        m_tibetanDateLabel->setText("Failed to load calendar data");
    }
}


void TibetanCalendarDialog::applySpecialDayFormatting()
{
    int year = m_calendar->yearShown();
    int month = m_calendar->monthShown() - 1;

    QMap<int, QString> specialDays = m_tibetanCalendar->getMonthSpecialDays(year, month + 1);

    QTextCharFormat format;
    //format.setBackground(QColor("#3a6ea5"));
    //format.setForeground(QColor("#ffd700"));
    format.setBackground(QColor("#8B0000"));  // Dark red background
    format.setForeground(QColor("#FFD700"));  // Gold text
    format.setFontWeight(QFont::Bold);

    for (int day : specialDays.keys()) {
        QDate date(year, month + 1, day);
        m_calendar->setDateTextFormat(date, format);
    }
}

void TibetanCalendarDialog::onDateSelected(const QDate &date)
{
    if (!m_tibetanCalendar) return;

    QString tibetanDate = m_tibetanCalendar->getTibetanDate(date);
    QString specialDay = m_tibetanCalendar->getSpecialDay(date);
    QJsonObject dayInfo = m_tibetanCalendar->getDayInfo(date);

    if (!tibetanDate.isEmpty()) {
        m_tibetanDateLabel->setText(QString("Tibetan Date: %1").arg(tibetanDate));
    } else {
        m_tibetanDateLabel->setText("Tibetan Date: --");
    }

    if (!specialDay.isEmpty()) {
        m_specialDayLabel->setText(QString("★ %1").arg(specialDay));
        m_specialDayLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ffd700;");

        QString details = QString("Observance: %1\nTibetan: %2\nGregorian: %3")
                          .arg(specialDay)
                          .arg(tibetanDate)
                          .arg(date.toString("yyyy-MM-dd"));

        if (dayInfo.contains("type")) {
            details += QString("\nType: %1").arg(dayInfo["type"].toString());
        }
        if (dayInfo.contains("multiplier")) {
            details += QString("\nKarmic Multiplier: %1x").arg(dayInfo["multiplier"].toInt());
        }

        m_detailsText->setText(details);
    } else {
        m_specialDayLabel->setText("No special observance");
        m_detailsText->setText(QString("Tibetan: %1\nGregorian: %2")
                               .arg(tibetanDate.isEmpty() ? "--" : tibetanDate)
                               .arg(date.toString("yyyy-MM-dd")));
    }
}

void TibetanCalendarDialog::onDateActivated(const QDate &date)
{
    // User double-clicked or pressed enter on a date
    QString specialDay = m_tibetanCalendar->getSpecialDay(date);
    if (!specialDay.isEmpty()) {
        // You could emit a signal to jump to practice for that day
    }
}
