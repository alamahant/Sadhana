#ifndef DEITYMODULE_H
#define DEITYMODULE_H

#include <QString>
#include <QMap>
#include"stage.h"

class DeityModule
{
private:
    QString m_pdfPath;
    QString m_deityInfo;
public:
    DeityModule();
    DeityModule(const QString& id,
                const QString& name,
                const QString& mantraText,
                const QString& openingText,
                const QString& closingText,
                const QString& defaultImagePath);


    // Getters
    virtual QString name() const { return m_name; }

    virtual QString id() const { return m_id; }
    QString mantraText() const { return m_mantraText; }
    QString openingText() const { return m_openingText; }
    QString closingText() const { return m_closingText; }
    QString defaultImagePath() const { return m_defaultImagePath; }
    QString userImagePath() const { return m_userImagePath; }
    QString audioPath() const { return m_audioPath; }
    int lifetimeCount() const { return m_lifetimeCount; }
    QString deityInfo() const { return m_deityInfo; }
    // Setters
    void setUserImagePath(const QString& path) { m_userImagePath = path; }
    void setAudioPath(const QString& path) { m_audioPath = path; }
    void setLifetimeCount(int count) { m_lifetimeCount = count; }
    void addToLifetimeCount(int count) { m_lifetimeCount += count; }
    void setDeityInfo(const QString& info) { m_deityInfo = info; }
    // Get effective image path (user override if exists, else default)
    virtual  QString effectiveImagePath() const;
    QString pdfPath() const { return m_pdfPath; }
    void setPdfPath(const QString& path) { m_pdfPath = path; }

private:
    QString m_id;                 // Unique identifier, e.g., "chenrezig"
    QString m_name;               // Display name, e.g., "Chenrezig"
    QString m_mantraText;         // The mantra to display
    QString m_openingText;        // Opening prayers/invocation
    QString m_closingText;        // Dedication and auspicious wishes
    QString m_defaultImagePath;   // Bundled default image
    QString m_userImagePath;      // User's custom image (empty if not set)
    QString m_audioPath;          // User's assigned audio file (empty if not set)
    int m_lifetimeCount;          // Total accumulated repetitions


private:
    QVector<Stage> m_stages;

public:
    void setStages(const QVector<Stage>& stages) { m_stages = stages; }
    QVector<Stage> stages() const { return m_stages; }
};

#endif // DEITYMODULE_H
