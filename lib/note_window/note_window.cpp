//
// Created by roki on 2026-02-16.
//

#include "note_window.h"
#include "note_action/note_action.h"

#include <QDebug> // Testing

#include "shared.h"
#include "save_handler/save_handler.h"
#include <QScreen>
#include <QGuiApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

static constexpr int WINDOW_MARGIN = 8;

sticky_note::NoteWindow::NoteWindow(QWidget* parent)
    : QWidget(parent), id(QUuid::createUuid()), current_color("#fff6a8")
{
    quit_action = new QAction("Quit", this);
    edit_action = new QAction("Edit", this);
    create_action = new QAction("New", this);
    save_action = new QAction("Save", this);
    color_action = new QAction("Change Color", this);

    layout = new QVBoxLayout(this);
    top_layout = new QHBoxLayout();

    title_label = new QLabel(this);
    note_label = new QLabel(this);

    title_edit = new QLineEdit(this);
    note_edit = new QTextEdit(this);

    edit_btn = new QPushButton(this);
    quit_btn = new QPushButton(this);
    color_btn = new QPushButton(this);

    edit_btn->setFixedSize(36, 36);
    quit_btn->setFixedSize(36, 36);
    color_btn->setFixedSize(36, 36);

    edit_btn->setIcon(QIcon("icons/pen.png"));
    quit_btn->setIcon(QIcon("icons/exit.png"));
    color_btn->setIcon(QIcon("icons/wheel.png"));

    edit_btn->setFlat(true);
    quit_btn->setFlat(true);
    color_btn->setFlat(true);

    edit_btn->setCursor(Qt::PointingHandCursor);
    quit_btn->setCursor(Qt::PointingHandCursor);
    color_btn->setCursor(Qt::PointingHandCursor);

    connect(quit_btn, &QPushButton::clicked, quit_action, &QAction::trigger);
    quit_action->setShortcut(QKeySequence::Close); // Ctrl + W

    connect(edit_btn, &QPushButton::clicked, edit_action, &QAction::trigger);
    edit_action->setShortcut(QKeySequence("Ctrl+E"));

    connect(color_btn, &QPushButton::clicked, this, [this]()
    {
        QMenu menu(this);

        container = new QWidget(&menu);
        grid_layout = new QGridLayout(container);

        grid_layout->setSpacing(2);
        grid_layout->setContentsMargins(5, 5, 5, 5);

        const QVector<QColor> colors = {
            "#ffadad", "#ffd6a5", "#fdffb6", "#caffbf",
            "#9bf6ff", "#a0c4ff", "#bdb2ff", "#ffc6ff",
            "#fffffc", "#f0f0f0", "#d9d9d9", "#bfbfbf",
            "#ff7eb9", "#7afcff", "#feff9c", "#ff9bbb"
        };

        for (int i = 0; i < colors.size(); ++i)
        {
            color_pick_btn = new QPushButton();
            color_pick_btn->setFixedSize(25, 25);
            color_pick_btn->setStyleSheet(
                QString("background-color: %1; border: 1px solid gray;").arg(colors[i].name()));
            connect(color_pick_btn, &QPushButton::clicked, this, [this, color = colors[i], &menu]()
            {
                change_color(color);
                menu.close();
            });
            grid_layout->addWidget(color_pick_btn, i / 4, i % 4);
        }

        drop_down = new QWidgetAction(&menu);
        drop_down->setDefaultWidget(container);
        menu.addAction(drop_down);

        menu.addSeparator();
        QAction* more_colors_action = menu.addAction("More colors...");
        connect(more_colors_action, &QAction::triggered, this, [this]()
        {
            QColor color = QColorDialog::getColor(palette().window().color(), this, "Select Color");
            if (color.isValid())
            {
                change_color(color);
            }
        });

        menu.exec(color_btn->mapToGlobal(QPoint(0, color_btn->height())));
    });
    connect(color_action, &QAction::triggered, this, [this]()
    {
        color_btn->click();
    });
    color_action->setShortcut(QKeySequence("Ctrl+C"));

    connect(save_action, &QAction::triggered, this, &NoteWindow::save);
    save_action->setShortcut(QKeySequence("Ctrl+S"));

    create_action->setShortcut(QKeySequence::New); // Ctrl + N

    title_label->setFont(note_fonts::TITLE_FONT);
    title_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    title_label->setStyleSheet("background: transparent; border: none;");

    title_edit->setFont(note_fonts::TITLE_FONT);
    title_edit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    title_edit->setStyleSheet("background: transparent; border: none;");
    title_edit->hide();

    note_label->setFont(note_fonts::REGULAR_FONT);
    note_label->setWordWrap(true);
    note_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    note_label->setStyleSheet("background: transparent; border: none;");

    note_edit->setFont(note_fonts::REGULAR_FONT);
    note_edit->setStyleSheet("background: transparent; border: none;");
    note_edit->hide();

    quit_btn->setEnabled(false);
    edit_btn->setEnabled(false);
    color_btn->setEnabled(false);

    layout->setContentsMargins(WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN);

    top_layout->addStretch();
    top_layout->addWidget(title_label, 1);
    top_layout->addWidget(title_edit, 1);
    top_layout->addWidget(edit_btn, 0);
    top_layout->addWidget(color_btn, 0);
    top_layout->addWidget(quit_btn, 0);
    top_layout->addStretch();

    layout->addLayout(top_layout);
    layout->addWidget(note_label, 1);
    layout->addWidget(note_edit, 1);

    setLayout(layout);
    addAction(quit_action);
    addAction(edit_action);
    addAction(create_action);
    addAction(save_action);
    addAction(color_action);

    setStyleSheet("background: " + current_color.name() + ";");
    setAttribute(Qt::WA_DeleteOnClose);

    connect(quit_action, &QAction::triggered, this, [this]()
    {
        SaveHandler::delete_json(id);
        close();
    });

    connect(create_action, &QAction::triggered, this, [this]()
    {
        auto* note_window = new NoteWindow();
        auto* note_action = new NoteAction(note_window);
        note_action->create_note(note_window);
    });

    connect(edit_action, &QAction::triggered, this, [this]()
    {
        if (title_edit->isHidden())
        {
            title_label->hide();
            title_edit->setText(title_label->text());
            title_edit->show();

            note_label->hide();
            note_edit->setText(note_label->text());
            note_edit->show();

            title_edit->setFocus();
        }
        else
        {
            save();
        }
    });

    setMouseTracking(true);
}

