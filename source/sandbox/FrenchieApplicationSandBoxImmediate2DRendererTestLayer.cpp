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
    // create or load window
    ImmedidateUserInterfaceWindow& window = m_Cache[_Name];

    if(window.IsDirty)
    {
        window.CurrentSize  = gs_vec2f(512.f, 512.f);
        window.IsDirty = false;
    }

    // calculate geometry
    gs_2dboxf windowTitleBoundingBox  = m_Renderer->calculate_bounding_box(_Name, m_Style.FontSize, m_Renderer->m_RenderingQueue->get_default_font());
    gs_2dboxf windowBoundingBox       = gs_2dbox(gs_vec2f(0.f, 0.f), window.CurrentSize);
    gs_2dboxf windowFrameBoundingBox  = gs_2dbox(gs_vec2f(0.f, 0.f), gs_vec2f(window.CurrentSize.x, m_Style.FontSize));
    gs_2dboxf windowBottomBoundingBox = gs_2dbox(gs_vec2f(0.f, window.CurrentSize.y - m_Style.FontSize), gs_vec2f(window.CurrentSize.x, window.CurrentSize.y));
    gs_mat4f  wnidowTransformMatrix   = m_Renderer->calculate_transform_matrix(window.Depth, window.CurrentPosition);
    gs_vec2f  cursorPosition          = m_Renderer->get_cursor_postion();

    // catch window move event
    if(windowFrameBoundingBox.transform(wnidowTransformMatrix).contains(cursorPosition))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left))
            window.IsBeingDragged = true;
    }

    // catch window resize top left event
    gs_2d_ellipsef topLeft (windowBoundingBox.Min, 32.f);
    gs_2d_ellipsef topRight(windowBoundingBox.Min + gs_vec2f(window.CurrentSize.x, 0.f), 32.f);
    gs_2d_ellipsef bottomLeft(windowBoundingBox.Max - gs_vec2f(window.CurrentSize.x, 0.f), 32.f);
    gs_2d_ellipsef bottomRight(windowBoundingBox.Max, 32.f);

    if(topLeft.transform(wnidowTransformMatrix).contains(cursorPosition))
    {
        window.IsBeingResizedTopLeft = true;
        m_Renderer->push_arc_filled(topLeft.Center, 32.f, 32.f, 0.f, 360.f, gs_vec4f(255.f, 0.f, 0.f, 128.f), 1000.f, windowBoundingBox.transform(wnidowTransformMatrix).Min);
    }

    if(topRight.transform(wnidowTransformMatrix).contains(cursorPosition))
    {
        window.IsBeingResizedTopRight = true;
        m_Renderer->push_arc_filled(topRight.Center, 32.f, 32.f, 0.f, 360.f, gs_vec4f(255.f, 0.f, 0.f, 128.f), 1000.f, windowBoundingBox.transform(wnidowTransformMatrix).Min);
    }

    if(bottomLeft.transform(wnidowTransformMatrix).contains(cursorPosition))
    {
        window.IsBeingResizedBottomLeft = true;
        m_Renderer->push_arc_filled(bottomLeft.Center, 32.f, 32.f, 0.f, 360.f, gs_vec4f(255.f, 0.f, 0.f, 128.f), 1000.f, windowBoundingBox.transform(wnidowTransformMatrix).Min);
    }

    if(bottomRight.transform(wnidowTransformMatrix).contains(cursorPosition))
    {
        window.IsBeingResizedBottomRight = true;
        m_Renderer->push_arc_filled(bottomRight.Center, 32.f, 32.f, 0.f, 360.f, gs_vec4f(255.f, 0.f, 0.f, 128.f), 1000.f, windowBoundingBox.transform(wnidowTransformMatrix).Min);
    }

    if(!application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left)  &&
        !application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Right) &&
        !application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Middle))
    {
        window.IsBeingDragged            = false;
        window.IsBeingResizedTopLeft     = false;
        window.IsBeingResizedTopRight    = false;
        window.IsBeingResizedBottomLeft  = false;
        window.IsBeingResizedBottomRight = false;
    }

    // back
    m_Renderer->push_rectangle_filled(
        windowBoundingBox.Min,
        windowBoundingBox.Max,
        gs_vec4f(200.f, 200.f, 200.f, 255.f),
        window.Depth,
        window.CurrentPosition
    );

    // frame
    {
        m_Renderer->push_rectangle_filled(
            windowFrameBoundingBox.Min,
            windowFrameBoundingBox.Max,
            gs_vec4f(80.f, 200.f, 200.f, 255.f),
            window.Depth + 1.f,
            window.CurrentPosition
        );
    }

    // bottom
    m_Renderer->push_rectangle_filled(
        windowBottomBoundingBox.Min,
        windowBottomBoundingBox.Max,
        gs_vec4f(0.f, 200.f, 200.f, 255.f),
        window.Depth + 2.f,
        window.CurrentPosition
    );

    // move the window
    if(window.IsBeingDragged)
        window.CurrentPosition = window.PreviousPosition + application()->get_window_cursor_dragdelta();
    else
        window.PreviousPosition = window.CurrentPosition;

    // resize window

    return true;
}

