//
// Created by roki on 2026-02-16.
//

#ifndef IWINDOW_H
#define IWINDOW_H

#include <string>

namespace sticky_note
{
    class IWindow
    {
    public:
        virtual void init(int _w, int _h, std::string _title) = 0;

        virtual ~IWindow() = default;

    public:
        virtual void set_title(std::string _title) = 0;

        virtual void show(bool is_note) = 0;

        // virtual void hide() = 0;
        //
        // virtual IWindow* clone() = 0;
        //
        // virtual void edit() = 0;
        //
        // virtual void close() = 0;
        //
        // virtual void change_color() = 0;
    };
}

#endif //IWINDOW_H
