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

        class HierarchyView : public Layer
        {
        public:

            HierarchyView(const std::string& _Name, std::shared_ptr<Scene3D> _Scene3D);
            virtual ~HierarchyView();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void close() override;
            virtual bool is_closed() override;

        protected:
            std::shared_ptr<Scene3D> m_Scene = nullptr;
            char m_TextInput[512]{};

            void DrawTree(Object*, int&);
        };
    }
};