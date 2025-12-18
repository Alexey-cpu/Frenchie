#include <ImmedidateUserInterfaceLayer.hpp>

using namespace Frenchie::Application;

// Immedidate2DRendererTestLayer
ImmedidateUserInterfaceContextLayer::ImmedidateUserInterfaceContextLayer(){}
ImmedidateUserInterfaceContextLayer::~ImmedidateUserInterfaceContextLayer(){}

auto ImmedidateUserInterfaceContextLayer::widget_prepare_for_rendering(const gs_vec2f& _Size)
{
    struct
    {
        gs_2dboxf                    BoundingBox;
        gs_2dboxf                    ClippingBox;
        ImmedidateUserInterfaceNode* Node;
    } WidgetData = 
    {
        gs_2dboxf(gs_vec2f(0.f, 0.f), _Size),
        gs_2dboxf(gs_vec2f(0.f, 0.f), _Size),
        nullptr
    };

    if(node_hierarchy_is_empty())
    {
        WidgetData.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), _Size);
        WidgetData.ClippingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), _Size);
        WidgetData.Node        = nullptr;
        return WidgetData; // TODO: do we have to create default window or let's draw right onto context window ???
    }

    ImmedidateUserInterfaceNode* window  = node_hierarchy_top();
    float padding = 16.f; // TODO: this MUST BE a setting !!!

    // move cursor
    gs_vec2f cursorDirection = m_NextNodeCursorDirection.has_value() ? m_NextNodeCursorDirection.value() : node_vertical_cursor_direction();
    m_NextNodeCursorDirection.reset();

    // go next line if cursor direction changes to vertical
    if(cursorDirection == node_vertical_cursor_direction())
    {
        window->State.LayoutCursorPositon = gs_vec2f(0.f, window->State.WindowContentBox.height());
        window->State.LayoutCursorSize    = gs_vec2f(0.f, 0.f);
    }

    window->State.LayoutCursorDirection = cursorDirection;
    window->State.LayoutCursorPositon  += window->State.LayoutCursorSize * window->State.LayoutCursorDirection;

    gs_vec2f min = window->State.LayoutCursorPositon + gs_vec2f(m_Style->FrameWidth, m_Style->FrameWidth);

    window->State.LayoutCursorSize =
        _Size + gs_vec2f(m_Style->FrameWidth, m_Style->FrameWidth) * 2.f +
                gs_vec2f(padding, padding) * window->State.LayoutCursorDirection;

    node_calculate_geometry(window);

    auto geometryBox = gs_2dboxf(min, min + _Size);
    WidgetData.BoundingBox = gs_2dboxf(geometryBox.Min + window->State.WindowScrollAreaBox.Min, geometryBox.Max + window->State.WindowScrollAreaBox.Min);
    WidgetData.ClippingBox = window->State.WindowInnerClipAreaBox.clip_with(window->State.WindowOuterClipAreaBox);
    WidgetData.Node        = window;

    // receive events
    m_WidgetMouseHovered.reset();
    m_WidgetMouseDown.reset();
    m_WidgetMouseClicked.reset();
    m_WidgetMouseDoubleClicked.reset();

    m_WidgetMouseHovered = WidgetData.BoundingBox.transform(window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) &&
                           WidgetData.ClippingBox.overlaps(WidgetData.BoundingBox.transform(window->State.WindowTransform));

    if(m_WidgetMouseHovered.value())
    {
        if(application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
            m_WidgetMouseDown = ApplicationMouseButton::Button::ApplicationMouseButton_Left;
        else if(application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Right))
            m_WidgetMouseDown = ApplicationMouseButton::Button::ApplicationMouseButton_Right;
        else if(application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
            m_WidgetMouseDown = ApplicationMouseButton::Button::ApplicationMouseButton_Middle;

        if(application()->is_mouse_button_pressed(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
            m_WidgetMousePressed = ApplicationMouseButton::Button::ApplicationMouseButton_Left;
        else if(application()->is_mouse_button_pressed(ApplicationMouseButton::Button::ApplicationMouseButton_Right))
            m_WidgetMousePressed = ApplicationMouseButton::Button::ApplicationMouseButton_Right;
        else if(application()->is_mouse_button_pressed(ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
            m_WidgetMousePressed = ApplicationMouseButton::Button::ApplicationMouseButton_Middle;

        if(application()->is_mouse_button_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
            m_WidgetMouseClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Left;
        else if(application()->is_mouse_button_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Right))
            m_WidgetMouseClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Right;
        else if(application()->is_mouse_button_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
            m_WidgetMouseClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Middle;

        if(application()->is_mouse_button_double_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
            m_WidgetMouseDoubleClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Left;
        else if(application()->is_mouse_button_double_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Right))
            m_WidgetMouseDoubleClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Right;
        else if(application()->is_mouse_button_double_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
            m_WidgetMouseDoubleClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Middle;
    }

    return WidgetData;
}

auto ImmedidateUserInterfaceContextLayer::widget_prepare_for_rendering(const std::string& _Text)
{
    return widget_prepare_for_rendering(m_Renderer->calculate_bounding_box(_Text, m_Style->FontSize, m_Style->Font).size() + gs_vec2f(m_Style->FontSize * 0.5f));
}

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

    // static bool bettaWindowOpened = true;

    // if(begin_node("Beta window",
    //     &bettaWindowOpened,
    //     ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable   |
    //     ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Resizable |
    //     ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally
    // ))
    // {
    //     for (int i = 0; i < 5; i++)
    //     {
    //         for (int j = 0; j < 5; j++)
    //         {
    //             default_button_widget("BUTTON");
    //             same_line();
    //             default_button_widget("BUTTON");
    //         }
    //     }

    //     if(begin_node("Container window",
    //         nullptr,
    //         ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable   |
    //         ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Resizable |
    //         ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally))
    //     {
    //         if(begin_node("Theta window",
    //             nullptr, 
    //             ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable   |
    //             ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Resizable |
    //             ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally))
    //         {
    //             if(begin_node("Theta-1 window")) end_node();
    //             if(begin_node("Theta-2 window")) end_node();

    //             end_node();
    //         }

    //         end_node();
    //     }

    //     end_node();
    // }

    // static bool independentWindowOpened = true;

    // if(begin_node("Independent window",
    //     &independentWindowOpened,
    //     ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable   |
    //     ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Closable  |
    //     ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Resizable | ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents
    // ))
    // {
    //     for (int i = 0; i < 10; i++)default_button_widget("BUTTON");
        
    //     end_node()();
    // }

    if(begin_window(
        "Window-1",
        nullptr,
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable   |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Resizable |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally))
    {
        if(begin_window("Window-2"))
        {
            for (int j = 0; j < 5; j++)
            {
                widget_push_button("BUTTON");
                same_line();
                widget_push_button("BUTTON");
            }

            end_window();
        }

        if(begin_window("Window-3"))
        {
            for (int j = 0; j < 5; j++)
            {
                widget_push_button("BUTTON");
                same_line();
                widget_push_button("BUTTON");
            }

            end_window();
        }

        end_window();
    }

    // if(begin_menu("MENU"))
    // {
    //     if(begin_menu("SUBMENU-0"))
    //     {
    //         // menu_item("Action-5");
    //         // menu_item("Action-6");
    //         // menu_item("Action-7");
    //         // menu_item("Action-8");

    //         if(begin_menu("SUBMENU-1"))
    //         {
    //             widget_menu_button("Action-5");
    //             widget_menu_button("Action-6");
    //             widget_menu_button("Action-7");
    //             widget_menu_button("Action-8");

    //             if(begin_menu("SUBMENU-2"))
    //             {
    //                 widget_menu_button("Action-5");
    //                 widget_menu_button("Action-6");
    //                 widget_menu_button("Action-7");
    //                 widget_menu_button("Action-8");

    //                 // if(begin_menu("SUBMENU-10"))
    //                 // {
    //                 //     menu_item("Action-5");
    //                 //     menu_item("Action-6");
    //                 //     menu_item("Action-7");
    //                 //     menu_item("Action-8");
    //                 //     end_menu();
    //                 // }

    //                 end_menu();
    //             }

    //             end_menu();
    //         }

    //         end_menu();
    //     }

    //     // menu_item("Action-1");
    //     // menu_item("Action-2");
    //     // menu_item("Action-3");

    //     if(begin_menu("SUBMENU-3"))
    //     {
    //         widget_menu_button("Action-5");
    //         widget_menu_button("Action-6");
    //         widget_menu_button("Action-7");
    //         widget_menu_button("Action-8");

    //         if(begin_menu("SUBMENU-4"))
    //         {
    //             widget_menu_button("Action-5");
    //             widget_menu_button("Action-6");
    //             widget_menu_button("Action-7");
    //             widget_menu_button("Action-8");
    //             end_menu();
    //         }
        
    //         end_menu();
    //     }

    //     widget_menu_button("Action-4");

    //     end_menu();
    // }

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
    // poll windows events
    node_receive_events();

    // focus the top most clicked node
    for (int button = ApplicationMouseButton::ApplicationMouseButton_Begin;
             button < ApplicationMouseButton::ApplicationMouseButton_End;
             button++)
    {
        if(application()->is_mouse_button_pressed((ApplicationMouseButton::Button)button))
        {
            ImmedidateUserInterfaceNode* topMostWindow = nullptr;
            int                          maxDepth      = -1;
            for (auto drawnWindow : m_NodesDrawList)
            {
                node_end_focus(drawnWindow);

                if(!drawnWindow->State.WindowBox.transform(drawnWindow->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) ||
                    drawnWindow->State.Depth <= maxDepth)
                {
                    continue;
                }

                maxDepth      = drawnWindow->State.Depth;
                topMostWindow = drawnWindow;
            }

            if(topMostWindow != nullptr)
                node_begin_focus(topMostWindow);

            break;
        }
    }

    // focus any modified or moved node
    for (int i = 0; i < (int)m_NodesDrawList.size(); i++)
    {
        if(node_is_being_modified(m_NodesDrawList[i]) || node_is_being_moved(m_NodesDrawList[i]))
        {
            for (int j = 0; j < (int)m_NodesDrawList.size(); j++)
                node_end_focus(m_NodesDrawList[j]);
            node_begin_focus(m_NodesDrawList[i]);
            break;
        }
    }

    // clean up nodes hierarchy and layout data
    for (auto& cachedWindow : m_NodesDrawList)
    {
        // save state
        if(!node_is_being_modified(cachedWindow))
            cachedWindow->Cache = cachedWindow->State;

        // layout
        cachedWindow->State.LayoutTotalWeight    = 0.f;
        cachedWindow->State.LayoutCursorPositon  = gs_vec2f(0.f, 0.f);
        cachedWindow->State.LayoutCursorSize     = gs_vec2f(0.f, 0.f);
        cachedWindow->State.WindowScrollAreaBox  = cachedWindow->State.WindowViewportBox;
        cachedWindow->State.WindowContentBox     = gs_2dboxf(cachedWindow->State.WindowViewportBox.Min, cachedWindow->State.WindowViewportBox.Min);

        // hierarchy
        cachedWindow->State.Depth      = 0;
        cachedWindow->State.Parent     = nullptr;
        cachedWindow->State.Thickness  = 0;
        cachedWindow->State.FirstChild = 0;
        cachedWindow->State.LastChild  = 0;
    }

    // clear hierarchy and draw lists
    GS_ASSERT(m_NodesHierarchy.empty());
    m_NodesDrawList.clear();
    m_NodesHierarchy.clear();
}

void ImmedidateUserInterfaceContextLayer::set_next_node_maximum_size(const gs_vec2f& _Value)
{
    m_NextNodeMaximumSize = _Value;
}

void ImmedidateUserInterfaceContextLayer::set_next_node_position(const gs_vec2f& _Value)
{
    m_NextNodePosition = _Value;
}

void ImmedidateUserInterfaceContextLayer::set_next_node_size(const gs_vec2f& _Value)
{
    m_NextNodeSize = _Value;
}

void ImmedidateUserInterfaceContextLayer::same_line()
{
    m_NextNodeCursorDirection = node_horizontal_cursor_direction();
}

bool ImmedidateUserInterfaceContextLayer::node_hierarchy_is_empty() const
{
    return m_NodesHierarchy.empty();
}

ImmedidateUserInterfaceNode* ImmedidateUserInterfaceContextLayer::node_hierarchy_top() const
{
    return !m_NodesHierarchy.empty() ? m_NodesHierarchy[m_NodesHierarchy.size() - 1] : nullptr;
}

bool ImmedidateUserInterfaceContextLayer::node_cache_is_empty() const
{
    return m_NodesCache.empty();
}

ImmedidateUserInterfaceNode* ImmedidateUserInterfaceContextLayer::node_cache_request(
    const std::string&                     _Name,
    const ImmedidateUserInterfaceNodeType& _Type) const
{
    if(m_NodesCache[_Type].find(_Name) == m_NodesCache[_Type].end())
    {
        // create window
        std::unique_ptr<ImmedidateUserInterfaceNode> window = std::make_unique<ImmedidateUserInterfaceNode>();
        
        // move window to cache
        window->Name               = _Name;
        m_NodesCache[_Type][_Name] = std::move(window);
    }

    ImmedidateUserInterfaceNode* window   = m_NodesCache[_Type][_Name].get();
    gs_vec2f                     position = m_NextNodePosition.has_value() ? m_NextNodePosition.value() : window->State.WindowBox.Min;
    gs_vec2f                     size     = m_NextNodeSize.has_value() ? m_NextNodeSize.value() : window->State.WindowBox.size();

    window->State.WindowBox = 
        gs_2dboxf(
            position,
            position + gs_clamp(
                size,
                gs_vec2f(window->State.WindowMinimumWidth, window->State.WindowMinimumHeight),
                gs_vec2f(window->State.WindowMaximumWidth, window->State.WindowMaximumHeight)));

    // reset optional next frame paramters
    m_NextNodePosition.reset();
    m_NextNodeSize.reset();

    return window;
}

bool ImmedidateUserInterfaceContextLayer::begin_node(
    const std::string&                  _Name,
    bool*                               _Rendered,
    ImmedidateUserInterfaceNodeSettings _Settings,
    ImmedidateUserInterfaceNodeType     _Type
)
{
    ImmedidateUserInterfaceNode* window = node_cache_request(_Name, _Type);
    window->State.Type                  = _Type;
    window->State.Settings              = _Settings;
    window->State.FirstChild            = (int)m_NodesDrawList.size() + 1;

    if(!m_NodesHierarchy.empty() && !(window->State.Settings & ImmedidateUserInterfaceNodeSettings_IgnoreParent))
    {
        window->State.Parent                           = m_NodesHierarchy[m_NodesHierarchy.size() - 1];
        window->State.Depth                            = node_calculate_depth(window->State.Parent);
        window->State.Parent->State.LastChild          = window->State.FirstChild;
        window->State.Parent->State.LayoutTotalWeight += window->State.LayoutFillWeight;

        // move cursor
        window->State.Parent->State.LayoutCursorPositon += window->State.Parent->State.LayoutCursorSize * window->State.Parent->State.LayoutCursorDirection;

        // compute parent layout box
        gs_2dboxf parentLayoutBox = gs_2dboxf(
            window->State.Parent->State.WindowScrollAreaBox.Min,
            window->State.Parent->State.WindowScrollAreaBox.Min + window->State.Parent->State.WindowViewportBox.size());

        // compute child layout size
        if(window->State.Parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally)
        {
            float width  = (parentLayoutBox.size() * window->State.LayoutFillWeight / window->State.Parent->Cache.LayoutTotalWeight).x;
            float height = parentLayoutBox.height();

            window->State.Parent->State.LayoutCursorSize      = gs_vec2f(width, height);
            window->State.Parent->State.LayoutCursorDirection = node_horizontal_cursor_direction();
        }
        else if(window->State.Parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenVertically)
        {
            float width  = parentLayoutBox.width();
            float height = (parentLayoutBox.size() * window->State.LayoutFillWeight / window->State.Parent->Cache.LayoutTotalWeight).y;

            window->State.Parent->State.LayoutCursorSize      = gs_vec2f(width, height);
            window->State.Parent->State.LayoutCursorDirection = node_vertical_cursor_direction();
        }
        else
        {
            window->State.Parent->State.LayoutCursorSize = window->State.WindowBox.size();
        }

        window->State.Parent->State.LayoutCursorSize =
            gs_clamp(
                window->State.Parent->State.LayoutCursorSize,
                gs_vec2f(window->State.WindowMinimumWidth, window->State.WindowMinimumHeight),
                gs_vec2f(window->State.WindowMaximumWidth, window->State.WindowMaximumHeight));

        window->State.WindowBox = gs_2dboxf(
            parentLayoutBox.Min,
            parentLayoutBox.Min + window->State.Parent->State.LayoutCursorSize);

        window->State.WindowTransform = window->State.Parent->State.WindowTransform * m_Renderer->calculate_transform_matrix(
            0.f,
            window->State.Parent->State.LayoutCursorPositon);

        window->State.Settings &= ~ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable;
    }
    else
    {
        if(node_is_being_focused(window))
        {
            window->State.Depth = (int)(m_Renderer->get_far_plane() / 2);
        }
        else
        {            
            int windowMaximumDepth = 0;
            for (auto drawnWindow : m_NodesDrawList)
            {
                if(!node_is_being_focused(drawnWindow))
                    windowMaximumDepth = gs_max(drawnWindow->State.Depth + drawnWindow->State.Thickness, windowMaximumDepth);
            }
            window->State.Depth = windowMaximumDepth;
        }
    }

    node_calculate_geometry(window);
    
    // check self
    if((window->State.Parent != nullptr) &&
       (std::find(m_NodesDrawList.begin(), m_NodesDrawList.end(), window->State.Parent) == m_NodesDrawList.end()) || (_Rendered != nullptr && !(*_Rendered))) return false;

    // memorize current clipping
    if(!(window->State.Settings & ImmedidateUserInterfaceNodeSettings_IgnoreClipping))
    {
        if((window->State.Settings & ImmedidateUserInterfaceNodeSettings_IgnoreParent))
        {
            m_NodesClipBoxes.push_back(m_Renderer->current_clip_box());
            m_Renderer->pop_clip_box();
        }

        node_render_clipbox(window);
    }

    node_render_background(window);

    // push window into hierarchy stack and draw list
    m_NodesDrawList.push_back(window);
    m_NodesHierarchy.push_back(window);

    return true;
}

void ImmedidateUserInterfaceContextLayer::end_node()
{
    if(m_NodesHierarchy.empty()) return;

    auto window = m_NodesHierarchy[m_NodesHierarchy.size() - 1];

    // update parental depth
    auto child = window;

    while (child->State.Parent)
    {
        child->State.Parent->State.Depth = node_calculate_depth(child);
        child = child->State.Parent;
    }

    if(!(window->State.Settings & ImmedidateUserInterfaceNodeSettings_IgnoreClipping))
        m_Renderer->pop_clip_box();

    node_render_background_frame(window);
    if(node_is_vertical_scroll_bar_needed(window))
        node_render_vertical_scrollbar(window);
    if(node_is_horizontal_scroll_bar_needed(window))
        node_render_horizontal_scrollbar(window);

    node_render_resize_events_gizmos(window);

    node_process_events(window);

    if((window->State.Settings & ImmedidateUserInterfaceNodeSettings_IgnoreParent) && !m_NodesClipBoxes.empty())
    {
        m_Renderer->push_clip_box(m_NodesClipBoxes[m_NodesClipBoxes.size() - 1]);
        m_NodesClipBoxes.pop_back();
    }

    m_NodesHierarchy.pop_back();
}

bool ImmedidateUserInterfaceContextLayer::begin_window(const std::string& _Name, bool* _Rendered, ImmedidateUserInterfaceNodeSettings _Settings)
{
    if(!begin_node(_Name, _Rendered, _Settings, ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Window))
        return false;

    // calculate window frame geometry
    auto geometryBox = gs_2dboxf(
        node_hierarchy_top()->State.WindowBox.Min,
        node_hierarchy_top()->State.WindowBox.Min + gs_vec2f(node_hierarchy_top()->State.WindowBox.width() - m_Style->FrameWidth * 2.f, m_Style->FontSize + m_Style->FrameWidth * 2.f));

    auto renderingData = widget_prepare_for_rendering(geometryBox.size());
    auto boundingBox   = renderingData.BoundingBox;

    // render frame
    m_Renderer->push_rectangle_rounded_filled(
        boundingBox.Min,
        boundingBox.Max,
        m_Style->FrameRoundingRadius,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        node_hierarchy_top()->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(node_hierarchy_top())));

    m_Renderer->push_rectangle_rounded(
        boundingBox.Min,
        boundingBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        gs_vec4f(0.f, 255.f, 0.f, 255.f),
        node_hierarchy_top()->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(node_hierarchy_top())));

    m_Renderer->push_text(
        _Name,
        m_Style->FontSize,
        gs_vec4f(255.f, 255.f, 255.f, 255.f),
        node_hierarchy_top()->State.WindowTransform * m_Renderer->calculate_transform_matrix(
            (float)node_calculate_depth(node_hierarchy_top()),
            gs_vec2f(
                boundingBox.Min.x + m_Style->FrameWidth * 2.f,
                (boundingBox.center() - m_Renderer->calculate_bounding_box(_Name, m_Style->FontSize, m_Style->Font).size() * 0.5f).y)),
        m_Style->Font);

    // render frame expand button
    // render frame close  button

    return true;
}

void ImmedidateUserInterfaceContextLayer::end_window()
{
    end_node(); // window
}

bool ImmedidateUserInterfaceContextLayer::begin_menu(const std::string& _Name)
{
    ImmedidateUserInterfaceNodeSettings settings =
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_IgnoreParent;

    ImmedidateUserInterfaceNodeType type =
        ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Menu;

    // TODO: identify, that we are trying to add something into a menu
    if(node_is_of_type(node_hierarchy_top(), ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Menu))
    {
         // TODO: differentiate between menu item and menu...
        widget_push_button(_Name);
        
        auto cachedMenu = node_cache_request(_Name, ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Menu);

        if(widget_is_mouse_hovered())
            node_begin_hover(cachedMenu);

        // check self hover
        bool isHovered = cachedMenu != nullptr && (node_is_being_hovered(cachedMenu) ||
                         Frenchie::Core::elapsed<std::chrono::microseconds>(cachedMenu->State.WindowHoverStart, Frenchie::Core::tic()) < 0.1 * std::micro().den);

        if(isHovered)
        {
            m_HoveredMenus.push_back(cachedMenu);
        }
        else
        {
            bool anyHovered = false;
            for (auto& window : m_HoveredMenus)
            {
                if((window->State.Type & ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Menu))
                {
                    anyHovered = anyHovered || node_is_being_hovered(window);

                    if(window == cachedMenu)
                        isHovered = true;
                }
            }

            if(!anyHovered)
                m_HoveredMenus.clear();
        }
        

        if(!widget_is_mouse_hovered() && !isHovered)
            return false;

        auto hierarchyTopMenu = node_hierarchy_top();

        hierarchyTopMenu->State.LayoutCursorDirection = node_horizontal_cursor_direction();

        set_next_node_position(
            gs_vec2f(
                hierarchyTopMenu->State.WindowScrollAreaBox.transform(hierarchyTopMenu->State.WindowTransform).Max.x + hierarchyTopMenu->State.ScrollBarOffset.x,
                hierarchyTopMenu->State.WindowScrollAreaBox.transform(hierarchyTopMenu->State.WindowTransform).Min.y + hierarchyTopMenu->State.LayoutCursorPositon.y));

        if(begin_node(_Name, nullptr, settings, type))
        {
            if(cachedMenu != nullptr && cachedMenu->State.WindowBox.transform(cachedMenu->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
            {
                node_begin_hover(cachedMenu);
            }

            return true;
        }

        return false;
    }

    return begin_node(_Name, nullptr, settings, type);
}

void ImmedidateUserInterfaceContextLayer::end_menu()
{
    end_node();
}

bool ImmedidateUserInterfaceContextLayer::widget_push_button(const std::string& _Name)
{
    if(m_NodesHierarchy.empty())
        return false;
    
    auto widgetData  = widget_prepare_for_rendering(_Name);
    auto boundingBox = widgetData.BoundingBox;
    auto clippingBox = widgetData.ClippingBox;
    auto node        = widgetData.Node;

    // render
    return widget_render_default_button_widget(_Name, boundingBox, clippingBox, node);
}

bool ImmedidateUserInterfaceContextLayer::widget_menu_button(const std::string& _Name)
{
    return widget_push_button(_Name);
}

void ImmedidateUserInterfaceContextLayer::node_calculate_geometry(ImmedidateUserInterfaceNode* _Window) const
{
    if(_Window == nullptr) return;

    // window box
    _Window->State.ScrollBarOffset = gs_vec2f(
        node_is_vertical_scroll_bar_needed(_Window)   ? m_Style->WindowScrollBarSliderWidth : 0.f,
        node_is_horizontal_scroll_bar_needed(_Window) ? m_Style->WindowScrollBarSliderWidth : 0.f);

    if(_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents)
    {
        _Window->State.WindowBox = gs_2dboxf(
            _Window->State.WindowBox.Min,
            _Window->State.WindowBox.Min +
                gs_clamp(
                    _Window->Cache.WindowContentBox.size() + _Window->State.ScrollBarOffset,
                    gs_vec2f(_Window->State.WindowMinimumWidth, _Window->State.WindowMinimumHeight),
                    gs_vec2f(_Window->State.WindowMaximumWidth, _Window->State.WindowMaximumHeight)));

        _Window->State.HorizontalScrollBar.SliderPosition = 0.f;
        _Window->State.VerticalScrollBar.SliderPosition   = 0.f;
    }
    else if(_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)
    {
        _Window->State.WindowBox = gs_2dboxf(
            _Window->State.WindowBox.Min,
            _Window->State.WindowBox.Min +
                gs_clamp(
                    gs_vec2f(_Window->Cache.WindowContentBox.width(), _Window->State.WindowBox.height()) + _Window->State.ScrollBarOffset,
                    gs_vec2f(_Window->State.WindowMinimumWidth, _Window->State.WindowMinimumHeight),
                    gs_vec2f(_Window->State.WindowMaximumWidth, _Window->State.WindowMaximumHeight)));

        _Window->State.HorizontalScrollBar.SliderPosition = 0.f;
    }
    else if(_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically)
    {
        _Window->State.WindowBox = gs_2dboxf(
            _Window->State.WindowBox.Min,
            _Window->State.WindowBox.Min +
                gs_clamp(
                    gs_vec2f(_Window->State.WindowBox.width(), _Window->Cache.WindowContentBox.height()) + _Window->State.ScrollBarOffset,
                    gs_vec2f(_Window->State.WindowMinimumWidth, _Window->State.WindowMinimumHeight),
                    gs_vec2f(_Window->State.WindowMaximumWidth, _Window->State.WindowMaximumHeight)));

        _Window->State.VerticalScrollBar.SliderPosition   = 0.f;
    }

    // // frame
    // {
    //     // frame bounding box
    //     _Window->State.WindowFrameBox =
    //         gs_2dboxf(
    //             _Window->State.WindowBox.Min,
    //             _Window->State.WindowBox.Min + gs_vec2f(_Window->State.WindowBox.width(), m_Style->FontSize));

    //     // frame title
    //     gs_vec2f titleSize = m_Renderer->calculate_bounding_box(_Window->Name, m_Style->FontSize, m_Style->Font).size();
    //     gs_vec2f titleMin  = gs_vec2f(
    //         _Window->State.WindowFrameBox.Min.x + m_Style->FrameWidth,
    //         _Window->State.WindowFrameBox.center().y - titleSize.y * 0.5f);
    //     gs_vec2f titleMax  = titleMin + titleSize;

    //     _Window->State.WindowTitleBox = gs_2dboxf(titleMin, titleMax);

    //     // frame close button
    //     gs_vec2f closeButtonMin = gs_vec2f(
    //         _Window->State.WindowFrameBox.Max.x - m_Style->WindowResizeAngleGizmoRadius - titleSize.y,
    //         _Window->State.WindowTitleBox.Min.y);
    //     gs_vec2f closeButtonMax = closeButtonMin + gs_vec2f(titleSize.y, titleSize.y);
    //     _Window->State.WindowCloseButtonBox = gs_2dboxf(closeButtonMin, closeButtonMax);
    // }

    // viewport
    {
        _Window->State.WindowViewportBox = gs_2dboxf(
            _Window->State.WindowBox.Min,
            _Window->State.WindowBox.Max - _Window->State.ScrollBarOffset);
    }

    // vertical scrollbar
    {
        _Window->State.VerticalScrollBar.setup(
            0.f,
            _Window->State.WindowViewportBox.height(),
            _Window->Cache.WindowContentBox.height() + m_Style->WindowScrollBarSliderWidth,
            _Window->State.WindowViewportBox.height());

        if(!node_is_vertical_scroll_bar_needed(_Window))
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

        if(!node_is_horizontal_scroll_bar_needed(_Window))
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

            _Window->State.WindowInnerClipAreaBox =
                gs_2dboxf(
                    _Window->State.WindowViewportBox.transform(_Window->State.WindowTransform).Min,
                    _Window->State.WindowViewportBox.transform(_Window->State.WindowTransform).Max + _Window->State.ScrollBarOffset);

            _Window->State.WindowOuterClipAreaBox = next->State.WindowInnerClipAreaBox; // this clipbox is already transformed to parent coordinates
        }
        else
        {
            gs_vec2f outerBorder =
                gs_vec2f(m_Style->WindowResizeAngleGizmoRadius, m_Style->WindowResizeAngleGizmoRadius);

            _Window->State.WindowInnerClipAreaBox =
                gs_2dboxf(
                    _Window->State.WindowViewportBox.transform(_Window->State.WindowTransform).Min,
                    _Window->State.WindowViewportBox.transform(_Window->State.WindowTransform).Max + _Window->State.ScrollBarOffset);

            _Window->State.WindowOuterClipAreaBox =
                gs_2dboxf(
                    _Window->State.WindowBox.transform(_Window->State.WindowTransform).Min - outerBorder,
                    _Window->State.WindowBox.transform(_Window->State.WindowTransform).Max + outerBorder);
        }
    }

    node_calculate_geometry(_Window->State.Parent);
}

int ImmedidateUserInterfaceContextLayer::node_calculate_depth(ImmedidateUserInterfaceNode* _Window) const
{
    if(_Window == nullptr) return 0;
    return (_Window->State.Depth + (_Window->State.Thickness++));
}

gs_vec2f ImmedidateUserInterfaceContextLayer::node_vertical_cursor_direction() const
{
    return gs_vec2f(0.f, 1.f);
}

gs_vec2f ImmedidateUserInterfaceContextLayer::node_horizontal_cursor_direction() const
{
    return gs_vec2f(1.f, 0.f);
}

bool ImmedidateUserInterfaceContextLayer::node_is_of_type(const ImmedidateUserInterfaceNode* _Node, const ImmedidateUserInterfaceNodeType& _Type) const
{
    return _Node != nullptr && (_Node->State.Type & _Type);
}

bool ImmedidateUserInterfaceContextLayer::node_is_vertical_scroll_bar_needed(const ImmedidateUserInterfaceNode* _Window) const
{
    if(_Window == nullptr) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_NeverVerticalScrollBar)     return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_ResizeToContents)           return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_AlwaysVerticalScrollBar)    return true;

    return _Window->Cache.WindowContentBox.height() > _Window->Cache.WindowViewportBox.height() &&
            gs_abs(_Window->Cache.WindowViewportBox.height() - _Window->Cache.WindowContentBox.height()) > _Window->Cache.WindowViewportBox.height() * 0.1f;
}

bool ImmedidateUserInterfaceContextLayer::node_is_horizontal_scroll_bar_needed(const ImmedidateUserInterfaceNode* _Window) const
{
    if(_Window == nullptr) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_NeverHorizontalScrollBar)     return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents)             return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar)    return true;

    return _Window->Cache.WindowContentBox.width() > _Window->Cache.WindowViewportBox.width() &&
            gs_abs(_Window->Cache.WindowViewportBox.width() - _Window->Cache.WindowContentBox.width()) > _Window->Cache.WindowViewportBox.width() * 0.1f;
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_hovered(const ImmedidateUserInterfaceNode* _Window)
{
    return _Window != nullptr &&
            (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHovered);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_resized_top_left(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_resized_top_right(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_resized_bottom_left(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_resized_bottom_right(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_resized_top(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_resized_left(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_resized_right(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_resized_bottom(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_resized(const ImmedidateUserInterfaceNode* _Window) const
{
    return node_is_being_resized_top_left(_Window)      ||
            node_is_being_resized_top_right(_Window)    ||
            node_is_being_resized_bottom_left(_Window)  ||
            node_is_being_resized_bottom_right(_Window) ||
            node_is_being_resized_top(_Window)          ||
            node_is_being_resized_left(_Window)         ||
            node_is_being_resized_right(_Window)        ||
            node_is_being_resized_bottom(_Window);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_moved(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMoved);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_modified(const ImmedidateUserInterfaceNode* _Window) const
{
    return node_is_being_moved(_Window) || node_is_being_resized(_Window) || node_is_being_scrolled(_Window);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_focused(const ImmedidateUserInterfaceNode* _Window) const
{
    if (_Window == nullptr) return false;

    auto parent = _Window->State.Parent;
    auto window = _Window;

    while (parent != nullptr)
    {
        window = parent;
        parent = parent->State.Parent;
    }

    return (window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingFocused);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_scrolled_vertically(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_scrolled_horizontally(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally);
}

bool ImmedidateUserInterfaceContextLayer::node_is_being_scrolled(const ImmedidateUserInterfaceNode* _Window) const
{
    return node_is_being_scrolled_vertically(_Window) || node_is_being_scrolled_horizontally(_Window);
}

void ImmedidateUserInterfaceContextLayer::node_begin_hover(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;

    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHovered;

    _Window->State.WindowHoverStart = Frenchie::Core::tic();
}

void ImmedidateUserInterfaceContextLayer::node_begin_resize_top_left(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr                                                                                                                ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents)           ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;

    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft;
}

void ImmedidateUserInterfaceContextLayer::node_begin_resize_top_right(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr                                                                                                                ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents)           ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight;
}

void ImmedidateUserInterfaceContextLayer::node_begin_resize_bottom_left(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr                                                                                                                ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents)           ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft;
}

void ImmedidateUserInterfaceContextLayer::node_begin_resize_bottom_right(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr                                                                                                              ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents)           ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight;
}

void ImmedidateUserInterfaceContextLayer::node_begin_resize_top(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents) ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically)) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop;
}

void ImmedidateUserInterfaceContextLayer::node_begin_resize_left(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents) ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;

    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft;
}

void ImmedidateUserInterfaceContextLayer::node_begin_resize_right(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents) ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight;
}

void ImmedidateUserInterfaceContextLayer::node_begin_resize_bottom(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContents) ||
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically)) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom;
}

