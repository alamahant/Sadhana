#include "readerdialog.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPdfDocument>
#include <QMessageBox>
#include <QStandardPaths>
#include "constants.h"
#include<QSettings>
#include<QCheckBox>
#include<QFileDialog>
#include"constants.h"

ReaderDialog::ReaderDialog(QWidget* parent)
    : QDialog(parent, Qt::Window)
{
    setWindowTitle("Reader");
    setMinimumSize(800, 600);
    resize(1000, 700);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Toolbar
    QHBoxLayout* toolbar = new QHBoxLayout();
    m_openTextButton = new QPushButton("Open Text File", this);
    m_openPdfButton = new QPushButton("Open PDF File", this);
    openTextBrowserButton = new QPushButton("Open Editor",this);
    m_closeButton = new QPushButton("Close Tab", this);

    // Style buttons
    QString buttonStyle = "QPushButton { background-color: #3a6ea5; color: white; border: none; padding: 6px 12px; border-radius: 4px; } QPushButton:hover { background-color: #4a7eb5; }";
    m_openTextButton->setStyleSheet(buttonStyle);
    m_openPdfButton->setStyleSheet(buttonStyle);
    openTextBrowserButton->setStyleSheet(buttonStyle);
    m_closeButton->setStyleSheet(buttonStyle);

    toolbar->addWidget(m_openTextButton);
    toolbar->addWidget(m_openPdfButton);
    toolbar->addWidget(openTextBrowserButton);
    toolbar->addWidget(m_closeButton);
    toolbar->addStretch();
    mainLayout->addLayout(toolbar);

    // Tab widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(false);
    mainLayout->addWidget(m_tabWidget);

    // Bottom toolbar
    QWidget* bottomToolbarWidget = new QWidget(this);
    bottomToolbarWidget->setStyleSheet("background-color: #2a2a2a;");
    bottomToolbarWidget->setFixedHeight(40);

    QHBoxLayout* bottomToolbar = new QHBoxLayout(bottomToolbarWidget);
    bottomToolbar->setContentsMargins(10, 0, 10, 0);

    m_zoomInButton = new QPushButton("Zoom In", this);
    m_zoomOutButton = new QPushButton("Zoom Out", this);
    m_fitWidthButton = new QPushButton("Fit Width", this);
    m_fitPageButton = new QPushButton("Fit Page", this);

    m_saveDocButton = new QPushButton("Save As", this);
    m_zoomInButton->setStyleSheet(buttonStyle);
    m_zoomOutButton->setStyleSheet(buttonStyle);
    m_fitWidthButton->setStyleSheet(buttonStyle);
    m_fitPageButton->setStyleSheet(buttonStyle);
    m_saveDocButton->setStyleSheet(buttonStyle);

    bottomToolbar->addWidget(m_zoomInButton);
    bottomToolbar->addWidget(m_zoomOutButton);
    bottomToolbar->addWidget(m_fitWidthButton);
    bottomToolbar->addWidget(m_fitPageButton);
    bottomToolbar->addSpacing(10);
    bottomToolbar->addWidget(m_saveDocButton);
    bottomToolbar->addStretch();

    mainLayout->addWidget(bottomToolbarWidget);


    // Connect signals
    connect(m_openTextButton, &QPushButton::clicked, this, &ReaderDialog::openTextFile);
    connect(m_openPdfButton, &QPushButton::clicked, this, &ReaderDialog::openPdfFile);
    connect(m_closeButton, &QPushButton::clicked, this, &ReaderDialog::closeCurrentTab);
    connect(openTextBrowserButton, &QPushButton::clicked, this, &ReaderDialog::openEditor);

    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &ReaderDialog::closeCurrentTab);

    // Connect zoom buttons
    connect(m_zoomInButton, &QPushButton::clicked, this, &ReaderDialog::zoomIn);
    connect(m_zoomOutButton, &QPushButton::clicked, this, &ReaderDialog::zoomOut);
    connect(m_fitWidthButton, &QPushButton::clicked, this, &ReaderDialog::fitToWidth);
    connect(m_fitPageButton, &QPushButton::clicked, this, &ReaderDialog::fitToPage);
    connect(m_saveDocButton, &QPushButton::clicked, this, &ReaderDialog::saveDoc);

}

void ReaderDialog::openTextFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        "Open Text File",
        Constants::filesPath,
        "Text Files (*.txt *.utf8 *.md);;All Files (*)");

    if (filePath.isEmpty()) return;
    addTextTab(filePath);
}

void ReaderDialog::openPdfFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        "Open PDF File",
        Constants::filesPath,
        "PDF Files (*.pdf);;All Files (*)");

    if (filePath.isEmpty()) return;
    addPdfTab(filePath);
}

void ReaderDialog::addTextTab(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString tabName = fileInfo.fileName();


    // Limit tab title to 30 characters
    if (tabName.length() > 30) {
        tabName = tabName.left(27) + "...";
    }

    QTextBrowser* textBrowser = new QTextBrowser();

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        QString content = stream.readAll();
        textBrowser->setText(content);
        file.close();
    } else {
        textBrowser->setText("Error: Could not open file.");
    }

    m_tabWidget->addTab(textBrowser, tabName);
    m_tabWidget->setCurrentWidget(textBrowser);
}

