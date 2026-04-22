#include "mainwindow.h"
#include "modulemanager.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QApplication>
#include "createmoduledialog.h"
#include "custommodule.h"
#include<QIcon>
#include"constants.h"
#include<QDesktopServices>
#include<QDir>
#include<QFileDialog>
#include"donationdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , readerDialog(new ReaderDialog(this))
    , calendarDialog(new TibetanCalendarDialog(this))
{
    setupUI();
    setupMenuBar();
    applyDarkTheme();

    setWindowTitle("Sadhana");
    setWindowIcon(QIcon(":/io.github.alamahant.Sadhana.png"));

    setMinimumSize(900, 700);
    resize(1000, 800);

    connect(m_gridView, &GridView::createCustomModuleRequested,
               this, &MainWindow::onCreateCustomModule);


    connect(m_gridView, &GridView::deleteModuleRequested,
                this, &MainWindow::onDeleteModule);

#ifdef FLATPAK_BUILD
    QString jsonPath = QCoreApplication::applicationDirPath() + "/files/tibetan_special_days.json";
#else
    QString jsonPath = "files/tibetan_special_days.json";
#endif

    calendarDialog->loadCalendarData(jsonPath);

}

MainWindow::~MainWindow()
{
    // Save any pending data
    ModuleManager::instance().saveModules();
}

void MainWindow::setupUI()
{
    // Central widget and stacked widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);

    // Grid View (Home)
    m_gridView = new GridView(this);
    m_gridView->setModules(ModuleManager::instance().getAllModules());
    connect(m_gridView, &GridView::moduleSelected, this, &MainWindow::onModuleSelected);
    m_stackedWidget->addWidget(m_gridView);

    // Puja View (Practice Space)
    m_pujaView = new PujaView(this);
    connect(m_pujaView, &PujaView::backToGrid, this, &MainWindow::onBackToGrid);
    m_stackedWidget->addWidget(m_pujaView);

    // Start at grid view
    m_stackedWidget->setCurrentWidget(m_gridView);
}

void MainWindow::setupMenuBar()
{
    // File menu
    QMenu* fileMenu = menuBar()->addMenu("&File");

    fileMenu->addSeparator();

    QAction *openFolderAction = fileMenu->addAction("&Open Data Directory");
    connect(openFolderAction, &QAction::triggered, this, &MainWindow::openFolder);
    fileMenu->addSeparator();

    fileMenu->addSeparator();
    QString menuText = QString("Create Shortcut to %1 Data").arg(qAppName());
    QAction *createSymlinkAction = fileMenu->addAction("Create Shortcut to Sadhana Data");
    connect(createSymlinkAction, &QAction::triggered, this, &MainWindow::createSymlink);
    fileMenu->addSeparator();


    QAction* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // View menu
    QMenu* viewMenu = menuBar()->addMenu("&View");

    QAction* backToGridAction = viewMenu->addAction("&Home Grid");
    backToGridAction->setShortcut(QKeySequence("Ctrl+H"));
    connect(backToGridAction, &QAction::triggered, this, &MainWindow::onBackToGrid);

    QMenu* toolsMenu = menuBar()->addMenu("&Tools");
    QAction* readerAction = toolsMenu->addAction("&Reader");
    readerAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(readerAction, &QAction::triggered, this, &MainWindow::openReader);

    toolsMenu->addSeparator();
    QAction* calendarAction = toolsMenu->addAction("&Tibetan Calendar");
    calendarAction->setShortcut(QKeySequence("Ctrl+K"));
    connect(calendarAction, &QAction::triggered, this, &MainWindow::showTibetanCalendar);

    // Help menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);

    QAction* instructionsAction = helpMenu->addAction("&Instructions");
    connect(instructionsAction, &QAction::triggered, this, &MainWindow::showInstructionsDialog);

    QAction* resourcesAction = helpMenu->addAction("&Find Practice Resources");
        connect(resourcesAction, &QAction::triggered, this, &MainWindow::showFindResourcesDialog);

    QAction* respectAction = helpMenu->addAction("&A Friendly Note");
    connect(respectAction, &QAction::triggered, this, &MainWindow::showRespectDialog);

    QAction* changelogAction = helpMenu->addAction("&What's New");
    connect(changelogAction, &QAction::triggered, this, &MainWindow::showChangelogDialog);

    helpMenu->addSeparator();
    QAction *supportusAction = helpMenu->addAction("Support Us");
                connect(supportusAction, &QAction::triggered, [this]() {
                    DonationDialog dialog(this);
                    dialog.exec();
                });
    helpMenu->addAction(supportusAction);
}


