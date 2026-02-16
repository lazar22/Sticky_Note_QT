//
// Created by roki on 2026-02-16.
//

#include <QApplication>
#include "shared.h"
#include "main_window/main_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    sticky_note::MainWindow main_window;
    main_window.init(
        sticky_note::main_window::WIDTH,
        sticky_note::main_window::HEIGHT,
        sticky_note::main_window::TITLE);

    return app.exec();
}
