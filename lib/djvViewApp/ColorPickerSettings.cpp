// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorPickerSettings.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerSettings::Private
        {
            size_t sampleSize = 1;
            AV::Image::Type typeLock = AV::Image::Type::None;
            glm::vec2 pickerPos = glm::vec2(0.F, 0.F);
            std::map<std::string, BBox2f> widgetGeom;
        };

        void ColorPickerSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ColorPickerSettings", context);
            _load();
        }

        ColorPickerSettings::ColorPickerSettings() :
            _p(new Private)
        {}

        ColorPickerSettings::~ColorPickerSettings()
        {}

        std::shared_ptr<ColorPickerSettings> ColorPickerSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ColorPickerSettings>(new ColorPickerSettings);
            out->_init(context);
            return out;
        }

        size_t ColorPickerSettings::getSampleSize() const
        {
            return _p->sampleSize;
        }

        AV::Image::Type ColorPickerSettings::getTypeLock() const
        {
            return _p->typeLock;
        }

        const glm::vec2& ColorPickerSettings::getPickerPos() const
        {
            return _p->pickerPos;
        }

        void ColorPickerSettings::setSampleSize(size_t value)
        {
            _p->sampleSize = value;
        }

        void ColorPickerSettings::setTypeLock(AV::Image::Type value)
        {
            _p->typeLock = value;
        }

        void ColorPickerSettings::setPickerPos(const glm::vec2& value)
        {
            _p->pickerPos = value;
        }

        const std::map<std::string, BBox2f>& ColorPickerSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ColorPickerSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ColorPickerSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("sampleSize", object, p.sampleSize);
                UI::Settings::read("typeLock", object, p.typeLock);
                UI::Settings::read("pickerPos", object, p.pickerPos);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value ColorPickerSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("sampleSize", p.sampleSize, object);
            UI::Settings::write("typeLock", p.typeLock, object);
            UI::Settings::write("pickerPos", p.pickerPos, object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

