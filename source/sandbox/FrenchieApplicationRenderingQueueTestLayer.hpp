#pragma once

#include <FrenchieApplication.hpp>

#include <chrono>
#include <iostream>

namespace Frenchie
{
    namespace Application
    {
        template<typename Type>
        struct gs_rect
        {
            gs_rect(){}
            

            gs_vector<Type, 2> Min{gs_vec2f(0.f)};
            gs_vector<Type, 2> Max{gs_vec2f(0.f)};

            gs_vector<Type, 2> get_size() const
            {
                return Max - Min;
            }
        };

        typedef gs_rect<float> gs_rectf;

        struct Renderer2DPrimitive
        {
            gs_mat4f Transform;
            gs_rectf BoundingRect;
        };

        //
        class RenderingQueueTestLayer : public Layer
        {
        public:
            RenderingQueueTestLayer(){}
            virtual ~RenderingQueueTestLayer(){}

            virtual bool awake() override
            {
                // register default shader here
                m_DefaultShader = Frenchie::Application::application_rendering_queue()->construct_shader(
                    {
                        // Vertex shader
                        {
                            std::string(
R"(
#version 330 core

// vertex attributes
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

// outputs
out vec3 Normal;
out vec2 UV;

// uniforms
uniform mat4 u_ModelMatrix;
uniform mat4 u_CameraViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main()
{
    // setup position
    gl_Position = u_ProjectionMatrix * u_CameraViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);

    // setup outputs
    Normal = a_Normal;
    UV     = a_UV;
}            
)"),
                            RenderingQueueShaderType_::RenderingQueueShaderType_Vertex
                        },

                        // fragment shader
                        {
                            std::string(
R"(
#version 330 core

// inputs
in vec2 UV;

//outputs
out vec4 fragColor;

//uniforms
uniform vec4      u_Color;
uniform sampler2D u_Texture;

void main()
{
    // setup vertex color
    //fragColor = u_Color;
    //fragColor = texture(u_Texture, UV);
    fragColor = u_Color * texture(u_Texture, UV);
}
)"),
                            RenderingQueueShaderType_::RenderingQueueShaderType_Fragment
                            },
                    }
                );    

                // create default white pattern texture
                const int     height   = 4;
                const int     width    = 4;
                const int     channels = 4;
                const int     red      = 0;
                const int     green    = 1;
                const int     blue     = 2;
                const int     alpha    = 3;
                unsigned char image[width * height * channels]{};

                for (int y = 0; y < height; y++)
                {
                    for (int x = 0; x < width; x++)
                    {
                        image[channels * (y * width + x) + red  ] = 255;
                        image[channels * (y * width + x) + green] = 255;
                        image[channels * (y * width + x) + blue ] = 255;
                        image[channels * (y * width + x) + alpha] = 255;
                    }
                }

                m_DefaultTexture = Frenchie::Application::application_rendering_queue()->construct_texture(image, width, height);

                return true;
            }

