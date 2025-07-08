#pragma once

#include <FrenchieRendererTransformComponent.hpp>
#include <FrenchieRendererMesh.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        // MeshRenderer
        class MeshRenderer : public Component::Registry<MeshRenderer>, public IShader, public IMesh
        {
        public:
            MeshRenderer(
                const std::shared_ptr<Mesh>&   _Mesh   = nullptr, 
                const std::shared_ptr<Shader>& _Shader = nullptr);

            virtual ~MeshRenderer();

            // Component
            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual void draw() override;

            // IShader
            virtual std::shared_ptr<Shader> get_shader() const override;

            // IMesh
            virtual std::shared_ptr<Mesh> get_mesh() const override
            {
                return m_Mesh;
            }

            // Component::Register<TReturnType>
            static TReturnType create()
            {
                return std::make_unique<MeshRenderer>();
            }

            bool cast_ray(const Ray& _Ray, float _ZScale);

        protected:

            class MeshBox : public Mesh
            {
            public:

                MeshBox(const Aabb& _AABB) : 
                    Mesh(MeshBox::generate_vertexes(_AABB)){}

                virtual ~MeshBox(){}

            protected:

                static std::vector<Vertex> generate_vertexes(const Aabb& _AABB)
                {
                    std::vector<Vertex>   vertexes  = std::vector<Vertex>();
                    std::vector<Triangle> triangles = _AABB.get_triangles();

                    for(auto&& triangle : triangles)
                    {
                        vertexes.push_back({triangle.A, glm::vec3(0.f), glm::vec2(0.f)});
                        vertexes.push_back({triangle.B, glm::vec3(0.f), glm::vec2(0.f)});
                        vertexes.push_back({triangle.C, glm::vec3(0.f), glm::vec2(0.f)});
                    }

                    return vertexes;
                }
            };

            std::shared_ptr<Mesh>    m_Mesh    = nullptr;
            std::shared_ptr<MeshBox> m_MeshBox = nullptr;
            std::shared_ptr<Shader>  m_Shader  = nullptr;
        };
    }
}