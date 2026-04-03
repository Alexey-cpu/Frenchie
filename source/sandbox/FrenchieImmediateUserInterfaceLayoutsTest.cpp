#include <FrenchieImmediateUserInterfaceLayoutsTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceLayoutsTest::FrenchieImmediateUserInterfaceLayoutsTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceLayoutsTest)){}
FrenchieImmediateUserInterfaceLayoutsTest::~FrenchieImmediateUserInterfaceLayoutsTest(){}

bool FrenchieImmediateUserInterfaceLayoutsTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();
    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceLayoutsTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Layouts test window", "LayoutsTestWindow"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        float minimum = 0.f;
        float maximum = 128.f;

        if(m_UI->begin_scrollarea(m_UI->next_id("Layouts", "Layouts")))
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
                    m_UI->input_scalar_slider(m_UI->next_id("TopMarginValue"), m_TopMargin, minimum, maximum, 1);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("TopMarginLabel"), "Top margin");

                    // left
                    m_UI->input_scalar_slider(m_UI->next_id("LeftMarginValue"), m_LeftMargin, minimum, maximum, 1);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("LeftMarginLabel"), "Left margin");

                    // right
                    m_UI->input_scalar_slider(m_UI->next_id("RightMarginValue"), m_RightMargin, minimum, maximum, 1);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("RightMarginLabel"), "Right margin");

                    // bottom
                    m_UI->input_scalar_slider(m_UI->next_id("BottomMarginValue"), m_BottomMargin, minimum, maximum, 1);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("BottomMarginLabel"), "Bottom margin");

                    m_UI->end_scrollarea();
                }

                m_UI->same_line();

                if(m_UI->begin_scrollarea(m_UI->next_id("Padding"), settings))
                {
                    m_UI->label(m_UI->next_id("Padding"), "Padding");

                    // top
                    m_UI->input_scalar_slider(m_UI->next_id("TopPaddingValue"), m_TopPadding, minimum, maximum, 1);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("TopPaddingLabel"), "Top padding");

                    // left
                    m_UI->input_scalar_slider(m_UI->next_id("LeftPaddingValue"), m_LeftPadding, minimum, maximum, 1);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("LeftPaddingLabel"), "Left padding");

                    // right
                    m_UI->input_scalar_slider(m_UI->next_id("RightPaddingValue"), m_RightPadding, minimum, maximum, 1);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("RightPaddingLabel"), "Right padding");

                    // bottom
                    m_UI->input_scalar_slider(m_UI->next_id("BottomPaddingValue"), m_BottomPadding, minimum, maximum, 1);
                    m_UI->same_line();
                    m_UI->label(m_UI->next_id("BottomPaddingLabel"), "Bottom padding");

                    m_UI->end_scrollarea();
                }

                m_UI->same_line();

                if(m_UI->begin_scrollarea(m_UI->next_id("Alignment"), settings))
                {
                    // horizontal alignment
                    m_UI->label(m_UI->next_id("HorizontalAlignment"), "Horizontal alignment");

                    if(m_UI->begin_combobox(m_UI->next_id("HorizontalAlignemntType"), m_HorizontalAligmentComboboxPreview))
                    {
                        if(m_UI->combobox_item(m_UI->next_id("Left", "Left")))
                            m_HorizontalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft;
                        
                        if(m_UI->combobox_item(m_UI->next_id("Center", "Center")))
                            m_HorizontalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter;
                        
                        if(m_UI->combobox_item(m_UI->next_id("Right", "Right")))
                            m_HorizontalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight;

                        m_UI->end_combobox();
                    }

                    if((m_HorizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft))
                        m_HorizontalAligmentComboboxPreview = "Left";
                    
                    if((m_HorizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
                        m_HorizontalAligmentComboboxPreview = "Center";
                    
                    if((m_HorizontalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight))
                        m_HorizontalAligmentComboboxPreview = "Right";

                    m_UI->next_line();
                    m_UI->next_line();

                    // vertical alignment
                    m_UI->label(m_UI->next_id("VerticalAlignment"), "Vertical alignment");

                    if(m_UI->begin_combobox(m_UI->next_id("VerticalAlignemntType"), m_VerticalAligmentComboboxPreview))
                    {
                        if(m_UI->combobox_item(m_UI->next_id("Top", "Top")))
                            m_VerticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop;
                        
                        if(m_UI->combobox_item(m_UI->next_id("Center", "Center")))
                            m_VerticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter;
                        
                        if(m_UI->combobox_item(m_UI->next_id("Bottom", "Bottom")))
                            m_VerticalAlignmentSettings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom;

                        m_UI->end_combobox();
                    }

                    if((m_VerticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop))
                        m_VerticalAligmentComboboxPreview = "Top";
                    
                    if((m_VerticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
                        m_VerticalAligmentComboboxPreview = "Center";
                    
                    if((m_VerticalAlignmentSettings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom))
                        m_VerticalAligmentComboboxPreview = "Bottom";

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

                    m_UI->next_content_margin(gs_vec4f(m_TopMargin, m_LeftMargin, m_RightMargin, m_BottomMargin));
                    m_UI->next_content_padding(gs_vec4f(m_TopPadding, m_LeftPadding, m_RightPadding, m_BottomPadding));

                    if(m_UI->begin_panel(m_UI->next_id("Panel"), m_HorizontalAlignmentSettings | m_VerticalAlignmentSettings))
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

                    m_UI->next_content_margin(gs_vec4f(m_TopMargin, m_LeftMargin, m_RightMargin, m_BottomMargin));
                    m_UI->next_content_padding(gs_vec4f(m_TopPadding, m_LeftPadding, m_RightPadding, m_BottomPadding));

                    if(m_UI->begin_vertical_stack(m_UI->next_id("VerticalStack"), m_HorizontalAlignmentSettings | m_VerticalAlignmentSettings))
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

                    m_UI->next_content_margin(gs_vec4f(m_TopMargin, m_LeftMargin, m_RightMargin, m_BottomMargin));
                    m_UI->next_content_padding(gs_vec4f(m_TopPadding, m_LeftPadding, m_RightPadding, m_BottomPadding));

                    if(m_UI->begin_horizontal_stack(m_UI->next_id("HorizontalStack"), m_HorizontalAlignmentSettings | m_VerticalAlignmentSettings))
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
                m_UI->next_content_margin(gs_vec4f(m_TopMargin, m_LeftMargin, m_RightMargin, m_BottomMargin));
                m_UI->next_content_padding(gs_vec4f(m_TopPadding, m_LeftPadding, m_RightPadding, m_BottomPadding));

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

            m_UI->end_scrollarea();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterfaceLayoutsTest::allows_multiple_instances() const
{
    return false;
}