#include "pujaview.h"
#include "modulemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QMediaPlayer>
#include <QStandardPaths>
#include <QScrollArea>
#include <QPixmap>
#include"imagefullscreendialog.h"
#include<QPdfDocument>
#include"journalmanager.h"
#include"constants.h"

PujaView::PujaView(QWidget *parent)
    : QWidget(parent)
    , m_currentModule(nullptr)
    , m_currentSection(0)
    , m_isPlaying(false)
    , journalDialog(new JournalDialog(this))
{
    setupUI();

}

PujaView::~PujaView()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        delete m_mediaPlayer;
    }
}

void PujaView::setupUI()
{
    setStyleSheet("background-color: #000000;");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 20, 40, 20);
    mainLayout->setSpacing(15);

    // ========== HEADER ==========
    QHBoxLayout* headerLayout = new QHBoxLayout();
    m_backButton = new QPushButton("← Back", this);
    m_backButton->setFixedSize(95, 32);
    m_backButton->setStyleSheet("QPushButton { background-color: #333; border-radius: 4px; } QPushButton:hover { background-color: #444; }");
    connect(m_backButton, &QPushButton::clicked, this, [this]() {
        // Stop media player if playing
        if (m_mediaPlayer && m_isPlaying) {
            m_mediaPlayer->stop();
            m_isPlaying = false;
            m_playPauseButton->setText("▶");
        }
        // Then go back to grid
        if(m_controlPanel->isVisible()) onToggleControlPanel();
        emit backToGrid();
    });
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();

    // Add info button at the opposite corner (right side)
    m_infoButton = new QPushButton("ⓘ", this);
    m_infoButton->setFixedSize(32, 32);

    //m_infoButton->setToolTip("Show keyboard shortcuts");
    m_infoButton->setToolTip("Keyboard Shortcuts"
                             "<html><body style='white-space: nowrap;'>"
                             "<b>🎹 Keyboard Shortcuts</b><br>"
                             "━━━━━━━━━━━━━━━━━━━━━━━━━━━━<br><br>"

                             "<b>Global Shortcuts:</b><br>"
                             "• <b>F</b> - Toggle fullscreen for text/PDF view<br>"
                             "• <b>I</b> - Enter image fullscreen mode<br>"
                             "• <b>ESC</b> - Exit fullscreen mode<br><br>"

                             "<b>📄 PDF Viewer (when PDF is visible):</b><br>"
                             "• <b>Ctrl + +</b> - Zoom in<br>"
                             "• <b>Ctrl + -</b> - Zoom out<br>"
                             "• <b>Ctrl + 0</b> - Reset to 100%<br>"
                             "• <b>Ctrl + 9</b> - Fit to width<br>"
                             "• <b>Ctrl + 8</b> - Fit to page<br><br>"

                             "<b>📝 Text Viewer (when text is visible):</b><br>"
                             "• <b>Ctrl + +</b> - Increase font size<br>"
                             "• <b>Ctrl + -</b> - Decrease font size<br>"
                             "• <b>Ctrl + 0</b> - Reset to default size (14pt)<br><br>"

                             "<b>🎵 Audio Controls:</b><br>"
                             "• Use the control panel buttons below<br>"
                             "• Volume slider available in control panel<br>"
                             "</body></html>"
                         );
    m_infoButton->setObjectName("infoButton");

    m_infoButton->setStyleSheet(
        "QPushButton#infoButton { "
        "background-color: rgba(13, 71, 161, 80); "   // Very faint
        "border-radius: 16px; "
        "font-size: 18px; "
        "font-weight: bold; "
        "color: rgba(255, 255, 255, 80); "            // Very faint text
        "border: none; "
        "padding: 0px; "
        "} "
        "QPushButton#infoButton:hover { "
        "background-color: rgba(21, 101, 192, 255); " // Full color on hover
        "color: rgba(255, 255, 255, 255); "           // Full white on hover
        "}"
    );

    headerLayout->addWidget(m_infoButton);

    mainLayout->addLayout(headerLayout);

    // ========== DEITY IMAGE ==========
    m_imageLabel = new QLabel(this);
    m_imageLabel->setFixedSize(280, 280);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet("border: 2px solid #333; border-radius: 12px; background-color: #111;");
    mainLayout->addWidget(m_imageLabel, 0, Qt::AlignCenter);

    // ========== MANTRA TEXT ==========
    m_mantraLabel = new QLabel(this);
    m_mantraLabel->setAlignment(Qt::AlignCenter);
    m_mantraLabel->setWordWrap(false);  // Changed to false - no line breaks
    m_mantraLabel->setVisible(false);
    m_mantraLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #ffd700; padding: 15px;");
    m_mantraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // Allow horizontal expansion
    mainLayout->addWidget(m_mantraLabel, 0, Qt::AlignCenter);

    // ========== TEXT AREA ==========
    m_textArea = new QTextBrowser(this);
    m_textArea->setMinimumHeight(200);
    m_textArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //m_textArea->setStyleSheet("QTextBrowser { background-color: #111; color: #ccc; border: 1px solid #333; border-radius: 8px; padding: 20px; font-size: 14px; }");
    m_textArea->setStyleSheet("QTextBrowser { background-color: #111; color: #ffd700; border: 1px solid #333; border-radius: 8px; padding: 20px; }");

    QFont font = m_textArea->font();
    font.setPointSize(14);
    m_textArea->setFont(font);
    mainLayout->addWidget(m_textArea, 1);

    // ========== PDF VIEW (hidden initially) ==========
    m_pdfView = new QPdfView(this);
    m_pdfView->setMinimumHeight(200);
    m_pdfView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pdfView->setStyleSheet("border: 1px solid #333; border-radius: 8px; background-color: #111;");
    m_pdfView->setVisible(false);

    // ADD THESE LINES FOR MULTI-PAGE VIEW
    //m_pdfView->setZoomMode(QPdfView::ZoomMode::FitToWidth);
    m_pdfView->setPageMode(QPdfView::PageMode::MultiPage);
   //m_pdfView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    mainLayout->addWidget(m_pdfView, 1);
    // ========== PROGRESS DOTS ==========
    m_dotsContainer = new QWidget(this);
    QHBoxLayout* dotsLayout = new QHBoxLayout(m_dotsContainer);
    dotsLayout->setAlignment(Qt::AlignCenter);
    dotsLayout->setSpacing(20);
    dotsLayout->setContentsMargins(0, 10, 0, 10);

    for (int i = 0; i < 4; ++i) {
        QPushButton* dot = new QPushButton(this);
        dot->setFixedSize(14, 14);
        dot->setCursor(Qt::PointingHandCursor);
        dot->setStyleSheet("QPushButton { background-color: #555; border-radius: 7px; } QPushButton:hover { background-color: #888; }");
        connect(dot, &QPushButton::clicked, this, [this, i]() { onSectionClicked(i); });
        dotsLayout->addWidget(dot);
        m_dots.append(dot);
    }
    mainLayout->addWidget(m_dotsContainer, 0, Qt::AlignCenter);

    // ========== CONTROL PANEL TOGGLE ==========
    m_togglePanelButton = new QPushButton("▼ Show Controls", this);

    m_togglePanelButton->setFixedHeight(32);
    m_togglePanelButton->setCursor(Qt::PointingHandCursor);
    m_togglePanelButton->setStyleSheet("QPushButton { background-color: #2a2a2a; border: 1px solid #444; border-radius: 4px; } QPushButton:hover { background-color: #3a3a3a; }");
    connect(m_togglePanelButton, &QPushButton::clicked, this, &PujaView::onToggleControlPanel);
    mainLayout->addWidget(m_togglePanelButton, 0, Qt::AlignCenter);

    // ========== CONTROL PANEL ==========
    m_controlPanel = new QWidget(this);
    m_controlPanel->setMinimumHeight(300);
    m_controlPanel->setVisible(false);
    m_panelVisible = false;
    setupControlPanel();
    mainLayout->addWidget(m_controlPanel);
    onSectionClicked(0);
}

