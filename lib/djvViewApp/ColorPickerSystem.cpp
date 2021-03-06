// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorPickerSystem.h>

#include <djvViewApp/ColorPickerSettings.h>
#include <djvViewApp/ColorPickerWidget.h>

#include <djvUI/Action.h>
#include <djvUI/Shortcut.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerSystem::Private
        {
            std::shared_ptr<ColorPickerSettings> settings;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::weak_ptr<ColorPickerWidget> widget;
        };

        void ColorPickerSystem::_init(const std::shared_ptr<Context>& context)
        {
            IToolSystem::_init("djv::ViewApp::ColorPickerSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = ColorPickerSettings::create(context);
            _setWidgetGeom(p.settings->getWidgetGeom());

            p.actions["ColorPicker"] = UI::Action::create();
            p.actions["ColorPicker"]->setIcon("djvIconColorPicker");
            p.actions["ColorPicker"]->setShortcut(GLFW_KEY_K, UI::Shortcut::getSystemModifier());

            _textUpdate();
        }

        ColorPickerSystem::ColorPickerSystem() :
            _p(new Private)
        {}

        ColorPickerSystem::~ColorPickerSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("ColorPicker");
            p.settings->setWidgetGeom(_getWidgetGeom());
        }

        std::shared_ptr<ColorPickerSystem> ColorPickerSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorPickerSystem>(new ColorPickerSystem);
            out->_init(context);
            return out;
        }

        ToolActionData ColorPickerSystem::getToolAction() const
        {
            return
            {
                _p->actions["ColorPicker"],
                "B"
            };
        }

        void ColorPickerSystem::setCurrentTool(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value)
            {
                if (auto context = getContext().lock())
                {
                    auto widget = ColorPickerWidget::create(context);
                    widget->setSampleSize(p.settings->getSampleSize());
                    widget->setTypeLock(p.settings->getTypeLock());
                    widget->setPickerPos(p.settings->getPickerPos());
                    auto weak = std::weak_ptr<ColorPickerSystem>(std::dynamic_pointer_cast<ColorPickerSystem>(shared_from_this()));
                    p.widget = widget;
                    _openWidget("ColorPicker", widget);
                }
            }
            else
            {
                _closeWidget("ColorPicker");
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ColorPickerSystem::getActions() const
        {
            return _p->actions;
        }

        void ColorPickerSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            if (auto widget = p.widget.lock())
            {
                p.settings->setSampleSize(widget->getSampleSize());
                p.settings->setTypeLock(widget->getTypeLock());
                p.settings->setPickerPos(widget->getPickerPos());
                p.widget.reset();
            }
            IToolSystem::_closeWidget(value);
        }

        void ColorPickerSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.actions.size())
            {
                p.actions["ColorPicker"]->setText(_getText(DJV_TEXT("menu_tools_color_picker")));
                p.actions["ColorPicker"]->setTooltip(_getText(DJV_TEXT("menu_color_picker_tooltip")));
            }
        }
        
    } // namespace ViewApp
} // namespace djv

