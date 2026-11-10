#pragma once

// Application
#include <FrenchieApplication.hpp>

// Engine
#include <FrenchieEngineScene.hpp>
#include <FrenchieEngineSystem.hpp>

namespace Frenchie
{
    namespace Engine
    {
        using namespace Frenchie::Application;
        using namespace Frenchie::Engine;

        class World : public Layer
        {
        public:

            World();
            virtual ~World();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_input() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;

            template<typename Type, typename ... Arguments>
            static std::shared_ptr<Type> push_system(Arguments... _Parameters)
            {
                // return found system if it does not allow multiple instances
                std::shared_ptr<Type> found = find_system<Type>();
                if(found != nullptr && !found->allows_multiple_instances())
                    return found;

                // create system
                std::shared_ptr<Type> system = std::make_shared<Type>(_Parameters...);
                m_Awakes.push_back(system);
                return system;
            }

            template<typename Type>
            static std::shared_ptr<Type> find_system()
            {
                // search within existing systems
                auto system = std::find_if(
                    m_Systems.begin(),
                    m_Systems.end(),
                    [](std::shared_ptr<Layer> _System)->bool{return std::dynamic_pointer_cast<Type>(_System) != nullptr;});

                if(system != m_Systems.end())
                    return std::dynamic_pointer_cast<Type>(*system);

                // search within systems to be awakened
                auto awake = std::find_if(
                    m_Awakes.begin(),
                    m_Awakes.end(),
                    [](std::shared_ptr<Layer> _System)->bool{ return std::dynamic_pointer_cast<Type>(_System) != nullptr;});

                if(awake != m_Awakes.end())
                    return std::dynamic_pointer_cast<Type>(*awake);

                return nullptr;
            }

        private:
            Scene                              m_Scene;
            std::list<std::shared_ptr<System>> m_Systems;
            std::list<std::shared_ptr<System>> m_Awakes;
        };
    }
}