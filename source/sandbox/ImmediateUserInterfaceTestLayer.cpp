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
    //windows_test();   
    //scrollarea_test();
    //panel_test();
    //vertical_stack_test();
    //horizontal_stack_test();

    menu_test();
    
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
}

void ImmediateUserInterfaceTestLayer::finish()
{
    if(m_ImmediateUserInterface != nullptr)
        m_ImmediateUserInterface->close();
}

void ImmediateUserInterfaceTestLayer::windows_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking |ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking;

    static bool opened = true;
    static std::string button;

    if(m_ImmediateUserInterface->begin_window(
        "Window-1",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        if(m_ImmediateUserInterface->begin_scrollarea(
            "Window-1/ScrollArea",
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults |
            ImmediateUserInterfaceScrollAreaSettings_::ImmediateUserInterfaceScrollAreaSettings_Defaults))
        {
            int k = 0;

            for(int i = 0 ; i < 1e1; i++)
            {
                for(int j = 0 ; j < 1e1; j++)
                    m_ImmediateUserInterface->push_button(std::string("Button-").append(std::to_string(k++)), gs_vec2f(512.f, 128.f));

                m_ImmediateUserInterface->next_line();
            }

            m_ImmediateUserInterface->end_scrollarea();
        }

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-2",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable, &opened))
    {
        if(m_ImmediateUserInterface->begin_horizontal_stack("Window-2/Stack"))
        {
            m_ImmediateUserInterface->push_button("Button-1###Window-2/Button-1");
            m_ImmediateUserInterface->push_button("Button-2###Window-2/Button-2");

            m_ImmediateUserInterface->end_horizontal_stack();
        }

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-3",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
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
}

void ImmediateUserInterfaceTestLayer::scrollarea_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_DisableDocking;

    if(m_ImmediateUserInterface->begin_scrollarea(
        "ScrollArea",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults |
        ImmediateUserInterfaceScrollAreaSettings_::ImmediateUserInterfaceScrollAreaSettings_Defaults// |
        // ImmediateUserInterfaceScrollAreaSettings_::ImmediateUserInterfaceScrollAreaSettings_ResizeToContentsVertically |
        // ImmediateUserInterfaceScrollAreaSettings_::ImmediateUserInterfaceScrollAreaSettings_ResizeToContentsHorizontally
    ))
    {
        int k = 0;

        for(int i = 0 ; i < 5; i++)
        {
            for(int j = 0 ; j < 3; j++)
                m_ImmediateUserInterface->push_button(std::string("Button-").append(std::to_string(k++)));

            m_ImmediateUserInterface->next_line();
        }

        m_ImmediateUserInterface->end_scrollarea();
    }
}

void ImmediateUserInterfaceTestLayer::panel_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_DisableDocking;

    if(m_ImmediateUserInterface->begin_panel(
        "Panel-1",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults |
        ImmediateUserInterfaceLayoutAlignmentSettings_::ImmediateUserInterfaceLayoutAlignmentSettings_VerticalAlignTop |
        ImmediateUserInterfaceLayoutAlignmentSettings_::ImmediateUserInterfaceLayoutAlignmentSettings_HorizontalAlignCenter))
    {
        m_ImmediateUserInterface->push_button("Panel-1/Button");
        m_ImmediateUserInterface->end_panel();
    }
}

void ImmediateUserInterfaceTestLayer::vertical_stack_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_DisableDocking;

    if(m_ImmediateUserInterface->begin_vertial_stack(
        "VerticalStack",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults |
        ImmediateUserInterfaceLayoutAlignmentSettings_::ImmediateUserInterfaceLayoutAlignmentSettings_VerticalAlignBottom |
        ImmediateUserInterfaceLayoutAlignmentSettings_::ImmediateUserInterfaceLayoutAlignmentSettings_HorizontalAlignRight))
    {
        for(int i = 0 ; i < 5; i++)
            m_ImmediateUserInterface->push_button(std::string("Button-").append(std::to_string(i)));
        m_ImmediateUserInterface->end_vertical_stack();
    }
}

void ImmediateUserInterfaceTestLayer::horizontal_stack_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_DisableDocking;

    if(m_ImmediateUserInterface->begin_horizontal_stack(
        "VerticalStack",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults |
        ImmediateUserInterfaceLayoutAlignmentSettings_::ImmediateUserInterfaceLayoutAlignmentSettings_VerticalAlignCenter |
        ImmediateUserInterfaceLayoutAlignmentSettings_::ImmediateUserInterfaceLayoutAlignmentSettings_HorizontalAlignCenter))
    {
        for(int i = 0 ; i < 5; i++)
            m_ImmediateUserInterface->push_button(std::string("Button-").append(std::to_string(i)));
        m_ImmediateUserInterface->end_horizontal_stack();
    }
}

void ImmediateUserInterfaceTestLayer::menu_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_DisableDocking;

    if(m_ImmediateUserInterface->begin_window("Window"))
    {
        if(m_ImmediateUserInterface->begin_menu("Menu-1"))
        {
            if(m_ImmediateUserInterface->begin_menu("Menu-2"))
            {
                static bool checked = false;
                std::string name= "Checkbox";
                m_ImmediateUserInterface->checkbox(name, &checked);

                m_ImmediateUserInterface->menu_action("Action-4");
                m_ImmediateUserInterface->menu_action("Action-5");
                m_ImmediateUserInterface->menu_action("Action-6");
                m_ImmediateUserInterface->menu_action("Action-7");
                m_ImmediateUserInterface->menu_action("Action-8");
                m_ImmediateUserInterface->menu_action("Action-9");
                m_ImmediateUserInterface->menu_action("Action-10");
                m_ImmediateUserInterface->menu_action("Action-11");
                m_ImmediateUserInterface->menu_action("Action-12");

                if(m_ImmediateUserInterface->begin_menu("Menu-333"))
                {
                    m_ImmediateUserInterface->menu_action("Action-333-1");
                    m_ImmediateUserInterface->menu_action("Action-333-22");
                    m_ImmediateUserInterface->menu_action("Action-333-333");
                    m_ImmediateUserInterface->menu_action("Action-333-4444");
                    m_ImmediateUserInterface->menu_action("Action-333-55555");

                    if(m_ImmediateUserInterface->begin_menu("Menu-3333"))
                    {
                        m_ImmediateUserInterface->menu_action("Action-3333-1");
                        m_ImmediateUserInterface->menu_action("Action-3333-22");
                        m_ImmediateUserInterface->menu_action("Action-3333-333");
                        m_ImmediateUserInterface->menu_action("Action-3333-4444");
                        m_ImmediateUserInterface->menu_action("Action-3333-55555");

                        m_ImmediateUserInterface->end_menu();
                    }

                    m_ImmediateUserInterface->end_menu();
                }

                m_ImmediateUserInterface->end_menu();
            }

            m_ImmediateUserInterface->end_menu();
        }

        m_ImmediateUserInterface->end_window();
    }

    // if(m_ImmediateUserInterface->begin_window("Window-1"))
    // {
    //     if(m_ImmediateUserInterface->begin_menu("Menu-11"))
    //     {
    //         if(m_ImmediateUserInterface->begin_menu("Menu-22"))
    //         {
    //             m_ImmediateUserInterface->end_menu();
    //         }

    //         m_ImmediateUserInterface->end_menu();
    //     }

    //     m_ImmediateUserInterface->end_window();
    // }
}