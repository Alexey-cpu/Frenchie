#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

// Core
#include <FrenchieCoreHelpers.hpp>

// STL
#include <queue>

namespace Frenchie
{
    namespace Application
    {
        class Command : public Frenchie::Core::Factory::Creator<Command>
        {
        public:
            Command(){}
            virtual ~Command(){}
            virtual void execute() = 0;
        };

        class CallbackCommand : 
            public Command::Registry<CallbackCommand, const std::function<void()>&>
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

            // Command::TRegistryType
            static std::string factory_id()
            {
                return STRINGIFY(Frenchie::Core::CallbackCommand);
            }

        protected:
            std::function<void()> m_Callback = nullptr;
        };

        // CommandsQueue
        class CommandsQueue : public Layer
        {
        public:
            CommandsQueue();
            virtual ~CommandsQueue();

            void push(const std::string& _Command)
            {
                std::unique_ptr<Command> command = 
                    Frenchie::Core::Factory::create<Command>(_Command);

                if(command != nullptr)
                    m_Commands.push(std::move(command));
            }

            template<typename Type, typename ...Arguments>
            void push(Arguments ... _Args)
            {
                m_Commands.push(std::make_unique<Type>(_Args...));
            }

            // Frenchie::Application::Layer
            virtual void frame_start() override;

        protected:
            std::queue<std::unique_ptr<Command>> m_Commands = 
                std::queue<std::unique_ptr<Command>>();
        };
    }
}