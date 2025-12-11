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
    if(m_Style.Font.is_null())
        m_Style.Font = m_Renderer->m_RenderingQueue->get_default_font(); 
}

void Immedidate2DRendererTestLayer::frame_update()
{
    if(m_Renderer == nullptr)
        return;

    static bool bettaWindowOpened = true;

    if(push_window("Beta window",
        1.f,
        &bettaWindowOpened,
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutHorizontal))
    {
        if(push_window("Alpha-1 window", 1.f)) pop_window();
        if(push_window("Alpha-2 window", 1.f)) pop_window();
        if(push_window("Alpha-3 window", 1.f)) pop_window();

        for (int i = 0; i < 10; i++)push_close_button_widget();

        //if(push_window("Alpha-4 window", 1.f)) pop_window();

        if(push_window("Container window",
            1.f,
            nullptr,
            ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
            ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
            ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable |
            ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutVertical))
        {
            if(push_window("Theta window",
                1.f,
                nullptr, 
                ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
                ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
                ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable |
                ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutHorizontal))
            {
                if(push_window("Theta-1 window", 1.f)) pop_window();
                if(push_window("Theta-2 window", 1.f)) pop_window();

                pop_window();
            }

            if(push_window("Cappa window",
                1.f,
                nullptr, 
                ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
                ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
                ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable |
                ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutHorizontal))
            {
                if(push_window("Cappa-1 window", 1.f)) pop_window();
                if(push_window("Cappa-2 window", 1.f)) pop_window();

                pop_window();
            }

            pop_window();
        }

        pop_window();
    }

    // static bool independentWindowOpened = true;

    // if(push_window("Independent window",
    //     1.f,
    //     &independentWindowOpened,
    //     ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
    //     ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
    //     ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable |
    //     ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutHorizontal))
    // {
    //     for (int i = 0; i < 10; i++)push_close_button_widget();
        
    //     pop_window();
    // }

    // render cursor
    m_Renderer->push_text(
        std::to_string(m_Renderer->get_cursor_postion().x).append(" ").append(
             std::to_string(m_Renderer->get_cursor_postion().y).append("\t").append(std::to_string(-1000))),
        m_Style.FontSize,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        m_Renderer->calculate_transform_matrix(m_Renderer->get_far_plane(), m_Renderer->get_cursor_postion() + gs_vec3f(Immediate2DRenderer::bottom_right(gs_vec2f(12.f, 12.f)))));

    // rendeer viewport rect
    m_Renderer->push_rectangle(
        m_Renderer->m_Viewport.Min,
        m_Renderer->m_Viewport.Max,
        8.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        0.f);
}

void Immedidate2DRendererTestLayer::frame_finish()
{
    // focus the top most clicked window
    for (int button = ApplicationMouseButton::ApplicationMouseButton_Begin;
             button < ApplicationMouseButton::ApplicationMouseButton_End;
             button++)
    {
        if(application()->is_mouse_button_pressed((ApplicationMouseButton::Button)button))
        {
            ImmedidateUserInterfaceWindow* topMostWindow = nullptr;
            int                            maxDepth      = -1;
            for (auto drawnWindow : m_WindowsDrawList)
            {
                drawnWindow->end_focus();

                if(!drawnWindow->State.WindowBox.transform(drawnWindow->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) ||
                    drawnWindow->State.Depth <= maxDepth)
                {
                    continue;
                }

                maxDepth      = drawnWindow->State.Depth;
                topMostWindow = drawnWindow;
            }

            if(topMostWindow != nullptr)
                topMostWindow->begin_focus();

            break;
        }
    }

    // focus any modified or moved window
    for (int i = 0; i < (int)m_WindowsDrawList.size(); i++)
    {
        if(m_WindowsDrawList[i]->is_being_modified() || m_WindowsDrawList[i]->is_being_moved())
        {
            for (int j = 0; j < (int)m_WindowsDrawList.size(); j++)
                m_WindowsDrawList[j]->end_focus();
            m_WindowsDrawList[i]->begin_focus();
            break;
        }
    }

    // clean up windows hierarchy and layout data
    for (auto& cachedWindow : m_WindowsCache)
    {
        // save state
        if(!cachedWindow.second->is_being_modified())
            cachedWindow.second->Cache = cachedWindow.second->State;

        // TODO: this MUST BE DEFAULTS
        // layouting
        cachedWindow.second->State.LayoutTotalWeight    = 0.f;
        cachedWindow.second->State.LayoutCursorPositon  = gs_vec2f(0.f, 0.f);
        cachedWindow.second->State.LayoutCursorSize     = gs_vec2f(0.f, 0.f);
        cachedWindow.second->State.WindowContentBox     = cachedWindow.second->State.WindowViewportBox;

        // hierarchy
        cachedWindow.second->State.Depth      = 0;
        cachedWindow.second->State.Thickness  = 0;
        cachedWindow.second->State.Parent     = nullptr;
    }

    GS_ASSERT(m_WindowsHierarchy.empty());

    // clear hierarchy and draw lists
    m_WindowsDrawList.clear();
    m_WindowsHierarchy.clear();
}

