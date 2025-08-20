#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieRendererScene3D.hpp>

#include <FrenchieApplicationCommandsQueueLayer.hpp>
#include <FrenchieApplicationTimeProviderLayer.hpp>

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

            virtual bool on_awake() override;
            virtual void on_frame_start() override;
            virtual void on_frame_update() override;
            virtual void on_frame_finish() override;
            virtual void on_finish() override;

        protected:
            std::shared_ptr<Scene3D>            m_Scene         = nullptr;
            std::shared_ptr<CommandsQueueLayer> m_CommandsQueue = nullptr;
            std::shared_ptr<TimeProviderLayer>  m_TimeProvider  = nullptr;
            char                                m_TextInput[512]{};

            void draw_tree(Object*, int&);
        };
    }
};