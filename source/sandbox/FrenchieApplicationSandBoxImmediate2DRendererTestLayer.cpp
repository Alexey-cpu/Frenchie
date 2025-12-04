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

    push_window("Some window");

    // draw_push_button_widget(
    //     "BTN !!!",
    //     gs_vec2f(256.f, gs_min(m_Style.FontSize, 8.f)),
    //     true,
    //     0.f,
    //     start
    // );

    // draw_push_button_widget(
    //     "Hello !!! I am ANOTHER Button !!!",
    //     gs_vec2f(256.f, gs_min(m_Style.FontSize, 8.f)),
    //     false,
    //     0.f,
    //     start + Immediate2DRenderer::bottom_right(gs_vec2f(0.f, 256.f))
    // );

    // static bool pushed = true;
    // draw_radio_button_widget(
    //     "Hello !!! I am radio button !!!",
    //     32.f,
    //     pushed,
    //     true,
    //     0.f,
    //     start + Immediate2DRenderer::bottom_right(gs_vec2f(0.f, 512.f))
    // );
}

bool Immedidate2DRendererTestLayer::push_window(
    const std::string&                        _Name,
    const ImmedidateUserInterfaceWindowHints& _Hints)
{
    float _Depth = 0.f;

    ImmedidateUserInterfaceWindow& window = m_Cache[_Name];

    gs_2dboxf windowTitleBoundingBox = 
        m_Renderer->calculate_bounding_box(
            _Depth,
            gs_vec2f(0.f, 0.f),
            0.f,
            gs_vec2f(1.f, 1.f),
            _Name,
            m_Style.FontSize,
            m_Renderer->m_RenderingQueue->get_default_font());

    gs_2dboxf windowFrameBoundingBox =
        gs_2dbox(
            gs_vec2f(0.f, 0.f),
            Immediate2DRenderer::bottom_right(gs_vec2f(window.Size.x, m_Style.FontSize)));

    gs_2dboxf windowBoundingBox =
        gs_2dbox(
            gs_vec2f(0.f, 0.f),
            Immediate2DRenderer::bottom_right(window.Size));

    if(m_Renderer->calculate_bounding_box(
        _Depth,
        window.Position,
        0.f,
        gs_vec2f(1.f, 1.f),
        windowFrameBoundingBox.Min,
        windowFrameBoundingBox.Max).contains(m_Renderer->get_cursor_postion()) &&
        Frenchie::Application::application()->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
    {
        window.Dragging = true;
    }

    if(window.Dragging)
    {
        window.Position = m_Renderer->get_cursor_postion();
    }

    if(!Frenchie::Application::application()->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left)  &&
        !Frenchie::Application::application()->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Right) &&
        !Frenchie::Application::application()->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Middle))
    {
        window.Dragging = false;
    }

    // render window
    m_Renderer->push_rectangle_rounded_filled(
        windowBoundingBox.Min,
        windowBoundingBox.Max,
        m_Style.PushButtonRoundingRadius,
        gs_vec4f(200.f, 200.f, 200.f, 255.f),
        _Depth,
        window.Position
    );

    // render window frame
    {
        float windowFrameTextPadding = 4.f; 

        m_Renderer->push_rectangle_rounded_filled(
            windowFrameBoundingBox.Min,
            windowFrameBoundingBox.Max,
            0.f,
            gs_vec4f(0.f, 0.f, 200.f, 255.f),
            _Depth + 1.f,
            window.Position
        );

        // draw text
        gs_mat4f transform = m_Renderer->calculate_transform_matrix(_Depth, window.Position, 0.f, gs_vec2f(1.f, 1.f));
        gs_vec2f min = transform * gs_vec4f(windowFrameBoundingBox.Min, _Depth, 1.f);
        gs_vec2f max = transform * gs_vec4f(windowFrameBoundingBox.Max, _Depth, 1.f);

        m_Renderer->push_text(
            _Name,
            m_Style.FontSize,
            retreive_text_color(true, false, false),
            _Depth + 2.f,
            gs_vec2f(min.x + windowFrameTextPadding, (min + max).y * 0.5f - windowTitleBoundingBox.get_size().y * 0.5f)
        );
    }

    return true;
}

void Immedidate2DRendererTestLayer::pop_window()
{
}

bool Immedidate2DRendererTestLayer::draw_radio_button_widget(
    const std::string& _Name,
    const float&       _Radius,
    bool&              _Pushed,
    const bool&        _Enabled,
    const float&       _Depth,
    const gs_vec2f&    _Position,
    const float&       _Rotation,
    const gs_vec2f&    _Scale)
{
    bool hovered = false;

    gs_2dboxf textBoundingBox = 
        m_Renderer->calculate_bounding_box(
            _Depth,
            gs_vec2f(0.f),
            0.f,
            gs_vec2f(1.f, 1.f),
            _Name,
            m_Style.FontSize,
            m_Renderer->m_RenderingQueue->get_default_font());

    float radius = textBoundingBox.get_size().y;

    gs_2dboxf buttonBoundingBox =
        gs_2dboxf(
            gs_vec2f(0.f, 0.f),
            gs_vec2f(radius, radius) * 2.f);
    
    hovered = m_Renderer->calculate_bounding_box(
        _Depth,
        _Position,
        _Rotation,
        _Scale,
        buttonBoundingBox.Min,
        buttonBoundingBox.Max).contains(m_Renderer->get_cursor_postion());

    if(Frenchie::Application::application()
        ->is_mouse_button_clicked(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
    {        
        if(hovered && _Enabled)
            _Pushed = !_Pushed;
    }

    // draw background
    m_Renderer->push_arc_filled(
        gs_vec2f(radius, radius),
        radius,
        radius,
        0.f,
        360.f,
        retreive_push_button_background_color(_Enabled, hovered, _Pushed),
        _Depth,
        _Position,
        _Rotation,
        _Scale
    );

    if(_Pushed)
    {
        m_Renderer->push_arc_filled(
            gs_vec2f(radius, radius),
            radius - m_Style.PushButtonFrameWidth,
            radius - m_Style.PushButtonFrameWidth,
            0.f,
            360.f,
            retrieve_radio_button_pressed_state_color(_Enabled, _Pushed),
            _Depth + 1,
            _Position,
            _Rotation,
            _Scale
        );
    }

    m_Renderer->push_arc(
        gs_vec2f(radius, radius),
        radius,
        radius,
        0.f,
        360.f,
        m_Style.PushButtonFrameWidth,
        retreive_push_frame_color(_Enabled, hovered, _Pushed),
        _Depth + 2,
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
        retreive_text_color(_Enabled, false, false),
        _Depth + 2.f,
        gs_vec2f(max.x + m_Style.PushButtonFrameWidth, (max.y + min.y) * 0.5f) + gs_vec2f(0.f, -textBoundingBox.get_size().y * 0.5f),
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
        retreive_push_button_background_color(_Enabled, hovered, pressed),
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
        retreive_push_frame_color(_Enabled, hovered, pressed),
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