#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreReference.hpp>
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieCoreObject.hpp>

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

                bool awake();
                void frame_start();
                void frame_update();
                void frame_finish();
                void finish();
                
                bool is_closed();
                void close();
                
                // native API
                int execute();

                template<typename Type, typename ... Arguments>
                Core::Reference<Type> push(Arguments... _Parameters)
                {
                    auto layer = std::make_shared<Type>(_Parameters ...);
                    
                    if(contains<Type>() && 
                        !layer->allows_multiple_instances())
                    {
                        Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::OPENGL::APPLICATION::ERROR"));
                        Frenchie::Core::Logger::instance()->error(fmt::format("Application queue already contains {}", layer->get_name()));
                        return nullptr;
                    }

                    if(!layer->awake())
                    {
                        Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::OPENGL::APPLICATION::ERROR"));
                        Frenchie::Core::Logger::instance()->error(fmt::format("Could not awake layer {}", layer->get_name()));
                        return nullptr;
                    }

                    m_Layers.push_back(layer);
                    return Core::Reference<Type>(layer);
                }

                template<typename Type>
                bool contains()
                {
                    return std::find_if(
                            m_Layers.begin(),
                            m_Layers.end(),
                            [](std::shared_ptr<Layer> _Layer)->bool
                            {
                                return std::dynamic_pointer_cast<Type>(_Layer) != nullptr;
                            }) != m_Layers.end();
                }

                template<typename Type>
                std::shared_ptr<Type> find()
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

            protected:
                std::list<std::shared_ptr<Layer>> m_Layers =  std::list<std::shared_ptr<Layer>>();
                std::string                       m_Name   = "Frenchie::Application";
                bool                              m_Closed = false;
                GLFWwindow*                       m_Window = nullptr;

                static void mouse_callback(GLFWwindow* _Window, double _X, double _Y);
            };
        };

        typedef Frenchie::Core::Singleton<Frenchie::Application::OpenGL::Application> Application;
    };
};