#pragma once

// Custom
#include <FrenchieApplication.hpp>
#include <FrenchieSingleton.hpp>

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
            class Application : public Frenchie::Application::Application
            {
                public:

                    Application();
                    virtual ~Application();

                    // getters
                    glm::u32vec2 get_window_size() const;

                    // setters
                    void set_window_size(const glm::u32vec2&);
                    void set_maximized(const bool&);

                    virtual bool awake()        override;
                    virtual void frame_start()  override;
                    virtual void frame_update() override;
                    virtual void frame_finish() override;
                    virtual void finish()       override;
                    virtual bool is_closed()    override;

                protected:
                    GLFWwindow* m_MainWindow = nullptr;
            };

            typedef Frenchie::Core::Singleton<Application> GLApplication;
        }
    }
}