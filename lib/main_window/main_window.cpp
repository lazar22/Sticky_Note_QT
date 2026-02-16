//
// Created by roki on 2026-02-16.
//

#include "main_window.h"

sticky_note::MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    editor_ = new QTextEdit(this);

    QVBoxLayout layout;
    layout.setParent(parent);

    layout.setContentsMargins(8, 8, 8, 8);
    layout.addWidget(editor_);

    setLayout(&layout);

    setStyleSheet("background: #fff6a8;");
    editor_->setStyleSheet("background: transparent;");
}

void sticky_note::MainWindow::init(const int _w, const int _h, const std::string _title)
{
    setWindowTitle(_title.c_str());
    resize(_w, _h);
    showNormal();
}
