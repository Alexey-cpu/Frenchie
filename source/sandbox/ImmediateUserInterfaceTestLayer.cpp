#include <ImmediateUserInterfaceTestLayer.hpp>

#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Application;

ImmediateUserInterfaceTestLayer::ImmediateUserInterfaceTestLayer(){}
ImmediateUserInterfaceTestLayer::~ImmediateUserInterfaceTestLayer(){}

bool ImmediateUserInterfaceTestLayer::awake()
{
    if(m_ImmediateUserInterface == nullptr)
        m_ImmediateUserInterface = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    m_FolderClosedTexture = ApplicationRenderingBackend::construct_texture("C:/SDK/Qt_Projects/OpenGL/shared/appData/themes/icons/default_folder.png");
    m_FolderOpenedTexture = ApplicationRenderingBackend::construct_texture("C:/SDK/Qt_Projects/OpenGL/shared/appData/themes/icons/default_folder_opened.png");
    m_FileTexture         = ApplicationRenderingBackend::construct_texture("C:/SDK/Qt_Projects/OpenGL/shared/appData/themes/icons/default_file.png");

    return true;
}

void ImmediateUserInterfaceTestLayer::frame_update()
{
    //widgets_test();
    develop_test();
    //windows_test();
    //scrollarea_test();
    
    //renderer_test();

    auto FPS = std::string("FPS ").append(std::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().FrameRate));

    m_ImmediateUserInterface->m_Renderer->push_text(
        gs_vec2f(0.f, 0.f),
        FPS.begin(),
        FPS.end(),
        32.f,
        gs_color_rgba(255, 255, 255, 255));
}

void ImmediateUserInterfaceTestLayer::finish()
{
    if(m_ImmediateUserInterface != nullptr)
        m_ImmediateUserInterface->close();
}

void ImmediateUserInterfaceTestLayer::windows_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes |
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking |
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking;

    static bool opened = true;
    static std::string button;

    if(m_ImmediateUserInterface->begin_window("Metrics"))
    {
        m_ImmediateUserInterface->label(
            "Metrics/FPS",
            Frenchie::Core::String::format("Частота кадров %.4f", m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().FrameRate));

        m_ImmediateUserInterface->label(
            "Metrics/Commands",
            Frenchie::Core::String::format("Количество комманд рисовки %d", m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderingCommandsCount));

        m_ImmediateUserInterface->label(
            "Metrics/Triangles",
            Frenchie::Core::String::format("Количество треугольников в сетке %d", m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderedTrianglesCount));

        m_ImmediateUserInterface->push_button("Button-1###Metrics/Button-1");
        m_ImmediateUserInterface->push_button("Button-2###Metrics/Button-2");

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-1",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable  |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        if(m_ImmediateUserInterface->begin_scrollarea(
            "Window-1/ScrollArea",
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults))
        {
            int k = 0;

            for(int i = 0 ; i < 1e1; i++)
            {
                for(int j = 0 ; j < 1e1; j++)
                    m_ImmediateUserInterface->push_button(std::string("Button-").append(std::to_string(k++)));

                m_ImmediateUserInterface->next_line();
            }

            m_ImmediateUserInterface->end_scrollarea();
        }

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window("Какое-то окно=)###Window"))
    {
        if(m_ImmediateUserInterface->begin_menu("Menu-1"))
        {
            if(m_ImmediateUserInterface->begin_menu("Menu-2"))
            {
                static bool checked = false;
                m_ImmediateUserInterface->check_button("Checkbox", checked, ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox);
                m_ImmediateUserInterface->menu_action("Action-4");

                m_ImmediateUserInterface->check_button("RadioButton", checked, ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton);
                m_ImmediateUserInterface->menu_action("Action-5");

                m_ImmediateUserInterface->check_button("SliderButton", checked, ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton);
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

                    m_ImmediateUserInterface->end_menu();
                }

                m_ImmediateUserInterface->end_menu();
            }

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

        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-2",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable, &opened))
    {
        // if(m_ImmediateUserInterface->begin_horizontal_stack("Window-2/Stack"))
        // {
        //     m_ImmediateUserInterface->push_button("Button-1###Window-2/Button-1");
        //     m_ImmediateUserInterface->push_button("Button-2###Window-2/Button-2");

        //     m_ImmediateUserInterface->end_horizontal_stack();
        // }

        m_ImmediateUserInterface->end_window();
    }
}

