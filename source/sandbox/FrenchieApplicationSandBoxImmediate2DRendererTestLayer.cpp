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

    push_window("Beta window");
    push_window("Alpha window", ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable);
    pop_window();
    pop_window();

    set_next_window_position(gs_vec2f(512.f, 512.f));
    push_window("Theta window");
    pop_window();

    set_next_window_position(gs_vec2f(1024.f, 1024.f));
    push_window("Cappa window");
    pop_window();

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

    // clean up windows hierarchy data
    for (auto& cachedWindow : m_WindowsCache)
    {
        cachedWindow.second->Depth      = 0;
        cachedWindow.second->Thickness  = 0;
        cachedWindow.second->ChildIndex = 0;
        cachedWindow.second->Parent     = nullptr;
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
    const std::string&                        _Name,
    const ImmedidateUserInterfaceWindowHints& _Hints)
{
    ImmedidateUserInterfaceWindow* window = create_window(_Name);

    render_window_background(window, _Hints);
    render_window_frame(window, _Hints);
    render_window_title(window, _Hints);
    sink_window_events(window, _Hints);

    m_WindowsDrawList.push_back(window);
    m_WindowsHierarchy.push_back(window);

    return true;
}

void Immedidate2DRendererTestLayer::pop_window()
{
    if(!m_WindowsHierarchy.empty())
        m_WindowsHierarchy.pop_back();
}

ImmedidateUserInterfaceWindow* Immedidate2DRendererTestLayer::create_window(const std::string& _Name)
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

        m_WindowsCache[_Name] = std::move(window);
    }

    auto& window = m_WindowsCache[_Name];

    // calculate window transform
    if(!m_WindowsHierarchy.empty())
    {
        auto hierarchyInfo = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1]->push_child();
        window->Parent     = m_WindowsHierarchy[m_WindowsHierarchy.size() - 1];
        window->ChildIndex = hierarchyInfo.ChildIndex;
        window->Depth      = hierarchyInfo.Depth;
        window->Transform  = m_Renderer->calculate_transform_matrix(0.f, window->Parent->CurrentBox.Min + gs_vec2f(0.f, m_Style.FontSize));
    }
    else
    {
        // calculate window depth
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

    return m_WindowsCache[_Name].get();
}

gs_2dboxf Immedidate2DRendererTestLayer::calculate_window_bounding_box(ImmedidateUserInterfaceWindow* _Window)
{
    return gs_2dboxf(gs_vec2f(0.f, 0.f), _Window->CurrentBox.size()).transform(_Window->Transform);
}

gs_2dboxf Immedidate2DRendererTestLayer::calculate_window_frame_bounding_box(ImmedidateUserInterfaceWindow* _Window)
{
    return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(_Window->CurrentBox.width(), m_Style.FontSize)).transform(_Window->Transform);
}

void Immedidate2DRendererTestLayer::render_window_background(ImmedidateUserInterfaceWindow* _Window, const ImmedidateUserInterfaceWindowHints&)
{
    gs_2dboxf windowBoundingBox       = calculate_window_bounding_box(_Window);
    gs_2dboxf windowFrameBoundingBox  = calculate_window_frame_bounding_box(_Window);

    m_Renderer->push_rectangle_rounded_filled(
        windowBoundingBox.Min,
        windowBoundingBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceColor],
        m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));

    m_Renderer->push_rectangle_rounded(
        windowBoundingBox.Min,
        windowBoundingBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.FrameWidth,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));
}