void PujaView::setupControlPanel()
{
    QScrollArea* scrollArea = new QScrollArea(m_controlPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    QWidget* contentWidget = new QWidget();
    QVBoxLayout* panelLayout = new QVBoxLayout(contentWidget);
    panelLayout->setSpacing(15);
    panelLayout->setContentsMargins(15, 15, 15, 15);
    panelLayout->setAlignment(Qt::AlignTop);


    // ========== STAGE MANAGEMENT (for custom modules) ==========
    QHBoxLayout* stageManagementLayout = new QHBoxLayout();
    m_addStageButton = new QPushButton("+ Add Stage", this);
    m_addStageButton->setVisible(false);
    m_addStageButton->setFixedHeight(32);
    m_addStageButton->setMinimumWidth(100);
    m_addStageButton->setStyleSheet("QPushButton { background-color: #3a6ea5; border-radius: 4px; }");
    connect(m_addStageButton, &QPushButton::clicked, this, &PujaView::addNewStage);
    stageManagementLayout->addWidget(m_addStageButton);
    stageManagementLayout->addStretch();
    panelLayout->addLayout(stageManagementLayout);


    stageNameLabel = new QLabel("Stage Name:", this);
    stageNameLabel->setStyleSheet("color: #ffd700; font-weight: bold; font-size: 12px; margin-top: 5px;");
    panelLayout->addWidget(stageNameLabel);

    m_stageNameEdit = new QLineEdit(this);
    m_stageNameEdit->setPlaceholderText("Stage name...");
    m_stageNameEdit->setStyleSheet("QLineEdit { background-color: #111; color: #ccc; border: 1px solid #333; border-radius: 4px; padding: 6px; }");
    connect(m_stageNameEdit, &QLineEdit::textChanged, this, &PujaView::onStageNameChanged);
    panelLayout->addWidget(m_stageNameEdit);


    // ========== AUDIO SECTION ==========
    QLabel* audioLabel = new QLabel("AUDIO", this);
    audioLabel->setStyleSheet("color: #ffd700; font-weight: bold; font-size: 12px; letter-spacing: 1px;");
    panelLayout->addWidget(audioLabel);

    QHBoxLayout* audioLayout = new QHBoxLayout();
    audioLayout->setSpacing(6);

    // Buttons
    m_assignAudioButton = new QPushButton("Assign Audio", this);
    m_prevButton = new QPushButton("⏮", this);
    m_prevButton->setVisible(false);
    m_playPauseButton = new QPushButton("▶", this);
    m_stopButton = new QPushButton("⏹", this);
    m_nextButton = new QPushButton("⏭", this);
    m_nextButton->setVisible(false);

    m_repeatButton = new QPushButton("R", this);
    m_repeatButton->setCheckable(true);
    m_repeatButton->setChecked(true);
    m_repeatButton->setEnabled(true);

    // Same style as Assign Image/PDF
    QString grayButtonStyle = "QPushButton { background-color: #3a3a3a; border-radius: 4px; } QPushButton:hover { background-color: #4a4a4a; }";
    m_assignAudioButton->setStyleSheet(grayButtonStyle);
    m_prevButton->setStyleSheet(grayButtonStyle);
    m_playPauseButton->setStyleSheet(grayButtonStyle);
    m_stopButton->setStyleSheet(grayButtonStyle);
    m_nextButton->setStyleSheet(grayButtonStyle);
    m_repeatButton->setStyleSheet(grayButtonStyle);

    m_assignAudioButton->setFixedHeight(32);
    m_playPauseButton->setFixedHeight(32);
    m_stopButton->setFixedHeight(32);
    m_repeatButton->setFixedHeight(32);

   m_playPauseButton->setEnabled(true);
    m_stopButton->setEnabled(true);
    m_prevButton->setEnabled(false);
    m_nextButton->setEnabled(false);

    connect(m_assignAudioButton, &QPushButton::clicked, this, &PujaView::onAssignAudio);
    connect(m_playPauseButton, &QPushButton::clicked, this, &PujaView::onPlayPause);
    connect(m_stopButton, &QPushButton::clicked, this, &PujaView::onStop);



    m_currentAudioLabel = new QLabel("No audio assigned", this);
    m_currentAudioLabel->setStyleSheet("color: #888; font-size: 11px;");
    m_currentAudioLabel->setWordWrap(true);
    panelLayout->addWidget(m_currentAudioLabel);

    audioLayout->addWidget(m_assignAudioButton);
    audioLayout->addWidget(m_prevButton);
    audioLayout->addWidget(m_playPauseButton);
    audioLayout->addWidget(m_stopButton);
    audioLayout->addWidget(m_nextButton);
    audioLayout->addWidget(m_repeatButton);

    m_progressSlider = new QSlider(Qt::Horizontal, this);
    m_progressSlider->setEnabled(false);
    connect(m_progressSlider, &QSlider::sliderMoved, this, &PujaView::onSeek);
    audioLayout->addWidget(m_progressSlider, 1);

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(70);
    m_volumeSlider->setFixedWidth(80);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &PujaView::onVolumeChanged);

    audioLayout->addWidget(m_volumeSlider);
    audioLayout->addStretch();
    panelLayout->addLayout(audioLayout);
    // ========== IMAGE SECTION ==========
    QLabel* imageLabel = new QLabel("IMAGE", this);
    imageLabel->setStyleSheet("color: #ffd700; font-weight: bold; font-size: 12px; letter-spacing: 1px; margin-top: 5px;");
    panelLayout->addWidget(imageLabel);

    QHBoxLayout* imageLayout = new QHBoxLayout();
    m_assignImageButton = new QPushButton("Assign Image", this);
    m_assignImageButton->setFixedHeight(32);
    m_assignImageButton->setMinimumWidth(120);
    m_assignImageButton->setStyleSheet("QPushButton { background-color: #3a3a3a; border-radius: 4px; } QPushButton:hover { background-color: #4a4a4a; }");
    connect(m_assignImageButton, &QPushButton::clicked, this, &PujaView::onAssignImage);
    imageLayout->addWidget(m_assignImageButton);
    imageLayout->addStretch();
    panelLayout->addLayout(imageLayout);

    m_currentImageLabel = new QLabel("No image assigned", this);
    m_currentImageLabel->setStyleSheet("color: #888; font-size: 11px;");
    m_currentImageLabel->setWordWrap(true);
    panelLayout->addWidget(m_currentImageLabel);


    liturgyLabel = new QLabel("LITURGY TEXT", this);
    liturgyLabel->setStyleSheet("color: #ffd700; font-weight: bold; font-size: 12px; letter-spacing: 1px; margin-top: 5px;");
    panelLayout->addWidget(liturgyLabel);

    m_liturgyEdit = new QTextEdit(this);
    m_liturgyEdit->setMaximumHeight(150);
    m_liturgyEdit->setPlaceholderText("Enter liturgy text for this stage...");
    m_liturgyEdit->setStyleSheet("QTextEdit { background-color: #111; color: #ccc; border: 1px solid #333; border-radius: 8px; padding: 8px; }");
    connect(m_liturgyEdit, &QTextEdit::textChanged, this, &PujaView::onLiturgyTextChanged);
    panelLayout->addWidget(m_liturgyEdit);


    loadTextButton = new QPushButton("Load", this);
    loadTextButton->setFixedHeight(32);
    loadTextButton->setFixedWidth(120);
    loadTextButton->setStyleSheet("QPushButton { background-color: #3a3a3a; border-radius: 4px; }");
    connect(loadTextButton, &QPushButton::clicked, this, &PujaView::onLoadLiturgyFromFile);
    panelLayout->addWidget(loadTextButton);


    // ========== MANTRA TEXT EDITOR ==========
    mantraLabel = new QLabel("MANTRA TEXT", this);
    mantraLabel->setStyleSheet("color: #ffd700; font-weight: bold; font-size: 12px; letter-spacing: 1px; margin-top: 5px;");
    panelLayout->addWidget(mantraLabel);

    m_mantraEdit = new QTextEdit(this);
    m_mantraEdit->setMaximumHeight(80);
    m_mantraEdit->setPlaceholderText("Enter mantra text for this stage...");
    m_mantraEdit->setStyleSheet("QTextEdit { background-color: #111; color: #ccc; border: 1px solid #333; border-radius: 8px; padding: 8px; }");
    connect(m_mantraEdit, &QTextEdit::textChanged, this, &PujaView::onMantraTextChanged);
    panelLayout->addWidget(m_mantraEdit);


    // ========== PDF SECTION ==========
    pdfLabel = new QLabel("PDF", this);
    pdfLabel->setStyleSheet("color: #ffd700; font-weight: bold; font-size: 12px; letter-spacing: 1px; margin-top: 5px;");
    panelLayout->addWidget(pdfLabel);

    QHBoxLayout* pdfLayout = new QHBoxLayout();
    pdfLayout->setSpacing(10);

    m_assignPdfButton = new QPushButton("Assign PDF", this);
    m_viewPdfButton = new QPushButton("Toggle View", this);
    m_assignPdfButton->setFixedHeight(32);
    m_viewPdfButton->setFixedHeight(32);
    m_assignPdfButton->setMinimumWidth(90);
    m_viewPdfButton->setMinimumWidth(90);

    QString pdfButtonStyle = "QPushButton { background-color: #3a3a3a; border-radius: 4px; } QPushButton:hover { background-color: #4a4a4a; }";
    m_assignPdfButton->setStyleSheet(pdfButtonStyle);
    m_viewPdfButton->setStyleSheet(pdfButtonStyle);

    m_viewPdfButton->setEnabled(true);


    connect(m_assignPdfButton, &QPushButton::clicked, this, &PujaView::onAssignPdf);

    connect(m_viewPdfButton, &QPushButton::clicked, this, [this]{
        if (m_isCustomModule && m_currentStageIndex >= 0) {
            Stage& stage = m_currentStages[m_currentStageIndex];
            if (!stage.pdfPath.isEmpty()) {
                // Toggle the stored preference
                bool newShowPdf = !m_stageShowPdfMode.value(m_currentStageIndex, true);
                m_stageShowPdfMode[m_currentStageIndex] = newShowPdf;

                // Reload stage to apply change
                loadStage(m_currentStageIndex);
            }
        }
    });

    // Current PDF file display
    m_currentPdfLabel = new QLabel("No PDF assigned", this);
    m_currentPdfLabel->setStyleSheet("color: #888; font-size: 11px;");
    m_currentPdfLabel->setWordWrap(true);

    panelLayout->addWidget(m_currentPdfLabel);

    pdfLayout->addWidget(m_assignPdfButton);
    pdfLayout->addWidget(m_viewPdfButton);
    pdfLayout->addStretch();
    panelLayout->addLayout(pdfLayout);

    // ========== COUNTER SECTION ==========
    QLabel* counterLabel = new QLabel("COUNTER", this);
    counterLabel->setStyleSheet("color: #ffd700; font-weight: bold; font-size: 12px; letter-spacing: 1px; margin-top: 5px;");

    panelLayout->addWidget(counterLabel);

    QHBoxLayout* counterLayout = new QHBoxLayout();
    counterLayout->setSpacing(10);

    QLabel* totalLabel = new QLabel("Total:", this);
    totalLabel->setStyleSheet("color: #ccc;");
    m_totalCountLabel = new QLabel("0", this);
    m_totalCountLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffd700; min-width: 80px;");

    QLabel* sessionLabel = new QLabel("This session:", this);
    sessionLabel->setStyleSheet("color: #ccc;");
    m_sessionSpinBox = new QSpinBox(this);
    m_sessionSpinBox->setMinimum(0);
    m_sessionSpinBox->setMaximum(99999);
    m_sessionSpinBox->setButtonSymbols(QSpinBox::PlusMinus);
    m_sessionSpinBox->setFixedHeight(30);
    m_sessionSpinBox->setMinimumWidth(100);
    m_sessionSpinBox->setStyleSheet("QSpinBox { background-color: #222; color: white; border: 1px solid #444; border-radius: 4px; padding: 4px; }");

    m_addButton = new QPushButton("+ Add", this);
    m_addButton->setFixedHeight(32);
    m_addButton->setMinimumWidth(70);
    m_addButton->setStyleSheet("QPushButton { background-color: #3a6ea5; border-radius: 4px; } QPushButton:hover { background-color: #4a7eb5; }");
    connect(m_addButton, &QPushButton::clicked, this, &PujaView::onAddSession);

    // Reset counter button
    m_resetCounterButton = new QPushButton("Reset", this);
    m_resetCounterButton->setFixedHeight(32);
    m_resetCounterButton->setMinimumWidth(70);
    m_resetCounterButton->setStyleSheet("QPushButton { background-color: #8b3a3a; border-radius: 4px; } QPushButton:hover { background-color: #a04a4a; }");
    connect(m_resetCounterButton, &QPushButton::clicked, this, &PujaView::onResetCounter);

    // ADD MANTRA DISPLAY
    mantraLabelCpanel = new QLabel("Mantra: ", this);
    mantraLabelCpanel->setStyleSheet("color: #ccc;");
    //mantraLabelCpanel->setVisible(false);

    counterLayout->addWidget(totalLabel);
    counterLayout->addWidget(m_totalCountLabel);
    counterLayout->addSpacing(20);
    counterLayout->addWidget(sessionLabel);
    counterLayout->addWidget(m_sessionSpinBox);
    counterLayout->addWidget(m_addButton);
    counterLayout->addWidget(m_resetCounterButton);
    counterLayout->addWidget(mantraLabelCpanel);
    counterLayout->addStretch();
    panelLayout->addLayout(counterLayout);

    // ========== JOURNAL SECTION ==========
    QHBoxLayout* journalLayout = new QHBoxLayout();
    m_openJournalButton = new QPushButton("Open Journal", this);
    m_openJournalButton->setFixedHeight(34);
    m_openJournalButton->setMinimumWidth(120);
    m_openJournalButton->setStyleSheet("QPushButton { background-color: #3a3a3a; border-radius: 4px; } QPushButton:hover { background-color: #4a4a4a; }");
    connect(m_openJournalButton, &QPushButton::clicked, this, &PujaView::onOpenJournal);
    journalLayout->addWidget(m_openJournalButton);
    journalLayout->addStretch();
    panelLayout->addLayout(journalLayout);

    scrollArea->setWidget(contentWidget);

    QVBoxLayout* controlLayout = new QVBoxLayout(m_controlPanel);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->addWidget(scrollArea);

    // Media player
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);  // Create audio output
    m_mediaPlayer->setAudioOutput(m_audioOutput);

    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
        if (state == QMediaPlayer::StoppedState) {
            m_isPlaying = false;
            m_playPauseButton->setText("▶");
            m_progressSlider->setValue(0);
        } else if (state == QMediaPlayer::PausedState) {
            m_isPlaying = false;
            m_playPauseButton->setText("▶");
        } else if (state == QMediaPlayer::PlayingState) {
            m_isPlaying = true;
            m_playPauseButton->setText("||");
        }

    });

    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            if (m_repeatButton->isChecked()) {
                // Replay the current media
                m_mediaPlayer->setPosition(0);
                m_mediaPlayer->play();
            }
        }
    });

    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &PujaView::onPositionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &PujaView::onDurationChanged);
    connect(m_volumeSlider, &QSlider::valueChanged, m_audioOutput, &QAudioOutput::setVolume);

    m_audioOutput->setVolume(70);
}



