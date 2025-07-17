#pragma once

#include <FrenchieCoreObject.hpp>
#include <FrenchieRendererIEditor.hpp>
#include <FrenchieRendererIRenderer.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        class ScreenShoter : public Core::Component::Registry<ScreenShoter>, public IRenderer, public IEditor
        {
        public:
            
            ScreenShoter();
            virtual ~ScreenShoter();

            // public Frenchie::Core::Component
            virtual bool awake() override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

            // IEditor
            virtual void draw_editor() override;

            // IRenderer
            virtual unsigned int get_texture() const override;
            virtual void render() override;

            // Component::Register<TReturnType>
            static TReturnType create()
            {
                return std::make_unique<ScreenShoter>();
            }

        protected:

            glm::vec4    m_ClearColor         = glm::vec4(0.25f, 0.25f, 0.25f, 0.5f);
            unsigned int m_Framebuffer        = 0;
            unsigned int m_TextureColorBuffer = 0;
            unsigned int m_TextureDepthBuffer = 0;
        };
    }
}