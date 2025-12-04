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

    // m_Renderer->push_rectangle(
    //     gs_vec2f(0.f, 0.f),
    //     gs_vec2f(width * 0.5, -height * 0.5),
    //     8.f,
    //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
    //     0.f,
    //     gs_vec2f(0.f, 0.f),
    //     0.f,
    //     gs_vec2f(1.f, 1.f)
    // );

    m_Renderer->push_text(
        std::to_string(m_Renderer->get_cursor_postion().x).append(" ").append(
             std::to_string(m_Renderer->get_cursor_postion().y)
        ),
        m_Style.FontSize,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        0.f,
        m_Renderer->get_cursor_postion() + gs_vec3f(Immediate2DRenderer::bottom_right(gs_vec2f(12.f, 12.f)))
    );

    m_Renderer->push_rectangle(
        m_Renderer->m_Viewport.Min,
        m_Renderer->m_Viewport.Max,
        8.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        0.f,
        gs_vec2f(0.f, 0.f),
        0.f,
        gs_vec2f(1.f, 1.f)
    );

    gs_vec2f start = Immediate2DRenderer::bottom_right(gs_vec2f(12.f, 12.f));

    draw_push_button_widget(
        "BTN !!!",
        gs_vec2f(256.f, gs_min(m_Style.FontSize, 128.f)),
        true,
        0.f,
        start
    );

    draw_push_button_widget(
        "Hello !!! I am ANOTHER Button !!!",
        gs_vec2f(256.f, gs_min(m_Style.FontSize, 128.f)),
        false,
        0.f,
        start + Immediate2DRenderer::bottom_right(gs_vec2f(0.f, 256.f))
    );

    static bool pushed = true;
    draw_radio_button_widget(
        "Hello !!! I am radio button !!!",
        32.f,
        pushed,
        0.f,
        start + Immediate2DRenderer::bottom_right(gs_vec2f(0.f, 512.f))
    );
}

bool Immedidate2DRendererTestLayer::draw_radio_button_widget(
    const std::string& _Name,
    const float&       _Radius,
    bool&              _Pushed,
    const float&       _Depth,
    const gs_vec2f&    _Position,
    const float&       _Rotation,
    const gs_vec2f&    _Scale)
{
    gs_2dboxf buttonBoundingBox =
        gs_2dboxf(
            gs_vec2f(0.f, 0.f),
            gs_vec2f(_Radius, _Radius) * 2.f);
    
    if(Frenchie::Application::application()
        ->is_mouse_button_clicked(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
    {
        if(m_Renderer->calculate_bounding_box(
            _Depth,
            _Position,
            _Rotation,
            _Scale,
            buttonBoundingBox.Min,
            buttonBoundingBox.Max).contains(m_Renderer->get_cursor_postion()))
        {
            _Pushed = !_Pushed;
        }
    }

    // draw background
    m_Renderer->push_arc_filled(
        gs_vec2f(_Radius, _Radius),
        _Radius,
        _Radius,
        0.f,
        360.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        _Depth,
        _Position,
        _Rotation,
        _Scale
    );

    if(_Pushed)
    {
        m_Renderer->push_arc_filled(
            gs_vec2f(_Radius, _Radius),
            _Radius,
            _Radius,
            0.f,
            360.f,
            gs_vec4f(0.f, 255.f, 0.f, 255.f),
            _Depth + 1,
            _Position,
            _Rotation,
            _Scale
        );
    }

    m_Renderer->push_arc(
        gs_vec2f(_Radius, _Radius),
        _Radius,
        _Radius,
        0.f,
        360.f,
        12.f, // line width
        gs_vec4f(0.f, 0.f, 255.f, 255.f),
        _Depth + 2,
        _Position,
        _Rotation,
        _Scale
    );

    return true;
}

bool Immedidate2DRendererTestLayer::draw_push_button_widget(
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
            Immediate2DRenderer::bottom_right(gs_vec2f(+gs_max(_Size.x, textBoundingBox.get_size().x), gs_max(_Size.y, textBoundingBox.get_size().y)) + gs_vec2f(m_Style.FontSize, m_Style.FontSize)));

    bool hovered = m_Renderer->calculate_bounding_box(
        _Depth,
        _Position,
        _Rotation,
        _Scale,
        buttonBoundingBox.Min,
        buttonBoundingBox.Max).contains(m_Renderer->get_cursor_postion());
    
    bool pressed =
        hovered &&
        Frenchie::Application::application()->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left);

    // draw background
    m_Renderer->push_rectangle_rounded_filled(
        buttonBoundingBox.Min,
        buttonBoundingBox.Max,
        m_Style.PushButtonRoundingRadius,
        retreive_button_background_color(_Enabled, hovered, pressed),
        _Depth,
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
        _Depth + 1.f,
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
        _Depth + 2.f,
        (min + max) * 0.5f + Immediate2DRenderer::top_left(gs_vec2f(+textBoundingBox.get_size().x, +textBoundingBox.get_size().y) * 0.5f),
        _Rotation,
        _Scale
    );

    return pressed && _Enabled;
}