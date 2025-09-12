#pragma once

// Core
#include <FrenchieCoreISerializer.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationAsynchronousProcessesLayer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        namespace Configuration
        {
            class LoadFontsProcess : 
                public Frenchie::Application::Process,
                public Frenchie::Application::IProcessStatus,
                public Frenchie::Application::IProcessProgress
            {
            public:
                LoadFontsProcess(const std::set<std::filesystem::path>& _Paths);
                LoadFontsProcess(const std::filesystem::path& _Path);
                virtual ~LoadFontsProcess();

                virtual bool awake() override;
                virtual std::string iprocess_status_request_status() override;
                virtual float iprocess_progress_request_progress() override;

            protected:
                std::set<std::filesystem::path> m_Paths = 
                    std::set<std::filesystem::path>();

                std::string m_Status;
                float       m_Progress;
            };

            class Fonts : 
                public Frenchie::Application::Layer, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Fonts();
                virtual ~Fonts();

                // Frenchie::Application::Layer
                virtual bool allows_multiple_instances() const override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

                // API
                //void scan_fonts(const std::filesystem::path& _Paths);

                void load_fonts(
                    const std::set<std::filesystem::path>& _Paths, 
                    const std::string&                     _Font = std::string());

                // static API
                static Frenchie::Core::Reference<Fonts> instance();

            protected:
                std::set<std::filesystem::path> m_Paths;
            };
        }
    }
}