void Immedidate2DRendererTestLayer::set_next_window_position(const gs_vec2f& _Value)
{
    m_NextWindowPosition = _Value;
}

void Immedidate2DRendererTestLayer::set_next_window_size(const gs_vec2f& _Value)
{
    m_NextWindowSize = _Value;
}

bool Immedidate2DRendererTestLayer::push_close_button_widget()
{
    if(m_WindowsHierarchy.empty()) return false;

    auto window = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1];

    // m_Renderer->push_clip_box(
    //     gs_2dboxf(window->WindowViewportBox.Min, window->WindowBox.Max),
    //     window->WindowTransform);

    float width   = 64.f;
    float height  = 64.f;
    float padding = 8.f;

    window->State.LayoutCursorPositon += window->State.LayoutCursorSize * window->State.LayoutCursorDirection;

    gs_vec2f min = window->State.LayoutCursorPositon + gs_vec2f(m_Style.FrameWidth, m_Style.FrameWidth);
    
    gs_vec2f max = min + gs_vec2f(width, height);

    render_close_button_widget(
        window->State.WindowContentBox.Min + min,
        window->State.WindowContentBox.Min + max,
        window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)window->calculate_child_depth()));

    window->State.LayoutCursorSize      = gs_vec2f(width, height + padding);
    window->State.LayoutCursorDirection = gs_vec2f(0.f, 1.f);

    calculate_window_geometry(window);

    //m_Renderer->pop_clip_box();

    return true;
}

