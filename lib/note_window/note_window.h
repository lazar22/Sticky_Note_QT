//
// Created by roki on 2026-02-16.
//

#ifndef NOTE_WINDOW_H
#define NOTE_WINDOW_H


#include <QWidgetAction>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>
#include <QAction>
#include <QWindow>
#include <QLabel>
#include <QMenu>
#include <QUuid>

#include <QRegularExpression>
#include <QTextBrowser>

#include "scaling_button/scaling_button.h"
#include "IWindow.h"

namespace sticky_note
{
    class NoteWindow : public QWidget, public IWindow
    {
        Q_OBJECT

        QUuid id; // ID of the NoteWindow instance
        QString note_text; // Text content of the note
        QColor current_color; // Color of the note

        // Actions
        QAction* quit_action = nullptr;
        QAction* edit_action = nullptr;
        QAction* create_action = nullptr;
        QAction* save_action = nullptr;
        QAction* color_action = nullptr;
        QAction* pin_action = nullptr;

        QWidgetAction* drop_down = nullptr; // Dropdown menu action

        QLabel* title_label = nullptr; // Node Title
        QTextBrowser* note_label = nullptr; // Node content

        QLineEdit* title_edit = nullptr; // Editable Node Title
        QTextEdit* note_edit = nullptr; // Editable Node Content

        // Buttons
        ScalingButton* quit_btn = nullptr;
        ScalingButton* edit_btn = nullptr;
        ScalingButton* color_btn = nullptr;
        ScalingButton* pin_btn = nullptr;
        QPushButton* color_pick_btn = nullptr;

        // Layouts
        QBoxLayout* button_layout = nullptr;
        QVBoxLayout* layout = nullptr;
        QGridLayout* grid_layout = nullptr;

        QWidget* container = nullptr;

        bool is_dragging = false;
        bool is_resizing = false;
        bool is_pinned = false;
        bool is_menu_active = false;

        int checkbox_check_area = 20;

        QPoint drag_offset;
        QRect initial_geometry;
        Qt::Edges resize_edges;

    public:
        explicit NoteWindow(QWidget* parent = nullptr);

        explicit NoteWindow(QUuid _id, QPoint _pos, QColor _color, QString _title, QString _text,
                            bool _is_pinned = false, QWidget* parent = nullptr);

        void init(int _w, int _h, std::string _title) override;

        ~NoteWindow() override = default;

    public:
        /**
         * @brief Function to set Title Label of the Note
         *
         * @param _title you want to set for the note
         */
        void set_title(std::string _title) override;

        /**
         * @brief Function to show a window
         *
         * @param is_note if the window is a note window or not
         */
        void show(bool is_note) override;

        /**
         * @brief Function to edit the note content
         *
         * @param _note new content for the note
         */
        void edit(std::string _note) override;

        /**
         * @brief Function to save the note content
         */
        void save();

        /**
         * @brief Function to close the window
         */
        void close() override;

        /**
         * @brief Function to change the color of the note
         *
         * @param color new color for the note
         */
        void change_color(const QColor& color);

        /**
         * @brief Function to check the Event of the Mouse entering the window
         *
         * @param event you want to check for entry
         */
        void enterEvent(QEnterEvent* event) override;

        /**
         * @brief Function to check the Event of the Mouse leaving the window
         *
         * @param event you want to check for leaving
         */
        void leaveEvent(QEvent* event) override;

        /**
         * @brief Function to check the Event of the Mouse Movement in the window
         *
         * @param event you want to check for movement
         */
        void mouseMoveEvent(QMouseEvent* event) override;

        /**
         * @brief Function to check the Event of the Mouse press in the window
         *
         * @param event you want to check for press
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * @brief Function to check the Event of the Mouse release in the window
         *
         * @param event you want to check for release
         */
        void mouseReleaseEvent(QMouseEvent* event) override;

        /**
         * @brief Function to check the Event of the window resize
         *
         * @param event you want to check for resize
         */
        void resizeEvent(QResizeEvent* event) override;

        /**
         * @brief Function to update the contents height based on content
         */
        void update_height();

    private:
        /**
         * @brief Function to update font size depending on the size of the window
         */
        void update_font_sizes() const;

        /**
         * @brief Function to change the style of the note
         */
        void apply_styles();

        /**
         * @brief Function to change the mouse cursor to the correct cursor
         *
         * @param pos of the mouse cursor
         */
        Qt::Edges get_resize_edges(const QPoint& pos) const;

        /**
         * @brief Function to transfer edited md text to regular Label text
         *
         * @param md format of the text you want to adapt
         */
        static QString to_view_markdown(const QString& md);

        /**
         * @brief Function to transfer from the regular Label text to edited md text
         *
         * @param md regular text to adapt to md text
         */
        static QString from_view_markdown(const QString& md);
    };
}

#endif //NOTE_WINDOW_H
