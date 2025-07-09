#pragma once

#include <functional>
#include <memory>
#include <queue>

namespace Frenchie
{
    namespace Core
    {
        class Command
        {
        public:
            Command(){}
            virtual ~Command(){}
            virtual void execute() = 0;
        };

        class CallbackCommand : public Command
        {
        public:
            CallbackCommand(const std::function<void()>& _Callback) : 
                m_Callback(_Callback){}
            
            virtual ~CallbackCommand(){}

            virtual void execute() override
            {
                if(m_Callback != nullptr) 
                    m_Callback();
            }

        protected:
            std::function<void()> m_Callback = nullptr;
        };

        class Commands
        {
        public:
            Commands(){}
            virtual ~Commands(){}

            template<typename Type, typename ...Arguments>
            void add_command(Arguments ... _Args)
            {
                m_Commands.push(std::make_unique<Type>(_Args...));
            }

            void execute()
            {
                while (!m_Commands.empty())
                {
                    // execute command
                    auto& command = m_Commands.front();
                    if(command != nullptr) 
                        command->execute();

                    // pop command out-of queue
                    m_Commands.pop();
                }
            }

        protected:
            std::queue<std::unique_ptr<Command>> m_Commands = 
                std::queue<std::unique_ptr<Command>>();
        };
    }
}