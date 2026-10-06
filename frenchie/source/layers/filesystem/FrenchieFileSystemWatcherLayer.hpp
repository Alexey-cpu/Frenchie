#pragma once

// Applicatiuon
#include <FrenchieApplicationLayer.hpp>

// STL
#include <map>
#include <iostream>
#include <filesystem>
#include <functional>

namespace Frenchie
{
    namespace Application
    {
        class FileSystemWatcher : public Layer
        {
        public:
            FileSystemWatcher(const std::vector<std::filesystem::path>& _Paths, const std::function<void(const std::filesystem::path&)> _Callback);
            virtual ~FileSystemWatcher();

            virtual bool awake() override;
            virtual void frame_start() override;

        private:

            void process_paths();

            std::map<std::filesystem::path, std::filesystem::file_time_type> m_Files;
            std::function<void(const std::filesystem::path&)>                m_Callback;
        };
    }
}