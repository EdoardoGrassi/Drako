#ifndef _DRAKO_LOCALIZATION_HPP_
#define _DRACO_LOCALIZATION_HPP_

#include <cstdlib>
#include <unordered_map>

namespace drako::gui
{
    typedef std::size_t DkLocalizationID;

    class DkLocalizationManager
    {
    public:
        explicit DkLocalizationManager(std::string assetFile);
        ~DkLocalizationManager() = default;

        std::string get_localized_text(DkLocalizationID key) const;

    private:
        std::unordered_map<DkLocalizationID, std::string> m_database;
    };
}

#endif // !_DRAKO_LOCALIZATION_HPP_