void MainWindow::applyDarkTheme()
{
    QString darkStyle = R"(
        QMainWindow {
            background-color: #1a1a1a;
        }
        QMenuBar {
            background-color: #2a2a2a;
            color: #ffffff;
        }
        QMenuBar::item:selected {
            background-color: #3a3a3a;
        }
        QMenu {
            background-color: #2a2a2a;
            color: #ffffff;
            border: 1px solid #3a3a3a;
        }
        QMenu::item:selected {
            background-color: #3a3a3a;
        }
        QMessageBox {
            background-color: #2a2a2a;
            color: #ffffff;
        }
        QPushButton {
            background-color: #3a6ea5;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #4a7eb5;
        }
        QPushButton:pressed {
            background-color: #2a5e95;
        }
        QPushButton:checked {
            background-color: #ffd700;
            color: black;
        }
        QMessageBox {
            background-color: #2a2a2a;
            color: #ffffff;
        }
        QMessageBox QLabel {
            color: #ffffff;
        }
        QMessageBox QPushButton {
            background-color: #3a6ea5;
            color: white;
            border: none;
            padding: 5px 10px;
            border-radius: 4px;
            min-width: 70px;
        }
        QMessageBox QPushButton:hover {
            background-color: #4a7eb5;
        }
        QToolTip {
            background-color: #2a2a2a;
            color: #ffd700;
            border: 1px solid #555;
            padding: 8px;
            border-radius: 4px;
            font-size: 12px;
        }
        QCheckBox {
            color: #ffffff;
            background-color: transparent;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border-radius: 3px;
            border: 1px solid #555;
            background-color: #333;
        }
        QCheckBox::indicator:checked {
            background-color: #3a6ea5;
            border: 1px solid #3a6ea5;
        }
        QCheckBox::indicator:hover {
            border: 1px solid #ffd700;
        }
        /* ReaderDialog styles */
        QDialog {
            background-color: #1a1a1a;
        }
        QTabWidget::pane {
            background-color: #111;
            border: none;
        }
        QTabBar::tab {
            background-color: #2a2a2a;
            color: #ccc;
            padding: 8px 16px;
        }
        QTabBar::tab:selected {
            background-color: #3a6ea5;
            color: white;
        }
        QTabBar::tab:hover {
            background-color: #3a3a3a;
        }
        QTextBrowser {
            background-color: #111;
            color: #ccc;
            border: none;
        }
        QScrollArea {
            background-color: #111;
            border: none;
        }
        QScrollBar:vertical {
            background-color: #2a2a2a;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #555;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #777;
                }
        QSlider::groove:horizontal {
            background: #3a3a3a;
            height: 4px;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #ffd700;
            width: 12px;
            margin: -4px 0;
            border-radius: 6px;
        }
        QSlider::handle:horizontal:hover {
            background: #ffcc00;
        }
)";
    qApp->setStyleSheet(darkStyle);
}

void MainWindow::onModuleSelected(DeityModule* module)
{
    if (!module) return;

    // Check if it's a custom module (by checking if it exists in custom modules list)
    CustomModule* customModule = ModuleManager::instance().getCustomModuleById(module->id());

    if (customModule) {
        // Load as custom module
        m_pujaView->loadCustomModule(customModule);
    } else {
        // Load as default module
        m_pujaView->loadModule(module);
    }

    m_stackedWidget->setCurrentWidget(m_pujaView);
}

void MainWindow::onBackToGrid()
{
    // Refresh grid in case user updated any images
    m_gridView->refreshImages();
    // Switch back to grid view
    m_stackedWidget->setCurrentWidget(m_gridView);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save all module data before closing
    ModuleManager::instance().saveModules();
    event->accept();
}

