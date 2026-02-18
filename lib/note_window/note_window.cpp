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
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>

static constexpr int WINDOW_MARGIN = 8;
static constexpr int RESIZE_HANDLE_SIZE = 10;

sticky_note::NoteWindow::NoteWindow(QWidget* parent)
    : QWidget(parent), id(QUuid::createUuid()), current_color("#fff6a8")
{
    setObjectName("NoteWindow");
    quit_action = new QAction("Quit", this);
    edit_action = new QAction("Edit", this);
    create_action = new QAction("New", this);
    save_action = new QAction("Save", this);
    color_action = new QAction("Change Color", this);
    pin_action = new QAction("Pin", this);

    layout = new QVBoxLayout(this);
    auto* content_layout = new QHBoxLayout();
    auto* text_layout = new QVBoxLayout();
    button_layout = new QBoxLayout(QBoxLayout::TopToBottom);

    title_label = new QLabel(this);
    note_label = new QTextBrowser(this);

    title_edit = new QLineEdit(this);
    note_edit = new QTextEdit(this);

    edit_btn = new QPushButton(this);
    quit_btn = new QPushButton(this);
    color_btn = new QPushButton(this);
    pin_btn = new QPushButton(this);

    note_label->setFrameStyle(QFrame::NoFrame);
    note_label->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    note_label->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    note_label->setReadOnly(true);
    note_label->setAttribute(Qt::WA_TransparentForMouseEvents);
    note_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    note_label->setMinimumHeight(0);

    constexpr int circle_size = 36;
    edit_btn->setFixedSize(circle_size, circle_size);
    quit_btn->setFixedSize(circle_size, circle_size);
    color_btn->setFixedSize(circle_size, circle_size);
    pin_btn->setFixedSize(circle_size, circle_size);

    edit_btn->setIcon(QIcon(":/icons/pen.png"));
    quit_btn->setIcon(QIcon(":/icons/exit.png"));
    color_btn->setIcon(QIcon(":/icons/wheel.png"));
    pin_btn->setIcon(QIcon(":/icons/pin.png"));

    edit_btn->setFlat(true);
    quit_btn->setFlat(true);
    color_btn->setFlat(true);
    pin_btn->setFlat(true);

    edit_btn->setCursor(Qt::PointingHandCursor);
    quit_btn->setCursor(Qt::PointingHandCursor);
    color_btn->setCursor(Qt::PointingHandCursor);
    pin_btn->setCursor(Qt::PointingHandCursor);

    connect(quit_btn, &QPushButton::clicked, quit_action, &QAction::trigger);
    quit_action->setShortcut(QKeySequence::Close); // Ctrl + W

    connect(edit_btn, &QPushButton::clicked, edit_action, &QAction::trigger);
    edit_action->setShortcut(QKeySequence("Ctrl+E"));

    connect(color_btn, &QPushButton::clicked, this, [this]()
    {
        is_menu_active = true;
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
        is_menu_active = false;
    });
    connect(color_action, &QAction::triggered, this, [this]()
    {
        color_btn->click();
    });
    color_action->setShortcut(QKeySequence("Ctrl+C"));

    connect(save_action, &QAction::triggered, this, &NoteWindow::save);
    save_action->setShortcut(QKeySequence("Ctrl+S"));

    auto pin_func = [this]()
    {
        is_pinned = !is_pinned;
        pin_btn->setIcon(is_pinned ? QIcon(":/icons/pin_active.png") : QIcon(":/icons/pin.png"));
        show(true);
        SaveHandler::save_to_json({
            id, pos(), current_color, title_label->text(), note_label->toPlainText(), is_pinned
        });
    };

    connect(pin_btn, &QPushButton::clicked, this, pin_func);
    connect(pin_action, &QAction::triggered, this, pin_func);
    pin_action->setShortcut(QKeySequence("Ctrl+P"));

    create_action->setShortcut(QKeySequence::New); // Ctrl + N

    title_label->setFont(note_fonts::TITLE_FONT);
    title_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    title_label->setStyleSheet("background: transparent; border: none;");

    title_edit->setFont(note_fonts::TITLE_FONT);
    title_edit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    title_edit->setStyleSheet("background: transparent; border: none;");
    title_edit->hide();

    note_label->setFont(note_fonts::REGULAR_FONT);
    note_label->setStyleSheet("background: transparent; border: none;");

    note_edit->setFont(note_fonts::REGULAR_FONT);
    note_edit->setStyleSheet("background: transparent; border: none;");
    note_edit->hide();

    quit_btn->setEnabled(false);
    edit_btn->setEnabled(false);
    color_btn->setEnabled(false);
    pin_btn->setEnabled(false);

    layout->setContentsMargins(WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN);

    button_layout->setSpacing(0);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->addWidget(quit_btn);
    button_layout->addWidget(pin_btn);
    button_layout->addWidget(color_btn);
    button_layout->addWidget(edit_btn);

    text_layout->addWidget(title_label);
    text_layout->addWidget(title_edit);
    text_layout->addWidget(note_label, 1);
    text_layout->addWidget(note_edit, 1);
    text_layout->setSpacing(2);

    content_layout->addLayout(text_layout, 1);
    content_layout->addLayout(button_layout);
    content_layout->setAlignment(button_layout, Qt::AlignTop);

    layout->addLayout(content_layout);

    setLayout(layout);
    addAction(quit_action);
    addAction(edit_action);
    addAction(create_action);
    addAction(save_action);
    addAction(color_action);
    addAction(pin_action);

    setStyleSheet(QString("#NoteWindow { background: %1; }").arg(current_color.name()));
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(sticky_note::note_window::MIN_WIDTH, sticky_note::note_window::MIN_HEIGHT);

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
            note_edit->setText(note_label->toPlainText());
            note_edit->show();

            title_edit->setFocus();
            edit_btn->setIcon(QIcon(":/icons/pen_active.png"));
        }
        else
        {
            save();
        }
    });

    setMouseTracking(true);
}

