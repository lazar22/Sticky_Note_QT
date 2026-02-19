//
// Created by roki on 2026-02-19.
//

#include "menage_window.h"
#include "shared.h"
#include "save_handler/save_handler.h"
#include "note_window/note_window.h"
#include "note_action/note_action.h"
#include <QApplication>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace sticky_note
{
    MenageWindow::MenageWindow(QWidget* parent)
        : QWidget(parent)
    {
        setAcceptDrops(true);
        auto* layout = new QVBoxLayout(this);

        upload_btn = new QPushButton("Upload Notes", this);
        download_btn = new QPushButton("Download Notes", this);

        upload_btn->setFixedHeight(50);
        download_btn->setFixedHeight(50);

        layout->addWidget(upload_btn);
        layout->addWidget(download_btn);

        connect(upload_btn, &QPushButton::clicked, this, &MenageWindow::upload_notes);
        connect(download_btn, &QPushButton::clicked, this, &MenageWindow::download_notes);

        setStyleSheet("background: #fff6a8;");
        upload_btn->setStyleSheet("background: #ffffff;");
        download_btn->setStyleSheet("background: #ffffff;");
    }

    void MenageWindow::init(const int _w, const int _h, const std::string _title)
    {
        set_title(_title);
        setFixedSize(_w, _h);
    }

    void MenageWindow::set_title(const std::string _title)
    {
        setWindowTitle(_title.c_str());
    }

    void MenageWindow::show(const bool is_note)
    {
        (void)is_note;
        QWidget::show();
    }

    void MenageWindow::close()
    {
        QWidget::close();
    }

    void MenageWindow::mousePressEvent(QMouseEvent* event)
    {
        QWidget::mousePressEvent(event);
    }

    void MenageWindow::dragEnterEvent(QDragEnterEvent* event)
    {
        if (event->mimeData()->hasUrls())
        {
            event->acceptProposedAction();
        }
    }

    void MenageWindow::dropEvent(QDropEvent* event)
    {
        const QMimeData* mimeData = event->mimeData();
        if (mimeData->hasUrls())
        {
            for (const QUrl& url : mimeData->urls())
            {
                QString filePath = url.toLocalFile();
                if (filePath.endsWith(".json", Qt::CaseInsensitive))
                {
                    process_json_file(filePath);
                }
            }
        }
    }

    void MenageWindow::upload_notes()
    {
        QString filePath = QFileDialog::getOpenFileName(this, "Open Notes File", "", "JSON Files (*.json)");
        if (!filePath.isEmpty())
        {
            process_json_file(filePath);
        }
    }

    void MenageWindow::download_notes()
    {
        QString filePath = QFileDialog::getSaveFileName(this, "Save Notes Copy", "notes_backup.json",
                                                        "JSON Files (*.json)");
        if (filePath.isEmpty()) return;

        QVector<NoteData> notes = SaveHandler::load_notes();
        QJsonArray notesArray;

        for (const auto& note : notes)
        {
            QJsonObject noteObj;
            noteObj["id"] = note.id.toString();
            noteObj["x"] = note.pos.x();
            noteObj["y"] = note.pos.y();
            noteObj["color"] = note.color.name();
            noteObj["title"] = note.title;
            noteObj["text"] = note.text;
            noteObj["is_pinned"] = note.is_pinned;
            notesArray.append(noteObj);
        }

        QJsonDocument doc(notesArray);
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(doc.toJson());
            file.close();
            QMessageBox::information(this, "Success", "Notes exported successfully!");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Could not save file.");
        }
    }

    void MenageWindow::process_json_file(const QString& filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this, "Error", "Could not open file.");
            return;
        }

        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull())
        {
            QMessageBox::critical(this, "Error", "Invalid JSON file.");
            return;
        }

        QVector<NoteData> importedNotes;
        if (doc.isObject())
        {
            // Single note upload
            QJsonObject obj = doc.object();
            NoteData note;
            note.id = QUuid::createUuid();
            note.id = QUuid::fromString(obj["id"].toString());
            if (note.id.isNull()) note.id = QUuid::createUuid();

            note.pos = QPoint(obj["x"].toInt(), obj["y"].toInt());
            note.color = QColor(obj["color"].toString());
            note.title = obj["title"].toString();
            note.text = obj["text"].toString();
            note.is_pinned = obj["is_pinned"].toBool();
            importedNotes.push_back(note);
        }
        else if (doc.isArray())
        {
            // Multiple notes upload
            QJsonArray array = doc.array();
            for (auto value : array)
            {
                QJsonObject obj = value.toObject();
                NoteData note;
                note.id = QUuid::fromString(obj["id"].toString());
                if (note.id.isNull()) note.id = QUuid::createUuid();

                note.pos = QPoint(obj["x"].toInt(), obj["y"].toInt());
                note.color = QColor(obj["color"].toString());
                note.title = obj["title"].toString();
                note.text = obj["text"].toString();
                note.is_pinned = obj["is_pinned"].toBool();
                importedNotes.push_back(note);
            }
        }

        for (const auto& note : importedNotes)
        {
            SaveHandler::save_to_json(note);
            // Open the note window
            auto* note_window = new NoteWindow(note.id, note.pos, note.color, note.title, note.text, note.is_pinned);
            auto* note_action = new NoteAction(note_window);
            note_action->create_note(note_window);
        }

        QMessageBox::information(this, "Success", QString("Imported %1 notes.").arg(importedNotes.size()));
    }
}
