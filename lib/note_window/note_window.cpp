//
// Created by roki on 2026-02-16.
//

#include "note_window.h"
#include "note_action/note_action.h"

#include <QDebug> // Testing

#include "shared.h"

static constexpr int WINDOW_MARGIN = 8;

sticky_note::NoteWindow::NoteWindow(QWidget* parent)
    : QWidget(parent)
{
    quit_action = new QAction("Quit", this);
    layout = new QVBoxLayout(this);
    top_layout = new QHBoxLayout();

    title_label = new QLabel(this);
    quit_btn = new QPushButton("Quit", this);

    quit_btn->setCursor(Qt::PointingHandCursor);

    connect(quit_btn, &QPushButton::clicked, quit_action, &QAction::trigger);
    quit_action->setShortcut(QKeySequence::Close);

    title_label->setFont(note_fonts::TITLE_FONT);
    quit_btn->setEnabled(false);

    layout->setContentsMargins(WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN);

    top_layout->addStretch();
    top_layout->addWidget(title_label, 4, Qt::AlignCenter);
    top_layout->addWidget(quit_btn, 0, Qt::AlignCenter);
    top_layout->addStretch();

    layout->addLayout(top_layout);
    setLayout(layout);
    addAction(quit_action);

    setStyleSheet("background: #fff6a8;");
    setAttribute(Qt::WA_DeleteOnClose);

    connect(quit_action, &QAction::triggered, this, [this]()
    {
        close();
    });

    setMouseTracking(true);
}

void sticky_note::NoteWindow::init(const int _w, const int _h, const std::string _title)
{
    resize(_w, _h);
    set_title(_title);
}

void sticky_note::NoteWindow::set_title(const std::string _title)
{
    setWindowTitle(QString::fromStdString(_title));
    title_label->setText(QString::fromStdString(_title));
}

void sticky_note::NoteWindow::show(const bool is_note)
{
    if (is_note)
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
    }

    QWidget::show();
}

void sticky_note::NoteWindow::close()
{
    QWidget::close();
}

void sticky_note::NoteWindow::enterEvent(QEnterEvent* event)
{
    qDebug() << "NoteWindow enter event";
    quit_btn->setEnabled(true);
    setCursor(Qt::OpenHandCursor);
    QWidget::enterEvent(event);
}

void sticky_note::NoteWindow::leaveEvent(QEvent* event)
{
    qDebug() << "NoteWindow leave event";
    quit_btn->setEnabled(false);
    unsetCursor();
    QWidget::leaveEvent(event);
}

void sticky_note::NoteWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (is_dragging && (event->buttons() & Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() + drag_offset);
        event->accept();
    }

    QWidget::mouseMoveEvent(event);
}

void sticky_note::NoteWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QWidget* child = childAt(event->position().toPoint());
        if (child && (qobject_cast<QAbstractButton*>(child) != nullptr))
        {
            QWidget::mousePressEvent(event);
            return;
        }

        is_dragging = true;
        setCursor(Qt::ClosedHandCursor);
        drag_offset = frameGeometry().topLeft() - event->globalPosition().toPoint();
        event->accept();
    }
}

void sticky_note::NoteWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && is_dragging)
    {
        is_dragging = false;
        setCursor(Qt::OpenHandCursor);
        event->accept();
    }

    QWidget::mouseReleaseEvent(event);
}
