#include <FrenchieApplicationSandBoxImmediate2DRendererTestLayer.hpp>

using namespace Frenchie::Application;

// Immedidate2DRendererTestLayer
ImmedidateUserInterfaceContextLayer::ImmedidateUserInterfaceContextLayer(){}
ImmedidateUserInterfaceContextLayer::~ImmedidateUserInterfaceContextLayer(){}

bool ImmedidateUserInterfaceContextLayer::awake()
{
    if(m_Style == nullptr)
        m_Style = std::make_shared<ImmedidateUserInterfaceStyle>();

    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

    return m_Renderer != nullptr && m_Style != nullptr;
}

void ImmedidateUserInterfaceContextLayer::frame_start()
{
    if(m_Style->Font.is_null())
        m_Style->Font = m_Renderer->m_RenderingQueue->get_default_font();
}

void ImmedidateUserInterfaceContextLayer::frame_update()
{
    if(m_Renderer == nullptr)
        return;

    static bool bettaWindowOpened = true;

    if(begin_window("Beta window",
        &bettaWindowOpened,
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutChildrenHorizontally
    ))
    {
        for (int i = 0; i < 10; i++)
        {
            if(begin_window(std::string("Child").append(std::to_string(i)))) end_window();
        }

        // for (int i = 0; i < 10; i++)
        // {
        //     same_line();
        //     close_button_widget();
        // }

        // if(begin_window("Container window",
        //     nullptr,
        //     ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
        //     ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
        //     ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
        // {
        //     same_line();
        //     if(begin_window("Theta window",
        //         nullptr, 
        //         ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
        //         ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
        //         ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
        //     {
        //         if(begin_window("Theta-1 window")) end_window();
        //         if(begin_window("Theta-2 window")) end_window();

        //         end_window();
        //     }

        //     same_line();
        //     if(begin_window("Cappa window",
        //         nullptr, 
        //         ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
        //         ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
        //         ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
        //     {
        //         if(begin_window("Cappa-1 window")) end_window();
        //         if(begin_window("Cappa-2 window")) end_window();

        //         end_window();
        //     }

        //     end_window();
        // }

        // for (int i = 0; i < 10; i++)
        // {
        //     close_button_widget();
        // }

        end_window();
    }

    static bool independentWindowOpened = true;

    if(begin_window("Independent window",
        &independentWindowOpened,
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable // | ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutHorizontal
    ))
    {
        for (int i = 0; i < 10; i++)close_button_widget();
        
        end_window();
    }

    // render cursor
    m_Renderer->push_text(
        std::to_string(m_Renderer->get_cursor_postion().x).append(" ").append(
             std::to_string(m_Renderer->get_cursor_postion().y).append("\t").append(std::to_string(-1000))),
        m_Style->FontSize,
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

void ImmedidateUserInterfaceContextLayer::frame_finish()
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
                end_focus(drawnWindow);

                if(!drawnWindow->State.WindowBox.transform(drawnWindow->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) ||
                    drawnWindow->State.Depth <= maxDepth)
                {
                    continue;
                }

                maxDepth      = drawnWindow->State.Depth;
                topMostWindow = drawnWindow;
            }

            if(topMostWindow != nullptr)
                begin_focus(topMostWindow);

            break;
        }
    }

    // focus any modified or moved window
    for (int i = 0; i < (int)m_WindowsDrawList.size(); i++)
    {
        if(is_being_modified(m_WindowsDrawList[i]) || is_being_moved(m_WindowsDrawList[i]))
        {
            for (int j = 0; j < (int)m_WindowsDrawList.size(); j++)
                end_focus(m_WindowsDrawList[j]);
            begin_focus(m_WindowsDrawList[i]);
            break;
        }
    }

    // poll windows events
    receive_window_events();

    // clean up windows hierarchy and layout data
    for (auto& cachedWindow : m_WindowsCache)
    {
        // save state
        if(!is_being_modified(cachedWindow.second.get()))
            cachedWindow.second->Cache = cachedWindow.second->State;

        // TODO: this MUST BE DEFAULTS
        // layouting
        cachedWindow.second->State.LayoutTotalWeight    = 0.f;
        cachedWindow.second->State.LayoutCursorPositon  = gs_vec2f(0.f, 0.f);
        cachedWindow.second->State.LayoutCursorSize     = gs_vec2f(0.f, 0.f);
        cachedWindow.second->State.LayoutChildIndex     = 0;
        cachedWindow.second->State.LayoutChildrenCount  = 0;

        cachedWindow.second->State.WindowScrollAreaBox  = cachedWindow.second->State.WindowViewportBox;
        cachedWindow.second->State.WindowContentBox     = gs_2dboxf(cachedWindow.second->State.WindowViewportBox.Min, cachedWindow.second->State.WindowViewportBox.Min);

        //cachedWindow.second->Changed = cachedWindow.second->State;

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

void ImmedidateUserInterfaceContextLayer::set_next_window_position(const gs_vec2f& _Value)
{
    m_NextWindowPosition = _Value;
}

void ImmedidateUserInterfaceContextLayer::set_next_window_size(const gs_vec2f& _Value)
{
    m_NextWindowSize = _Value;
}

void ImmedidateUserInterfaceContextLayer::same_line()
{
    m_NextCursorDirection = gs_vec2f(1.f, 0.f);
}

bool ImmedidateUserInterfaceContextLayer::begin_window(const std::string& _Name, bool* _Opened, ImmedidateUserInterfaceWindowHints _Hints)
{
    // request window from cache
    if(m_WindowsCache.find(_Name) == m_WindowsCache.end())
    {
        std::unique_ptr<ImmedidateUserInterfaceWindow> window = std::make_unique<ImmedidateUserInterfaceWindow>();

        gs_vec2f position = m_NextWindowPosition.has_value() ? m_NextWindowPosition.value() : gs_vec2f(0.f, 0.f);
        gs_vec2f size     = m_NextWindowSize.has_value() ? m_NextWindowSize.value() : gs_vec2f(512.f, 512.f);

        window->Name            = _Name;
        window->State.WindowBox = gs_2dboxf(position, position + size);
        m_WindowsCache[_Name]   = std::move(window);

        // reset optional next frame paramters
        m_NextWindowPosition.reset();
        m_NextWindowSize.reset();
    }

    ImmedidateUserInterfaceWindow* window = m_WindowsCache.find(_Name)->second.get();
    window->State.Settings                = _Hints;

    if(!m_WindowsHierarchy.empty())
    {
        window->State.Parent                           = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1];
        window->State.Depth                            = calculate_child_depth(window->State.Parent);
        window->State.LayoutChildIndex                 = window->State.Parent->State.LayoutChildrenCount++;
        window->State.Parent->State.LayoutTotalWeight += window->State.LayoutFillWeight;

        // compute parent layout box
        gs_2dboxf parentLayoutBox = gs_2dboxf(
            window->State.Parent->State.WindowScrollAreaBox.Min,
            window->State.Parent->State.WindowScrollAreaBox.Min + window->State.Parent->State.WindowViewportBox.size());

        // compute child layout size
        gs_vec2f childLayoutSize =
            parentLayoutBox.size() * window->State.LayoutFillWeight / window->State.Parent->Cache.LayoutTotalWeight;

        if(window->State.Parent->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutChildrenHorizontally)
        {
            childLayoutSize = gs_vec2f(childLayoutSize.x, parentLayoutBox.height());
        }
        else if(window->State.Parent->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutChildrenVertically)
        {
            childLayoutSize = gs_vec2f(parentLayoutBox.width(), childLayoutSize.y);
        }

        // move cursor
        window->State.Parent->State.LayoutCursorPositon +=
            window->State.Parent->State.LayoutCursorSize * window->State.Parent->State.LayoutCursorDirection;

        if(window->State.Parent->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutChildrenHorizontally)
        {
            window->State.WindowBox = gs_2dboxf(
                parentLayoutBox.Min,
                parentLayoutBox.Min + childLayoutSize);

            window->State.WindowTransform = window->State.Parent->State.WindowTransform * m_Renderer->calculate_transform_matrix(
                0.f,
                window->State.Parent->State.LayoutCursorPositon);

            window->State.Parent->State.LayoutCursorSize      = childLayoutSize;
            window->State.Parent->State.LayoutCursorDirection = gs_vec2f(1.f, 0.f);
        }
        
        else if(window->State.Parent->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutChildrenVertically)
        {
            window->State.WindowBox = gs_2dboxf(
                parentLayoutBox.Min,
                parentLayoutBox.Min + childLayoutSize);

            window->State.WindowTransform = window->State.Parent->State.WindowTransform * m_Renderer->calculate_transform_matrix(
                0.f,
                window->State.Parent->State.LayoutCursorPositon);

            window->State.Parent->State.LayoutCursorSize      = childLayoutSize;
            window->State.Parent->State.LayoutCursorDirection = gs_vec2f(0.f, 1.f);
        }
        else
        {

        }

        window->State.Settings &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable;
        //window->State.Hints &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable;
        window->State.Settings &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable;

        // update parental depth
        auto parent = window->State.Parent;

        while (parent)
        {
            parent->State.Depth = calculate_child_depth(parent);
            parent = parent->State.Parent;
        }
    }
    else
    {
        if(is_being_focused(window))
        {
            window->State.Depth = (int)(m_Renderer->get_far_plane() / 2);
        }
        else
        {
            int windowMaximumDepth = 0;
            for (auto drawnWindow : m_WindowsDrawList)
            {
                if(!is_being_focused(drawnWindow))
                    windowMaximumDepth = gs_max(drawnWindow->State.Depth + drawnWindow->State.Thickness, windowMaximumDepth);
            }
            window->State.Depth = windowMaximumDepth;
        }
    }

    if(_Opened == nullptr)
        window->State.Settings &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable;
    else
        window->State.Settings |= ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable;

    calculate_window_geometry(window);
    
    // check self
    if((window->State.Parent != nullptr) && (std::find(m_WindowsDrawList.begin(), m_WindowsDrawList.end(), window->State.Parent) == m_WindowsDrawList.end()) || (_Opened != nullptr && !(*_Opened)))
        return false;

    render_window_background(window);

    //if(window->State.Parent == nullptr)
    {
        bool frameCloseButtonPressed = render_window_classic_frame(window);
        if(_Opened != nullptr)
            *_Opened = !frameCloseButtonPressed;
    }

    render_window_clipbox(window);

    // push window into hierarchy stack and draw list
    m_WindowsDrawList.push_back(window);
    m_WindowsHierarchy.push_back(window);

    return true;
}

