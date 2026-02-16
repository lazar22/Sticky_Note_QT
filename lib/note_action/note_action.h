//
// Created by roki on 2026-02-16.
//

#ifndef NOTE_ACTION_H
#define NOTE_ACTION_H

#include <QObject>

#include "IWindow.h"

namespace sticky_note
{
    class NoteAction : public QObject
    {
        Q_OBJECT

    public:
        explicit NoteAction(QObject* parent = nullptr);

    public slots:
        void create_note(IWindow* window);

        void toggle_edit();

        void close_note();

    signals:
        void request_create_note();
    };
}

#endif //NOTE_ACTION_H
