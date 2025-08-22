#pragma once

#include <FrenchieCoreCommand.hpp>
#include <FrenchieCoreHelpers.hpp>

#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class CommandsQueue : public Layer
        {
        public:
            CommandsQueue();
            virtual ~CommandsQueue();
            
            // API
            void push(const std::string& _Command)
            {
                m_CommandsQueue.push(_Command);
            }

            template<typename Type, typename ... Arguments>
            void push(Arguments... _Args)
            {
                m_CommandsQueue.push<Type>(std::move(_Args...));
            }

            // Layer
            virtual void frame_start() override;

        protected:
            Frenchie::Core::Commands m_CommandsQueue;
        };
    }
}