//
// Created by roki on 2026-02-16.
//

#ifndef NOTE_WINDOW_H
#define NOTE_WINDOW_H

#include <QWidget>
#include <QAction>
#include <QWindow>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "IWindow.h"

namespace sticky_note
{
    class NoteWindow : public QWidget, public IWindow
    {
        Q_OBJECT

        QAction* quit_action = nullptr;
        QAction* edit_action = nullptr;
        QAction* create_action = nullptr;

        QLabel* title_label = nullptr;

        QPushButton* quit_btn = nullptr;
        QPushButton* edit_btn = nullptr;

        QHBoxLayout* top_layout = nullptr;
        QVBoxLayout* layout = nullptr;

        bool is_dragging = false;
        QPoint drag_offset;

    public:
        explicit NoteWindow(QWidget* parent = nullptr);

        void init(int _w, int _h, std::string _title) override;

        ~NoteWindow() override = default;

    public:
        void set_title(std::string _title) override;

        void show(bool is_note) override;

        void close() override;

        void enterEvent(QEnterEvent* event) override;

        void leaveEvent(QEvent* event) override;

        void mouseMoveEvent(QMouseEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;

        void mouseReleaseEvent(QMouseEvent* event) override;
    };
}

#endif //NOTE_WINDOW_H
