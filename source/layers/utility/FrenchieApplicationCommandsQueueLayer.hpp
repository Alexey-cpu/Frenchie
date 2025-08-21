#pragma once

#include <FrenchieCoreCommand.hpp>
#include <FrenchieCoreHelpers.hpp>

#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class CommandsQueueLayer : public Layer
        {
        public:
            CommandsQueueLayer();
            virtual ~CommandsQueueLayer();
            
            // Layer
            virtual void frame_start() override;

            template<typename Type, typename ... Arguments>
            void push(Arguments... _Args)
            {
                m_CommandsQueue.push<Type>(std::move(_Args...));
            }

        protected:
            Frenchie::Core::Commands m_CommandsQueue;
        };
    }
}