#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreFlyweight.hpp>
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
                std::string get_name() const;
                glm::u32vec2 get_window_size() const;

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

                template<typename __type, typename ... __parameters>
                std::shared_ptr<__type> push(__parameters... _Parameters)
                {
                    std::shared_ptr<__type> layer = 
                        std::make_shared<__type>(_Parameters ...);
                    layer->awake();
                    m_Layers.push_back(layer);
                    return layer;
                }

                template<typename __type>
                bool contains()
                {
                    return std::find_if(
                            m_Layers.begin(),
                            m_Layers.end(),
                            [](std::shared_ptr<Layer> _Layer)->bool
                            {
                                return std::dynamic_pointer_cast<__type>(_Layer) != nullptr;
                            }) != m_Layers.end();
                }

            protected:
                std::list<std::shared_ptr<Layer>> m_Layers     =  std::list<std::shared_ptr<Layer>>();
                std::string                       m_Name       = "Frenchie::Application";
                bool                              m_Closed     = false;
                GLFWwindow*                       m_MainWindow = nullptr;
            };
        };

        typedef Frenchie::Core::Singleton<Frenchie::Application::OpenGL::Application> Application;
        typedef Frenchie::Core::Singleton<Frenchie::Core::Flyweight<std::string>> AssetManager;
    };
};