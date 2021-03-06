// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/PlaybackSettings.h>

#include <djvCore/Context.h>
#include <djvCore/Speed.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSettings::Private
        {
            std::shared_ptr<ValueSubject<bool> > startPlayback;
            std::shared_ptr<ValueSubject<PlaybackSpeed> > playbackSpeed;
            std::shared_ptr<ValueSubject<Time::Speed> > customSpeed;
            std::shared_ptr<ValueSubject<bool> > playEveryFrame;
            std::shared_ptr<ValueSubject<PlaybackMode> > playbackMode;
            std::shared_ptr<ValueSubject<bool> > pip;
        };

        void PlaybackSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::PlaybackSettings", context);

            DJV_PRIVATE_PTR();
            p.startPlayback = ValueSubject<bool>::create(false);
            p.playbackSpeed = ValueSubject<PlaybackSpeed>::create(PlaybackSpeed::Default);
            p.customSpeed = ValueSubject<Time::Speed>::create(Time::Speed(1));
            p.playEveryFrame = ValueSubject<bool>::create(false);
            p.playbackMode = ValueSubject<PlaybackMode>::create(PlaybackMode::Loop);
            p.pip = ValueSubject<bool>::create(true);
            _load();
        }

        PlaybackSettings::PlaybackSettings() :
            _p(new Private)
        {}

        std::shared_ptr<PlaybackSettings> PlaybackSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<PlaybackSettings>(new PlaybackSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<bool> > PlaybackSettings::observeStartPlayback() const
        {
            return _p->startPlayback;
        }

        void PlaybackSettings::setStartPlayback(bool value)
        {
            _p->startPlayback->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<PlaybackSpeed> > PlaybackSettings::observePlaybackSpeed() const
        {
            return _p->playbackSpeed;
        }

        void PlaybackSettings::setPlaybackSpeed(PlaybackSpeed value)
        {
            _p->playbackSpeed->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<Time::Speed> > PlaybackSettings::observeCustomSpeed() const
        {
            return _p->customSpeed;
        }

        void PlaybackSettings::setCustomSpeed(const Time::Speed& value)
        {
            _p->customSpeed->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > PlaybackSettings::observePlayEveryFrame() const
        {
            return _p->playEveryFrame;
        }

        void PlaybackSettings::setPlayEveryFrame(bool value)
        {
            _p->playEveryFrame->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<PlaybackMode> > PlaybackSettings::observePlaybackMode() const
        {
            return _p->playbackMode;
        }

        void PlaybackSettings::setPlaybackMode(PlaybackMode value)
        {
            _p->playbackMode->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > PlaybackSettings::observePIP() const
        {
            return _p->pip;
        }

        void PlaybackSettings::setPIP(bool value)
        {
            _p->pip->setIfChanged(value);
        }

        void PlaybackSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("StartPlayback", object, p.startPlayback);
                UI::Settings::read("PlaybackSpeed", object, p.playbackSpeed);
                UI::Settings::read("CustomSpeed", object, p.customSpeed);
                UI::Settings::read("PlayEveryFrame", object, p.playEveryFrame);
                UI::Settings::read("PlaybackMode", object, p.playbackMode);
                UI::Settings::read("PIP", object, p.pip);
            }
        }

        picojson::value PlaybackSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("StartPlayback", p.startPlayback->get(), object);
            UI::Settings::write("PlaybackSpeed", p.playbackSpeed->get(), object);
            UI::Settings::write("CustomSpeed", p.customSpeed->get(), object);
            UI::Settings::write("PlayEveryFrame", p.playEveryFrame->get(), object);
            UI::Settings::write("PlaybackMode", p.playbackMode->get(), object);
            UI::Settings::write("PIP", p.pip->get(), object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

