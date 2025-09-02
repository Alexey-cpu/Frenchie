#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

// Core
#include <FrenchieCoreISerializer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            // Preferences
            class Preferences : public Layer, public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Preferences();
                virtual ~Preferences();

                // Frenchie::Application::Layer
                virtual bool awake() override;
                virtual void frame_update() override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:
                std::list<std::shared_ptr<Layer>> m_Layers = 
                    std::list<std::shared_ptr<Layer>>();
            };
        }
    }
}