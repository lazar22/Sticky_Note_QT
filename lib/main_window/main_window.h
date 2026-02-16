//
// Created by roki on 2026-02-16.
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>
#include <QWindow>
#include <QTextEdit>
#include <QVBoxLayout>

#include "IWindow.h"

namespace sticky_note
{
    class MainWindow : public QWidget, public IWindow
    {
        Q_OBJECT
        QTextEdit* editor_ = nullptr;
        QWindow* main_window_app = nullptr;

    public:
        explicit MainWindow(QWidget* parent = nullptr);

        void init(int _w, int _h, std::string _title) override;

        ~MainWindow() override = default;

    public:
    };
}

#endif //MAIN_WINDOW_H
