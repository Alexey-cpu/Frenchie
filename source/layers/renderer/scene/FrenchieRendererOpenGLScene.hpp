#pragma once

#include <FrenchieCoreObject.hpp>

#include <FrenchieRendererOpenGLCamera.hpp>
#include <FrenchieRendererOpenGLViewport.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class Scene : public Frenchie::Core::Object
            {
            public:
                Scene(const std::string& _Name = "Frenchie::Renderer::OpenGL::Scene", Frenchie::Core::Object* _Parent = nullptr);
                virtual ~Scene();
            
                // virtual API
                virtual bool awake() override;
                virtual void frame_start() override;
                virtual void frame_update() override;
                virtual void frame_finish() override;
                
            protected:
            };
        }
    }
}