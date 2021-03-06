// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AudioData.h>
#include <djvAV/Image.h>
#include <djvAV/Tags.h>

#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/ISystem.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/Speed.h>
#include <djvCore/Time.h>
#include <djvCore/ValueObserver.h>

#include <future>
#include <queue>
#include <mutex>
#include <set>

namespace djv
{
    namespace Core
    {
        class LogSystem;
        class ResourceSystem;
        class TextSystem;

    } // namespace Core

    namespace AV
    {
        //! This namespace provides I/O functionality.
        namespace IO
        {
            //! This class provides video I/O information.
            class VideoInfo
            {
            public:
                VideoInfo();
                VideoInfo(
                    const Image::Info &,
                    const Core::Time::Speed & = Core::Time::Speed(),
                    const Core::Frame::Sequence& = Core::Frame::Sequence());

                Image::Info info;
                Core::Time::Speed speed;
                Core::Frame::Sequence sequence;
                std::string codec;

                bool operator == (const VideoInfo &) const;
            };

            //! This class provides audio I/O information.
            class AudioInfo
            {
            public:
                AudioInfo();
                explicit AudioInfo(const Audio::Info &);

                Audio::Info info;
                std::string codec;

                bool operator == (const AudioInfo &) const;
            };

            //! This class provides I/O information.
            class Info
            {
            public:
                Info();
                Info(const std::string & fileName, const VideoInfo &);
                Info(const std::string & fileName, const AudioInfo &);
                Info(const std::string & fileName, const VideoInfo &, const AudioInfo &);
                Info(const std::string & fileName, const std::vector<VideoInfo> &, const std::vector<AudioInfo> &);

                std::string fileName;
                std::vector<VideoInfo> video;
                std::vector<AudioInfo> audio;
                Tags tags;

                bool operator == (const Info &) const;
            };

            //! This class provides a video frame.
            class VideoFrame
            {
            public:
                VideoFrame();
                VideoFrame(Core::Frame::Number, const std::shared_ptr<Image::Image>&);

                Core::Frame::Number           frame = 0;
                std::shared_ptr<Image::Image> image;

                bool operator == (const VideoFrame&) const;
            };

            //! This class provides a queue of video frames.
            class VideoQueue
            {
                DJV_NON_COPYABLE(VideoQueue);

            public:
                VideoQueue();

                size_t getMax() const;
                void setMax(size_t);

                bool isEmpty() const;
                size_t getCount() const;
                VideoFrame getFrame() const;
                void addFrame(const VideoFrame&);
                VideoFrame popFrame();
                void clearFrames();

                bool isFinished() const;
                void setFinished(bool);

            private:
                size_t _max = 0;
                std::queue<VideoFrame> _queue;
                bool _finished = false;
            };

            //! This class provides an audio frame.
            class AudioFrame
            {
            public:
                AudioFrame();
                explicit AudioFrame(const std::shared_ptr<Audio::Data>&);

                std::shared_ptr<Audio::Data> audio;
                
                bool operator == (const AudioFrame&) const;
            };

            //! This class provides a queue of audio frames.
            class AudioQueue
            {
                DJV_NON_COPYABLE(AudioQueue);

            public:
                AudioQueue();

                size_t getMax() const;
                void setMax(size_t);

                bool isEmpty() const;
                size_t getCount() const;
                AudioFrame getFrame() const;
                void addFrame(const AudioFrame &);
                AudioFrame popFrame();
                void clearFrames();

                bool isFinished() const;
                void setFinished(bool);

            private:
                std::mutex _mutex;
                size_t _max = 0;
                std::queue<AudioFrame> _queue;
                bool _finished = false;
            };

            //! This class provides I/O options.
            struct IOOptions
            {
                size_t videoQueueSize = 1;
                //! \todo What is a good default for this value?
                size_t audioQueueSize = 30;
            };

            //! This class provides an interface for I/O.
            class IIO : public std::enable_shared_from_this<IIO>
            {
            protected:
                void _init(
                    const Core::FileSystem::FileInfo&,
                    const IOOptions&,
                    const std::shared_ptr<Core::TextSystem>&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

            public:
                virtual ~IIO() = 0;

                virtual bool isRunning() const = 0;

                size_t getThreadCount() const;
                void setThreadCount(size_t);

                std::mutex& getMutex();
                VideoQueue& getVideoQueue();
                AudioQueue& getAudioQueue();

            protected:
                std::shared_ptr<Core::LogSystem> _logSystem;
                std::shared_ptr<Core::ResourceSystem> _resourceSystem;
                std::shared_ptr<Core::TextSystem> _textSystem;
                Core::FileSystem::FileInfo _fileInfo;
                std::mutex _mutex;
                VideoQueue _videoQueue;
                AudioQueue _audioQueue;
                size_t _threadCount = 4;
            };

            //! This class provides options for reading.
            struct ReadOptions : IOOptions
            {
                size_t layer = 0;
                std::string colorSpace;
            };

            //! This class provides playback in/out points.
            class InOutPoints
            {
            public:
                InOutPoints();
                InOutPoints(bool, Core::Frame::Index, Core::Frame::Index);

                bool isEnabled() const;
                Core::Frame::Index getIn() const;
                Core::Frame::Index getOut() const;

                Core::Range::Range<Core::Frame::Index> getRange(size_t) const;

                bool operator == (const InOutPoints&) const;

            private:
                bool _enabled = false;
                Core::Frame::Index _in = Core::Frame::invalid;
                Core::Frame::Index _out = Core::Frame::invalid;
            };

            //! This enumeration provides the playback direction for caching.
            enum class Direction
            {
                Forward,
                Reverse
            };

            //! This class provides a frame cache.
            class Cache
            {
            public:
                Cache();
                
