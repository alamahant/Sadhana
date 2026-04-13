#include "deitymodule.h"

DeityModule::DeityModule()
    : m_lifetimeCount(0)
{
}

DeityModule::DeityModule(const QString& id,
                         const QString& name,
                         const QString& mantraText,
                         const QString& openingText,
                         const QString& closingText,
                         const QString& defaultImagePath)
    : m_id(id)
    , m_name(name)
    , m_mantraText(mantraText)
    , m_openingText(openingText)
    , m_closingText(closingText)
    , m_defaultImagePath(defaultImagePath)
    , m_lifetimeCount(0)
    , m_deityInfo("")
{
}

QString DeityModule::effectiveImagePath() const
{
    if (!m_userImagePath.isEmpty())
        return m_userImagePath;
    return m_defaultImagePath;
}

