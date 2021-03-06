// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/SequenceIO.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This namespace provides NetPBM file I/O.
            //!
            //! References:
            //! - Netpbm, "PPM Format Specification"
            //!   http://netpbm.sourceforge.net/doc/ppm.html
            namespace PPM
            {
                static const std::string pluginName = "PPM";
                static const std::set<std::string> fileExtensions = { ".ppm" };

                //! This enumeration provides the PPM file data types.
                enum class Data
                {
                    ASCII,
                    Binary,

                    Count,
                    First = ASCII
                };
                DJV_ENUM_HELPERS(Data);

                //! This struct provides the PPM file I/O options.
                struct Options
                {
                    Data data = Data::Binary;
                };

                //! Get the number of bytes in a scanline.
                size_t getScanlineByteCount(
                    int    width,
                    size_t channelCount,
                    size_t componentSize);

                //! Read PPM file ASCII data.
                void readASCII(
                    const std::shared_ptr<Core::FileSystem::FileIO>& io,
                    uint8_t* out,
                    size_t size,
                    size_t componentSize);

                //! Save PPM file ASCII data.
                size_t writeASCII(
                    const uint8_t * in,
                    char *          out,
                    size_t          size,
                    size_t          componentSize);

                //! This class provides the PPM file reader.
                class Read : public ISequenceRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const Core::FileSystem::FileInfo&,
                        const ReadOptions&,
                        const std::shared_ptr<Core::TextSystem>&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                protected:
                    Info _readInfo(const std::string &) override;
                    std::shared_ptr<Image::Image> _readImage(const std::string &) override;

                private:
                    Info _open(const std::string&, const std::shared_ptr<Core::FileSystem::FileIO>&, Data&);
                };
                
                //! This class provides the PPM file writer.
                class Write : public ISequenceWrite
                {
                    DJV_NON_COPYABLE(Write);

                protected:
                    Write();

                public:
                    ~Write() override;

                    static std::shared_ptr<Write> create(
                        const Core::FileSystem::FileInfo&,
                        const Info &,
                        const WriteOptions&,
                        const Options&,
                        const std::shared_ptr<Core::TextSystem>&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                protected:
                    Image::Type _getImageType(Image::Type) const override;
                    Image::Layout _getImageLayout() const override;
                    void _write(const std::string & fileName, const std::shared_ptr<Image::Image> &) override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the PPM file I/O plugin.
                class Plugin : public ISequencePlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    Plugin();

                public:
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<Core::Context>&);

                    picojson::value getOptions() const override;
                    void setOptions(const picojson::value &) override;

                    std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions&) const override;
                    std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&, const Info &, const WriteOptions&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace PPM
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::PPM::Data);
    
    picojson::value toJSON(const AV::IO::PPM::Options &);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value &, AV::IO::PPM::Options &);

} // namespace djv
