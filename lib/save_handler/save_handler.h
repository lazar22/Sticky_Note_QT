//
// Created by roki on 2026-02-17.
//

#ifndef SAVE_HANDLER_H
#define SAVE_HANDLER_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QFile>
#include <QUuid>
#include <QPoint>
#include <QSize>
#include <QColor>
#include <QStandardPaths>

namespace sticky_note
{
    struct NoteData
    {
        QUuid id;
        QPoint pos;
        QSize size;
        QColor color;
        QString title;
        QString text;
        bool is_pinned = false;
    };

    class SaveHandler
    {
    public:
        static void save_to_json(const NoteData& data);

        static void delete_json(const QUuid& id);

        static QVector<NoteData> load_notes();

    private:
        static QString get_save_path();
    };
}

#endif //SAVE_HANDLER_H