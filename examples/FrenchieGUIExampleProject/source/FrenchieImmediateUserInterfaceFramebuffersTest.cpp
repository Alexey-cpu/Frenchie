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
        m_UI->next_id("Framebuffers test window", "Framebuffers test window"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_horizontal_stack(m_UI->next_id("Framebuffers")))
        {
            m_UI->image(m_UI->next_id("Scene-1"), gs_color_rgb(255, 255, 255), m_Renderer1->get_framebuffer_texture());
            m_UI->image(m_UI->next_id("Scene-2"), gs_color_rgb(255, 255, 255), m_Renderer2->get_framebuffer_texture());

            m_UI->end_horizontal_stack();
        }
        m_UI->end_window();
    }

    // scene 1
    {
        // render ball
        m_Renderer1->render_to_texture();

        int depth = 0;

        // рисуем подложку
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

        // рисуем эллипс
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

        m_Renderer2->push_arc_filled(
            m_Renderer2->current_viewport().center(),
            gs_min(m_Renderer2->current_viewport().width(), m_Renderer2->current_viewport().height()) * 0.5f,
            gs_min(m_Renderer2->current_viewport().width(), m_Renderer2->current_viewport().height()) * 0.5f,
            0.f,
            360.f,
            gs_color_rgb(32, 16, 16),
            m_Renderer2->calculate_transform_matrix((float)depth++));
    }
}

bool FrenchieImmediateUserInterfaceFramebuffersTest::allows_multiple_instances() const
{
    return false;
}