void ImmedidateUserInterfaceContextLayer::end_window()
{
    if(m_WindowsHierarchy.empty()) return;

    m_Renderer->pop_clip_box();

    auto window = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1];

    if(needs_vertical_scroll_bar(window))
        render_window_vertical_scrollbar(window);
    if(needs_horizontal_scroll_bar(window))
        render_window_horizontal_scrollbar(window);

    render_window_resize_events_gizmos(window);

    process_window_events(window);

    m_WindowsHierarchy.pop_back();
}

bool ImmedidateUserInterfaceContextLayer::close_button_widget(const gs_vec2f& _Size)
{
    if(m_WindowsHierarchy.empty()) return false;

    auto window = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1];

    auto box = calculate_widget_geometry(_Size);

    render_close_button_widget(
        gs_2dboxf(box.Min + window->State.WindowScrollAreaBox.Min, box.Max + window->State.WindowScrollAreaBox.Min),
        window->State.WindowInnerClipAreaBox,
        window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(window)));

    return true;
}

void ImmedidateUserInterfaceContextLayer::calculate_window_geometry(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;

    // frame
    {
        // frame bounding box
        _Window->State.WindowFrameBox =
            gs_2dboxf(
                _Window->State.WindowBox.Min,
                _Window->State.WindowBox.Min + gs_vec2f(_Window->State.WindowBox.width(), m_Style->FontSize));

        // frame title
        gs_vec2f titleSize = m_Renderer->calculate_bounding_box(_Window->Name, m_Style->FontSize, m_Style->Font).size();
        gs_vec2f titleMin  = gs_vec2f(
            _Window->State.WindowFrameBox.Min.x + m_Style->FrameWidth,
            _Window->State.WindowFrameBox.center().y - titleSize.y * 0.5f);
        gs_vec2f titleMax  = titleMin + titleSize;

        _Window->State.WindowTitleBox = gs_2dboxf(titleMin, titleMax);

        // frame close button
        gs_vec2f closeButtonMin = gs_vec2f(
            _Window->State.WindowFrameBox.Max.x - m_Style->WindowResizeAngleGizmoRadius - titleSize.y,
            _Window->State.WindowTitleBox.Min.y);
        gs_vec2f closeButtonMax = closeButtonMin + gs_vec2f(titleSize.y, titleSize.y);
        _Window->State.WindowCloseButtonBox = gs_2dboxf(closeButtonMin, closeButtonMax);
    }

    // viewport
    {
        float verticalScrollBarPadding   = needs_vertical_scroll_bar(_Window)   ? m_Style->WindowScrollBarSliderWidth : 0.f;
        float horizontalScrollBarPadding = needs_horizontal_scroll_bar(_Window) ? m_Style->WindowScrollBarSliderWidth : 0.f;

        _Window->State.WindowViewportBox = gs_2dboxf(
            _Window->State.WindowBox.Min + gs_vec2f(0.f, _Window->State.WindowFrameBox.height()),
            _Window->State.WindowBox.Max - gs_vec2f(verticalScrollBarPadding, horizontalScrollBarPadding));
    }

    // vertical scrollbar
    {
        _Window->State.VerticalScrollBar.setup(
            0.f,
            _Window->State.WindowViewportBox.height(),
            _Window->Cache.WindowContentBox.height() + m_Style->WindowScrollBarSliderWidth,
            _Window->State.WindowViewportBox.height());

        if(!needs_vertical_scroll_bar(_Window))
            _Window->State.VerticalScrollBar.SliderPosition = 0.f;

        _Window->State.WindowVerticalScrollBarBox = gs_2dboxf(
            gs_vec2f(_Window->State.WindowViewportBox.Max.x, _Window->State.WindowViewportBox.Min.y),
            gs_vec2f(_Window->State.WindowViewportBox.Max.x, _Window->State.WindowViewportBox.Max.y) + gs_vec2f(m_Style->WindowScrollBarSliderWidth, 0.f));

        _Window->State.WindowVerticalScrollBarSliderBox = gs_2dboxf(
            _Window->State.WindowVerticalScrollBarBox.Min + gs_vec2f(0.f, _Window->State.VerticalScrollBar.SliderPosition),
            gs_vec2f(_Window->State.WindowVerticalScrollBarBox.Max.x, _Window->State.WindowVerticalScrollBarBox.Min.y + _Window->State.VerticalScrollBar.SliderLength + _Window->State.VerticalScrollBar.SliderPosition));
    }

    // horizontal scrollbar
    {
        _Window->State.HorizontalScrollBar.setup(
            0.f,
            _Window->State.WindowViewportBox.width(),
            _Window->Cache.WindowContentBox.width() + m_Style->WindowScrollBarSliderWidth,
            _Window->State.WindowViewportBox.width());

        if(!needs_horizontal_scroll_bar(_Window))
            _Window->State.HorizontalScrollBar.SliderPosition = 0.f;

        _Window->State.WindowHorizontalScrollBarBox = gs_2dboxf(
            gs_vec2f(_Window->State.WindowViewportBox.Min.x, _Window->State.WindowViewportBox.Max.y),
            gs_vec2f(_Window->State.WindowViewportBox.Max.x, _Window->State.WindowViewportBox.Max.y) + gs_vec2f(0.f, m_Style->WindowScrollBarSliderWidth));

        _Window->State.WindowHorizontalScrollBarSliderBox = gs_2dboxf(
            _Window->State.WindowHorizontalScrollBarBox.Min + gs_vec2f(_Window->State.HorizontalScrollBar.SliderPosition, 0.f),
            gs_vec2f(_Window->State.WindowHorizontalScrollBarBox.Min.x + _Window->State.HorizontalScrollBar.SliderLength + _Window->State.HorizontalScrollBar.SliderPosition, _Window->State.WindowHorizontalScrollBarBox.Max.y));
    }
    
    // scroll area
    {
        _Window->State.WindowScrollAreaBox  = gs_2dboxf(
            _Window->State.WindowViewportBox.Min,
            _Window->State.WindowViewportBox.Max,
            _Window->State.WindowViewportBox.Min + _Window->State.LayoutCursorPositon + _Window->State.LayoutCursorSize);

        _Window->State.WindowScrollAreaBox.Min += gs_vec2f(
            -_Window->State.HorizontalScrollBar.SliderPosition * _Window->State.HorizontalScrollBar.SliderScale,
            -_Window->State.VerticalScrollBar.SliderPosition * _Window->State.VerticalScrollBar.SliderScale);
        
        _Window->State.WindowScrollAreaBox.Max += gs_vec2f(
            -_Window->State.HorizontalScrollBar.SliderPosition * _Window->State.HorizontalScrollBar.SliderScale,
            -_Window->State.VerticalScrollBar.SliderPosition * _Window->State.VerticalScrollBar.SliderScale);
    }

    // content box
    {
        // self content bounding box
        _Window->State.WindowContentBox  = gs_2dboxf(
            _Window->State.WindowScrollAreaBox.Min,
            _Window->State.WindowScrollAreaBox.Min + _Window->State.WindowContentBox.size(),
            _Window->State.WindowScrollAreaBox.Min + _Window->State.LayoutCursorPositon + _Window->State.LayoutCursorSize);

        // recalculate parental content box
        auto parent = _Window->State.Parent;

        while (parent)
        {
            parent->State.WindowContentBox = gs_2dboxf(
                parent->State.WindowScrollAreaBox.Min,
                parent->State.WindowScrollAreaBox.Min + gs_max(_Window->State.WindowContentBox.size(), parent->State.WindowContentBox.size()),
                parent->State.WindowScrollAreaBox.Min + parent->State.LayoutCursorPositon + parent->State.LayoutCursorSize);

            parent = parent->State.Parent;
        }
    }
    
    // clipping box
    {
        if(_Window->State.Parent != nullptr)
        {
            auto next   = _Window;
            auto parent = _Window->State.Parent;

            while (parent != nullptr)
            {
                next   = parent;
                parent = parent->State.Parent;
            }

            _Window->State.WindowInnerClipAreaBox = next->State.WindowInnerClipAreaBox;
            _Window->State.WindowOuterClipAreaBox = next->State.WindowOuterClipAreaBox;
        }
        else
        {
            gs_vec2f outerBorder =
                gs_vec2f(m_Style->WindowResizeAngleGizmoRadius, m_Style->WindowResizeAngleGizmoRadius);

            _Window->State.WindowInnerClipAreaBox =
                gs_2dboxf(
                    _Window->State.WindowViewportBox.transform(_Window->State.WindowTransform).Min,
                    _Window->State.WindowViewportBox.transform(_Window->State.WindowTransform).Max);

            _Window->State.WindowOuterClipAreaBox =
                gs_2dboxf(
                    _Window->State.WindowBox.transform(_Window->State.WindowTransform).Min - outerBorder,
                    _Window->State.WindowBox.transform(_Window->State.WindowTransform).Max + outerBorder);
        }
    }

    calculate_window_geometry(_Window->State.Parent);
}

