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

        inline QIcon DOWNLOAD_ICON() { return QIcon(":/icons/download.png"); }
    }

    namespace note_shortcuts
    {
        const QKeySequence EDIT_NOTE_SHORTCUT = QKeySequence("Ctrl+E");
        const QKeySequence QUIT_NOTE_SHORTCUT = QKeySequence("Ctrl+Q");
        const QKeySequence COLOR_NOTE_SHORTCUT = QKeySequence("Ctrl+C");
        const QKeySequence PIN_NOTE_SHORTCUT = QKeySequence("Ctrl+P");
        const QKeySequence SAVE_NOTE_SHORTCUT = QKeySequence("Ctrl+S");
        const QKeySequence DOWNLOAD_NOTE_SHORTCUT = QKeySequence("Ctrl+D");
    }

    namespace note_styles
    {
        // Code Block Styling
        constexpr const char* CODE_BLOCK_BG = "#2b2d31";
        constexpr const char* CODE_BLOCK_TEXT = "#dbdee1";
        constexpr const char* CODE_BLOCK_BORDER = "#3f4147";
        constexpr const char* CODE_BLOCK_FONT_FAMILY = "'Courier New', monospace";

        // Syntax Highlighting Colors
        constexpr const char* COLOR_COMMENT = "#6a9955";
        constexpr const char* COLOR_STRING = "#ce9178";
        constexpr const char* COLOR_PREPROCESSOR = "#c586c0";
        constexpr const char* COLOR_STD_NAMESPACE = "#4ec9b0";
        constexpr const char* COLOR_KEYWORD = "#569cd6";
        constexpr const char* COLOR_NUMBER = "#b5cea8";
        constexpr const char* COLOR_PYTHON_DECORATOR = "#dcdcaa";

        // Note UI Colors
        constexpr const char* DEFAULT_NOTE_COLOR = "#fff6a8";
        const QVector<QColor> PICKABLE_COLORS = {
            "#ffadad", "#ffd6a5", "#fdffb6", "#caffbf",
            "#9bf6ff", "#a0c4ff", "#bdb2ff", "#ffc6ff",
            "#fffffc", "#f0f0f0", "#d9d9d9", "#bfbfbf",
            "#ff7eb9", "#7afcff", "#feff9c", "#ff9bbb"
        };
    }
}

#endif //SHARED_H