void MainWindow::onCreateCustomModule()
{
    static QDateTime lastCall;
        if (lastCall.isValid() && lastCall.msecsTo(QDateTime::currentDateTime()) < 500) {
            return;
        }
        lastCall = QDateTime::currentDateTime();

    CreateModuleDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString moduleName = dialog.getModuleName();
        QString imagePath = dialog.getImagePath();

        // Generate unique ID
        QString id = "custom_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");

        // Create custom module
        CustomModule* module = new CustomModule(id, moduleName);

        // Explicitly set both to EMPTY
        module->setUserImagePath("");      // Grid image - empty
        module->setDefaultImagePath("");   // Stage fallback - empty

        if (!imagePath.isEmpty()) {
            module->setUserImagePath(imagePath);
        }

        // Create default first stage
        Stage firstStage;
        firstStage.name = "Stage 1";
        firstStage.imagePath = imagePath;
        firstStage.showImage = true;
        firstStage.showMantra = true;
        module->addStage(firstStage);

        // Save to disk
        ModuleManager::instance().saveCustomModule(module);
        ModuleManager::instance().addCustomModule(module);

        // Refresh grid view
        refreshGrid();
    }
}

void MainWindow::refreshGrid()
{
    // Get all modules (default + custom)
    QVector<DeityModule*> allModules;

    // Add default modules
    for (DeityModule* module : ModuleManager::instance().getAllModules()) {
        allModules.append(module);
    }

    // Update grid
    m_gridView->setModules(allModules);
}

void MainWindow::onDeleteModule(DeityModule* module)
{
    if (!module) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Module",
        "Are you sure you want to delete \"" + module->name() + "\"?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Check if it's a custom module
        CustomModule* custom = ModuleManager::instance().getCustomModuleById(module->id());
        if (custom) {
            ModuleManager::instance().deleteCustomModule(module->id());
        }
        refreshGrid();
    }
}


void MainWindow::openReader()
{
    //ReaderDialog* dialog = new ReaderDialog(this);
    readerDialog->show();
}

void MainWindow::showTibetanCalendar()
{

        calendarDialog->show();
}

void MainWindow::openFolder() {
    // Optional: Check if the folder exists
    QDir dir(Constants::appDirPath);
    if (!dir.exists()) {
        return;
    }

    // Convert local path to URL and open
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(Constants::appDirPath))) {
    }
}

void MainWindow::createSymlink()
{
#ifdef FLATPAK_BUILD
    QString msg = "";
    QMessageBox msgBox;
    msgBox.setWindowTitle("Flatpak Permission Required");
    msgBox.setText(QString(
                       "%1 is running as a Flatpak and may not have access to your home directory.\n\n"
                       "To create a symlink, you may need to grant home directory access first.\n\n"
                       "Option 1 - Terminal:\n"
                       "  Grant access:\n"
                       "    flatpak override --user --filesystem=home io.github.alamahant.%1\n\n"
                       "  Revoke access later:\n"
                       "    flatpak override --user --nofilesystem=home io.github.alamahant.%1\n\n"
                       "Option 2 - Flatseal:\n"
                       "  Install Flatseal from Flathub and grant 'Home' access to %1.\n\n"
                       "If you have already granted permissions, you can continue."
                       ).arg(QApplication::applicationName()));

    msgBox.setIcon(QMessageBox::Information);

    QPushButton *continueButton = msgBox.addButton("Continue", QMessageBox::AcceptRole);
    QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
    msgBox.setDefaultButton(cancelButton);

    msgBox.exec();

    if (msgBox.clickedButton() != continueButton) {
        return; // User cancelled
    }

#endif
    // Open dialog to select destination folder
    QString destinationDir = QFileDialog::getExistingDirectory(
                this,
                "Select Destination Folder for Symlink",
                QDir::homePath(),
                QFileDialog::ShowDirsOnly
                );

    if (destinationDir.isEmpty()) {
        return; // User cancelled
    }

    // Create symlink path
    QString symlinkPath = QDir(destinationDir).filePath(QApplication::applicationName());
    // Check if symlink already exists
    if (QFile::exists(symlinkPath) || QFileInfo(symlinkPath).isSymLink()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
                    this,
                    "Symlink Exists",
                    QString("A file or symlink already exists at:\n%1\n\nOverwrite?").arg(symlinkPath),
                    QMessageBox::Yes | QMessageBox::No
                    );

        if (reply != QMessageBox::Yes) {
            return;
        }

        // Remove existing file/symlink
        if (!QFile::remove(symlinkPath)) {
            QMessageBox::warning(this, "Error", "Could not remove existing file/symlink");
            return;
        }
    }

    // Create the symlink
    QString targetPath = Constants::appDirPath;

    if (!QFile::exists(targetPath)) {
        QMessageBox::warning(this, "Error",
                             QString("Target directory does not exist:\n%1").arg(targetPath));
        return;
    }

    if (QFile::link(targetPath, symlinkPath)) {
        QMessageBox::information(
                    this,
                    "Symlink Created",
                    QString("Symlink created successfully!\n\n"
                            "Name: %3\n"
                            "Location: %1\n\n"
                            "Now you can access %3 data from:\n%2")
                    .arg(destinationDir)
                    .arg(symlinkPath)
                    .arg(QApplication::applicationName())
                    );
    } else {
        QMessageBox::warning(
                    this,
                    "Error",
                    QString("Failed to create symlink.\n\n"
                            "Destination: %1\n"
                            "Target: %2\n\n"
                            "Possible reasons:\n"
                            "• Insufficient permissions\n"
                            "• Invalid destination path\n"
                            "• Filesystem doesn't support symlinks")
                    .arg(symlinkPath)
                    .arg(targetPath)
                    );
    }
}




