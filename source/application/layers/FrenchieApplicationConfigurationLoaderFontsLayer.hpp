#pragma once

// Core
#include <FrenchieCoreISerializer.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

// Editor
#include <FrenchieEditorLocalThreadQueue.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Configuration
        {
            class Fonts : 
                public Frenchie::Application::Layer, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Fonts();
                virtual ~Fonts();

                // getters
                std::filesystem::path get_app_fonts_files_directory() const;

                // Frenchie::Application::Layer
                virtual bool allows_multiple_instances() const override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

                // API
                void load_fonts(
                    const std::set<std::filesystem::path>& _Paths, 
                    const std::string&                     _Font = std::string());

                // static API
                //static Frenchie::Core::Reference<Fonts> instance();

            protected:

                mutable std::filesystem::path           m_AppFontsFilesPath;
                mutable std::set<std::filesystem::path> m_Paths;
                mutable LocalThreadQueue                m_ThreadsQueue;
            };

            Frenchie::Core::Reference<Frenchie::Application::Configuration::Fonts> fonts();
        }
    }
}