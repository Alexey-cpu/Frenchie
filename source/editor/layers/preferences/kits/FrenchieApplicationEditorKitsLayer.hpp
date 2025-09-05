#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class Kits : 
                public Frenchie::Application::Layer::Registry<Kits>,
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Kits();
                virtual ~Kits();

                virtual bool awake() override;
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const;

                // Frenchie::Application::Layer::Registry<KitsSettings>
                static std::string factory_id();

                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:

                // kits
                std::map<std::filesystem::path, bool> m_CMakes;
                std::map<std::filesystem::path, bool> m_CPack;
                std::map<std::filesystem::path, bool> m_CCompilers;
                std::map<std::filesystem::path, bool> m_CXXCompilers;
                std::map<std::filesystem::path, bool> m_Generators;
                std::map<std::filesystem::path, bool> m_Debuggers;

                // system environment variables 
                std::set<std::filesystem::path> m_PATH;
                std::set<std::filesystem::path> m_LIB;
                std::set<std::filesystem::path> m_INCLUDE;

                // static service methods
                static std::map<std::filesystem::path, bool> scan_for_package(
                    const std::set<std::filesystem::path>&, 
                    const std::string&);

                static void draw_package(
                    std::map<std::filesystem::path, bool>& _Package, 
                    const std::string& _Name);
            };
        }
    }
}