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

    static bool independentWindowOpened = true;

    if(push_window("Independent window",
        1.f,
        &independentWindowOpened,
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable   |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable  |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable |
        ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutHorizontal))
    {
        pop_window();
    }

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

                if(!drawnWindow->CurrentBox.transform(drawnWindow->Transform).contains(m_Renderer->get_cursor_postion()) ||
                    drawnWindow->Depth <= maxDepth)
                {
                    continue;
                }

                maxDepth      = drawnWindow->Depth;
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
        // layouting
        cachedWindow.second->LayoutFillWeightPreviousSumm = cachedWindow.second->LayoutFillWeightCurrentSumm;
        cachedWindow.second->LayoutFillWeightCurrentSumm  = 0.f;
        cachedWindow.second->LayoutCursorPositon          = gs_vec2f(0.f, 0.f);

        // hierarchy
        cachedWindow.second->Depth         = 0;
        cachedWindow.second->Thickness     = 0;
        cachedWindow.second->Parent        = nullptr;
    }

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

        gs_vec2f position = m_NextWindowPosition.has_value() ? m_NextWindowPosition.value() : gs_vec2f(0.f, 0.f);
        gs_vec2f size     = m_NextWindowSize.has_value() ? m_NextWindowSize.value() : gs_vec2f(512.f, 512.f);
        m_NextWindowPosition.reset();
        m_NextWindowSize.reset();

        window->Name        = _Name;
        window->CurrentBox  = gs_2dboxf(position, position + size);
        window->PreviousBox = window->CurrentBox;
        window->FrameHeight = m_Style.FontSize;
        window->Transform   = gs_mat4f(1.f);

        m_WindowsCache[_Name] = std::move(window);
    }

    ImmedidateUserInterfaceWindow* window = m_WindowsCache.find(_Name)->second.get();
    window->LayoutFillWeight = _Weight;
    window->Hints            = _Hints;

    if(_Opened == nullptr)
        window->Hints &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable;
    else
        window->Hints |= ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable;

    // calculate window geometry
    if(!m_WindowsHierarchy.empty())
    {
        window->Parent                               = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1];
        window->Depth                                = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1]->calculate_child_width();
        window->Parent->LayoutFillWeightCurrentSumm += window->LayoutFillWeight;

        gs_vec2f childSize = window->Parent->ContentBox.size() * window->LayoutFillWeight / window->Parent->LayoutFillWeightPreviousSumm;

        if((window->Parent->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_LayoutHorizontal))
        {
            window->CurrentBox = gs_2dboxf(
                window->Parent->ContentBox.Min,
                window->Parent->ContentBox.Min + gs_vec2f(childSize.x, window->Parent->ContentBox.height()));

            window->Transform = window->Parent->Transform * m_Renderer->calculate_transform_matrix(
                0.f,
                window->Parent->LayoutCursorPositon);

            window->Parent->LayoutCursorPositon += gs_vec2f(childSize.x, 0.f);
        }
        else
        {
            window->CurrentBox = gs_2dboxf(
                window->Parent->ContentBox.Min,
                window->Parent->ContentBox.Min + gs_vec2f(window->Parent->CurrentBox.width(), childSize.y));

            window->Transform = window->Parent->Transform * m_Renderer->calculate_transform_matrix(
                0.f,
                window->Parent->LayoutCursorPositon);

            window->Parent->LayoutCursorPositon += gs_vec2f(0.f, childSize.y);
        }

        window->Hints &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable;
        window->Hints &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable;
        window->Hints &= ~ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable;
    }
    else
    {
        if(window->is_being_focused())
        {
            window->Depth = (int)(m_Renderer->get_far_plane() / 2);
        }
        else
        {
            int windowMaximumDepth = 0;
            for (auto drawnWindow : m_WindowsDrawList)
            {
                if(!drawnWindow->is_being_focused())
                    windowMaximumDepth = gs_max(drawnWindow->Depth + drawnWindow->Thickness, windowMaximumDepth);
            }
            window->Depth = windowMaximumDepth;
        }
    }

    // check parent
    if((window->Parent != nullptr) &&
       (std::find(m_WindowsDrawList.begin(), m_WindowsDrawList.end(), window->Parent) == m_WindowsDrawList.end()))
    {
        m_WindowsHierarchy.push_back(window);
        return false;
    }
    
    // check self
    if(_Opened != nullptr && !(*_Opened))
    {
        m_WindowsHierarchy.push_back(window);
        return false;
    }

    calculate_window_geometry(window);

    // render window
    render_window_background(window);

    //if(window->Parent == nullptr)
    {
        render_window_classic_frame(window, _Opened);
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
    // frame
    _Window->FrameHeight = m_Style.FontSize;
    _Window->FrameBox    = gs_2dboxf(_Window->CurrentBox.Min, _Window->CurrentBox.Min + gs_vec2f(_Window->CurrentBox.width(), _Window->FrameHeight));
    
    // content
    _Window->ContentBox  = gs_2dboxf(_Window->CurrentBox.Min + gs_vec2f(0.f, _Window->FrameHeight), _Window->CurrentBox.Max);

    // title
    auto titleSize = m_Renderer->calculate_bounding_box(_Window->Name, m_Style.FontSize, m_Style.Font).size();
    auto titleMin  = gs_vec2f(_Window->FrameBox.Min.x + m_Style.FrameWidth, _Window->FrameBox.center().y - titleSize.y * 0.5f);
    auto titleMax  = titleMin + titleSize;

    _Window->TitleBox = gs_2dboxf(titleMin, titleMax);

    // close button
    auto closeButtonMin = gs_vec2f(_Window->FrameBox.Max.x - m_Style.WindowResizeAngleGizmoRadius - titleSize.y, _Window->TitleBox.Min.y);
    auto closeButtonMax = closeButtonMin + gs_vec2f(titleSize.y, titleSize.y);
    _Window->CloseButtonBox = gs_2dboxf(closeButtonMin, closeButtonMax);
}

