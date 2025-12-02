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
        gs_vec2f(0.f, 0.f),
        gs_vec2f(256.f, 128.f),
        true,
        0.f,
        gs_vec2f(width, -height) * 0.5f
    );

    push_button_widget(
        gs_vec2f(0.f, 0.f),
        gs_vec2f(256.f, 128.f),
        false,
        0.f,
        gs_vec2f(width, -height) * 0.5f + gs_vec2f(512.f, 0.f)
    );
}

bool Immedidate2DRendererTestLayer::push_button_widget(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const bool&     _Enabled,
    const float&    _Depth,
    const gs_vec2f& _Position,
    const float&    _Rotation,
    const gs_vec2f& _Scale)
{
    bool pressed = false;

    // draw background
    m_Renderer->push_rectangle_rounded_filled(
        _Min,
        _Max,
        m_Style.PushButtonRoundingRadius,
         _Enabled ?
            m_Style.Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledColor] :
            m_Style.Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledColor],
        _Depth,
        _Position,
        _Rotation,
        _Scale
    );

    if(m_Renderer->calculate_bounding_box(_Depth, _Position, _Rotation, _Scale, _Min, _Max).contains(
       m_Renderer->get_rendering_queue()->get_cursor_postion()))
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Min,
            _Max,
            m_Style.PushButtonRoundingRadius,
            _Enabled ?
                m_Style.Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredColor] :
                m_Style.Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredColor],
            _Depth + 1.f,
            _Position,
            _Rotation,
            _Scale
        );

        if(Frenchie::Application::application()
            ->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
        {
            pressed = true;

            m_Renderer->push_rectangle_rounded_filled(
                _Min,
                _Max,
                m_Style.PushButtonRoundingRadius,
                _Enabled ?
                    m_Style.Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledPressedColor] :
                    m_Style.Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledPressedColor],
                _Depth + 2.f,
                _Position,
                _Rotation,
                _Scale
            );
        }
    }

    // draw frame
    m_Renderer->push_rectangle_rounded(
        _Min,
        _Max,
        m_Style.PushButtonRoundingRadius,
        m_Style.PushButtonFrameWidth,
        _Enabled ?
            m_Style.Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor] :
            m_Style.Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor],
        _Depth + 3.f,
        _Position,
        _Rotation,
        _Scale
    );
    
    return pressed && _Enabled;
}