bool Immedidate2DRendererTestLayer::push_window(
    const std::string&                 _Name,
    const float&                       _Weight,
    bool*                              _Opened,
    ImmedidateUserInterfaceWindowHints _Hints)
{
    // request window from cache
    if(m_WindowsCache.find(_Name) == m_WindowsCache.end())
    {
        std::unique_ptr<ImmedidateUserInterfaceWindow> window = std::make_unique<ImmedidateUserInterfaceWindow>();

        gs_vec2f position     = m_NextWindowPosition.has_value() ? m_NextWindowPosition.value() : gs_vec2f(0.f, 0.f);
        gs_vec2f size         = m_NextWindowSize.has_value() ? m_NextWindowSize.value() : gs_vec2f(512.f, 512.f);
        window->Name          = _Name;
        window->State.WindowBox     = gs_2dboxf(position, position + size);
        m_WindowsCache[_Name] = std::move(window);

        // reset optional next frame paramters
        m_NextWindowPosition.reset();
        m_NextWindowSize.reset();
    }

    ImmedidateUserInterfaceWindow* window = m_WindowsCache.find(_Name)->second.get();
    window->State.LayoutFillWeight = _Weight;
    window->State.Hints            = _Hints;

    window->State.WindowContentBox.Min = window->Cache.WindowContentBox.Min + gs_vec2f(0.f, -window->State.VerticalScrollBar.SliderPosition);
    window->State.WindowContentBox.Max = window->Cache.WindowContentBox.Max + gs_vec2f(0.f, -window->State.VerticalScrollBar.SliderPosition);

    if(_Opened == nullptr)
        window->State.Hints &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable;
    else
        window->State.Hints |= ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable;

    // calculate window geometry
    if(!m_WindowsHierarchy.empty())
    {
        window->State.Parent                           = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1];
        window->State.Depth                            = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1]->calculate_child_depth();
        window->State.Parent->State.LayoutTotalWeight += window->State.LayoutFillWeight;

        gs_vec2f childSize = window->State.Parent->State.WindowContentBox.size() * window->State.LayoutFillWeight / window->State.Parent->Cache.LayoutTotalWeight;

        if((window->State.Parent->State.Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutHorizontal))
        {
            window->State.WindowBox = gs_2dboxf(
                window->State.Parent->State.WindowContentBox.Min,
                window->State.Parent->State.WindowContentBox.Min + gs_vec2f(childSize.x, window->State.Parent->State.WindowContentBox.height()));

            window->State.Parent->State.LayoutCursorPositon += window->State.Parent->State.LayoutCursorSize * window->State.Parent->State.LayoutCursorDirection;

            window->State.WindowTransform = window->State.Parent->State.WindowTransform * m_Renderer->calculate_transform_matrix(
                0.f,
                window->State.Parent->State.LayoutCursorPositon);

            window->State.Parent->State.LayoutCursorSize      = gs_vec2f(childSize.x, window->State.Parent->State.WindowContentBox.height());
            window->State.Parent->State.LayoutCursorDirection = gs_vec2f(1.f, 0.f);
        }
        else
        {
            window->State.WindowBox = gs_2dboxf(
                window->State.Parent->State.WindowContentBox.Min,
                window->State.Parent->State.WindowContentBox.Min + gs_vec2f(window->State.Parent->State.WindowBox.width(), childSize.y));

            window->State.Parent->State.LayoutCursorPositon += window->State.Parent->State.LayoutCursorSize * window->State.Parent->State.LayoutCursorDirection;

            window->State.WindowTransform = window->State.Parent->State.WindowTransform * m_Renderer->calculate_transform_matrix(
                0.f,
                window->State.Parent->State.LayoutCursorPositon);

            window->State.Parent->State.LayoutCursorSize      = gs_vec2f(window->State.Parent->State.WindowBox.width(), childSize.y);
            window->State.Parent->State.LayoutCursorDirection = gs_vec2f(0.f, 1.f);
        }

        // recursivelly recompute state.parental geometry
        calculate_window_geometry(window);

        window->State.Hints &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable;
        window->State.Hints &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable;
        window->State.Hints &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable;
    }
    else
    {
        if(window->is_being_focused())
        {
            window->State.Depth = (int)(m_Renderer->get_far_plane() / 2);
        }
        else
        {
            int windowMaximumDepth = 0;
            for (auto drawnWindow : m_WindowsDrawList)
            {
                if(!drawnWindow->is_being_focused())
                    windowMaximumDepth = gs_max(drawnWindow->State.Depth + drawnWindow->State.Thickness, windowMaximumDepth);
            }
            window->State.Depth = windowMaximumDepth;
        }

        calculate_window_geometry(window);
    }

    // check state.parent
    if((window->State.Parent != nullptr) &&
       (std::find(m_WindowsDrawList.begin(), m_WindowsDrawList.end(), window->State.Parent) == m_WindowsDrawList.end()))
    {
        return false;
    }
    
    // check self
    if(_Opened != nullptr && !(*_Opened))
    {
        return false;
    }

    // render window
    render_window_background(window);

    //if(window->State.Parent == nullptr)
    {
        render_window_classic_frame(window, _Opened);
    }

    // render vertical scrollbar
    if((window->State.Hints & ImmedidateUserInterfaceWindowHints_AlwaysVerticalScrollBar) ||
        window->State.WindowViewportBox.height() < window->Cache.WindowContentBox.height())
    {
        if(!(window->State.Hints & ImmedidateUserInterfaceWindowHints_NeverVerticalScrollBar))
            render_window_vertical_scrollbar(window);
    }

    // render horizontal scrollbar
    if((window->State.Hints & ImmedidateUserInterfaceWindowHints_AlwaysHorizontalScrollBar) ||
        window->State.WindowViewportBox.width() < window->Cache.WindowContentBox.width())
    {
        if(!(window->State.Hints & ImmedidateUserInterfaceWindowHints_NeverHorizontalScrollBar)){} // TODO: render horizontal scrollbar here...
    }

    sink_window_events(window);

    // push window into hierarchy stack and draw list
    m_WindowsDrawList.push_back(window);
    m_WindowsHierarchy.push_back(window);

    return true;
}

void Immedidate2DRendererTestLayer::pop_window()
{
    if(!m_WindowsHierarchy.empty())
        m_WindowsHierarchy.pop_back();
}

