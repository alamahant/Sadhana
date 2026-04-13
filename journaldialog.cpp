#include "journaldialog.h"
#include "journalmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCalendarWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>
#include<QHeaderView>
#include<QTableView>

JournalDialog::JournalDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    refreshForDate(QDate::currentDate());
}

void JournalDialog::setupUI()
{
    setWindowTitle("Journal");
    setMinimumSize(800, 600);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Top: Calendar
    m_calendar = new QCalendarWidget(this);
    m_calendar->setObjectName("tibetanCalendar");
    m_calendar->setGridVisible(true);
    m_calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    m_calendar->setStyleSheet(
        "QCalendarWidget QAbstractItemView {"
        "   color: #666666;"
        "}"

    );

    connect(m_calendar, &QCalendarWidget::selectionChanged, this, [this]() {
        onDateSelected(m_calendar->selectedDate());
    });
    mainLayout->addWidget(m_calendar);
    
    // Middle: Entries display
    QLabel* entriesLabel = new QLabel("Past Entries:", this);
    entriesLabel->setStyleSheet("color: #ffd700; font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(entriesLabel);
    
    m_entriesDisplay = new QTextBrowser(this);
    m_entriesDisplay->setMinimumHeight(200);
    m_entriesDisplay->setStyleSheet("QTextBrowser { background-color: #111; color: #ccc; border: 1px solid #333; border-radius: 4px; padding: 10px; }");
    mainLayout->addWidget(m_entriesDisplay);
    
    // Bottom: Add new entry
    QLabel* newEntryLabel = new QLabel("Add Entry for Selected Date:", this);
    newEntryLabel->setStyleSheet("color: #ffd700; font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(newEntryLabel);
    
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setMaximumHeight(100);
    m_notesEdit->setPlaceholderText("Write your journal notes here...");
    m_notesEdit->setStyleSheet("QTextEdit { background-color: #111; color: #ccc; border: 1px solid #333; border-radius: 4px; padding: 8px; }");
    mainLayout->addWidget(m_notesEdit);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_saveButton = new QPushButton("Save Entry", this);
    m_saveButton->setFixedHeight(32);
    m_saveButton->setStyleSheet("QPushButton { background-color: #3a6ea5; border-radius: 4px; }");
    connect(m_saveButton, &QPushButton::clicked, this, &JournalDialog::onSaveEntry);
    
    m_closeButton = new QPushButton("Close", this);
    m_closeButton->setFixedHeight(32);
    m_closeButton->setStyleSheet("QPushButton { background-color: #3a3a3a; border-radius: 4px; }");
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_closeButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    setStyleSheet("QDialog { background-color: #1a1a1a; } QLabel { color: #ccc; }");
}

void JournalDialog::onDateSelected(const QDate& date)
{
    refreshForDate(date);
}

void JournalDialog::refreshForDate(const QDate& date)
{
    m_currentDate = date;
    loadEntriesForDate(date);
    m_notesEdit->clear();
}

void JournalDialog::loadEntriesForDate(const QDate& date)
{
    QVector<JournalEntry> entries = JournalManager::instance().getEntriesForDate(date);
    displayEntries(entries);
}

void JournalDialog::displayEntries(const QVector<JournalEntry>& entries)
{
    if (entries.isEmpty()) {
        m_entriesDisplay->setText("No entries for this date.");
        return;
    }
    
    QString html = "<table width='100%'>";
    for (const JournalEntry& entry : entries) {
        html += "<tr>";
        html += QString("<td valign='top' width='80'><b>%1</b></td>")
                .arg(entry.timestamp.toString("hh:mm"));
        html += QString("<td><b>%1</b>").arg(entry.moduleName);
        if (!entry.stageName.isEmpty()) {
            html += QString(" - %1").arg(entry.stageName);
        }
        html += QString("<br>Repetitions: %1").arg(entry.repetitions);
        if (!entry.notes.isEmpty()) {
            html += QString("<br><i>%1</i>").arg(entry.notes.toHtmlEscaped());
        }
        html += "</td></tr><tr><td colspan='2'><hr></td></tr>";
    }
    html += "</table>";
    
    m_entriesDisplay->setHtml(html);
}

void JournalDialog::onSaveEntry()
{
    QString notes = m_notesEdit->toPlainText().trimmed();
    if (notes.isEmpty()) {
        // Don't save empty entries
        return;
    }
    
    // Get current module info from somewhere? Or just save notes without module?
    JournalEntry entry;
    entry.timestamp = QDateTime(m_currentDate, QTime::currentTime());
    //entry.moduleName = "General";
    entry.moduleName = m_currentModuleName;
    entry.stageName = "";
    entry.repetitions = -1;
    entry.notes = notes;
    
    JournalManager::instance().addEntry(entry);
    refreshForDate(m_currentDate);
}
