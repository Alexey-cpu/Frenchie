#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

// STL
#include <queue>
#include <memory>

namespace Frenchie
{
    namespace Application
    {
        class CommandPayload
        {
        public:
            CommandPayload(){}
            virtual ~CommandPayload(){}
        };

        template<typename Type>
        class CommandDataPayload : public CommandPayload
        {
        public:
            CommandDataPayload(Type _Value) : m_Data(_Value){}
            virtual ~CommandDataPayload(){}

            Type get() const
            {
                return m_Data;
            }

            void set(Type _Data)
            {
                m_Data = _Data;
            }

        protected:
            Type m_Data;
        };

        typedef std::list<std::shared_ptr<CommandPayload>> CommandPayloads;

        class Command : public Frenchie::Core::Factory::Creator<Command>
        {
        public:
            Command(const CommandPayloads& _Payload = CommandPayloads()) : m_Payloads(_Payload){}
            virtual ~Command(){}

            // API
            template<typename T> 
            std::shared_ptr<T> get_payload() const
            {
                for (auto&& payload : m_Payloads)
                {
                    auto found = std::dynamic_pointer_cast<T>(payload);

                    if(found != nullptr)
                        return found; 
                }

                return nullptr;
            }

            // virtual API
            virtual void execute() = 0;

        protected:
            CommandPayloads m_Payloads;
        };

        class CallbackCommand : 
            public Command::Registry<CallbackCommand, const std::function<void()>&, const CommandPayloads&>
        {
        public:
            CallbackCommand(const std::function<void()>& _Callback, const CommandPayloads& _Payload = CommandPayloads()) : 
                Command::Registry<CallbackCommand, const std::function<void()>&, const CommandPayloads&>(_Payload), 
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

        class CommandQueue : public Layer
        {
        public:
            CommandQueue();
            virtual ~CommandQueue();

            void push(const std::string& _Command, const CommandPayloads& _Payload = CommandPayloads())
            {
                std::unique_ptr<Command> command = 
                    Frenchie::Core::Factory::create<Command, const CommandPayloads&>(_Command, _Payload);

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

        protected:
            std::queue<std::unique_ptr<Command>> m_Commands{std::queue<std::unique_ptr<Command>>()};
        };
    }
}