void ReaderDialog::addPdfTab(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString tabName = fileInfo.fileName();

    // Limit tab title to 30 characters
    if (tabName.length() > 30) {
        tabName = tabName.left(27) + "...";
    }

    QPdfView* pdfView = new QPdfView();
    QPdfDocument* doc = new QPdfDocument(pdfView);
    doc->load(filePath);
    pdfView->setDocument(doc);
    pdfView->setPageMode(QPdfView::PageMode::MultiPage);

    m_tabWidget->addTab(pdfView, tabName);
    m_tabWidget->setCurrentWidget(pdfView);
}

void ReaderDialog::closeCurrentTab()
{
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex == -1) return;

    // Check if user doesn't want to see the confirmation again
    QSettings settings;
    if (!settings.value("ReaderDialog/skipTabCloseConfirmation", false).toBool()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Close Tab");
        msgBox.setText("Are you sure you want to close this tab?");
        msgBox.setIcon(QMessageBox::Question);

        QCheckBox* dontAskAgain = new QCheckBox("Don't ask again");
        msgBox.setCheckBox(dontAskAgain);

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);

        int result = msgBox.exec();

        // Save the "don't ask again" preference
        if (dontAskAgain->isChecked()) {
            settings.setValue("ReaderDialog/skipTabCloseConfirmation", true);
        }

        if (result != QMessageBox::Yes) {
            return;
        }
    }

    // Close the tab
    QWidget* widget = m_tabWidget->widget(currentIndex);

    // Disconnect all signals from this widget
    widget->disconnect();

    // If it's a PDF view, clear its document first
    if (QPdfView* pdfView = qobject_cast<QPdfView*>(widget)) {
        pdfView->setDocument(nullptr);
    }

    m_tabWidget->removeTab(currentIndex);
    delete widget;
}

void ReaderDialog::openEditor()
{
    QString tabName = "Editor";
    QTextEdit* textEdit = new QTextEdit(this);
    textEdit->setStyleSheet("QTextEdit { background-color: black; color: white; }");
    textEdit->setAcceptRichText(true);
    textEdit->setUndoRedoEnabled(true);
    textEdit->setPlaceholderText("Start typing or paste text here...");
    m_tabWidget->addTab(textEdit, tabName);
    m_tabWidget->setCurrentWidget(textEdit);

}

void ReaderDialog::saveDoc()
{
    QWidget* currentWidget = m_tabWidget->currentWidget();
    if (currentWidget && qobject_cast<QTextEdit*>(currentWidget))
    {
        QString defaultName = "document.txt";
        QString defaultPath = Constants::filesPath + "/" + defaultName;

        QString filePath = QFileDialog::getSaveFileName(this,
            "Save Text File",
            defaultPath,
            "Text Files (*.txt *.utf8 *.md);;All Files (*.*)");

        if (!filePath.isEmpty()) {
            QTextEdit* textEdit = qobject_cast<QTextEdit*>(currentWidget);
            QString content = textEdit->toPlainText();

            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly)) {
                QTextStream stream(&file);
                stream << content;
                file.close();
                QMessageBox::information(this, "Success", "File saved successfully.");
            } else {
                QMessageBox::warning(this, "Error", "Could not save file.");
            }
        }
    }
}

void ReaderDialog::zoomIn()
{
    QWidget* current = m_tabWidget->currentWidget();
    if (current && qobject_cast<QPdfView*>(current)) {
        QPdfView* pdfView = qobject_cast<QPdfView*>(current);
        pdfView->setZoomMode(QPdfView::ZoomMode::Custom);
        pdfView->setZoomFactor(pdfView->zoomFactor() + 0.25);
    }
    else if (current && qobject_cast<QTextEdit*>(current)) {
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(current);
        QFont font = textEdit->font();
        font.setPointSize(font.pointSize() + 1);
        textEdit->setFont(font);
    }
    else if (current && qobject_cast<QTextBrowser*>(current)) {
        QTextBrowser* textBrowser = qobject_cast<QTextBrowser*>(current);
        QFont font = textBrowser->font();
        font.setPointSize(font.pointSize() + 1);
        textBrowser->setFont(font);
    }
}

void ReaderDialog::zoomOut()
{
    QWidget* current = m_tabWidget->currentWidget();
    if (current && qobject_cast<QPdfView*>(current)) {
        QPdfView* pdfView = qobject_cast<QPdfView*>(current);
        pdfView->setZoomMode(QPdfView::ZoomMode::Custom);
        pdfView->setZoomFactor(pdfView->zoomFactor() - 0.25);
    }
    else if (current && qobject_cast<QTextEdit*>(current)) {
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(current);
        QFont font = textEdit->font();
        font.setPointSize(font.pointSize() - 1);
        textEdit->setFont(font);
    }
    else if (current && qobject_cast<QTextBrowser*>(current)) {
        QTextBrowser* textBrowser = qobject_cast<QTextBrowser*>(current);
        QFont font = textBrowser->font();
        font.setPointSize(font.pointSize() - 1);
        textBrowser->setFont(font);
    }
}


void ReaderDialog::fitToWidth()
{
    QWidget* current = m_tabWidget->currentWidget();
    if (current && qobject_cast<QPdfView*>(current)) {
        QPdfView* pdfView = qobject_cast<QPdfView*>(current);
        pdfView->setZoomMode(QPdfView::ZoomMode::FitToWidth);
    }
}

void ReaderDialog::fitToPage()
{
    QWidget* current = m_tabWidget->currentWidget();
    if (current && qobject_cast<QPdfView*>(current)) {
        QPdfView* pdfView = qobject_cast<QPdfView*>(current);
        pdfView->setZoomMode(QPdfView::ZoomMode::FitInView);
    }
}