void ImmedidateUserInterfaceContextLayer::node_begin_move(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMoved;
}

void ImmedidateUserInterfaceContextLayer::node_begin_focus(ImmedidateUserInterfaceNode* _Window)
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
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingFocused;
}

void ImmedidateUserInterfaceContextLayer::node_begin_scroll_vertically(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically;
}

void ImmedidateUserInterfaceContextLayer::node_begin_scroll_horizontally(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes |=
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally;
}

void ImmedidateUserInterfaceContextLayer::node_end_hover(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;

    _Window->State.Changes &= ~(ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHovered);
}

void ImmedidateUserInterfaceContextLayer::node_end_resize(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;

    _Window->State.Changes &= ~(
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft    |
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight   |
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft |
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight|
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop        |
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft       |
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight      |
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom
    );
}

void ImmedidateUserInterfaceContextLayer::node_end_move(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes &=
        ~ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMoved;
}

void ImmedidateUserInterfaceContextLayer::node_end_focus(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes &=
        ~ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingFocused;
}

void ImmedidateUserInterfaceContextLayer::node_end_scroll(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;

    _Window->State.Changes &= ~(
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically |
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally);
}

bool ImmedidateUserInterfaceContextLayer::node_render_clipbox(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    m_Renderer->push_clip_box(
        _Window->State.WindowInnerClipAreaBox.clip_with(_Window->State.WindowOuterClipAreaBox));

    // m_Renderer->push_rectangle_rounded(
    //     _Window->State.WindowInnerClipAreaBox.clip_with(_Window->State.WindowOuterClipAreaBox).Min,
    //     _Window->State.WindowInnerClipAreaBox.clip_with(_Window->State.WindowOuterClipAreaBox).Max,
    //     m_Style->FrameRoundingRadius,
    //     m_Style->FrameWidth,
    //     gs_vec4f(0.f, 255.f, 0.f, 255.f),
    //     m_Renderer->calculate_transform_matrix(m_Renderer->get_far_plane() * 0.5f));

    return true;
}

