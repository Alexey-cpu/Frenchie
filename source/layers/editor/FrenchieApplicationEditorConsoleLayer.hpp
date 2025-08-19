#pragma once

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

                // nested types
                struct Message
                {
                    std::chrono::system_clock::time_point Time;
                    int                                   Level;
                    std::string                           Message;
                    std::string                           LoggerName;
                };

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
                mutable std::vector<Message> m_Messages            = std::vector<Message>();
                
                mutable std::vector<Filter>  m_Filters = 
                {
                    {"trace",    false},
                    {"debug",    false},
                    {"info",     true},
                    {"warn",     true},
                    {"error",    true},
                    {"critical", true}
                };

                mutable size_t               m_MaximumMessageCount = 100;

                // friends
                friend class ConsoleSink;
            };
        };
    }
}