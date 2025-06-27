#pragma once

// Custom
#include <FrenchieCoreObject.hpp>
#include <FrenchieRendererInterfaces.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        using namespace Frenchie::Core;

        class Transform : public Object, public IRenderer
        {
            public:

            Transform(const std::string& _Name = std::string(), Object* _Parent = nullptr);
            virtual ~Transform();

            // getters
            glm::vec3 get_position() const;
            glm::vec3 get_rotation() const;
            glm::vec3 get_scale() const;
            glm::mat4 get_model_matrix() const;

            // setters
            void set_position(const glm::vec3&);
            void set_rotation(const glm::vec3&);
            void set_scale(const glm::vec3&);

            // virtual API
            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

            // service methods
            glm::mat4 compute_local_model_matrix() const;

            protected:

                glm::vec3 m_Position    = glm::vec3(0.f);
                glm::vec3 m_Rotation    = glm::vec3(0.f);
                glm::vec3 m_Scale       = glm::vec3(1.f);
                glm::mat4 m_ModelMatrix = glm::mat4(1.f);
        };
    }   
}