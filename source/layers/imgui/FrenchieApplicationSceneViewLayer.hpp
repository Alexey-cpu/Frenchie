#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationCommandsQueueLayer.hpp>
#include <FrenchieApplicationTimeProviderLayer.hpp>

#include <FrenchieRendererScene3D.hpp>

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
        using namespace Frenchie::Renderer;

        class SceneView : public Layer
        {
        public:

            SceneView(const std::string& _Name, std::shared_ptr<Scene3D> _Scene3D);
            virtual ~SceneView();

            // Layer
            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void close() override;
            virtual bool is_closed() override;

        protected:

            struct SceneCursor
            {
                glm::vec3 CurrentPosition;
                glm::vec3 PreviousPosition;
                glm::vec3 PositionDelta;
            } m_SceneCursor;

            std::shared_ptr<Scene3D>            m_Scene         = nullptr;
            std::shared_ptr<CommandsQueueLayer> m_CommandsQueue = nullptr;
            std::shared_ptr<TimeProviderLayer>  m_TimeProvider  = nullptr;
            Scene3DMousePicker::PickedObjects   m_Selection     = Scene3DMousePicker::PickedObjects();

            static glm::vec3 to_ndc(const glm::vec2& _ScreenSize, const glm::vec3& _OpenGLPosition);
            void process_events(const glm::vec3&);
        };
    }
}