            virtual void frame_update() override
            {
                // setup projection matrix
                {
                    float width  = Frenchie::Application::application()->get_size().x;
                    float height = Frenchie::Application::application()->get_size().y;
                    float left   = -width  * 0.5f + width  * 0.5f; // The x-coordinate of the left edge of the viewable area.
                    float right  = +width  * 0.5f + width  * 0.5f; // The x-coordinate of the right edge of the viewable area.
                    float bottom = -height * 0.5f - height * 0.5f; // The y-coordinate of the bottom edge of the viewable area.
                    float top    = +height * 0.5f - height * 0.5f; // The y-coordinate of the top edge of the viewable area.

                    Frenchie::Application::application_rendering_queue()
                        ->set_projection_matrix(gs_matrix_ortho(left, right, bottom, top, -1000.0f, 1000.0f));

                    // Frenchie::Application::application_rendering_queue()
                    //     ->set_projection_matrix(gs_matrix_perspective(gs_to_radians(90.f), 1.f, -1000.f, +1000.f));
                }

                // add graphical primitives
                if(Frenchie::Application::application()
                    ->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
                {   
                    auto x = Frenchie::Application::application()->get_cursor_position().x;
                    auto y = Frenchie::Application::application()->get_cursor_position().y;

                    Transform = gs_matrix_translate(
                        gs_mat4f(1),
                        gs_vec3f(
                            x, 
                            -y, 
                            -100.f
                        )
                    );
                }

                //Transform = gs_matrix_translate(gs_mat4f(1), gs_vec3f(Frenchie::Application::application()->get_size().x * 0.25f, -Frenchie::Application::application()->get_size().y * 0.25f, -100.f));

                // push_triangle_filled(
                //     gs_vec3f(-0.f, +0.f, +0.f),
                //     gs_vec3f(+100.f, +100.f, 0.f),
                //     gs_vec3f(+100.f, -100.f, 0.f),
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform
                // );

                // push_rect_filled(
                //     gs_vec3f(-0.f, +0.f, +0.f),
                //     gs_vec3f(+100.f, -100.f, 0.f),
                //     gs_vec4f(255.f, 255.f, 255.f, 255.f),
                //     Transform
                // );

                float lineWidth = 64.f;

                // push_triangle(
                //     gs_vec3f(+0.f, +0.f, +0.f),
                //     gs_vec3f(+150.f, +100.f, 0.f),
                //     gs_vec3f(+150.f, -100.f, 0.f),
                //     lineWidth,
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform
                // );

                // push_rectangle(
                //     gs_vec3f(+0.f, +0.f, +0.f),
                //     gs_vec3f(+100.f, +100.f, 0.f),
                //     lineWidth,
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform
                // );

                // push_line(
                //     gs_vec3f(-0.f, +0.f, +0.f),
                //     gs_vec3f(+100.f, -0.f, 0.f),
                //     lineWidth,
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform
                // );

                // push_line(
                //     gs_vec3f(+100.f, -0.f, -100.f),
                //     gs_vec3f(+100.f, -400.f, -100.f),
                //     lineWidth,
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform
                // );

                // push_line(
                //     gs_vec3f(+100.f, -400.f, -100.f),
                //     gs_vec3f(+200.f, -200.f, 0.f),
                //     lineWidth,
                //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                //     Transform
                // );
            }
            
            virtual void finish() override
            {
                Frenchie::Application::application_rendering_queue()->destroy_shader(m_DefaultShader);
                Frenchie::Application::application_rendering_queue()->destroy_texture(m_DefaultTexture);
            }
        
            // rendering API
            void push_triangle_filled(const gs_vec3f& _P1, const gs_vec3f& _P2, const gs_vec3f& _P3, const gs_vec4f& _Color, const gs_mat4f& _Transform)
            {
                m_Vertexes.push_back(RenderingQueueVertex(_P1, gs_vec3f(0.f), gs_vec2f(_P1.x / m_DefaultTexture.Width, _P1.y / m_DefaultTexture.Height)));
                m_Vertexes.push_back(RenderingQueueVertex(_P2, gs_vec3f(0.f), gs_vec2f(_P2.x / m_DefaultTexture.Width, _P2.y / m_DefaultTexture.Height)));
                m_Vertexes.push_back(RenderingQueueVertex(_P3, gs_vec3f(0.f), gs_vec2f(_P3.x / m_DefaultTexture.Width, _P3.y / m_DefaultTexture.Height)));
                m_Indexes.push_back(0);
                m_Indexes.push_back(1);
                m_Indexes.push_back(2);

                Frenchie::Application::application_rendering_queue()->push_command(
                    // construct mesh
                    Frenchie::Application::application_rendering_queue()->construct_mesh(
                        &m_Vertexes[0],
                        (int)m_Vertexes.size(),
                        &m_Indexes[0],
                        (int)m_Indexes.size()),
                    
                    // provide default shader
                    m_DefaultShader,

                    // setup texture
                    RenderingQueueTexture(
                        m_DefaultTexture.Ptr,
                        m_DefaultTexture.Width,
                        m_DefaultTexture.Height,
                        _Color,
                        m_DefaultTexture.Format,
                        m_DefaultTexture.Wrap,
                        m_DefaultTexture.MinFilter,
                        m_DefaultTexture.MaxFilter),
                    _Transform);

                // clean-up
                m_Indexes.clear();
                m_Vertexes.clear();
            }

            void push_rectangle_filled(const gs_vec3f& _Min, const gs_vec3f& _Max, const gs_vec4f& _Color, const gs_mat4f& _Transform)
            {
                // triangle 1
                {
                    const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, gs_min(_Min.z, _Max.z));
                    const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, gs_min(_Min.z, _Max.z));
                    const gs_vec3f _P3 = gs_vec3f(_Min.x, _Max.y, gs_min(_Min.z, _Max.z));
                    m_Vertexes.push_back(RenderingQueueVertex(_P1, gs_vec3f(0.f), gs_vec2f(_P1.x / m_DefaultTexture.Width, _P1.y / m_DefaultTexture.Height)));
                    m_Vertexes.push_back(RenderingQueueVertex(_P2, gs_vec3f(0.f), gs_vec2f(_P2.x / m_DefaultTexture.Width, _P2.y / m_DefaultTexture.Height)));
                    m_Vertexes.push_back(RenderingQueueVertex(_P3, gs_vec3f(0.f), gs_vec2f(_P3.x / m_DefaultTexture.Width, _P3.y / m_DefaultTexture.Height)));
                }

