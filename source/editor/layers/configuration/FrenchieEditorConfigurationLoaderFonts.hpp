#pragma once

// Core
#include <FrenchieCoreISerializer.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

// Editor
#include <FrenchieEditorLocalThreadQueue.hpp>

namespace Frenchie
{
    namespace Editor
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
                static Frenchie::Core::Reference<Fonts> instance();

            protected:
                mutable std::set<std::filesystem::path> m_Paths;
                mutable LocalThreadQueue                m_ThreadsQueue;
            };
        }
    }
}