int ImmedidateUserInterfaceContextLayer::calculate_child_depth(ImmedidateUserInterfaceWindow* _Window) const
{
    if(_Window == nullptr) return 0;
    return (_Window->State.Depth + (_Window->State.Thickness++));
}

bool ImmedidateUserInterfaceContextLayer::needs_vertical_scroll_bar(const ImmedidateUserInterfaceWindow* _Window) const
{
    if(_Window == nullptr) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceWindowHints_NeverVerticalScrollBar) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceWindowHints_AlwaysVerticalScrollBar) return true;

    return _Window->Cache.WindowContentBox.height() > _Window->Cache.WindowViewportBox.height() &&
            gs_abs(_Window->Cache.WindowViewportBox.height() - _Window->Cache.WindowContentBox.height()) > _Window->Cache.WindowViewportBox.width() * 0.1f;
}

bool ImmedidateUserInterfaceContextLayer::needs_horizontal_scroll_bar(const ImmedidateUserInterfaceWindow* _Window) const
{
    if(_Window == nullptr) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceWindowHints_NeverHorizontalScrollBar) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceWindowHints_AlwaysHorizontalScrollBar) return true;

    return _Window->Cache.WindowContentBox.width() > _Window->Cache.WindowViewportBox.width() &&
            gs_abs(_Window->Cache.WindowViewportBox.width() - _Window->Cache.WindowContentBox.width()) > _Window->Cache.WindowViewportBox.width() * 0.1f;
}