void PujaView::onToggleControlPanel()
{
    m_panelVisible = !m_panelVisible;
    m_controlPanel->setVisible(m_panelVisible);

    if (m_panelVisible) {
        // Hiding control panel
        m_togglePanelButton->setText("▲ Hide Controls");
        m_imageLabel->setVisible(false);
        m_mantraLabel->setVisible(false);
        m_pdfView->setVisible(false);
        m_textArea->setVisible(false);
    } else {
        // Showing control panel - restore based on m_currentlyShowingPdf
        m_togglePanelButton->setText("▼ Show Controls");
        if(hasImage) m_imageLabel->setVisible(true);

        //if(hasMantra) m_mantraLabel->setVisible(!m_mantraLabel->text().isEmpty());
        if(hasMantra) m_mantraLabel->setVisible(true);

        if (m_currentlyShowingPdf) {
            m_pdfView->setVisible(true);
            m_textArea->setVisible(false);
        } else {
            m_pdfView->setVisible(false);
            m_textArea->setVisible(true);
        }
    }
}

void PujaView::onSectionClicked(int sectionIndex)
{
    m_currentSection = sectionIndex;

    for (int i = 0; i < m_dots.size(); ++i) {
        if (i == sectionIndex) {
            m_dots[i]->setStyleSheet("QPushButton { background-color: #ffd700; border-radius: 6px; }");
        } else {
            m_dots[i]->setStyleSheet("QPushButton { background-color: #555; border-radius: 6px; } QPushButton:hover { background-color: #888; }");
        }
    }

    updateTextArea();

    if (sectionIndex == 2) {
        m_mantraLabel->setVisible(true);
        hasMantra = true;
    } else {
        m_mantraLabel->setVisible(false);
        hasMantra = false;

    }
}

