#include <FrenchieImmediateUserIntefaceCanvasTest.hpp>

#include <iostream>

using namespace Frenchie::Core;
using namespace Frenchie::Application;

FrenchieImmediateUserIntefaceCanvasTest::FrenchieImmediateUserIntefaceCanvasTest() : Layer(STRINGIFY(FrenchieImmediateUserIntefaceCanvasTest)){}
FrenchieImmediateUserIntefaceCanvasTest::~FrenchieImmediateUserIntefaceCanvasTest(){}

bool FrenchieImmediateUserIntefaceCanvasTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::Application::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return m_UI != nullptr;
}

void FrenchieImmediateUserIntefaceCanvasTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("2D Canvas rendering example", "2DCustomCanvasRenderingTest"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        m_UI->next_content_padding(gs_vec4f(16.f, 16.f, 0.f, 0.f));
        
        if(m_UI->begin_vertical_stack(
            m_UI->next_id("VerticalStack"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
        {
            if(m_UI->begin_scrollarea(
                m_UI->next_id("Settings"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
            {
                m_UI->input_scalar_slider(m_UI->next_id("BallSpeedSlider"), m_BallMoveSpeed, 1.f, 16.f);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("BallSpeedLabel"), Frenchie::Core::String::format("Ball speed - %.f", m_BallMoveSpeed));
                
                m_UI->end_scrollarea();
            }

            if(m_UI->begin_canvas(m_UI->next_id("Canvas")))
            {
                gs_2d_boxf canvasBox = m_UI->current_bounding_box();

                // render canvas frame
                m_UI->m_Renderer->push_rectangle_filled(
                    canvasBox.Min,
                    canvasBox.Max,
                    gs_color_rgb(255, 255, 255),
                    m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()),
                    8.f);

                m_UI->m_Renderer->push_arc_filled(
                    m_Ball.Center,
                    m_Ball.MinorRadius,
                    m_Ball.MajorRadius,
                    0.f,
                    360.f,
                    gs_color_rgb(128, 128, 128),
                    m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()));

                // render ball
                gs_2d_boxf ballCollidingBox = gs_2d_boxf(canvasBox.Min + gs_max(m_Ball.MajorRadius, m_Ball.MinorRadius), canvasBox.Max - gs_max(m_Ball.MajorRadius, m_Ball.MinorRadius));

                if(!ballCollidingBox.contains(m_Ball.Center))
                {
                    m_BallDirection = gs_vector_normalize(
                        gs_matrix_rotate(
                            gs_mat4f(1.f),
                            gs_to_radians(gs_pseudo_random(-30.f, +30.f)),
                            gs_vec3f(0.f, 0.f, 1.f)) * gs_vec4f(m_Ball.Center - canvasBox.center(), 0.f, 1.f)) * (-1.f);
                }

                m_Ball = gs_2d_ellipsef(
                    gs_clamp(m_Ball.Center + m_BallDirection * m_BallMoveSpeed, ballCollidingBox.Min - 4.f, ballCollidingBox.Max + 4.f),
                    gs_max(m_Ball.MajorRadius, m_Ball.MinorRadius));

                m_UI->end_canvas();
            }

            m_UI->end_vertical_stack();
        }

        m_UI->end_window();
    }
}