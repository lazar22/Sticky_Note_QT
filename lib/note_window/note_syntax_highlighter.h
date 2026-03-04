#ifndef NOTE_SYNTAX_HIGHLIGHTER_H
#define NOTE_SYNTAX_HIGHLIGHTER_H

#include <QString>

namespace sticky_note
{
    class NoteSyntaxHighlighter
    {
    public:
        /**
         * @brief Function to apply syntax highlighting to code blocks
         *
         * @param code the code to highlight
         * @param lang the language for highlighting
         */
        static QString highlight_code(const QString& code, const QString& lang);
    };
}

#endif //NOTE_SYNTAX_HIGHLIGHTER_H