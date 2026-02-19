//
// Created by roki on 2026-02-19.
//

#ifndef MENAGE_WINDOW_H
#define MENAGE_WINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "IWindow.h"

namespace sticky_note
{
    class MenageWindow : public QWidget, public IWindow
    {
        Q_OBJECT

        QPushButton* upload_btn;
        QPushButton* download_btn;

    public:
        explicit MenageWindow(QWidget* parent = nullptr);

        ~MenageWindow() override = default;

        void init(int _w, int _h, std::string _title) override;

        void set_title(std::string _title) override;

        void show(bool is_note) override;

        void edit(std::string _note) override
        {
        }

        void close() override;

        void enterEvent(QEnterEvent* event) override
        {
        }

        void leaveEvent(QEvent* event) override
        {
        }

        void mouseMoveEvent(QMouseEvent* event) override
        {
        }

        void mousePressEvent(QMouseEvent* event) override;

        void mouseReleaseEvent(QMouseEvent* event) override
        {
        }

    protected:
        void dragEnterEvent(QDragEnterEvent* event) override;

        void dropEvent(QDropEvent* event) override;

    private slots :
        void upload_notes();

        void download_notes();

    private:
        void process_json_file(const QString& filePath);
    };
}

#endif //MENAGE_WINDOW_H