void PujaView::updateTextArea()
{
    if (!m_currentModule) return;

    QString text;
    switch (m_currentSection) {
    case 0:  // About
        text = m_currentModule->deityInfo();
        break;
    case 1:  // Opening
        text = m_currentModule->openingText();
        break;
    case 2:  // Main practice
        text = "Recite the mantra:\n\n" + m_currentModule->mantraText() + "\n\nTap the counter below after each mala or set of recitations.";
        break;
    case 3:  // Closing
        text = m_currentModule->closingText();
        break;
    }

    m_textArea->setText(text);
}


void PujaView::updateImageDisplay()
{
    if (!m_currentModule) return;

    QPixmap pixmap(m_currentModule->effectiveImagePath());
    if (!pixmap.isNull()) {
        // Scale to fit 280x280 while preserving aspect ratio
        QPixmap scaled = pixmap.scaled(280, 280,
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);
        m_imageLabel->setPixmap(scaled);
    } else {
        m_imageLabel->setText("Image not found");
    }
}

void PujaView::updateAudioButtonState()
{
    if (m_currentModule && !m_currentModule->audioPath().isEmpty()) {
        m_playPauseButton->setEnabled(true);
        m_stopButton->setEnabled(true);
    } else {
        m_playPauseButton->setEnabled(false);
        m_stopButton->setEnabled(false);
    }
}


/*
void PujaView::onStop()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        m_progressSlider->setValue(0);
    }
}
*/

void PujaView::onStop()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        m_isPlaying = false;
        m_playPauseButton->setText("▶");
        m_progressSlider->setValue(0);
        // Don't clear m_nextAudioUrl - keep it for next play
    }
}

void PujaView::onSeek(int position)
{
    if (m_mediaPlayer) {
        m_mediaPlayer->setPosition(position);
    }
}

void PujaView::onVolumeChanged(int volume)
{
    if (m_audioOutput) {
        m_audioOutput->setVolume(volume / 100.0);  // Convert 0-100 to 0.0-1.0
    }
}

void PujaView::onPositionChanged(qint64 position)
{
    if (!m_progressSlider->isSliderDown()) {
        m_progressSlider->setValue(position);
    }
}

void PujaView::onDurationChanged(qint64 duration)
{
    m_progressSlider->setRange(0, duration);
    m_progressSlider->setEnabled(duration > 0);
}

void PujaView::onAssignImage()
{



    if (m_isCustomModule && m_currentCustomModule) {
        // Custom module - save to current stage
        QString filePath = QFileDialog::getOpenFileName(this,
            "Select Image for Stage: " + m_currentStages[m_currentStageIndex].name,
            Constants::imagesPath,
            "Image Files (*.png *.jpg *.jpeg *.bmp)");

        if (!filePath.isEmpty()) {
            m_currentStages[m_currentStageIndex].imagePath = filePath;
            m_currentCustomModule->setStages(m_currentStages);
            ModuleManager::instance().saveCustomModule(m_currentCustomModule);
            hasImage = true;
            m_togglePanelButton->click();

            loadStage(m_currentStageIndex); // Refresh display

            QFileInfo imageInfo(filePath);
            m_currentImageLabel->setText("Image: " + imageInfo.fileName());
            m_currentImageLabel->setStyleSheet("color: #ffd700; font-size: 11px;");

        }
    } else if (m_currentModule) {
        // Default module - save globally
        QString filePath = QFileDialog::getOpenFileName(this,
            "Select Image for " + m_currentModule->name(),
            Constants::imagesPath,
            "Image Files (*.png *.jpg *.jpeg *.bmp)");

        if (!filePath.isEmpty()) {
            m_togglePanelButton->click();
            ModuleManager::instance().saveImageOverride(m_currentModule->id(), filePath);
            hasImage = true;
            m_currentModule->setUserImagePath(filePath);
            updateImageDisplay();

            QFileInfo imageInfo(filePath);
            m_currentImageLabel->setText("Image: " + imageInfo.fileName());
            m_currentImageLabel->setStyleSheet("color: #ffd700; font-size: 11px;");

            emit backToGrid();
        }
    }
}



void PujaView::saveModuleData()
{
    ModuleManager::instance().saveModules();
}

