#include <FrenchieApplicationSandBoxImmediate2DRendererTestLayer.hpp>

using namespace Frenchie::Application;

bool Immedidate2DRendererTestLayer::awake()
{
    // setup renderer
    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

    return m_Renderer != nullptr;
}

void Immedidate2DRendererTestLayer::frame_start()
{
}

void Immedidate2DRendererTestLayer::frame_update()
{
    if(m_Renderer == nullptr)
        return;

    float width  = Frenchie::Application::application()->get_window_size().x;
    float height = Frenchie::Application::application()->get_window_size().y;

    m_Renderer->push_rectangle(
        gs_vec2f(0.f, 0.f),
        gs_vec2f(width * 0.5, -height * 0.5),
        8.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        0.f,
        gs_vec2f(0.f, 0.f),
        0.f,
        gs_vec2f(1.f, 1.f)
    );

    m_Renderer->push_rectangle(
        gs_vec2f(0.f, 0.f),
        gs_vec2f(width, -height),
        8.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        0.f,
        gs_vec2f(0.f, 0.f),
        0.f,
        gs_vec2f(1.f, 1.f)
    );

    push_button_widget(
        "Hello !!! I am Button !!!",
        gs_vec2f(256.f, gs_min(m_Style.FontSize, 128.f)),
        true,
        0.f,
        gs_vec2f(width, -height) * 0.5f
    );

    push_button_widget(
        "Hello !!! I am Button !!!",
        gs_vec2f(256.f, gs_min(m_Style.FontSize, 128.f)),
        false,
        0.f,
        gs_vec2f(width, -height - 512.f) * 0.5f
    );
}

bool Immedidate2DRendererTestLayer::push_button_widget(
    const std::string& _Name,
    const gs_vec2f&    _Size,
    const bool&        _Enabled,
    const float&       _Depth,
    const gs_vec2f&    _Position,
    const float&       _Rotation,
    const gs_vec2f&    _Scale)
{
    // auxiliary lambdas
    auto retreive_button_background_color = [this](const bool& _Enabled, const bool _Hovered, const bool& _Pressed)->gs_vec4f
    {
        if(_Pressed)
        {
            return _Enabled ?
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedBackgroundColor] :
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedBackgroundColor];
        }

        if(_Hovered)
        {
            return _Enabled ?
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredBackgroundColor] :
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredBackgroundColor];
        }

        return _Enabled ?
            m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledBackgroundColor] :
            m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledBackgroundColor];
    };

    auto retreive_frame_color = [this](const bool& _Enabled, const bool _Hovered, const bool& _Pressed)->gs_vec4f
    {
        if(_Pressed)
        {
            return _Enabled ?
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedFrameColor] :
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedFrameColor];
        }

        if(_Hovered)
        {
            return _Enabled ?
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredFrameColor] :
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredFrameColor];
        }

        return _Enabled ?
            m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor] :
            m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor];
    };

    auto retreive_text_color = [this](const bool& _Enabled, const bool _Hovered, const bool& _Pressed)->gs_vec4f
    {
        if(_Hovered || _Pressed)
        {
            return _Enabled ?
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextHoveredColor] :
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextDisabledColor];
        }

        return _Enabled ?
            m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextEnabledColor] :
            m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextDisabledColor];
    };

    gs_2dboxf textBoundingBox = 
        m_Renderer->calculate_bounding_box(
            _Depth,
            gs_vec2f(0.f),
            0.f,
            gs_vec2f(1.f, 1.f),
            _Name,
            m_Style.FontSize,
            m_Renderer->m_RenderingQueue->get_default_font());

    gs_2dboxf buttonBoundingBox =
        gs_2dboxf(
            gs_vec2f(0.f, 0.f),
            gs_vec2f(+gs_max(_Size.x, textBoundingBox.get_size().x), -gs_max(_Size.y, textBoundingBox.get_size().y)));

    bool hovered = m_Renderer->calculate_bounding_box(
        _Depth,
        _Position,
        _Rotation,
        _Scale,
        buttonBoundingBox.Min,
        buttonBoundingBox.Max).contains(m_Renderer->get_rendering_queue()->get_cursor_postion());
    
    bool pressed =
        hovered &&
        Frenchie::Application::application()->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left);

    // draw background
    m_Renderer->push_rectangle_rounded_filled(
        buttonBoundingBox.Min,
        buttonBoundingBox.Max,
        m_Style.PushButtonRoundingRadius,
        retreive_button_background_color(_Enabled, hovered, pressed),
        _Depth + 3.f,
        _Position,
        _Rotation,
        _Scale
    );

    // draw frame
    m_Renderer->push_rectangle_rounded(
        buttonBoundingBox.Min,
        buttonBoundingBox.Max,
        m_Style.PushButtonRoundingRadius,
        m_Style.PushButtonFrameWidth,
        retreive_frame_color(_Enabled, hovered, pressed),
        _Depth + 3.f,
        _Position,
        _Rotation,
        _Scale
    );
    
    // draw text
    gs_mat4f transform = m_Renderer->calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);
    gs_vec2f min = transform * gs_vec4f(buttonBoundingBox.Min, _Depth, 1.f);
    gs_vec2f max = transform * gs_vec4f(buttonBoundingBox.Max, _Depth, 1.f);

    m_Renderer->push_text(
        _Name,
        m_Style.FontSize,
        retreive_text_color(_Enabled, hovered, pressed),
        _Depth + 6.f,
        (min + max) * 0.5f + gs_vec2f(-textBoundingBox.get_size().x, +textBoundingBox.get_size().y) * 0.5f,
        _Rotation,
        _Scale
    );

    return pressed && _Enabled;
}