void ImmediateUserInterfaceTestLayer::widgets_test()
{
    if(State.ShowContextOverlay)
    {
        m_ImmediateUserInterface->next_position(gs_vec2f(0.f, 0.f));

        if(m_ImmediateUserInterface->begin_scrollarea(
            m_ImmediateUserInterface->next_id("Overlay"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
        {
            // FPS
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("FPSLabel"), "FPS");
            m_ImmediateUserInterface->same_line();
            m_ImmediateUserInterface->indent(32.f);
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("FPSValue"), Frenchie::Core::String::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().FrameRate));

            // CMD
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("CMDLabel"), "CMD");
            m_ImmediateUserInterface->same_line();
            m_ImmediateUserInterface->indent(32.f);
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("CMDValue"), Frenchie::Core::String::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderingCommandsCount));

            // Triangles
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("TrianglesLabel"), "Triangles");
            m_ImmediateUserInterface->same_line();
            m_ImmediateUserInterface->indent(32.f);
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("TrianglesValue"), Frenchie::Core::String::to_string(m_ImmediateUserInterface->m_Renderer->get_rendering_queue_metrics().RenderedTrianglesCount));

            m_ImmediateUserInterface->end_scrollarea();
        }
    }

    // UI context settings
    if(m_ImmediateUserInterface->begin_window(
        m_ImmediateUserInterface->next_id("Context settings", "ContextSettings"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &State.ShowContextSettings))
    {
        if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Settings")))
        {
            // windows mutual docking
            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("WindowsMutualDocking"), State.ContextWindowsDockingEnabled);
            m_ImmediateUserInterface->same_line();
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("WindowsMutualDockingLabel"), "Windows mutual docking");

            if(State.ContextWindowsDockingEnabled)
                m_ImmediateUserInterface->m_Settings |= ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking;
            else
                m_ImmediateUserInterface->m_Settings &= ~ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking;

            // windows workspace docking
            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("WindowsWorkspaceDocking"), State.ContextWorkspaceDockingEnabled);
            m_ImmediateUserInterface->same_line();
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("WindowsWorkspaceDockingLabel"), "Windows workspace docking");

            if(State.ContextWorkspaceDockingEnabled)
                m_ImmediateUserInterface->m_Settings |= ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking;
            else
                m_ImmediateUserInterface->m_Settings &= ~ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking;

            // highlight hovered nodes
            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("HighlightHoveredNodes"), State.ContextHighlightHoveredNodes);
            m_ImmediateUserInterface->same_line();
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("HighlightHoveredNodesLabel"), "Highlight hovered nodes");

            if(State.ContextHighlightHoveredNodes)
                m_ImmediateUserInterface->m_Settings |= ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;
            else
                m_ImmediateUserInterface->m_Settings &= ~ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

            // overlay
            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("Show overlay", "ShowOverlay"), State.ShowContextOverlay);
            m_ImmediateUserInterface->same_line();
            m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("ShowOverlayLabel"), "Show overlay");


            m_ImmediateUserInterface->end_scrollarea();
        }

        m_ImmediateUserInterface->end_window();
    }

    // main test window
    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Test window", "Window")))
    {
        if(m_ImmediateUserInterface->begin_menubar(m_ImmediateUserInterface->next_id("MenuBar")))
        {
            for (int i = 0; i < 10; i++)
            {
                std::string id = Frenchie::Core::String::format("Menu-%d", i);

                if(m_ImmediateUserInterface->begin_menu(m_ImmediateUserInterface->next_id(id, id)))
                {
                    m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Action-1", "Action-1"));
                    m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Action-2", "Action-2"));
                    m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Action-3", "Action-3"));

                    m_ImmediateUserInterface->end_menu();
                }
            }

            // if(m_ImmediateUserInterface->begin_menu(m_ImmediateUserInterface->next_id("Windows", "Windows")))
            // {
            //     if(m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Open context settings window", "OpenWindow1")))
            //         State.ShowContextSettings = true;

            //     m_ImmediateUserInterface->end_menu();
            // }

            // if(m_ImmediateUserInterface->begin_menu(m_ImmediateUserInterface->next_id("TestMenu", "TestMenu")))
            // {
            //     for (int i = 0; i < 40; i++)
            //     {
            //         std::string id = Frenchie::Core::String::format("Action-%d", i);
            //         m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id(id, id));
            //     }

            //     if(m_ImmediateUserInterface->begin_menu(m_ImmediateUserInterface->next_id("Menu", "Menu-1")))
            //     {
            //         m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Action-1", "Action-1"));
            //         m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Action-2", "Action-2"));
            //         m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Action-3", "Action-3"));

            //         if(m_ImmediateUserInterface->begin_menu(m_ImmediateUserInterface->next_id("Menu", "Menu-2")))
            //         {
            //             m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Action-1", "Action-1"));
            //             m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Action-2", "Action-2"));
            //             m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Action-3", "Action-3"));

            //             m_ImmediateUserInterface->end_menu();
            //         }

            //         m_ImmediateUserInterface->end_menu();
            //     }

            //     m_ImmediateUserInterface->end_menu();
            // }

            m_ImmediateUserInterface->end_menubar();
        }

        if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Contents")))
        {
            float minimum = 0.f;
            float maximum = 128.f;

            if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Layouts", "Layouts")))
            {
                int settings =
                      ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically;

                if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Configuration"), settings))
                {
                    if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Margin"), settings))
                    {
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("Margin"), "Margin");

                        // top
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("TopMarginValue"), State.TopMargin, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("TopMarginLabel"), "Top margin");

                        // left
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("LeftMarginValue"), State.LeftMargin, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("LeftMarginLabel"), "Left margin");

                        // right
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("RightMarginValue"), State.RightMargin, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("RightMarginLabel"), "Right margin");

                        // bottom
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("BottomMarginValue"), State.BottomMargin, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("BottomMarginLabel"), "Bottom margin");

                        m_ImmediateUserInterface->end_scrollarea();
                    }

                    m_ImmediateUserInterface->same_line();

                    if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Padding"), settings))
                    {
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("Padding"), "Padding");

                        // top
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("TopPaddingValue"), State.TopPadding, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("TopPaddingLabel"), "Top padding");

                        // left
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("LeftPaddingValue"), State.LeftPadding, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("LeftPaddingLabel"), "Left padding");

                        // right
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("RightPaddingValue"), State.RightPadding, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("RightPaddingLabel"), "Right padding");

                        // bottom
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("BottomPaddingValue"), State.BottomPadding, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("BottomPaddingLabel"), "Bottom padding");

                        m_ImmediateUserInterface->end_scrollarea();
                    }

                    m_ImmediateUserInterface->same_line();

                    if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Alignment"), settings))
                    {
                        // horizontal alignment
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("HorizontalAlignment"), "Horizontal alignment");

                        if(m_ImmediateUserInterface->begin_combobox(m_ImmediateUserInterface->next_id("HorizontalAlignemntType"), State.HorizontalAligmentComboboxPreview))
                        {
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Left", "Left")))
                                State.HorizontalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft;
                            
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Center", "Center")))
                                State.HorizontalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter;
                            
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Right", "Right")))
                                State.HorizontalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight;

                            m_ImmediateUserInterface->end_combobox();
                        }

                        if((State.HorizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft))
                            State.HorizontalAligmentComboboxPreview = "Left";
                        
                        if((State.HorizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
                            State.HorizontalAligmentComboboxPreview = "Center";
                        
                        if((State.HorizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight))
                            State.HorizontalAligmentComboboxPreview = "Right";

                        m_ImmediateUserInterface->next_line();
                        m_ImmediateUserInterface->next_line();

                        // vertical alignment
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("VerticalAlignment"), "Vertical alignment");

                        if(m_ImmediateUserInterface->begin_combobox(m_ImmediateUserInterface->next_id("VerticalAlignemntType"), State.VerticalAligmentComboboxPreview))
                        {
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Top", "Top")))
                                State.VerticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop;
                            
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Center", "Center")))
                                State.VerticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter;
                            
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Bottom", "Bottom")))
                                State.VerticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom;

                            m_ImmediateUserInterface->end_combobox();
                        }

                        if((State.VerticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop))
                            State.VerticalAligmentComboboxPreview = "Top";
                        
                        if((State.VerticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
                            State.VerticalAligmentComboboxPreview = "Center";
                        
                        if((State.VerticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom))
                            State.VerticalAligmentComboboxPreview = "Bottom";

                        m_ImmediateUserInterface->end_scrollarea();
                    }

                    m_ImmediateUserInterface->end_scrollarea();
                }

                // panel
                if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Panel", "Panel")))
                {
                    m_ImmediateUserInterface->next_maximum_size(gs_vec2f(gs_huge<float>(), 256.f));

                    if(m_ImmediateUserInterface->begin_panel(m_ImmediateUserInterface->next_id("Panel")))
                    {
                        m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Background"), gs_color_rgb(255, 255, 255));

                        m_ImmediateUserInterface->next_content_margin(gs_vec4f(State.TopMargin, State.LeftMargin, State.RightMargin, State.BottomMargin));
                        m_ImmediateUserInterface->next_content_padding(gs_vec4f(State.TopPadding, State.LeftPadding, State.RightPadding, State.BottomPadding));

                        if(m_ImmediateUserInterface->begin_panel(m_ImmediateUserInterface->next_id("Panel"), State.HorizontalAlignmentSettings | State.VerticalAlignmentSettings))
                        {
                            m_ImmediateUserInterface->next_size(gs_vec2f(256.f, 256.f));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("One"), gs_color_rgb(255, 0, 0));
                            m_ImmediateUserInterface->end_panel();
                        }

                        m_ImmediateUserInterface->end_panel();
                    }

                    m_ImmediateUserInterface->end_tree_node();
                }

                // vertical stack
                if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("VerticalStack", "Vertical stack")))
                {
                    m_ImmediateUserInterface->next_maximum_size(gs_vec2f(gs_huge<float>(), 256.f));

                    if(m_ImmediateUserInterface->begin_panel(m_ImmediateUserInterface->next_id("Panel")))
                    {
                        m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Background"), gs_color_rgb(255, 255, 255));

                        m_ImmediateUserInterface->next_content_margin(gs_vec4f(State.TopMargin, State.LeftMargin, State.RightMargin, State.BottomMargin));
                        m_ImmediateUserInterface->next_content_padding(gs_vec4f(State.TopPadding, State.LeftPadding, State.RightPadding, State.BottomPadding));

                        if(m_ImmediateUserInterface->begin_vertical_stack(m_ImmediateUserInterface->next_id("VerticalStack"), State.HorizontalAlignmentSettings | State.VerticalAlignmentSettings))
                        {
                            m_ImmediateUserInterface->next_size(gs_vec2f(256.f, 64.f));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("One"), gs_color_rgb(255, 0, 0));
                            
                            m_ImmediateUserInterface->next_size(gs_vec2f(256.f, 64.f));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Two"), gs_color_rgb(0, 255, 0));
                            
                            m_ImmediateUserInterface->next_size(gs_vec2f(256.f, 64.f));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Three"), gs_color_rgb(0, 0, 255));
                            
                            m_ImmediateUserInterface->next_size(gs_vec2f(256.f, 64.f));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Four"), gs_color_rgb(0, 255, 255));

                            m_ImmediateUserInterface->end_vertical_stack();
                        }

                        m_ImmediateUserInterface->end_panel();
                    }

                    m_ImmediateUserInterface->end_tree_node();
                }

                // horizontal stack
                if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("HorizontalStack", "Horizontal stack")))
                {
                    m_ImmediateUserInterface->next_maximum_size(gs_vec2f(gs_huge<float>(), 256.f));

                    if(m_ImmediateUserInterface->begin_panel(m_ImmediateUserInterface->next_id("Panel")))
                    {
                        m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Background"), gs_color_rgb(255, 255, 255));

                        m_ImmediateUserInterface->next_content_margin(gs_vec4f(State.TopMargin, State.LeftMargin, State.RightMargin, State.BottomMargin));
                        m_ImmediateUserInterface->next_content_padding(gs_vec4f(State.TopPadding, State.LeftPadding, State.RightPadding, State.BottomPadding));

                        if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("HorizontalStack"), State.HorizontalAlignmentSettings | State.VerticalAlignmentSettings))
                        {
                            m_ImmediateUserInterface->next_size(gs_vec2f(128.f, 64.f));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("One"), gs_color_rgb(255, 0, 0));

                            m_ImmediateUserInterface->next_size(gs_vec2f(128.f, 64.f));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Two"), gs_color_rgb(0, 255, 0));

                            m_ImmediateUserInterface->next_size(gs_vec2f(128.f, 64.f));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Three"), gs_color_rgb(0, 0, 255));

                            m_ImmediateUserInterface->next_size(gs_vec2f(128.f, 64.f));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Four"), gs_color_rgb(0, 255, 255));

                            m_ImmediateUserInterface->end_horizontal_stack();
                        }

                        m_ImmediateUserInterface->end_panel();
                    }

                    m_ImmediateUserInterface->end_tree_node();
                }

                // scrollarea
                if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Scrollarea", "Scrollarea")))
                {
                    m_ImmediateUserInterface->next_content_margin(gs_vec4f(State.TopMargin, State.LeftMargin, State.RightMargin, State.BottomMargin));
                    m_ImmediateUserInterface->next_content_padding(gs_vec4f(State.TopPadding, State.LeftPadding, State.RightPadding, State.BottomPadding));

                    if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Scrollarea", "Scrollarea")))
                    {
                        for(int i = 0; i < 10; i++)
                        {
                            for(int j = 0; j < 10; j++)
                            {
                                m_ImmediateUserInterface->push_button(
                                    m_ImmediateUserInterface->next_id(
                                        "Button-1",
                                        std::string("Button-").append(std::to_string(i)).append("_").append(std::to_string(j))));

                                m_ImmediateUserInterface->same_line();
                            }

                            m_ImmediateUserInterface->next_line();
                        }

                        m_ImmediateUserInterface->end_scrollarea();
                    }

                    m_ImmediateUserInterface->end_tree_node();
                }

                m_ImmediateUserInterface->end_tree_node();
            }

            // widgets
            if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Widgets", "Widgets")))
            {
                // scalar input
                if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Scalar input", "ScalarInput")))
                {
                    char label[] = "unsigned short";
                    
                    gs_vec2f labelSize = m_ImmediateUserInterface->m_Renderer->calculate_bounding_box(
                        &label[0],
                        &label[sizeof(label) / sizeof(char)],
                        m_ImmediateUserInterface->m_Style.get_font_size(),
                        m_ImmediateUserInterface->m_Style.get_current_font()).size() + gs_vec2f(m_ImmediateUserInterface->m_Style.get_font_size(), 0.f);
                    
                    // float
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("floatScalarLabel"), "float");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar<float>(m_ImmediateUserInterface->next_id("floatScalarValue"), State.FloatScalar, 0.f, 256.f);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar_slider<float>(m_ImmediateUserInterface->next_id("floatScalarSlider"), State.FloatScalar, 0.f, 256.f, 1);

                    // double
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("doubleScalarLabel"), "double");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar<double>(m_ImmediateUserInterface->next_id("doubleScalarValue"), State.DoubleScalar, 0.0, 256);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar_slider<double>(m_ImmediateUserInterface->next_id("doubleScalarSlider"), State.DoubleScalar, 0.0, 256.0, 1);

                    // long double
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("longDoubleScalarLabel"), "long double");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar<long double>(m_ImmediateUserInterface->next_id("longDoubleScalarValue"), State.LongDoubleScalar, 0, 256);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar_slider<long double>(m_ImmediateUserInterface->next_id("longDoubleScalarSlider"), State.LongDoubleScalar, 0, 256, 1);

                    // int
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("intScalarLabel"), "int");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("intScalarValue"), State.IntScalar, 0, 256);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar_slider<int>(m_ImmediateUserInterface->next_id("intScalarSlider"), State.IntScalar, 0, 256, 1);

                    // short
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("shortScalarLabel"), "short");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar<short>(m_ImmediateUserInterface->next_id("shortScalarValue"), State.ShortScalar, 0, 256);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar_slider<short>(m_ImmediateUserInterface->next_id("shortScalarSlider"), State.ShortScalar, 0, 256, 1);
                    

                    // unsigned int
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("unsignedIntScalarLabel"), "unsigned int");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar<unsigned int>(m_ImmediateUserInterface->next_id("unsignedIntScalarValue"), State.UnsignedIntScalar, 0, 256);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar_slider<unsigned int>(m_ImmediateUserInterface->next_id("unsignedIntScalarSlider"), State.UnsignedIntScalar, 0, 256, 1);

                    // unsigned short
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("unsignedShortScalarLabel"), "unsigned short");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar<unsigned short>(m_ImmediateUserInterface->next_id("unsignedShortScalarValue"), State.UnsignedShortScalar, 0, 256);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent();
                    m_ImmediateUserInterface->input_scalar_slider<unsigned short>(m_ImmediateUserInterface->next_id("UnsignedShortSlider"), State.UnsignedShortScalar, 0, 256, 1);

                    m_ImmediateUserInterface->end_tree_node();
                }

                // color pickers
                if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Color modifiers", "ColorModifiers")))
                {
                    float indent = 64.f;

                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableRGB"), State.EnableRGB);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableRGBLabel"), "RGB");
                    
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent(indent);
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableHSV"), State.EnableHSV);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableHSVLabel"), "HSV");

                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent(indent);
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableHSL"), State.EnableHSL);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableHSLLabel"), "HSL");

                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent(indent);
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("PreviewColor"), State.PreviewColor);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("PreviewColorLabel"), "Preview color");

                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent(indent);
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableAlpha"), State.EnableAlpha);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableAlphaLabel"), "Alpha");
                    m_ImmediateUserInterface->next_line();

                    if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Color picker", "ColorPicker")))
                    {
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("ColorPickerType"), "Type");
                        m_ImmediateUserInterface->same_line();

                        if(m_ImmediateUserInterface->begin_combobox(m_ImmediateUserInterface->next_id("Combobox"), State.RGBA ? "RGBA" : "HSVA"))
                        {
                            bool rgbaSelected     = State.RGBA;
                            bool hsvaSelected     = !State.RGBA;
                            int  checkboxSettings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox;

                            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("RGBASelected"), rgbaSelected, checkboxSettings);
                            m_ImmediateUserInterface->same_line();
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("RGBA", "RGBA"))) State.RGBA = true;

                            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("HSVASelected"), hsvaSelected, checkboxSettings);
                            m_ImmediateUserInterface->same_line();
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("HSVA", "HSVA"))) State.RGBA = false;

                            m_ImmediateUserInterface->end_combobox();
                        }

                        m_ImmediateUserInterface->next_line();
                        m_ImmediateUserInterface->next_line();
                        m_ImmediateUserInterface->next_size(gs_vec2f(1024.f, 512.f));

                        if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("Pickers")))
                        {
                            // RGBA
                            if(State.RGBA)
                            {
                                m_ImmediateUserInterface->color_picker_rgba(
                                    m_ImmediateUserInterface->next_id("RGBAColorPicker"),
                                    State.ColorPickerColor,
                                      (State.EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                                    | (State.EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                                    | (State.EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                                    | (State.EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                                    | (State.PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                                );
                            }
                            // HSVA
                            else
                            {
                                m_ImmediateUserInterface->color_picker_hsva(
                                    m_ImmediateUserInterface->next_id("HSVAColorPicker"),
                                    State.ColorPickerColor,
                                      (State.EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                                    | (State.EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                                    | (State.EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                                    | (State.EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                                    | (State.PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                                );
                            }

                            m_ImmediateUserInterface->end_horizontal_stack();
                        }

                        m_ImmediateUserInterface->end_tree_node();
                    }

                    if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Color input", "ColorInput")))
                    {
                        m_ImmediateUserInterface->next_size(gs_vec2f(1024.f, 512.f));
                        
                        m_ImmediateUserInterface->input_color(
                            m_ImmediateUserInterface->next_id("ColorInput"),
                            State.ColorPickerColor,
                              (State.EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                            | (State.EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                            | (State.EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                            | (State.EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                            | (State.PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                        );

                        m_ImmediateUserInterface->end_tree_node();
                    }

                    m_ImmediateUserInterface->end_tree_node();
                }

                // text input
                if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Text input", "TextInput")))
                {
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("SingleLineText"), "Single line text");

                    m_ImmediateUserInterface->next_maximum_size(gs_vec2f(256.f, 129.f));

                    if(m_ImmediateUserInterface->begin_scrollarea(
                        m_ImmediateUserInterface->next_id("SingleLine"),
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
                    {
                        m_ImmediateUserInterface->input_string_singleline(
                            m_ImmediateUserInterface->next_id("SingleLine"),
                            State.SinglelineString,
                            (State.RenderSingleLineAsPassword ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Password : 0));

                        m_ImmediateUserInterface->end_scrollarea();
                    }

                    m_ImmediateUserInterface->indent(32.f);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("DisplayAsPassword"), State.RenderSingleLineAsPassword);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("DisplayAsPasswordLabel"), "Password");

                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("MultilineText"), "Multiline text");
                    
                    m_ImmediateUserInterface->next_content_padding(16.f);

                    if(m_ImmediateUserInterface->begin_horizontal_stack(
                        m_ImmediateUserInterface->next_id("MultilineTextEditors"),
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        if(m_ImmediateUserInterface->begin_scrollarea(
                            m_ImmediateUserInterface->next_id("Multiline"),
                            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar))
                        {
                            m_ImmediateUserInterface->input_string_multiline(m_ImmediateUserInterface->next_id("Multiline"), State.MultilineString);
                            m_ImmediateUserInterface->end_scrollarea();
                        }

                        if(m_ImmediateUserInterface->begin_scrollarea(
                            m_ImmediateUserInterface->next_id("Buffer"),
                            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                        {
                            m_ImmediateUserInterface->input_string_multiline(m_ImmediateUserInterface->next_id("Buffer"), State.MultilineBuffer);
                            m_ImmediateUserInterface->end_scrollarea();
                        }

                        m_ImmediateUserInterface->end_horizontal_stack();
                    }

                    m_ImmediateUserInterface->end_tree_node();
                }

                m_ImmediateUserInterface->end_tree_node();
            }

            m_ImmediateUserInterface->end_scrollarea();
        }

        m_ImmediateUserInterface->end_window();
    }
}

void ImmediateUserInterfaceTestLayer::renderDirectory(const std::filesystem::path& _Path)
{
    auto textureOpened = std::filesystem::is_directory(_Path) ? m_FolderOpenedTexture : m_FileTexture;
    auto textureClosed = std::filesystem::is_directory(_Path) ? m_FolderClosedTexture : m_FileTexture;

    if(m_ImmediateUserInterface->begin_tree_node(
        m_ImmediateUserInterface->next_id(_Path.filename().string(), _Path.string()),
        ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_Defaults,
        textureOpened,
        textureClosed))
    {
        if(std::filesystem::is_directory(_Path))
        {
            for(const auto& directory : std::filesystem::directory_iterator(_Path, std::filesystem::directory_options::skip_permission_denied))
                renderDirectory(directory.path());
        }

        m_ImmediateUserInterface->end_tree_node();
    }
}

void ImmediateUserInterfaceTestLayer::develop_test()
{
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking |
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

    static std::string input;
    static float       value;

    int scrollSettings =


        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar;

    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Тестовое окно", "Window")))
    {
        if(m_ImmediateUserInterface->begin_scrollarea1(
            m_ImmediateUserInterface->next_id("String"),
            scrollSettings))
        {
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    std::string name = Frenchie::Core::String::format("Label-%d-%d", i, j);
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id(name, name)); m_ImmediateUserInterface->same_line();
                }

                m_ImmediateUserInterface->next_line();
            }
            

            m_ImmediateUserInterface->end_scrollarea1();
        }

        // int rowsCount    = 10000;
        // int colsCount    = 10000;
        // int cellSettings = 
        //       ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
        //     | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
        //     | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable
        //     | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;

        
        // if(m_ImmediateUserInterface->begin_table(m_ImmediateUserInterface->next_id("Table"), rowsCount, colsCount))
        // {
        //     ImmediateUserInterfaceGridClipper clipper = m_ImmediateUserInterface->current_table_clipper();

        //     // corner header
        //     if(m_ImmediateUserInterface->begin_table_corner_title(cellSettings))
        //     {
        //         m_ImmediateUserInterface->label(
        //             m_ImmediateUserInterface->next_id("Dimintions"),
        //             Frenchie::Core::String::format("%dx%d", rowsCount, colsCount),
        //             ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter);

        //         m_ImmediateUserInterface->end_table_corner_title();
        //     }

        //     // columns titles
        //     for (int j = clipper.SourceCol; j < clipper.TargetCol; j++)
        //     {
        //         if(m_ImmediateUserInterface->begin_table_horizontal_title(j, cellSettings))
        //         {
        //             m_ImmediateUserInterface->label(
        //                 m_ImmediateUserInterface->next_id("Title"),
        //                 Frenchie::Core::String::to_string(j),
        //                 ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter);
        //             m_ImmediateUserInterface->end_table_horizontal_title();
        //         }
        //     }

        //     // rows titles
        //     for (int j = clipper.SourceRow; j < clipper.TargetRow; j++)
        //     {
        //         if(m_ImmediateUserInterface->begin_table_vertical_title(j, cellSettings))
        //         {
        //             m_ImmediateUserInterface->label(
        //                 m_ImmediateUserInterface->next_id("Title"),
        //                 Frenchie::Core::String::to_string(j),
        //                 ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter);
        //             m_ImmediateUserInterface->end_table_vertical_title();
        //         }
        //     }

        //     for (int i = clipper.SourceRow; i < clipper.TargetRow; i++)
        //     { 
        //         for (int j = clipper.SourceCol; j < clipper.TargetCol; j++)
        //         {
        //             m_ImmediateUserInterface->next_content_margin(gs_vec4f(8.f));

        //             if(m_ImmediateUserInterface->begin_table_data_cell(i, j, cellSettings))
        //             {
        //                 m_ImmediateUserInterface->input_scalar(
        //                     m_ImmediateUserInterface->next_id("String"),
        //                     value);
                        
        //                 m_ImmediateUserInterface->end_table_data_cell();
        //             }
        //         }
        //     }

        //     m_ImmediateUserInterface->end_table();
        // }

        m_ImmediateUserInterface->end_window();
    }

    // std::filesystem::path currentPath = std::filesystem::current_path();

    // if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Тестовое окно", "Window")))
    // {
    //     if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Contents")))
    //     {
    //         renderDirectory(currentPath);

    //         m_ImmediateUserInterface->end_scrollarea();
    //     }

    //     m_ImmediateUserInterface->end_window();
    // }
}