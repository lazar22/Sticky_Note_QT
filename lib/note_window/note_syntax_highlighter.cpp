#include "note_syntax_highlighter.h"
#include "shared.h"
#include <QRegularExpression>
#include <QStringList>
#include <QList>
#include <algorithm>

QString sticky_note::NoteSyntaxHighlighter::highlight_code(const QString& code, const QString& lang)
{
    QString highlighted = code;

    struct Rule
    {
        QRegularExpression pattern;
        QString color;
    };

    QList<Rule> rules;

    if (lang == "C++" || lang == "C" || lang == "Java")
    {
        // Comments`
        rules.append({QRegularExpression("//.*"), note_styles::COLOR_COMMENT});
        rules.append({
            QRegularExpression("/\\*.*?\\*/", QRegularExpression::DotMatchesEverythingOption),
            note_styles::COLOR_COMMENT
        });

        // Strings
        rules.append({QRegularExpression("\".*?\""), note_styles::COLOR_STRING});

        // Preprocessor
        rules.append({QRegularExpression("^\\s*#\\w+"), note_styles::COLOR_PREPROCESSOR});

        // std:: types and members
        rules.append({QRegularExpression("std::\\w+"), note_styles::COLOR_STD_NAMESPACE});

        // Keywords
        QStringList keywords = {
            "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "auto",
            "bitand", "bitor", "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t", "class",
            "compl", "concept", "const", "consteval", "constexpr", "constinit", "const_cast", "continue", "co_await",
            "co_return", "co_yield", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum",
            "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long",
            "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
            "protected", "public", "reflexpr", "register", "reinterpret_cast", "requires", "return", "short", "signed",
            "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized", "template", "this",
            "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
            "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq", "final", "override", "import", "module",
            // Java specific
            "abstract", "assert", "boolean", "byte", "extends", "finally", "implements", "instanceof", "interface",
            "native", "package", "strictfp", "super", "throws", "transient"
        };
        rules.append({QRegularExpression("\\b(" + keywords.join("|") + ")\\b"), note_styles::COLOR_KEYWORD});

        // Numeric literals
        rules.append({QRegularExpression("\\b\\d+(\\.\\d+)?([eE][+-]?\\d+)?[fFdD]?\\b"), note_styles::COLOR_NUMBER});
    }
    else if (lang == "Python")
    {
        // Comments
        rules.append({QRegularExpression("#.*"), note_styles::COLOR_COMMENT});

        // Strings
        rules.append({QRegularExpression("(['\"])(?:\\\\.|[^\\\\])*?\\1"), note_styles::COLOR_STRING});
        rules.append({
            QRegularExpression("('''|\"\"\")(.*?)\\1", QRegularExpression::DotMatchesEverythingOption),
            note_styles::COLOR_STRING
        });

        // Keywords
        QStringList keywords = {
            "False", "None", "True", "and", "as", "assert", "async", "await", "break", "class", "continue", "def",
            "del",
            "elif", "else", "except", "finally", "for", "from", "global", "if", "import", "in", "is", "lambda",
            "nonlocal", "not", "or", "pass", "raise", "return", "try", "while", "with", "yield"
        };
        rules.append({QRegularExpression("\\b(" + keywords.join("|") + ")\\b"), note_styles::COLOR_KEYWORD});

        // Numeric literals
        rules.append({QRegularExpression("\\b\\d+(\\.\\d+)?([eE][+-]?\\d+)?[jJ]?\\b"), note_styles::COLOR_NUMBER});

        // Decorators
        rules.append({QRegularExpression("@\\w+"), note_styles::COLOR_PYTHON_DECORATOR});
    }

    // Apply rules
    struct MatchEntry
    {
        int start;
        int length;
        QString color;
    };
    QList<MatchEntry> matchEntries;

    for (const auto& rule : rules)
    {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(highlighted);
        while (it.hasNext())
        {
            QRegularExpressionMatch m = it.next();
            // Basic conflict resolution: only add if not overlapping with existing matches
            bool overlap = false;
            for (const auto& existing : matchEntries)
            {
                if ((static_cast<int>(m.capturedStart()) >= existing.start && static_cast<int>(m.capturedStart()) <
                        existing.start + existing.length) ||
                    (existing.start >= static_cast<int>(m.capturedStart()) && existing.start < static_cast<int>(m.
                        capturedStart()) + static_cast<int>(m.capturedLength())))
                {
                    overlap = true;
                    break;
                }
            }
            if (!overlap)
            {
                matchEntries.append({
                    static_cast<int>(m.capturedStart()), static_cast<int>(m.capturedLength()), rule.color
                });
            }
        }
    }

    // Sort matches by start position in descending order to replace from the end
    std::sort(matchEntries.begin(), matchEntries.end(), [](const MatchEntry& a, const MatchEntry& b)
    {
        return a.start > b.start;
    });

    for (const auto& m : matchEntries)
    {
        QString snippet = highlighted.mid(m.start, m.length);
        highlighted.replace(m.start, m.length, QString("<span style=\"color: %1;\">%2</span>").arg(m.color, snippet));
    }

    return highlighted;
}
