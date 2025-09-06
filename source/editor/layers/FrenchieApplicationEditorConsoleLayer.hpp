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
                    std::string                           message;
                };

                // info
                mutable std::map<
                    spdlog::level::level_enum, 
                    std::vector<Message>> m_Messages;

                mutable char  m_MessageContentFilter[1024]{};
                mutable int   m_MaximumMessageCount = 100;
                mutable bool  m_InitializeDockSpace = true;

                // service methods
                static unsigned int get_color(spdlog::level::level_enum _Level);
                static std::string get_group_name(spdlog::level::level_enum _Level);

                // friends
                friend class ConsoleSink;
            };
        };
    }
}