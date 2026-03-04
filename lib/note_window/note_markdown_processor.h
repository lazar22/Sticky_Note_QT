#ifndef NOTE_MARKDOWN_PROCESSOR_H
#define NOTE_MARKDOWN_PROCESSOR_H

#include <QString>

namespace sticky_note
{
    class NoteMarkdownProcessor
    {
    public:
        /**
         * @brief Function to transfer edited md text to regular Label text
         *
         * @param md format of the text you want to adapt
         * @return QString formatted HTML for view
         */
        static QString to_view_markdown(const QString& md);

        /**
         * @brief Function to transfer from the regular Label text to edited md text
         *
         * @param html regular text to adapt to md text
         * @return QString original markdown text
         */
        static QString from_view_markdown(const QString& html);
    };
}

#endif //NOTE_MARKDOWN_PROCESSOR_H
