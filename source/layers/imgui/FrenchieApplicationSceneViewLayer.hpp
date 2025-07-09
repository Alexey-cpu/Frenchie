#pragma once

#include <FrenchieApplicationLayer.hpp>

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

            // info
            std::shared_ptr<Scene3D> m_Scene = nullptr;

            std::map<Transform*, glm::vec3> m_Items;

            // service methods
            void process_mouse_events();
        };
    }
}