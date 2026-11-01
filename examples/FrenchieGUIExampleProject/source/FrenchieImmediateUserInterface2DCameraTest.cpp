#include <FrenchieImmediateUserInterface2DCameraTest.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;

FrenchieImmediateUserInterface2DCameraTest::FrenchieImmediateUserInterface2DCameraTest() : Layer(STRINGIFY(FrenchieImmediateUserInterface2DCameraTest)){}
FrenchieImmediateUserInterface2DCameraTest::~FrenchieImmediateUserInterface2DCameraTest(){}

bool FrenchieImmediateUserInterface2DCameraTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterface>();

    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::App::push_layer<Frenchie::Application::RenderingQueue2D>();

    return m_UI != nullptr && m_Renderer != nullptr;
}

void FrenchieImmediateUserInterface2DCameraTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("2D Camera", "2D Camera"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_vertical_stack(m_UI->next_id("Framebuffers")))
        {
            // editor
            if(m_UI->begin_scrollarea(
                m_UI->next_id("Editor"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
            {
                m_UI->label(m_UI->next_id("CameraOffset"), "Camera offset");
                m_UI->input_scalar(m_UI->next_id("CameraOffsetValueX"), m_Renderer->get_camera().Offset.x, -2048.f, +2048.f, 10.f);
                m_UI->input_scalar(m_UI->next_id("CameraOffsetValueY"), m_Renderer->get_camera().Offset.y, -2048.f, +2048.f, 10.f);

                m_UI->label(m_UI->next_id("CameraScale"), "Camera scale");
                m_UI->input_scalar(m_UI->next_id("CameraScaleValue"), m_Renderer->get_camera().Scale, 0.05f, 4.f, 5.f);

                m_UI->label(m_UI->next_id("AmbientLightStrengthLabel"), "Ambient light strenth");
                m_UI->input_scalar_slider(m_UI->next_id("AmbientLightStrength"), m_AmbientStrength, 0.f, 1.f);

                m_UI->end_scrollarea();
            }

            // canvas
            m_UI->custom_widget(
                m_UI->next_id("Canvas"),
                [](ImmediateUserInterface*, ImmediateUserInterfaceNode*){},
                [](ImmediateUserInterface*, ImmediateUserInterfaceNode*){},
                [this](ImmediateUserInterface* _Context, ImmediateUserInterfaceNode* _Node)
                {
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

                    auto texSize = gs_vec2f(m_Renderer->get_framebuffer_texture().Width, m_Renderer->get_framebuffer_texture().Height);

                    gs_vec2f uvs[] =
                    {
                        (points[0] - points[0]) / texSize,
                        (points[1] - points[0]) / texSize,
                        (points[2] - points[0]) / texSize,
                        (points[3] - points[0]) / texSize
                    };

                    _Context->renderer()->build_poly_mesh_filled(points, colors, uvs, sizeof(points) / sizeof(points[0]));

                    _Context->renderer()->push_rendering_command(
                        m_Renderer->get_framebuffer_texture(),
                        gs_color_rgb(255, 255, 255),
                        _Context->renderer()->calculate_transform_matrix(_Node->place_in_follow()));
                });

            m_UI->end_vertical_stack();
        }
        m_UI->end_window();
    }

    m_Renderer->render_to_texture();

    int depth = 0;

    m_Renderer->ambient_light(gs_color_rgb(255 * m_AmbientStrength, 255 * m_AmbientStrength, 255 * m_AmbientStrength));

    m_Renderer->push_rectangle_filled(
        m_Renderer->current_viewport().Min,
        m_Renderer->current_viewport().Max,
        gs_color_rgba(32, 32, 32, 255),
        m_Renderer->calculate_transform_matrix((float)depth++));

    m_Renderer->push_rectangle_filled(
        gs_vec2f(0.f, 0.f),
        gs_vec2f(256.f, 256.f),
        gs_color_rgb(255, 0, 0),
        m_Renderer->calculate_transform_matrix((float)depth++));

    m_Renderer->push_rectangle_filled(
        gs_vec2f(512.f, 512.f),
        gs_vec2f(512.f, 512.f) + gs_vec2f(256.f, 256.f),
        gs_color_rgb(0, 255, 0),
        m_Renderer->calculate_transform_matrix((float)depth++));
}

void FrenchieImmediateUserInterface2DCameraTest::finish()
{
    if(m_Renderer != nullptr)
        m_Renderer->close();
}

bool FrenchieImmediateUserInterface2DCameraTest::allows_multiple_instances() const
{
    return false;
}