void PujaView::keyPressEvent(QKeyEvent* event)
{


    // PDF Zoom shortcuts
    if (m_pdfView && m_pdfView->isVisible()) {
        // Ctrl + = (plus) - Zoom in
        if ((event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) &&
            event->modifiers() == Qt::ControlModifier) {
            qreal currentZoom = m_pdfView->zoomFactor();
            m_pdfView->setZoomMode(QPdfView::ZoomMode::Custom);
            m_pdfView->setZoomFactor(currentZoom + 0.25);
            event->accept();
            return;
        }
        // Ctrl + - (minus) - Zoom out
        else if (event->key() == Qt::Key_Minus && event->modifiers() == Qt::ControlModifier) {
            qreal currentZoom = m_pdfView->zoomFactor();
            m_pdfView->setZoomMode(QPdfView::ZoomMode::Custom);
            m_pdfView->setZoomFactor(currentZoom - 0.25);
            event->accept();
            return;
        }
        // Ctrl + 0 - Reset to 100%
        else if (event->key() == Qt::Key_0 && event->modifiers() == Qt::ControlModifier) {
            m_pdfView->setZoomMode(QPdfView::ZoomMode::Custom);
            m_pdfView->setZoomFactor(1.0);
            event->accept();
            return;
        }
        // Ctrl + 9 - Fit to width
        else if (event->key() == Qt::Key_9 && event->modifiers() == Qt::ControlModifier) {
            m_pdfView->setZoomMode(QPdfView::ZoomMode::FitToWidth);
            event->accept();
            return;
        }
        // Ctrl + 8 - Fit to page
        else if (event->key() == Qt::Key_8 && event->modifiers() == Qt::ControlModifier) {
            m_pdfView->setZoomMode(QPdfView::ZoomMode::FitInView);
            event->accept();
            return;
        }
    }

    else if (m_textArea && m_textArea->isVisible()) {
        if ((event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) &&
            event->modifiers() == Qt::ControlModifier) {
            QFont font = m_textArea->font();
            font.setPointSize(font.pointSize() + 1);
            m_textArea->setFont(font);
            event->accept();
            return;
        }
        else if (event->key() == Qt::Key_Minus && event->modifiers() == Qt::ControlModifier) {
            QFont font = m_textArea->font();
            int newSize = font.pointSize() - 1;
            if (newSize >= 6) {
                font.setPointSize(newSize);
                m_textArea->setFont(font);
            }
            event->accept();
            return;
        }
        else if (event->key() == Qt::Key_0 && event->modifiers() == Qt::ControlModifier) {
            QFont font = m_textArea->font();
            font.setPointSize(14);
            m_textArea->setFont(font);
            event->accept();
            return;
        }
    }


    if (event->key() == Qt::Key_F) {
        if (!m_fullscreenMode) {
            // Find which widget is currently visible (text or PDF only)
            QWidget* activeWidget = nullptr;

            if (m_pdfView && m_pdfView->isVisible()) {
                activeWidget = m_pdfView;
            } else if (m_textArea && m_textArea->isVisible()) {
                activeWidget = m_textArea;
            }

            if (activeWidget) {
                m_fullscreenMode = true;
                m_fullscreenWidget = activeWidget;

                // Hide all controls and other widgets
                m_backButton->setVisible(false);
                m_mantraLabel->setVisible(false);
                m_dotsContainer->setVisible(false);
                m_togglePanelButton->setVisible(false);
                m_controlPanel->setVisible(false);

                // Hide other content widgets
                if (m_textArea && m_textArea != activeWidget) m_textArea->setVisible(false);
                if (m_pdfView && m_pdfView != activeWidget) m_pdfView->setVisible(false);
                m_imageLabel->setVisible(false);

                // Make active widget fill the space
                activeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                activeWidget->setMinimumHeight(0);
            }
        } else {
            // Exit fullscreen mode (text or PDF only)
            m_fullscreenMode = false;
            m_fullscreenWidget = nullptr;

            // Show all controls
            m_backButton->setVisible(true);
            if(hasMantra) m_mantraLabel->setVisible(true);
            m_dotsContainer->setVisible(true);
            m_togglePanelButton->setVisible(true);
            //m_controlPanel->setVisible(true);
            m_pdfView->setVisible(isPdf);
            m_textArea->setVisible(!isPdf);
            if(hasImage) m_imageLabel->setVisible(true);

            // Restore original sizes
            m_imageLabel->setFixedSize(280, 280);
            m_textArea->setMinimumHeight(200);

            // Restore original image display
            updateImageDisplay();
        }
    } else if (event->key() == Qt::Key_I) {
        // Image fullscreen dialog on I key
        QString imagePath;

        if (m_isCustomModule && m_currentCustomModule) {
            // Get image from current stage or module default
            if (m_currentStageIndex >= 0 && m_currentStageIndex < m_currentStages.size()) {
                if (!m_currentStages[m_currentStageIndex].imagePath.isEmpty()) {
                    imagePath = m_currentStages[m_currentStageIndex].imagePath;
                }
            }
            if (imagePath.isEmpty()) {
                imagePath = m_currentCustomModule->effectiveImagePath();
            }
        } else if (m_currentModule) {
            imagePath = m_currentModule->effectiveImagePath();
        }

        if (!imagePath.isEmpty() && m_imageLabel && m_imageLabel->isVisible()) {
            ImageFullscreenDialog* dialog = new ImageFullscreenDialog(imagePath, this);
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            dialog->show();
            return;
        }
    }
    QWidget::keyPressEvent(event);
}


/*
void PujaView::enableEditing(bool enabled)
{


    // Assignment buttons
    //m_assignAudioButton->setEnabled(enabled);
    //m_assignImageButton->setEnabled(enabled);
    m_assignPdfButton->setEnabled(enabled);
    m_viewPdfButton->setEnabled(enabled);
    m_stageNameEdit->setEnabled(enabled);
    // Custom module specific widgets (visible only for custom modules)
    if (m_addStageButton) m_addStageButton->setVisible(enabled);
    if (m_liturgyEdit) m_liturgyEdit->setVisible(enabled);
    if (m_mantraEdit) m_mantraEdit->setVisible(enabled);
    if (loadTextButton) loadTextButton->setVisible(enabled);
}
*/

void PujaView::enableEditing(bool enabled)
{
    // Labels
    if (stageNameLabel) stageNameLabel->setVisible(enabled);
    if (liturgyLabel) liturgyLabel->setVisible(enabled);
    if (mantraLabel) mantraLabel->setVisible(enabled);

    // PDF section
    m_assignPdfButton->setVisible(enabled);
    m_viewPdfButton->setVisible(enabled);
    m_currentPdfLabel->setVisible(enabled);
    if (pdfLabel) pdfLabel->setVisible(enabled);

    // Stage name edit
    m_stageNameEdit->setVisible(enabled);

    // Text edit widgets
    m_liturgyEdit->setVisible(enabled);
    m_mantraEdit->setVisible(enabled);
    loadTextButton->setVisible(enabled);

    // Custom module specific widgets
    if (m_addStageButton) m_addStageButton->setVisible(enabled);

    // AUDIO, IMAGE, COUNTER, JOURNAL sections remain always visible (not touched)
}

void PujaView::rebuildStageDots()
{
    // Clear existing dots
    for (QPushButton* dot : m_dots) {
        delete dot;
    }
    m_dots.clear();

    // Clear the layout
    QLayout* dotsLayout = m_dotsContainer->layout();
    while (QLayoutItem* item = dotsLayout->takeAt(0)) {
        delete item->widget();
    }

    // Create new dots
    for (int i = 0; i < m_currentStages.size(); ++i) {
        QPushButton* dot = new QPushButton(this);
        dot->setFixedSize(14, 14);
        dot->setCursor(Qt::PointingHandCursor);
        dot->setStyleSheet("QPushButton { background-color: #555; border-radius: 7px; }");
        dot->setToolTip(m_currentStages[i].name);
        connect(dot, &QPushButton::clicked, this, [this, i]() { loadStage(i); });
        dotsLayout->addWidget(dot);
        m_dots.append(dot);
    }
}