bool ImmedidateUserInterfaceContextLayer::is_being_resized_top_left(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTopLeft);
}

bool ImmedidateUserInterfaceContextLayer::is_being_resized_top_right(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTopRight);
}

bool ImmedidateUserInterfaceContextLayer::is_being_resized_bottom_left(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottomLeft);
}

bool ImmedidateUserInterfaceContextLayer::is_being_resized_bottom_right(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottomRight);
}

bool ImmedidateUserInterfaceContextLayer::is_being_resized_top(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTop);
}

bool ImmedidateUserInterfaceContextLayer::is_being_resized_left(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedLeft);
}

bool ImmedidateUserInterfaceContextLayer::is_being_resized_right(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedRight);
}

bool ImmedidateUserInterfaceContextLayer::is_being_resized_bottom(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottom);
}

bool ImmedidateUserInterfaceContextLayer::is_being_resized(const ImmedidateUserInterfaceWindow* _Window) const
{
    return is_being_resized_top_left(_Window)      ||
            is_being_resized_top_right(_Window)    ||
            is_being_resized_bottom_left(_Window)  ||
            is_being_resized_bottom_right(_Window) ||
            is_being_resized_top(_Window)          ||
            is_being_resized_left(_Window)         ||
            is_being_resized_right(_Window)        ||
            is_being_resized_bottom(_Window);
}

bool ImmedidateUserInterfaceContextLayer::is_being_moved(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMoved);
}

bool ImmedidateUserInterfaceContextLayer::is_being_modified(const ImmedidateUserInterfaceWindow* _Window) const
{
    return is_being_moved(_Window) || is_being_resized(_Window) || is_being_scrolled(_Window);
}

bool ImmedidateUserInterfaceContextLayer::is_being_focused(const ImmedidateUserInterfaceWindow* _Window) const
{
    if (_Window == nullptr) return false;

    auto parent = _Window->State.Parent;
    auto window = _Window;

    while (parent != nullptr)
    {
        window = parent;
        parent = parent->State.Parent;
    }

    return (window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingFocused);
}

bool ImmedidateUserInterfaceContextLayer::is_being_scrolled_vertically(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingScrolledVertically);
}

bool ImmedidateUserInterfaceContextLayer::is_being_scrolled_horizontally(const ImmedidateUserInterfaceWindow* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingScrolledHorizontally);
}

bool ImmedidateUserInterfaceContextLayer::is_being_scrolled(const ImmedidateUserInterfaceWindow* _Window) const
{
    return is_being_scrolled_vertically(_Window) || is_being_scrolled_horizontally(_Window);
}

void ImmedidateUserInterfaceContextLayer::begin_resize_top_left(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;

    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTopLeft;
}

void ImmedidateUserInterfaceContextLayer::begin_resize_top_right(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTopRight;
}

void ImmedidateUserInterfaceContextLayer::begin_resize_bottom_left(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottomLeft;
}

void ImmedidateUserInterfaceContextLayer::begin_resize_bottom_right(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottomRight;
}

void ImmedidateUserInterfaceContextLayer::begin_resize_top(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTop;
}

