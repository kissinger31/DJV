// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Widget.h>

#include <djvUI/Action.h>
#include <djvUI/EventSystem.h>
#include <djvUI/ITooltipWidget.h>
#include <djvUI/Shortcut.h>
#include <djvUI/Style.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Tooltip.h>
#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Math.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo Should this be configurable?
            const Time::Duration tooltipTimeout = std::chrono::milliseconds(500);
            const float tooltipHideDelta = 1.F;

            size_t globalWidgetCount = 0;

            class DefaultTooltipWidget : public ITooltipWidget
            {
            protected:
                void _init(const std::shared_ptr<Context>& context)
                {
                    ITooltipWidget::_init(context);
                    _textBlock = TextBlock::create(context);
                    _textBlock->setTextColorRole(ColorRole::TooltipForeground);
                    _textBlock->setBackgroundRole(ColorRole::TooltipBackground);
                    _textBlock->setMargin(Layout::Margin(MetricsRole::Margin));
                    addChild(_textBlock);
                }

            public:
                static std::shared_ptr<DefaultTooltipWidget> create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<DefaultTooltipWidget>(new DefaultTooltipWidget);
                    out->_init(context);
                    return out;
                }

                void setTooltip(const std::string& value) override
                {
                    _textBlock->setText(value);
                }

                float getHeightForWidth(float value) const override
                {
                    return _textBlock->getHeightForWidth(value);
                }

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override
                {
                    _setMinimumSize(_textBlock->getMinimumSize());
                }

                void _layoutEvent(Core::Event::Layout&) override
                {
                    _textBlock->setGeometry(getGeometry());
                }

            private:
                std::shared_ptr<TextBlock> _textBlock;
            };

        } // namespace

        std::chrono::steady_clock::time_point Widget::_updateTime;
        bool Widget::_tooltipsEnabled = true;
        bool Widget::_resizeRequest   = true;
        bool Widget::_redrawRequest   = true;

        void Widget::_init(const std::shared_ptr<Context>& context)
        {
            IObject::_init(context);
            
            setClassName("djv::UI::Widget");

            ++globalWidgetCount;

            _eventSystem = context->getSystemT<EventSystem>();
            _render = context->getSystemT<AV::Render2D::Render>();
            auto uiSystem = context->getSystemT<UISystem>();
            _style = uiSystem->getStyle();
        }

        Widget::~Widget()
        {
            --globalWidgetCount;
        }
        
        std::shared_ptr<Widget> Widget::create(const std::shared_ptr<Context>& context)
        {
            //! \bug It would be prefereable to use std::make_shared() here, but how can we do that
            //! with protected contructors?
            auto out = std::shared_ptr<Widget>(new Widget);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Window> Widget::getWindow() const
        {
            return getParentRecursiveT<Window>();
        }

        void Widget::setVisible(bool value)
        {
            if (value == _visible)
                return;
            _visible = value;
            _visibleInit = value;
            _resize();
        }

        void Widget::setOpacity(float value)
        {
            if (value == _opacity)
                return;
            _opacity = value;
            _resize();
        }

        void Widget::setGeometry(const BBox2f & value)
        {
            if (value == _geometry)
                return;
            _geometry = value;
            _resize();
        }

        void Widget::setMargin(const Layout::Margin & value)
        {
            if (value == _margin)
                return;
            _margin = value;
            _resize();
        }

        void Widget::setHAlign(HAlign value)
        {
            if (value == _hAlign)
                return;
            _hAlign = value;
            _resize();
        }

        void Widget::setVAlign(VAlign value)
        {
            if (value == _vAlign)
                return;
            _vAlign = value;
            _resize();
        }

        BBox2f Widget::getAlign(const BBox2f & value, const glm::vec2 & minimumSize, HAlign hAlign, VAlign vAlign)
        {
            float x = 0.F;
            float y = 0.F;
            float w = 0.F;
            float h = 0.F;
            switch (hAlign)
            {
            case HAlign::Center:
                w = minimumSize.x;
                x = value.min.x + value.w() / 2.F - minimumSize.x / 2.F;
                break;
            case HAlign::Left:
                x = value.min.x;
                w = minimumSize.x;
                break;
            case HAlign::Right:
                w = minimumSize.x;
                x = value.min.x + value.w() - minimumSize.x;
                break;
            case HAlign::Fill:
                x = value.min.x;
                w = value.w();
                break;
            default: break;
            }
            switch (vAlign)
            {
            case VAlign::Center:
                h = minimumSize.y;
                y = value.min.y + value.h() / 2.F - minimumSize.y / 2.F;
                break;
            case VAlign::Top:
                y = value.min.y;
                h = minimumSize.y;
                break;
            case VAlign::Bottom:
                h = minimumSize.y;
                y = value.min.y + value.h() - minimumSize.y;
                break;
            case VAlign::Fill:
                y = value.min.y;
                h = value.h();
                break;
            default: break;
            }
            return BBox2f(floorf(x), floorf(y), ceilf(w), ceilf(h));
        }

        void Widget::setBackgroundRole(ColorRole value)
        {
            if (value == _backgroundRole)
                return;
            _backgroundRole = value;
            _redraw();
        }

        void Widget::setShadowOverlay(const std::vector<Side>& value)
        {
            if (value == _shadowOverlay)
                return;
            _shadowOverlay = value;
            _redraw();
        }

        void Widget::setPointerEnabled(bool value)
        {
            _pointerEnabled = value;
        }

        std::shared_ptr<Widget> Widget::getFocusWidget()
        {
            return std::dynamic_pointer_cast<Widget>(shared_from_this());
        }

        bool Widget::acceptFocus(TextFocusDirection direction)
        {
            bool out = false;
            if (isEnabled(true) && isVisible(true) && !isClipped())
            {
                const auto& childWidgets = getChildWidgets();
                const size_t size = childWidgets.size();
                switch (direction)
                {
                case TextFocusDirection::Next:
                    for (size_t i = 0; i < size; ++i)
                    {
                        if (childWidgets[i]->acceptFocus(direction))
                        {
                            out = true;
                            break;
                        }
                    }
                    break;
                case TextFocusDirection::Prev:
                    for (int i = static_cast<int>(size) - 1; i >= 0; --i)
                    {
                        if (childWidgets[i]->acceptFocus(direction))
                        {
                            out = true;
                            break;
                        }
                    }
                    break;
                default: break;
                }
            }
            return out;
        }

        bool Widget::hasTextFocus()
        {
            bool out = false;
            if (auto eventSystem = _getEventSystem().lock())
            {
                out = eventSystem->getTextFocus().lock() == getFocusWidget();
            }
            return out;
        }

        void Widget::takeTextFocus()
        {
            if (auto eventSystem = _getEventSystem().lock())
            {
                eventSystem->setTextFocus(getFocusWidget());
            }
        }

        void Widget::releaseTextFocus()
        {
            if (auto eventSystem = _getEventSystem().lock())
            {
                if (eventSystem->getTextFocus().lock() == getFocusWidget())
                {
                    eventSystem->setTextFocus(nullptr);
                }
            }
        }

        void Widget::nextTextFocus(const std::shared_ptr<Widget>& widget)
        {
            // Find the next sibling that can accept focus.
            const auto& childWidgets = getChildWidgets();
            const int size = static_cast<int>(childWidgets.size());
            bool accepted = false;
            auto i = std::find_if(
                childWidgets.begin(),
                childWidgets.end(),
                [widget](const std::shared_ptr<Widget>& value)
                {
                    return widget && widget->getFocusWidget() == value->getFocusWidget();
                });
            if (i != childWidgets.end())
            {
                for (int j = static_cast<int>(i - childWidgets.begin()) + 1; j < size; ++j)
                {
                    if (childWidgets[j]->getFocusWidget()->acceptFocus(TextFocusDirection::Next))
                    {
                        accepted = true;
                        break;
                    }
                }
            }

            if (!accepted)
            {
                if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                {
                    // Try the parent widget.
                    parent->nextTextFocus(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                }
                else
                {
                    // Try the child widgets.
                    for (int j = 0; j < size; ++j)
                    {
                        if (childWidgets[j]->getFocusWidget()->acceptFocus(TextFocusDirection::Next))
                        {
                            break;
                        }
                    }
                }
            }
        }

        void Widget::prevTextFocus(const std::shared_ptr<Widget>& widget)
        {
            // Find the previous sibling that can accept focus.
            const auto& childWidgets = getChildWidgets();
            const int size = static_cast<int>(childWidgets.size());
            bool accepted = false;
            auto i = std::find_if(
                childWidgets.begin(),
                childWidgets.end(),
                [widget](const std::shared_ptr<Widget>& value)
                {
                    return widget && widget->getFocusWidget() == value->getFocusWidget();
                });
            if (i != childWidgets.end())
            {
                for (int j = static_cast<int>(i - childWidgets.begin()) - 1; j >= 0; --j)
                {
                    if (childWidgets[j]->getFocusWidget()->acceptFocus(TextFocusDirection::Prev))
                    {
                        accepted = true;
                        break;
                    }
                }
            }
            if (!accepted)
            {
                if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                {
                    // Try the parent widget.
                    parent->prevTextFocus(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                }
                else
                {
                    // Try the child widgets.
                    for (int j = size - 1; j >= 0; --j)
                    {
                        if (childWidgets[j]->getFocusWidget()->acceptFocus(TextFocusDirection::Prev))
                        {
                            break;
                        }
                    }
                }
            }
        }

        void Widget::addAction(const std::shared_ptr<Action> & action)
        {
            _actions.push_back(action);
        }

        void Widget::removeAction(const std::shared_ptr<Action> & action)
        {
            const auto i = std::find(_actions.begin(), _actions.end(), action);
            if (i != _actions.end())
            {
                _actions.erase(i);
            }
        }

        void Widget::clearActions()
        {
            _actions.clear();
        }

        void Widget::setTooltip(const std::string & value)
        {
            _tooltipText = value;
            for (auto& i : _pointerToTooltips)
            {
                if (auto tooltip = i.second.tooltip)
                {
                    tooltip->getWidget()->setTooltip(_tooltipText);
                }
            }
            setPointerEnabled(true);
        }

        void Widget::setTooltipsEnabled(bool value)
        {
            _tooltipsEnabled = value;
        }

        size_t Widget::getGlobalWidgetCount()
        {
            return globalWidgetCount;
        }

        void Widget::moveToFront()
        {
            IObject::moveToFront();
            if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
            {
                auto object = std::dynamic_pointer_cast<Widget>(shared_from_this());
                auto& siblings = parent->_childWidgets;
                const auto i = std::find(siblings.begin(), siblings.end(), object);
                if (i != siblings.end())
                {
                    siblings.erase(i);
                }
                siblings.push_back(object);
            }
        }

        void Widget::moveToBack()
        {
            IObject::moveToBack();
            if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
            {
                auto object = std::dynamic_pointer_cast<Widget>(shared_from_this());
                auto& siblings = parent->_childWidgets;
                const auto i = std::find(siblings.begin(), siblings.end(), object);
                if (i != siblings.end())
                {
                    siblings.erase(i);
                }
                siblings.insert(siblings.begin(), object);
            }
        }

        void Widget::setEnabled(bool value)
        {
            IObject::setEnabled(value);
            if (!value)
            {
                releaseTextFocus();
            }
        }

        bool Widget::event(Event::Event & event)
        {
            bool out = IObject::event(event);
            if (!out)
            {
                switch (event.getEventType())
                {
                case Event::Type::ParentChanged:
                {
                    auto& parentChangedEvent = static_cast<Event::ParentChanged &>(event);
                    const bool newParent = parentChangedEvent.getNewParent() ? true : false;
                    if (!newParent)
                    {
                        if (auto eventSystem = _getEventSystem().lock())
                        {
                            auto parent = shared_from_this();
                            auto widget = eventSystem->getTextFocus().lock();
                            while (widget)
                            {
                                if (widget == parent)
                                {
                                    eventSystem->setTextFocus(nullptr);
                                    break;
                                }
                                widget = widget->getParent().lock();
                            }
                        }
                    }
                    _clipped = newParent;
                    _clipRect = BBox2f(0.F, 0.F, 0.F, 0.F);
                    _redraw();
                    break;
                }
                case Event::Type::ChildAdded:
                {
                    auto& childAddedEvent = static_cast<Event::ChildAdded&>(event);
                    if (auto widget = std::dynamic_pointer_cast<Widget>(childAddedEvent.getChild()))
                    {
                        const auto i = std::find(_childWidgets.begin(), _childWidgets.end(), widget);
                        if (i != _childWidgets.end())
                        {
                            _childWidgets.erase(i);
                        }
                        _childWidgets.push_back(widget);
                    }
                    _resize();
                    break;
                }
                case Event::Type::ChildRemoved:
                {
                    auto& childRemovedEvent = static_cast<Event::ChildRemoved&>(event);
                    if (auto widget = std::dynamic_pointer_cast<Widget>(childRemovedEvent.getChild()))
                    {
                        const auto i = std::find(_childWidgets.begin(), _childWidgets.end(), widget);
                        if (i != _childWidgets.end())
                        {
                            _childWidgets.erase(i);
                        }
                    }
                    _resize();
                    break;
                }
                case Event::Type::ChildOrder:
                case Event::Type::Init:
                    _resize();
                    break;
                case Event::Type::Update:
                {
                    auto & updateEvent = static_cast<Event::Update &>(event);
                    _updateTime = updateEvent.getTime();

                    if (auto context = getContext().lock())
                    {
                        for (auto & i : _pointerToTooltips)
                        {
                            const auto j = _pointerHover.find(i.first);
                            const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(_updateTime - i.second.timer);
                            const auto& g = getGeometry();
                            if (_tooltipsEnabled &&
                                t > tooltipTimeout &&
                                !i.second.tooltip &&
                                j != _pointerHover.end() &&
                                g.contains(j->second))
                            {
                                for (
                                    auto widget = std::dynamic_pointer_cast<Widget>(shared_from_this());
                                    widget;
                                    widget = std::dynamic_pointer_cast<Widget>(widget->getParent().lock()))
                                {
                                    if (auto tooltipWidget = widget->_createTooltip(j->second))
                                    {
                                        i.second.tooltip = Tooltip::create(getWindow(), j->second, tooltipWidget, context);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                case Event::Type::InitLayout:
                    _visibleInit = false;
                    _initLayoutEvent(static_cast<Event::InitLayout&>(event));
                    break;
                case Event::Type::PreLayout:
                    _preLayoutEvent(static_cast<Event::PreLayout &>(event));
                    break;
                case Event::Type::Layout:
                    _layoutEvent(static_cast<Event::Layout&>(event));
                    break;
                case Event::Type::Clip:
                {
                    auto& clipEvent = static_cast<Event::Clip &>(event);
                    if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                    {
                        _parentsVisible = parent->_visible && parent->_parentsVisible;
                        _clipped =
                            !clipEvent.getClipRect().isValid() ||
                            !_visible ||
                            !_parentsVisible ||
                            0.F == _opacity ||
                            0.F == _parentsOpacity;
                        _clipRect = clipEvent.getClipRect();
                    }
                    else
                    {
                        _parentsVisible = true;
                        _clipped = false;
                        _clipRect = BBox2f(0.F, 0.F, 0.F, 0.F);
                    }
                    if (_clipped)
                    {
                        for (auto& i : _pointerToTooltips)
                        {
                            i.second.tooltip.reset();
                            i.second.timer = _updateTime;
                        }
                        releaseTextFocus();
                    }
                    _clipEvent(clipEvent);
                    break;
                }
                case Event::Type::Paint:
                {
                    if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                    {
                        _parentsOpacity = parent->_opacity * parent->_parentsOpacity;
                    }
                    else
                    {
                        _parentsOpacity = 1.F;
                    }
                    if (!_visibleInit)
                    {
                        float opacity = getOpacity(true);
                        opacity *= isEnabled(true) ? 1.F : _style->getPalette().getDisabledMult();
                        _render->setAlphaMult(opacity);
                        _paintEvent(static_cast<Event::Paint &>(event));
                    }
                    break;
                }
                case Event::Type::PaintOverlay:
                {
                    if (!_visibleInit)
                    {
                        _paintOverlayEvent(static_cast<Event::PaintOverlay&>(event));
                    }
                    break;
                }
                case Event::Type::PointerEnter:
                {
                    auto & pointerEvent = static_cast<Event::PointerEnter &>(event);
                    const auto & info = pointerEvent.getPointerInfo();
                    const auto id = info.id;
                    _pointerHover[id] = info.projectedPos;
                    _pointerToTooltips[id] = TooltipData();
                    _pointerToTooltips[id].timer = _updateTime;
                    _pointerEnterEvent(static_cast<Event::PointerEnter &>(event));
                    break;
                }
                case Event::Type::PointerLeave:
                {
                    auto & pointerEvent = static_cast<Event::PointerLeave &>(event);
                    const auto id = pointerEvent.getPointerInfo().id;
                    const auto i = _pointerHover.find(id);
                    if (i != _pointerHover.end())
                    {
                        _pointerHover.erase(i);
                    }
                    const auto j = _pointerToTooltips.find(id);
                    if (j != _pointerToTooltips.end())
                    {
                        _pointerToTooltips.erase(j);
                    }
                    _pointerLeaveEvent(static_cast<Event::PointerLeave &>(event));
                    break;
                }
                case Event::Type::PointerMove:
                {
                    auto & pointerEvent = static_cast<Event::PointerMove &>(event);
                    const auto & info = pointerEvent.getPointerInfo();
                    const auto id = info.id;
                    const auto i = _pointerToTooltips.find(id);
                    if (i != _pointerToTooltips.end())
                    {
                        const auto delta = info.projectedPos - _pointerHover[id];
                        const float l = glm::length(delta);
                        if (l > tooltipHideDelta)
                        {
                            i->second.tooltip.reset();
                            i->second.timer = _updateTime;
                        }
                    }
                    _pointerHover[id] = info.projectedPos;
                    _pointerMoveEvent(static_cast<Event::PointerMove &>(event));
                    break;
                }
                case Event::Type::ButtonPress:
                    _buttonPressEvent(static_cast<Event::ButtonPress &>(event));
                    break;
                case Event::Type::ButtonRelease:
                    _buttonReleaseEvent(static_cast<Event::ButtonRelease &>(event));
                    break;
                case Event::Type::Scroll:
                    _scrollEvent(static_cast<Event::Scroll &>(event));
                    break;
                case Event::Type::Drop:
                    _dropEvent(static_cast<Event::Drop &>(event));
                    break;
                case Event::Type::KeyPress:
                    _keyPressEvent(static_cast<Event::KeyPress &>(event));
                    break;
                case Event::Type::KeyRelease:
                    _keyReleaseEvent(static_cast<Event::KeyRelease &>(event));
                    break;
                case Event::Type::TextFocus:
                    _textFocusEvent(static_cast<Event::TextFocus &>(event));
                    break;
                case Event::Type::TextFocusLost:
                    _textFocusLostEvent(static_cast<Event::TextFocusLost &>(event));
                    break;
                case Event::Type::TextInput:
                    _textInputEvent(static_cast<Event::TextInput&>(event));
                    break;
                default: break;
                }
                out = event.isAccepted();
            }
            return out;
        }

        void Widget::_paintEvent(Event::Paint& event)
        {
            if (_backgroundRole != ColorRole::None)
            {
                _render->setFillColor(_style->getColor(_backgroundRole));
                _render->drawRect(getGeometry());
            }
        }

        void Widget::_paintOverlayEvent(Event::PaintOverlay& event)
        {
            if (_shadowOverlay.size())
            {
                const auto& style = _getStyle();
                const float ss = style->getMetric(MetricsRole::ShadowSmall);
                const BBox2f& g = getGeometry();
                _render->setFillColor(_style->getColor(ColorRole::Shadow));
                for (const auto& i : _shadowOverlay)
                {
                    switch (i)
                    {
                    case Side::Left:
                        _render->drawShadow(BBox2f(g.min.x, g.min.y, ss, g.h()), AV::Side::Right);
                        break;
                    case Side::Right:
                        _render->drawShadow(BBox2f(g.max.x - ss, g.min.y, ss, g.h()), AV::Side::Left);
                        break;
                    case Side::Top:
                        _render->drawShadow(BBox2f(g.min.x, g.min.y, g.w(), ss), AV::Side::Bottom);
                        break;
                    case Side::Bottom:
                        _render->drawShadow(BBox2f(g.min.x, g.max.y - ss, g.w(), ss), AV::Side::Top);
                        break;
                    default: break;
                    }
                }
            }
        }

        void Widget::_pointerEnterEvent(Event::PointerEnter & event)
        {
            if (_pointerEnabled && !event.isRejected())
            {
                event.accept();
            }
        }

        void Widget::_pointerLeaveEvent(Event::PointerLeave & event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::_pointerMoveEvent(Event::PointerMove & event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::_keyPressEvent(Event::KeyPress & event)
        {
            switch (event.getKey())
            {
            case GLFW_KEY_TAB:
                if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                {
                    event.accept();
                    auto widget = std::dynamic_pointer_cast<Widget>(shared_from_this());
                    if (event.getKeyModifiers() & GLFW_MOD_SHIFT)
                    {
                        parent->prevTextFocus(widget);
                    }
                    else
                    {
                        parent->nextTextFocus(widget);
                    }
                }
                else
                {
                    if (event.getKeyModifiers() & GLFW_MOD_SHIFT)
                    {
                        prevTextFocus(nullptr);
                    }
                    else
                    {
                        nextTextFocus(nullptr);
                    }
                }
                break;
            default:
                // Find the shortcuts.
                std::vector<std::shared_ptr<Shortcut> > shortcuts;
                for (const auto& i : _actions)
                {
                    if (i->observeEnabled()->get())
                    {
                        for (auto j : i->observeShortcuts()->get())
                        {
                            shortcuts.push_back(j);
                        }
                    }
                }

                // Sort the actions so that we test those with keyboard modifiers first.
                std::sort(shortcuts.begin(), shortcuts.end(),
                    [](const std::shared_ptr<Shortcut>& a, const std::shared_ptr<Shortcut>& b)
                    {
                        return a->observeShortcutModifiers()->get() > b->observeShortcutModifiers()->get();
                    });

                for (const auto& i : shortcuts)
                {
                    const int key = i->observeShortcutKey()->get();
                    const int modifiers = i->observeShortcutModifiers()->get();
                    if ((key == event.getKey() && event.getKeyModifiers() == modifiers) ||
                        (key == event.getKey() && modifiers == 0 && event.getKeyModifiers() == 0))
                    {
                        event.accept();
                        i->doCallback();
                        break;
                    }
                }
                break;
            }
        }

        void Widget::_setMinimumSize(const glm::vec2& value)
        {
            if (value == _minimumSize)
                return;
            _minimumSize = value;
            _resize();
        }

        void Widget::_setDesiredSize(const glm::vec2& value)
        {
            if (value == _desiredSize)
                return;
            _desiredSize = value;
            _resize();
        }

        std::string Widget::_getTooltipText() const
        {
            std::stringstream out;
            if (!_tooltipText.empty())
            {
                out << _tooltipText;
            }
            else
            {
                for (const auto & action : _actions)
                {
                    const auto & actionTooltip = action->observeTooltip()->get();
                    if (!actionTooltip.empty())
                    {
                        out << actionTooltip;
                        break;
                    }
                }
            }
            if (_actions.size())
            {
                const auto action = _actions[0];
                const auto& shortcuts = action->observeShortcuts()->get();
                const size_t shortcutsSize = shortcuts.size();
                if (shortcutsSize)
                {
                    out << "\n\n";
                    out << "(";
                    out << _getText(1 == shortcutsSize ?
                        DJV_TEXT("keyboard_shortcut") :
                        DJV_TEXT("keyboard_shortcuts"));
                    out << ": ";
                    std::vector<std::string> shortcutsText;
                    const auto& textSystem = _getTextSystem();
                    for (const auto& shortcut : action->observeShortcuts()->get())
                    {
                        shortcutsText.push_back(Shortcut::getText(shortcut, textSystem));
                    }
                    out << String::join(shortcutsText, ", ");
                    out << ")";
                }
            }
            return out.str();
        }

        std::shared_ptr<ITooltipWidget> Widget::_createTooltipDefault()
        {
            std::shared_ptr<ITooltipWidget> out;
            if (auto context = getContext().lock())
            {
                out = DefaultTooltipWidget::create(context);
            }
            return out;
        }

        std::shared_ptr<ITooltipWidget> Widget::_createTooltip(const glm::vec2 &)
        {
            std::shared_ptr<ITooltipWidget> out;
            if (!_tooltipText.empty())
            {
                out = _createTooltipDefault();
                out->setTooltip(_getTooltipText());
            }
            return out;
        }

    } // namespace UI
} // namespace djv