void PujaView::loadStage(int index)
{
    // Handle default module (no stages in m_currentStages)
    if (!m_isCustomModule && m_currentModule) {
        if (index < 0 || index > 3) return;
    m_loadingStage = true;


        if(m_controlPanel->isVisible()) onToggleControlPanel();
        m_currentStageIndex = index;

        // Update dot highlighting
        for (int i = 0; i < m_dots.size(); ++i) {
            if (i == index) {
                m_dots[i]->setStyleSheet("QPushButton { background-color: #ffd700; border-radius: 6px; }");
            } else {
                m_dots[i]->setStyleSheet("QPushButton { background-color: #555; border-radius: 6px; }");
            }
        }

        // Load image from default module
        QString imagePath = m_currentModule->effectiveImagePath();
        if (!imagePath.isEmpty()) {
            hasImage = true;
            QPixmap pixmap(imagePath);
            if (!pixmap.isNull()) {
                QPixmap scaled = pixmap.scaled(280, 280, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                m_imageLabel->setPixmap(scaled);
                m_imageLabel->setVisible(true);
            }
        } else {
            hasImage = false;
            m_imageLabel->setVisible(false);
        }

        if(m_mantraLabel->text().isEmpty()){
            hasMantra = false;
        }else{
            hasMantra = true;;

        }
        // Load content based on stage index
        if (index == 0) {
            // About/Info stage
            m_textArea->setText(m_currentModule->deityInfo());
            m_mantraLabel->setVisible(false);
            mantraLabelCpanel->setVisible(false);
            m_pdfView->setVisible(false);
            m_textArea->setVisible(true);
            m_imageLabel->setVisible(true);
            m_mantraLabel->setVisible(false);
            hasMantra = false;
        } else if (index == 1) {
            // Opening
            m_textArea->setText(m_currentModule->openingText());
            m_mantraLabel->setVisible(false);
            mantraLabelCpanel->setVisible(false);
            m_pdfView->setVisible(false);
            m_textArea->setVisible(true);
            m_imageLabel->setVisible(true);
            m_mantraLabel->setVisible(false);
            hasMantra = false;

        } else if (index == 2) {
            // Main practice
            m_textArea->setText("");

            m_mantraLabel->setText(m_currentModule->mantraText());
            m_mantraLabel->setVisible(true);
            mantraLabelCpanel->setText("Mantra: " + m_currentModule->mantraText());
            mantraLabelCpanel->setVisible(true);
            m_sessionSpinBox->setEnabled(true);
            m_addButton->setEnabled(true);
            m_pdfView->setVisible(false);
            m_textArea->setVisible(true);
            m_imageLabel->setVisible(true);
            m_mantraLabel->setVisible(true);
            hasMantra = true;

        } else if (index == 3) {
            // Closing
            m_textArea->setText(m_currentModule->closingText());
            m_mantraLabel->setVisible(false);
            mantraLabelCpanel->setVisible(false);
            m_pdfView->setVisible(false);
            m_textArea->setVisible(true);
            m_imageLabel->setVisible(true);
            m_mantraLabel->setVisible(false);
            hasMantra = false;

        }
        // Clear editors
        m_liturgyEdit->clear();
        m_mantraEdit->clear();

        // Update labels
        m_currentImageLabel->setText("Image: " + QFileInfo(imagePath).fileName());


        if (!m_currentModule->audioPath().isEmpty()) {
            m_nextAudioUrl = QUrl::fromLocalFile(m_currentModule->audioPath());
            QFileInfo audioInfo(m_currentModule->audioPath());
            m_currentAudioLabel->setText("Audio: " + audioInfo.fileName());
            m_currentAudioLabel->setStyleSheet("color: #ffd700; font-size: 11px;");
        } else {
            m_nextAudioUrl = QUrl();
            m_currentAudioLabel->setText("No audio assigned");
            m_currentAudioLabel->setStyleSheet("color: #888; font-size: 11px;");
        }


        m_currentPdfLabel->setText("No PDF assigned");

        // Update counter display
        m_totalCountLabel->setText(QString::number(m_currentModule->lifetimeCount()));

        return;
    }

    // Existing code for custom modules
    if (index < 0 || index >= m_currentStages.size()) return;

    if(m_controlPanel->isVisible()) onToggleControlPanel();

    m_currentStageIndex = index;
    Stage& stage = m_currentStages[index];

    // Update dot highlighting
    for (int i = 0; i < m_dots.size(); ++i) {
        if (i == index) {
            m_dots[i]->setStyleSheet("QPushButton { background-color: #ffd700; border-radius: 6px; }");
        } else {
            m_dots[i]->setStyleSheet("QPushButton { background-color: #555; border-radius: 6px; }");
        }
    }

    // Load image
    if (stage.showImage && !stage.imagePath.isEmpty()) {
        QPixmap pixmap(stage.imagePath);
        if (!pixmap.isNull()) {
            hasImage = true;
            QPixmap scaled = pixmap.scaled(280, 280,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation);
            m_imageLabel->setPixmap(scaled);
            m_imageLabel->setVisible(true);
        } else {
            hasImage = false;
            m_imageLabel->setVisible(false);
        }
    } else {
        hasImage = false;
        m_imageLabel->setVisible(false);
    }
    // Load mantra
    hasMantra = stage.showMantra && !stage.mantraText.isEmpty();
    if (hasMantra) {
        m_mantraLabel->setText(stage.mantraText);
        m_mantraLabel->setVisible(true);
        mantraLabelCpanel->setText("Mantra: " + stage.mantraText);
        mantraLabelCpanel->setVisible(true);
    } else {
        m_mantraLabel->setText("");
        m_mantraLabel->setVisible(false);
        mantraLabelCpanel->setVisible(false);
    }

    // Enable/disable counter based on mantra
    m_sessionSpinBox->setEnabled(true);
    m_addButton->setEnabled(true);

    // Load PDF or Text
    bool hasPdf = !stage.pdfPath.isEmpty();
    bool showPdf = m_stageShowPdfMode.value(index, hasPdf);  // Default: if has PDF, show it

    if (hasPdf && showPdf) {
        // Show PDF
        isPdf = true;
        m_currentlyShowingPdf = true;
        if (m_pdfView->document()) {
            QPdfDocument* oldDoc = m_pdfView->document();
            m_pdfView->setDocument(nullptr);
            oldDoc->disconnect();
            oldDoc->deleteLater();
        }

        QPdfDocument* doc = new QPdfDocument(this);
        doc->load(stage.pdfPath);
        m_pdfView->setDocument(doc);
        m_pdfView->setVisible(true);
        m_textArea->setVisible(false);
        m_viewPdfButton->setText("View Text");

    } else {
        // Show text
        isPdf = false;
        m_currentlyShowingPdf = false;

        if (m_pdfView->document()) {
            QPdfDocument* oldDoc = m_pdfView->document();
            m_pdfView->setDocument(nullptr);
            oldDoc->disconnect();
            oldDoc->deleteLater();
        }
        m_pdfView->setVisible(false);
        m_textArea->setVisible(true);
        m_textArea->setText(stage.liturgyText);
        //m_textArea->setAlignment(Qt::AlignCenter);
        m_viewPdfButton->setText("View PDF");
    }

    // ========== AUDIO  ==========
    bool hasAudio = !stage.audioPath.isEmpty();

    if (hasAudio) {
        m_nextAudioUrl = QUrl::fromLocalFile(stage.audioPath);  // ← This line is key
        QFileInfo audioInfo(stage.audioPath);
        m_currentAudioLabel->setText("Audio: " + audioInfo.fileName());
        m_currentAudioLabel->setStyleSheet("color: #ffd700; font-size: 11px;");
    } else {
        m_nextAudioUrl = QUrl();  // Clear it if no audio
        m_currentAudioLabel->setText("No audio assigned");
        m_currentAudioLabel->setStyleSheet("color: #888; font-size: 11px;");
    }
    // Update counter display for this stage
    if (m_isCustomModule) {
        m_totalCountLabel->setText(QString::number(stage.lifetimeCount));
    }



    // Update image label
    if (!stage.imagePath.isEmpty()) {
        QFileInfo imageInfo(stage.imagePath);
        m_currentImageLabel->setText("Image: " + imageInfo.fileName());
        m_currentImageLabel->setStyleSheet("color: #ffd700; font-size: 11px;");
    } else {
        m_currentImageLabel->setText("No image assigned");
        m_currentImageLabel->setStyleSheet("color: #888; font-size: 11px;");
    }

    // Update PDF label
    if (!stage.pdfPath.isEmpty()) {
        QFileInfo pdfInfo(stage.pdfPath);
        m_currentPdfLabel->setText("PDF: " + pdfInfo.fileName());
        m_currentPdfLabel->setStyleSheet("color: #ffd700; font-size: 11px;");
    } else {
        m_currentPdfLabel->setText("No PDF assigned");
        m_currentPdfLabel->setStyleSheet("color: #888; font-size: 11px;");
    }

    // Load into editors for custom modules
    if (m_isCustomModule) {
        m_liturgyEdit->blockSignals(true);
        m_mantraEdit->blockSignals(true);
        m_stageNameEdit->blockSignals(true);

        m_liturgyEdit->setText(stage.liturgyText);
        m_mantraEdit->setText(stage.mantraText);
        m_stageNameEdit->setText(stage.name);

        m_liturgyEdit->blockSignals(false);
        m_mantraEdit->blockSignals(false);
        m_stageNameEdit->blockSignals(false);
    }
    m_loadingStage = false;
}

void PujaView::onLiturgyTextChanged()
{
    if (!m_isCustomModule) return;
    if (m_currentStageIndex >= 0 && m_currentStageIndex < m_currentStages.size()) {
        m_currentStages[m_currentStageIndex].liturgyText = m_liturgyEdit->toPlainText();
        if (m_currentCustomModule) {
            m_currentCustomModule->setStages(m_currentStages);
            ModuleManager::instance().saveCustomModule(m_currentCustomModule);
        }
        // Update display if not in PDF mode
            m_textArea->setText(m_currentStages[m_currentStageIndex].liturgyText);
            //m_textArea->setAlignment(Qt::AlignCenter);
        }
    }

void PujaView::onMantraTextChanged()
{
    if (!m_isCustomModule) return;
    if (m_currentStageIndex >= 0 && m_currentStageIndex < m_currentStages.size()) {
        m_currentStages[m_currentStageIndex].mantraText = m_mantraEdit->toPlainText();
        if (m_currentCustomModule) {
            m_currentCustomModule->setStages(m_currentStages);
            ModuleManager::instance().saveCustomModule(m_currentCustomModule);
            hasMantra = true;
        }
        // Update display
        if (m_currentStages[m_currentStageIndex].showMantra) {
            m_mantraLabel->setText(m_currentStages[m_currentStageIndex].mantraText);
        }
    }
}


void PujaView::onAssignPdf()
{
    if (m_isCustomModule && m_currentCustomModule) {
        // Custom module - save to current stage
        QString filePath = QFileDialog::getOpenFileName(this,
            "Select PDF for Stage: " + m_currentStages[m_currentStageIndex].name,
            Constants::filesPath,
            "PDF Files (*.pdf)");

        if (!filePath.isEmpty()) {
            m_currentStages[m_currentStageIndex].pdfPath = filePath;
            m_currentCustomModule->setStages(m_currentStages);
            ModuleManager::instance().saveCustomModule(m_currentCustomModule);

            // Update PDF label immediately
            QFileInfo pdfInfo(filePath);
            m_currentPdfLabel->setText("PDF: " + pdfInfo.fileName());
            m_currentPdfLabel->setStyleSheet("color: #ffd700; font-size: 11px;");
            //m_preferPdfCheckbox->setChecked(true);
            // If this stage is currently active and PDF mode is on, reload
            loadStage(m_currentStageIndex);

        }
    } else if (m_currentModule) {
        return;
        // Default module - save globally
        QString filePath = QFileDialog::getOpenFileName(this,
            "Select PDF for " + m_currentModule->name(),
            Constants::filesPath,
            "PDF Files (*.pdf)");

        if (!filePath.isEmpty()) {
            ModuleManager::instance().savePdfPath(m_currentModule->id(), filePath);
            m_currentModule->setPdfPath(filePath);

            // Update PDF label immediately
            QFileInfo pdfInfo(filePath);
            m_currentPdfLabel->setText("PDF: " + pdfInfo.fileName());
            m_currentPdfLabel->setStyleSheet("color: #ffd700; font-size: 11px;");

        }
    }
}



void PujaView::onAssignAudio()
{
    if (m_isCustomModule && m_currentCustomModule) {
        // Custom module - save to current stage

        QString filePath = QFileDialog::getOpenFileName(this,
            "Select Audio for Stage: " + m_currentStages[m_currentStageIndex].name,
            Constants::audioPath,
            "Audio Files (*.mp3 *.wav *.flac *.m4a *.ogg *.webm *.aac *.wma *.opus);;All Files (*.*)"
            );


        if (!filePath.isEmpty()) {
            m_currentStages[m_currentStageIndex].audioPath = filePath;
            m_currentCustomModule->setStages(m_currentStages);
            ModuleManager::instance().saveCustomModule(m_currentCustomModule);

            m_nextAudioUrl = QUrl::fromLocalFile(filePath);

            QFileInfo audioInfo(filePath);
            m_currentAudioLabel->setText("Audio: " + audioInfo.fileName());
            m_currentAudioLabel->setStyleSheet("color: #ffd700; font-size: 11px;");

            // Enable audio controls
            m_playPauseButton->setEnabled(true);
            m_stopButton->setEnabled(true);
        }
    } else if (m_currentModule) {
        // Default module - save globally
        QString filePath = QFileDialog::getOpenFileName(this,
            "Select Audio for " + m_currentModule->name(),
            Constants::audioPath,
            "Audio Files (*.mp3 *.wav *.flac *.m4a *.ogg *.webm *.aac *.wma *.opus);;All Files (*.*)");

        if (!filePath.isEmpty()) {
            ModuleManager::instance().saveAudioPath(m_currentModule->id(), filePath);
            m_currentModule->setAudioPath(filePath);
            m_nextAudioUrl = QUrl::fromLocalFile(filePath);
            QFileInfo audioInfo(filePath);
            m_currentAudioLabel->setText("Audio: " + audioInfo.fileName());
            m_currentAudioLabel->setStyleSheet("color: #ffd700; font-size: 11px;");

            m_playPauseButton->setEnabled(true);
            m_stopButton->setEnabled(true);
        }
    }
}

/*
void PujaView::onPlayPause()
{
    QString audioPath;

    if (m_isCustomModule && m_currentCustomModule) {
        // Get audio from current stage
        audioPath = m_currentStages[m_currentStageIndex].audioPath;
    } else if (m_currentModule) {
        audioPath = m_currentModule->audioPath();
    }

    if (audioPath.isEmpty()) return;
    if (m_isPlaying) {
        m_mediaPlayer->pause();
    } else {
        if (m_mediaPlayer->mediaStatus() == QMediaPlayer::NoMedia ||
            m_mediaPlayer->position() == m_mediaPlayer->duration()) {
            m_mediaPlayer->setSource(QUrl::fromLocalFile(audioPath));
        }
        m_mediaPlayer->play();
    }
}
*/

void PujaView::onPlayPause()
{
    if (m_nextAudioUrl.isEmpty()) {
        QMessageBox::information(this, "Play Audio", "No audio assigned to this stage.");
        return;
    }

    if (m_isPlaying) {
        m_mediaPlayer->pause();
        m_isPlaying = false;
        m_playPauseButton->setText("▶");
    } else {
        // Always load from m_nextAudioUrl when playing
        m_mediaPlayer->setSource(m_nextAudioUrl);
        m_mediaPlayer->play();
        m_isPlaying = true;
        m_playPauseButton->setText("||");
    }
}

void PujaView::onAddSession()
{
    int sessionCount = m_sessionSpinBox->value();
    if (sessionCount <= 0) {
        QMessageBox::information(this, "Add Session", "Please enter a number of repetitions for this session.");
        return;
    }

    QString moduleName;
    QString stageName;

    if (m_isCustomModule && m_currentCustomModule) {
        moduleName = m_currentCustomModule->name();
        stageName = m_currentStages[m_currentStageIndex].name;

        m_currentStages[m_currentStageIndex].lifetimeCount += sessionCount;
        m_currentCustomModule->setStages(m_currentStages);
        ModuleManager::instance().saveCustomModule(m_currentCustomModule);
        updateCounterDisplay();
    } else if (m_currentModule) {
        moduleName = m_currentModule->name();
        stageName = "";

        m_currentModule->addToLifetimeCount(sessionCount);
        ModuleManager::instance().saveUserData();
        updateCounterDisplay();
    }

    // Save to journal with empty notes
    JournalManager::instance().addEntry(moduleName, stageName, sessionCount, "");

    m_sessionSpinBox->setValue(0);

    QMessageBox::information(this, "Session Added",
            QString("Added %1 repetitions.\nNew total: %2")
            .arg(sessionCount)
            .arg(m_isCustomModule ?
                 m_currentStages[m_currentStageIndex].lifetimeCount :
                 m_currentModule->lifetimeCount()));
}

void PujaView::onOpenJournal()
{

    // Get current module name
    QString currentModuleName = getCurrentModuleName(); // You need to implement this

    // Set the module name in journal dialog before showing
    journalDialog->setCurrentModuleName(currentModuleName);
    journalDialog->refreshForDate(QDate::currentDate());
    journalDialog->show();
}

void PujaView::addNewStage()
{
    if (!m_isCustomModule || !m_currentCustomModule) return;

    // Create new stage
    Stage newStage;
    newStage.name = "Stage " + QString::number(m_currentStages.size() + 1);
    newStage.showImage = true;
    newStage.showMantra = true;

    // Add to current stages
    m_currentStages.append(newStage);
    m_currentCustomModule->setStages(m_currentStages);

    // Save to disk
    ModuleManager::instance().saveCustomModule(m_currentCustomModule);
    hasMantra = false;
    hasImage = false;
    // Rebuild dots
    rebuildStageDots();

    // Load the new stage
    loadStage(m_currentStages.size() - 1);
    onToggleControlPanel();
}

void PujaView::updateCounterDisplay()
{
    if (m_isCustomModule && m_currentCustomModule) {
        if (m_currentStageIndex >= 0 && m_currentStageIndex < m_currentStages.size()) {
            m_totalCountLabel->setText(QString::number(m_currentStages[m_currentStageIndex].lifetimeCount));
        }
    } else if (m_currentModule) {
        m_totalCountLabel->setText(QString::number(m_currentModule->lifetimeCount()));
    }
}


void PujaView::onLoadLiturgyFromFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        "Load Liturgy Text File",
        Constants::filesPath,
        "Text Files (*.txt *.utf8)");

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
            QString content = stream.readAll();
            m_liturgyEdit->setText(content);
            file.close();
        } else {
            QMessageBox::warning(this, "Error", "Could not open file.");
        }
    }
}

