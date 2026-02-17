//
// Created by roki on 2026-02-17.
//

#include "save_handler.h"

namespace sticky_note
{
    void SaveHandler::save_to_json(const NoteData& data)
    {
        QJsonObject note_object;
        note_object["id"] = data.id.toString();
        note_object["x"] = data.pos.x();
        note_object["y"] = data.pos.y();
        note_object["color"] = data.color.name();
        note_object["title"] = data.title;
        note_object["text"] = data.text;

        const QJsonDocument doc(note_object);
        const QString path = QDir::current().absoluteFilePath("notes");
        QFile file(path + "/" + data.id.toString() + ".json");
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(doc.toJson());
            file.close();
        }
    }

    void SaveHandler::delete_json(const QUuid& id)
    {
        QString path = QDir::current().absoluteFilePath("notes");
        QFile file(path + "/" + id.toString() + ".json");
        if (file.exists())
        {
            file.remove();
        }
    }

    QVector<NoteData> SaveHandler::load_notes()
    {
        QVector<NoteData> notes;
        QString path = QDir::current().absoluteFilePath("notes");
        QDir notes_dir(path);
        if (!notes_dir.exists())
        {
            return notes;
        }

        QStringList filters;
        filters << "*.json";
        notes_dir.setNameFilters(filters);

        for (const QString& filename : notes_dir.entryList())
        {
            QFile file(notes_dir.filePath(filename));
            if (file.open(QIODevice::ReadOnly))
            {
                QByteArray data = file.readAll();
                file.close();

                QJsonDocument doc = QJsonDocument::fromJson(data);
                if (!doc.isNull() && doc.isObject())
                {
                    QJsonObject obj = doc.object();
                    NoteData note;
                    note.id = QUuid::fromString(obj["id"].toString());
                    note.pos = QPoint(obj["x"].toInt(), obj["y"].toInt());
                    note.color = QColor(obj["color"].toString());
                    note.title = obj["title"].toString();
                    note.text = obj["text"].toString();
                    notes.push_back(note);
                }
            }
        }
        return notes;
    }
}
