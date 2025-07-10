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
            CommandsQueue() : Layer(STRINGIFY(CommandsQueue)){}
            virtual ~CommandsQueue(){}

            template<typename Type, typename ... Arguments>
            void push(Arguments... _Args)
            {
                m_CommandsQueue.push<Type>(std::move(_Args...));
            }

            virtual bool awake() override
            {
                return Layer::awake();
            }
            
            virtual void frame_start() override
            {
                m_CommandsQueue.execute(); // execute all command added into queue
            }
            
            virtual void frame_update() override
            {
                Layer::frame_update();
            }
            
            virtual void frame_finish() override
            {
                Layer::frame_finish();
            }
            
            virtual void finish() override
            {
                Layer::finish();
            }
            
            virtual void close() override
            {
                Layer::close();
            }
            
            virtual bool is_closed() override
            {
                return Layer::is_closed();
            }

        protected:
            Frenchie::Core::Commands m_CommandsQueue;
        };
    }
}