void PujaView::loadModule(DeityModule* module)
{
    if (!module) return;

    // Set module first
    m_currentModule = module;
    m_isCustomModule = false;
    m_currentCustomModule = nullptr;
    // Load stages from module (default modules now have stages)
    m_currentStages = module->stages();
    m_currentStageIndex = 0;

    // Rebuild UI
    rebuildStageDots();  // Call ONCE after stages are set
    loadStage(0);

    // Disable editing controls for default modules
    enableEditing(false);
    updateImageDisplay();

    // Update image label
    if (!module->effectiveImagePath().isEmpty()) {
        QFileInfo imageInfo(module->effectiveImagePath());
        m_currentImageLabel->setText("Image: " + imageInfo.fileName());
        m_currentImageLabel->setStyleSheet("color: #ffd700; font-size: 11px;");
    } else {
        m_currentImageLabel->setText("No image assigned");
        m_currentImageLabel->setStyleSheet("color: #888; font-size: 11px;");
    }

    // Update mantra label
    m_mantraLabel->setText(module->mantraText());
    mantraLabelCpanel->setText("Mantra: " + module->mantraText());
    updateCounterDisplay();

    // Update audio
    bool hasAudio = !module->audioPath().isEmpty();
    m_playPauseButton->setEnabled(hasAudio);
    m_stopButton->setEnabled(hasAudio);
    m_playPauseButton->setText("▶");
    m_isPlaying = false;

    if (!module->audioPath().isEmpty()) {
        QFileInfo audioInfo(module->audioPath());
        m_currentAudioLabel->setText("Audio: " + audioInfo.fileName());
        m_currentAudioLabel->setStyleSheet("color: #ffd700; font-size: 11px;");
    } else {
        m_currentAudioLabel->setText("No audio assigned");
        m_currentAudioLabel->setStyleSheet("color: #888; font-size: 11px;");
    }

    onSectionClicked(0);
    //resetCpanel();
}


