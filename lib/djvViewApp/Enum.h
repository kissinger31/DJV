//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvCore/Enum.h>
#include <djvCore/PicoJSON.h>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            class Speed;

        } // namespace Time
    } // namespace Core

    namespace ViewApp
    {
        enum class ImageViewLock
        {
            None,
            Fill,
            Frame,
            Center,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(ImageViewLock);

        enum class ImageViewGridLabels
        {
            None,
            X_Y,
            A_Y,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(ImageViewGridLabels);

        enum class ScrollWheelZoomSpeed
        {
            Slow,
            Medium,
            Fast,

            Count,
            First = Slow
        };
        DJV_ENUM_HELPERS(ScrollWheelZoomSpeed);
        float getScrollWheelZoomSpeed(ScrollWheelZoomSpeed);

        enum class Playback
        {
            Stop,
            Forward,
            Reverse,

            Count,
            First = Stop
        };
        DJV_ENUM_HELPERS(Playback);

        enum class PlaybackSpeed
        {
            Default,
            Custom,
            _6,
            _8,
            _12,
            _16,
            _23_98,
            _24,
            _25,
            _29_97,
            _30,
            _48,
            _50,
            _59_94,
            _60,
            _120,
            _240,

            Count,
            First = Default
        };
        DJV_ENUM_HELPERS(PlaybackSpeed);
        Core::Time::Speed getPlaybackSpeed(PlaybackSpeed);

        enum class PlaybackMode
        {
            Once,
            Loop,
            PingPong,

            Count,
            First = Once
        };
        DJV_ENUM_HELPERS(PlaybackMode);

    } // namespace ViewApp

    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::ImageViewLock);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::ImageViewGridLabels);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::ScrollWheelZoomSpeed);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::Playback);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::PlaybackSpeed);
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::PlaybackMode);

    picojson::value toJSON(ViewApp::ImageViewLock);
    picojson::value toJSON(ViewApp::ImageViewGridLabels);
    picojson::value toJSON(ViewApp::ScrollWheelZoomSpeed);
    picojson::value toJSON(ViewApp::PlaybackSpeed);
    picojson::value toJSON(ViewApp::PlaybackMode);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::ImageViewLock&);
    void fromJSON(const picojson::value&, ViewApp::ImageViewGridLabels&);
    void fromJSON(const picojson::value&, ViewApp::ScrollWheelZoomSpeed&);
    void fromJSON(const picojson::value&, ViewApp::PlaybackSpeed&);
    void fromJSON(const picojson::value&, ViewApp::PlaybackMode&);

} // namespace djv

