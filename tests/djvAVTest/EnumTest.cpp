// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/EnumTest.h>

#include <djvAV/Enum.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        EnumTest::EnumTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::EnumTest", context)
        {}
        
        void EnumTest::run()
        {
            for (auto i : getSideEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "side string: " << _getText(ss.str());
                _print(ss2.str());
            }

            for (auto i : getAlphaBlendEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "alpha blend string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            {
                const AlphaBlend value = AlphaBlend::First;
                auto json = toJSON(value);
                AlphaBlend value2 = AlphaBlend::Count;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                auto json = picojson::value(picojson::object_type, true);
                AlphaBlend value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

    } // namespace AVTest
} // namespace djv