bool ImmedidateUserInterfaceContextLayer::node_render_background(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowBox.Min,
        _Window->State.WindowBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));

    // m_Renderer->push_rectangle_rounded(
    //     _Window->State.WindowContentBox.Min,
    //     _Window->State.WindowContentBox.Max,
    //     m_Style->FrameRoundingRadius,
    //     m_Style->FrameWidth,
    //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
    //     _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix(m_Renderer->get_far_plane() * 0.5f));

    return true;
}

bool ImmedidateUserInterfaceContextLayer::node_render_background_frame(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowBox.Min,
        _Window->State.WindowBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));

    return true;
}

bool ImmedidateUserInterfaceContextLayer::node_render_vertical_scrollbar(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    // draw scroll panel
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowVerticalScrollBarBox.Min,
        _Window->State.WindowVerticalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));

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
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
    }
    else
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.WindowVerticalScrollBarSliderBox.Min,
            _Window->State.WindowVerticalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderColor],
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
    }
    
    // draw frame
    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowVerticalScrollBarBox.Min,
        _Window->State.WindowVerticalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
    
    return true;
}

bool ImmedidateUserInterfaceContextLayer::node_render_horizontal_scrollbar(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    // draw scroll panel
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowHorizontalScrollBarBox.Min,
        _Window->State.WindowHorizontalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));

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
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
    }
    else
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.WindowHorizontalScrollBarSliderBox.Min,
            _Window->State.WindowHorizontalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderColor],
            _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
    }

    // draw frame
    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowHorizontalScrollBarBox.Min,
        _Window->State.WindowHorizontalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
    
    return true;
}