void MainWindow::showAboutDialog()
{
    QString version = QCoreApplication::applicationVersion();

    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("About Sadhana");
    dialog->setFixedSize(500, 350);
    dialog->setStyleSheet("QDialog { background-color: #1a1a1a; }");
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Icon and Title Row
    QHBoxLayout* headerLayout = new QHBoxLayout();

    QLabel* iconLabel = new QLabel(dialog);
    iconLabel->setPixmap(QPixmap(":/io.github.alamahant.Sadhana.png").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel* titleLabel = new QLabel("Sadhana", dialog);
    titleLabel->setStyleSheet("color: #ffd700; font-size: 28px; font-weight: bold;");

    headerLayout->addWidget(iconLabel);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    // Content with word wrap enabled
    QLabel* contentLabel = new QLabel(dialog);
    contentLabel->setWordWrap(true);
    contentLabel->setAlignment(Qt::AlignCenter);
    contentLabel->setStyleSheet("color: #ffffff; font-size: 14px; line-height: 1.6;");
    contentLabel->setText(QString(
        "<p><b>Version %1</b></p>"
        "<p style='margin-top: 20px;'>A digital shrine and practice tracker for spiritual practitioners. Designed to support daily sadhana, mantra accumulation, and contemplative practice.</p>"
        "<p style='margin-top: 30px; color: #aaaaaa;'>© 2026 Alamahant<br>All rights reserved.</p>"
        "<p style='margin-top: 20px; font-style: italic; color: #ffd700;'>\"By this merit, may all beings attain enlightenment.\"</p>"
    ).arg(version));

    mainLayout->addWidget(contentLabel);
    mainLayout->addStretch();

    // Close Button
    QPushButton* closeButton = new QPushButton("Close", dialog);
    closeButton->setFixedSize(100, 35);
    closeButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3a6ea5;
            color: white;
            border: none;
            border-radius: 4px;
            font-size: 13px;
        }
        QPushButton:hover {
            background-color: #4a7eb5;
        }
    )");
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    dialog->exec();
}

