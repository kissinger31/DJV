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

#include <djvCore/Context.h>

#include <djvCore/CoreSystem.h>
#include <djvCore/FileIO.h>
#include <djvCore/IObject.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Time.h>
#include <djvCore/Timer.h>

#include <thread>

namespace
{
    const size_t fpsSamplesCount = 100;

} // namespace

namespace djv
{
    namespace Core
    {
        void Context::_init(int & argc, char ** argv)
        {
            for (int i = 0; i < argc; ++i)
            {
                _args.push_back(argv[i]);
            }
            const std::string argv0 = argc ? argv[0] : std::string();
            _name = FileSystem::Path(argv0).getBaseName();

            _timerSystem = Time::TimerSystem::create(this);
            _resourceSystem = ResourceSystem::create(argv0, this);
            _logSystem = LogSystem::create(_resourceSystem->getPath(FileSystem::ResourcePath::LogFile), this);
            _textSystem = TextSystem::create(_resourceSystem->getPath(FileSystem::ResourcePath::TextDirectory), this);
            CoreSystem::create(argv0, this);

            {
                std::stringstream s;
                s << "Application: " << _name << '\n';
                s << "System information: " << OS::getInformation() << '\n';
                s << "Hardware concurrency: " << std::thread::hardware_concurrency() << '\n';
                s << "Resource paths:" << '\n';
                for (auto path : FileSystem::getResourcePathEnums())
                {
                    s << "    " << path << ": " << _resourceSystem->getPath(path) << '\n';
                }
                _logSystem->log("djv::Core::Context", s.str());
            }

            _fpsTimer = Time::Timer::create(this);
            _fpsTimer->setRepeating(true);
            _fpsTimer->start(
                Time::getMilliseconds(Time::TimerValue::VerySlow),
                [this](float)
            {
                float average = 1.f;
                for (const auto & i : _fpsSamples)
                {
                    average += i;
                }
                average /= static_cast<float>(_fpsSamples.size());
                std::stringstream s;
                s << "FPS: " << average;
                _logSystem->log("djv::Core::Context", s.str());
            });
        }

        Context::~Context()
        {}

        std::unique_ptr<Context> Context::create(int & argc, char ** argv)
        {
            auto out = std::unique_ptr<Context>(new Context);
            out->_init(argc, argv);
            return std::move(out);
        }

        void Context::tick(float dt)
        {
            const auto now = std::chrono::system_clock::now();
            const std::chrono::duration<float> delta = now - _fpsTime;
            _fpsTime = now;
            _fpsSamples.push_front(1.f / delta.count());
            while (_fpsSamples.size() > fpsSamplesCount)
            {
                _fpsSamples.pop_back();
            }
            _fpsAverage = 0.f;
            for (auto i : _fpsSamples)
            {
                _fpsAverage += i;
            }
            _fpsAverage /= static_cast<float>(_fpsSamples.size());
            //std::cout << "fps = " << _fpsAverage << std::endl;

            static bool logSystemOrder = true;
            size_t count = 0;
            if (logSystemOrder)
            {
                logSystemOrder = false;
                std::vector<std::string> dot;
                dot.push_back("digraph {");
                for (const auto & system : _systems)
                {
                    {
                        std::stringstream s;
                        s << "Tick system #" << count << ": " << system->getSystemName();
                        _logSystem->log("djv::Core::Context", s.str());
                        ++count;
                    }
                    for (const auto & dependency : system->getDependencies())
                    {
                        std::stringstream ss;
                        ss << "    " << "\"" << system->getSystemName() << "\"";
                        ss << " -> " << "\"" << dependency->getSystemName() << "\"";
                        dot.push_back(ss.str());
                    }
                }
                dot.push_back("}");
                FileSystem::FileIO::writeLines("systems.dot", dot);
            }

            for (const auto & system : _systems)
            {
                system->tick(dt);
            }
        }

        void Context::_addSystem(const std::shared_ptr<ISystemBase> & system)
        {
            _systems.push_back(system);
        }

    } // namespace ViewExperiment
} // namespace djv
