//
// Created by roki on 2026-02-16.
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>
#include <QAction>
#include <QWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSystemTrayIcon>

#include "IWindow.h"
#include "note_action/note_action.h"

namespace sticky_note
{
    class MainWindow : public QWidget, public IWindow
    {
        Q_OBJECT
        QPushButton* create_btn = nullptr;
        QWindow* main_window_app = nullptr;

    public:
        explicit MainWindow(QWidget* parent = nullptr);

        void init(int _w, int _h, std::string _title) override;

        ~MainWindow() override = default;

    public:
        void set_title(std::string _title) override;

        void show(bool is_note) override;

        void close() override
        {
        };

        void enterEvent(QEnterEvent* event) override
        {
        }

        void leaveEvent(QEvent* event) override
        {
        }

        void mouseMoveEvent(QMouseEvent* event) override
        {
        }

        void mousePressEvent(QMouseEvent* event) override
        {
        }

        void mouseReleaseEvent(QMouseEvent* event) override
        {
        }
    };
}

#endif //MAIN_WINDOW_H
