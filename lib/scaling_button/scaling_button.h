#ifndef SCALING_BUTTON_H
#define SCALING_BUTTON_H

#include <QPushButton>
#include <QPropertyAnimation>
#include <QEnterEvent>
#include <QColor>

namespace sticky_note
{
    class ScalingButton : public QPushButton
    {
        Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)

        QSize baseSize{24, 24};
        QPropertyAnimation* animation{nullptr};
        QColor hoverBg{Qt::transparent};

        int cornerRadius{0};
        bool useHoverBg{false};

    public:
        explicit ScalingButton(QWidget* parent = nullptr);


        void setBaseIconSize(const QSize& size);

        void setHoverBackground(const QColor& color, int radius_px);

    protected:
        void enterEvent(QEnterEvent* event) override;

        void leaveEvent(QEvent* event) override;
    };
}

#endif // SCALING_BUTTON_H
