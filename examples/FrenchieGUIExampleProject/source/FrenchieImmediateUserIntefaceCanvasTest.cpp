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
        
        if(m_UI->begin_horizontal_stack(
            m_UI->next_id("Stack"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
        {
            m_UI->color_picker_rgba(
                m_UI->next_id("ColorPicker"),
                m_ColorPickerColor,
                ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane
                | ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB);

            if(m_UI->begin_canvas(m_UI->next_id("Canvas")))
            {
                const float clockRadius = gs_min(m_UI->current_bounding_box().width(), m_UI->current_bounding_box().height()) * 0.5f;

                bool frameHovered =
                    gs_2d_ellipsef(m_UI->current_bounding_box().center(), clockRadius).contains(m_UI->m_Renderer->get_cursor_postion()) &&
                    !gs_2d_ellipsef(m_UI->current_bounding_box().center(), clockRadius - 8.f).contains(m_UI->m_Renderer->get_cursor_postion());

                bool backgroundHovered =
                    gs_2d_ellipsef(m_UI->current_bounding_box().center(), clockRadius - 8.f).contains(m_UI->m_Renderer->get_cursor_postion());

                // pick colors
                if(m_UI->drop().has_value() && backgroundHovered)
                    m_ClockBackgroundColor = std::any_cast<gs_color>(m_UI->drop());

                if(m_UI->drop().has_value() && frameHovered)
                    m_ClockFrameColor = std::any_cast<gs_color>(m_UI->drop());

                // render frame
                m_UI->m_Renderer->push_arc_filled(
                    m_UI->current_bounding_box().center(),
                    clockRadius,
                    clockRadius,
                    0.f,
                    360.f,
                    frameHovered && m_UI->dragging()  ? gs_color_rgba(
                        gs_clamp<gs_color>(gs_color_rgba_get_r(m_ClockFrameColor) * 2, 0, 255),
                        gs_clamp<gs_color>(gs_color_rgba_get_g(m_ClockFrameColor) * 2, 0, 255),
                        gs_clamp<gs_color>(gs_color_rgba_get_b(m_ClockFrameColor) * 2, 0, 255),
                        200) : m_ClockFrameColor,
                    m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()));

                // render background
                m_UI->m_Renderer->push_arc_filled(
                    m_UI->current_bounding_box().center(),
                    clockRadius - 8.f,
                    clockRadius - 8.f,
                    0.f,
                    360.f,
                    backgroundHovered && m_UI->dragging()  ? gs_color_rgba(
                        gs_clamp<gs_color>(gs_color_rgba_get_r(m_ClockBackgroundColor) * 2, 0, 255),
                        gs_clamp<gs_color>(gs_color_rgba_get_g(m_ClockBackgroundColor) * 2, 0, 255),
                        gs_clamp<gs_color>(gs_color_rgba_get_b(m_ClockBackgroundColor) * 2, 0, 255),
                        200) : m_ClockBackgroundColor,
                    m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()));

                // hours
                {
                    const float deltaAngle  = 360.f / 12.f;
                    const float sourceAngle = 0.f;
                    const float targetAngle = 360.f;
                    
                    for (float angle = gs_min(sourceAngle, targetAngle); angle < gs_max(sourceAngle, targetAngle); angle += deltaAngle)
                    {
                        // ticks
                        m_UI->m_Renderer->push_line(
                            m_UI->current_bounding_box().center() + clockRadius * gs_vec2f(cos(gs_to_radians(angle)), sin(gs_to_radians(angle))) * 0.9f, 
                            m_UI->current_bounding_box().center() + clockRadius * gs_vec2f(cos(gs_to_radians(angle)), sin(gs_to_radians(angle))),
                            12.f,
                            frameHovered && m_UI->dragging()  ? gs_color_rgba(
                                gs_clamp<gs_color>(gs_color_rgba_get_r(m_ClockFrameColor) * 2, 0, 255),
                                gs_clamp<gs_color>(gs_color_rgba_get_g(m_ClockFrameColor) * 2, 0, 255),
                                gs_clamp<gs_color>(gs_color_rgba_get_b(m_ClockFrameColor) * 2, 0, 255),
                                200) : m_ClockFrameColor,
                            m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()));

                        // text
                        std::string text = Frenchie::Core::String::to_string<int>((int)(13.f - (targetAngle - angle) / targetAngle * 12.f));
                        float fontSize = 64.f * clockRadius / 512.f;

                        m_UI->m_Renderer->push_text(
                            m_UI->current_bounding_box().center() + clockRadius * gs_vec2f(cos(gs_to_radians(angle + 300.f)), sin(gs_to_radians(angle + 300.f))) * 0.8f -
                                m_UI->m_Renderer->calculate_bounding_box(text.begin(), text.end(), fontSize, ApplicationRenderingBackendFont()).size() * 0.5f,
                            text.begin(),
                            text.end(),
                            fontSize,
                            frameHovered && m_UI->dragging() ? gs_color_rgba(
                                gs_clamp<gs_color>(gs_color_rgba_get_r(m_ClockFrameColor) * 2, 0, 255),
                                gs_clamp<gs_color>(gs_color_rgba_get_g(m_ClockFrameColor) * 2, 0, 255),
                                gs_clamp<gs_color>(gs_color_rgba_get_b(m_ClockFrameColor) * 2, 0, 255),
                                200) : m_ClockFrameColor,
                            m_UI->m_Renderer->calculate_transform_matrix(
                                (float)m_UI->current_canvas_place_in_follow()));
                    }
                }

                // minutes
                {
                    const float deltaAngle  = 360.f / 60.f;
                    const float sourceAngle = 0.f;
                    const float targetAngle = 360.f;
                    const float radius      = gs_min(m_UI->current_bounding_box().width(), m_UI->current_bounding_box().height()) * 0.5f;
                    
                    for (float angle = gs_min(sourceAngle, targetAngle); angle < gs_max(sourceAngle, targetAngle); angle += deltaAngle)
                    {
                        m_UI->m_Renderer->push_line(
                            m_UI->current_bounding_box().center() + radius * gs_vec2f(cos(gs_to_radians(angle)), sin(gs_to_radians(angle))) * 0.95f, 
                            m_UI->current_bounding_box().center() + radius * gs_vec2f(cos(gs_to_radians(angle)), sin(gs_to_radians(angle))),
                            12.f,
                            frameHovered && m_UI->dragging()  ? gs_color_rgba(
                                gs_clamp<gs_color>(gs_color_rgba_get_r(m_ClockFrameColor) * 2, 0, 255),
                                gs_clamp<gs_color>(gs_color_rgba_get_g(m_ClockFrameColor) * 2, 0, 255),
                                gs_clamp<gs_color>(gs_color_rgba_get_b(m_ClockFrameColor) * 2, 0, 255),
                                200) : m_ClockFrameColor,
                            m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()));
                    }
                }

                // arrows
                {
                    m_UI->m_Renderer->push_arc_filled(
                        m_UI->current_bounding_box().center(),
                        16.f,
                        16.f,
                        0.f,
                        360.f,
                        frameHovered && m_UI->dragging()  ? gs_color_rgba(
                            gs_clamp<gs_color>(gs_color_rgba_get_r(m_ClockFrameColor) * 2, 0, 255),
                            gs_clamp<gs_color>(gs_color_rgba_get_g(m_ClockFrameColor) * 2, 0, 255),
                            gs_clamp<gs_color>(gs_color_rgba_get_b(m_ClockFrameColor) * 2, 0, 255),
                            200) : m_ClockFrameColor,
                        m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()));

                    // hours
                    float hourAngle = (float)(Frenchie::Core::Clock::local_time_hour() - 13) / 12.f * 360.f + 300.f;
                    
                    m_UI->m_Renderer->push_arrow(
                        m_UI->current_bounding_box().center(),
                        m_UI->current_bounding_box().center() + clockRadius * gs_vec2f(cos(gs_to_radians(hourAngle)), sin(gs_to_radians(hourAngle))) * 0.5f,
                        12.f,
                        32.f,
                        frameHovered && m_UI->dragging()  ? gs_color_rgba(
                            gs_clamp<gs_color>(gs_color_rgba_get_r(m_ClockFrameColor) * 2, 0, 255),
                            gs_clamp<gs_color>(gs_color_rgba_get_g(m_ClockFrameColor) * 2, 0, 255),
                            gs_clamp<gs_color>(gs_color_rgba_get_b(m_ClockFrameColor) * 2, 0, 255),
                            200) : m_ClockFrameColor,
                         m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()));

                    // minutes
                    float minuteAngle = (float)(Frenchie::Core::Clock::local_time_minute() - 65) / 60.f * 360.f + 300.f;
                    
                    m_UI->m_Renderer->push_arrow(
                        m_UI->current_bounding_box().center(),
                        m_UI->current_bounding_box().center() + clockRadius * gs_vec2f(cos(gs_to_radians(minuteAngle)), sin(gs_to_radians(minuteAngle))) * 0.6f,
                        12.f,
                        32.f,
                        frameHovered && m_UI->dragging()  ? gs_color_rgba(
                            gs_clamp<gs_color>(gs_color_rgba_get_r(m_ClockFrameColor) * 2, 0, 255),
                            gs_clamp<gs_color>(gs_color_rgba_get_g(m_ClockFrameColor) * 2, 0, 255),
                            gs_clamp<gs_color>(gs_color_rgba_get_b(m_ClockFrameColor) * 2, 0, 255),
                            200) : m_ClockFrameColor,
                         m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()));

                    // seconds
                    float secondsAngle = (float)(Frenchie::Core::Clock::local_time_second() - 65) / 60.f * 360.f + 300.f;
                    
                    m_UI->m_Renderer->push_arrow(
                        m_UI->current_bounding_box().center(),
                        m_UI->current_bounding_box().center() + clockRadius * gs_vec2f(cos(gs_to_radians(secondsAngle)), sin(gs_to_radians(secondsAngle))) * 0.7f,
                        4.f,
                        32.f,
                        frameHovered && m_UI->dragging()  ? gs_color_rgba(
                            gs_clamp<gs_color>(gs_color_rgba_get_r(m_ClockFrameColor) * 2, 0, 255),
                            gs_clamp<gs_color>(gs_color_rgba_get_g(m_ClockFrameColor) * 2, 0, 255),
                            gs_clamp<gs_color>(gs_color_rgba_get_b(m_ClockFrameColor) * 2, 0, 255),
                            200) : m_ClockFrameColor,
                        m_UI->m_Renderer->calculate_transform_matrix((float)m_UI->current_canvas_place_in_follow()));
                }

                m_UI->end_canvas();
            }

            m_UI->end_horizontal_stack();
        }

        m_UI->end_window();
    }
}