void Immedidate2DRendererTestLayer::render_window_background(ImmedidateUserInterfaceWindow* _Window)
{
    m_Renderer->push_rectangle_rounded_filled(
        _Window->CurrentBox.Min,
        _Window->CurrentBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceColor],
        _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width())
    );

    m_Renderer->push_rectangle_rounded(
        _Window->CurrentBox.Min,
        _Window->CurrentBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.FrameWidth,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width())
    );

    // // draw content gizmo
    // m_Renderer->push_rectangle(
    //     _Window->ContentBox.Min,
    //     _Window->ContentBox.Max,
    //     m_Style.FrameWidth,
    //     gs_vec4f(255.f),
    //     _Window->Transform * m_Renderer->calculate_transform_matrix(m_Renderer->get_far_plane()));
}

void Immedidate2DRendererTestLayer::render_window_classic_frame(ImmedidateUserInterfaceWindow* _Window, bool* _Opened)
{
    // frame rectangle
    m_Renderer->push_rectangle_rounded_filled(
        _Window->FrameBox.Min,
        _Window->FrameBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameColor],
        _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));

    // frame border
    m_Renderer->push_rectangle_rounded(
        _Window->FrameBox.Min,
        _Window->FrameBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.FrameWidth,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameFrameColor],
        _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));

    // window title
    m_Renderer->push_clip_box(
        gs_2dboxf(_Window->FrameBox.Min + gs_vec2f(m_Style.FrameWidth, 0.f), _Window->FrameBox.Max - gs_vec2f(m_Style.FrameWidth, 0.f)),
        _Window->Transform);

    m_Renderer->push_text(
        _Window->Name,
        m_Style.FontSize,
        gs_vec4f(80.f, 0.f, 0.f, 255.f),
        _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width(), _Window->TitleBox.Min));

    // frame close button
    if(_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Closable && _Opened != nullptr)
    {
        *_Opened = !render_close_button_widget(
            _Window->CloseButtonBox.Min,
            _Window->CloseButtonBox.Max,
            _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));
    }

    m_Renderer->pop_clip_box();
}

