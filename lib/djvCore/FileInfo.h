// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Frame.h>
#include <djvCore/Path.h>
#include <djvCore/PicoJSON.h>

#include <set>

#include <sys/types.h>

#if defined(DJV_PLATFORM_WINDOWS)
typedef int uid_t;
#endif // DJV_PLATFORM_WINDOWS

namespace djv
{
    namespace Core
    {
        //! This namespace provides file system functionality.
        namespace FileSystem
        {
            //! This enumeration provides file types.
            enum class FileType
            {
                File,      //!< Regular file
                Sequence,  //!< File sequence
                Directory, //!< Directory

                Count,
                First = File
            };
            DJV_ENUM_HELPERS(FileType);

            //! This enumeration provides the file permissions.
            enum class FilePermissions
            {
                Read  = 1, //!< Readable
                Write = 2, //!< Writable
                Exec  = 4, //!< Executable
            };
            std::string getFilePermissionsLabel(int);

            //! This enumeration provides the directory listing sort options.
            enum class DirectoryListSort
            {
                Name,
                Size,
                Time,

                Count,
                First = Name
            };
            DJV_ENUM_HELPERS(DirectoryListSort);

            //! This struct provides directory listing options.
            struct DirectoryListOptions
            {
                std::set<std::string>       fileExtensions;
                bool                        fileSequences           = false;
                std::set<std::string>       fileSequenceExtensions;
                bool                        showHidden              = false;
                DirectoryListSort           sort                    = DirectoryListSort::Name;
                bool                        reverseSort             = false;
                bool                        sortDirectoriesFirst    = true;
                std::string                 filter;
            };

            //! This class provides information about files and file sequences.
            //!
            //! A file sequence is a list of file names that share a common name and
            //! have frame numbers. File sequences are used to store animation or movie
            //! footage where each frame is an individual file.
            //!
            //! A file sequence is expressed as a file name with a start and end frame
            //! separated with a dash ('-'), for example "render.1-100.exr". Multiple
            //! start and end frames and individual frames are separated with a comma
            //! (','), for example "render.1-10,20-30,33,35,37.exr". File sequences
            //! are always sorted in ascending order and frame numbers are always
            //! positive.
            //!
            //! File sequences may also have frames with padded zeroes, for example
            //! "render.0001-0100.exr".
            //!
            //! Note the file sequences return information for the entire sequence.
            class FileInfo
            {
            public:
                FileInfo();
                FileInfo(const Path&, bool stat = true);
                explicit FileInfo(const Path&, FileType, bool stat = true);
                FileInfo(const std::string &, bool stat = true);

                //! \name Path
                ///@{

                const Path& getPath() const;
                bool isEmpty() const;
                void setPath(const Path& path, bool stat = true);
                void setPath(const Path& path, FileType fileType, bool stat = true);

                //! Get the file name.
                //! \param frame Specify a frame number or -1 for the entire sequence.
                //! \param path Include the path in the file name.
                std::string getFileName(Frame::Number frame = Frame::invalid, bool path = true) const;

                //! Get whether this file exists.
                bool doesExist() const;

                ///@}

                //! \name File Information
                ///@{

                FileType getType() const;
                uint64_t getSize() const;
                uid_t getUser() const;
                int getPermissions() const;
                time_t getTime() const;

                //! Get information from the file system.
                bool stat(std::string* error = nullptr);

                ///@}

                //! \name File Sequences
                ///@{

                const Frame::Sequence& getSequence() const;
                void setSequence(const Frame::Sequence &);
                void evalSequence();
                void sortSequence();
                bool isSequenceValid() const;
                bool isCompatible(const FileInfo &) const;
                bool addToSequence(const FileInfo &);
                
                ///@}

                //! \name Utilities
                ///@{

                //! Test whether the string contains all '#' characters.
                static bool isSequenceWildcard(const std::string&);

                //! Get the contents of the given directory.
                static std::vector<FileInfo> directoryList(const Path& path, const DirectoryListOptions& options = DirectoryListOptions());

                //! Get the file sequence for the given file.
                static FileInfo getFileSequence(const Path&, const std::set<std::string>& extensions);

                ///@}

                bool operator == (const FileInfo &) const;
                bool operator != (const FileInfo &) const;
                bool operator < (const FileInfo &) const;

                explicit operator std::string() const;

            private:
                static void _fileSequence(FileInfo&, const DirectoryListOptions&, std::vector<FileInfo>&);
                static void _sort(const DirectoryListOptions&, std::vector<FileInfo>&);
                
                Path            _path;
                bool            _exists      = false;
                FileType        _type        = FileType::File;
                uint64_t        _size        = 0;
                uid_t           _user        = 0;
                int             _permissions = 0;
                time_t          _time        = 0;
                Frame::Sequence _sequence;
            };
        } // namespace Core

    } // namespace FileSystem

    picojson::value toJSON(Core::FileSystem::FileType);
    picojson::value toJSON(Core::FileSystem::DirectoryListSort);
    picojson::value toJSON(const Core::FileSystem::FileInfo&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, Core::FileSystem::FileType&);
    void fromJSON(const picojson::value&, Core::FileSystem::DirectoryListSort&);
    void fromJSON(const picojson::value&, Core::FileSystem::FileInfo&);

    DJV_ENUM_SERIALIZE_HELPERS(Core::FileSystem::FileType);
    DJV_ENUM_SERIALIZE_HELPERS(Core::FileSystem::DirectoryListSort);

    std::ostream & operator << (std::ostream &, const Core::FileSystem::FileInfo &);
    
} // namespace djv

#include <djvCore/FileInfoInline.h>

