#include <ImmediateUserInterfaceTestLayer.hpp>

using namespace Frenchie::Application;

ImmediateUserInterfaceTestLayer::ImmediateUserInterfaceTestLayer(){}
ImmediateUserInterfaceTestLayer::~ImmediateUserInterfaceTestLayer(){}

bool ImmediateUserInterfaceTestLayer::awake()
{
    if(m_ImmediateUserInterface == nullptr)
        m_ImmediateUserInterface = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return true;
}

void ImmediateUserInterfaceTestLayer::frame_update()
{
    auto create_horizontal_layout = [this](const std::string& _Name)
    {
        ImmediateUserInterfaceNodeSettings settins =
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;
        
        if(m_ImmediateUserInterface->begin_horizontal_stack(_Name, settins))
        {
            if(m_ImmediateUserInterface->begin_vertial_stack(std::string(_Name).append("/Child-1"), settins))
            {
                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-1"), settins))
                {
                    if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-1/Child-1"), settins))
                        m_ImmediateUserInterface->end_horizontal_stack();

                    if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-1/Child-2"), settins))
                        m_ImmediateUserInterface->end_horizontal_stack();

                    if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-1/Child-3"), settins))
                        m_ImmediateUserInterface->end_horizontal_stack();

                    m_ImmediateUserInterface->end_horizontal_stack();
                }

                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-2"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-3"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                m_ImmediateUserInterface->end_vertical_stack();
            }

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-2"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-3"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-4"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            m_ImmediateUserInterface->end_horizontal_stack();
        }
    };

    auto create_vertical_layout = [this](const std::string& _Name)
    {
        ImmediateUserInterfaceNodeSettings settins =
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;
        
        if(m_ImmediateUserInterface->begin_vertial_stack(_Name, settins))
        {
            if(m_ImmediateUserInterface->begin_vertial_stack(std::string(_Name).append("/Child-1"), settins))
            {
                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-1"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-2"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-3"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                m_ImmediateUserInterface->end_vertical_stack();
            }

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-2"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-3"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-4"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            m_ImmediateUserInterface->end_vertical_stack();
        }
    };

    //create_horizontal_layout("SOMELAYOUT");

    // if(m_ImmediateUserInterface->begin_window(
    //     "Window-1",
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    // {
    //     create_horizontal_layout("Window-1/Layout");
    //     m_ImmediateUserInterface->end_window();
    // }

    static bool opened = true;

    if(m_ImmediateUserInterface->begin_window(
        "Window-2",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable, &opened))
    {
        if(m_ImmediateUserInterface->begin_horizontal_stack("Window-2/Stack"))
        {
            m_ImmediateUserInterface->push_button("Button-1");
            m_ImmediateUserInterface->push_button("Button-2");
            m_ImmediateUserInterface->push_button("Button-3");

            m_ImmediateUserInterface->end_horizontal_stack();
        }
        

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-3",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        if(m_ImmediateUserInterface->begin_vertial_stack("Window-3/Stack"))
        {
            m_ImmediateUserInterface->push_button("Button-4");
            m_ImmediateUserInterface->push_button("Button-5");
            m_ImmediateUserInterface->push_button("Button-6");

            m_ImmediateUserInterface->end_vertical_stack();
        }

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-4",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-5",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {        
        m_ImmediateUserInterface->end_window();
    }

    m_ImmediateUserInterface->m_Renderer->push_text(
        std::string("FPS ").append(std::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().FrameRate)),
        64.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        m_ImmediateUserInterface->m_Renderer->calculate_transform_matrix(
            m_ImmediateUserInterface->m_Renderer->get_far_plane()
        )
    );

    m_ImmediateUserInterface->m_Renderer->push_text(
        std::string("CMD ").append(std::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderingCommandsCount)),
        64.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        m_ImmediateUserInterface->m_Renderer->calculate_transform_matrix(
            m_ImmediateUserInterface->m_Renderer->get_far_plane(),
            gs_vec2f(0.f, 64.f)
        )
    );

    m_ImmediateUserInterface->m_Renderer->push_text(
        std::string("Triangles ").append(std::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderedTrianglesCount)),
        64.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        m_ImmediateUserInterface->m_Renderer->calculate_transform_matrix(
            m_ImmediateUserInterface->m_Renderer->get_far_plane(),
            gs_vec2f(0.f, 128.f)
        )
    );
}

void ImmediateUserInterfaceTestLayer::finish()
{
    if(m_ImmediateUserInterface != nullptr)
        m_ImmediateUserInterface->close();
}