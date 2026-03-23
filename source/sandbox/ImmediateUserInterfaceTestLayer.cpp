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
    widgets_test();
    //develop_test();
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
    // m_ImmediateUserInterface->m_Settings =
    //     ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking
    //     | ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

    static bool openWindow1 = false;
    static bool openWindow2 = false;
    static bool openWindow3 = false;

    if(m_ImmediateUserInterface->begin_window(
        m_ImmediateUserInterface->next_id("Window1", "Window-1"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &openWindow1))
    {
        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        m_ImmediateUserInterface->next_id("Window2", "Window-2"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &openWindow2))
    {
        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        m_ImmediateUserInterface->next_id("Window3", "Window-3"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &openWindow3))
    {
        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Test window", "Window")))
    {
        if(m_ImmediateUserInterface->begin_menu(m_ImmediateUserInterface->next_id("Menu", "Menu")))
        {
            if(m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Open window 1", "OpenWindow1")))
                openWindow1 = true;

            if(m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Open window 2", "OpenWindow2")))
                openWindow2 = true;

            if(m_ImmediateUserInterface->menu_action(m_ImmediateUserInterface->next_id("Open window 3", "OpenWindow3")))
                openWindow3 = true;

            m_ImmediateUserInterface->end_menu();
        }

        if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Contents")))
        {
            // margin
            static float topMargin    = 0.f;
            static float leftMargin   = 0.f;
            static float rightMargin  = 0.f;
            static float bottomMargin = 0.f;

            // padding
            static float topPadding    = 0.f;
            static float leftPadding   = 0.f;
            static float rightPadding  = 0.f;
            static float bottomPadding = 0.f;

            // alignment
            static int         horizontalAlignmentSettings       = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter;
            static int         verticalAlignmentSettings         = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter;
            static std::string horizontalAligmentComboboxPreview = "None";
            static std::string verticalAligmentComboboxPreview   = "None";

            float minimum = 0.f;
            float maximum = 128.f;

            if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Layouts", "Layouts")))
            {
                int settings =
                      ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically;

                if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("LayoutConfigurator"), settings))
                {
                    if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Margin"), settings))
                    {
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("Margin"), "Margin");

                        // top
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("TopMarginValue"), topMargin, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("TopMarginLabel"), "Top margin");

                        // left
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("LeftMarginValue"), leftMargin, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("LeftMarginLabel"), "Left margin");

                        // right
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("RightMarginValue"), rightMargin, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("RightMarginLabel"), "Right margin");

                        // bottom
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("BottomMarginValue"), bottomMargin, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("BottomMarginLabel"), "Bottom margin");

                        m_ImmediateUserInterface->end_scrollarea();
                    }

                    m_ImmediateUserInterface->same_line();

                    if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Padding"), settings))
                    {
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("Padding"), "Padding");

                        // top
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("TopPaddingValue"), topPadding, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("TopPaddingLabel"), "Top padding");

                        // left
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("LeftPaddingValue"), leftPadding, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("LeftPaddingLabel"), "Left padding");

                        // right
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("RightPaddingValue"), rightPadding, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("RightPaddingLabel"), "Right padding");

                        // bottom
                        m_ImmediateUserInterface->input_scalar_slider(m_ImmediateUserInterface->next_id("BottomPaddingValue"), bottomPadding, minimum, maximum, 1);
                        m_ImmediateUserInterface->same_line();
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("BottomPaddingLabel"), "Bottom padding");

                        m_ImmediateUserInterface->end_scrollarea();
                    }

                    m_ImmediateUserInterface->same_line();

                    if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Alignment"), settings))
                    {
                        // horizontal alignment
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("HorizontalAlignment"), "Horizontal alignment");

                        if(m_ImmediateUserInterface->begin_combobox(m_ImmediateUserInterface->next_id("HorizontalAlignemntType"), horizontalAligmentComboboxPreview))
                        {
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Left", "Left")) ||
                                (horizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft))
                            {
                                horizontalAligmentComboboxPreview = "Left";
                                horizontalAlignmentSettings       = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft;
                            }
                            
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Center", "Center")) ||
                                (horizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
                            {
                                horizontalAligmentComboboxPreview = "Center";
                                horizontalAlignmentSettings       = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter;
                            }
                            
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Right", "Right")) ||
                                (horizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight))
                            {
                                horizontalAligmentComboboxPreview = "Right";
                                horizontalAlignmentSettings       = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight;
                            }

                            m_ImmediateUserInterface->end_combobox();
                        }

                        m_ImmediateUserInterface->next_line();
                        m_ImmediateUserInterface->next_line();

                        // vertical alignment
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("VerticalAlignment"), "Vertical alignment");

                        if(m_ImmediateUserInterface->begin_combobox(m_ImmediateUserInterface->next_id("VerticalAlignemntType"), verticalAligmentComboboxPreview))
                        {
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Top", "Top")) ||
                                (verticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop))
                            {
                                verticalAligmentComboboxPreview   = "Top";
                                verticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop;
                            }
                            
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Center", "Center")) ||
                                (verticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
                            {
                                verticalAligmentComboboxPreview   = "Center";
                                verticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter;
                            }
                            
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("Bottom", "Bottom")) ||
                                (verticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom))
                            {
                                verticalAligmentComboboxPreview   = "Bottom";
                                verticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom;
                            }

                            m_ImmediateUserInterface->end_combobox();
                        }

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

                        m_ImmediateUserInterface->next_content_margin(gs_vec4f(topMargin, leftMargin, rightMargin, bottomMargin));
                        m_ImmediateUserInterface->next_content_padding(gs_vec4f(topPadding, leftPadding, rightPadding, bottomPadding));

                        if(m_ImmediateUserInterface->begin_panel(m_ImmediateUserInterface->next_id("VerticalStack"), horizontalAlignmentSettings | verticalAlignmentSettings))
                        {
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

                        m_ImmediateUserInterface->next_content_margin(gs_vec4f(topMargin, leftMargin, rightMargin, bottomMargin));
                        m_ImmediateUserInterface->next_content_padding(gs_vec4f(topPadding, leftPadding, rightPadding, bottomPadding));

                        if(m_ImmediateUserInterface->begin_vertical_stack(m_ImmediateUserInterface->next_id("VerticalStack"), horizontalAlignmentSettings | verticalAlignmentSettings))
                        {
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("One"), gs_color_rgb(255, 0, 0));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Two"), gs_color_rgb(0, 255, 0));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Three"), gs_color_rgb(0, 0, 255));
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

                        m_ImmediateUserInterface->next_content_margin(gs_vec4f(topMargin, leftMargin, rightMargin, bottomMargin));
                        m_ImmediateUserInterface->next_content_padding(gs_vec4f(topPadding, leftPadding, rightPadding, bottomPadding));

                        if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("HorizontalStack"), horizontalAlignmentSettings | verticalAlignmentSettings))
                        {
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("One"), gs_color_rgb(255, 0, 0));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Two"), gs_color_rgb(0, 255, 0));
                            m_ImmediateUserInterface->image(m_ImmediateUserInterface->next_id("Three"), gs_color_rgb(0, 0, 255));
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
                    m_ImmediateUserInterface->next_content_margin(gs_vec4f(topMargin, leftMargin, rightMargin, bottomMargin));
                    m_ImmediateUserInterface->next_content_padding(gs_vec4f(topPadding, leftPadding, rightPadding, bottomPadding));

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
                    static float          floatScalar       = 0.f;
                    static double         doubleScalar      = 0.0;
                    static long double    longDoubleScalar  = 0.0;
                    static int            intScalar         = 0;
                    static short          shortScalar       = 0;
                    static unsigned int   unsignedIntScalar = 0;
                    static unsigned short unsignedShortScalar = 0;

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
                    m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("floatScalarValue"), floatScalar);

                    // double
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("doubleScalarLabel"), "double");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("doubleScalarValue"), doubleScalar);

                    // long double
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("longDoubleScalarLabel"), "long double");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("longDoubleScalarValue"), longDoubleScalar);

                    // int
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("intScalarLabel"), "int");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("intScalarValue"), intScalar);

                    // short
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("shortScalarLabel"), "short");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("shortScalarValue"), shortScalar);

                    // unsigned int
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("unsignedIntScalarLabel"), "unsigned int");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("unsignedIntScalarValue"), unsignedIntScalar);

                    // unsigned short
                    m_ImmediateUserInterface->next_size(labelSize);
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("unsignedShortScalarLabel"), "unsigned short");
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->input_scalar(m_ImmediateUserInterface->next_id("unsignedShortScalarValue"), unsignedShortScalar);

                    m_ImmediateUserInterface->end_tree_node();
                }

                // color pickers
                if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Color modifiers", "ColorModifiers")))
                {
                    static gs_color colorPickerColor = gs_color_rgba(255, 0, 0, 255);
                    static bool     EnableRGB        = true;
                    static bool     EnableHSV        = true;
                    static bool     EnableHSL        = true;
                    static bool     EnableAlpha      = true;
                    static bool     PreviewColor     = true;
                    static bool     RGBA             = true;
                    static float    indent           = 64.f;

                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableRGB"), EnableRGB);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableRGBLabel"), "RGB");
                    
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent(indent);
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableHSV"), EnableHSV);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableHSVLabel"), "HSV");

                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent(indent);
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableHSL"), EnableHSL);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableHSLLabel"), "HSL");

                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent(indent);
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("PreviewColor"), PreviewColor);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("PreviewColorLabel"), "Preview color");

                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->indent(indent);
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("EnableAlpha"), EnableAlpha);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("EnableAlphaLabel"), "Alpha");
                    m_ImmediateUserInterface->next_line();

                    if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Color picker", "ColorPicker")))
                    {
                        m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("ColorPickerType"), "Type");
                        m_ImmediateUserInterface->same_line();

                        if(m_ImmediateUserInterface->begin_combobox(m_ImmediateUserInterface->next_id("Combobox"), RGBA ? "RGBA" : "HSVA"))
                        {
                            bool rgbaSelected     = RGBA;
                            bool hsvaSelected     = !RGBA;
                            int  checkboxSettings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox;

                            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("RGBASelected"), rgbaSelected, checkboxSettings);
                            m_ImmediateUserInterface->same_line();
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("RGBA", "RGBA"))) RGBA = true;

                            m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("HSVASelected"), hsvaSelected, checkboxSettings);
                            m_ImmediateUserInterface->same_line();
                            if(m_ImmediateUserInterface->combobox_item(m_ImmediateUserInterface->next_id("HSVA", "HSVA"))) RGBA = false;

                            m_ImmediateUserInterface->end_combobox();
                        }

                        m_ImmediateUserInterface->next_line();
                        m_ImmediateUserInterface->next_line();
                        m_ImmediateUserInterface->next_size(gs_vec2f(1024.f, 512.f));

                        if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("Pickers")))
                        {
                            // RGBA
                            if(RGBA)
                            {
                                m_ImmediateUserInterface->color_picker_rgba(
                                    m_ImmediateUserInterface->next_id("RGBAColorPicker"),
                                    colorPickerColor,
                                      (EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                                    | (EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                                    | (EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                                    | (EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                                    | (PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                                );
                            }
                            // HSVA
                            else
                            {
                                m_ImmediateUserInterface->color_picker_hsva(
                                    m_ImmediateUserInterface->next_id("HSVAColorPicker"),
                                    colorPickerColor,
                                      (EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                                    | (EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                                    | (EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                                    | (EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                                    | (PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
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
                            colorPickerColor,
                              (EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                            | (EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                            | (EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                            | (EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                            | (PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                        );

                        m_ImmediateUserInterface->end_tree_node();
                    }

                    m_ImmediateUserInterface->end_tree_node();
                }

                // text input
                if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Text input", "TextInput")))
                {
                    static std::string multilineString;
                    static std::string multilineBuffer;
                    static std::string singlelineString;
                    static bool        password;

                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("SingleLineText"), "Single line text");

                    m_ImmediateUserInterface->input_string_singleline(
                        m_ImmediateUserInterface->next_id("SingleLine"),
                        singlelineString,
                        (password ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Password : 0));

                    m_ImmediateUserInterface->indent(32.f);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->check_button(m_ImmediateUserInterface->next_id("DisplayAsPassword"), password);
                    m_ImmediateUserInterface->same_line();
                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("DisplayAsPasswordLabel"), "Password");

                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("MultilineText"), "Multiline text");
                    
                    m_ImmediateUserInterface->next_content_padding(16.f);

                    if(m_ImmediateUserInterface->begin_horizontal_stack(
                        m_ImmediateUserInterface->next_id("MultilineTextEditors"),
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        m_ImmediateUserInterface->input_string_multiline(m_ImmediateUserInterface->next_id("Multiline"), multilineString);

                        m_ImmediateUserInterface->input_string_multiline(m_ImmediateUserInterface->next_id("Buffer"), multilineBuffer);

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

    int settings =
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally
        | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically;

    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Тестовое окно", "Window")))
    {
        if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("LayoutTweaks", "LayoutTweaks")))
        {
            if(m_ImmediateUserInterface->begin_horizontal_stack(m_ImmediateUserInterface->next_id("Child-2")))
            {
                if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Margin"), settings))
                {
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Button-1", "Button-1"));
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Button-2", "Button-2"));
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Button-3", "Button-3"));

                    m_ImmediateUserInterface->end_scrollarea();
                }

                if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Margin1"), settings))
                {
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Button-1", "Button-1"));
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Button-2", "Button-2"));
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Button-3", "Button-3"));

                    m_ImmediateUserInterface->end_scrollarea();
                }

                if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Margin2"), settings))
                {
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Button-1", "Button-1"));
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Button-2", "Button-2"));
                    m_ImmediateUserInterface->push_button(m_ImmediateUserInterface->next_id("Button-3", "Button-3"));

                    m_ImmediateUserInterface->end_scrollarea();
                }

                m_ImmediateUserInterface->end_horizontal_stack();
            }

            m_ImmediateUserInterface->end_tree_node();
        }


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