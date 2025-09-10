#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>

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
                void scan_fonts(const std::filesystem::path& _Paths);

                void load_fonts(
                    const std::set<std::filesystem::path>& _Paths, 
                    const std::string&                     _Font = std::string());

            protected:
                // info
                std::set<std::filesystem::path> m_Path = 
                    std::set<std::filesystem::path>();
            };
        }
    }
}