bool ImmedidateUserInterfaceContextLayer::node_render_resize_events_gizmos(ImmedidateUserInterfaceNode* _Window)
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


    if((_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Resizable))
    {
        if((resizeTopLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || node_is_being_resized_top_left(_Window)) && !node_is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeTopLeft.Center,
                resizeTopLeft.Radius,
                resizeTopLeft.Radius,
                0.f,
                360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
        }
        else if((resizeTopRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || node_is_being_resized_top_right(_Window)) && !node_is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeTopRight.Center,
                resizeTopRight.Radius,
                resizeTopRight.Radius,
                0.f,
                360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
        }
        else if((resizeBottomLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || node_is_being_resized_bottom_left(_Window)) && !node_is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeBottomLeft.Center,
                resizeBottomLeft.Radius,
                resizeBottomLeft.Radius,
                0.f, 360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
        }
        else if((resizeBottomRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || node_is_being_resized_bottom_right(_Window)) && !node_is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeBottomRight.Center,
                resizeBottomRight.Radius,
                resizeBottomRight.Radius,
                0.f,
                360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
        }
        else if((resizeTop.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || node_is_being_resized_top(_Window)) && !node_is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeTop.Min,
                resizeTop.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
        }
        else if((resizeLeft.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || node_is_being_resized_left(_Window)) && !node_is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeLeft.Min,
                resizeLeft.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
        }
        else if((resizeRight.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || node_is_being_resized_right(_Window)) && !node_is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeRight.Min,
                resizeRight.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
        }
        else if((resizeBottom.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) || node_is_being_resized_bottom(_Window)) && !node_is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeBottom.Min,
                resizeBottom.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Window)));
        }
    }

    return true;
}

