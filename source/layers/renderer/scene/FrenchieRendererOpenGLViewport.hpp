#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class Viewport final
            {
            public:

                Viewport(float _Depth = 100.f, float _Aspect = 1.f, float _Fovy = 90.f, glm::vec3 _Axis = glm::vec3(1.f, 1.f, 1.f));
                ~Viewport();

                glm::mat4 get_projection_matrix() const;
                glm::vec3 get_viewport_scale(const glm::vec2& _ViewportSize) const;
                glm::vec3 get_axis() const;
                float get_aspect() const;
                float get_depth() const;
                float get_fovy() const;

                void set_axis(const glm::vec3& _Value);
                void set_aspect(const float& _Value);
                void set_depth(const float& _Value);
                void set_fovy(const float& _Value);

                protected:

                    // info
                    float     m_Depth  = 100.f; 
                    float     m_Aspect = 1.f;
                    float     m_Fovy   = 90.f;
                    glm::vec3 m_Axis   = glm::vec3(1.f, 1.f, 1.f);
            };
        }
    }
}