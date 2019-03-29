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

#pragma once

#include <djvAV/OpenGLImage.h>
#include <djvAV/OpenGLOffscreenBuffer.h>

#include <QGuiApplication>
#include <QScopedPointer>

namespace djv
{
    namespace Core
    {
        class Sequence;

    } // namespace Core

    namespace convert
    {
        class Context;

        //! This program provides a command line tool for image and movie conversion.
        class Application : public QGuiApplication
        {
            Q_OBJECT

        public:
            Application(int &, char **);
            ~Application() override;

            //! This enumeration provides error codes.
            enum ERROR
            {
                ERROR_OPEN_INPUT,
                ERROR_OPEN_OUTPUT,
                ERROR_OPEN_SLATE,
                ERROR_READ_INPUT,
                ERROR_WRITE_OUTPUT,

                ERROR_COUNT
            };

            //! Get the error code labels.
            static const QStringList & errorLabels();

        private Q_SLOTS:
            void commandLineExit();
            void work();

        private:
            QString labelImage(const AV::PixelDataInfo &, const Core::Sequence &) const;

            QScopedPointer<Context> _context;
        };

    } // namespace convert
} // namespace djv
