//
// Created by roki on 2026-02-16.
//

#ifndef SHARED_H
#define SHARED_H

#include <string>
#include <QFont>

namespace sticky_note
{
    namespace main_window
    {
        constexpr int WIDTH = 300;
        constexpr int HEIGHT = 300;
        constexpr const char* TITLE = "Main Window";
    }

    namespace note_window
    {
        constexpr int WIDTH = 300;
        constexpr int HEIGHT = 300;
        constexpr int MIN_WIDTH = 200;
        constexpr int MIN_HEIGHT = 200;
        constexpr const char* TITLE = "Default Note Title";
    }

    namespace note_fonts
    {
        const QFont TITLE_FONT("Arial", 22, QFont::Bold);
        const QFont REGULAR_FONT("Arial", 16, QFont::Bold);
    }
}

#endif //SHARED_H