void PujaView::loadCustomModule(CustomModule* module)
{
    if (!module) return;

    // Reset default module
    m_currentModule = nullptr;
    m_currentCustomModule = module;
    m_isCustomModule = true;

    // Reset PDF state
    m_currentlyShowingPdf = false;
    isPdf = false;

    // Clean up old PDF document
    if (m_pdfView->document()) {
        QPdfDocument* oldDoc = m_pdfView->document();
        m_pdfView->setDocument(nullptr);
        oldDoc->disconnect();
        oldDoc->deleteLater();
    }
    m_pdfView->setVisible(false);
    m_textArea->setVisible(true);

    // Enable editing controls for custom modules
    //enableEditing(true);

    // Load stages from custom module
    m_currentStages = module->stages();
    m_currentStageIndex = 0;

    // Rebuild progress dots
    rebuildStageDots();

    // Load first stage
    loadStage(0);

    // Update counter
    updateCounterDisplay();
    enableEditing(true);
}

void PujaView::resetCpanel(){

    m_panelVisible = false;
    m_controlPanel->setVisible(false);
    m_togglePanelButton->setText("▼ Show Controls");
    m_imageLabel->setVisible(true);
    m_mantraLabel->setVisible(!m_mantraLabel->text().isEmpty());
    m_textArea->setVisible(true);
    m_pdfView->setVisible(false);
}

void PujaView::onResetCounter()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset Counter",
        "Are you sure you want to reset the counter for this stage?\n\n"
        "This will set the lifetime count to 0.",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (m_isCustomModule && m_currentCustomModule) {
            if (m_currentStageIndex >= 0 && m_currentStageIndex < m_currentStages.size()) {
                m_currentStages[m_currentStageIndex].lifetimeCount = 0;
                m_currentCustomModule->setStages(m_currentStages);
                ModuleManager::instance().saveCustomModule(m_currentCustomModule);
                updateCounterDisplay();
            }
        } else if (m_currentModule) {
            m_currentModule->setLifetimeCount(0);
            ModuleManager::instance().saveUserData();
            updateCounterDisplay();
        }

        QMessageBox::information(this, "Counter Reset", "The counter has been reset to 0.");
    }
}

void PujaView::onStageNameChanged()
{
    if (!m_isCustomModule || m_currentStageIndex < 0) return;

    QString newName = m_stageNameEdit->text();
    if (m_currentStages[m_currentStageIndex].name == newName) return;

    m_currentStages[m_currentStageIndex].name = newName;
    m_currentCustomModule->setStages(m_currentStages);
    ModuleManager::instance().saveCustomModule(m_currentCustomModule);
    rebuildStageDots();  // Update tooltips on dots
}


QString PujaView::getCurrentModuleName()
{
    if (m_isCustomModule && m_currentCustomModule) {
        return m_currentCustomModule->name();
    } else if (m_currentModule) {
        return m_currentModule->name();
    }
    return "General";
}