void Immedidate2DRendererTestLayer::render_window_frame(ImmedidateUserInterfaceWindow* _Window, const ImmedidateUserInterfaceWindowHints&)
{
    gs_2dboxf windowBoundingBox       = calculate_window_bounding_box(_Window);
    gs_2dboxf windowFrameBoundingBox  = calculate_window_frame_bounding_box(_Window);

    // frame rectangle
    m_Renderer->push_rectangle_rounded_filled(
        windowFrameBoundingBox.Min,
        windowFrameBoundingBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameColor],
        m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));

    m_Renderer->push_rectangle_rounded(
        windowFrameBoundingBox.Min,
        windowFrameBoundingBox.Max,
        m_Style.FrameRoundingRadius,
        m_Style.FrameWidth,
        m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameFrameColor],
        m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));

    // frame close button
    auto windowFrameBoundingBoxTransformed = windowFrameBoundingBox.transform(m_Renderer->calculate_transform_matrix(0.f, _Window->CurrentBox.Min));
    auto windowFrameCloseButtonSize        = gs_min(m_Style.WindowFrameCloseButtonSize, (windowFrameBoundingBoxTransformed.size().y - m_Style.FrameWidth * 2.f) * 0.9f);

    gs_2dboxf windowFrameCloseButtonBoundingBox =
        gs_2dboxf(
            gs_vec2f(0.f, 0.f),
            gs_vec2f(windowFrameCloseButtonSize));

    m_Renderer->push_rectangle_filled(
        windowFrameCloseButtonBoundingBox.Min,
        windowFrameCloseButtonBoundingBox.Max,
        gs_vec4f(80.f, 0.f, 0.f, 255.f),
        m_Renderer->calculate_transform_matrix(
            (float)_Window->push_child().Depth,
            gs_vec2f(windowFrameBoundingBoxTransformed.Max.x - windowFrameCloseButtonSize - m_Style.WindowResizeAngleGizmoRadius,
                        windowFrameBoundingBoxTransformed.center().y - windowFrameCloseButtonSize * 0.5f)));
}

void Immedidate2DRendererTestLayer::render_window_title(ImmedidateUserInterfaceWindow*, const ImmedidateUserInterfaceWindowHints&)
{
}

