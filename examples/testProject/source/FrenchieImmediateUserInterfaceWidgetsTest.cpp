#include <FrenchieImmediateUserInterfaceWidgetsTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceWidgetsTest::FrenchieImmediateUserInterfaceWidgetsTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceWidgetsTest)){}
FrenchieImmediateUserInterfaceWidgetsTest::~FrenchieImmediateUserInterfaceWidgetsTest(){}

bool FrenchieImmediateUserInterfaceWidgetsTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();
    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceWidgetsTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Widgets test window", "WidgetsTestWindow"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_scrollarea(m_UI->next_id("Widgets")))
        {
            // buttons
            if(m_UI->begin_tree_node(m_UI->next_id("Buttons", "Buttons")))
            {
                // push button
                m_UI->push_button(m_UI->next_id("Button", "PushButton"));
                
                // checkbox
                m_UI->check_button(
                    m_UI->next_id("CheckButton"),
                    m_CheckButtonState,
                    ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable
                    | ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox);
                
                // radio button
                m_UI->check_button(
                    m_UI->next_id("RadioButton"),
                    m_CheckButtonState,
                    ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable
                    | ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton);
                
                // slider button
                m_UI->check_button(
                    m_UI->next_id("SliderButton"),
                    m_CheckButtonState,
                    ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable
                    | ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton);
                    
                m_UI->end_tree_node();
            }

            // text
            if(m_UI->begin_tree_node(m_UI->next_id("Text", "Text")))
            {
                // label
                m_UI->label(
                    m_UI->next_id("Label"),
                    "Hello, I am a very usefull text");

                // label wrapped
                m_UI->label(
                    m_UI->next_id("LabelWrapped"),
                    "Hello, I am a very usefull text", ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignLeft, 11);
                
                // what is it popup
                if(m_UI->begin_what_is_it(m_UI->next_id("WhatIsIt"), m_UI->get_rendered_stack_top()))
                {
                    m_UI->label(m_UI->next_id("WhatIsItLabel"), "Unwrapped text:\nHello, I am a very usefull text");
                    m_UI->end_what_is_it();
                }

                m_UI->next_maximum_size(gs_vec2f(256.f, 129.f));

                if(m_UI->begin_scrollarea(
                    m_UI->next_id("SingleLine"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar  |
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar |
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
                {
                    m_UI->input_string_singleline(
                        m_UI->next_id("SingleLine"),
                        m_SinglelineString,
                        (m_RenderSingleLineAsPassword ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Password : 0));

                    m_UI->end_scrollarea();
                }

                m_UI->indent(32.f);
                m_UI->same_line();
                m_UI->check_button(m_UI->next_id("DisplayAsPassword"), m_RenderSingleLineAsPassword);
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
                        m_UI->input_string_multiline(m_UI->next_id("Multiline"), m_MultilineString);
                        m_UI->end_scrollarea();
                    }

                    if(m_UI->begin_scrollarea(
                        m_UI->next_id("Buffer"),
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        m_UI->input_string_multiline(m_UI->next_id("Buffer"), m_MultilineBuffer);
                        m_UI->end_scrollarea();
                    }

                    m_UI->end_horizontal_stack();
                }

                m_UI->end_tree_node();
            }

            // scalar input
            if(m_UI->begin_tree_node(m_UI->next_id("Scalar input", "Scalar input")))
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
                m_UI->input_scalar<float>(m_UI->next_id("floatScalarValue"), m_FloatScalar, 0.f, 256.f);
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar_slider<float>(m_UI->next_id("floatScalarSlider"), m_FloatScalar, 0.f, 256.f, 1);

                // double
                m_UI->next_size(labelSize);
                m_UI->label(m_UI->next_id("doubleScalarLabel"), "double");
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar<double>(m_UI->next_id("doubleScalarValue"), m_DoubleScalar, 0.0, 256);
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar_slider<double>(m_UI->next_id("doubleScalarSlider"), m_DoubleScalar, 0.0, 256.0, 1);

                // long double
                m_UI->next_size(labelSize);
                m_UI->label(m_UI->next_id("longDoubleScalarLabel"), "long double");
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar<long double>(m_UI->next_id("longDoubleScalarValue"), m_LongDoubleScalar, 0, 256);
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar_slider<long double>(m_UI->next_id("longDoubleScalarSlider"), m_LongDoubleScalar, 0, 256, 1);

                // int
                m_UI->next_size(labelSize);
                m_UI->label(m_UI->next_id("intScalarLabel"), "int");
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar(m_UI->next_id("intScalarValue"), m_IntScalar, 0, 256);
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar_slider<int>(m_UI->next_id("intScalarSlider"), m_IntScalar, 0, 256, 1);

                // short
                m_UI->next_size(labelSize);
                m_UI->label(m_UI->next_id("shortScalarLabel"), "short");
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar<short>(m_UI->next_id("shortScalarValue"), m_ShortScalar, 0, 256);
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar_slider<short>(m_UI->next_id("shortScalarSlider"), m_ShortScalar, 0, 256, 1);
                

                // unsigned int
                m_UI->next_size(labelSize);
                m_UI->label(m_UI->next_id("unsignedIntScalarLabel"), "unsigned int");
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar<unsigned int>(m_UI->next_id("unsignedIntScalarValue"), m_UnsignedIntScalar, 0, 256);
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar_slider<unsigned int>(m_UI->next_id("unsignedIntScalarSlider"), m_UnsignedIntScalar, 0, 256, 1);

                // unsigned short
                m_UI->next_size(labelSize);
                m_UI->label(m_UI->next_id("unsignedShortScalarLabel"), "unsigned short");
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar<unsigned short>(m_UI->next_id("unsignedShortScalarValue"), m_UnsignedShortScalar, 0, 256);
                m_UI->same_line();
                m_UI->indent();
                m_UI->input_scalar_slider<unsigned short>(m_UI->next_id("UnsignedShortSlider"), m_UnsignedShortScalar, 0, 256, 1);

                m_UI->end_tree_node();
            }

            // color pickers
            if(m_UI->begin_tree_node(m_UI->next_id("Color pickers", "Color pickers")))
            {
                float indent = 64.f;

                m_UI->check_button(m_UI->next_id("EnableRGB"), m_EnableRGB);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("EnableRGBLabel"), "RGB");
                
                m_UI->same_line();
                m_UI->indent(indent);
                m_UI->check_button(m_UI->next_id("EnableHSV"), m_EnableHSV);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("EnableHSVLabel"), "HSV");

                m_UI->same_line();
                m_UI->indent(indent);
                m_UI->check_button(m_UI->next_id("EnableHSL"), m_EnableHSL);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("EnableHSLLabel"), "HSL");

                m_UI->same_line();
                m_UI->indent(indent);
                m_UI->check_button(m_UI->next_id("PreviewColor"), m_PreviewColor);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("PreviewColorLabel"), "Preview color");

                m_UI->same_line();
                m_UI->indent(indent);
                m_UI->check_button(m_UI->next_id("EnableAlpha"), m_EnableAlpha);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("EnableAlphaLabel"), "Alpha");
                m_UI->next_line();

                if(m_UI->begin_combobox(m_UI->next_id("Combobox"), m_RGBA ? "RGBA" : "HSVA"))
                {
                    bool rgbaSelected     = m_RGBA;
                    bool hsvaSelected     = !m_RGBA;
                    int  checkboxSettings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox;

                    m_UI->check_button(m_UI->next_id("RGBASelected"), rgbaSelected, checkboxSettings);
                    m_UI->same_line();
                    if(m_UI->combobox_item(m_UI->next_id("RGBA", "RGBA"))) m_RGBA = true;

                    m_UI->check_button(m_UI->next_id("HSVASelected"), hsvaSelected, checkboxSettings);
                    m_UI->same_line();
                    if(m_UI->combobox_item(m_UI->next_id("HSVA", "HSVA"))) m_RGBA = false;

                    m_UI->end_combobox();
                }

                // RGBA
                if(m_RGBA)
                {
                    m_UI->color_picker_rgba(
                        m_UI->next_id("RGBAColorPicker"),
                        m_ColorPickerColor,
                            (m_EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                        | (m_EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                        | (m_EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                        | (m_EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                        | (m_PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                    );
                }
                // HSVA
                else
                {
                    m_UI->color_picker_hsva(
                        m_UI->next_id("HSVAColorPicker"),
                        m_ColorPickerColor,
                            (m_EnableRGB    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB      : 0)
                        | (m_EnableHSV    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV      : 0)
                        | (m_EnableHSL    ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL      : 0)
                        | (m_EnableAlpha  ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha    : 0)
                        | (m_PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                    );
                }

                m_UI->end_tree_node();
            }

            // tables
            if(m_UI->begin_tree_node(m_UI->next_id("Tables", "Tables")))
            {
                m_UI->check_button(m_UI->next_id("RenderCornerHeaderCheckbox"), m_RenderCornerHeader);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("RenderCornerHeaderLabel"), "Render corner header");
                
                m_UI->check_button(m_UI->next_id("RenderColumnHeadersCheckbox"), m_RenderColumnHeaders);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("RenderColumnHeadersLabel"), "Render column headers");

                m_UI->check_button(m_UI->next_id("RenderRowHeadersCheckbox"), m_RenderRowHeaders);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("RenderRowHeadersLabel"), "Render row headers");

                m_UI->input_scalar(m_UI->next_id("RowsCountInput"), m_RowsCount, 0, 10000);
                m_UI->same_line();
                m_UI->input_scalar_slider(m_UI->next_id("RowsCountSlider"), m_RowsCount, 0, 10000);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("RowsCountLabel"), "Rows count");

                m_UI->input_scalar(m_UI->next_id("ColsCountInput"), m_ColumnsCount, 0, 10000);
                m_UI->same_line();
                m_UI->input_scalar_slider(m_UI->next_id("ColsCountSlider"), m_ColumnsCount, 0, 10000);
                m_UI->same_line();
                m_UI->label(m_UI->next_id("ColsCountLabel"), "Cols count");

                m_UI->next_line();
                m_UI->next_line();

                if(m_UI->begin_table(m_UI->next_id("Table"), m_RowsCount, m_ColumnsCount))
                {
                    // retrieve clipper
                    auto clipper = m_UI->current_clipper(m_UI->get_rendering_stack_top());

                    // corner title
                    if(m_RenderCornerHeader)
                    {
                        if(m_UI->begin_table_corner_header())
                        {
                            m_UI->label(m_UI->next_id("Label"), Frenchie::Core::String::format("%d x %d", m_RowsCount, m_ColumnsCount));
                            m_UI->end_table_corner_header();
                        }
                    }

                    // column titles
                    if(m_RenderColumnHeaders)
                    {
                        for (int j = clipper.SourceCol; j < clipper.TargetCol; ++j)
                        {
                            if(m_UI->begin_table_column_header(j))
                            {
                                m_UI->label(m_UI->next_id("Label"), Frenchie::Core::String::to_string(j));
                                m_UI->end_table_column_header();
                            }
                        }
                    }

                    // row titles
                    if(m_RenderRowHeaders)
                    {
                        for (int i = clipper.SourceRow; i < clipper.TargetRow; ++i)
                        {
                            if(m_UI->begin_table_row_header(i))
                            {
                                m_UI->label(m_UI->next_id("Label"), Frenchie::Core::String::to_string(i));
                                m_UI->end_table_row_header();
                            }
                        }
                    }

                    // data cells
                    for (int i = clipper.SourceRow; i < clipper.TargetRow; ++i)
                    {
                        for (int j = clipper.SourceCol; j < clipper.TargetCol; ++j)
                        {
                            if(m_UI->begin_table_data_cell(i, j))
                            {
                                m_UI->input_string_singleline(m_UI->next_id("Data"), m_TableDataCellString);
                                m_UI->end_table_data_cell();
                            }
                        }
                    }
                    

                    m_UI->end_table();
                }

                m_UI->end_tree_node();
            }

            m_UI->end_scrollarea();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterfaceWidgetsTest::allows_multiple_instances() const
{
    return false;
}