#pragma once

#include <FrenchieCoreObject.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        class ScreenShoter : public Core::Component::Registry<ScreenShoter>, public IRenderer
        {
        public:
            
            ScreenShoter();
            virtual ~ScreenShoter();

            // public Frenchie::Core::Component
            virtual bool awake() override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual void draw() override;

            // IRenderer
            virtual unsigned int get_texture() const override;
            virtual void render() override;

            // Component::Register<TReturnType>
            static TReturnType create()
            {
                return std::make_unique<ScreenShoter>();
            }

        protected:

            unsigned int m_Framebuffer;
            unsigned int m_TextureColorBuffer;
            unsigned int m_TextureDepthBuffer;
        };
    }
}