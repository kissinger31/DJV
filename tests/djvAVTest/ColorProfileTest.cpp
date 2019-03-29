//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAVTest/ColorProfileTest.h>

#include <djvAV/ColorProfile.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        void ColorProfileTest::run(int &, char **)
        {
            DJV_DEBUG("ColorProfileTest::run");
            ctors();
            operators();
        }

        void ColorProfileTest::ctors()
        {
            DJV_DEBUG("ColorProfileTest::ctors");
            {
                const AV::ColorProfile::Exposure exposure;
                DJV_ASSERT(Math::fuzzyCompare(0.f, exposure.value));
                DJV_ASSERT(Math::fuzzyCompare(0.f, exposure.defog));
                DJV_ASSERT(Math::fuzzyCompare(0.f, exposure.kneeLow));
                DJV_ASSERT(Math::fuzzyCompare(5.f, exposure.kneeHigh));
            }
            {
                const AV::ColorProfile::Exposure exposure(1.f, 2.f, 3.f, 4.f);
                DJV_ASSERT(Math::fuzzyCompare(1.f, exposure.value));
                DJV_ASSERT(Math::fuzzyCompare(2.f, exposure.defog));
                DJV_ASSERT(Math::fuzzyCompare(3.f, exposure.kneeLow));
                DJV_ASSERT(Math::fuzzyCompare(4.f, exposure.kneeHigh));
            }
            {
                const AV::ColorProfile colorProfile;
                DJV_ASSERT(AV::ColorProfile::RAW == colorProfile.type);
                DJV_ASSERT(Math::fuzzyCompare(2.2f, colorProfile.gamma));
                DJV_ASSERT(!colorProfile.lut.isValid());
            }
        }

        void ColorProfileTest::operators()
        {
            DJV_DEBUG("ColorProfileTest::operators");
            {
                AV::ColorProfile a, b;
                a.type = b.type = AV::ColorProfile::LUT;
                a.gamma = b.gamma = 1.f;
                a.lut = b.lut = AV::PixelData(AV::PixelDataInfo(16, 1, AV::Pixel::L_U8));
                a.exposure = b.exposure = AV::ColorProfile::Exposure(1.f, 2.f, 3.f, 4.f);
                a.lut.zero();
                b.lut.zero();
                DJV_ASSERT(a.exposure == b.exposure);
                DJV_ASSERT(a.exposure != AV::ColorProfile::Exposure());
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != AV::ColorProfile());
            }
            {
                AV::ColorProfile::Exposure exposure;
                QStringList s = QStringList() << "1.0" << "2.0" << "3.0" << "4.0";
                s >> exposure;
                DJV_ASSERT(Math::fuzzyCompare(1.f, exposure.value));
                DJV_ASSERT(Math::fuzzyCompare(2.f, exposure.defog));
                DJV_ASSERT(Math::fuzzyCompare(3.f, exposure.kneeLow));
                DJV_ASSERT(Math::fuzzyCompare(4.f, exposure.kneeHigh));
            }
            {
                AV::ColorProfile::Exposure exposure(1.f, 2.f, 3.f, 4.f);
                QStringList s;
                s << exposure;
                DJV_ASSERT((QStringList() << "1" << "2" << "3" << "4") == s);
            }
            {
                const AV::ColorProfile::Exposure a(1.f, 2.f, 3.f, 4.f);
                QStringList tmp;
                tmp << a;
                AV::ColorProfile::Exposure b;
                tmp >> b;
                DJV_ASSERT(a == b);
            }
            {
                const AV::ColorProfile::PROFILE a = AV::ColorProfile::LUT;
                QStringList tmp;
                tmp << a;
                AV::ColorProfile::PROFILE b = static_cast<AV::ColorProfile::PROFILE>(0);
                tmp >> b;
                DJV_ASSERT(a == b);
            }
            {
                DJV_DEBUG_PRINT(AV::ColorProfile::Exposure());
                DJV_DEBUG_PRINT(AV::ColorProfile::RAW);
                DJV_DEBUG_PRINT(AV::ColorProfile());
            }
        }

    } // namespace AVTest
} // namespace djv
