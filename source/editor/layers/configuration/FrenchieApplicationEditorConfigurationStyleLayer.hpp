#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace Configuration
            {
                class Style : 
                    public Layer, 
                    public Frenchie::Core::Serialization::ISerializer
                {
                public:
                    Style();
                    virtual ~Style();

                    // Frenchie::Application::Layer
                    virtual bool allows_multiple_instances() const override;

                    // Frenchie::Core::Serialization::ISerializer
                    virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                    virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                };
            }
        }
    }
}