void ImmedidateUserInterfaceContextLayer::begin_resize_left(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedLeft;
}

void ImmedidateUserInterfaceContextLayer::begin_resize_right(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedRight;
}

void ImmedidateUserInterfaceContextLayer::begin_resize_bottom(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottom;
}

void ImmedidateUserInterfaceContextLayer::begin_move(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMoved;
}

void ImmedidateUserInterfaceContextLayer::begin_focus(ImmedidateUserInterfaceWindow* _Window)
{
    if (_Window == nullptr) return;

    auto parent = _Window->State.Parent;
    auto window = _Window;

    while (parent != nullptr)
    {
        window = parent;
        parent = parent->State.Parent;
    }

    window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingFocused;
}

void ImmedidateUserInterfaceContextLayer::begin_scroll_vertically(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingScrolledVertically;
}

void ImmedidateUserInterfaceContextLayer::begin_scroll_horizontally(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingScrolledHorizontally;
}

void ImmedidateUserInterfaceContextLayer::end_resize(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;

    _Window->State.Changes &= ~(
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTopLeft    |
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTopRight   |
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottomLeft |
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottomRight|
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTop        |
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedLeft       |
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedRight      |
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottom
    );
}

void ImmedidateUserInterfaceContextLayer::end_move(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes &=
        ~ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMoved;
}

void ImmedidateUserInterfaceContextLayer::end_focus(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes &=
        ~ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingFocused;
}

void ImmedidateUserInterfaceContextLayer::end_scroll(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;

    _Window->State.Changes &= ~(
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingScrolledVertically |
        ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingScrolledHorizontally
    );
}

bool ImmedidateUserInterfaceContextLayer::render_window_clipbox(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return false;

    m_Renderer->push_clip_box(
        gs_2dboxf(
            gs_clamp(
                _Window->State.WindowInnerClipAreaBox.Min,
                _Window->State.WindowOuterClipAreaBox.Min,
                _Window->State.WindowOuterClipAreaBox.Max),
            gs_clamp(
                _Window->State.WindowInnerClipAreaBox.Max,
                _Window->State.WindowOuterClipAreaBox.Min,
                _Window->State.WindowOuterClipAreaBox.Max)));

    return true;
}

bool ImmedidateUserInterfaceContextLayer::render_window_background(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return false;

    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowBox.Min,
        _Window->State.WindowBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));

    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowBox.Min,
        _Window->State.WindowBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));

    // draw content gizmo
    m_Renderer->push_rectangle(
        _Window->Cache.WindowContentBox.Min,
        _Window->Cache.WindowContentBox.Max,
        m_Style->FrameWidth,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix(m_Renderer->get_far_plane()));

    // m_Renderer->push_rectangle(
    //     _Window->Cache.WindowScrollAreaBox.Min,
    //     _Window->Cache.WindowScrollAreaBox.Max,
    //     m_Style->FrameWidth,
    //     gs_vec4f(0.f, 255.f, 0.f, 255.f),
    //     _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix(m_Renderer->get_far_plane()));


    return true;
}

bool ImmedidateUserInterfaceContextLayer::render_window_classic_frame(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return true;

    bool closeButtonPressed = false;

    // frame rectangle
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowFrameBox.Min,
        _Window->State.WindowFrameBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));

    // frame border
    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowFrameBox.Min,
        _Window->State.WindowFrameBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));

    // window title
    gs_2dboxf clipbox = gs_2dboxf(
        _Window->State.WindowFrameBox.Min + gs_vec2f(m_Style->FrameWidth, 0.f),
        gs_vec2f(_Window->State.WindowCloseButtonBox.Min.x, _Window->State.WindowCloseButtonBox.Max.y));

    if(!(_Window->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable))
    {
        clipbox = gs_2dboxf(
            _Window->State.WindowFrameBox.Min,
            _Window->State.WindowFrameBox.Max + gs_vec2f(-m_Style->FrameWidth, 0.f));
    }

    bool cipping = _Window->State.WindowOuterClipAreaBox.overlaps(clipbox);
    if(cipping) m_Renderer->push_clip_box(clipbox, _Window->State.WindowTransform);

    m_Renderer->push_text(
        _Window->Name,
        m_Style->FontSize,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextEnabledColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window), _Window->State.WindowTitleBox.Min));

    if(cipping) m_Renderer->pop_clip_box();

    // frame close button
    if(_Window->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable)
    {
        closeButtonPressed = render_close_button_widget(
            _Window->State.WindowCloseButtonBox,
            _Window->State.WindowOuterClipAreaBox,
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
    }

    return closeButtonPressed;
}

bool ImmedidateUserInterfaceContextLayer::render_window_vertical_scrollbar(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return false;

    // draw scroll panel
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowVerticalScrollBarBox.Min,
        _Window->State.WindowVerticalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));

    // draw slider
    if(_Window->State.WindowVerticalScrollBarSliderBox.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.WindowVerticalScrollBarSliderBox.Min,
            _Window->State.WindowVerticalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left) ?
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderPressedColor] :
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderHoveredColor],
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
    }
    else
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.WindowVerticalScrollBarSliderBox.Min,
            _Window->State.WindowVerticalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderColor],
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
    }

    // draw frame
    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowVerticalScrollBarBox.Min,
        _Window->State.WindowVerticalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
    
    return true;
}

bool ImmedidateUserInterfaceContextLayer::render_window_horizontal_scrollbar(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return false;

    // draw scroll panel
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowHorizontalScrollBarBox.Min,
        _Window->State.WindowHorizontalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));

    // draw slider
    if(_Window->State.WindowHorizontalScrollBarSliderBox.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.WindowHorizontalScrollBarSliderBox.Min,
            _Window->State.WindowHorizontalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left) ?
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderPressedColor] :
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderHoveredColor],
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
    }
    else
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.WindowHorizontalScrollBarSliderBox.Min,
            _Window->State.WindowHorizontalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderColor],
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
    }

    // draw frame
    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowHorizontalScrollBarBox.Min,
        _Window->State.WindowHorizontalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
    
    return true;
}