void MainWindow::showInstructionsDialog()
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Instructions");
    dialog->setMinimumSize(550, 450);
    dialog->resize(600, 500);
    dialog->setStyleSheet("QDialog { background-color: #1a1a1a; }");

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    // Scrollable text area
    QTextBrowser* textBrowser = new QTextBrowser(dialog);
    textBrowser->setOpenExternalLinks(true);
    textBrowser->setStyleSheet(R"(
        QTextBrowser {
            background-color: #111;
            color: #ffffff;
            border: 1px solid #333;
            border-radius: 6px;
            padding: 15px;
            font-family: 'Noto Serif Tibetan', sans-serif;
            font-size: 13px;
        }
        QTextBrowser h2 {
            color: #ffd700;
            margin-bottom: 15px;
        }
        QTextBrowser h3 {
            color: #ffd700;
            margin: 15px 0 5px 0;
        }
        QTextBrowser p {
            margin: 5px 0;
            line-height: 1.5;
        }
        QScrollBar:vertical {
            background-color: #2a2a2a;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #555;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #777;
        }
    )");

    QString html = QString(R"(
        <h2>How to Use Sadhana</h2>

        <h3>Getting Started</h3>
        <p>
            • Select a deity from the home grid to begin a practice session.<br>
            • Navigate through stages using the dots at the bottom.<br>
            • Each module includes opening prayers, mantra recitation, and closing dedications.
        </p>

        <h3>Custom Modules</h3>
        <p>
            • Click <span style="color: #3a6ea5;">"+ Create Custom Module"</span> on the home screen.<br>
            • Add unlimited stages with custom liturgy, mantras, images, audio, and PDFs.<br>
            • Right-click any module to change its grid image or delete it.
        </p>

        <h3>Tracking Practice</h3>
        <p>
            • Use the counter panel to log session repetitions.<br>
            • Lifetime counts are saved automatically.<br>
            • Open the Journal to view your practice history by date.
        </p>

        <h3>Keyboard Shortcuts</h3>
        <p>
            <b>Ctrl+H</b> — Return to home grid<br>
            <b>Ctrl+R</b> — Open Reader<br>
            <b>Ctrl+K</b> — Open Tibetan Calendar<br>
            <b>Ctrl+J</b> — Open Journal<br>
            <b>F</b> — Toggle fullscreen (text/PDF)<br>
            <b>I</b> — Fullscreen image view<br>
            <b>Esc</b> — Exit fullscreen
        </p>

        <h3>Tibetan Calendar</h3>
        <p>
            • Special observance days (Düchen) are highlighted.<br>
            • Karmic multipliers are shown for auspicious days.<br>
            • Tibetan lunar dates appear alongside Gregorian dates.
        </p>

        <h3>Audio Controls</h3>
        <p>
            • Assign audio files (MP3, WAV, FLAC, etc.) per stage.<br>
            • Use play/pause, stop, and progress slider.<br>
            • Enable repeat mode for continuous mantra playback.
        </p>

        <h3>PDF & Image Support</h3>
        <p>
            • Assign PDFs for practice texts or sadhana manuals.<br>
            • Zoom, fit to width, or fit to page with toolbar buttons.<br>
            • Click images or press I key for fullscreen zoomable view.
        </p>

        <h3>Data Storage</h3>
        <p>
            • All data stored locally in %1<br>
            • Custom modules saved as JSON files.<br>
            • Journal entries logged with timestamps.
        </p>
        )").arg(Constants::appDirPath);

    textBrowser->setHtml(html);
    layout->addWidget(textBrowser);

    // Close button
    QPushButton* closeButton = new QPushButton("Close", dialog);
    closeButton->setFixedHeight(32);
    closeButton->setFixedWidth(100);
    closeButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3a6ea5;
            color: white;
            border: none;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #4a7eb5;
        }
    )");
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}

void MainWindow::showRespectDialog()
{
    QString html = R"(
        <div style="color: #ffffff; font-family: 'Noto Serif Tibetan', sans-serif;">

            <h2 style="color: #ffd700; margin-bottom: 15px;">A Note of Respect</h2>

            <p style="margin: 10px 0;">
                The materials in this application are sacred to many traditions.<br>
                Please treat them with <span style="color: #ffd700;">respect and reverence</span>.
            </p>

            <p style="margin: 15px 0; padding: 10px; background-color: #2a2a2a; border-left: 4px solid #ffd700; border-radius: 4px;">
                <i>
                This application is intended to give you exposure to these profound<br>
                practices and teachings. It is a support tool for your personal practice.
                </i>
            </p>

            <p style="margin: 15px 0;">
                <b style="color: #ffd700;">Sadhana is not a replacement</b> for the guidance and<br>
                supervision of a qualified spiritual master or lineage teacher.
            </p>

            <p style="margin: 20px 0; font-size: 14px;">
                If the fire of longing for these teachings ignites in your heart,<br>
                we encourage you to <span style="color: #ffd700;">seek out an authentic teacher</span> and follow the path<br>
                under their compassionate guidance.
            </p>

            <p style="margin: 20px 0 0 0; color: #aaaaaa; font-style: italic;">
                May this application serve as a stepping stone on your journey.
            </p>

        </div>
    )";

    QMessageBox respectBox(this);
    respectBox.setWindowTitle("Respect and Guidance");
    respectBox.setTextFormat(Qt::RichText);
    respectBox.setText(html);
    respectBox.setStandardButtons(QMessageBox::Ok);
    respectBox.setStyleSheet("QMessageBox { background-color: #1a1a1a; } QMessageBox QLabel { color: #ffffff; min-width: 480px; }");
    respectBox.exec();
}


