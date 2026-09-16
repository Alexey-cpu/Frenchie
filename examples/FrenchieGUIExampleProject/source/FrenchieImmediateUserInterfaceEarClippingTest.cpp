#include <FrenchieImmediateUserInterfaceEarClippingTest.hpp>


using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceEarClippingTest::FrenchieImmediateUserInterfaceEarClippingTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceEarClippingTest)){}
FrenchieImmediateUserInterfaceEarClippingTest::~FrenchieImmediateUserInterfaceEarClippingTest(){}

bool FrenchieImmediateUserInterfaceEarClippingTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    if(m_Scene == nullptr)
        m_Scene = Frenchie::Application::App::push_layer<Frenchie::Application::RenderingQueue2D>();

    return m_UI != nullptr && m_Scene != nullptr;
}

void FrenchieImmediateUserInterfaceEarClippingTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Framebuffers test window", "Framebuffers test window"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_horizontal_stack(m_UI->next_id("Framebuffers")))
        {
            if(m_UI->begin_canvas(m_UI->next_id("Canvas")))
            {
                gs_2d_boxf boundingBox    = m_UI->current_bounding_box();
                gs_vec2f   cursorPosition = m_UI->m_Input.get_cusor_position();

                if(m_UI->is_current_node_mouse_double_clicked())
                {
                    m_Points.push_back(cursorPosition);
                    m_Colors.push_back(gs_color_rgb(255, 255, 255));
                }

                m_UI->m_Renderer->push_poly_filled(
                    m_Points.data(),
                    m_Colors.data(),
                    m_Points.size(),
                    m_UI->m_Renderer->calculate_transform_matrix(m_UI->current_place_in_follow()));

                for (int i = 0; i < (int)m_Points.size(); i++)
                {
                    gs_2d_ellipsef ellipse(m_Points[i], 16.f);

                    m_UI->m_Renderer->push_arc_filled(
                        m_Points[i],
                        ellipse.MinorRadius,
                        ellipse.MajorRadius,
                        0.f,
                        360.f,
                        m_Colors[i],
                        m_UI->m_Renderer->calculate_transform_matrix(m_UI->current_place_in_follow()));

                    if(m_UI->m_Input.is_mouse_button_down() && ellipse.contains(cursorPosition) && m_Moving < 0)
                        m_Moving = i;
                }

                if(!m_UI->m_Input.is_mouse_button_down())
                    m_Moving = -1;

                if(m_Moving >= 0)
                    m_Points[m_Moving] = cursorPosition;

                m_UI->end_canvas();
            }

            m_UI->image(m_UI->next_id("FrameBuffer"), gs_color_rgb(255, 255, 255), m_Scene->get_framebuffer_texture());

            m_UI->end_horizontal_stack();
        }
        m_UI->end_window();
    }

    // scene 1
    {
        int depth = 0;

        m_Scene->render_to_texture();

        m_Scene->push_rectangle_filled(
            m_Scene->current_viewport().Min,
            m_Scene->current_viewport().Max,
            gs_color_rgb(128, 128, 128),
            m_Scene->calculate_transform_matrix((float)depth++));

        m_Scene->push_mesh_rendering_hints(ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Lines);
        
        m_Scene->push_poly_filled(m_Points.data(), m_Colors.data(), m_Points.size(), m_Scene->calculate_transform_matrix((float)depth++));
    }
}

bool FrenchieImmediateUserInterfaceEarClippingTest::allows_multiple_instances() const
{
    return false;
}