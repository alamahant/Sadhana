#ifndef READERDIALOG_H
#define READERDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QTextBrowser>
#include <QPdfView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class ReaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReaderDialog(QWidget* parent = nullptr);

private slots:
    void openTextFile();
    void openPdfFile();
    void closeCurrentTab();
    void openEditor();
    void saveDoc();
private:
    void addTextTab(const QString& filePath);
    void addPdfTab(const QString& filePath);

    QTabWidget* m_tabWidget;
    QPushButton* m_openTextButton;
    QPushButton* m_openPdfButton;
    QPushButton* m_closeButton;
    QPushButton* m_saveDocButton;
private:
    QWidget* m_bottomBar;
    QPushButton* m_zoomInButton;
    QPushButton* m_zoomOutButton;
    QPushButton* m_fitWidthButton;
    QPushButton* m_fitPageButton;
    QPushButton* openTextBrowserButton;
private slots:
    void zoomIn();
    void zoomOut();
    void fitToWidth();
    void fitToPage();
};

#endif // READERDIALOG_H
