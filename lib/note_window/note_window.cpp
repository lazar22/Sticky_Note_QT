//
// Created by roki on 2026-02-16.
//

#include "save_handler/save_handler.h"
#include "note_action/note_action.h"
#include "note_window.h"
#include "shared.h"

#include <QGuiApplication>
#include <QResizeEvent>
#include <QColorDialog>
#include <QMouseEvent>
#include <QFileInfo>
#include <QPainter>
#include <QScreen>
#include <QFileDialog>
#include <QDir>

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

    edit_btn = new ScalingButton(this);
    quit_btn = new ScalingButton(this);
    color_btn = new ScalingButton(this);
    pin_btn = new ScalingButton(this);
    download_btn = new ScalingButton(this);

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
    download_btn->setFixedSize(circle_size, circle_size);

    constexpr QSize base_icon_size(24, 24);
    edit_btn->setBaseIconSize(base_icon_size);
    quit_btn->setBaseIconSize(base_icon_size);
    color_btn->setBaseIconSize(base_icon_size);
    pin_btn->setBaseIconSize(base_icon_size);
    download_btn->setBaseIconSize(base_icon_size);

    edit_btn->setIcon(sticky_note::note_icons::PEN_ICON());
    quit_btn->setIcon(sticky_note::note_icons::EXIT_ICON());
    color_btn->setIcon(sticky_note::note_icons::WHEEL_ICON());
    pin_btn->setIcon(sticky_note::note_icons::PIN_ICON());
    download_btn->setIcon(sticky_note::note_icons::DOWNLOAD_ICON());

    edit_btn->setFlat(true);
    quit_btn->setFlat(true);
    color_btn->setFlat(true);
    pin_btn->setFlat(true);
    download_btn->setFlat(true);

    quit_btn->setHoverBackground(QColor("#e74c3c"), circle_size / 2);

    edit_btn->setToolTip(QString("Edit (%1)").arg(sticky_note::note_shortcuts::EDIT_NOTE_SHORTCUT.toString()));
    quit_btn->setToolTip(QString("Quit (%1)").arg(sticky_note::note_shortcuts::QUIT_NOTE_SHORTCUT.toString()));
    color_btn->setToolTip(QString("Color (%1)").arg(sticky_note::note_shortcuts::COLOR_NOTE_SHORTCUT.toString()));
    pin_btn->setToolTip(QString("Pin (%1)").arg(sticky_note::note_shortcuts::PIN_NOTE_SHORTCUT.toString()));
    download_btn->setToolTip(
        QString("Download (%1)").arg(sticky_note::note_shortcuts::DOWNLOAD_NOTE_SHORTCUT.toString()));

    edit_btn->setCursor(Qt::PointingHandCursor);
    quit_btn->setCursor(Qt::PointingHandCursor);
    color_btn->setCursor(Qt::PointingHandCursor);
    pin_btn->setCursor(Qt::PointingHandCursor);
    download_btn->setCursor(Qt::PointingHandCursor);

    connect(quit_btn, &QPushButton::clicked, quit_action, &QAction::trigger);
    quit_action->setShortcut(sticky_note::note_shortcuts::QUIT_NOTE_SHORTCUT);

    connect(edit_btn, &QPushButton::clicked, edit_action, &QAction::trigger);
    edit_action->setShortcut(sticky_note::note_shortcuts::EDIT_NOTE_SHORTCUT);

    save_action->setShortcut(sticky_note::note_shortcuts::SAVE_NOTE_SHORTCUT);
    color_action->setShortcut(sticky_note::note_shortcuts::COLOR_NOTE_SHORTCUT);
    pin_action->setShortcut(sticky_note::note_shortcuts::PIN_NOTE_SHORTCUT);

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
    color_action->setShortcut(sticky_note::note_shortcuts::COLOR_NOTE_SHORTCUT);

    connect(save_action, &QAction::triggered, this, &NoteWindow::save);
    save_action->setShortcut(sticky_note::note_shortcuts::SAVE_NOTE_SHORTCUT);

    auto pin_func = [this]()
    {
        is_pinned = !is_pinned;
        pin_btn->setIcon(is_pinned ? sticky_note::note_icons::PIN_ACTIVE_ICON() : sticky_note::note_icons::PIN_ICON());
        show(true);
        SaveHandler::save_to_json({
            id, pos(), size(), current_color, title_label->text(), note_text, is_pinned
        });
    };

    connect(pin_btn, &QPushButton::clicked, this, pin_func);
    connect(pin_action, &QAction::triggered, this, pin_func);
    pin_action->setShortcut(sticky_note::note_shortcuts::PIN_NOTE_SHORTCUT);

    download_action = new QAction("Download", this);
    connect(download_btn, &QPushButton::clicked, download_action, &QAction::trigger);
    connect(download_action, &QAction::triggered, this, &NoteWindow::download);
    download_action->setShortcut(sticky_note::note_shortcuts::DOWNLOAD_NOTE_SHORTCUT);

    create_action->setShortcut(QKeySequence::New); // Ctrl + N

    title_label->setFont(note_fonts::TITLE_FONT());
    title_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    title_label->setStyleSheet("background: transparent; border: none;");

    title_edit->setFont(note_fonts::TITLE_FONT());
    title_edit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    title_edit->setStyleSheet("background: transparent; border: none;");
    title_edit->hide();

    note_label->setFont(note_fonts::REGULAR_FONT());
    note_label->setStyleSheet("background: transparent; border: none;");

    note_edit->setFont(note_fonts::REGULAR_FONT());
    note_edit->setStyleSheet("background: transparent; border: none;");
    note_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    note_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    note_edit->hide();

    quit_btn->setEnabled(false);
    edit_btn->setEnabled(false);
    color_btn->setEnabled(false);
    pin_btn->setEnabled(false);
    download_btn->setEnabled(false);

    layout->setContentsMargins(WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN / 2);

    button_layout->setSpacing(0);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->addWidget(quit_btn);
    button_layout->addWidget(pin_btn);
    button_layout->addWidget(color_btn);
    button_layout->addWidget(edit_btn);
    button_layout->addStretch();
    button_layout->addWidget(download_btn);

    text_layout->addWidget(title_label);
    text_layout->addWidget(title_edit);
    text_layout->addWidget(note_label, 1);
    text_layout->addWidget(note_edit, 1);
    text_layout->setSpacing(2);

    content_layout->addLayout(text_layout, 1);
    content_layout->addLayout(button_layout);

    layout->addLayout(content_layout);

    setLayout(layout);
    addAction(quit_action);
    addAction(edit_action);
    addAction(create_action);
    addAction(save_action);
    addAction(color_action);
    addAction(pin_action);

    apply_styles();

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
            note_edit->setPlainText(note_text);
            note_edit->show();

            title_edit->setFocus();
            edit_btn->setIcon(sticky_note::note_icons::PEN_ACTIVE_ICON());
        }
        else
        {
            save();
        }
    });

    setMouseTracking(true);
}

