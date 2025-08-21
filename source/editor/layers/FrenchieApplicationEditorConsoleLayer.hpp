#pragma once

#include <FrenchieCoreSerializationNode.hpp>
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class Console : 
                public Layer::Registry<Console>, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:

                struct Filter
                {
                    std::string Level;
                    bool        Selected;
                };

                // constructors
                Console();

                // virtual destructor
                virtual ~Console();

                // Layer
                virtual bool awake() override;
                virtual void frame_update() override;
                virtual void finish() override;
                virtual bool allows_multiple_instances() const override;

                // Component::Register<TReturnType>
                static TReturnType create();
                static const char* factory_id();

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:

                // info
                Frenchie::Core::Serialization::Document m_Messages;

                mutable std::vector<Filter>  m_MessageTypeFilter   = std::vector<Filter>();
                mutable char                 m_MessageContentFilter[1024]{};
                mutable int                  m_MaximumMessageCount = 100;
                mutable bool                 m_InitializeDockSpace = true;

                // friends
                friend class ConsoleSink;
            };
        };
    }
}