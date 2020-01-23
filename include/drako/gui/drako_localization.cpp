#include "drako_localization.hpp"

namespace drako::gui
{
    DkLocalizationManager::DkLocalizationManager(std::string assetFile)
    {
        // TODO
    }

    std::string DkLocalizationManager::get_localized_text(DkLocalizationID key) const
    {
        // TODO
        return m_database[key];
    }
}