sticky_note::NoteWindow::NoteWindow(QUuid _id, QPoint _pos, QSize _size, QColor _color, QString _title, QString _text,
                                    bool _is_pinned, QWidget* parent)
    : NoteWindow(parent)
{
    id = _id;
    current_color = _color;
    is_pinned = _is_pinned;

    move(_pos);
    resize(_size);

    NoteWindow::set_title(_title.toStdString());
    NoteWindow::edit(_text.toStdString());

    apply_styles();
    if (is_pinned)
    {
        pin_btn->setIcon(sticky_note::note_icons::PIN_ACTIVE_ICON());
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
        if (size() == QSize(sticky_note::note_window::MIN_WIDTH, sticky_note::note_window::MIN_HEIGHT) || size().
            isEmpty())
        {
            resize(width, height);
        }
    }
    else
    {
        if (size() == QSize(sticky_note::note_window::MIN_WIDTH, sticky_note::note_window::MIN_HEIGHT) || size().
            isEmpty())
        {
            resize(_w, _h);
        }
    }

    if (title_label->text().isEmpty() || title_label->text() == sticky_note::note_window::TITLE)
    {
        set_title(_title);
    }
    SaveHandler::save_to_json({
        id, pos(), size(), current_color, title_label->text(), note_text, is_pinned
    });
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
    note_text = QString::fromStdString(_note);
    note_label->setMarkdown(to_view_markdown(note_text));
    note_edit->setPlainText(note_text);
}

void sticky_note::NoteWindow::save()
{
    note_text = note_edit->toPlainText();

    title_label->setText(title_edit->text());
    note_label->setMarkdown(to_view_markdown(note_text));

    title_edit->hide();
    title_label->show();

    note_edit->hide();
    note_label->show();

    edit_btn->setIcon(sticky_note::note_icons::PEN_ICON());

    setWindowTitle(title_label->text());
    SaveHandler::save_to_json({
        id, pos(), size(), current_color, title_label->text(), note_text, is_pinned
    });
}