void Immedidate2DRendererTestLayer::sink_window_events(ImmedidateUserInterfaceWindow* _Window)
{
    // begin poll events
    gs_2d_ellipsef resizeTopLeft     = gs_2d_ellipsef(_Window->CurrentBox.Min, m_Style.WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeTopRight    = gs_2d_ellipsef(_Window->CurrentBox.Min + gs_vec2f(_Window->CurrentBox.width(), 0.f), m_Style.WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeBottomLeft  = gs_2d_ellipsef(_Window->CurrentBox.Max - gs_vec2f(_Window->CurrentBox.width(), 0.f), m_Style.WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeBottomRight = gs_2d_ellipsef(_Window->CurrentBox.Max, m_Style.WindowResizeAngleGizmoRadius);
    gs_2dboxf      resizeTop         = gs_2dboxf(_Window->CurrentBox.Min - gs_vec2f(0.f, m_Style.WindowResizeSideGizmoWidth), _Window->CurrentBox.Min + gs_vec2f(_Window->CurrentBox.width(), m_Style.WindowResizeSideGizmoWidth));
    gs_2dboxf      resizeLeft        = gs_2dboxf(_Window->CurrentBox.Min - gs_vec2f(m_Style.WindowResizeSideGizmoWidth, 0.f), _Window->CurrentBox.Min + gs_vec2f(m_Style.WindowResizeSideGizmoWidth, _Window->CurrentBox.height()));
    gs_2dboxf      resizeRight       = gs_2dboxf(_Window->CurrentBox.Min + gs_vec2f(_Window->CurrentBox.width() - m_Style.WindowResizeSideGizmoWidth, 0.f), _Window->CurrentBox.Max + gs_vec2f(m_Style.WindowResizeSideGizmoWidth, 0.f));
    gs_2dboxf      resizeBottom      = gs_2dboxf(_Window->CurrentBox.Min + gs_vec2f(0.f, _Window->CurrentBox.height() - m_Style.WindowResizeSideGizmoWidth), _Window->CurrentBox.Max + gs_vec2f(0.f, m_Style.WindowResizeSideGizmoWidth));

    if(resizeTopLeft.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_top_left();
    }
    else if(resizeTopRight.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_top_right();
    }
    else if(resizeBottomLeft.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_bottom_left();
    }
    else if(resizeBottomRight.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_bottom_right();
    }
    else if(resizeTop.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_top();
    }
    else if(resizeLeft.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_left();
    }
    else if(resizeRight.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_right();
    }
    else if(resizeBottom.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_bottom();
    }
    else if(_Window->FrameBox.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()) && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left))
            _Window->being_move();
    }

    // end poll events
    bool anyWindowIsBeingModified   = false;
    bool allMouseButtonsAreReleased = true;

    for (auto& cachedwindow : m_WindowsCache)
    {
        if (cachedwindow.second->Name == _Window->Name) continue;

        if(cachedwindow.second->is_being_modified()) anyWindowIsBeingModified = true;
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
    }

    // process events
    if(!anyWindowIsBeingModified && (_Window->Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
    {
        if((resizeTopLeft.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_top_left()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeTopLeft.Center,
                resizeTopLeft.Radius,
                resizeTopLeft.Radius,
                0.f,
                360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));
        }
        else if((resizeTopRight.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_top_right()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeTopRight.Center,
                resizeTopRight.Radius,
                resizeTopRight.Radius,
                0.f,
                360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));
        }
        else if((resizeBottomLeft.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_bottom_left()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeBottomLeft.Center,
                resizeBottomLeft.Radius,
                resizeBottomLeft.Radius,
                0.f, 360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));
        }
        else if((resizeBottomRight.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_bottom_right()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeBottomRight.Center,
                resizeBottomRight.Radius,
                resizeBottomRight.Radius,
                0.f,
                360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));
        }
        else if((resizeTop.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_top()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeTop.Min,
                resizeTop.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));
        }
        else if((resizeLeft.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_left()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeLeft.Min,
                resizeLeft.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));
        }
        else if((resizeRight.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_right()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeRight.Min,
                resizeRight.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));
        }
        else if((resizeBottom.transform(_Window->Transform).contains(m_Renderer->get_cursor_postion()) || _Window->is_being_resized_bottom()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeBottom.Min,
                resizeBottom.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->Transform * m_Renderer->calculate_transform_matrix((float)_Window->calculate_child_width()));
        }
    }

    if(_Window->is_being_resized())
    {
        gs_2dboxf estimatedBox;
        gs_vec2f  cursorDelta = application()->get_window_cursor_dragdelta();

        if(_Window->is_being_resized_top_left())
        {
            estimatedBox = gs_2dboxf(_Window->PreviousBox.Min + cursorDelta, _Window->PreviousBox.Max);
        }
        else if(_Window->is_being_resized_top_right())
        {
            estimatedBox = gs_2dboxf(
                _Window->PreviousBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Window->PreviousBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
        }
        else if(_Window->is_being_resized_bottom_left())
        {
            estimatedBox = gs_2dboxf(
                _Window->PreviousBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Window->PreviousBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
        }
        else if(_Window->is_being_resized_bottom_right())
        {
            estimatedBox = gs_2dboxf(
                _Window->PreviousBox.Min,
                _Window->PreviousBox.Max + application()->get_window_cursor_dragdelta());
        }
        else if(_Window->is_being_resized_top())
        {
            estimatedBox = gs_2dboxf(
                _Window->PreviousBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Window->PreviousBox.Max);
        }
        else if(_Window->is_being_resized_left())
        {
            estimatedBox = gs_2dboxf(
                _Window->PreviousBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Window->PreviousBox.Max);
        }
        else if(_Window->is_being_resized_right())
        {
            estimatedBox = gs_2dboxf(
                _Window->PreviousBox.Min,
                _Window->PreviousBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
        }
        else if(_Window->is_being_resized_bottom())
        {
            estimatedBox = gs_2dboxf(
                _Window->PreviousBox.Min,
                _Window->PreviousBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
        }

        float minX = estimatedBox.size().x > m_Style.WindowMinimumWidth ? estimatedBox.Min.x : _Window->PreviousBox.Min.x + (_Window->CurrentBox.Min.x - _Window->PreviousBox.Min.x);
        float maxX = estimatedBox.size().x > m_Style.WindowMinimumWidth ? estimatedBox.Max.x : _Window->PreviousBox.Max.x + (_Window->CurrentBox.Max.x - _Window->PreviousBox.Max.x);
        float minY = estimatedBox.size().y > m_Style.WindowMinimumHeight ? estimatedBox.Min.y : _Window->PreviousBox.Min.y + (_Window->CurrentBox.Min.y - _Window->PreviousBox.Min.y);
        float maxY = estimatedBox.size().y > m_Style.WindowMinimumHeight ? estimatedBox.Max.y : _Window->PreviousBox.Max.y + (_Window->CurrentBox.Max.y - _Window->PreviousBox.Max.y);
        
        _Window->CurrentBox = gs_2dboxf(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));
    }
    else if(_Window->is_being_moved())
    {
        _Window->CurrentBox = gs_2dboxf(
            _Window->PreviousBox.Min + application()->get_window_cursor_dragdelta(),
            _Window->PreviousBox.Max + application()->get_window_cursor_dragdelta());

        // dock space
    }
    else
    {
        _Window->PreviousBox = _Window->CurrentBox;
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
        (_Min + _Max) * 0.5f,
        12.f,
        12.f,
        0.f,
        360.f,
        retrieve_close_button_color(hovered && application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left), hovered),
       _Transform);

    m_Renderer->push_line(
        _Min,
        _Max,
        2.f,
        gs_vec4f(0, 0, 0, 255.f),
        _Transform * m_Renderer->calculate_transform_matrix(1.f));

    m_Renderer->push_line(
        gs_vec2f(_Max.x, _Min.y),
        gs_vec2f(_Min.x, _Max.y),
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