                size_t getMax() const;
                size_t getCount() const;
                size_t getTotalByteCount() const;
                Core::Frame::Sequence getFrames() const;
                size_t getReadBehind() const;
                const Core::Frame::Sequence& getSequence() const;
                void setMax(size_t);
                void setSequenceSize(size_t);
                void setInOutPoints(const InOutPoints&);
                void setDirection(Direction);
                void setCurrentFrame(Core::Frame::Index);

                bool contains(Core::Frame::Index) const;
                bool get(Core::Frame::Index, std::shared_ptr<AV::Image::Image>&) const;
                void add(Core::Frame::Index, const std::shared_ptr<AV::Image::Image>&);
                void clear();

            private:
                void _cacheUpdate();

                size_t _max = 0;
                size_t _sequenceSize = 0;
                InOutPoints _inOutPoints;
                Direction _direction = Direction::Forward;
                Core::Frame::Index _currentFrame = 0;
                //! \todo Should this be configurable?
                size_t _readBehind = 10;
                Core::Frame::Sequence _sequence;
                std::map<Core::Frame::Index, std::shared_ptr<AV::Image::Image> > _cache;
            };

            //! This class provides an interface for reading.
            class IRead : public IIO
            {
            protected:
                void _init(
                    const Core::FileSystem::FileInfo&,
                    const ReadOptions&,
                    const std::shared_ptr<Core::TextSystem>&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

            public:
                virtual ~IRead() = 0;

                virtual std::future<Info> getInfo() = 0;

                void setPlayback(bool);
                void setLoop(bool);
                void setInOutPoints(const InOutPoints&);

                //! \param value For video files this value represents the
                //! frame number, for audio files it represents the audio sample.
                virtual void seek(int64_t value, Direction) = 0;

                virtual bool hasCache() const { return false; }
                bool isCacheEnabled() const;
                size_t getCacheMaxByteCount() const;
                size_t getCacheByteCount();
                Core::Frame::Sequence getCacheSequence();
                Core::Frame::Sequence getCachedFrames();
                void setCacheEnabled(bool);
                void setCacheMaxByteCount(size_t);

            protected:
                ReadOptions _options;
                InOutPoints _inOutPoints;
                Direction _direction = Direction::Forward;
                bool _playback = false;
                bool _loop = false;
                bool _cacheEnabled = false;
                size_t _cacheMaxByteCount = 0;
                size_t _cacheByteCount = 0;
                Core::Frame::Sequence _cacheSequence;
                Core::Frame::Sequence _cachedFrames;
                Cache _cache;
            };

            //! This class provides options for writing.
            struct WriteOptions : IOOptions
            {
                std::string colorSpace;
            };

            //! This class provides an interface for writing.
            class IWrite : public IIO
            {
            protected:
                void _init(
                    const Core::FileSystem::FileInfo &,
                    const Info &,
                    const WriteOptions&,
                    const std::shared_ptr<Core::TextSystem>&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

            public:
                virtual ~IWrite() = 0;

            protected:
                Info _info;
                WriteOptions _options;
            };

            //! This class provides an interface for I/O plugins.
            class IPlugin : public std::enable_shared_from_this<IPlugin>
            {
            protected:
                void _init(
                    const std::string& pluginName,
                    const std::string& pluginInfo,
                    const std::set<std::string>& fileExtensions,
                    const std::shared_ptr<Core::Context>&);

            public:
                virtual ~IPlugin() = 0;

                const std::string & getPluginName() const;
                const std::string & getPluginInfo() const;
                const std::set<std::string> & getFileExtensions() const;

                virtual bool canSequence() const;
                virtual bool canRead(const Core::FileSystem::FileInfo&) const;
                virtual bool canWrite(const Core::FileSystem::FileInfo&, const Info &) const;

                virtual picojson::value getOptions() const;

                //! Throws:
                //! - std::invalid_argument
                virtual void setOptions(const picojson::value &);

                //! Throws:
                //! - Core::FileSystem::Error
                virtual std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions&) const;

                //! Throws:
                //! - Core::FileSystem::Error
                virtual std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&, const Info&, const WriteOptions&) const;

            protected:
                std::weak_ptr<Core::Context> _context;
                std::shared_ptr<Core::LogSystem> _logSystem;
                std::shared_ptr<Core::ResourceSystem> _resourceSystem;
                std::shared_ptr<Core::TextSystem> _textSystem;
                std::string _pluginName;
                std::string _pluginInfo;
                std::set<std::string> _fileExtensions;
            };

            //! This class provides an I/O system.
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                System();

            public:
                virtual ~System();

                static std::shared_ptr<System> create(const std::shared_ptr<Core::Context>&);

                std::set<std::string> getPluginNames() const;
                std::set<std::string> getFileExtensions() const;

                picojson::value getOptions(const std::string & pluginName) const;

                //! Throws:
                //! - std::invalid_argument
                void setOptions(const std::string & pluginName, const picojson::value &);

                std::shared_ptr<Core::IValueSubject<bool> > observeOptionsChanged() const;

                const std::set<std::string>& getSequenceExtensions() const;
                bool canSequence(const Core::FileSystem::FileInfo&) const;
                bool canRead(const Core::FileSystem::FileInfo&) const;
                bool canWrite(const Core::FileSystem::FileInfo&, const Info &) const;

                //! Throws:
                //! - Core::FileSystem::Error
                std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions& = ReadOptions());

                //! Throws:
                //! - Core::FileSystem::Error
                std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&, const Info &, const WriteOptions& = WriteOptions());

            private:
                DJV_PRIVATE();
            };

        } // namespace IO
    } // namespace AV
} // namespace djv

#include <djvAV/IOInline.h>