void sticky_note::NoteWindow::download()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save Note",
                                                    QDir::homePath() + "/" + title_label->text() + ".json",
                                                    "JSON Files (*.json)");

    if (fileName.isEmpty())
        return;

    QJsonObject note_object;
    note_object["id"] = id.toString();
    note_object["x"] = pos().x();
    note_object["y"] = pos().y();
    note_object["w"] = size().width();
    note_object["h"] = size().height();
    note_object["color"] = current_color.name();
    note_object["title"] = title_label->text();
    note_object["text"] = note_text;
    note_object["is_pinned"] = is_pinned;

    QJsonDocument doc(note_object);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }
}

void sticky_note::NoteWindow::change_color(const QColor& color)
{
    current_color = color;
    apply_styles();
    SaveHandler::save_to_json({
        id, pos(), size(), current_color, title_label->text(), note_text, is_pinned
    });
}

void sticky_note::NoteWindow::apply_styles()
{
    const QString style = QString(
        "#NoteWindow { background: %1; border: 1px solid black; color: black; }"
        "QToolTip { color: black; background-color: %1; border: 1px solid #000000; padding: 6px; border-radius: 6px; }"
        "QLabel, QTextBrowser, QLineEdit, QTextEdit { color: black; }"
    ).arg(current_color.name());
    setStyleSheet(style);

    if (edit_btn) edit_btn->setHoverBackground(current_color.darker(110), 18);
    if (color_btn) color_btn->setHoverBackground(current_color.darker(110), 18);
    if (pin_btn) pin_btn->setHoverBackground(current_color.darker(110), 18);
    if (download_btn) download_btn->setHoverBackground(current_color.darker(110), 18);
}

void sticky_note::NoteWindow::enterEvent(QEnterEvent* event)
{
    quit_btn->setEnabled(true);
    edit_btn->setEnabled(true);
    color_btn->setEnabled(true);
    pin_btn->setEnabled(true);
    download_btn->setEnabled(true);
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
        download_btn->setEnabled(false);
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
            const int target_width = std::max(minimumWidth(), initial_geometry.width() - delta.x());
            const int new_left = initial_geometry.right() - target_width + 1;
            new_geometry.setLeft(new_left);
        }
        if (resize_edges & Qt::RightEdge)
        {
            const int target_width = std::max(minimumWidth(), initial_geometry.width() + delta.x());
            const int new_right = initial_geometry.left() + target_width - 1;
            new_geometry.setRight(new_right);
        }
        if (resize_edges & Qt::TopEdge)
        {
            const int target_height = std::max(minimumHeight(), initial_geometry.height() - delta.y());
            const int new_top = initial_geometry.bottom() - target_height + 1;
            new_geometry.setTop(new_top);
        }
        if (resize_edges & Qt::BottomEdge)
        {
            const int target_height = std::max(minimumHeight(), initial_geometry.height() + delta.y());
            const int new_bottom = initial_geometry.top() + target_height - 1;
            new_geometry.setBottom(new_bottom);
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
            bool over_checkbox = false;
            if (note_label->isVisible() && note_label->geometry().contains(event->pos()))
            {
                const QPoint labelPos = note_label->mapFromParent(event->pos());
                const QTextCursor cursor = note_label->cursorForPosition(labelPos);
                QRect charRect = note_label->cursorRect(cursor);

                // Check if the cursor is reasonably close to the character position
                if (std::abs(labelPos.x() - (charRect.x() + charRect.width() / 2)) < 35)
                {
                    QTextCursor selectCursor = cursor;
                    selectCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                    QString selectedText = selectCursor.selectedText();

                    if (selectedText == "☐" || selectedText == "☑")
                    {
                        over_checkbox = true;
                    }
                }
            }

            if (over_checkbox)
            {
                if (cursor().shape() != Qt::PointingHandCursor)
                    setCursor(Qt::PointingHandCursor);
            }
            else if (cursor().shape() != Qt::OpenHandCursor && !is_menu_active)
            {
                setCursor(Qt::OpenHandCursor);
            }
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

        if (note_label->isVisible() && note_label->geometry().contains(event->pos()))
        {
            const QPoint labelPos = note_label->mapFromParent(event->pos());
            const QTextCursor cursor = note_label->cursorForPosition(labelPos);
            const QRect charRect = note_label->cursorRect(cursor);

            // Check if the click is reasonably close to the cursor position
            const int center_x = charRect.x() + charRect.width() / 2;
            const int dx = std::abs(labelPos.x() - center_x);

            if (dx <= checkbox_check_area)
            {
                QTextCursor selectCursor = cursor;
                selectCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                QString selectedText = selectCursor.selectedText();

                if (selectedText == "☐" || selectedText == "☑" || selectedText == "☒")
                {
                    const QString newText = selectedText == "☐" ? "☑" : "☐";
                    selectCursor.insertText(newText);

                    // Update note_edit and save
                    note_edit->setPlainText(from_view_markdown(note_label->toMarkdown()));
                    SaveHandler::save_to_json({
                        id, pos(), size(), current_color, title_label->text(),
                        from_view_markdown(note_label->toMarkdown()),
                        is_pinned
                    });

                    event->accept();
                    return;
                }
            }
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
                id, pos(), size(), current_color, title_label->text(), note_text,
                is_pinned
            });
            event->accept();
        }
    }

    QWidget::mouseReleaseEvent(event);
}

