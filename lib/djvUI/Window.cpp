// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Window.h>

#include <djvUI/StackLayout.h>

#include <djvCore/Context.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Window::Private
        {
            bool closed = false;
        };

        void Window::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::UI::Window");
            setVisible(false);
            setBackgroundRole(ColorRole::Background);
            setPointerEnabled(true);
        }

        Window::Window() :
            _p(new Private)
        {}

        Window::~Window()
        {}

        std::shared_ptr<Window> Window::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<Window>(new Window);
            out->_init(context);
            return out;
        }

        bool Window::isClosed() const
        {
            return _p->closed;
        }

        void Window::close()
        {
            DJV_PRIVATE_PTR();
            p.closed = true;
            hide();
            moveToBack();
        }

        void Window::_preLayoutEvent(Event::PreLayout &)
        {
            _setMinimumSize(StackLayout::minimumSize(getChildWidgets(), Layout::Margin(), _getStyle()));
        }

        void Window::_layoutEvent(Event::Layout &)
        {
            StackLayout::layout(getGeometry(), getChildWidgets(), Layout::Margin(), _getStyle());
        }

    } // namespace UI
} // namespace djv
