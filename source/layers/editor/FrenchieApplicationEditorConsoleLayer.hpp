#pragma once

#include <FrenchieCoreSerializationNode.hpp>
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class Console : public Layer
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
                virtual bool awake();
                virtual void frame_start();
                virtual void frame_update();
                virtual void frame_finish();
                virtual void finish();
                virtual bool allows_multiple_instances() const;

            protected:

                // info
                Frenchie::Core::Serialization::Document m_Messages;

                mutable std::vector<Filter>  m_MessageTypeFilter   = std::vector<Filter>();
                mutable char                 m_MessageContentFilter[1024]{};
                mutable size_t               m_MaximumMessageCount = 100;
                mutable bool                 m_InitializeDockSpace = true;

                // friends
                friend class ConsoleSink;
            };
        };
    }
}