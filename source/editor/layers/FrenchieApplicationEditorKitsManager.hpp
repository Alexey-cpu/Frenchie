#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class KitsManager : public Layer, public Frenchie::Core::Serialization::ISerializer
            {
            public:
                KitsManager();
                virtual ~KitsManager();

                virtual bool awake() override;
                virtual void frame_update() override;

                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:

                // kits
                std::map<std::filesystem::path, bool> m_CMakes;
                std::map<std::filesystem::path, bool> m_CCompilers;
                std::map<std::filesystem::path, bool> m_CXXCompilers;
                std::map<std::filesystem::path, bool> m_Generators;
            };
        }
    }
}