void sticky_note::NoteWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

void sticky_note::NoteWindow::update_height()
{
    int content_height = 0;
    if (note_edit && !note_edit->isHidden())
    {
        content_height = note_edit->document()->size().height();
    }
    else if (note_label && !note_label->isHidden())
    {
        content_height = note_label->document()->size().height();
    }

    if (content_height > 0)
    {
        // Calculate the height occupied by other elements
        int non_content_height = layout->contentsMargins().top() + layout->contentsMargins().bottom();
        non_content_height += title_label->isHidden()
                                  ? title_edit->sizeHint().height()
                                  : title_label->sizeHint().height();
        non_content_height += layout->spacing();

        int target_height = content_height + non_content_height + 10; // Extra padding
        if (target_height < sticky_note::note_window::MIN_HEIGHT)
        {
            target_height = sticky_note::note_window::MIN_HEIGHT;
        }

        if (height() != target_height)
        {
            resize(width(), target_height);
        }
    }
}

void sticky_note::NoteWindow::update_font_sizes() const
{
    const int w = width();
    const int h = height();

    const int base_dim = std::min(w, h);

    const int title_size = std::max(12, base_dim / 10);
    const int regular_size = std::max(10, base_dim / 15);

    QFont title_font = title_label->font();
    title_font.setPointSize(title_size);
    title_label->setFont(title_font);
    title_edit->setFont(title_font);

    QFont regular_font = note_label->font();
    regular_font.setPointSize(regular_size);
    note_label->setFont(regular_font);
    note_edit->setFont(regular_font);
}

Qt::Edges sticky_note::NoteWindow::get_resize_edges(const QPoint& pos) const
{
    Qt::Edges edges = Qt::Edges();

    if (pos.x() < RESIZE_HANDLE_SIZE) edges |= Qt::LeftEdge;
    if (pos.x() > width() - RESIZE_HANDLE_SIZE) edges |= Qt::RightEdge;
    if (pos.y() < RESIZE_HANDLE_SIZE) edges |= Qt::TopEdge;
    if (pos.y() > height() - RESIZE_HANDLE_SIZE) edges |= Qt::BottomEdge;

    return edges;
}

QString sticky_note::NoteWindow::to_view_markdown(const QString& md)
{
    QString result = md;

    // Render Markdown-like checkboxes as symbols
    // Support "- [ ]", "* [ ]", "+ [ ]" and also "[ ]", "[]" at start of line
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)\\[ \\]", QRegularExpression::MultilineOption), "\\1☐");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)\\[\\]", QRegularExpression::MultilineOption), "\\1☐");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)\\[x\\]", QRegularExpression::MultilineOption), "\\1☑");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)\\[X\\]", QRegularExpression::MultilineOption), "\\1☒");

    // Preserve single newlines as visible line breaks in Markdown by converting to soft-breaks
    // Markdown collapses single newlines inside a paragraph; adding two spaces before a newline forces a break
    result.replace("\n", "  \n");

    return result;
}

QString sticky_note::NoteWindow::from_view_markdown(const QString& md)
{
    QString result = md;

    // Convert rendered checkbox symbols back to Markdown-like syntax
    // If it was a symbol with a bullet, it stays with a bullet.
    // If it was just "☐" (originally from "[]" or "[ ]"), it becomes "[ ]".
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)☐", QRegularExpression::MultilineOption), "\\1[ ]");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)☑", QRegularExpression::MultilineOption), "\\1[x]");
    result.replace(QRegularExpression("^(\\s*(?:[-*+] )?)☒", QRegularExpression::MultilineOption), "\\1[X]");

    // Revert soft-breaks (two spaces before newline) back to plain newlines for storage/editing
    result.replace("  \n", "\n");

    return result;
}
