#include <FrenchieApplicationSandBoxImmediate2DRendererTestLayer.hpp>

using namespace Frenchie::Application;

bool Immedidate2DRendererTestLayer::awake()
{
    // setup renderer
    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

    // setup color scheme
    m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledColor        ] = gs_vec4f(4, 41, 189, 255);
    m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredColor ] = gs_vec4f(60, 98, 250, 255);
    m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledPressedColor ] = gs_vec4f(87, 120, 250, 255);
    m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor   ] = gs_vec4f(1, 16, 77, 255);
    m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledColor       ] = gs_vec4f(53, 53, 54, 255);
    m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredColor] = gs_vec4f(87, 87, 89, 255);
    m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledPressedColor] = gs_vec4f(117, 117, 120, 255);
    m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor  ] = gs_vec4f(30, 30, 31, 255);

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
        32.f,
        16.f,
        true,
        0.f,
        gs_vec2f(width, -height) * 0.5f
    );
}

bool Immedidate2DRendererTestLayer::push_button_widget(
    const gs_vec2f& _Min,
    const gs_vec2f& _Max,
    const float&    _Radius,
    const float&    _FrameWidth,
    const bool&     _Enabled,
    const float&    _Depth,
    const gs_vec2f& _Position,
    const float&    _Rotation,
    const gs_vec2f& _Scale)
{
    gs_rectf box     = m_Renderer->calculate_bounding_box(_Depth, _Position, _Rotation, _Scale, _Min, _Max);
    gs_vec2f cursor  = Frenchie::Application::application_rendering_queue()->get_cursor_postion();
    bool     pressed = false;

    if(_Enabled)
    {
            m_Renderer->push_rectangle_rounded_filled(
                _Min,
                _Max,
                _Radius,
                m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledColor],
                _Depth,
                _Position,
                _Rotation,
                _Scale
            );

        // draw background
        if(box.contains(cursor))
        {
            m_Renderer->push_rectangle_rounded_filled(
                _Min,
                _Max,
                _Radius,
                m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredColor],
                _Depth + 1.f,
                _Position,
                _Rotation,
                _Scale
            );

            if(Frenchie::Application::application()
                ->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
            {
                pressed = m_Renderer->push_rectangle_rounded_filled(
                    _Min,
                    _Max,
                    _Radius,
                    m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledPressedColor],
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
            _Radius,
            _FrameWidth,
            m_Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor],
            _Depth + 3.f,
            _Position,
            _Rotation,
            _Scale
        );
    }
    
     return pressed && _Enabled;
}