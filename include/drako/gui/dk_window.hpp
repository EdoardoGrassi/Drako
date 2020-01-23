#ifndef DRAKO_GUI_WINDOW_HPP
#define DRAKO_GUI_WINDOW_HPP

#include <string>

namespace drako::gui
{
    // CLASS
    // Top-level container that wraps a native OS window.
    class Window
    {
    public:

        explicit Window();
        explicit Window(std::string title);

    private:

        std::string _name;
    };

} // namespace drako::gui

#endif // !DRAKO_GUI_WINDOW_HPP

