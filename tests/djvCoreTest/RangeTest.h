// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class RangeTest : public Test::ITest
        {
        public:
            RangeTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
        
        private:
            void _ctor();
            void _util();
            void _operators();
            void _serialize();
        };
        
    } // namespace CoreTest
} // namespace djv