                // triangle 2
                {
                    const gs_vec3f _P1 = gs_vec3f(_Max.x, _Min.y, gs_min(_Min.z, _Max.z));
                    const gs_vec3f _P2 = _Max;
                    const gs_vec3f _P3 = gs_vec3f(_Min.x, _Max.y, gs_min(_Min.z, _Max.z));
                    m_Vertexes.push_back(RenderingQueueVertex(_P1, gs_vec3f(0.f), gs_vec2f(_P1.x / m_DefaultTexture.Width, _P1.y / m_DefaultTexture.Height)));
                    m_Vertexes.push_back(RenderingQueueVertex(_P2, gs_vec3f(0.f), gs_vec2f(_P2.x / m_DefaultTexture.Width, _P2.y / m_DefaultTexture.Height)));
                    m_Vertexes.push_back(RenderingQueueVertex(_P3, gs_vec3f(0.f), gs_vec2f(_P3.x / m_DefaultTexture.Width, _P3.y / m_DefaultTexture.Height)));
                }

                m_Indexes.push_back(0);
                m_Indexes.push_back(1);
                m_Indexes.push_back(2);
                m_Indexes.push_back(3);
                m_Indexes.push_back(4);
                m_Indexes.push_back(5);

                Frenchie::Application::application_rendering_queue()->push_command(
                    // construct mesh
                    Frenchie::Application::application_rendering_queue()->construct_mesh(
                        &m_Vertexes[0],
                        (int)m_Vertexes.size(),
                        &m_Indexes[0],
                        (int)m_Indexes.size()),
                    
                    // provide default shader
                    m_DefaultShader,

                    // setup texture
                    RenderingQueueTexture(
                        m_DefaultTexture.Ptr,
                        m_DefaultTexture.Width,
                        m_DefaultTexture.Height,
                        _Color,
                        m_DefaultTexture.Format,
                        m_DefaultTexture.Wrap,
                        m_DefaultTexture.MinFilter,
                        m_DefaultTexture.MaxFilter),
                    _Transform);

                // clean-up
                m_Indexes.clear();
                m_Vertexes.clear();
            }

            void push_line(const gs_vec3f& _P1, const gs_vec3f& _P2, const float& _Width, const gs_vec4f& _Color, const gs_mat4f& _Transform)
            {
                gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
                gs_vec3f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * _Width * 0.5f;
                push_triangle_filled(_P1 - perpendicular, _P2 - perpendicular, _P1 + perpendicular, _Color, _Transform);
                push_triangle_filled(_P1 + perpendicular, _P2 - perpendicular, _P2 + perpendicular, _Color, _Transform);
            }

            void push_triangle(const gs_vec3f& _P1, const gs_vec3f& _P2, const gs_vec3f& _P3, const float& _LineWidth, const gs_vec4f& _Color, const gs_mat4f& _Transform)
            {
                push_line(_P1, _P2, _LineWidth, _Color, _Transform);
                push_line(_P2, _P3, _LineWidth, _Color, _Transform);
                push_line(_P3, _P1, _LineWidth, _Color, _Transform);
            }

