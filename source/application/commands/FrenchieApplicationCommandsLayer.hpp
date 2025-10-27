#pragma once

// Application
#include <FrenchieApplication.hpp>

// Core
#include <FrenchieCoreStringConvert.hpp>

// STL
#include <queue>

namespace Frenchie
{
    namespace Application
    {
        class Command : public Frenchie::Core::Factory::Creator<Command>
        {
        public:
            Command(void* _Sender = nullptr) : m_Sender(_Sender){}
            virtual ~Command(){}

            // API
            template<typename T> 
            T* get_sender() const
            {
                return reinterpret_cast<T*>(m_Sender);
            }

            // virtual API
            virtual void execute() = 0;

        protected:
            void* m_Sender = nullptr;
        };

        class CallbackCommand : 
            public Command::Registry<CallbackCommand, const std::function<void()>&, void*>
        {
        public:
            CallbackCommand(const std::function<void()>& _Callback, void* _Sender = nullptr) : 
                Command::Registry<CallbackCommand, const std::function<void()>&, void*>(_Sender), 
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

        class CommandsQueue : public Layer
        {
        public:
            CommandsQueue();
            virtual ~CommandsQueue();

            void push(const std::string& _Command, void* _Sender = nullptr)
            {
                std::unique_ptr<Command> command = 
                    Frenchie::Core::Factory::create<Command>(_Command, _Sender);

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
            virtual bool allows_multiple_instances() const override;

            // static API
            static Frenchie::Core::Reference<CommandsQueue> instance()
            {
                auto commands = Frenchie::Application::application()->find_layer<CommandsQueue>();
                
                if(commands == nullptr) 
                    commands = Frenchie::Application::application()->push_layer<CommandsQueue>();

                return commands;
            }

        protected:
            std::queue<std::unique_ptr<Command>> m_Commands = 
                std::queue<std::unique_ptr<Command>>();
        };
    }
}