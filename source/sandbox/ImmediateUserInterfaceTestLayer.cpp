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
    m_ImmediateUserInterface->m_Settings =
        ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking
        | ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Тестовое окно", "Window")))
    { 
        if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Contents")))
        {
            auto windowSize = m_ImmediateUserInterface->get_rendering_stack_top()->State.BoundingBox.size() * 0.8f;

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
                                      (EnableRGB ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB : 0)
                                    | (EnableHSV ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV : 0)
                                    | (EnableHSL ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL : 0)
                                    | (EnableAlpha ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha : 0)
                                    | (PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                                );
                            }
                            // HSVA
                            else
                            {
                                m_ImmediateUserInterface->color_picker_hsva(
                                    m_ImmediateUserInterface->next_id("HSVAColorPicker"),
                                    colorPickerColor,
                                      (EnableRGB ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB : 0)
                                    | (EnableHSV ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV : 0)
                                    | (EnableHSL ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL : 0)
                                    | (EnableAlpha ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha : 0)
                                    | (PreviewColor ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor : 0)
                                );
                            }

                            m_ImmediateUserInterface->end_horizontal_stack();
                        }

                        m_ImmediateUserInterface->end_tree_node();
                    }

                    if(m_ImmediateUserInterface->begin_tree_node(m_ImmediateUserInterface->next_id("Color input", "ColorInput")))
                    {
                        m_ImmediateUserInterface->input_color(
                            m_ImmediateUserInterface->next_id("ColorInput"),
                            colorPickerColor,
                              (EnableRGB ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB : 0)
                            | (EnableHSV ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV : 0)
                            | (EnableHSL ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL : 0)
                            | (EnableAlpha ? ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha : 0)
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
                    static std::string singleLineString;

                    m_ImmediateUserInterface->label(m_ImmediateUserInterface->next_id("MultilineText"), "MultilineText");
                    m_ImmediateUserInterface->next_size(gs_vec2f(windowSize.x, 256.f));
                    m_ImmediateUserInterface->input_string_multiline(m_ImmediateUserInterface->next_id("Multiline"), multilineString);

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

    std::filesystem::path currentPath = std::filesystem::current_path();

    if(m_ImmediateUserInterface->begin_window(m_ImmediateUserInterface->next_id("Тестовое окно", "Window")))
    {
        if(m_ImmediateUserInterface->begin_scrollarea(m_ImmediateUserInterface->next_id("Contents")))
        {
            renderDirectory(currentPath);

            m_ImmediateUserInterface->end_scrollarea();
        }

        m_ImmediateUserInterface->end_window();
    }
}