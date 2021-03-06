// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/OCIOTest.h>

#include <djvAV/OCIO.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        OCIOTest::OCIOTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::OCIOTest", context)
        {}
        
        void OCIOTest::run()
        {
            _convert();
            _view();
            _display();
            _operators();
        }

        void OCIOTest::_convert()
        {
            {
                const OCIO::Convert convert;
                DJV_ASSERT(convert.input.empty());
                DJV_ASSERT(convert.output.empty());
                DJV_ASSERT(!convert.isValid());
            }
            
            {
                const std::string input = "input";
                const std::string output = "output";
                const OCIO::Convert convert(input, output);
                DJV_ASSERT(input == convert.input);
                DJV_ASSERT(output == convert.output);
                DJV_ASSERT(convert.isValid());
            }
        }
        
        void OCIOTest::_view()
        {
            {
                const OCIO::View view;
                DJV_ASSERT(view.name.empty());
                DJV_ASSERT(view.colorSpace.empty());
                DJV_ASSERT(view.looks.empty());
            }
        }
        
        void OCIOTest::_display()
        {
            {
                const OCIO::Display display;
                DJV_ASSERT(display.name.empty());
                DJV_ASSERT(display.defaultView.empty());
                DJV_ASSERT(display.views.empty());
            }
        }
        
        void OCIOTest::_operators()
        {
            {
                const OCIO::Convert convert("input", "output");
                DJV_ASSERT(convert == convert);
                DJV_ASSERT(OCIO::Convert() < convert);
            }
            
            {
                OCIO::View view;
                view.name = "name";
                view.colorSpace = "colorSpace";
                view.looks = "looks";
                DJV_ASSERT(view == view);
            }
            
            {
                OCIO::Display display;
                display.name = "name";
                display.defaultView = "defaultView";
                display.views.push_back(OCIO::View());
                DJV_ASSERT(display == display);
            }
        }

    } // namespace AVTest
} // namespace djv

