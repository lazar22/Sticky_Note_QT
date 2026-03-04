#include "note_markdown_processor.h"
#include "note_syntax_highlighter.h"
#include "shared.h"
#include <QRegularExpression>
#include <QList>

QString sticky_note::NoteMarkdownProcessor::to_view_markdown(const QString& md)
{
    QString result = md;

    // Discord-like code blocks for C, C++, Java, and Python
    QRegularExpression codeRegex("```(?:C\\+\\+|C|Java|Python)\\n(.*?)\\n```",
                                 QRegularExpression::DotMatchesEverythingOption);

    // Find code blocks and store them, replacing with placeholders to protect them from further transformations
    QList<QString> codeBlocks;
    QRegularExpressionMatchIterator it = codeRegex.globalMatch(result);
    QList<QRegularExpressionMatch> matches;
    while (it.hasNext())
    {
        matches.append(it.next());
    }

    for (int i = matches.size() - 1; i >= 0; --i)
    {
        QRegularExpressionMatch match = matches.at(i);
        QString firstLine = match.captured(0).section('\n', 0, 0);
        QString lang = firstLine.mid(3).trimmed();
        QString code = match.captured(1);

        // Escape HTML in the code block
        code.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");

        QString highlighted = NoteSyntaxHighlighter::highlight_code(code, lang);

        QString styledCode = QString(
            "<div style=\"background-color: %1; color: %2; border-radius: 6px; padding: 16px 20px; margin: 6px 0; border: 1px solid %3; font-family: %4;\">"
            "<div style=\"font-size: 1pt; color: %1; line-height: 0; height: 0; overflow: hidden;\">%5</div>"
            "<pre style=\"margin: 0; padding: 0; white-space: pre-wrap;\">%6</pre>"
            "</div>"
        ).arg(note_styles::CODE_BLOCK_BG, note_styles::CODE_BLOCK_TEXT, note_styles::CODE_BLOCK_BORDER,
              note_styles::CODE_BLOCK_FONT_FAMILY, lang, highlighted);

        // Replace with placeholder
        QString placeholder = QString("@@@CODE_BLOCK_%1@@@").arg(i);
        codeBlocks.insert(0, styledCode); // Store in original order
        result.replace(match.capturedStart(), match.capturedLength(), placeholder);
    }

    // Escape Markdown headers
    result.replace(QRegularExpression("^(\\s*)(#+)", QRegularExpression::MultilineOption), "\\1\u200B\\2");

    // Render Markdown-like checkboxes
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)\\[ \\]", QRegularExpression::MultilineOption), "\\1☐");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)\\[\\]", QRegularExpression::MultilineOption), "\\1☐");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)\\[x\\]", QRegularExpression::MultilineOption), "\\1☑");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)\\[X\\]", QRegularExpression::MultilineOption), "\\1☒");

    // Add soft-breaks
    result.replace(QRegularExpression("(?<!  )\n"), "  \n");

    // Put code blocks back
    for (int i = 0; i < codeBlocks.size(); ++i)
    {
        QString placeholder = QString("@@@CODE_BLOCK_%1@@@").arg(i);
        result.replace(placeholder + "  \n", codeBlocks[i] + "\n");
        result.replace(placeholder, codeBlocks[i]);
    }

    return result;
}

QString sticky_note::NoteMarkdownProcessor::from_view_markdown(const QString& html)
{
    QString result = html;

    // Revert Discord-like code blocks
    // Note: We need to use the actual values from shared.h or build the regex dynamically
    QString pattern = QString(
        "<div style=\"background-color: %1; color: %2; border-radius: 6px; padding: 16px 20px; margin: 6px 0; border: 1px solid %3; font-family: %4;\">"
        "<div style=\"font-size: 1pt; color: %1; line-height: 0; height: 0; overflow: hidden;\">(.*?)</div>"
        "<pre style=\"margin: 0; padding: 0; white-space: pre-wrap;\">(.*?)</pre>"
        "</div>"
    ).arg(QRegularExpression::escape(note_styles::CODE_BLOCK_BG),
          QRegularExpression::escape(note_styles::CODE_BLOCK_TEXT),
          QRegularExpression::escape(note_styles::CODE_BLOCK_BORDER),
          QRegularExpression::escape(note_styles::CODE_BLOCK_FONT_FAMILY));

    QRegularExpression styledCodeRegex(pattern, QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator it = styledCodeRegex.globalMatch(result);
    QList<QRegularExpressionMatch> matches;
    while (it.hasNext())
    {
        matches.append(it.next());
    }

    for (int i = matches.size() - 1; i >= 0; --i)
    {
        QRegularExpressionMatch match = matches.at(i);
        QString lang = match.captured(1);
        QString code = match.captured(2);

        // Strip HTML span tags used for highlighting
        code.replace(QRegularExpression("<span style=\"color: #[0-9a-fA-F]{6};\">"), "");
        code.replace("</span>", "");

        // Unescape HTML in the code block
        code.replace("&lt;", "<").replace("&gt;", ">").replace("&amp;", "&");

        QString originalCode = QString("```%1\n%2\n```").arg(lang, code);
        result.replace(match.capturedStart(), match.capturedLength(), originalCode);
    }

    // Remove escaping for '#'
    result.replace(QRegularExpression("^(\\s*)\u200B(#+)", QRegularExpression::MultilineOption), "\\1\\2");

    // Convert checkboxes back
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)☐", QRegularExpression::MultilineOption), "\\1[ ]");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)☑", QRegularExpression::MultilineOption), "\\1[x]");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)☒", QRegularExpression::MultilineOption), "\\1[X]");

    // Revert soft-breaks
    result.replace("  \n", "\n");

    return result;
}