#pragma once

// Core
#include <FrenchieCoreReference.hpp>
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieCoreISerializer.hpp>
#include <FrenchieCoreSerializationNode.hpp>
#include <FrenchieCoreSerializationFormatXML.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationPlatformLayer.hpp>
#include <FrenchieApplicationRendererLayer.hpp>
#include <FrenchieApplicationThreadQueueLayer.hpp>
#include <FrenchieApplicationCommandQueueLayer.hpp>
#include <FrenchieApplicationUserInterfaceLayer.hpp>

// STL
#include <iostream>

namespace Frenchie
{
    namespace Application
    {
        class ApplicationInstance
        {
        public:
            ApplicationInstance();
            virtual ~ApplicationInstance();

            // getters
            std::string ApplicationInstance::get_name() const;

            // setters
            void ApplicationInstance::set_name(const std::string&);

            // API
            bool awake();
            void frame_start();
            void frame_update();
            void frame_render();
            void frame_finish();
            void finish();
            void quit();
            
            bool is_closed();
            void close();

            int execute();
            std::list<std::shared_ptr<Layer>>::const_iterator begin() const;
            std::list<std::shared_ptr<Layer>>::const_iterator end() const;
            size_t size() const;

            // layers
            template<typename Type, typename ... Arguments>
            Core::Reference<Type> push_layer(Arguments... _Parameters)
            {
                // create layer
                auto layer = std::make_shared<Type>(_Parameters...);

                // check if layer allows multiple instances
                if(contains_layer<Type>() && !layer->allows_multiple_instances())
                    return find_layer<Type>();

                // awake layer
                if(!layer->awake())
                {
                    Frenchie::Core::Logger::instance()->error(fmt::format("Could not awake layer {}", layer->get_name()));
                    return nullptr;
                }

                // push layer into layers stack
                m_Layers.push_back(layer);
                return Core::Reference<Type>(layer);
            }

            template<typename Type>
            Core::Reference<Type> find_layer()
            {
                auto layer = std::find_if(
                    m_Layers.begin(),
                    m_Layers.end(),
                    [](std::shared_ptr<Layer> _Layer)->bool
                    {
                        return std::dynamic_pointer_cast<Type>(_Layer) != nullptr;
                    }
                );

                return layer != m_Layers.end() ? std::dynamic_pointer_cast<Type>(*layer) : nullptr;
            }

            template<typename Type>
            bool contains_layer()
            {
                return std::find_if(
                        m_Layers.begin(),
                        m_Layers.end(),
                        [](std::shared_ptr<Layer> _Layer)->bool
                        {
                            return std::dynamic_pointer_cast<Type>(_Layer) != nullptr;
                        }) != m_Layers.end();
            }

            typedef std::list<std::shared_ptr<Layer>>::const_iterator const_iterator;

        protected:
            std::list<std::shared_ptr<Layer>> m_Layers =  std::list<std::shared_ptr<Layer>>();
            std::string                       m_Name   = "Frenchie::Application";
            bool                              m_Opened = true;
        };

        Frenchie::Application::ApplicationInstance* application();
        Frenchie::Core::Reference<Platform>         platform();
        Frenchie::Core::Reference<Renderer>         renderer();
        Frenchie::Core::Reference<Interface>        interface();
        Frenchie::Core::Reference<ThreadQueue>      thread_queue();
        Frenchie::Core::Reference<CommandQueue>     commands();
    };
};