void MainWindow::showFindResourcesDialog()
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Find Practice Resources");
    dialog->setMinimumSize(550, 450);
    dialog->resize(600, 500);
    dialog->setStyleSheet("QDialog { background-color: #1a1a1a; }");
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    QTextBrowser* textBrowser = new QTextBrowser(dialog);

#ifdef FLATPAK_BUILD
    textBrowser->setOpenExternalLinks(false);
    connect(textBrowser, &QTextBrowser::anchorClicked, this, [this, dialog](const QUrl& link) {
        if (link.scheme() == "http" || link.scheme() == "https") {
            QDesktopServices::openUrl(link);
            dialog->close();
            showFindResourcesDialog();
        }
    });
#else
    textBrowser->setOpenExternalLinks(true);
#endif



    // Rest of your stylesheet...
    textBrowser->setStyleSheet(R"(
        QTextBrowser {
            background-color: #111;
            color: #ffffff;
            border: 1px solid #333;
            border-radius: 6px;
            padding: 15px;
            font-family: 'Noto Serif Tibetan', sans-serif;
            font-size: 13px;
        }
        QTextBrowser h2 { color: #ffd700; margin-bottom: 15px; }
        QTextBrowser h3 { color: #ffd700; margin: 15px 0 5px 0; }
        QTextBrowser p { margin: 5px 0; line-height: 1.5; }
        QScrollBar:vertical { background-color: #2a2a2a; width: 12px; border-radius: 6px; }
        QScrollBar::handle:vertical { background-color: #555; border-radius: 6px; min-height: 20px; }
        QScrollBar::handle:vertical:hover { background-color: #777; }
    )");

    QString html = R"(
        <h2>Finding Resources for Your Modules</h2>

        <p>
            Sadhana is designed to be a flexible framework. You can enrich your practice by finding
            authentic texts, images, and audio from a variety of online sources. Here are some suggestions:
        </p>

        <h3>Texts and Translations</h3>
        <p>
            • <a href="https://www.lotsawahouse.org/">Lotsawa House</a> - A rich trove of sadhanas and Buddhist texts in translation.<br>
            • <a href="https://www.84000.co/">84000: Translating the Words of the Buddha</a> - Canonical sūtras and tantras.<br>
            • <a href="https://www.sakya.org/">Sakya Resource Center</a> - Extensive collection of practice texts.<br>
            • <a href="https://fpmt.org/education/prayers-and-practices/">FPMT Prayers and Practices</a> - Free PDF sadhanas.
        </p>

        <h3>Mantra Recitation Videos (YouTube)</h3>
        <p>
            • Search for specific mantras to find traditional chanting styles.<br>
            • Many channels offer guided recitations with correct pronunciation.<br>
            • Look for recordings by authentic lineage teachers and monasteries.
        </p>

        <h3>Facebook Communities</h3>
        <p>
            • <a href="https://www.facebook.com/kgyaltshen">Khenpo Gyaltshen's Page</a> - Teachings and practice materials.<br>
            • <a href="https://www.facebook.com/ngawang123">Ngawang's Profile</a> - Additional resources and inspiration.<br>
            • Search for groups related to your specific tradition or practice.
        </p>

        <h3 style="color: #ffd700; margin-top: 25px;">Important Usage Notice</h3>
        <p style="padding: 10px; background-color: #2a2a2a; border-left: 4px solid #ffd700; border-radius: 4px;">
            <b>Personal Use Only:</b> Materials found online are for your <b>personal practice only</b>.
            Do not redistribute, republish, or share files obtained from these sources without explicit
            permission from the copyright holders. Respecting the intellectual property of translators,
            artists, and teachers is an essential part of Dharma practice.
        </p>

        <p style="margin-top: 20px; color: #aaaaaa; font-style: italic;">
            Tip: When assigning images, audio, or PDFs in Sadhana, use the "Assign" buttons in the
            control panel to link to files stored locally on your device.
        </p>
    )";

    textBrowser->setHtml(html);
    layout->addWidget(textBrowser);

    QPushButton* closeButton = new QPushButton("Close", dialog);
    closeButton->setFixedHeight(32);
    closeButton->setFixedWidth(100);
    closeButton->setStyleSheet(R"(
        QPushButton { background-color: #3a6ea5; color: white; border: none; border-radius: 4px; }
        QPushButton:hover { background-color: #4a7eb5; }
    )");
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    dialog->exec();
}


void MainWindow::showChangelogDialog()
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Changelog");
    dialog->setMinimumSize(550, 450);
    dialog->resize(600, 500);
    dialog->setStyleSheet("QDialog { background-color: #1a1a1a; }");

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    // Scrollable text area
    QTextBrowser* textBrowser = new QTextBrowser(dialog);
    textBrowser->setOpenExternalLinks(false);
    textBrowser->setStyleSheet(R"(
        QTextBrowser {
            background-color: #111;
            color: #ffffff;
            border: 1px solid #333;
            border-radius: 6px;
            padding: 15px;
            font-family: 'Noto Serif Tibetan', sans-serif;
            font-size: 13px;
        }
        QTextBrowser h1 {
            color: #ffd700;
            font-size: 22px;
            margin-bottom: 10px;
        }
        QTextBrowser h2 {
            color: #ffd700;
            font-size: 18px;
            margin: 20px 0 10px 0;
        }
        QTextBrowser h3 {
            color: #ffd700;
            font-size: 16px;
            margin: 15px 0 5px 0;
        }
        QTextBrowser p {
            margin: 5px 0;
            line-height: 1.5;
        }
        QTextBrowser ul {
            margin: 5px 0 10px 20px;
        }
        QTextBrowser li {
            margin: 3px 0;
            line-height: 1.5;
        }
        QTextBrowser .version {
            color: #ffd700;
            font-weight: bold;
            font-size: 16px;
        }
        QTextBrowser .date {
            color: #888;
            font-style: italic;
        }
        QTextBrowser .note {
            color: #ffd700;
            font-style: italic;
            margin-top: 15px;
            padding: 10px;
            background-color: #2a2a2a;
            border-left: 3px solid #ffd700;
        }
        QScrollBar:vertical {
            background-color: #2a2a2a;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #555;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #777;
        }
    )");

    QString html = QString(R"(
        <h1>📋 Changelog</h1>
        <p>All notable changes to Sadhana are documented here.</p>

        <h2>Version 1.0.1 <span class="date">— April 24, 2026</span></h2>
        <p><span class="version">Tradition-Agnostic Update</span></p>
        <ul>
            <li><b>Temporarily disabled default deity modules</b> (Chenrezig, Green Tara, Guru Rinpoche, Manjushri) to make the application tradition-agnostic out of the box</li>
            <li><b>Added instructional example module</b> teaching users how to use Sadhana without assuming any specific religious tradition</li>
            <li>Default modules may be restored in a future update based on user feedback</li>
            <li>Sadhana now launches as a neutral practice tracking tool — users are encouraged to create their own custom modules that fit their personal needs</li>
        </ul>

        <h2>Version 1.0.0 <span class="date">— April 14, 2026</span></h2>
        <p><span class="version">Initial Release</span></p>
        <ul>
            <li>Four default deity modules: Chenrezig, Green Tara, Guru Rinpoche, and Manjushri</li>
            <li>Complete Tibetan liturgy with phonetic and English translations</li>
            <li>Custom module creation with unlimited stages</li>
            <li>Mantra counter with lifetime and session tracking</li>
            <li>Integrated practice journal</li>
            <li>Tibetan Buddhist calendar with special observance days (2025-2049)</li>
            <li>Audio player with repeat functionality</li>
            <li>PDF viewer and text editor</li>
            <li>Fullscreen image and text viewing</li>
            <li>Dark theme interface with gold accents</li>
            <li>Keyboard shortcuts for efficient navigation</li>
            <li>Cross-platform support (Linux Flatpak, Windows, macOS)</li>
        </ul>

        <div class="note">
            💡 <b>Note:</b> Default deity modules from version 1.0.0 are temporarily disabled in version 1.1.0.
            They may be restored in a future update. In the meantime, use the <b>"+ Create Custom Module"</b>
            button to build practices that match your own tradition or needs.
        </div>
    )");

    textBrowser->setHtml(html);
    layout->addWidget(textBrowser);

    // Close button
    QPushButton* closeButton = new QPushButton("Close", dialog);
    closeButton->setFixedHeight(32);
    closeButton->setFixedWidth(100);
    closeButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3a6ea5;
            color: white;
            border: none;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #4a7eb5;
        }
    )");
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}
