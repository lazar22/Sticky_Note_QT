//
// Created by roki on 2026-02-16.
//

#include "note_action.h"
#include "shared.h"
#include <iostream>

sticky_note::NoteAction::NoteAction(QObject* parent) : QObject(parent)
{
}

void sticky_note::NoteAction::create_note(IWindow* window)
{
    window->init(note_window::WIDTH, note_window::HEIGHT, note_window::TITLE);
    window->show(true);
}

void sticky_note::NoteAction::toggle_edit()
{
}

void sticky_note::NoteAction::close_note()
{
    emit request_create_note();
}
