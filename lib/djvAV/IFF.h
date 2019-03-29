//------------------------------------------------------------------------------
// Copyright (c) 2008-2009 Mikael Sundell
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

#include <djvAV/IO.h>

#include <djvCore/FileIO.h>

namespace djv
{
    namespace AV
    {
        //! This struct provides IFF utilities.
        struct IFF
        {
            //! Plugin name.
            static const QString staticName;

            //! This enumeration provides the file compression.
            enum COMPRESSION
            {
                COMPRESSION_NONE,
                COMPRESSION_RLE,

                COMPRESSION_COUNT
            };

            //! Get compression labels.
            static const QStringList & compressionLabels();

            //! Load information.
            //!
            //! Throws:
            //! - Core::Error
            static void loadInfo(Core::FileIO & io, IOInfo & info, int * tiles, bool * compression);

            //! Save information.
            //!
            //! Throws:
            //! - Core::Error
            static void saveInfo(Core::FileIO & io, const IOInfo & info, bool compression);

            //! Load RLE compressed data.
            static int readRle(const quint8 * in, quint8 * end, int size);

            //! Save RLE compressed data.
            static int writeRle(const quint8 * in, quint8 * out, int size);

            //! Get alignment size.
            static quint32 alignSize(quint32 size, quint32 alignment);

            //! Get tile size.
            static glm::ivec2 tileSize(quint32 width, quint32 height);

            //! Get tile width.
            static int tileWidth();

            //! Get tile height.
            static int tileHeight();

            //! This enumeration provides the options.
            enum OPTIONS
            {
                COMPRESSION_OPTION,

                OPTIONS_COUNT
            };

            //! Get option labels.
            static const QStringList & optionsLabels();

            //! This struct provides options.
            struct Options
            {
                COMPRESSION compression = IFF::COMPRESSION_RLE;
            };
        };

    } // namespace AV

    DJV_STRING_OPERATOR(AV::IFF::COMPRESSION);

} // namespace djv
