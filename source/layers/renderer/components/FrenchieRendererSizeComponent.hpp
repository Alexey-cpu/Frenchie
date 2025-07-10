#pragma once

// Custom
#include <FrenchieCoreObject.hpp>
#include <FrenchieRendererIEditor.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Frenchie
{
    namespace Renderer
    {
        class Size : public Frenchie::Core::Component::Registry<Size>, public IEditor
        {
        public:
            Size(const glm::vec2& _Size = glm::vec2(0.f));
            virtual ~Size();

            // getters
            glm::vec2 get_size() const;

            // setters
            void set_size(const glm::vec2& _Value);

            // IRenderer
            virtual bool awake() override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual void draw_editor() override;

            // Component::Register<TReturnType>
            static TReturnType create()
            {
                return std::make_unique<Size>();
            }

        protected:
            glm::vec2 m_Size = glm::vec2(0.f, 0.f);
        };
    }
}