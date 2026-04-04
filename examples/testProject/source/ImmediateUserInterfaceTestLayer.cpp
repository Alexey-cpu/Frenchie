#include <ImmediateUserInterfaceTestLayer.hpp>

#include <FrenchieCoreStringUtilities.hpp>

using namespace Frenchie::Application;

ImmediateUserInterfaceTestLayer::ImmediateUserInterfaceTestLayer(){}
ImmediateUserInterfaceTestLayer::~ImmediateUserInterfaceTestLayer(){}

bool ImmediateUserInterfaceTestLayer::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();
    return m_UI != nullptr;
}

void ImmediateUserInterfaceTestLayer::frame_start(){}

void ImmediateUserInterfaceTestLayer::frame_update()
{
    //widgets_test();
    develop_test();
    //windows_test();
    //scrollarea_test();
    
    //renderer_test();

    auto FPS = std::string("FPS ").append(std::to_string(m_UI->m_Renderer->get_rendering_queue_metrics().FrameRate));

    m_UI->m_Renderer->push_text(
        gs_vec2f(0.f, 0.f),
        FPS.begin(),
        FPS.end(),
        32.f,
        gs_color_rgba(255, 255, 255, 255));
}

void ImmediateUserInterfaceTestLayer::finish()
{
}

void ImmediateUserInterfaceTestLayer::widgets_test()
{
    if(State.ShowContextOverlay)
    {
        m_UI->next_position(gs_vec2f(0.f, 0.f));

        if(m_UI->begin_scrollarea(
            m_UI->next_id("Overlay"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
        {
            // FPS
            m_UI->label(m_UI->next_id("FPSLabel"), "FPS");
            m_UI->same_line();
            m_UI->indent(32.f);
            m_UI->label(m_UI->next_id("FPSValue"), Frenchie::Core::String::to_string(m_UI->m_Renderer->get_rendering_queue_metrics().FrameRate));

            // CMD
            m_UI->label(m_UI->next_id("CMDLabel"), "CMD");
            m_UI->same_line();
            m_UI->indent(32.f);
            m_UI->label(m_UI->next_id("CMDValue"), Frenchie::Core::String::to_string(m_UI->m_Renderer->get_rendering_queue_metrics().RenderingCommandsCount));

            // Triangles
            m_UI->label(m_UI->next_id("TrianglesLabel"), "Triangles");
            m_UI->same_line();
            m_UI->indent(32.f);
            m_UI->label(m_UI->next_id("TrianglesValue"), Frenchie::Core::String::to_string(m_UI->m_Renderer->get_rendering_queue_metrics().RenderedTrianglesCount));

            m_UI->end_scrollarea();
        }
    }

    // UI context settings
    if(m_UI->begin_window(
        m_UI->next_id("Context settings", "ContextSettings"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &State.ShowContextSettings))
    {
        if(m_UI->begin_scrollarea(m_UI->next_id("Settings")))
        {
            // windows mutual docking
            m_UI->check_button(m_UI->next_id("WindowsMutualDocking"), State.ContextWindowsDockingEnabled);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("WindowsMutualDockingLabel"), "Windows mutual docking");

            if(State.ContextWindowsDockingEnabled)
                m_UI->m_Settings |= ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking;
            else
                m_UI->m_Settings &= ~ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking;

            // windows workspace docking
            m_UI->check_button(m_UI->next_id("WindowsWorkspaceDocking"), State.ContextWorkspaceDockingEnabled);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("WindowsWorkspaceDockingLabel"), "Windows workspace docking");

            if(State.ContextWorkspaceDockingEnabled)
                m_UI->m_Settings |= ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking;
            else
                m_UI->m_Settings &= ~ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking;

            // highlight hovered nodes
            m_UI->check_button(m_UI->next_id("HighlightHoveredNodes"), State.ContextHighlightHoveredNodes);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("HighlightHoveredNodesLabel"), "Highlight hovered nodes");

            if(State.ContextHighlightHoveredNodes)
                m_UI->m_Settings |= ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;
            else
                m_UI->m_Settings &= ~ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

            // overlay
            m_UI->check_button(m_UI->next_id("Show overlay", "ShowOverlay"), State.ShowContextOverlay);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("ShowOverlayLabel"), "Show overlay");


            m_UI->end_scrollarea();
        }

        m_UI->end_window();
    }

    if(m_UI->begin_dialog(m_UI->next_id("Color picker dialog", "ColorPicker"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults, &State.showColorPciker))
    {
        m_UI->color_picker_rgba(
            m_UI->next_id("ColorPicker"),
            m_UI->m_Style.get_color((ImmediateUserInterfaceNodeColors_)State.ColorPickerColor));
        
        m_UI->end_dialog();
    }

    if(m_UI->begin_window(
        m_UI->next_id("Style Settings", "StyleSettings"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &State.ShowStyleSettings))
    {
        if(m_UI->begin_scrollarea(m_UI->next_id("ScrollArea")))
        {
            for (int i = ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Begin;
                     i < ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End;
                     i++)
            {
                if(m_UI->input_color(
                    m_UI->next_id(Frenchie::Core::String::format("Color-%d", i)),
                    m_UI->m_Style.get_color((ImmediateUserInterfaceNodeColors_)i),
                    ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB | ImmediateUserInterfaceColorPickerSettings_PreviewColor))
                {
                    State.showColorPciker  = true;
                    State.ColorPickerColor = i;
                }

                m_UI->same_line();
                m_UI->indent(32.f);

                m_UI->label(m_UI->next_id(Frenchie::Core::String::format("Label-%d", i)), m_UI->m_Style.style_color_to_string((ImmediateUserInterfaceNodeColors_)i));
            }

            m_UI->end_scrollarea();
        }        

        m_UI->end_window();
    }

    // main test window
    if(m_UI->begin_window(m_UI->next_id("Test window", "Window")))
    {
        if(m_UI->begin_menubar(m_UI->next_id("MenuBar")))
        {
            if(m_UI->begin_menu(m_UI->next_id("Windows", "Windows")))
            {
                if(m_UI->menu_action(m_UI->next_id("Open context settings window", "OpenContextSettingsWindow")))
                    State.ShowContextSettings = true;

                if(m_UI->menu_action(m_UI->next_id("Open style settings window", "OpenStyleSettingsWindow")))
                    State.ShowStyleSettings = true;

                m_UI->end_menu();
            }

            for (int i = 0; i < 20; i++)
            {
                auto menuID = Frenchie::Core::String::format("Menu-%d", i);

                if(m_UI->begin_menu(m_UI->next_id(menuID, menuID)))
                {
                    m_UI->menu_action(m_UI->next_id("Action-1"));
                    m_UI->menu_action(m_UI->next_id("Action-2"));
                    m_UI->menu_action(m_UI->next_id("Action-3"));

                    m_UI->end_menu();
                }
            }

            m_UI->end_menubar();
        }

        if(m_UI->begin_scrollarea(m_UI->next_id("Contents")))
        {
            float minimum = 0.f;
            float maximum = 128.f;

            if(m_UI->begin_tree_node(m_UI->next_id("Layouts", "Layouts")))
            {
                int settings =
                      ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically;

                if(m_UI->begin_scrollarea(m_UI->next_id("LayoutSettings"), settings))
                {
                    if(m_UI->begin_scrollarea(m_UI->next_id("Margin"), settings))
                    {
                        m_UI->label(m_UI->next_id("Margin"), "Margin");

                        // top
                        m_UI->input_scalar_slider(m_UI->next_id("TopMarginValue"), State.TopMargin, minimum, maximum, 1);
                        m_UI->same_line();
                        m_UI->label(m_UI->next_id("TopMarginLabel"), "Top margin");

                        // left
                        m_UI->input_scalar_slider(m_UI->next_id("LeftMarginValue"), State.LeftMargin, minimum, maximum, 1);
                        m_UI->same_line();
                        m_UI->label(m_UI->next_id("LeftMarginLabel"), "Left margin");

                        // right
                        m_UI->input_scalar_slider(m_UI->next_id("RightMarginValue"), State.RightMargin, minimum, maximum, 1);
                        m_UI->same_line();
                        m_UI->label(m_UI->next_id("RightMarginLabel"), "Right margin");

                        // bottom
                        m_UI->input_scalar_slider(m_UI->next_id("BottomMarginValue"), State.BottomMargin, minimum, maximum, 1);
                        m_UI->same_line();
                        m_UI->label(m_UI->next_id("BottomMarginLabel"), "Bottom margin");

                        m_UI->end_scrollarea();
                    }

                    m_UI->same_line();

                    if(m_UI->begin_scrollarea(m_UI->next_id("Padding"), settings))
                    {
                        m_UI->label(m_UI->next_id("Padding"), "Padding");

                        // top
                        m_UI->input_scalar_slider(m_UI->next_id("TopPaddingValue"), State.TopPadding, minimum, maximum, 1);
                        m_UI->same_line();
                        m_UI->label(m_UI->next_id("TopPaddingLabel"), "Top padding");

                        // left
                        m_UI->input_scalar_slider(m_UI->next_id("LeftPaddingValue"), State.LeftPadding, minimum, maximum, 1);
                        m_UI->same_line();
                        m_UI->label(m_UI->next_id("LeftPaddingLabel"), "Left padding");

                        // right
                        m_UI->input_scalar_slider(m_UI->next_id("RightPaddingValue"), State.RightPadding, minimum, maximum, 1);
                        m_UI->same_line();
                        m_UI->label(m_UI->next_id("RightPaddingLabel"), "Right padding");

                        // bottom
                        m_UI->input_scalar_slider(m_UI->next_id("BottomPaddingValue"), State.BottomPadding, minimum, maximum, 1);
                        m_UI->same_line();
                        m_UI->label(m_UI->next_id("BottomPaddingLabel"), "Bottom padding");

                        m_UI->end_scrollarea();
                    }

                    m_UI->same_line();

                    if(m_UI->begin_scrollarea(m_UI->next_id("Alignment"), settings))
                    {
                        // horizontal alignment
                        m_UI->label(m_UI->next_id("HorizontalAlignment"), "Horizontal alignment");

                        if(m_UI->begin_combobox(m_UI->next_id("HorizontalAlignemntType"), State.HorizontalAligmentComboboxPreview))
                        {
                            if(m_UI->combobox_item(m_UI->next_id("Left", "Left")))
                                State.HorizontalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft;
                            
                            if(m_UI->combobox_item(m_UI->next_id("Center", "Center")))
                                State.HorizontalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter;
                            
                            if(m_UI->combobox_item(m_UI->next_id("Right", "Right")))
                                State.HorizontalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight;

                            m_UI->end_combobox();
                        }

                        if((State.HorizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft))
                            State.HorizontalAligmentComboboxPreview = "Left";
                        
                        if((State.HorizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
                            State.HorizontalAligmentComboboxPreview = "Center";
                        
                        if((State.HorizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight))
                            State.HorizontalAligmentComboboxPreview = "Right";

                        m_UI->next_line();
                        m_UI->next_line();

                        // vertical alignment
                        m_UI->label(m_UI->next_id("VerticalAlignment"), "Vertical alignment");

                        if(m_UI->begin_combobox(m_UI->next_id("VerticalAlignemntType"), State.VerticalAligmentComboboxPreview))
                        {
                            if(m_UI->combobox_item(m_UI->next_id("Top", "Top")))
                                State.VerticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop;
                            
                            if(m_UI->combobox_item(m_UI->next_id("Center", "Center")))
                                State.VerticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter;
                            
                            if(m_UI->combobox_item(m_UI->next_id("Bottom", "Bottom")))
                                State.VerticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom;

                            m_UI->end_combobox();
                        }

                        if((State.VerticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop))
                            State.VerticalAligmentComboboxPreview = "Top";
                        
                        if((State.VerticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
                            State.VerticalAligmentComboboxPreview = "Center";
                        
                        if((State.VerticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom))
                            State.VerticalAligmentComboboxPreview = "Bottom";

                        m_UI->end_scrollarea();
                    }

                    m_UI->end_scrollarea();
                }

                // panel
                if(m_UI->begin_tree_node(m_UI->next_id("Panel", "Panel")))
                {
                    m_UI->next_maximum_size(gs_vec2f(gs_huge<float>(), 256.f));

                    if(m_UI->begin_panel(m_UI->next_id("Panel")))
                    {
                        m_UI->image(m_UI->next_id("Background"), gs_color_rgb(255, 255, 255));

                        m_UI->next_content_margin(gs_vec4f(State.TopMargin, State.LeftMargin, State.RightMargin, State.BottomMargin));
                        m_UI->next_content_padding(gs_vec4f(State.TopPadding, State.LeftPadding, State.RightPadding, State.BottomPadding));

                        if(m_UI->begin_panel(m_UI->next_id("Panel"), State.HorizontalAlignmentSettings | State.VerticalAlignmentSettings))
                        {
                            m_UI->image(m_UI->next_id("Preview"), gs_color_rgb(255, 0, 0));
                            m_UI->end_panel();
                        }

                        m_UI->end_panel();
                    }

                    m_UI->end_tree_node();
                }

                // vertical stack
                if(m_UI->begin_tree_node(m_UI->next_id("VerticalStack", "Vertical stack")))
                {
                    m_UI->next_maximum_size(gs_vec2f(gs_huge<float>(), 256.f));

                    if(m_UI->begin_panel(m_UI->next_id("Panel")))
                    {
                        m_UI->image(m_UI->next_id("Background"), gs_color_rgb(255, 255, 255));

                        m_UI->next_content_margin(gs_vec4f(State.TopMargin, State.LeftMargin, State.RightMargin, State.BottomMargin));
                        m_UI->next_content_padding(gs_vec4f(State.TopPadding, State.LeftPadding, State.RightPadding, State.BottomPadding));

                        if(m_UI->begin_vertical_stack(m_UI->next_id("VerticalStack"), State.HorizontalAlignmentSettings | State.VerticalAlignmentSettings))
                        {
                            m_UI->image(m_UI->next_id("One"), gs_color_rgb(255, 0, 0));
                            m_UI->image(m_UI->next_id("Two"), gs_color_rgb(0, 255, 0));
                            m_UI->image(m_UI->next_id("Three"), gs_color_rgb(0, 0, 255));
                            m_UI->image(m_UI->next_id("Four"), gs_color_rgb(0, 255, 255));

                            m_UI->end_vertical_stack();
                        }

                        m_UI->end_panel();
                    }

                    m_UI->end_tree_node();
                }

                // horizontal stack
                if(m_UI->begin_tree_node(m_UI->next_id("HorizontalStack", "Horizontal stack")))
                {
                    m_UI->next_maximum_size(gs_vec2f(gs_huge<float>(), 256.f));

                    if(m_UI->begin_panel(m_UI->next_id("Panel")))
                    {
                        m_UI->image(m_UI->next_id("Background"), gs_color_rgb(255, 255, 255));

                        m_UI->next_content_margin(gs_vec4f(State.TopMargin, State.LeftMargin, State.RightMargin, State.BottomMargin));
                        m_UI->next_content_padding(gs_vec4f(State.TopPadding, State.LeftPadding, State.RightPadding, State.BottomPadding));

                        if(m_UI->begin_horizontal_stack(m_UI->next_id("HorizontalStack"), State.HorizontalAlignmentSettings | State.VerticalAlignmentSettings))
                        {
                            m_UI->image(m_UI->next_id("One"), gs_color_rgb(255, 0, 0));
                            m_UI->image(m_UI->next_id("Two"), gs_color_rgb(0, 255, 0));
                            m_UI->image(m_UI->next_id("Three"), gs_color_rgb(0, 0, 255));
                            m_UI->image(m_UI->next_id("Four"), gs_color_rgb(0, 255, 255));

                            m_UI->end_horizontal_stack();
                        }

                        m_UI->end_panel();
                    }

                    m_UI->end_tree_node();
                }

                // scrollarea
                if(m_UI->begin_tree_node(m_UI->next_id("Scrollarea", "Scrollarea")))
                {
                    m_UI->next_content_margin(gs_vec4f(State.TopMargin, State.LeftMargin, State.RightMargin, State.BottomMargin));
                    m_UI->next_content_padding(gs_vec4f(State.TopPadding, State.LeftPadding, State.RightPadding, State.BottomPadding));

                    if(m_UI->begin_scrollarea(m_UI->next_id("Scrollarea", "Scrollarea")))
                    {
                        for(int i = 0; i < 10; i++)
                        {
                            for(int j = 0; j < 10; j++)
                            {
                                m_UI->push_button(
                                    m_UI->next_id(
                                        "Button-1",
                                        std::string("Button-").append(std::to_string(i)).append("_").append(std::to_string(j))));

                                m_UI->same_line();
                            }

                            m_UI->next_line();
                        }

                        m_UI->end_scrollarea();
                    }

                    m_UI->end_tree_node();
                }

                m_UI->end_tree_node();
            }

            // widgets
            if(m_UI->begin_tree_node(m_UI->next_id("Widgets", "Widgets")))
            {
                // scalar input
                if(m_UI->begin_tree_node(m_UI->next_id("Scalar input", "ScalarInput")))
                {
                    char label[] = "unsigned short";
                    
                    gs_vec2f labelSize = m_UI->m_Renderer->calculate_bounding_box(
                        &label[0],
                        &label[sizeof(label) / sizeof(char)],
                        m_UI->m_Style.get_font_size(),
                        m_UI->m_Style.get_current_font()).size() + gs_vec2f(m_UI->m_Style.get_font_size(), 0.f);
                    
                    // float
                    m_UI->next_size(labelSize);
                    m_UI->label(m_UI->next_id("floatScalarLabel"), "float");
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar<float>(m_UI->next_id("floatScalarValue"), State.FloatScalar, 0.f, 256.f);
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar_slider<float>(m_UI->next_id("floatScalarSlider"), State.FloatScalar, 0.f, 256.f, 1);

                    // double
                    m_UI->next_size(labelSize);
                    m_UI->label(m_UI->next_id("doubleScalarLabel"), "double");
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar<double>(m_UI->next_id("doubleScalarValue"), State.DoubleScalar, 0.0, 256);
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar_slider<double>(m_UI->next_id("doubleScalarSlider"), State.DoubleScalar, 0.0, 256.0, 1);

                    // long double
                    m_UI->next_size(labelSize);
                    m_UI->label(m_UI->next_id("longDoubleScalarLabel"), "long double");
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar<long double>(m_UI->next_id("longDoubleScalarValue"), State.LongDoubleScalar, 0, 256);
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar_slider<long double>(m_UI->next_id("longDoubleScalarSlider"), State.LongDoubleScalar, 0, 256, 1);

                    // int
                    m_UI->next_size(labelSize);
                    m_UI->label(m_UI->next_id("intScalarLabel"), "int");
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar(m_UI->next_id("intScalarValue"), State.IntScalar, 0, 256);
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar_slider<int>(m_UI->next_id("intScalarSlider"), State.IntScalar, 0, 256, 1);

                    // short
                    m_UI->next_size(labelSize);
                    m_UI->label(m_UI->next_id("shortScalarLabel"), "short");
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar<short>(m_UI->next_id("shortScalarValue"), State.ShortScalar, 0, 256);
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar_slider<short>(m_UI->next_id("shortScalarSlider"), State.ShortScalar, 0, 256, 1);
                    

                    // unsigned int
                    m_UI->next_size(labelSize);
                    m_UI->label(m_UI->next_id("unsignedIntScalarLabel"), "unsigned int");
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar<unsigned int>(m_UI->next_id("unsignedIntScalarValue"), State.UnsignedIntScalar, 0, 256);
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar_slider<unsigned int>(m_UI->next_id("unsignedIntScalarSlider"), State.UnsignedIntScalar, 0, 256, 1);

                    // unsigned short
                    m_UI->next_size(labelSize);
                    m_UI->label(m_UI->next_id("unsignedShortScalarLabel"), "unsigned short");
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar<unsigned short>(m_UI->next_id("unsignedShortScalarValue"), State.UnsignedShortScalar, 0, 256);
                    m_UI->same_line();
                    m_UI->indent();
                    m_UI->input_scalar_slider<unsigned short>(m_UI->next_id("UnsignedShortSlider"), State.UnsignedShortScalar, 0, 256, 1);

                    m_UI->end_tree_node();
                }

                // color pickers
                if(m_UI->begin_tree_node(m_UI->next_id("Color modifiers", "ColorModifiers")))
                {
                    float indent = 64.f;

                    m_UI->check_button(m_UI->next_id("EnableRGB"), State.EnableRGB);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("EnableRGBLabel"), "RGB");
                    
                    m_UI->same_line();
                    m_UI->indent(indent);
                    m_UI->check_button(m_UI->next_id("EnableHSV"), State.EnableHSV);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("EnableHSVLabel"), "HSV");

                    m_UI->same_line();
                    m_UI->indent(indent);
                    m_UI->check_button(m_UI->next_id("EnableHSL"), State.EnableHSL);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("EnableHSLLabel"), "HSL");

                    m_UI->same_line();
                    m_UI->indent(indent);
                    m_UI->check_button(m_UI->next_id("PreviewColor"), State.PreviewColor);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("PreviewColorLabel"), "Preview color");

                    m_UI->same_line();
                    m_UI->indent(indent);
                    m_UI->check_button(m_UI->next_id("EnableAlpha"), State.EnableAlpha);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("EnableAlphaLabel"), "Alpha");
                    m_UI->next_line();

                    if(m_UI->begin_tree_node(m_UI->next_id("Color picker", "ColorPicker")))
                    {
                        m_UI->label(m_UI->next_id("ColorPickerType"), "Type");
                        m_UI->same_line();

                        if(m_UI->begin_combobox(m_UI->next_id("Combobox"), State.RGBA ? "RGBA" : "HSVA"))
                        {
                            bool rgbaSelected     = State.RGBA;
                            bool hsvaSelected     = !State.RGBA;
                            int  checkboxSettings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox;

                            m_UI->check_button(m_UI->next_id("RGBASelected"), rgbaSelected, checkboxSettings);
                            m_UI->same_line();
                            if(m_UI->combobox_item(m_UI->next_id("RGBA", "RGBA"))) State.RGBA = true;

                            m_UI->check_button(m_UI->next_id("HSVASelected"), hsvaSelected, checkboxSettings);
                            m_UI->same_line();
                            if(m_UI->combobox_item(m_UI->next_id("HSVA", "HSVA"))) State.RGBA = false;

                            m_UI->end_combobox();
                        }

                        m_UI->next_line();
                        m_UI->next_line();
                        m_UI->next_size(gs_vec2f(1024.f, 512.f));

                        if(m_UI->begin_horizontal_stack(m_UI->next_id("Pickers")))
                        {
                            // RGBA
                            if(State.RGBA)
                            {
                                m_UI->color_picker_rgba(
                                    m_UI->next_id("RGBAColorPicker"),
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
                                m_UI->color_picker_hsva(
                                    m_UI->next_id("HSVAColorPicker"),
                                    State.ColorPickerColor,
                                      (State.EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                                    | (State.EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                                    | (State.EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                                    | (State.EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                                    | (State.PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                                );
                            }

                            m_UI->end_horizontal_stack();
                        }

                        m_UI->end_tree_node();
                    }

                    if(m_UI->begin_tree_node(m_UI->next_id("Color input", "ColorInput")))
                    {
                        m_UI->next_size(gs_vec2f(1024.f, 512.f));
                        
                        m_UI->input_color(
                            m_UI->next_id("ColorInput"),
                            State.ColorPickerColor,
                              (State.EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                            | (State.EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                            | (State.EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                            | (State.EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                            | (State.PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                        );

                        m_UI->end_tree_node();
                    }

                    m_UI->end_tree_node();
                }

                // text input
                if(m_UI->begin_tree_node(m_UI->next_id("Text input", "TextInput")))
                {
                    m_UI->label(m_UI->next_id("SingleLineText"), "Single line text");

                    m_UI->next_maximum_size(gs_vec2f(256.f, 129.f));

                    if(m_UI->begin_scrollarea(
                        m_UI->next_id("SingleLine"),
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar  |
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar |
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
                    {
                        m_UI->input_string_singleline(
                            m_UI->next_id("SingleLine"),
                            State.SinglelineString,
                            (State.RenderSingleLineAsPassword ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Password : 0));

                        m_UI->end_scrollarea();
                    }

                    m_UI->indent(32.f);
                    m_UI->same_line();
                    m_UI->check_button(m_UI->next_id("DisplayAsPassword"), State.RenderSingleLineAsPassword);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("DisplayAsPasswordLabel"), "Password");

                    m_UI->label(m_UI->next_id("MultilineText"), "Multiline text");
                    
                    m_UI->next_content_padding(16.f);

                    if(m_UI->begin_horizontal_stack(
                        m_UI->next_id("MultilineTextEditors"),
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        if(m_UI->begin_scrollarea(
                            m_UI->next_id("Multiline"),
                            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar))
                        {
                            m_UI->input_string_multiline(m_UI->next_id("Multiline"), State.MultilineString);
                            m_UI->end_scrollarea();
                        }

                        if(m_UI->begin_scrollarea(
                            m_UI->next_id("Buffer"),
                            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                        {
                            m_UI->input_string_multiline(m_UI->next_id("Buffer"), State.MultilineBuffer);
                            m_UI->end_scrollarea();
                        }

                        m_UI->end_horizontal_stack();
                    }

                    m_UI->end_tree_node();
                }

                m_UI->end_tree_node();
            }

            m_UI->end_scrollarea();
        }

        m_UI->end_window();
    }
}

void ImmediateUserInterfaceTestLayer::develop_test()
{
    m_UI->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking |
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

    static float value;

    if(m_UI->begin_window(m_UI->next_id("Тестовое окно", "Window")))
    {
        int rowsCount    = 1000;
        int colsCount    = 1000;
        int cellSettings = 
              ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;

        
        if(m_UI->begin_table(m_UI->next_id("Table"), rowsCount, colsCount))
        {
            ImmediateUserInterfaceGridClipper clipper = m_UI->current_table_clipper();

            // corner header
            if(m_UI->begin_table_corner_title(cellSettings))
            {
                m_UI->label(
                    m_UI->next_id("Dimintions"),
                    Frenchie::Core::String::format("%dx%d", rowsCount, colsCount),
                    ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter);

                m_UI->end_table_corner_title();
            }

            // columns titles
            for (int j = clipper.SourceCol; j < clipper.TargetCol; j++)
            {
                if(m_UI->begin_table_horizontal_title(j, cellSettings))
                {
                    m_UI->label(
                        m_UI->next_id("Title"),
                        Frenchie::Core::String::to_string(j),
                        ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter);
                    m_UI->end_table_horizontal_title();
                }
            }

            // rows titles
            for (int j = clipper.SourceRow; j < clipper.TargetRow; j++)
            {
                if(m_UI->begin_table_vertical_title(j, cellSettings))
                {
                    m_UI->label(
                        m_UI->next_id("Title"),
                        Frenchie::Core::String::to_string(j),
                        ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter);
                    m_UI->end_table_vertical_title();
                }
            }

            for (int i = clipper.SourceRow; i < clipper.TargetRow; i++)
            { 
                for (int j = clipper.SourceCol; j < clipper.TargetCol; j++)
                {
                    m_UI->next_content_margin(gs_vec4f(8.f));

                    if(m_UI->begin_table_data_cell(i, j, cellSettings))
                    {
                        // std::string id = Frenchie::Core::String::format("Label-%d-%d", i, j);

                        // if(i == 0)
                        //     m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id(id), Frenchie::Core::String::to_string(j));
                        // else if(j == 0)
                        //     m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id(id), Frenchie::Core::String::to_string(i));

                        m_UI->input_scalar(
                            m_UI->next_id("String"),
                            value);
                        
                        m_UI->end_table_data_cell();
                    }
                }
            }

            m_UI->end_table();
        }

        m_UI->end_window();
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