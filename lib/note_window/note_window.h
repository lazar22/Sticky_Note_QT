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
#include <QUuid>

#include <QTextBrowser>
#include <QRegularExpression>

#include "IWindow.h"

namespace sticky_note
{
    class NoteWindow : public QWidget, public IWindow
    {
        Q_OBJECT

        QUuid id;
        QString note_text;
        QColor current_color;

        QAction* quit_action = nullptr;
        QAction* edit_action = nullptr;
        QAction* create_action = nullptr;
        QAction* save_action = nullptr;
        QAction* color_action = nullptr;
        QAction* pin_action = nullptr;

        QWidgetAction* drop_down = nullptr;

        QLabel* title_label = nullptr;
        QTextBrowser* note_label = nullptr;

        QLineEdit* title_edit = nullptr;
        QTextEdit* note_edit = nullptr;

        QPushButton* quit_btn = nullptr;
        QPushButton* edit_btn = nullptr;
        QPushButton* color_btn = nullptr;
        QPushButton* pin_btn = nullptr;
        QPushButton* color_pick_btn = nullptr;

        QBoxLayout* button_layout = nullptr;
        QVBoxLayout* layout = nullptr;
        QGridLayout* grid_layout = nullptr;

        QWidget* container = nullptr;

        bool is_dragging = false;
        bool is_resizing = false;
        QPoint drag_offset;
        QRect initial_geometry;
        Qt::Edges resize_edges;
        bool is_pinned = false;
        bool is_menu_active = false;

    public:
        explicit NoteWindow(QWidget* parent = nullptr);
        explicit NoteWindow(QUuid _id, QPoint _pos, QColor _color, QString _title, QString _text,
                            bool _is_pinned = false, QWidget* parent = nullptr);

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

        void resizeEvent(QResizeEvent* event) override;

    private:
        void update_font_sizes() const;

        Qt::Edges get_resize_edges(const QPoint& pos) const;

        static QString to_view_markdown(const QString& md);

        static QString from_view_markdown(const QString& md);
    };
}

#endif //NOTE_WINDOW_H