void ImmedidateUserInterfaceContextLayer::node_receive_events()
{
    if(m_NodesDrawList.empty()) return;

    ImmedidateUserInterfaceNode* sink = nullptr;

    for (auto next : m_NodesDrawList)
    {
        gs_2d_ellipsef resizeTopLeft     = gs_2d_ellipsef(next->State.WindowBox.Min, m_Style->WindowResizeAngleGizmoRadius);
        gs_2d_ellipsef resizeTopRight    = gs_2d_ellipsef(next->State.WindowBox.Min + gs_vec2f(next->State.WindowBox.width(), 0.f), m_Style->WindowResizeAngleGizmoRadius);
        gs_2d_ellipsef resizeBottomLeft  = gs_2d_ellipsef(next->State.WindowBox.Max - gs_vec2f(next->State.WindowBox.width(), 0.f), m_Style->WindowResizeAngleGizmoRadius);
        gs_2d_ellipsef resizeBottomRight = gs_2d_ellipsef(next->State.WindowBox.Max, m_Style->WindowResizeAngleGizmoRadius);
        gs_2dboxf      resizeTop         = gs_2dboxf(next->State.WindowBox.Min - gs_vec2f(0.f, m_Style->WindowResizeSideGizmoWidth), next->State.WindowBox.Min + gs_vec2f(next->State.WindowBox.width(), m_Style->WindowResizeSideGizmoWidth));
        gs_2dboxf      resizeLeft        = gs_2dboxf(next->State.WindowBox.Min - gs_vec2f(m_Style->WindowResizeSideGizmoWidth, 0.f), next->State.WindowBox.Min + gs_vec2f(m_Style->WindowResizeSideGizmoWidth, next->State.WindowBox.height()));
        gs_2dboxf      resizeRight       = gs_2dboxf(next->State.WindowBox.Min + gs_vec2f(next->State.WindowBox.width() - m_Style->WindowResizeSideGizmoWidth, 0.f), next->State.WindowBox.Max + gs_vec2f(m_Style->WindowResizeSideGizmoWidth, 0.f));
        gs_2dboxf      resizeBottom      = gs_2dboxf(next->State.WindowBox.Min + gs_vec2f(0.f, next->State.WindowBox.height() - m_Style->WindowResizeSideGizmoWidth), next->State.WindowBox.Max + gs_vec2f(0.f, m_Style->WindowResizeSideGizmoWidth));

        // mouse hover
        if(next->State.WindowBox.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
        {
            node_begin_hover(next);
        }

        // poll window resize events
        if( (next->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Resizable) &&
            application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                &&
            !node_is_being_scrolled(next)                                                                            &&
            !node_is_being_resized(next)                                                                             &&
            !node_is_being_moved(next))
        {
            if(resizeTopLeft.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                node_begin_resize_top_left(next);
            else if(resizeTopRight.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                node_begin_resize_top_right(next);
            else if(resizeBottomLeft.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                node_begin_resize_bottom_left(next);
            else if(resizeBottomRight.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                node_begin_resize_bottom_right(next);
            else if(resizeTop.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
                node_begin_resize_top(next);
            else if(resizeLeft.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
                node_begin_resize_left(next);
            else if(resizeRight.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                node_begin_resize_right(next);
            else if(resizeBottom.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) && next->State.Parent == nullptr)
                node_begin_resize_bottom(next);
        }

        // poll window move event
        if( (next->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable)        &&
            application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                     &&
            next->State.WindowViewportBox.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) &&
            !node_is_being_resized(next)                                                                                  &&
            !node_is_being_scrolled(next))
        {
            node_begin_move(next);
        }

        // poll window vertical scroll event
        if( application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                                    &&
            next->State.WindowVerticalScrollBarSliderBox.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) &&
            node_is_vertical_scroll_bar_needed(next)                                                                                     &&
            !node_is_being_resized(next)                                                                                                 &&
            !node_is_being_moved(next))
        {
            node_begin_scroll_vertically(next);
        }

        // poll window horizontal scroll event
        if( application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                                      &&
            next->State.WindowHorizontalScrollBarSliderBox.transform(next->State.WindowTransform).contains(m_Renderer->get_cursor_postion()) &&
            node_is_horizontal_scroll_bar_needed(next)                                                                                     &&
            !node_is_being_resized(next)                                                                                                   &&
            !node_is_being_moved(next))
        {
            node_begin_scroll_horizontally(next);
        }

        if (node_is_being_modified(next))
        {
            sink = next;
            // if(sink == nullptr)
            //     sink = next;
            // else if(next->State.Depth > sink->State.Depth && sink->State.Parent != next)
            //     sink = next;
        }
    }

    if(sink != nullptr)
    {
        for (auto& window : m_NodesDrawList)
        {
            if(window != sink)
            {
                node_end_move(window);
                node_end_resize(window);
                node_end_scroll(window);
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
            node_end_move(sink);
            node_end_resize(sink);
            node_end_scroll(sink);
        }
    }
}

void ImmedidateUserInterfaceContextLayer::node_process_events(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;

    // end mouse hover
    if(!_Window->State.WindowBox.transform(_Window->State.WindowTransform).contains(m_Renderer->get_cursor_postion()))
        node_end_hover(_Window);

    if(node_is_being_resized(_Window))
    {
        gs_2dboxf estimatedBox;
        gs_vec2f  cursorDelta = application()->get_window_cursor_dragdelta();

        if(node_is_being_resized_top_left(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + cursorDelta,
                _Window->Cache.WindowBox.Max);
        }
        else if(node_is_being_resized_top_right(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Window->Cache.WindowBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
        }
        else if(node_is_being_resized_bottom_left(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Window->Cache.WindowBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
        }
        else if(node_is_being_resized_bottom_right(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min,
                _Window->Cache.WindowBox.Max + application()->get_window_cursor_dragdelta());
        }
        else if(node_is_being_resized_top(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Window->Cache.WindowBox.Max);
        }
        else if(node_is_being_resized_left(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Window->Cache.WindowBox.Max);
        }
        else if(node_is_being_resized_right(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min,
                _Window->Cache.WindowBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
        }
        else if(node_is_being_resized_bottom(_Window))
        {
            estimatedBox = gs_2dboxf(
                _Window->Cache.WindowBox.Min,
                _Window->Cache.WindowBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
        }

        // handle minimum width
        float minX = estimatedBox.size().x > _Window->State.WindowMinimumWidth  ? estimatedBox.Min.x : _Window->Cache.WindowBox.Min.x + (_Window->State.WindowBox.Min.x - _Window->Cache.WindowBox.Min.x);
        float maxX = estimatedBox.size().x > _Window->State.WindowMinimumWidth  ? estimatedBox.Max.x : _Window->Cache.WindowBox.Max.x + (_Window->State.WindowBox.Max.x - _Window->Cache.WindowBox.Max.x);
        float minY = estimatedBox.size().y > _Window->State.WindowMinimumHeight ? estimatedBox.Min.y : _Window->Cache.WindowBox.Min.y + (_Window->State.WindowBox.Min.y - _Window->Cache.WindowBox.Min.y);
        float maxY = estimatedBox.size().y > _Window->State.WindowMinimumHeight ? estimatedBox.Max.y : _Window->Cache.WindowBox.Max.y + (_Window->State.WindowBox.Max.y - _Window->Cache.WindowBox.Max.y);
        
        // handle maximum width
        minX = estimatedBox.size().x < _Window->State.WindowMaximumWidth  ? estimatedBox.Min.x : _Window->Cache.WindowBox.Min.x + (_Window->State.WindowBox.Min.x - _Window->Cache.WindowBox.Min.x);
        maxX = estimatedBox.size().x < _Window->State.WindowMaximumWidth  ? estimatedBox.Max.x : _Window->Cache.WindowBox.Max.x + (_Window->State.WindowBox.Max.x - _Window->Cache.WindowBox.Max.x);
        minY = estimatedBox.size().y < _Window->State.WindowMaximumHeight ? estimatedBox.Min.y : _Window->Cache.WindowBox.Min.y + (_Window->State.WindowBox.Min.y - _Window->Cache.WindowBox.Min.y);
        maxY = estimatedBox.size().y < _Window->State.WindowMaximumHeight ? estimatedBox.Max.y : _Window->Cache.WindowBox.Max.y + (_Window->State.WindowBox.Max.y - _Window->Cache.WindowBox.Max.y);

        _Window->State.WindowBox = gs_2dboxf(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));

        if(_Window->State.Parent)
        {
            gs_vec2f fillSize   = (estimatedBox.size() / _Window->Cache.Parent->Cache.WindowBox.size()) * _Window->State.Parent->Cache.LayoutTotalWeight;
            float    fillWeight = 1.f;
            
            if(_Window->State.Parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally)
                fillWeight = fillSize.x;
            else if(_Window->Cache.Parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenVertically)
                fillWeight = fillSize.y;

            float fillWeightDelta = (fillWeight - _Window->State.LayoutFillWeight) / gs_max(((_Window->State.FirstChild - _Window->State.Parent->State.FirstChild) - 1), 1);

            for (auto child : m_NodesDrawList)
            {
                if(child->State.Parent != _Window->State.Parent || child == _Window)
                    continue;

                child->State.LayoutFillWeight -= fillWeightDelta;
                child->State.LayoutFillWeight  = gs_clamp(child->State.LayoutFillWeight, 0.1f, child->State.Parent->Cache.LayoutTotalWeight);
            }

            _Window->State.LayoutFillWeight = fillWeight;
        }
    }
    else if(node_is_being_moved(_Window))
    {
        _Window->State.WindowBox = gs_2dboxf(
            _Window->Cache.WindowBox.Min + application()->get_window_cursor_dragdelta(),
            _Window->Cache.WindowBox.Max + application()->get_window_cursor_dragdelta());

        // dock space
    }
    else if(node_is_being_scrolled_horizontally(_Window))
    {
        _Window->State.HorizontalScrollBar.reposition(
            _Window->Cache.HorizontalScrollBar.SliderPosition + application()->get_window_cursor_dragdelta().x);
    }
    else if(node_is_being_scrolled_vertically(_Window))
    {
        _Window->State.VerticalScrollBar.reposition(
            _Window->Cache.VerticalScrollBar.SliderPosition + application()->get_window_cursor_dragdelta().y);
    }
}

bool ImmedidateUserInterfaceContextLayer::widget_is_mouse_hovered() const
{
    return m_WidgetMouseHovered.has_value() && m_WidgetMouseHovered.value();
}

bool ImmedidateUserInterfaceContextLayer::widget_is_mouse_pressed(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetMousePressed.has_value() &&
            m_WidgetMousePressed.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_is_mouse_down(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetMouseDown.has_value() &&
            m_WidgetMouseDown.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_is_mouse_clicked(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetMouseClicked.has_value() &&
            m_WidgetMouseClicked.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_is_mouse_double_clicked(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetMouseDoubleClicked.has_value() &&
            m_WidgetMouseDoubleClicked.value() == _Button;
}

// bool ImmedidateUserInterfaceContextLayer::widget_render_close_button_widget(
//     const gs_2dboxf& _ButtonBox,
//     const gs_2dboxf& _ClipBox,
//     const gs_mat4f&  _Transform)
// {
//     auto retrieve_close_button_color = [this](const bool& _Pressed, const bool& _Hovered)->gs_vec4f
//     {
//         if(_Pressed) return m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonPressedColor];
//         if(_Hovered) return m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonHoveredColor];
//         return m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonDefaultColor];
//     };

//     bool hovered = _ButtonBox.transform(_Transform).contains(m_Renderer->get_cursor_postion()) && _ClipBox.overlaps(_ButtonBox.transform(_Transform));
    
//     m_Renderer->push_arc_filled(
//         _ButtonBox.center(),
//         _ButtonBox.width()  * 0.5f,
//         _ButtonBox.height() * 0.5f,
//         0.f,
//         360.f,
//         retrieve_close_button_color(hovered && application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left), hovered),
//        _Transform);

//     m_Renderer->push_line(
//         _ButtonBox.Min + gs_vec2f(_ButtonBox.width(), _ButtonBox.height()) * 0.25f,
//         _ButtonBox.Max - gs_vec2f(_ButtonBox.width(), _ButtonBox.height()) * 0.25f,
//         2.f,
//         gs_vec4f(0, 0, 0, 255.f),
//         _Transform * m_Renderer->calculate_transform_matrix(1.f));

//     m_Renderer->push_line(
//         gs_vec2f(_ButtonBox.Max.x, _ButtonBox.Min.y) + gs_vec2f(-_ButtonBox.width(), +_ButtonBox.height()) * 0.25f,
//         gs_vec2f(_ButtonBox.Min.x, _ButtonBox.Max.y) + gs_vec2f(+_ButtonBox.width(), -_ButtonBox.height()) * 0.25f,
//         2.f,
//         gs_vec4f(0, 0, 0, 255.f),
//         _Transform * m_Renderer->calculate_transform_matrix(2.f));
    
//     return hovered && application()->is_mouse_button_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Left);
// }

bool ImmedidateUserInterfaceContextLayer::widget_render_default_button_widget(
    const std::string&             _Text,
    const gs_2dboxf&               _Box,
    const gs_2dboxf&               _ClipBox,
    ImmedidateUserInterfaceNode*   _Context)
{
    auto textBox = m_Renderer->calculate_bounding_box(_Text, m_Style->FontSize, m_Style->Font);

    auto retreive_push_button_background_color = [this](const bool& _Enabled, const bool _Hovered, const bool& _Pressed)->gs_vec4f
    {
        if(_Pressed)
        {
            return _Enabled ?
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedBackgroundColor] :
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedBackgroundColor];
        }

        if(_Hovered)
        {
            return _Enabled ?
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredBackgroundColor] :
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredBackgroundColor];
        }

        return _Enabled ?
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledBackgroundColor] :
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledBackgroundColor];
    };

    auto retreive_push_frame_color = [this](const bool& _Enabled, const bool _Hovered, const bool& _Pressed)->gs_vec4f
    {
        if(_Pressed)
        {
            return _Enabled ?
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedFrameColor] :
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedFrameColor];
        }

        if(_Hovered)
        {
            return _Enabled ?
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredFrameColor] :
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredFrameColor];
        }

        return _Enabled ?
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor] :
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor];
    };

    bool hovered = widget_is_mouse_hovered();
    bool pressed = widget_is_mouse_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left)  ||
                   widget_is_mouse_down(ApplicationMouseButton::Button::ApplicationMouseButton_Right) ||
                   widget_is_mouse_down(ApplicationMouseButton::Button::ApplicationMouseButton_Middle);

    m_Renderer->push_rectangle_rounded_filled(
        _Box.Min,
        _Box.Max,
        m_Style->FrameRoundingRadius,
        retreive_push_button_background_color(true, hovered, pressed),
        _Context->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Context)));

    m_Renderer->push_rectangle_rounded(
        _Box.Min,
        _Box.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        retreive_push_frame_color(true, hovered, pressed),
        _Context->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Context)));

    m_Renderer->push_text(
        _Text,
        m_Style->FontSize,
        gs_vec4f(255.f, 255.f, 255.f, 255.f),
        _Context->State.WindowTransform * m_Renderer->calculate_transform_matrix((float)node_calculate_depth(_Context), _Box.center() - textBox.size() * 0.5f),
        m_Style->Font);

    return pressed;
}