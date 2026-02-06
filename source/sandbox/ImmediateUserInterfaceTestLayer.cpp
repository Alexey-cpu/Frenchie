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

    if(m_ImmediateUserInterface->begin_window(
        "Window-1",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        create_horizontal_layout("Window-1/Layout");
        m_ImmediateUserInterface->end_window();
    }

    static bool opened = true;
    static std::string button;

    if(m_ImmediateUserInterface->begin_window(
        "Window-2",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable, &opened))
    {
        if(m_ImmediateUserInterface->begin_horizontal_stack("Window-2/Stack"))
        {
            if(m_ImmediateUserInterface->push_button("Button-1")) button = "Button-1";
            if(m_ImmediateUserInterface->push_button("Button-2")) button = "Button-2";
            if(m_ImmediateUserInterface->push_button("Button-3")) button = "Button-3";

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
            if(m_ImmediateUserInterface->push_button("Button-4")) button = "Button-4";
            if(m_ImmediateUserInterface->push_button("Button-5")) button = "Button-5";
            if(m_ImmediateUserInterface->push_button("Button-6")) button = "Button-6";

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

    if(m_ImmediateUserInterface->begin_window(
        "Window-5###Window-6",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {        
        m_ImmediateUserInterface->end_window();
    }

    m_ImmediateUserInterface->m_Renderer->push_text(
        std::string("FPS ").append(std::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().FrameRate)),
        64.f,
        RenderingQueueGraphicsApi::construct_rgba_color(255, 0, 0, 255),
        m_ImmediateUserInterface->m_Renderer->calculate_transform_matrix(
            m_ImmediateUserInterface->m_Renderer->get_far_plane()
        ));

    m_ImmediateUserInterface->m_Renderer->push_text(
        std::string("Commands ").append(std::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderingCommandsCount)),
        64.f,
        RenderingQueueGraphicsApi::construct_rgba_color(255, 0, 0, 255),
        m_ImmediateUserInterface->m_Renderer->calculate_transform_matrix(
            m_ImmediateUserInterface->m_Renderer->get_far_plane(),
            gs_vec2f(0.f, 64.f)
        ));

    m_ImmediateUserInterface->m_Renderer->push_text(
        std::string("Triangles ").append(std::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderedTrianglesCount)),
        64.f,
        RenderingQueueGraphicsApi::construct_rgba_color(255, 0, 0, 255),
        m_ImmediateUserInterface->m_Renderer->calculate_transform_matrix(
            m_ImmediateUserInterface->m_Renderer->get_far_plane(),
            gs_vec2f(0.f, 128.f)
        ));

    // m_ImmediateUserInterface->m_Renderer->push_text(
    //     std::string("Pressed ").append(button),
    //     64.f,
    //     RenderingQueueGraphicsApi::construct_rgba_color(255, 0, 0, 255),
    //     m_ImmediateUserInterface->m_Renderer->calculate_transform_matrix(
    //         m_ImmediateUserInterface->m_Renderer->get_far_plane(),
    //         gs_vec2f(0.f, 256.f)
    //     )
    // );

    // m_ImmediateUserInterface->m_Renderer->push_rectangle_rounded(
    //     gs_vec2f(512.f, 512.f),
    //     gs_vec2f(1024.f, 1024.f),
    //     0.f,
    //     0.f,
    //     RenderingQueueGraphicsApi::construct_rgba_color(255, 0, 0, 255),
    //     m_ImmediateUserInterface->m_Renderer->calculate_transform_matrix(
    //         m_ImmediateUserInterface->m_Renderer->get_far_plane())
    // );
}

void ImmediateUserInterfaceTestLayer::finish()
{
    if(m_ImmediateUserInterface != nullptr)
        m_ImmediateUserInterface->close();
}