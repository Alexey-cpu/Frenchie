#include <FrenchieImmediateUserInterfaceEarClippingTest.hpp>


using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceEarClippingTest::FrenchieImmediateUserInterfaceEarClippingTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceEarClippingTest)){}
FrenchieImmediateUserInterfaceEarClippingTest::~FrenchieImmediateUserInterfaceEarClippingTest(){}

bool FrenchieImmediateUserInterfaceEarClippingTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceEarClippingTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Ear clipping test window", "EarClippingTestWindow"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_vertical_stack(m_UI->next_id("Framebuffers")))
        {
            // editor
            if(m_UI->begin_scrollarea(m_UI->next_id("Editor"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
            {
                m_UI->check_box(m_UI->next_id("FilledMeshTick"), m_Filled);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("FilledMeshLabel"), "Filled mesh");

                m_UI->check_box(m_UI->next_id("WiresTick"), m_Wires);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("WiresTickLabel"), "Wire mode");

                m_UI->input_scalar_slider(m_UI->next_id("MeshRadiusSlider"), m_Radius, 0.f, 512.f);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("MeshRadiusLabel"), "Mesh rounding radius");

                m_UI->end_scrollarea();
            }

            // canvas
            if(m_UI->begin_canvas(m_UI->next_id("Canvas")))
            {
                gs_2d_boxf boundingBox    = m_UI->current_bounding_box();
                gs_vec2f   cursorPosition = m_UI->input().get_cusor_position();

                if(m_UI->is_current_node_mouse_double_clicked())
                {
                    m_Points.push_back(cursorPosition);
                    m_Colors.push_back(gs_color_rgb(255, 255, 255));
                }

                if(m_Wires)
                    m_UI->m_Renderer->push_mesh_rendering_hints(ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Lines);

                if(m_Filled)
                {
                    m_UI->m_Renderer->push_poly_filled(
                        m_Points.data(),
                        m_Colors.data(),
                        m_Points.size(),
                        m_UI->m_Renderer->calculate_transform_matrix(m_UI->current_place_in_follow()),
                        m_Radius);
                }
                else
                {
                    m_UI->m_Renderer->push_poly(
                        m_Points.data(),
                        m_Colors.empty() ? gs_color_rgb(0, 0, 0) : m_Colors[0],
                        m_Points.size(),
                        12.f,
                        m_UI->m_Renderer->calculate_transform_matrix(m_UI->current_place_in_follow()),
                        m_Radius);
                }

                if(m_Wires)
                    m_UI->m_Renderer->pop_mesh_rendering_hints();

                for (int i = 0; i < (int)m_Points.size(); i++)
                {
                    gs_2d_ellipsef ellipse(m_Points[i], 16.f);

                    m_UI->m_Renderer->push_arc_filled(
                        m_Points[i],
                        ellipse.MinorRadius,
                        ellipse.MajorRadius,
                        0.f,
                        360.f,
                        gs_color_rgb(255, 0, 0),
                        m_UI->m_Renderer->calculate_transform_matrix(m_UI->current_place_in_follow()));

                    if(m_UI->input().is_mouse_button_down() && ellipse.contains(cursorPosition) && m_Moving < 0)
                        m_Moving = i;
                }

                if(!m_UI->input().is_mouse_button_down())
                    m_Moving = -1;

                if(m_Moving >= 0)
                    m_Points[m_Moving] = cursorPosition;

                m_UI->end_canvas();
            }

            m_UI->end_vertical_stack();
        }
        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterfaceEarClippingTest::allows_multiple_instances() const
{
    return false;
}