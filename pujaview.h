#ifndef PUJAVIEW_H
#define PUJAVIEW_H

#include <QWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QMediaPlayer>
#include <QSpinBox>
#include <QSlider>
#include "deitymodule.h"
#include<QAudioOutput>
#include<QPdfView>
#include<QKeyEvent>
#include"custommodule.h"
#include<QCheckBox>
#include<QLineEdit>
#include"journaldialog.h"
#include<QMap>
#include<QUrl>

class PujaView : public QWidget
{
    Q_OBJECT

public:
    explicit PujaView(QWidget *parent = nullptr);
    ~PujaView();

    void loadModule(DeityModule* module);
protected:
        void keyPressEvent(QKeyEvent* event) override;
signals:
    void backToGrid();

private slots:
    void onSectionClicked(int sectionIndex);
    void onAssignAudio();
    void onPlayPause();
    void onStop();
    void onAssignImage();
    void onAddSession();
    void onToggleControlPanel();
    void onSeek(int position);
    void onVolumeChanged(int volume);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);

    // PDF slots
    void onAssignPdf();

    // Journal slot
    void onOpenJournal();

private:
    void setupUI();
    void setupControlPanel();
    void updateTextArea();
    void updateImageDisplay();
    void updateCounterDisplay();
    void updateAudioButtonState();
    void saveModuleData();

    // ========== UI Components - Main Area ==========
    QLabel* m_imageLabel;
    QLabel* m_mantraLabel;
    QTextBrowser* m_textArea;
    QWidget* m_dotsContainer;
    QVector<QPushButton*> m_dots;
    QPushButton* m_backButton;

    // ========== Control Panel ==========
    QPushButton* m_togglePanelButton;
    QWidget* m_controlPanel;
    bool m_panelVisible;

    // ----- Audio Controls -----
    QPushButton* m_assignAudioButton;
    QPushButton* m_playPauseButton;   // Play/Pause button (▶/⏸)
    QPushButton* m_stopButton;        // Stop button (⏹)
    QPushButton* m_prevButton;        // Previous button (⏮)
    QPushButton* m_nextButton;        // Next button (⏭)
    QSlider* m_progressSlider;
    QSlider* m_volumeSlider;
    QMediaPlayer* m_mediaPlayer;
    QAudioOutput* m_audioOutput;

    bool m_isPlaying;

    // ----- Image Control -----
    QPushButton* m_assignImageButton;

    // ----- PDF Controls -----
    QPushButton* m_assignPdfButton;
    QPushButton* m_viewPdfButton;

    // ----- Counter Controls -----
    QLabel* m_totalCountLabel;
    QSpinBox* m_sessionSpinBox;
    QPushButton* m_addButton;

    // ----- Journal Control -----
    QPushButton* m_openJournalButton;

    // ========== Data ==========
    DeityModule* m_currentModule;
    int m_currentSection; // 0 = Opening, 1 = Mantra, 2 = Closing
private:
    QPdfView* m_pdfView = nullptr;
    bool isPdf = false;
    bool m_fullscreenMode = false;
    QWidget* m_fullscreenWidget = nullptr;
    CustomModule* m_currentCustomModule = nullptr;
    bool m_isCustomModule = false;
    QVector<Stage> m_currentStages;
    int m_currentStageIndex = 0;
    void enableEditing(bool enabled);
    void rebuildStageDots();
    void loadStage(int index);

public:
    void loadCustomModule(CustomModule* module);
private:
    QTextEdit* m_liturgyEdit;
    QTextEdit* m_mantraEdit;
    QPushButton* m_addStageButton;
    QLabel* mantraLabelCpanel;
    QPushButton* loadTextButton;
    QLabel *m_currentAudioLabel;
    QLabel *m_currentPdfLabel;
    void resetCpanel();
    QPushButton* m_resetCounterButton;
    void onResetCounter();
    QLabel* m_currentImageLabel;
    QPushButton* m_repeatButton;
    QLineEdit* m_stageNameEdit;
    bool m_loadingStage = false;
    bool m_currentlyShowingPdf = false;
    //QCheckBox* m_preferPdfCheckbox;
    JournalDialog* journalDialog = nullptr;
    QMap<int, bool> m_stageShowPdfMode;
private slots:
    void onLiturgyTextChanged();
    void onMantraTextChanged();
    void addNewStage();
    void onLoadLiturgyFromFile();
    void onStageNameChanged();
private:
    QLabel* stageNameLabel;    // "Stage Name:" label
    QLabel* liturgyLabel;
    QLabel* mantraLabel;
    QLabel* pdfLabel;
    QString getCurrentModuleName();
    QUrl m_nextAudioUrl = QUrl();
    QPushButton* m_infoButton;
};

#endif // PUJAVIEW_H