void Immedidate2DRendererTestLayer::calculate_window_geometry(const ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;

    // frame
    _Window->State.WindowFrameBox = gs_2dboxf(_Window->State.WindowBox.Min, _Window->State.WindowBox.Min + gs_vec2f(_Window->State.WindowBox.width(), m_Style.FontSize));

    // title
    gs_vec2f titleSize = m_Renderer->calculate_bounding_box(_Window->Name, m_Style.FontSize, m_Style.Font).size();
    gs_vec2f titleMin  = gs_vec2f(_Window->State.WindowFrameBox.Min.x + m_Style.FrameWidth, _Window->State.WindowFrameBox.center().y - titleSize.y * 0.5f);
    gs_vec2f titleMax  = titleMin + titleSize;

    _Window->State.WindowTitleBox = gs_2dboxf(titleMin, titleMax);

    // close button
    gs_vec2f closeButtonMin = gs_vec2f(_Window->State.WindowFrameBox.Max.x - m_Style.WindowResizeAngleGizmoRadius - titleSize.y, _Window->State.WindowTitleBox.Min.y);
    gs_vec2f closeButtonMax = closeButtonMin + gs_vec2f(titleSize.y, titleSize.y);
    _Window->State.WindowCloseButtonBox = gs_2dboxf(closeButtonMin, closeButtonMax);

    // vertical scrollbar
    _Window->State.VerticalScrollBar.setup(
        0.f,
        _Window->State.WindowViewportBox.height(),
        _Window->Cache.WindowContentBox.height() + m_Style.WindowScrollBarSliderWidth,
        _Window->State.WindowViewportBox.height());

    _Window->State.WindowVerticalScrollBarBox = gs_2dboxf(
        gs_vec2f(_Window->State.WindowViewportBox.Max.x, _Window->State.WindowViewportBox.Min.y),
        gs_vec2f(_Window->State.WindowViewportBox.Max.x, _Window->State.WindowViewportBox.Max.y) + gs_vec2f(m_Style.WindowScrollBarSliderWidth, 0.f));

    _Window->State.WindowVerticalScrollBarSliderBox = gs_2dboxf(
        _Window->State.WindowVerticalScrollBarBox.Min + gs_vec2f(0.f, _Window->State.VerticalScrollBar.SliderPosition),
        gs_vec2f(_Window->State.WindowVerticalScrollBarBox.Max.x, _Window->State.WindowVerticalScrollBarBox.Min.y + _Window->State.VerticalScrollBar.SliderLength + _Window->State.VerticalScrollBar.SliderPosition));

    // horizontal scrollbar
    // TODO: implemente horizontal scrollbar geometry computation

    // viewport
    _Window->State.WindowViewportBox = gs_2dboxf(
        _Window->State.WindowBox.Min + gs_vec2f(0.f, _Window->State.WindowFrameBox.height()),
        _Window->State.WindowBox.Max + gs_vec2f(-m_Style.WindowScrollBarSliderWidth, 0.f));
    
    // content
    _Window->State.WindowContentBox  = gs_2dboxf(
        _Window->State.WindowViewportBox.Min,
        _Window->State.WindowViewportBox.Max,
        _Window->State.WindowViewportBox.Min + _Window->State.LayoutCursorPositon + _Window->State.LayoutCursorSize);

    _Window->State.WindowContentBox.Min += gs_vec2f(0.f, -_Window->State.VerticalScrollBar.SliderPosition * _Window->State.VerticalScrollBar.SliderScale);
    _Window->State.WindowContentBox.Max += gs_vec2f(0.f, -_Window->State.VerticalScrollBar.SliderPosition * _Window->State.VerticalScrollBar.SliderScale);

    calculate_window_geometry(_Window->State.Parent);
}

void Immedidate2DRendererTestLayer::render_window_background(ImmedidateUserInterfaceWindow* _Window)
{
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowBox.Min,
        _Window->State.WindowBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));

    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowBox.Min,
        _Window->State.WindowBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.FrameWidth,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));

    // draw content gizmo
    m_Renderer->push_rectangle(
        _Window->Cache.WindowContentBox.Min,
        _Window->Cache.WindowContentBox.Max,
        m_Style.FrameWidth,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix(m_Renderer->get_far_plane()));

    // m_Renderer->push_rectangle(
    //     _Window->WindowViewportBox.Min,
    //     _Window->WindowViewportBox.Max,
    //     m_Style.FrameWidth,
    //     gs_vec4f(0.f, 255.f, 0.f, 255.f),
    //     _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix(m_Renderer->get_far_plane()));
}