sticky_note::NoteWindow::NoteWindow(QUuid _id, QPoint _pos, QColor _color, QString _title, QString _text,
                                    bool _is_pinned, QWidget* parent)
    : NoteWindow(parent)
{
    id = _id;
    current_color = _color;
    is_pinned = _is_pinned;
    move(_pos);
    set_title(_title.toStdString());
    edit(_text.toStdString());
    setStyleSheet(QString("#NoteWindow { background: %1; }").arg(current_color.name()));
    if (is_pinned)
    {
        pin_btn->setIcon(QIcon(":/icons/pin_active.png"));
    }
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
    SaveHandler::save_to_json({id, pos(), current_color, title_label->text(), note_label->toPlainText(), is_pinned});
}

void sticky_note::NoteWindow::set_title(const std::string _title)
{
    setWindowTitle(QString::fromStdString(_title));
    title_label->setText(QString::fromStdString(_title));
    title_edit->setText(QString::fromStdString(_title));
}

void sticky_note::NoteWindow::show(const bool is_note)
{
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Window | Qt::Tool;
    if (is_pinned)
    {
        flags |= Qt::WindowStaysOnTopHint;
    }
    setWindowFlags(flags);
    QWidget::show();
}

void sticky_note::NoteWindow::close()
{
    QWidget::close();
}

void sticky_note::NoteWindow::edit(const std::string _note)
{
    note_label->setText(QString::fromStdString(_note));
    note_label->updateGeometry();
    note_edit->setText(QString::fromStdString(_note));
}

void sticky_note::NoteWindow::save()
{
    title_label->setText(title_edit->text());
    note_label->setText(note_edit->toPlainText());
    note_label->updateGeometry();

    title_edit->hide();
    title_label->show();

    note_edit->hide();
    note_label->show();

    edit_btn->setIcon(QIcon(":/icons/pen.png"));

    setWindowTitle(title_label->text());
    SaveHandler::save_to_json({id, pos(), current_color, title_label->text(), note_label->toPlainText(), is_pinned});
}

void sticky_note::NoteWindow::change_color(const QColor& color)
{
    current_color = color;
    setStyleSheet(QString("#NoteWindow { background: %1; }").arg(current_color.name()));
    SaveHandler::save_to_json({id, pos(), current_color, title_label->text(), note_label->toPlainText(), is_pinned});
}

void sticky_note::NoteWindow::enterEvent(QEnterEvent* event)
{
    quit_btn->setEnabled(true);
    edit_btn->setEnabled(true);
    color_btn->setEnabled(true);
    pin_btn->setEnabled(true);
    setCursor(Qt::OpenHandCursor);
    QWidget::enterEvent(event);
}

void sticky_note::NoteWindow::leaveEvent(QEvent* event)
{
    if (!is_menu_active)
    {
        quit_btn->setEnabled(false);
        edit_btn->setEnabled(false);
        color_btn->setEnabled(false);
        pin_btn->setEnabled(false);
        unsetCursor();
    }
    QWidget::leaveEvent(event);
}

