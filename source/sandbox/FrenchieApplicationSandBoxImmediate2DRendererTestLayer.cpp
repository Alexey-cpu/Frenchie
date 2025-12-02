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

    push_button_widget(
        "Button-1",
        gs_vec2f(256.f, 128.f),
        true,
        0.f,
        gs_vec2f(width, -height) * 0.5f
    );

    // push_button_widget(
    //     "Button-2",
    //     gs_vec2f(256.f, 128.f),
    //     false,
    //     0.f,
    //     gs_vec2f(width, -height) * 0.5f + gs_vec2f(512.f, 0.f)
    // );
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

    gs_rectf textBoundingBox = m_Renderer->calculate_bounding_box(
        _Depth,
        gs_vec2f(0.f),
        0.f,
        gs_vec2f(1.f, 1.f),
        _Name,
        m_Style.TextSize,
        m_Renderer->m_RenderingQueue->get_default_font());

    gs_vec2f min = gs_vec2f(0.f);
    gs_vec2f max = min + gs_vec2f(+_Size.x, -_Size.y);

    min = gs_vec2f(gs_min(textBoundingBox.Min.x, min.x), gs_min(textBoundingBox.Min.y, min.y));
    max = gs_vec2f(gs_max(textBoundingBox.Max.x, max.x), gs_max(textBoundingBox.Max.y, max.y));

    bool hovered = m_Renderer->calculate_bounding_box(
        _Depth,
        _Position,
        _Rotation,
        _Scale,
        min,
        max).contains(m_Renderer->get_rendering_queue()->get_cursor_postion());
    
    bool pressed =
        hovered &&
        Frenchie::Application::application()->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left);

    // draw background
    m_Renderer->push_rectangle_filled(
        min,
        max,
        retreive_button_background_color(_Enabled, hovered, pressed),
        _Depth + 1.f,
        _Position,
        _Rotation,
        _Scale
    );

    // draw frame
    m_Renderer->push_rectangle_rounded(
        min,
        max,
        m_Style.PushButtonRoundingRadius,
        m_Style.PushButtonFrameWidth,
        retreive_frame_color(_Enabled, hovered, pressed),
        _Depth + 3.f,
        _Position,
        _Rotation,
        _Scale
    );
    
    // draw text
    gs_mat4f transform = m_Renderer->construct_transform_matrix(_Depth, _Position, _Rotation, _Scale);
    gs_vec2f textSize  = textBoundingBox.get_size();
    min = transform * gs_vec4f(min, _Depth, 1.f);
    max = transform * gs_vec4f(max, _Depth, 1.f);

    m_Renderer->push_text(
        _Name,
        m_Style.TextSize,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        _Depth + 6.f,
        _Position,//(max + min) * 0.5f + gs_vec2f(-textSize.x, +textSize.y) * 0.5f,
        _Rotation,
        _Scale
    );

    return pressed && _Enabled;
}