void Immedidate2DRendererTestLayer::render_window_classic_frame(ImmedidateUserInterfaceWindow* _Window, bool* _Opened)
{
    // frame rectangle
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowFrameBox.Min,
        _Window->State.WindowFrameBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));

    // frame border
    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowFrameBox.Min,
        _Window->State.WindowFrameBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.FrameWidth,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));

    // window title
    m_Renderer->push_clip_box(
        gs_2dboxf(
            _Window->State.WindowFrameBox.Min + gs_vec2f(m_Style.FrameWidth, 0.f),
            gs_vec2f(_Window->State.WindowCloseButtonBox.Min.x, _Window->State.WindowCloseButtonBox.Max.y)),
        _Window->State.WindowTransform);

    m_Renderer->push_text(
        _Window->Name,
        m_Style.FontSize,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextEnabledColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth(), _Window->State.WindowTitleBox.Min));

    m_Renderer->pop_clip_box();

    // frame close button
    if(_Window->State.Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable && _Opened != nullptr)
    {
        *_Opened = !render_close_button_widget(
            _Window->State.WindowCloseButtonBox.Min,
            _Window->State.WindowCloseButtonBox.Max,
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
    }
}

void Immedidate2DRendererTestLayer::render_window_vertical_scrollbar(ImmedidateUserInterfaceWindow* _Window)
{
    m_Renderer->push_clip_box(
        gs_2dboxf(_Window->State.WindowVerticalScrollBarBox.Min - m_Style.FrameWidth, _Window->State.WindowVerticalScrollBarBox.Max + m_Style.FrameWidth),
        _Window->State.WindowTransform);

    // draw scroll panel
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowVerticalScrollBarBox.Min,
        _Window->State.WindowVerticalScrollBarBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));

    // draw slider
    if(_Window->State.WindowVerticalScrollBarSliderBox.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.WindowVerticalScrollBarSliderBox.Min,
            _Window->State.WindowVerticalScrollBarSliderBox.Max,
            m_Style.FrameRoundingRadius,
            application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left) ?
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderPressedColor] :
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderHoveredColor],
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
    }
    else
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.WindowVerticalScrollBarSliderBox.Min,
            _Window->State.WindowVerticalScrollBarSliderBox.Max,
            m_Style.FrameRoundingRadius,
            m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderColor],
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
    }

    // draw frame
    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowVerticalScrollBarBox.Min,
        _Window->State.WindowVerticalScrollBarBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.FrameWidth,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));

    m_Renderer->pop_clip_box();
}

