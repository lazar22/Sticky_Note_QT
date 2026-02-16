//
// Created by roki on 2026-02-16.
//

#include "main_window.h"

#include "shared.h"
#include "note_window/note_window.h"

static constexpr int WINDOW_MARGIN = 8;

sticky_note::MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    auto* note_action = new NoteAction(this);
    auto* create_action = new QAction("New", this);
    const auto layout = new QVBoxLayout(this);

    create_btn = new QPushButton(this);

    connect(create_btn, &QPushButton::clicked, create_action, &QAction::trigger);
    create_action->setShortcut(QKeySequence::New);

    layout->setContentsMargins(WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN);

    layout->addStretch();
    layout->addWidget(create_btn, 0, Qt::AlignCenter);
    layout->addStretch();

    setLayout(layout);
    addAction(create_action);

    setStyleSheet("background: #fff6a8;");

    connect(create_action, &QAction::triggered, this, [this]()
    {
        auto* note_window = new NoteWindow();
        auto* note_action = new NoteAction(note_window);
        note_action->create_note(note_window);
    });
}

void sticky_note::MainWindow::init(const int _w, const int _h, const std::string _title)
{
    set_title(_title);
    setFixedSize(_w, _h);
    show(false);

    create_btn->setText("Create New Note");
    create_btn->setFixedSize(width() - 2 * WINDOW_MARGIN, (height() * 0.05) * WINDOW_MARGIN);
    create_btn->setCursor(Qt::PointingHandCursor);
    create_btn->setStyleSheet("background: #fff6a8;");
}

void sticky_note::MainWindow::set_title(const std::string _title)
{
    setWindowTitle(_title.c_str());
}

void sticky_note::MainWindow::show(const bool is_note)
{
    if (is_note)
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
    }
    else
    {
        showNormal();
    }
}