            void push_rectangle(const gs_vec3f& _Min, const gs_vec3f& _Max, const float& _LineWidth, const gs_vec4f& _Color, const gs_mat4f& _Transform)
            {
                const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, gs_min(_Min.z, _Max.z));
                const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, gs_min(_Min.z, _Max.z));
                const gs_vec3f _P3 = gs_vec3f(_Max.x, _Max.y, gs_min(_Min.z, _Max.z));
                const gs_vec3f _P4 = gs_vec3f(_Min.x, _Max.y, gs_min(_Min.z, _Max.z));
                
                push_line(_P1, _P2, _LineWidth, _Color, _Transform);
                push_line(_P2, _P3, _LineWidth, _Color, _Transform);
                push_line(_P3, _P4, _LineWidth, _Color, _Transform);
                push_line(_P4, _P1, _LineWidth, _Color, _Transform);
            }

            void push_circle(const gs_vec3f _Center, gs_vec2f _Radius, const float& _LineWidth, const gs_vec4f& _Color, const gs_mat4f& _Transform)
            {
            }

            gs_mat4f Transform = gs_mat4f(1.f);

            // camera
            mutable gs_vec3f m_CameraWorldPosition           = gs_vec3f(+0.f, +0.f, +1.f);
            mutable gs_vec3f m_CameraWorldUpAxisDirection    = gs_vec3f(+0.f, +1.f, +0.f);
            mutable gs_vec3f m_CameraLocalFrontAxisDirection = gs_vec3f(0.f);
            mutable gs_vec3f m_CameraLocalRightAxisDirection = gs_vec3f(0.f);
            mutable gs_vec3f m_CameraLocalUpAxisDirection    = gs_vec3f(0.f);

            mutable float     m_Pitch         = 0.f;
            mutable float     m_Yaw           = 0.f;
            mutable float     m_Roll          = 0.f;


            mutable float     m_MovementSpeed = 1.0f;
            mutable float     m_Sensitivity   = 0.1f;

            gs_vec2f m_LastCursorPos;

            // this is a plipeline
            std::vector<RenderingQueueVertex>  m_Vertexes        {std::vector<RenderingQueueVertex>()};
            std::vector<int>                   m_Indexes         {std::vector<int>()};
            RenderingQueueShader               m_DefaultShader   {RenderingQueueShader()};
            RenderingQueueTexture              m_DefaultTexture  {RenderingQueueTexture()};
        };
    }
}

// template<typename Type> struct Tree;

// template<typename Type>
// struct Node
// {
//     int               Parent{-1};
//     int               Index {-1};
//     Type              Data  {Type()};
//     const Tree<Type>* Tree  {nullptr};
// };

// template<typename Type>
// struct Tree
// {
//     std::vector<Node<Type>> Nodes  {std::vector<Node<Type>>()};
//     std::vector<int>        Indexes{std::vector<int>()};
//     std::vector<int>        Entries{std::vector<int>()};
//     bool                    Dirty  {true};

//     template<typename ... Args>
//     Node<Type> construct_node(const Node<Type>& _Parent, Args ... _Args)
//     {
//         // make dirty
//         Dirty = true;

//         // create node
//         Node<Type> node;
//         node.Parent = _Parent.Index;
//         node.Index  = (int)Nodes.size();
//         node.Data   = Type(_Args ...);
//         node.Tree   = this;
//         Nodes.push_back(node);
//         return node;
//     }

//     void sort()
//     {
//         std::vector<Node<Type>> nodes(Nodes.size());
//         std::vector<int> workspace(Nodes.size()+1);

//         Indexes.resize(Nodes.size() + 1);
//         Entries.resize(Nodes.size());

//         for(int i = 0; i < Entries.size(); i++)
//         {
//             Entries[i] = 0;
//             Indexes[i] = 0;
//         }

//         // count items
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             if(Nodes[i].Parent < 0) continue;
//             ++Entries[Nodes[i].Parent];
//         }

//         // cumulative sum
//         int sum = 0;
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             Indexes  [i] = sum;
//             workspace[i] = sum;
//             sum += Entries[i];
//         }
//         Indexes[Nodes.size()] = sum;

//         for(int i = 0; i < Nodes.size(); i++ )
//         {
//             if(Nodes[i].Parent < 0) continue;
//             nodes[workspace[Nodes[i].Parent]++] = Nodes[i];
//         }

//         Nodes = nodes;
//     }

//     void clear()
//     {
//         Nodes.clear();
//     }
// };