void Immedidate2DRendererTestLayer::sink_window_events(ImmedidateUserInterfaceWindow* _Window)
{
    // begin poll events
    gs_2d_ellipsef resizeTopLeft     = gs_2d_ellipsef(_Window->State.WindowBox.Min, m_Style.WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeTopRight    = gs_2d_ellipsef(_Window->State.WindowBox.Min + gs_vec2f(_Window->State.WindowBox.width(), 0.f), m_Style.WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeBottomLeft  = gs_2d_ellipsef(_Window->State.WindowBox.Max - gs_vec2f(_Window->State.WindowBox.width(), 0.f), m_Style.WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeBottomRight = gs_2d_ellipsef(_Window->State.WindowBox.Max, m_Style.WindowResizeAngleGizmoRadius);
    gs_2dboxf      resizeTop         = gs_2dboxf(_Window->State.WindowBox.Min - gs_vec2f(0.f, m_Style.WindowResizeSideGizmoWidth), _Window->State.WindowBox.Min + gs_vec2f(_Window->State.WindowBox.width(), m_Style.WindowResizeSideGizmoWidth));
    gs_2dboxf      resizeLeft        = gs_2dboxf(_Window->State.WindowBox.Min - gs_vec2f(m_Style.WindowResizeSideGizmoWidth, 0.f), _Window->State.WindowBox.Min + gs_vec2f(m_Style.WindowResizeSideGizmoWidth, _Window->State.WindowBox.height()));
    gs_2dboxf      resizeRight       = gs_2dboxf(_Window->State.WindowBox.Min + gs_vec2f(_Window->State.WindowBox.width() - m_Style.WindowResizeSideGizmoWidth, 0.f), _Window->State.WindowBox.Max + gs_vec2f(m_Style.WindowResizeSideGizmoWidth, 0.f));
    gs_2dboxf      resizeBottom      = gs_2dboxf(_Window->State.WindowBox.Min + gs_vec2f(0.f, _Window->State.WindowBox.height() - m_Style.WindowResizeSideGizmoWidth), _Window->State.WindowBox.Max + gs_vec2f(0.f, m_Style.WindowResizeSideGizmoWidth));

    // poll window resize events
    if( (_Window->State.Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable) &&
        application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left)             &&
        !_Window->is_being_scrolled() &&
        !_Window->is_being_resized()  &&
        !_Window->is_being_moved())
    {
        if(resizeTopLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
            _Window->begin_resize_top_left();
        else if(resizeTopRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
            _Window->begin_resize_top_right();
        else if(resizeBottomLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
            _Window->begin_resize_bottom_left();
        else if(resizeBottomRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
            _Window->begin_resize_bottom_right();
        else if(resizeTop.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
            _Window->begin_resize_top();
        else if(resizeLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
            _Window->begin_resize_left();
        else if(resizeRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
            _Window->begin_resize_right();
        else if(resizeBottom.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
            _Window->begin_resize_bottom();
    }

    // poll window move event
    if( (_Window->State.Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable)      &&
        application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left)                &&
         _Window->State.WindowFrameBox.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) &&
        !_Window->is_being_resized() &&
        !_Window->is_being_scrolled())
    {
        _Window->being_move();
    }

    // poll window vertical scroll event
    if( application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left)                                  &&
         _Window->State.WindowVerticalScrollBarSliderBox.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) &&
        !_Window->is_being_resized() &&
        !_Window->is_being_moved())
    {
        _Window->begin_scroll_vertically();
    }

    // end poll events
    bool anyWindowIsBeingModified   = false;
    bool allMouseButtonsAreReleased = true;

    for (auto& cachedwindow : m_WindowsCache)
    {
        if(cachedwindow.second->Name == _Window->Name)
            continue;

        if(cachedwindow.second->is_being_modified())
            anyWindowIsBeingModified = true;
    }

    for (int button = ApplicationMouseButton::Button::ApplicationMouseButton_Begin;
             button < ApplicationMouseButton::Button::ApplicationMouseButton_End; button++)
    {
        allMouseButtonsAreReleased =
            allMouseButtonsAreReleased && !application()->is_mouse_button_down((ApplicationMouseButton::Button)button);
    }

    if(allMouseButtonsAreReleased || anyWindowIsBeingModified)
    {
        _Window->end_move();
        _Window->end_resize();
        _Window->end_scroll();
    }

    // process events
    if(!anyWindowIsBeingModified && (_Window->State.Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
    {
        if((resizeTopLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_top_left()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeTopLeft.Center,
                resizeTopLeft.Radius,
                resizeTopLeft.Radius,
                0.f,
                360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
        }
        else if((resizeTopRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_top_right()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeTopRight.Center,
                resizeTopRight.Radius,
                resizeTopRight.Radius,
                0.f,
                360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
        }
        else if((resizeBottomLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_bottom_left()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeBottomLeft.Center,
                resizeBottomLeft.Radius,
                resizeBottomLeft.Radius,
                0.f, 360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
        }
        else if((resizeBottomRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_bottom_right()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeBottomRight.Center,
                resizeBottomRight.Radius,
                resizeBottomRight.Radius,
                0.f,
                360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
        }
        else if((resizeTop.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_top()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeTop.Min,
                resizeTop.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
        }
        else if((resizeLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_left()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeLeft.Min,
                resizeLeft.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
        }
        else if((resizeRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_right()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeRight.Min,
                resizeRight.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
        }
        else if((resizeBottom.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_bottom()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeBottom.Min,
                resizeBottom.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_depth()));
        }
    }

    if(_Window->is_being_resized())
    {
        gs_2dboxf estimatedBox;
        gs_vec2f  cursorDelta = application()->get_window_cursor_dragdelta();

        if(_Window->is_being_resized_top_left())
        {
            estimatedBox = gs_2dboxf(_Window->Cache.WindowBox.Min + cursorDelta, _Window->Cache.WindowBox.Max);
        }
        else if(_Window->is_being_resized_top_right())
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Window->Cache.WindowBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
        }
        else if(_Window->is_being_resized_bottom_left())
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Window->Cache.WindowBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
        }
        else if(_Window->is_being_resized_bottom_right())
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min,
                _Window->Cache.WindowBox.Max + application()->get_window_cursor_dragdelta());
        }
        else if(_Window->is_being_resized_top())
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Window->Cache.WindowBox.Max);
        }
        else if(_Window->is_being_resized_left())
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Window->Cache.WindowBox.Max);
        }
        else if(_Window->is_being_resized_right())
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min,
                _Window->Cache.WindowBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
        }
        else if(_Window->is_being_resized_bottom())
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min,
                _Window->Cache.WindowBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
        }

        float minX = estimatedBox.size().x > m_Style.WindowMinimumWidth ? estimatedBox.Min.x : _Window->Cache.WindowBox.Min.x + (_Window->State.WindowBox.Min.x - _Window->Cache.WindowBox.Min.x);
        float maxX = estimatedBox.size().x > m_Style.WindowMinimumWidth ? estimatedBox.Max.x : _Window->Cache.WindowBox.Max.x + (_Window->State.WindowBox.Max.x - _Window->Cache.WindowBox.Max.x);
        float minY = estimatedBox.size().y > m_Style.WindowMinimumHeight ? estimatedBox.Min.y : _Window->Cache.WindowBox.Min.y + (_Window->State.WindowBox.Min.y - _Window->Cache.WindowBox.Min.y);
        float maxY = estimatedBox.size().y > m_Style.WindowMinimumHeight ? estimatedBox.Max.y : _Window->Cache.WindowBox.Max.y + (_Window->State.WindowBox.Max.y - _Window->Cache.WindowBox.Max.y);
        
        _Window->State.WindowBox = gs_2dboxf(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));
    }
    else if(_Window->is_being_moved())
    {
        _Window->State.WindowBox = gs_2dboxf(
            _Window->Cache.WindowBox.Min + application()->get_window_cursor_dragdelta(),
            _Window->Cache.WindowBox.Max + application()->get_window_cursor_dragdelta());

        // dock space
    }
    else if(_Window->is_being_scrolled_horizontally())
    {
        // TODO: rocess vertical scroll here
    }
    else if(_Window->is_being_scrolled_vertically())
    {
        // TODO: rocess vertical scroll here
        _Window->State.VerticalScrollBar.reposition(
            _Window->Cache.VerticalScrollBar.SliderPosition + application()->get_window_cursor_dragdelta().y);
    }
    else
    {
        // _Window->Cache.WindowBox                        = _Window->WindowBox;
        // _Window->VerticalScrollBar.SliderPreviousPosition = _Window->VerticalScrollBar.SliderPosition;
    }
}

bool Immedidate2DRendererTestLayer::render_close_button_widget(const gs_vec2f& _Min, const gs_vec2f& _Max, const gs_mat4f& _Transform)
{
    auto retrieve_close_button_color = [this](const bool& _Pressed, const bool& _Hovered)->gs_vec4f
    {
        if(_Pressed) return m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonPressedColor];
        if(_Hovered) return m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonHoveredColor];
        return m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonDefaultColor];
    };

    bool hovered   = gs_2dboxf(_Min, _Max).transform(_Transform).contains(m_Renderer->get_cursor_postion());
    
    m_Renderer->push_arc_filled(
        gs_2dboxf(_Min, _Max).center(),
        gs_2dboxf(_Min, _Max).width()  * 0.5f,
        gs_2dboxf(_Min, _Max).height() * 0.5f,
        0.f,
        360.f,
        retrieve_close_button_color(hovered && application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left), hovered),
       _Transform);

    m_Renderer->push_line(
        _Min + gs_vec2f(gs_2dboxf(_Min, _Max).width(), gs_2dboxf(_Min, _Max).height()) * 0.25f,
        _Max - gs_vec2f(gs_2dboxf(_Min, _Max).width(), gs_2dboxf(_Min, _Max).height()) * 0.25f,
        2.f,
        gs_vec4f(0, 0, 0, 255.f),
        _Transform * m_Renderer->calculate_transform_matrix(1.f));

    m_Renderer->push_line(
        gs_vec2f(_Max.x, _Min.y) + gs_vec2f(-gs_2dboxf(_Min, _Max).width(), +gs_2dboxf(_Min, _Max).height()) * 0.25f,
        gs_vec2f(_Min.x, _Max.y) + gs_vec2f(+gs_2dboxf(_Min, _Max).width(), -gs_2dboxf(_Min, _Max).height()) * 0.25f,
        2.f,
        gs_vec4f(0, 0, 0, 255.f),
        _Transform * m_Renderer->calculate_transform_matrix(2.f));
    
    return hovered && application()->is_mouse_button_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Left);
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