void Immedidate2DRendererTestLayer::pop_window()
{
}

// bool Immedidate2DRendererTestLayer::draw_radio_button_widget(
//     const std::string& _Name,
//     const float&       _Radius,
//     bool&              _Pushed,
//     const bool&        _Enabled,
//     const float&       _Depth,
//     const gs_vec2f&    _Position,
//     const float&       _Rotation,
//     const gs_vec2f&    _Scale)
// {
//     bool hovered = false;

//     gs_2dboxf textBoundingBox = 
//         m_Renderer->calculate_bounding_box(
//             gs_vec2f(0.f),
//             0.f,
//             gs_vec2f(1.f, 1.f),
//             _Name,
//             m_Style.FontSize,
//             m_Renderer->m_RenderingQueue->get_default_font());

//     float radius = textBoundingBox.get_size().y;

//     gs_2dboxf buttonBoundingBox =
//         gs_2dboxf(
//             gs_vec2f(0.f, 0.f),
//             gs_vec2f(radius, radius) * 2.f);
    
//     hovered = m_Renderer->calculate_bounding_box(
//         _Position,
//         _Rotation,
//         _Scale,
//         buttonBoundingBox.Min,
//         buttonBoundingBox.Max).contains(m_Renderer->get_cursor_postion());

//     if(Frenchie::Application::application()
//         ->is_mouse_button_clicked(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
//     {        
//         if(hovered && _Enabled)
//             _Pushed = !_Pushed;
//     }

//     // draw background
//     m_Renderer->push_arc_filled(
//         gs_vec2f(radius, radius),
//         radius,
//         radius,
//         0.f,
//         360.f,
//         retreive_push_button_background_color(_Enabled, hovered, _Pushed),
//         _Depth,
//         _Position,
//         _Rotation,
//         _Scale
//     );

//     if(_Pushed)
//     {
//         m_Renderer->push_arc_filled(
//             gs_vec2f(radius, radius),
//             radius - m_Style.PushButtonFrameWidth,
//             radius - m_Style.PushButtonFrameWidth,
//             0.f,
//             360.f,
//             retrieve_radio_button_pressed_state_color(_Enabled, _Pushed),
//             _Depth + 1,
//             _Position,
//             _Rotation,
//             _Scale
//         );
//     }

//     m_Renderer->push_arc(
//         gs_vec2f(radius, radius),
//         radius,
//         radius,
//         0.f,
//         360.f,
//         m_Style.PushButtonFrameWidth,
//         retreive_push_frame_color(_Enabled, hovered, _Pushed),
//         _Depth + 2,
//         _Position,
//         _Rotation,
//         _Scale
//     );

