//
// Created by roki on 2026-02-16.
//

#include "main_window.h"

#include "shared.h"
#include "note_window/note_window.h"
#include "save_handler/save_handler.h"

#include <QTimer>
#include <QMenu>
#include <QApplication>
#include <QDir>
#include <QUuid>

static constexpr int WINDOW_MARGIN = 8;

sticky_note::MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    auto* create_action = new QAction("New", this);
    const auto layout = new QVBoxLayout(this);

    const QIcon app_icon = QIcon("icons/note_icon.png");

    QApplication::setWindowIcon(app_icon);
    setWindowIcon(app_icon);

    tray_icon = new QSystemTrayIcon(app_icon, this);
    auto* tray_menu = new QMenu(this);

    auto* restore_action = new QAction("Restore", this);
    auto* close_all_action = new QAction("Close All Notes", this);
    auto* quit_action = new QAction("Quit", this);

    tray_menu->addAction(create_action);
    tray_menu->addSeparator();
    tray_menu->addAction(close_all_action);
    tray_menu->addAction(restore_action);
    tray_menu->addAction(quit_action);

    tray_icon->setContextMenu(tray_menu);
    tray_icon->setToolTip("Sticky Note");

    // Click the tray icon to restore
    connect(tray_icon, &QSystemTrayIcon::activated, this,
            [this](QSystemTrayIcon::ActivationReason reason)
            {
                if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
                {
                    restoreFromTray();
                }
            });

    create_btn = new QPushButton(this);

    connect(create_btn, &QPushButton::clicked, create_action, &QAction::trigger);
    create_action->setShortcut(QKeySequence::New);

    connect(restore_action, &QAction::triggered, this, &MainWindow::restoreFromTray);
    connect(close_all_action, &QAction::triggered, this, [this]()
    {
        for (auto* widget : QApplication::topLevelWidgets())
        {
            if (widget->inherits("sticky_note::NoteWindow"))
            {
                widget->close();
            }
        }
    });
    connect(quit_action, &QAction::triggered, qApp, &QCoreApplication::quit);

    tray_icon->show();

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

    load_notes();
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

void sticky_note::MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if (isMaximized())
        {
            QTimer::singleShot(0, this, &QWidget::hide);
        }
    }

    QWidget::changeEvent(event);
}

void sticky_note::MainWindow::restoreFromTray()
{
    show(false);
    setWindowState((windowState() & ~Qt::WindowMinimized) | (Qt::WindowActive));
    raise();
    activateWindow();
}

void sticky_note::MainWindow::load_notes()
{
    const QVector<NoteData> notes = SaveHandler::load_notes();
    for (const auto& data : notes)
    {
        auto* note_window = new NoteWindow(data.id, data.pos, data.color, data.title, data.text);
        auto* note_action = new NoteAction(note_window);
        note_action->create_note(note_window);
    }
}
