//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/IContainerWidget.h>

namespace djv
{
    namespace UI
    {
        namespace MDI
        {
            class Window;
            
            class Canvas : public Widget
            {
                DJV_NON_COPYABLE(Canvas);

            protected:
                void _init(Core::Context *);
                Canvas();

            public:
                virtual ~Canvas();

                static std::shared_ptr<Canvas> create(Core::Context *);

                std::shared_ptr<Window> addWidget(const std::string & title, const std::shared_ptr<Widget>&, const glm::vec2& = glm::vec2());
                void removeWidget(const std::shared_ptr<Widget>&);
                void clearWidgets();

                void preLayoutEvent(Core::PreLayoutEvent&) override;
                void layoutEvent(Core::LayoutEvent&) override;
                bool eventFilter(const std::shared_ptr<Core::IObject>&, Core::IEvent&) override;

            private:
                void _clampWindows();

                struct Private;
                std::unique_ptr<Private> _p;
            };

        } // namespace MDI
    } // namespace UI
} // namespace djv