//     // draw text
//     gs_mat4f transform = m_Renderer->calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);
//     gs_vec2f min = transform * gs_vec4f(buttonBoundingBox.Min, _Depth, 1.f);
//     gs_vec2f max = transform * gs_vec4f(buttonBoundingBox.Max, _Depth, 1.f);

//     m_Renderer->push_text(
//         _Name,
//         m_Style.FontSize,
//         retreive_text_color(_Enabled, false, false),
//         _Depth + 2.f,
//         gs_vec2f(max.x + m_Style.PushButtonFrameWidth, (max.y + min.y) * 0.5f) + gs_vec2f(0.f, -textBoundingBox.get_size().y * 0.5f),
//         _Rotation,
//         _Scale
//     );

//     return true;
// }

// bool Immedidate2DRendererTestLayer::draw_push_button_widget(
//     const std::string& _Name,
//     const gs_vec2f&    _Size,
//     const bool&        _Enabled,
//     const float&       _Depth,
//     const gs_vec2f&    _Position,
//     const float&       _Rotation,
//     const gs_vec2f&    _Scale)
// {
//     gs_2dboxf textBoundingBox = 
//         m_Renderer->calculate_bounding_box(
//             gs_vec2f(0.f),
//             0.f,
//             gs_vec2f(1.f, 1.f),
//             _Name,
//             m_Style.FontSize,
//             m_Renderer->m_RenderingQueue->get_default_font());

//     gs_2dboxf buttonBoundingBox =
//         gs_2dboxf(
//             gs_vec2f(0.f, 0.f),
//             Immediate2DRenderer::bottom_right(gs_vec2f(+gs_max(_Size.x, textBoundingBox.get_size().x), gs_max(_Size.y, textBoundingBox.get_size().y)) + gs_vec2f(m_Style.FontSize, m_Style.FontSize)));

//     bool hovered = m_Renderer->calculate_bounding_box(
//         _Position,
//         _Rotation,
//         _Scale,
//         buttonBoundingBox.Min,
//         buttonBoundingBox.Max).contains(m_Renderer->get_cursor_postion());
    
//     bool pressed =
//         hovered &&
//         Frenchie::Application::application()->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left);

//     // draw background
//     m_Renderer->push_rectangle_rounded_filled(
//         buttonBoundingBox.Min,
//         buttonBoundingBox.Max,
//         m_Style.PushButtonRoundingRadius,
//         retreive_push_button_background_color(_Enabled, hovered, pressed),
//         _Depth,
//         _Position,
//         _Rotation,
//         _Scale
//     );

//     // draw frame
//     m_Renderer->push_rectangle_rounded(
//         buttonBoundingBox.Min,
//         buttonBoundingBox.Max,
//         m_Style.PushButtonRoundingRadius,
//         m_Style.PushButtonFrameWidth,
//         retreive_push_frame_color(_Enabled, hovered, pressed),
//         _Depth + 1.f,
//         _Position,
//         _Rotation,
//         _Scale
//     );
    
//     // draw text
//     gs_mat4f transform = m_Renderer->calculate_transform_matrix(_Depth, _Position, _Rotation, _Scale);
//     gs_vec2f min = transform * gs_vec4f(buttonBoundingBox.Min, _Depth, 1.f);
//     gs_vec2f max = transform * gs_vec4f(buttonBoundingBox.Max, _Depth, 1.f);

//     m_Renderer->push_text(
//         _Name,
//         m_Style.FontSize,
//         retreive_text_color(_Enabled, hovered, pressed),
//         _Depth + 2.f,
//         (min + max) * 0.5f + Immediate2DRenderer::top_left(gs_vec2f(+textBoundingBox.get_size().x, +textBoundingBox.get_size().y) * 0.5f),
//         _Rotation,
//         _Scale
//     );

//     return pressed && _Enabled;
// }