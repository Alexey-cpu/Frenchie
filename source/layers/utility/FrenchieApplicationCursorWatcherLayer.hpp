#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreHelpers.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Application
    {
        class CursorWatcher : public Layer, public IMouseCallbackHandler
        {
        public:
            CursorWatcher();
            virtual ~CursorWatcher();

            // Layer
            virtual void frame_update() override;
            virtual void mouse_callback(GLFWwindow* _Window, double _X, double _Y) override;

            // API
            void hide_cursor();
            void show_cursor();

            glm::vec3 m_CurrentPosition  = glm::vec3(0.f);
            glm::vec3 m_PreviousPosition = glm::vec3(0.f);
            glm::vec3 m_PositionDelta    = glm::vec3(0.f);
        };
    }
}