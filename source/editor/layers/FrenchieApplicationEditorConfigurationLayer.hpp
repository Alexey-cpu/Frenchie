#pragma once

// Core
#include <FrenchieCoreISerializer.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class Config : 
                public Layer, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Config();
                virtual ~Config();

                // getters
                std::filesystem::path get_fonts_location() const;

                // API
                void load_fonts(const std::filesystem::path& _Path, const std::string& _Font = std::string());

                // Frenchie::Application::Layer
                virtual bool awake() override;
                virtual void frame_start() override;
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:

                // info
                std::filesystem::path m_FontsLocation = 
                    std::filesystem::current_path();

                // static API
                static std::filesystem::path get_app_exe_directory();
                static std::filesystem::path get_app_log_directory();
                static std::filesystem::path get_app_state_directory();
                static std::filesystem::path get_app_console_directory();
                static std::filesystem::path get_app_console_log_file_path();
            };
        }
    }
}