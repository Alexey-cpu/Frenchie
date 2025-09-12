#pragma once

//#include <FrenchieApplicationAsynchronousProcessesLayer.hpp>
// #include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreReference.hpp>
#include <FrenchieCoreSingleton.hpp>

#include <FrenchieCoreISerializer.hpp>
#include <FrenchieCoreSerializationNode.hpp>
#include <FrenchieCoreSerializationFormatXML.hpp>

// STL
#include <iostream>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace OpenGL
        {
            class Application
            {
            public:
                Application();
                virtual ~Application();

                // getters
                std::string  get_name() const;
                glm::u32vec2 get_window_size() const;
                GLFWwindow*  get_window() const;

                // setters
                void set_name(const std::string& _Name);
                void set_window_size(const glm::u32vec2& _Value);
                void set_maximized(const bool& _Value);

                // API
                bool awake();
                void frame_start();
                void frame_update();
                void frame_finish();
                void finish();
                
                bool is_closed();
                void close();
                void reload();

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
                    if(contains_layer<Type>() && 
                        !layer->allows_multiple_instances())
                    {
                       // Frenchie::Core::Logger::instance()->warn(fmt::format("Application queue already contains '{}'", layer->get_name()));
                        return find_layer<Type>();
                    }

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
                std::list<std::shared_ptr<Layer>>               m_Layers =  std::list<std::shared_ptr<Layer>>();
                std::string                                     m_Name   = "Frenchie::Application";
                bool                                            m_Opened = true;
                GLFWwindow*                                     m_Window = nullptr;
            };
        };

        inline Frenchie::Application::OpenGL::Application* application()
        {
            return Frenchie::Core::Singleton<Frenchie::Application::OpenGL::Application>::instance();
        }
    };
};