void Immedidate2DRendererTestLayer::sink_window_events(ImmedidateUserInterfaceWindow* _Window, const ImmedidateUserInterfaceWindowHints& _Hints)
{
    gs_2dboxf windowBoundingBox       = calculate_window_bounding_box(_Window);
    gs_2dboxf windowFrameBoundingBox  = calculate_window_frame_bounding_box(_Window);

    // begin poll events
    gs_2d_ellipsef resizeTopLeft     = gs_2d_ellipsef(windowBoundingBox.Min, m_Style.WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeTopRight    = gs_2d_ellipsef(windowBoundingBox.Min + gs_vec2f(_Window->CurrentBox.width(), 0.f), m_Style.WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeBottomLeft  = gs_2d_ellipsef(windowBoundingBox.Max - gs_vec2f(_Window->CurrentBox.width(), 0.f), m_Style.WindowResizeAngleGizmoRadius);
    gs_2d_ellipsef resizeBottomRight = gs_2d_ellipsef(windowBoundingBox.Max, m_Style.WindowResizeAngleGizmoRadius);
    gs_2dboxf      resizeTop         = gs_2dboxf(windowBoundingBox.Min - gs_vec2f(0.f, m_Style.WindowResizeSideGizmoWidth), windowBoundingBox.Min + gs_vec2f(_Window->CurrentBox.width(), m_Style.WindowResizeSideGizmoWidth));
    gs_2dboxf      resizeLeft        = gs_2dboxf(windowBoundingBox.Min - gs_vec2f(m_Style.WindowResizeSideGizmoWidth, 0.f), windowBoundingBox.Min + gs_vec2f(m_Style.WindowResizeSideGizmoWidth, _Window->CurrentBox.height()));
    gs_2dboxf      resizeRight       = gs_2dboxf(windowBoundingBox.Min + gs_vec2f(_Window->CurrentBox.width() - m_Style.WindowResizeSideGizmoWidth, 0.f), windowBoundingBox.Max + gs_vec2f(m_Style.WindowResizeSideGizmoWidth, 0.f));
    gs_2dboxf      resizeBottom      = gs_2dboxf(windowBoundingBox.Min + gs_vec2f(0.f, _Window->CurrentBox.height() - m_Style.WindowResizeSideGizmoWidth), windowBoundingBox.Max + gs_vec2f(0.f, m_Style.WindowResizeSideGizmoWidth));

    gs_mat4f  wnidowTransformMatrix   = m_Renderer->calculate_transform_matrix(0.f, _Window->CurrentBox.Min);
    gs_vec2f  wnidowCursorPosition    = m_Renderer->get_cursor_postion();

    if(resizeTopLeft.transform(wnidowTransformMatrix).contains(wnidowCursorPosition))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_top_left();
    }
    else if(resizeTopRight.transform(wnidowTransformMatrix).contains(wnidowCursorPosition))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_top_right();
    }
    else if(resizeBottomLeft.transform(wnidowTransformMatrix).contains(wnidowCursorPosition))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_bottom_left();
    }
    else if(resizeBottomRight.transform(wnidowTransformMatrix).contains(wnidowCursorPosition))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_bottom_right();
    }
    else if(resizeTop.transform(wnidowTransformMatrix).contains(wnidowCursorPosition))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_top();
    }
    else if(resizeLeft.transform(wnidowTransformMatrix).contains(wnidowCursorPosition))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_left();
    }
    else if(resizeRight.transform(wnidowTransformMatrix).contains(wnidowCursorPosition))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_right();
    }
    else if(resizeBottom.transform(wnidowTransformMatrix).contains(wnidowCursorPosition))
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::ApplicationMouseButton_Left) && !_Window->is_being_resized() && !_Window->is_being_moved() && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
            _Window->begin_resize_bottom();
    }
    else if(windowFrameBoundingBox.transform(wnidowTransformMatrix).contains(wnidowCursorPosition) && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Movable))
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

    // // process events
    if(!anyWindowIsBeingModified && (_Hints & ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Resizable))
    {
        if((resizeTopLeft.transform(wnidowTransformMatrix).contains(wnidowCursorPosition) || _Window->is_being_resized_top_left()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeTopLeft.Center,
                resizeTopLeft.Radius,
                resizeTopLeft.Radius,
                0.f,
                360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));
        }
        else if((resizeTopRight.transform(wnidowTransformMatrix).contains(wnidowCursorPosition) || _Window->is_being_resized_top_right()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeTopRight.Center,
                resizeTopRight.Radius,
                resizeTopRight.Radius,
                0.f,
                360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));
        }
        else if((resizeBottomLeft.transform(wnidowTransformMatrix).contains(wnidowCursorPosition) || _Window->is_being_resized_bottom_left()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeBottomLeft.Center,
                resizeBottomLeft.Radius,
                resizeBottomLeft.Radius,
                0.f, 360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));
        }
        else if((resizeBottomRight.transform(wnidowTransformMatrix).contains(wnidowCursorPosition) || _Window->is_being_resized_bottom_right()) && !_Window->is_being_moved())
        {
            m_Renderer->push_arc_filled(
                resizeBottomRight.Center,
                resizeBottomRight.Radius,
                resizeBottomRight.Radius,
                0.f,
                360.f,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));
        }
        else if((resizeTop.transform(wnidowTransformMatrix).contains(wnidowCursorPosition) || _Window->is_being_resized_top()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeTop.Min,
                resizeTop.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));
        }
        else if((resizeLeft.transform(wnidowTransformMatrix).contains(wnidowCursorPosition) || _Window->is_being_resized_left()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeLeft.Min,
                resizeLeft.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));
        }
        else if((resizeRight.transform(wnidowTransformMatrix).contains(wnidowCursorPosition) || _Window->is_being_resized_right()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeRight.Min,
                resizeRight.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));
        }
        else if((resizeBottom.transform(wnidowTransformMatrix).contains(wnidowCursorPosition) || _Window->is_being_resized_bottom()) && !_Window->is_being_moved())
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeBottom.Min,
                resizeBottom.Max,
                m_Style.FrameRoundingRadius,
                m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                m_Renderer->calculate_transform_matrix((float)_Window->push_child().Depth, _Window->CurrentBox.Min));
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
            _Window->PreviousBox.Min + application()->get_window_cursor_dragdelta() + _Window->PreviousBox.size());

        // dock space
    }
    else
    {
        _Window->PreviousBox = _Window->CurrentBox;
    }
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