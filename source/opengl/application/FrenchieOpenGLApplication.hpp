#pragma once

// Custom
#include <FrenchieApplication.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

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

                    virtual bool awake()        override;
                    virtual void frame_start()  override;
                    virtual void frame_update() override;
                    virtual void frame_finish() override;
                    virtual void finish()       override;
                    virtual bool is_closed()    override;
                    virtual void close()        override;

                protected:
                    GLFWwindow* m_MainWindow = nullptr;
                    bool        m_Closed     = false;
            };
        }
    }
}