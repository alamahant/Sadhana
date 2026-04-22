#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "gridview.h"
#include "pujaview.h"
#include "deitymodule.h"
#include "readerdialog.h"
#include"tibetancalendardialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onModuleSelected(DeityModule* module);
    void onBackToGrid();
    void onCreateCustomModule();
    void onDeleteModule(DeityModule* module);
    void openReader();
    void showTibetanCalendar();
    void openFolder();
    void createSymlink();
    void showAboutDialog();
    void showInstructionsDialog();
    void showRespectDialog();
    void showFindResourcesDialog();
    void showChangelogDialog();

private:
    void setupUI();
    void setupMenuBar();
    void applyDarkTheme();

    QStackedWidget* m_stackedWidget;
    GridView* m_gridView;
    PujaView* m_pujaView = nullptr;
    void refreshGrid();
    ReaderDialog* readerDialog = nullptr;
    TibetanCalendarDialog* calendarDialog = nullptr;

};

#endif // MAINWINDOW_H
