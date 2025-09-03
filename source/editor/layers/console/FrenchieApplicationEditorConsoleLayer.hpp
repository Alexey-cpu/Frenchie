#pragma once

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
                public Layer, 
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

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:

                struct Message
                {
                    std::chrono::system_clock::time_point time;
                    spdlog::level::level_enum             level;
                    unsigned int                          color;
                    bool                                  selected;
                    std::string                           message;
                };

                // info
                mutable std::vector<Message> m_Messages;

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