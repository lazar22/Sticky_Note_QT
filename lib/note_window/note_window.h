//
// Created by roki on 2026-02-16.
//

#ifndef NOTE_WINDOW_H
#define NOTE_WINDOW_H

#include <QWidget>
#include <QAction>
#include <QWindow>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <QWidgetAction>
#include <QGridLayout>
#include <QColorDialog>

#include "IWindow.h"

namespace sticky_note
{
    class NoteWindow : public QWidget, public IWindow
    {
        Q_OBJECT

        QAction* quit_action = nullptr;
        QAction* edit_action = nullptr;
        QAction* create_action = nullptr;
        QAction* save_action = nullptr;
        QAction* color_action = nullptr;

        QLabel* title_label = nullptr;
        QLabel* note_label = nullptr;

        QLineEdit* title_edit = nullptr;
        QTextEdit* note_edit = nullptr;

        QPushButton* quit_btn = nullptr;
        QPushButton* edit_btn = nullptr;
        QPushButton* color_btn = nullptr;

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

        void edit(std::string _note) override;

        void save();

        void close() override;

        void change_color(const QColor& color);

        void enterEvent(QEnterEvent* event) override;

        void leaveEvent(QEvent* event) override;

        void mouseMoveEvent(QMouseEvent* event) override;

        void mousePressEvent(QMouseEvent* event) override;

        void mouseReleaseEvent(QMouseEvent* event) override;
    };
}

#endif //NOTE_WINDOW_H
