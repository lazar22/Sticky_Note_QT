//
// Created by roki on 2026-02-16.
//

#ifndef SHARED_H
#define SHARED_H

#include <QKeySequence>
#include <string>
#include <QFont>
#include <QIcon>

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

    namespace menage_window
    {
        constexpr int WIDTH = 400;
        constexpr int HEIGHT = 200;
        constexpr const char* TITLE = "Manage Notes";
    }

    namespace note_fonts
    {
        inline QFont TITLE_FONT() { return QFont("Arial", 22, QFont::Bold); }
        inline QFont REGULAR_FONT() { return QFont("Arial", 16, QFont::Bold); }
    }

    namespace note_icons
    {
        inline QIcon PEN_ICON() { return QIcon(":/icons/pen.png"); }
        inline QIcon PEN_ACTIVE_ICON() { return QIcon(":/icons/pen_active.png"); }

        inline QIcon EXIT_ICON() { return QIcon(":/icons/exit.png"); }
        inline QIcon WHEEL_ICON() { return QIcon(":/icons/wheel.png"); }

        inline QIcon PIN_ICON() { return QIcon(":/icons/pin.png"); }
        inline QIcon PIN_ACTIVE_ICON() { return QIcon(":/icons/pin_active.png"); }
    }

    namespace note_shortcuts
    {
        const QKeySequence EDIT_NOTE_SHORTCUT = QKeySequence("Ctrl+E");
        const QKeySequence QUIT_NOTE_SHORTCUT = QKeySequence("Ctrl+Q");
        const QKeySequence COLOR_NOTE_SHORTCUT = QKeySequence("Ctrl+C");
        const QKeySequence PIN_NOTE_SHORTCUT = QKeySequence("Ctrl+P");
        const QKeySequence SAVE_NOTE_SHORTCUT = QKeySequence("Ctrl+S");
    }
}

#endif //SHARED_H