void sticky_note::NoteWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (is_dragging && (event->buttons() & Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() + drag_offset);
        event->accept();
        return;
    }

    if (is_resizing && (event->buttons() & Qt::LeftButton))
    {
        QRect new_geometry = initial_geometry;
        const QPoint global_pos = event->globalPosition().toPoint();
        const QPoint delta = global_pos - (initial_geometry.topLeft() - drag_offset);

        if (resize_edges & Qt::LeftEdge)
        {
            const int new_width = initial_geometry.width() - delta.x();
            if (new_width >= minimumWidth())
            {
                new_geometry.setLeft(initial_geometry.left() + delta.x());
            }
        }
        if (resize_edges & Qt::RightEdge)
        {
            new_geometry.setRight(initial_geometry.right() + delta.x());
        }
        if (resize_edges & Qt::TopEdge)
        {
            const int new_height = initial_geometry.height() - delta.y();
            if (new_height >= minimumHeight())
            {
                new_geometry.setTop(initial_geometry.top() + delta.y());
            }
        }
        if (resize_edges & Qt::BottomEdge)
        {
            new_geometry.setBottom(initial_geometry.bottom() + delta.y());
        }

        setGeometry(new_geometry);
        event->accept();
        return;
    }

    if (!is_dragging && !is_resizing)
    {
        Qt::Edges edges = get_resize_edges(event->position().toPoint());
        if (edges == Qt::Edges())
        {
            if (cursor().shape() != Qt::OpenHandCursor && !is_menu_active)
                setCursor(Qt::OpenHandCursor);
        }
        else
        {
            if ((edges & Qt::LeftEdge && edges & Qt::TopEdge) || (edges & Qt::RightEdge && edges & Qt::BottomEdge))
                setCursor(Qt::SizeFDiagCursor);
            else if ((edges & Qt::RightEdge && edges & Qt::TopEdge) || (edges & Qt::LeftEdge && edges & Qt::BottomEdge))
                setCursor(Qt::SizeBDiagCursor);
            else if (edges & Qt::LeftEdge || edges & Qt::RightEdge)
                setCursor(Qt::SizeHorCursor);
            else if (edges & Qt::TopEdge || edges & Qt::BottomEdge)
                setCursor(Qt::SizeVerCursor);
        }
    }

    QWidget::mouseMoveEvent(event);
}

void sticky_note::NoteWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        resize_edges = get_resize_edges(event->position().toPoint());
        if (resize_edges != Qt::Edges())
        {
            is_resizing = true;
            initial_geometry = geometry();
            drag_offset = initial_geometry.topLeft() - event->globalPosition().toPoint();
            event->accept();
            return;
        }

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
    if (event->button() == Qt::LeftButton)
    {
        if (is_dragging || is_resizing)
        {
            is_dragging = false;
            is_resizing = false;
            setCursor(Qt::OpenHandCursor);
            SaveHandler::save_to_json({
                id, pos(), current_color, title_label->text(), note_label->toPlainText(), is_pinned
            });
            event->accept();
        }
    }

    QWidget::mouseReleaseEvent(event);
}

void sticky_note::NoteWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update_font_sizes();
}

void sticky_note::NoteWindow::update_font_sizes()
{
    int w = width();
    int h = height();

    // Base font size on the smaller dimension
    int base_dim = std::min(w, h);

    int title_size = std::max(12, base_dim / 10);
    int regular_size = std::max(10, base_dim / 15);

    QFont title_font = title_label->font();
    title_font.setPointSize(title_size);
    title_label->setFont(title_font);
    title_edit->setFont(title_font);

    QFont regular_font = note_label->font();
    regular_font.setPointSize(regular_size);
    note_label->setFont(regular_font);
    note_edit->setFont(regular_font);
}

Qt::Edges sticky_note::NoteWindow::get_resize_edges(const QPoint& pos)
{
    Qt::Edges edges = Qt::Edges();
    if (pos.x() < RESIZE_HANDLE_SIZE) edges |= Qt::LeftEdge;
    if (pos.x() > width() - RESIZE_HANDLE_SIZE) edges |= Qt::RightEdge;
    if (pos.y() < RESIZE_HANDLE_SIZE) edges |= Qt::TopEdge;
    if (pos.y() > height() - RESIZE_HANDLE_SIZE) edges |= Qt::BottomEdge;
    return edges;
}
