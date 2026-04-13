#ifndef CUSTOMMODULE_H
#define CUSTOMMODULE_H

#include <QString>
#include <QVector>
#include "stage.h"
#include"deitymodule.h"

class CustomModule : public DeityModule
{
public:
    CustomModule();
    CustomModule(const QString& id, const QString& name);
    
    // Getters
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QVector<Stage> stages() const { return m_stages; }

    QString defaultImagePath() const { return m_defaultImagePath; }
    QString userImagePath() const { return m_userImagePath; }
    QString audioPath() const { return m_audioPath; }
    int lifetimeCount() const { return m_lifetimeCount; }
    bool isEditable() const { return true; }
    
    // Setters
    void setName(const QString& name) { m_name = name; }
    void setStages(const QVector<Stage>& stages) { m_stages = stages; }
    void setUserImagePath(const QString& path) { m_userImagePath = path; }
    void setAudioPath(const QString& path) { m_audioPath = path; }
    void setLifetimeCount(int count) { m_lifetimeCount = count; }
    void addToLifetimeCount(int count) { m_lifetimeCount += count; }
    
    // Stage management
    void addStage(const Stage& stage);
    void insertStage(int index, const Stage& stage);
    void removeStage(int index);
    void renameStage(int index, const QString& name);
    
    QString effectiveImagePath() const;
    
private:
    QString m_id;
    QString m_name;
    QVector<Stage> m_stages;
    QString m_defaultImagePath;
    QString m_userImagePath;
    QString m_audioPath;
    int m_lifetimeCount;
public:
    void setDefaultImagePath(const QString& imagePath);
};

#endif // CUSTOMMODULE_H
