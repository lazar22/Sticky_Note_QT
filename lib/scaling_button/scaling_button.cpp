//
// Created by roki on 2026-02-18.
//
#include "scaling_button.h"

sticky_note::ScalingButton::ScalingButton(QWidget* parent) : QPushButton(parent)
{
    animation = new QPropertyAnimation(this, "iconSize", this);
    animation->setDuration(160);
    setAttribute(Qt::WA_Hover, true);
    setAutoFillBackground(false);
    setStyleSheet("background: transparent; border: none;");
}

void sticky_note::ScalingButton::setBaseIconSize(const QSize& size)
{
    baseSize = size;
    setIconSize(baseSize);
}

void sticky_note::ScalingButton::setHoverBackground(const QColor& color, int radius_px)
{
    hoverBg = color;
    cornerRadius = radius_px;
    useHoverBg = true;
}


void sticky_note::ScalingButton::enterEvent(QEnterEvent* event)
{
    if (useHoverBg)
    {
        setStyleSheet(QString("background-color: %1; border-radius: %2px; border: none;")
                      .arg(hoverBg.name())
                      .arg(cornerRadius));
    }
    else
    {
        setStyleSheet("background: transparent; border: none;");
    }

    animation->stop();
    animation->setStartValue(iconSize());
    animation->setEndValue(QSize(static_cast<int>(baseSize.width() * 1.1),
                                 static_cast<int>(baseSize.height() * 1.1)));
    animation->start();
    QPushButton::enterEvent(event);
}

void sticky_note::ScalingButton::leaveEvent(QEvent* event)
{
    if (useHoverBg)
    {
        setStyleSheet(QString("background: transparent; border-radius: %1px; border: none;")
            .arg(cornerRadius));
    }
    else
    {
        setStyleSheet("background: transparent; border: none;");
    }

    animation->stop();
    animation->setStartValue(iconSize());
    animation->setEndValue(baseSize);
    animation->start();
    QPushButton::leaveEvent(event);
}