bool ImmedidateUserInterfaceContextLayer::render_window_resize_events_gizmos(ImmedidateUserInterfaceWindow* _Window)
{
    // begin poll events
    gs_2d_ellipsef resizeTopLeft     = gs_2d_ellipsef(_Window->State.WindowBox.Min, m_Style->WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeTopRight    = gs_2d_ellipsef(_Window->State.WindowBox.Min + gs_vec2f(_Window->State.WindowBox.width(), 0.f), m_Style->WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeBottomLeft  = gs_2d_ellipsef(_Window->State.WindowBox.Max - gs_vec2f(_Window->State.WindowBox.width(), 0.f), m_Style->WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeBottomRight = gs_2d_ellipsef(_Window->State.WindowBox.Max, m_Style->WindowResizeAngleGizmoRadius);
    gs_2dboxf      resizeTop         = gs_2dboxf(_Window->State.WindowBox.Min - gs_vec2f(0.f, m_Style->WindowResizeSideGizmoWidth), _Window->State.WindowBox.Min + gs_vec2f(_Window->State.WindowBox.width(), m_Style->WindowResizeSideGizmoWidth));
    gs_2dboxf      resizeLeft        = gs_2dboxf(_Window->State.WindowBox.Min - gs_vec2f(m_Style->WindowResizeSideGizmoWidth, 0.f), _Window->State.WindowBox.Min + gs_vec2f(m_Style->WindowResizeSideGizmoWidth, _Window->State.WindowBox.height()));
    gs_2dboxf      resizeRight       = gs_2dboxf(_Window->State.WindowBox.Min + gs_vec2f(_Window->State.WindowBox.width() - m_Style->WindowResizeSideGizmoWidth, 0.f), _Window->State.WindowBox.Max + gs_vec2f(m_Style->WindowResizeSideGizmoWidth, 0.f));
    gs_2dboxf      resizeBottom      = gs_2dboxf(_Window->State.WindowBox.Min + gs_vec2f(0.f, _Window->State.WindowBox.height() - m_Style->WindowResizeSideGizmoWidth), _Window->State.WindowBox.Max + gs_vec2f(0.f, m_Style->WindowResizeSideGizmoWidth));


    if((_Window->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
    {
        if((resizeTopLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || is_being_resized_top_left(_Window)) && !is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeTopLeft.Center,
                resizeTopLeft.Radius,
                resizeTopLeft.Radius,
                0.f,
                360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
        }
        else if((resizeTopRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || is_being_resized_top_right(_Window)) && !is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeTopRight.Center,
                resizeTopRight.Radius,
                resizeTopRight.Radius,
                0.f,
                360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
        }
        else if((resizeBottomLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || is_being_resized_bottom_left(_Window)) && !is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeBottomLeft.Center,
                resizeBottomLeft.Radius,
                resizeBottomLeft.Radius,
                0.f, 360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
        }
        else if((resizeBottomRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || is_being_resized_bottom_right(_Window)) && !is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeBottomRight.Center,
                resizeBottomRight.Radius,
                resizeBottomRight.Radius,
                0.f,
                360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
        }
        else if((resizeTop.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || is_being_resized_top(_Window)) && !is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeTop.Min,
                resizeTop.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
        }
        else if((resizeLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || is_being_resized_left(_Window)) && !is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeLeft.Min,
                resizeLeft.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
        }
        else if((resizeRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || is_being_resized_right(_Window)) && !is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeRight.Min,
                resizeRight.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
        }
        else if((resizeBottom.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || is_being_resized_bottom(_Window)) && !is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeBottom.Min,
                resizeBottom.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)calculate_child_depth(_Window)));
        }
    }

    return true;
}

void ImmedidateUserInterfaceContextLayer::receive_window_events()
{
    if(m_WindowsDrawList.empty()) return;
    
    ImmedidateUserInterfaceWindow* sink = nullptr;

    for (auto next : m_WindowsDrawList)
    {
        gs_2d_ellipsef resizeTopLeft     = gs_2d_ellipsef(next->State.WindowBox.Min, m_Style->WindowResizeAngleGizmoRadius);
        gs_2d_ellipsef resizeTopRight    = gs_2d_ellipsef(next->State.WindowBox.Min + gs_vec2f(next->State.WindowBox.width(), 0.f), m_Style->WindowResizeAngleGizmoRadius);
        gs_2d_ellipsef resizeBottomLeft  = gs_2d_ellipsef(next->State.WindowBox.Max - gs_vec2f(next->State.WindowBox.width(), 0.f), m_Style->WindowResizeAngleGizmoRadius);
        gs_2d_ellipsef resizeBottomRight = gs_2d_ellipsef(next->State.WindowBox.Max, m_Style->WindowResizeAngleGizmoRadius);
        gs_2dboxf      resizeTop         = gs_2dboxf(next->State.WindowBox.Min - gs_vec2f(0.f, m_Style->WindowResizeSideGizmoWidth), next->State.WindowBox.Min + gs_vec2f(next->State.WindowBox.width(), m_Style->WindowResizeSideGizmoWidth));
        gs_2dboxf      resizeLeft        = gs_2dboxf(next->State.WindowBox.Min - gs_vec2f(m_Style->WindowResizeSideGizmoWidth, 0.f), next->State.WindowBox.Min + gs_vec2f(m_Style->WindowResizeSideGizmoWidth, next->State.WindowBox.height()));
        gs_2dboxf      resizeRight       = gs_2dboxf(next->State.WindowBox.Min + gs_vec2f(next->State.WindowBox.width() - m_Style->WindowResizeSideGizmoWidth, 0.f), next->State.WindowBox.Max + gs_vec2f(m_Style->WindowResizeSideGizmoWidth, 0.f));
        gs_2dboxf      resizeBottom      = gs_2dboxf(next->State.WindowBox.Min + gs_vec2f(0.f, next->State.WindowBox.height() - m_Style->WindowResizeSideGizmoWidth), next->State.WindowBox.Max + gs_vec2f(0.f, m_Style->WindowResizeSideGizmoWidth));

        // poll window resize events
        if( (next->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable) &&
            application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)             &&
            !is_being_scrolled(next)                                                                                &&
            !is_being_resized(next)                                                                                 &&
            !is_being_moved(next))
        {
            if(resizeTopLeft.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                begin_resize_top_left(next);
            else if(resizeTopRight.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                begin_resize_top_right(next);
            else if(resizeBottomLeft.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                begin_resize_bottom_left(next);
            else if(resizeBottomRight.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                begin_resize_bottom_right(next);
            else if(resizeTop.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                begin_resize_top(next);
            else if(resizeLeft.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
                begin_resize_left(next);
            else if(resizeRight.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                begin_resize_right(next);
            else if(resizeBottom.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
                begin_resize_bottom(next);
        }

        // poll window move event
        if( (next->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable)        &&
            application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                  &&
            next->State.WindowFrameBox.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) &&
            !is_being_resized(next)                                                                                      &&
            !is_being_scrolled(next))
        {
            begin_move(next);
        }

        // poll window vertical scroll event
        if( application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                                    &&
            next->State.WindowVerticalScrollBarSliderBox.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) &&
            needs_vertical_scroll_bar(next)                                                                                                &&
            !is_being_resized(next)                                                                                                        &&
            !is_being_moved(next))
        {
            begin_scroll_vertically(next);
        }

        // poll window horizontal scroll event
        if( application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                                      &&
            next->State.WindowHorizontalScrollBarSliderBox.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) &&
            needs_horizontal_scroll_bar(next)                                                                                                &&
            !is_being_resized(next)                                                                                                          &&
            !is_being_moved(next))
        {
            begin_scroll_horizontally(next);
        }

        if (is_being_modified(next))
        {
            if(sink == nullptr)
                sink = next;
            else if(next->State.Depth > sink->State.Depth && sink->State.Parent != next)
                sink = next;
        }
    }

    if(sink != nullptr)
    {
        for (auto& window : m_WindowsDrawList)
        {
            if(window != sink)
            {
                end_move(window);
                end_resize(window);
                end_scroll(window);
            }
        }

        bool allMouseButtonsAreReleased = true;

        for (int button = ApplicationMouseButton::Button::ApplicationMouseButton_Begin;
                    button < ApplicationMouseButton::Button::ApplicationMouseButton_End; button++)
        {
            allMouseButtonsAreReleased =
                allMouseButtonsAreReleased && !application()->is_mouse_button_down((ApplicationMouseButton::Button)button);
        }

        if(allMouseButtonsAreReleased)
        {
            end_move(sink);
            end_resize(sink);
            end_scroll(sink);
        }
    }
}

void ImmedidateUserInterfaceContextLayer::process_window_events(ImmedidateUserInterfaceWindow* _Window)
{
    if(_Window == nullptr) return;

    if(is_being_resized(_Window))
    {
        gs_2dboxf estimatedBox;
        gs_vec2f  cursorDelta = application()->get_window_cursor_dragdelta();

        if(is_being_resized_top_left(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + cursorDelta,
                _Window->Cache.WindowBox.Max);
        }
        else if(is_being_resized_top_right(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Window->Cache.WindowBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
        }
        else if(is_being_resized_bottom_left(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Window->Cache.WindowBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
        }
        else if(is_being_resized_bottom_right(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min,
                _Window->Cache.WindowBox.Max + application()->get_window_cursor_dragdelta());
        }
        else if(is_being_resized_top(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Window->Cache.WindowBox.Max);
        }
        else if(is_being_resized_left(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Window->Cache.WindowBox.Max);
        }
        else if(is_being_resized_right(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min,
                _Window->Cache.WindowBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
        }
        else if(is_being_resized_bottom(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min,
                _Window->Cache.WindowBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
        }

        float minX = estimatedBox.size().x > m_Style->WindowMinimumWidth  ? estimatedBox.Min.x : _Window->Cache.WindowBox.Min.x + (_Window->State.WindowBox.Min.x - _Window->Cache.WindowBox.Min.x);
        float maxX = estimatedBox.size().x > m_Style->WindowMinimumWidth  ? estimatedBox.Max.x : _Window->Cache.WindowBox.Max.x + (_Window->State.WindowBox.Max.x - _Window->Cache.WindowBox.Max.x);
        float minY = estimatedBox.size().y > m_Style->WindowMinimumHeight ? estimatedBox.Min.y : _Window->Cache.WindowBox.Min.y + (_Window->State.WindowBox.Min.y - _Window->Cache.WindowBox.Min.y);
        float maxY = estimatedBox.size().y > m_Style->WindowMinimumHeight ? estimatedBox.Max.y : _Window->Cache.WindowBox.Max.y + (_Window->State.WindowBox.Max.y - _Window->Cache.WindowBox.Max.y);
        
        _Window->State.WindowBox = gs_2dboxf(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));

        if(_Window->State.Parent)
        {
            gs_vec2f fillSize   = (estimatedBox.size() / _Window->Cache.Parent->Cache.WindowBox.size()) * _Window->State.Parent->Cache.LayoutTotalWeight;
            float    fillWeight = 1.f;
            
            if(_Window->State.Parent->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutChildrenHorizontally)
                fillWeight = gs_clamp(fillSize.x, 0.f, _Window->State.Parent->Cache.LayoutTotalWeight);
            else if(_Window->Cache.Parent->State.Settings & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutChildrenVertically)
                fillWeight = gs_clamp(fillSize.y, 0.f, _Window->Cache.Parent->Cache.LayoutTotalWeight);

            float fillWeightDelta = (fillWeight - _Window->State.LayoutFillWeight) / _Window->State.Parent->Cache.LayoutTotalWeight;

            fillWeightDelta *= gs_max((float)(_Window->State.Parent->State.LayoutChildrenCount - _Window->State.LayoutChildIndex), 1.f);

            for (auto child : m_WindowsDrawList)
            {
                if(child->State.Parent != _Window->State.Parent || child == _Window)
                    continue;

                child->State.LayoutFillWeight -= fillWeightDelta;
                child->State.LayoutFillWeight  = gs_clamp(child->State.LayoutFillWeight, 0.1f, _Window->State.Parent->Cache.LayoutTotalWeight);
            }

            _Window->State.LayoutFillWeight += fillWeightDelta;
            _Window->State.LayoutFillWeight  = gs_clamp(_Window->State.LayoutFillWeight, 0.1f, _Window->State.Parent->Cache.LayoutTotalWeight);
        }
    }
    else if(is_being_moved(_Window))
    {
        _Window->State.WindowBox = gs_2dboxf(
            _Window->Cache.WindowBox.Min + application()->get_window_cursor_dragdelta(),
            _Window->Cache.WindowBox.Max + application()->get_window_cursor_dragdelta());

        // dock space
    }
    else if(is_being_scrolled_horizontally(_Window))
    {
        _Window->State.HorizontalScrollBar.reposition(
            _Window->Cache.HorizontalScrollBar.SliderPosition + application()->get_window_cursor_dragdelta().x);
    }
    else if(is_being_scrolled_vertically(_Window))
    {
        _Window->State.VerticalScrollBar.reposition(
            _Window->Cache.VerticalScrollBar.SliderPosition + application()->get_window_cursor_dragdelta().y);
    }
}

gs_2dboxf ImmedidateUserInterfaceContextLayer::calculate_widget_geometry(const gs_vec2f& _Size)
{
    if(m_WindowsHierarchy.empty())
        return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));

    ImmedidateUserInterfaceWindow* window  = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1];
    float padding = 16.f; // TODO: this MUST BE a setting !!!

    gs_vec2f cursorDirection = m_NextCursorDirection.has_value() ? m_NextCursorDirection.value() : gs_vec2f(0.f, 1.f);
    m_NextCursorDirection.reset();

    if(window->State.LayoutCursorDirection != cursorDirection)
    {
        window->State.LayoutCursorDirection = cursorDirection;
        window->State.LayoutCursorPositon   = gs_vec2f(0.f, window->State.WindowContentBox.height());
        window->State.LayoutCursorSize      = gs_vec2f(0.f, 0.f);
    }

    window->State.LayoutCursorPositon += window->State.LayoutCursorSize * window->State.LayoutCursorDirection;

    gs_vec2f min = window->State.LayoutCursorPositon + gs_vec2f(m_Style->FrameWidth, m_Style->FrameWidth);

    window->State.LayoutCursorSize = _Size +  gs_vec2f(padding, padding) * window->State.LayoutCursorDirection;

    calculate_window_geometry(window);

    return gs_2dboxf(min, min + _Size);
}

bool ImmedidateUserInterfaceContextLayer::render_close_button_widget(
    const gs_2dboxf& _ButtonBox,
    const gs_2dboxf& _ClipBox,
    const gs_mat4f&  _Transform)
{
    auto retrieve_close_button_color = [this](const bool& _Pressed, const bool& _Hovered)->gs_vec4f
    {
        if(_Pressed) return m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonPressedColor];
        if(_Hovered) return m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonHoveredColor];
        return m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonDefaultColor];
    };

    bool hovered = _ButtonBox.transform(_Transform).contains(m_Renderer->get_cursor_postion()) && _ClipBox.overlaps(_ButtonBox);
    
    m_Renderer->push_arc_filled(
        _ButtonBox.center(),
        _ButtonBox.width()  * 0.5f,
        _ButtonBox.height() * 0.5f,
        0.f,
        360.f,
        retrieve_close_button_color(hovered && application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left), hovered),
       _Transform);

    m_Renderer->push_line(
        _ButtonBox.Min + gs_vec2f(_ButtonBox.width(), _ButtonBox.height()) * 0.25f,
        _ButtonBox.Max - gs_vec2f(_ButtonBox.width(), _ButtonBox.height()) * 0.25f,
        2.f,
        gs_vec4f(0, 0, 0, 255.f),
        _Transform * m_Renderer->calculate_transform_matrix(1.f));

    m_Renderer->push_line(
        gs_vec2f(_ButtonBox.Max.x, _ButtonBox.Min.y) + gs_vec2f(-_ButtonBox.width(), +_ButtonBox.height()) * 0.25f,
        gs_vec2f(_ButtonBox.Min.x, _ButtonBox.Max.y) + gs_vec2f(+_ButtonBox.width(), -_ButtonBox.height()) * 0.25f,
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
//             m_Style->FontSize,
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
//             radius - m_Style->PushButtonFrameWidth,
//             radius - m_Style->PushButtonFrameWidth,
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
//         m_Style->PushButtonFrameWidth,
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
//         m_Style->FontSize,
//         retreive_text_color(_Enabled, false, false),
//         _Depth + 2.f,
//         gs_vec2f(max.x + m_Style->PushButtonFrameWidth, (max.y + min.y) * 0.5f) + gs_vec2f(0.f, -textBoundingBox.get_size().y * 0.5f),
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
//             m_Style->FontSize,
//             m_Renderer->m_RenderingQueue->get_default_font());

//     gs_2dboxf buttonBoundingBox =
//         gs_2dboxf(
//             gs_vec2f(0.f, 0.f),
//             Immediate2DRenderer::bottom_right(gs_vec2f(+gs_max(_Size.x, textBoundingBox.get_size().x), gs_max(_Size.y, textBoundingBox.get_size().y)) + gs_vec2f(m_Style->FontSize, m_Style->FontSize)));

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
//         m_Style->PushButtonRoundingRadius,
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
//         m_Style->PushButtonRoundingRadius,
//         m_Style->PushButtonFrameWidth,
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
//         m_Style->FontSize,
//         retreive_text_color(_Enabled, hovered, pressed),
//         _Depth + 2.f,
//         (min + max) * 0.5f + Immediate2DRenderer::top_left(gs_vec2f(+textBoundingBox.get_size().x, +textBoundingBox.get_size().y) * 0.5f),
//         _Rotation,
//         _Scale
//     );

//     return pressed && _Enabled;
// }