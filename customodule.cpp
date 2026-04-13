#include "custommodule.h"
#include <QUuid>

CustomModule::CustomModule()
    : m_lifetimeCount(0)

{
}

CustomModule::CustomModule(const QString& id, const QString& name)
    : m_id(id)
    , m_name(name)
    , m_lifetimeCount(0)
{
}

void CustomModule::addStage(const Stage& stage)
{
    m_stages.append(stage);
}

void CustomModule::insertStage(int index, const Stage& stage)
{
    m_stages.insert(index, stage);
}

void CustomModule::removeStage(int index)
{
    if (index >= 0 && index < m_stages.size()) {
        m_stages.removeAt(index);
    }
}

void CustomModule::renameStage(int index, const QString& name)
{
    if (index >= 0 && index < m_stages.size()) {
        m_stages[index].name = name;
    }
}

QString CustomModule::effectiveImagePath() const
{
    if (!m_userImagePath.isEmpty()) {
        return m_userImagePath;
    }
    return m_defaultImagePath;
}

void CustomModule::setDefaultImagePath(const QString &imagePath)
{
    m_defaultImagePath = imagePath;
}
