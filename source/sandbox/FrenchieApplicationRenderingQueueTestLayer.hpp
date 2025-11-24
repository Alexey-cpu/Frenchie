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

            virtual void frame_start() override
            {
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
            }

            virtual void frame_update() override
            {
                for(int i = 0; i < 1; ++i)
                {
                    for(int j = 0; j < 1; ++j)
                    {
                        float lineWidth = 2.f;
                        float R = 128.f;

                        push_rectangle_rounded(
                            gs_vec3f(i * R * 2, j * R * 2, -100.f),
                            gs_vec3f(i * R * 2 + R, j * R * 2 + R, -100.f),
                            32.f,
                            lineWidth,
                            gs_vec4f(0.f, 255.f, 0.f, 255.f),
                            Transform
                        );
                        
                        push_rectangle(
                            gs_vec3f(i * R * 2, j * R * 2, +0.f),
                            gs_vec3f(i * R * 2 + R, j * R * 2 + R, +0.f),
                            lineWidth,
                            gs_vec4f(255.f, 0.f, 0.f, 255.f),
                            Transform
                        );

                        // push_arc(
                        //     gs_vec3f(i * R * 2, j * R * 2, +0.f),
                        //     +R,
                        //     +R * 0.5f,
                        //     90.f,
                        //     270.f,
                        //     lineWidth,
                        //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                        //     Transform
                        // );
                    }
                }
            }
            
            virtual void finish() override
            {
                Frenchie::Application::application_rendering_queue()->destroy_shader(m_DefaultShader);
                Frenchie::Application::application_rendering_queue()->destroy_texture(m_DefaultTexture);
            }
        
            // rendering API
            void push_triangle_filled(
                const gs_vec3f& _P1,
                const gs_vec3f& _P2,
                const gs_vec3f& _P3,
                const gs_vec4f& _ItemColor,
                const gs_mat4f& _Transform)
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
                        _ItemColor,
                        m_DefaultTexture.Format,
                        m_DefaultTexture.Wrap,
                        m_DefaultTexture.MinFilter,
                        m_DefaultTexture.MaxFilter),
                    _Transform);

                // clean-up
                m_Indexes.clear();
                m_Vertexes.clear();
            }

            void push_rectangle_filled(
                const gs_vec3f& _Min,
                const gs_vec3f& _Max,
                const gs_vec4f& _ItemColor,
                const gs_mat4f& _Transform)
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
                        _ItemColor,
                        m_DefaultTexture.Format,
                        m_DefaultTexture.Wrap,
                        m_DefaultTexture.MinFilter,
                        m_DefaultTexture.MaxFilter),
                    _Transform);

                // clean-up
                m_Indexes.clear();
                m_Vertexes.clear();
            }

            void push_line(
                const gs_vec3f& _P1,
                const gs_vec3f& _P2,
                const float&    _LineWidth,
                const gs_vec4f& _LineColor,
                const gs_mat4f& _Transform)
            {
                gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
                gs_vec3f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * _LineWidth * 0.5f;
                push_triangle_filled(_P1 - perpendicular, _P2 - perpendicular, _P1 + perpendicular, _LineColor, _Transform);
                push_triangle_filled(_P1 + perpendicular, _P2 - perpendicular, _P2 + perpendicular, _LineColor, _Transform);
            }

            // non filled primitives
            void push_arc(
                const gs_vec3f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                float           _SourceAngle,
                float           _TargetAngle,
                const float&    _LineWidth,
                const gs_vec4f& _ItemColor,
                const gs_mat4f& _Transform)
            {
                gs_vec3f p0 = gs_vec3f(_Center.x + _MinorRadius * cos(gs_to_radians(_SourceAngle)), _Center.y + _MajorRadius * sin(gs_to_radians(_SourceAngle)), _Center.z);
                gs_vec3f p1 = p0;
                gs_vec3f p2 = p0;

                const float angleIncrement = _TargetAngle / 36.f;

                for (float angle = _SourceAngle; angle <= _TargetAngle; angle += angleIncrement, p1 = p2)
                {
                    p2 = gs_vec3f(_Center.x + _MinorRadius * cos(gs_to_radians(angle)), _Center.y + _MajorRadius * sin(gs_to_radians(angle)), _Center.z);
                    push_line(p1, p2, _LineWidth, _ItemColor, _Transform);
                }
            }

            void push_triangle(
                const gs_vec3f& _P1,
                const gs_vec3f& _P2,
                const gs_vec3f& _P3,
                const float&    _LineWidth,
                const gs_vec4f& _ItemColor,
                const gs_mat4f& _Transform)
            {
                push_line(_P1, _P2, _LineWidth, _ItemColor, _Transform);
                push_line(_P2, _P3, _LineWidth, _ItemColor, _Transform);
                push_line(_P3, _P1, _LineWidth, _ItemColor, _Transform);
            }

            void push_rectangle(
                const gs_vec3f& _Min,
                const gs_vec3f& _Max,
                const float&    _LineWidth,
                const gs_vec4f& _ItemColor,
                const gs_mat4f& _Transform)
            {
                const gs_vec3f _P1 = gs_vec3f(_Min.x, _Min.y, gs_min(_Min.z, _Max.z));
                const gs_vec3f _P2 = gs_vec3f(_Max.x, _Min.y, gs_min(_Min.z, _Max.z));
                const gs_vec3f _P3 = gs_vec3f(_Max.x, _Max.y, gs_min(_Min.z, _Max.z));
                const gs_vec3f _P4 = gs_vec3f(_Min.x, _Max.y, gs_min(_Min.z, _Max.z));
                push_line(_P1, _P2, _LineWidth, _ItemColor, _Transform);
                push_line(_P2, _P3, _LineWidth, _ItemColor, _Transform);
                push_line(_P3, _P4, _LineWidth, _ItemColor, _Transform);
                push_line(_P4, _P1, _LineWidth, _ItemColor, _Transform);
            }

            void push_rectangle_rounded(
                const gs_vec3f& _Min,
                const gs_vec3f& _Max,
                const float&    _Radius,
                const float&    _LineWidth,
                const gs_vec4f& _ItemColor,
                const gs_mat4f& _Transform)
            {
                if(_Radius <= gs_max(_LineWidth, 4.f))
                {
                    push_rectangle(_Min, _Max, _LineWidth, _ItemColor, _Transform);
                    return;
                }

                // compute radius
                float radius = gs_min(gs_min(_Radius, gs_abs(_Max.x - _Min.x) * 0.5f), gs_min(_Radius, gs_abs(_Max.y - _Min.y) * 0.5f));

                // auxiliary lambdas
                auto arc_point = [](
                    const gs_vec3f& _Center,
                    const float&    _MinorRadius,
                    const float&    _MajorRadius,
                    const float&    _ArcAngle
                )->gs_vec3f
                {
                    return gs_vec3f(
                        _Center.x + _MinorRadius * cos(gs_to_radians(_ArcAngle)),
                        _Center.y + _MajorRadius * sin(gs_to_radians(_ArcAngle)),
                        _Center.z);
                };

                // points
                gs_vec3f TL = gs_vec3f(_Min.x + radius, _Max.y - radius, _Max.z);
                gs_vec3f BL = gs_vec3f(_Min.x + radius, _Min.y + radius, _Max.z);
                gs_vec3f TR = gs_vec3f(_Max.x - radius, _Max.y - radius, _Max.z);
                gs_vec3f BR = gs_vec3f(_Max.x - radius, _Min.y + radius, _Max.z);

                // sides
                push_arc(TL, radius, radius, 90.f, 180.f, _LineWidth, _ItemColor, _Transform);
                push_arc(BL, radius, radius, 180.f, 270.f, _LineWidth, _ItemColor, _Transform);
                push_arc(TR, radius, radius, 0.f, 90.f, _LineWidth, _ItemColor, _Transform);
                push_arc(BR, radius, radius, 270.f, 360.f, _LineWidth, _ItemColor, _Transform);
                push_line(arc_point(TL, radius, radius, 180), arc_point(BL, radius, radius, 180), _LineWidth, _ItemColor, _Transform);
                push_line(arc_point(TL, radius, radius, 90), arc_point(TR, radius, radius, 90), _LineWidth, _ItemColor, _Transform);
                push_line(arc_point(TR, radius, radius, 0), arc_point(BR, radius, radius, 0), _LineWidth, _ItemColor, _Transform);
                push_line(arc_point(BL, radius, radius, 270), arc_point(BR, radius, radius, 270), _LineWidth, _ItemColor, _Transform);
            }

            gs_mat4f Transform = gs_mat4f(1.f);

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