sticky_note::NoteWindow::NoteWindow(QUuid _id, QPoint _pos, QColor _color, QString _title, QString _text,
                                    QWidget* parent)
    : NoteWindow(parent)
{
    id = _id;
    current_color = _color;
    move(_pos);
    set_title(_title.toStdString());
    edit(_text.toStdString());
    setStyleSheet("background: " + current_color.name() + ";");
}

void sticky_note::NoteWindow::init(const int _w, const int _h, const std::string _title)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        const QRect screenGeometry = screen->geometry();
        const int width = screenGeometry.width() * 0.15; // 15% of screen width
        const int height = screenGeometry.height() * 0.25; // 25% of screen height
        resize(width, height);
    }
    else
    {
        resize(_w, _h);
    }

    if (title_label->text().isEmpty() || title_label->text() == sticky_note::note_window::TITLE)
    {
        set_title(_title);
    }
    SaveHandler::save_to_json({id, pos(), current_color, title_label->text(), note_label->text()});
}

void sticky_note::NoteWindow::set_title(const std::string _title)
{
    setWindowTitle(QString::fromStdString(_title));
    title_label->setText(QString::fromStdString(_title));
    title_edit->setText(QString::fromStdString(_title));
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

void sticky_note::NoteWindow::edit(const std::string _note)
{
    note_label->setText(QString::fromStdString(_note));
    note_edit->setText(QString::fromStdString(_note));
}

void sticky_note::NoteWindow::save()
{
    title_label->setText(title_edit->text());
    note_label->setText(note_edit->toPlainText());

    title_edit->hide();
    title_label->show();

    note_edit->hide();
    note_label->show();

    setWindowTitle(title_label->text());
    SaveHandler::save_to_json({id, pos(), current_color, title_label->text(), note_label->text()});
}

void sticky_note::NoteWindow::change_color(const QColor& color)
{
    current_color = color;
    setStyleSheet(QString("background-color: %1;").arg(current_color.name()));
    SaveHandler::save_to_json({id, pos(), current_color, title_label->text(), note_label->text()});
}

void sticky_note::NoteWindow::enterEvent(QEnterEvent* event)
{
    quit_btn->setEnabled(true);
    edit_btn->setEnabled(true);
    color_btn->setEnabled(true);
    setCursor(Qt::OpenHandCursor);
    QWidget::enterEvent(event);
}

void sticky_note::NoteWindow::leaveEvent(QEvent* event)
{
    quit_btn->setEnabled(false);
    edit_btn->setEnabled(false);
    color_btn->setEnabled(false);
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
        SaveHandler::save_to_json({id, pos(), current_color, title_label->text(), note_label->text()});
        event->accept();
    }

    QWidget::mouseReleaseEvent(event);
}
