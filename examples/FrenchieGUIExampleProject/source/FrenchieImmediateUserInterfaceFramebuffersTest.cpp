#include <FrenchieImmediateUserInterfaceFramebuffersTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceFramebuffersTest::FrenchieImmediateUserInterfaceFramebuffersTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceFramebuffersTest)){}
FrenchieImmediateUserInterfaceFramebuffersTest::~FrenchieImmediateUserInterfaceFramebuffersTest(){}

bool FrenchieImmediateUserInterfaceFramebuffersTest::awake()
{
    if(m_Renderer1 == nullptr)
        m_Renderer1 = Frenchie::Application::App::push_layer<Frenchie::Application::RenderingQueue2D>();

    if(m_Renderer2 == nullptr)
        m_Renderer2 = Frenchie::Application::App::push_layer<Frenchie::Application::RenderingQueue2D>();

    // загружает слой UI приложение
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return m_UI != nullptr && m_Renderer1 != nullptr && m_Renderer2 != nullptr;
}

void FrenchieImmediateUserInterfaceFramebuffersTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Framebuffers test window", "FramebuffersTestWindow"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_vertical_stack(m_UI->next_id("Root")))
        {
            if(m_UI->begin_scrollarea(
                m_UI->next_id("Settings"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
            {
                m_UI->input_scalar(m_UI->next_id("FrameBufferWidth"), m_FrameBufferSize.x, 128.f, 2048.f);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("FrameBufferWidthLabel"), "frame buffer width");

                m_UI->input_scalar(m_UI->next_id("FrameBufferHeight"), m_FrameBufferSize.y, 128.f, 2048.f);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("FrameBufferHeightLabel"), "frame buffer height");

                m_UI->end_scrollarea();
            }

            m_UI->next_content_padding(gs_vec4f(0.f, 16.f, 0.f, 0.f));

            if(m_UI->begin_horizontal_stack(
                m_UI->next_id("Contents"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
            {
                m_UI->custom_widget(
                    m_UI->next_id("Scene-1"),
                    [](ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node){},
                    [](ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node){},
                    [this](ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
                    {
                        auto frameBuffer = m_Renderer1->get_framebuffer_texture();
                        auto viewportTex = ApplicationRenderingBackendTexture(
                            frameBuffer.Ptr,
                            m_FrameBufferSize.x,
                            m_FrameBufferSize.y,
                            frameBuffer.Color,
                            frameBuffer.Format,
                            ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToBorder,
                            frameBuffer.Filter);

                        gs_vec2f points[] =
                        {
                            gs_vec2f(_Node->State.BoundingBox.Min.x, _Node->State.BoundingBox.Min.y),
                            gs_vec2f(_Node->State.BoundingBox.Max.x, _Node->State.BoundingBox.Min.y),
                            gs_vec2f(_Node->State.BoundingBox.Max.x, _Node->State.BoundingBox.Max.y),
                            gs_vec2f(_Node->State.BoundingBox.Min.x, _Node->State.BoundingBox.Max.y)
                        };

                        gs_color colors[] =
                        {
                            gs_color_rgb(255, 255, 255),
                            gs_color_rgb(255, 255, 255),
                            gs_color_rgb(255, 255, 255),
                            gs_color_rgb(255, 255, 255)
                        };

                        auto texSize = gs_vec2f(viewportTex.Width, viewportTex.Height);

                        gs_vec2f uvs[] =
                        {
                            (points[0] - points[0]) / texSize,
                            (points[1] - points[0]) / texSize,
                            (points[2] - points[0]) / texSize,
                            (points[3] - points[0]) / texSize
                        };

                        _Context->renderer()->build_poly_mesh_filled(points, colors, uvs, sizeof(points) / sizeof(points[0]));

                        _Context->renderer()->push_rendering_command(
                            viewportTex,
                            gs_color_rgb(255, 255, 255),
                            _Context->renderer()->calculate_transform_matrix(_Node->place_in_follow()));
                    }
                );

                m_UI->custom_widget(
                    m_UI->next_id("Scene-2"),
                    [](ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node){},
                    [](ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node){},
                    [this](ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
                    {
                        auto frameBuffer = m_Renderer2->get_framebuffer_texture();
                        auto viewportTex = ApplicationRenderingBackendTexture(
                            frameBuffer.Ptr,
                            m_FrameBufferSize.x,
                            m_FrameBufferSize.y,
                            frameBuffer.Color,
                            frameBuffer.Format,
                            ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToBorder,
                            frameBuffer.Filter);

                        gs_vec2f points[] =
                        {
                            gs_vec2f(_Node->State.BoundingBox.Min.x, _Node->State.BoundingBox.Min.y),
                            gs_vec2f(_Node->State.BoundingBox.Max.x, _Node->State.BoundingBox.Min.y),
                            gs_vec2f(_Node->State.BoundingBox.Max.x, _Node->State.BoundingBox.Max.y),
                            gs_vec2f(_Node->State.BoundingBox.Min.x, _Node->State.BoundingBox.Max.y)
                        };

                        gs_color colors[] =
                        {
                            gs_color_rgb(255, 255, 255),
                            gs_color_rgb(255, 255, 255),
                            gs_color_rgb(255, 255, 255),
                            gs_color_rgb(255, 255, 255)
                        };

                        auto texSize = gs_vec2f(viewportTex.Width, viewportTex.Height);

                        gs_vec2f uvs[] =
                        {
                            (points[0] - points[0]) / texSize,
                            (points[1] - points[0]) / texSize,
                            (points[2] - points[0]) / texSize,
                            (points[3] - points[0]) / texSize
                        };

                        _Context->renderer()->build_poly_mesh_filled(points, colors, uvs, sizeof(points) / sizeof(points[0]));

                        _Context->renderer()->push_rendering_command(
                            viewportTex,
                            gs_color_rgb(255, 255, 255),
                            _Context->renderer()->calculate_transform_matrix(_Node->place_in_follow()));
                    }
                );

                m_UI->end_horizontal_stack();
            }

            m_UI->end_vertical_stack();
        }

        m_UI->next_content_padding(gs_vec4f(0.f, 16.f, 0.f, 0.f));

        m_UI->end_window();
    }

    // scene 1
    {
        // render ball
        m_Renderer1->render_to_texture();

        int depth = 0;

        m_Renderer1->push_rectangle_filled(
            m_Renderer1->current_viewport().Min,
            m_Renderer1->current_viewport().Max,
            gs_color_rgb(32, 16, 16),
            m_Renderer1->calculate_transform_matrix((float)depth++));

        gs_2d_boxf box                   = m_Renderer1->current_viewport();
        float      ballRadius            = gs_max(m_Ball.MajorRadius, m_Ball.MinorRadius);
        gs_vec2f   ballToCenterDirection = gs_vector_normalize(box.center() - m_Ball.Center);

        if(!box.contains(m_Ball.Center + ballRadius * ballToCenterDirection * -1.f) || !m_Direction.has_value())
            m_Direction = gs_vector_normalize(ballToCenterDirection + gs_vec2f(gs_pseudo_random(-1.f, +1.f), gs_pseudo_random(-1.f, +1.f)));
        m_Ball.Center += m_Direction.value() * 8.f;

        m_Renderer1->push_arc_filled(
            m_Ball.Center,
            m_Ball.MinorRadius,
            m_Ball.MajorRadius,
            0.f,
            360.f,
            gs_color_rgb(128, 128, 128),
            m_Renderer1->calculate_transform_matrix((float)depth++));
    }

    // scene-2
    {
        m_Renderer2->render_to_texture();

        int depth = 0;

        m_Renderer2->push_rectangle_filled(
            m_Renderer2->current_viewport().Min,
            m_Renderer2->current_viewport().Max,
            gs_color_rgb(128, 128, 128),
            m_Renderer2->calculate_transform_matrix((float)depth++));

        gs_2d_boxf box                   = m_Renderer2->current_viewport();
        float      ballRadius            = gs_max(m_Ball.MajorRadius, m_Ball.MinorRadius);
        gs_vec2f   ballToCenterDirection = gs_vector_normalize(box.center() - m_Ball.Center);

        if(!box.contains(m_Ball.Center + ballRadius * ballToCenterDirection * -1.f) || !m_Direction.has_value())
            m_Direction = gs_vector_normalize(ballToCenterDirection + gs_vec2f(gs_pseudo_random(-1.f, +1.f), gs_pseudo_random(-1.f, +1.f)));
        m_Ball.Center += m_Direction.value() * 8.f;

        m_Renderer2->push_arc_filled(
            m_Ball.Center,
            m_Ball.MinorRadius,
            m_Ball.MajorRadius,
            0.f,
            360.f,
            gs_color_rgb(32, 32, 128),
            m_Renderer2->calculate_transform_matrix((float)depth++));
    }
}

bool FrenchieImmediateUserInterfaceFramebuffersTest::allows_multiple_instances() const
{
    return false;
}