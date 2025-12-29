#include <ImmedidateUserInterfaceLayer.hpp>

using namespace Frenchie::Application;

// Immedidate2DRendererTestLayer
ImmedidateUserInterfaceContextLayer::ImmedidateUserInterfaceContextLayer(){}
ImmedidateUserInterfaceContextLayer::~ImmedidateUserInterfaceContextLayer(){}

auto ImmedidateUserInterfaceContextLayer::widget_prepare_for_rendering(const gs_vec2f& _Size, bool _CatchEvents, bool _UsePadding)
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

    if(ui_node_hierarchy_is_empty())
    {
        WidgetData.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), _Size);
        WidgetData.ClippingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), _Size);
        WidgetData.Node        = nullptr;
        return WidgetData; // TODO: do we have to create default window or let's draw right onto context window ???
    }

    ImmedidateUserInterfaceNode* window  = ui_node_hierarchy_top();
    float padding = _UsePadding ? 16.f : 0.f; // TODO: this MUST BE a setting !!!

    // move cursor
    gs_vec2f cursorDirection = m_NextNodeCursorDirection.has_value() ? m_NextNodeCursorDirection.value() : ui_node_vertical_cursor_direction();
    m_NextNodeCursorDirection.reset();

    // go next line if cursor direction changes to vertical
    if(cursorDirection == ui_node_vertical_cursor_direction())
    {
        window->State.LayoutCursorPositon = gs_vec2f(0.f, window->State.ContentBox.height());
        window->State.LayoutCursorSize    = gs_vec2f(0.f, 0.f);
    }

    window->State.LayoutCursorDirection = cursorDirection;
    window->State.LayoutCursorPositon  += window->State.LayoutCursorSize * window->State.LayoutCursorDirection;

    gs_vec2f min = window->State.LayoutCursorPositon + gs_vec2f(m_Style->FrameWidth, m_Style->FrameWidth);

    window->State.LayoutCursorSize =
        _Size + gs_vec2f(m_Style->FrameWidth, m_Style->FrameWidth) * 2.f +
                gs_vec2f(padding, padding) * window->State.LayoutCursorDirection;

    ui_node_calculate_geometry(window);

    auto geometryBox = gs_2dboxf(min, min + _Size);
    WidgetData.BoundingBox = gs_2dboxf(geometryBox.Min + window->State.ScrollAreaBox.Min, geometryBox.Max + window->State.ScrollAreaBox.Min);
    WidgetData.ClippingBox = window->State.InnerClipAreaBox.clip_with(window->State.OuterClipAreaBox);
    WidgetData.ClippingBox = gs_2dboxf(WidgetData.ClippingBox.Min, WidgetData.ClippingBox.Max - window->State.ScrollBarOffset);
    WidgetData.Node        = window;

    // receive events
    m_WidgetIsBeingMouseHovered.reset();
    m_WidgetIsBeingMouseDown.reset();
    m_WidgetIsBeingMousePressed.reset();
    m_WidgetIsBeingMouseClicked.reset();
    m_WidgetIsBeingMouseDoubleClicked.reset();

    if(!_CatchEvents)
        return WidgetData;

    auto next    = window->Cache.Creator;
    auto creator = next;

    while (next)
    {
        creator = next;
        next    = next->Cache.Creator;
    }
    
    if(creator == nullptr)
        creator = window;

    // receive events
    m_WidgetIsBeingMouseHovered =
        ui_node_is_being_hovered(window)  &&
        ui_node_is_being_focused(creator) &&
        WidgetData.BoundingBox.transform(window->State.Transform).contains(m_Renderer->get_cursor_postion()) &&
        WidgetData.ClippingBox.contains(m_Renderer->get_cursor_postion());

    if(m_WidgetIsBeingMouseHovered.has_value() && m_WidgetIsBeingMouseHovered.value())
    {
        m_WidgetHasBeenMouseHovered.reset();
        m_WidgetHasBeenMouseDown.reset();
        m_WidgetHasBeenMousePressed.reset();
        m_WidgetHasBeenMouseClicked.reset();
        m_WidgetHasBeenMouseDoubleClicked.reset();

        if(application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
            m_WidgetIsBeingMouseDown = ApplicationMouseButton::Button::ApplicationMouseButton_Left;
        else if(application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Right))
            m_WidgetIsBeingMouseDown = ApplicationMouseButton::Button::ApplicationMouseButton_Right;
        else if(application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
            m_WidgetIsBeingMouseDown = ApplicationMouseButton::Button::ApplicationMouseButton_Middle;

        if(application()->is_mouse_button_pressed(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
            m_WidgetIsBeingMousePressed = ApplicationMouseButton::Button::ApplicationMouseButton_Left;
        else if(application()->is_mouse_button_pressed(ApplicationMouseButton::Button::ApplicationMouseButton_Right))
            m_WidgetIsBeingMousePressed = ApplicationMouseButton::Button::ApplicationMouseButton_Right;
        else if(application()->is_mouse_button_pressed(ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
            m_WidgetIsBeingMousePressed = ApplicationMouseButton::Button::ApplicationMouseButton_Middle;

        if(application()->is_mouse_button_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
            m_WidgetIsBeingMouseClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Left;
        else if(application()->is_mouse_button_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Right))
            m_WidgetIsBeingMouseClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Right;
        else if(application()->is_mouse_button_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
            m_WidgetIsBeingMouseClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Middle;

        if(application()->is_mouse_button_double_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Left))
            m_WidgetIsBeingMouseDoubleClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Left;
        else if(application()->is_mouse_button_double_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Right))
            m_WidgetIsBeingMouseDoubleClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Right;
        else if(application()->is_mouse_button_double_clicked(ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
            m_WidgetIsBeingMouseDoubleClicked = ApplicationMouseButton::Button::ApplicationMouseButton_Middle;
    }

    if(m_WidgetIsBeingMouseHovered.has_value())
        m_WidgetHasBeenMouseHovered = m_WidgetIsBeingMouseHovered.value();
    
    if(m_WidgetIsBeingMouseDown.has_value())
        m_WidgetHasBeenMouseDown = m_WidgetIsBeingMouseDown.value();

    if(m_WidgetIsBeingMousePressed.has_value())
        m_WidgetHasBeenMousePressed = m_WidgetIsBeingMousePressed.value();

    if(m_WidgetIsBeingMouseClicked.has_value())
        m_WidgetHasBeenMouseClicked = m_WidgetIsBeingMouseClicked.value();

    if(m_WidgetIsBeingMouseDoubleClicked.has_value())
        m_WidgetHasBeenMouseDoubleClicked = m_WidgetIsBeingMouseDoubleClicked.value();


    return WidgetData;
}

auto ImmedidateUserInterfaceContextLayer::widget_prepare_for_rendering(const std::string& _Text, bool _CatchEvents, bool _UsePadding)
{
    return widget_prepare_for_rendering(m_Renderer->calculate_bounding_box(_Text, m_Style->FontSize, m_Style->Font).size() + gs_vec2f(m_Style->FontSize * 0.5f), _CatchEvents, _UsePadding);
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

    if(begin_window("Window-1", nullptr))
    {
        // render menubar
        if(begin_menubar("Window-1"))
        {
            if(begin_menu("Menu-1"))
            {
                widget_menu_button("Action-1");
                widget_menu_button("Action-2");

                if(begin_menu("Menu-2"))
                {
                    widget_menu_button("Action-1");
                    widget_menu_button("Action-2");
                    end_menu();
                }

                end_menu();
            }

            end_menubar();
        }

        // render some arbitary layout
        if(begin_node("Layout-1"))
        {
            widget_menu_button("Action-1");
            widget_menu_button("Action-2");
            widget_menu_button("Action-3");
            widget_menu_button("Action-4");

            if(begin_node("Layout-2"))
            {
                for(int i = 0; i < 1e1; ++i)
                    widget_push_button(std::string("Button-").append(std::to_string(i)));

                end_node();
            }

            if(begin_node("Layout-3"))
            {
                for(int i = 0; i < 1e1; ++i)
                {
                    set_next_ui_node_cursor_same_line();
                    widget_push_button(std::string("Button-").append(std::to_string(i)));
                }

                end_node();
            }

            end_node();
        }

        end_window();
    }

    if(begin_window("Window-2", nullptr))
    {
        widget_push_button("Window-2");
        end_window();
    }

    if(begin_window("Window-3", nullptr))
    {
        widget_push_button("Window-3");
        end_window();
    }

    // if(begin_window("Window-1", nullptr))
    // {
    //     if(begin_menubar("Window-1-Menubar-1"))
    //     {
    //         if(begin_menu("Menu-1"))
    //         {
    //             widget_menu_button("Action-1");
    //             widget_menu_button("Action-2");
    //             widget_menu_button("Action-3");
    //             widget_menu_button("Action-4");
    //             widget_menu_button("Action-5");
    //             widget_menu_button("Action-6");
    //             widget_menu_button("Action-7");
    //             widget_menu_button("Action-8");
    //             widget_menu_button("Action-9");
    //             widget_menu_button("Action-10");
    //             widget_menu_button("Action-11");
    //             widget_menu_button("Action-12");

    //             if(begin_menu("Menu-2"))
    //             {
    //                 widget_menu_button("Action-1");
    //                 widget_menu_button("Action-2");
    //                 widget_menu_button("Action-3");

    //                 end_menu();
    //             }

    //             if(begin_menu("Menu-3"))
    //             {
    //                 widget_menu_button("Action-2");
    //                 widget_menu_button("Action-3");

    //                 if(begin_menu("Menu-4"))
    //                 {
    //                     widget_menu_button("Action-2");
    //                     widget_menu_button("Action-3");
    //                     end_menu();
    //                 }

    //                 end_menu();
    //             }

    //             end_menu();
    //         }

    //         if(begin_menu("Menu-111"))
    //         {
    //             widget_menu_button("Action-2");
    //             widget_menu_button("Action-3");
    //             end_menubar();
    //         }

    //         end_menubar();
    //     }

    //     if(begin_menubar("Window-1-Menubar-2"))
    //     {
    //         if(begin_menu("Menu-44")) end_menu();
    //         if(begin_menu("Menu-33")) end_menu();

    //         end_menubar();
    //     }

    //     //if(begin_node("Content"))
    //     {
    //         for(int i = 0; i < 1e1; ++i) widget_push_button("Button");

    //         //end_node();
    //     }

    //     end_window();
    // }

    // if(begin_window("Window-2", nullptr))
    // {
    //     if(begin_window("Window-3", nullptr))
    //     {
    //         if(begin_node("Window-3-content", nullptr, ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally))
    //         {
    //             if(begin_window("Window-33", nullptr))end_window();
    //             if(begin_window("Window-44", nullptr))end_window();
    //             end_node();
    //         }

    //         end_window();
    //     }
    //     if(begin_window("Window-4", nullptr))end_window();

    //     end_window();
    // }

    // render cursor
    m_Renderer->push_text(
        std::to_string(m_Renderer->get_cursor_postion().x)
        //.append(" ")
        //.append(std::to_string(m_Renderer->get_cursor_postion().y).append("\t").append(std::to_string(m_Renderer->get_measured_frame_rate())))
        .append(" ")
        .append(HoveredName),
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

void ImmedidateUserInterfaceContextLayer::frame_render()
{
}

void ImmedidateUserInterfaceContextLayer::frame_finish()
{
    ui_node_layout_children();
    ui_node_receive_events();
    ui_node_process_events();
    ui_node_save_state();
}

void ImmedidateUserInterfaceContextLayer::set_next_ui_node_maximum_size(const gs_vec2f& _Value)
{
    m_NextNodeMaximumSize = _Value;
}

void ImmedidateUserInterfaceContextLayer::set_next_ui_node_position(const gs_vec2f& _Value)
{
    m_NextNodePosition = _Value;
}

void ImmedidateUserInterfaceContextLayer::set_next_ui_node_size(const gs_vec2f& _Value)
{
    m_NextNodeSize = _Value;
}

void ImmedidateUserInterfaceContextLayer::set_next_ui_node_cursor_same_line()
{
    m_NextNodeCursorDirection = ui_node_horizontal_cursor_direction();
}

bool ImmedidateUserInterfaceContextLayer::ui_node_cache_is_empty() const
{
    return m_NodesCache.empty();
}

ImmedidateUserInterfaceNode* ImmedidateUserInterfaceContextLayer::ui_node_cache_request(
    const std::string&                     _Name,
    const ImmedidateUserInterfaceNodeType& _Type) const
{
    if(m_NodesCache[_Type].find(_Name) == m_NodesCache[_Type].end())
    {
        // create node
        std::unique_ptr<ImmedidateUserInterfaceNode> window = std::make_unique<ImmedidateUserInterfaceNode>();
        
        // setup node and move it into the cache
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

bool ImmedidateUserInterfaceContextLayer::ui_node_hierarchy_is_empty() const
{
    return m_NodesRenderingStack.empty();
}

ImmedidateUserInterfaceNode* ImmedidateUserInterfaceContextLayer::ui_node_hierarchy_top() const
{
    return !m_NodesRenderingStack.empty() ? m_NodesRenderingStack[m_NodesRenderingStack.size() - 1] : nullptr;
}

bool ImmedidateUserInterfaceContextLayer::begin_node(
    const std::string&                  _Name,
    bool*                               _Rendered,
    ImmedidateUserInterfaceNodeSettings _Settings,
    ImmedidateUserInterfaceNodeType     _Type)
{
    ImmedidateUserInterfaceNode* window = ui_node_cache_request(_Name, _Type);
    window->State.Type                  = _Type;
    window->State.Settings              = _Settings;
    window->State.Creator               = ui_node_hierarchy_top();

    if(!ui_node_hierarchy_is_empty() &&
        !(window->State.Settings & ImmedidateUserInterfaceNodeSettings_NullParent))
    {
        // setup hierarchy
        window->State.Parent                                 = ui_node_hierarchy_top();
        window->State.LayerDepth                             = ui_node_calculate_child_depth_placed_in_follow(window->State.Parent, 1);
        window->State.Parent->State.LayoutTotalChildrenSize += window->State.WindowBox.size();
        window->State.Parent->State.ChildCount++;

        // move cursor
        window->State.Parent->State.LayoutCursorPositon +=
            window->State.Parent->State.LayoutCursorSize * window->State.Parent->State.LayoutCursorDirection;

        if(window->State.Parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally)
            window->State.Parent->State.LayoutCursorDirection = ui_node_horizontal_cursor_direction();
        else if(window->State.Parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenVertically)
            window->State.Parent->State.LayoutCursorDirection = ui_node_vertical_cursor_direction();

        window->State.Parent->State.LayoutCursorSize = window->State.WindowBox.size();

        // compute window box
        gs_2dboxf parentLayoutBox = gs_2dboxf(
            window->State.Parent->State.ScrollAreaBox.Min,
            window->State.Parent->State.ScrollAreaBox.Min + window->State.Parent->State.ViewportBox.size());

        window->State.WindowBox = gs_2dboxf(
            parentLayoutBox.Min,
            parentLayoutBox.Min + window->State.Parent->State.LayoutCursorSize);

        // compute transform
        window->State.Transform = window->State.Parent->State.Transform * m_Renderer->calculate_transform_matrix(
            0.f,
            window->State.Parent->State.LayoutCursorPositon);

        // don't move children
        window->State.Settings &= ~ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable;
    }
    else
    {
        // calculate window initial depth
        window->State.LayerDepth =
            ui_node_calculate_layer_depth(window->Cache.Layer);

        for (auto& drawnWindow : m_NodesRenderingList)
        {
            if(drawnWindow->Cache.Layer == window->Cache.Layer)
            {
                window->State.LayerDepth =
                    gs_max(drawnWindow->State.LayerDepth + drawnWindow->State.Thickness + 1, window->State.LayerDepth);
            }
        }
    }

    ui_node_calculate_geometry(window);
    
    // check self
    if((window->State.Parent != nullptr) &&
       (std::find(m_NodesRenderingList.begin(), m_NodesRenderingList.end(), window->State.Parent) == m_NodesRenderingList.end()) || (_Rendered != nullptr && !(*_Rendered))) return false;

    // setup clipping
    if(!(window->State.Settings & ImmedidateUserInterfaceNodeSettings_IgnoreClipping))
        ui_node_render_clipbox(window);

    ui_node_render_background(window);

    // push window into hierarchy stack and draw list
    m_NodesRenderingList.push_back(window);
    m_NodesRenderingStack.push_back(window);

    return true;
}

void ImmedidateUserInterfaceContextLayer::end_node()
{
    if(m_NodesRenderingStack.empty())
        return;

    // update parent thickness
    if(ui_node_hierarchy_top()->State.Parent != nullptr)
        ui_node_hierarchy_top()->State.Parent->State.Thickness += ui_node_hierarchy_top()->State.Thickness;

    // render scrollbars and background frame
    if(ui_node_is_vertical_scroll_bar_needed(ui_node_hierarchy_top()))
        ui_node_render_vertical_scrollbar(ui_node_hierarchy_top());
    if(ui_node_is_horizontal_scroll_bar_needed(ui_node_hierarchy_top()))
        ui_node_render_horizontal_scrollbar(ui_node_hierarchy_top());
    ui_node_render_background_frame(ui_node_hierarchy_top());
        
    // remove clipping
    if(!(ui_node_hierarchy_top()->State.Settings & ImmedidateUserInterfaceNodeSettings_IgnoreClipping))
        m_Renderer->pop_clip_box();

    // render resize events gizmos
    ui_node_render_resize_events_gizmos(ui_node_hierarchy_top());

    // pop node out-of rendering stack
    m_NodesRenderingStack.pop_back();
}

bool ImmedidateUserInterfaceContextLayer::begin_window(const std::string& _Name, bool* _Rendered)
{
    auto render_window_close_button = [this](ImmedidateUserInterfaceNode* _Window)
    {
        gs_2dboxf _ButtonBox = gs_2dboxf(gs_vec2f(0.f), gs_vec2f(_Window->State.WindowMinimumHeight * 0.5f));
        
        gs_vec2f  _ButtonPos = gs_vec2f(
            _Window->State.WindowBox.Max.x - _ButtonBox.width() - m_Style->FrameWidth,
            _Window->State.WindowBox.center().y - _ButtonBox.height() * 0.5f);
        
        gs_mat4f  _Transform =
            _Window->State.Transform *
            m_Renderer->calculate_transform_matrix(
                ui_node_calculate_child_depth_placed_in_follow(ui_node_hierarchy_top(), 1),
                _ButtonPos);

        m_Renderer->push_arc_filled(
            _ButtonBox.center(),
            _ButtonBox.width()  * 0.5f,
            _ButtonBox.height() * 0.5f,
            0.f,
            360.f,
            gs_vec4f(225.f, 0.f, 0.f, 255.f),
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
    };

    auto begin_window_frame = [this, &render_window_close_button](
        const std::string&                         _Name,
        bool*                                      _Rendered,
        const ImmedidateUserInterfaceNodeSettings& _Settings)
    {
        if(begin_node(
            _Name,
            _Rendered,
            _Settings,
            ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowDefaultFrame))
        {
            ui_node_hierarchy_top()->State.WindowMinimumHeight = m_Style->FontSize + 2.f * m_Style->FrameWidth;
            ui_node_hierarchy_top()->State.WindowMaximumHeight = m_Style->FontSize + 2.f * m_Style->FrameWidth;

            // render frame background
            m_Renderer->push_rectangle_rounded_filled(
                ui_node_hierarchy_top()->State.WindowBox.Min,
                ui_node_hierarchy_top()->State.WindowBox.Max,
                m_Style->FrameRoundingRadius,
                gs_vec4f(128.f, 128.f, 128.f, 255.f),
                ui_node_hierarchy_top()->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(ui_node_hierarchy_top(), 1)));

            // render frame outline
            m_Renderer->push_rectangle_rounded(
                ui_node_hierarchy_top()->State.WindowBox.Min,
                ui_node_hierarchy_top()->State.WindowBox.Max,
                m_Style->FrameRoundingRadius,
                m_Style->FrameWidth,
                gs_vec4f(0.f, 255.f, 0.f, 255.f),
                ui_node_hierarchy_top()->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(ui_node_hierarchy_top(), 1)));

            // render title
            m_Renderer->push_text(
                _Name,
                m_Style->FontSize,
                gs_vec4f(255.f, 255.f, 255.f, 255.f),
                ui_node_hierarchy_top()->State.Transform * m_Renderer->calculate_transform_matrix(
                    (float)ui_node_calculate_child_depth_placed_in_follow(ui_node_hierarchy_top(), 1),
                    gs_vec2f(
                        ui_node_hierarchy_top()->State.WindowBox.Min.x + m_Style->FrameWidth * 2.f,
                        (ui_node_hierarchy_top()->State.WindowBox.center() - m_Renderer->calculate_bounding_box(_Name, m_Style->FontSize, m_Style->Font).size() * 0.5f).y)),
                m_Style->Font);

            render_window_close_button(ui_node_hierarchy_top());

            return true;
        }

        return false;
    };

    auto end_window_frame = [this]()
    {
        end_node();
    };

    // render window
    if(begin_node(
        _Name,
        _Rendered,
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_NeverScrollBar |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Resizable      |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable        |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenVertically,
        ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Window))
    {
        auto window = ui_node_hierarchy_top();

        if(begin_node(
            std::string(_Name).append("/Frame"),
            _Rendered,
            ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_NeverScrollBar               |
            ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally   |
            ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically))
        {
            if(begin_node(
                std::string(_Name).append("/Dockbox"),
                _Rendered,
                ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_NeverScrollBar               |
                ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally   |
                ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutSetAllChildrenSameSize |
                ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically))
            {
                ui_node_hierarchy_top()->State.WindowMinimumWidth = ui_node_hierarchy_top()->State.Parent->State.WindowBox.width() * 0.8f;
                ui_node_hierarchy_top()->State.WindowMaximumWidth = ui_node_hierarchy_top()->State.Parent->State.WindowBox.width() * 0.8f;

                // render docker frame
                if(!ui_node_is_being_docked(window))
                {
                    if(begin_window_frame(
                        _Name,
                        _Rendered,
                        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically))
                    {
                        if(ui_node_is_being_mouse_pressed(ui_node_hierarchy_top(), ApplicationMouseButton::Button::ApplicationMouseButton_Left)   ||
                            ui_node_is_being_mouse_pressed(ui_node_hierarchy_top(), ApplicationMouseButton::Button::ApplicationMouseButton_Right) ||
                            ui_node_is_being_mouse_pressed(ui_node_hierarchy_top(), ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
                        {
                            ui_node_enqueue_focused_node(window);
                            ui_node_enqueue_moving_node(window);
                        }
                        
                        end_window_frame();
                    }
                }

                // render docked nodes
                for (auto renderedWindow : m_NodesRenderingCache)
                {
                    if(renderedWindow->State.Docker == nullptr ||
                        renderedWindow->State.Docker != window ||
                        renderedWindow == window)
                        continue;

                    if(begin_window_frame(
                        renderedWindow->Name,
                        _Rendered,
                        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically |
                        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_NeverScrollBar             |
                        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_IgnoreFocus))
                    {
                        if(ui_node_is_being_mouse_pressed(ui_node_hierarchy_top(), ApplicationMouseButton::Button::ApplicationMouseButton_Left)   ||
                            ui_node_is_being_mouse_pressed(ui_node_hierarchy_top(), ApplicationMouseButton::Button::ApplicationMouseButton_Right) ||
                            ui_node_is_being_mouse_pressed(ui_node_hierarchy_top(), ApplicationMouseButton::Button::ApplicationMouseButton_Middle))
                        {
                            ui_node_enqueue_focused_node(renderedWindow);
                            ui_node_enqueue_moving_node(renderedWindow);
                        }

                        end_window_frame();
                    }
                }

                end_node();
            }

            if(!ui_node_is_being_docked(window))
            {
                if(begin_node(
                    std::string(_Name).append("/FrameButtons"),
                    _Rendered,
                    ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_NeverScrollBar             |
                    ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally |
                    ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically))
                {
                    ui_node_hierarchy_top()->State.WindowMinimumHeight = m_Style->FontSize + 2.f * m_Style->FrameWidth;
                    ui_node_hierarchy_top()->State.WindowMaximumHeight = m_Style->FontSize + 2.f * m_Style->FrameWidth;
                    end_node();
                }
            }

            end_node();
        }

        return true;
    }

    return false;
}

void ImmedidateUserInterfaceContextLayer::end_window()
{
    end_node();
}

bool ImmedidateUserInterfaceContextLayer::begin_menu(const std::string& _Name)
{
    ImmedidateUserInterfaceNodeSettings settings =
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_IgnoreFocus                  |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_NullParent;

    ImmedidateUserInterfaceNodeType type =
        ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowMenu;

    // TODO: identify, that we are trying to add something into a menu
    if(ui_node_is_of_type(ui_node_hierarchy_top(), ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowMenu) ||
        ui_node_is_of_type(ui_node_hierarchy_top(), ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowMenubar))
    {
        if(ui_node_is_of_type(
            ui_node_hierarchy_top(),
            ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowMenubar))
        {
            set_next_ui_node_cursor_same_line();
        }

        widget_push_button(_Name);

        ImmedidateUserInterfaceNode* cachedMenu =
            ui_node_cache_request(_Name, ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowMenu);

        if(widget_is_being_hovered())
            ui_node_begin_hover(cachedMenu);
        
        // check self hover
        bool isHovered =
            widget_is_being_hovered() || ui_node_is_being_hovered(cachedMenu);

        if(isHovered)
        {
            m_ActiveMenusRenderingList.push_back(
                {
                    cachedMenu,
                    ui_node_hierarchy_top()
                }
            );
        }
        else
        {
            bool anyHovered = false;

            for (auto& window : m_ActiveMenusRenderingList)
            {
                if((window.Self->State.Type & ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowMenu))
                {
                    anyHovered = anyHovered || ui_node_is_being_hovered(window.Self);

                    if(window.Observable == cachedMenu)
                        isHovered = true;
                }
            }

            if(!anyHovered)
                m_ActiveMenusRenderingList.clear();
        }

        // check that creator window has been focused
        if(cachedMenu != nullptr && isHovered)
        {
            auto next    = cachedMenu->Cache.Creator;
            auto creator = next;

            while (next)
            {
                creator = next;
                next    = next->Cache.Creator;
            }

            if(creator != nullptr)
                isHovered = isHovered && ui_node_is_being_focused(creator);
        }

        if(!isHovered)
        {
            for(auto it = m_ActiveMenusRenderingList.begin(); it != m_ActiveMenusRenderingList.end(); ++it)
            {
                if(it->Self == cachedMenu)
                {
                    auto rm = it;
                    it++;
                    m_ActiveMenusRenderingList.erase(rm);

                    if(it == m_ActiveMenusRenderingList.end())
                        break;
                }
            }

            return false;
        }

        if(ui_node_is_of_type(
            ui_node_hierarchy_top(),
            ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowMenubar))
        {
            set_next_ui_node_position(gs_vec2f(
                ui_node_hierarchy_top()->State.ScrollAreaBox.transform(ui_node_hierarchy_top()->State.Transform).Min.x + ui_node_hierarchy_top()->State.LayoutCursorPositon.x,
                ui_node_hierarchy_top()->State.ScrollAreaBox.transform(ui_node_hierarchy_top()->State.Transform).Min.y + ui_node_hierarchy_top()->State.LayoutCursorSize.y));
        }
        else
        {
            set_next_ui_node_position(
                gs_vec2f(
                    ui_node_hierarchy_top()->State.ScrollAreaBox.transform(ui_node_hierarchy_top()->State.Transform).Max.x + ui_node_hierarchy_top()->State.ScrollBarOffset.x,
                    ui_node_hierarchy_top()->State.ScrollAreaBox.transform(ui_node_hierarchy_top()->State.Transform).Min.y + ui_node_hierarchy_top()->State.LayoutCursorPositon.y));
        }
    }

    return begin_node(_Name, nullptr, settings, type);
}

void ImmedidateUserInterfaceContextLayer::end_menu()
{
    end_node();
}

bool ImmedidateUserInterfaceContextLayer::begin_menubar(const std::string& _Name)
{
    ImmedidateUserInterfaceNodeSettings settings =
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically |
        ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_IgnoreFocus;

    ImmedidateUserInterfaceNodeType     type =
        ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowMenubar;

    if(begin_node(std::string(_Name).append("Menubar"), nullptr, settings, type))
    {
        ui_node_hierarchy_top()->State.WindowMinimumHeight = m_Style->FontSize + m_Style->FrameWidth * 2.f + ui_node_hierarchy_top()->State.ScrollBarOffset.y;
        ui_node_hierarchy_top()->State.WindowMaximumHeight = m_Style->FontSize + m_Style->FrameWidth * 2.f + ui_node_hierarchy_top()->State.ScrollBarOffset.y;
        return true;
    }

    return false;
}

void ImmedidateUserInterfaceContextLayer::end_menubar()
{
    end_node();
}

bool ImmedidateUserInterfaceContextLayer::widget_push_button(const std::string& _Text)
{
    if(m_NodesRenderingStack.empty())
        return false;
    
    // prepare for rendering
    auto widgetData  = widget_prepare_for_rendering(_Text);
    auto boundingBox = widgetData.BoundingBox;
    auto context     = widgetData.Node;

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

    bool hovered = widget_is_being_hovered();
    bool pressed = widget_is_being_mouse_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left)  ||
                   widget_is_being_mouse_down(ApplicationMouseButton::Button::ApplicationMouseButton_Right) ||
                   widget_is_being_mouse_down(ApplicationMouseButton::Button::ApplicationMouseButton_Middle);

    m_Renderer->push_rectangle_rounded_filled(
        boundingBox.Min,
        boundingBox.Max,
        m_Style->FrameRoundingRadius,
        retreive_push_button_background_color(true, hovered, pressed),
        context->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(context, 1)));

    m_Renderer->push_rectangle_rounded(
        boundingBox.Min,
        boundingBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        retreive_push_frame_color(true, hovered, pressed),
        context->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(context, 1)));

    m_Renderer->push_text(
        _Text,
        m_Style->FontSize,
        gs_vec4f(255.f, 255.f, 255.f, 255.f),
        context->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(context, 1), boundingBox.center() - textBox.size() * 0.5f),
        m_Style->Font);

    return pressed;
}

bool ImmedidateUserInterfaceContextLayer::widget_menu_button(const std::string& _Name)
{
    return widget_push_button(_Name);
}

void ImmedidateUserInterfaceContextLayer::ui_node_calculate_geometry(ImmedidateUserInterfaceNode* _Window) const
{
    if(_Window == nullptr) return;

    // window box
    _Window->State.ScrollBarOffset = gs_vec2f(
        ui_node_is_vertical_scroll_bar_needed(_Window)   ? m_Style->WindowScrollBarSliderWidth : 0.f,
        ui_node_is_horizontal_scroll_bar_needed(_Window) ? m_Style->WindowScrollBarSliderWidth : 0.f);

    if((_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally) &&
        (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically))
    {
        _Window->State.WindowBox = gs_2dboxf(
            _Window->State.WindowBox.Min,
            _Window->State.WindowBox.Min +
                gs_clamp(
                    _Window->Cache.ContentBox.size() + _Window->State.ScrollBarOffset,
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
                    gs_vec2f(_Window->Cache.ContentBox.width(), _Window->State.WindowBox.height()) + _Window->State.ScrollBarOffset,
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
                    gs_vec2f(_Window->State.WindowBox.width(), _Window->Cache.ContentBox.height()) + _Window->State.ScrollBarOffset,
                    gs_vec2f(_Window->State.WindowMinimumWidth, _Window->State.WindowMinimumHeight),
                    gs_vec2f(_Window->State.WindowMaximumWidth, _Window->State.WindowMaximumHeight)));

        _Window->State.VerticalScrollBar.SliderPosition = 0.f;
    }

    // viewport
    {
        _Window->State.ViewportBox = gs_2dboxf(
            _Window->State.WindowBox.Min,
            _Window->State.WindowBox.Max - _Window->State.ScrollBarOffset);
    }

    // vertical scrollbar
    {
        _Window->State.VerticalScrollBar.setup(
            0.f,
            _Window->State.ViewportBox.height(),
            _Window->Cache.ContentBox.height(),
            _Window->State.ViewportBox.height());

        if(!ui_node_is_vertical_scroll_bar_needed(_Window))
            _Window->State.VerticalScrollBar.SliderPosition = 0.f;

        _Window->State.VerticalScrollBar.reposition(_Window->State.VerticalScrollBar.SliderPosition);

        _Window->State.VerticalScrollBarBox = gs_2dboxf(
            gs_vec2f(_Window->State.ViewportBox.Max.x, _Window->State.ViewportBox.Min.y),
            gs_vec2f(_Window->State.ViewportBox.Max.x, _Window->State.ViewportBox.Max.y) + gs_vec2f(m_Style->WindowScrollBarSliderWidth, 0.f));

        _Window->State.VerticalScrollBarSliderBox = gs_2dboxf(
            _Window->State.VerticalScrollBarBox.Min + gs_vec2f(0.f, _Window->State.VerticalScrollBar.SliderPosition),
            gs_vec2f(_Window->State.VerticalScrollBarBox.Max.x, _Window->State.VerticalScrollBarBox.Min.y + _Window->State.VerticalScrollBar.SliderLength + _Window->State.VerticalScrollBar.SliderPosition));
    }

    // horizontal scrollbar
    {
        _Window->State.HorizontalScrollBar.setup(
            0.f,
            _Window->State.ViewportBox.width(),
            _Window->Cache.ContentBox.width(),
            _Window->State.ViewportBox.width());

        if(!ui_node_is_horizontal_scroll_bar_needed(_Window))
            _Window->State.HorizontalScrollBar.SliderPosition = 0.f;

        _Window->State.HorizontalScrollBar.reposition(_Window->State.HorizontalScrollBar.SliderPosition);

        _Window->State.HorizontalScrollBarBox = gs_2dboxf(
            gs_vec2f(_Window->State.ViewportBox.Min.x, _Window->State.ViewportBox.Max.y),
            gs_vec2f(_Window->State.ViewportBox.Max.x, _Window->State.ViewportBox.Max.y) + gs_vec2f(0.f, m_Style->WindowScrollBarSliderWidth));

        _Window->State.HorizontalScrollBarSliderBox = gs_2dboxf(
            _Window->State.HorizontalScrollBarBox.Min + gs_vec2f(_Window->State.HorizontalScrollBar.SliderPosition, 0.f),
            gs_vec2f(_Window->State.HorizontalScrollBarBox.Min.x + _Window->State.HorizontalScrollBar.SliderLength + _Window->State.HorizontalScrollBar.SliderPosition, _Window->State.HorizontalScrollBarBox.Max.y));
    }
    
    // scroll area
    {
        _Window->State.ScrollAreaBox  = gs_2dboxf(
            _Window->State.ViewportBox.Min,
            _Window->State.ViewportBox.Max,
            _Window->State.ViewportBox.Min + _Window->State.LayoutCursorPositon + _Window->State.LayoutCursorSize );

        _Window->State.ScrollAreaBox.Min += gs_vec2f(
            -_Window->State.HorizontalScrollBar.SliderPosition * _Window->State.HorizontalScrollBar.SliderScale,
            -_Window->State.VerticalScrollBar.SliderPosition * _Window->State.VerticalScrollBar.SliderScale);
        
        _Window->State.ScrollAreaBox.Max += gs_vec2f(
            -_Window->State.HorizontalScrollBar.SliderPosition * _Window->State.HorizontalScrollBar.SliderScale,
            -_Window->State.VerticalScrollBar.SliderPosition * _Window->State.VerticalScrollBar.SliderScale);
    }

    // content box
    {
        // self content bounding box
        _Window->State.ContentBox  = gs_2dboxf(
            _Window->State.ScrollAreaBox.Min,
            _Window->State.ScrollAreaBox.Min + _Window->State.ContentBox.size(),
            _Window->State.ScrollAreaBox.Min + _Window->State.LayoutCursorPositon + _Window->State.LayoutCursorSize);
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

            _Window->State.InnerClipAreaBox =
                gs_2dboxf(
                    _Window->State.ViewportBox.transform(_Window->State.Transform).Min - m_Style->FrameWidth,
                    _Window->State.ViewportBox.transform(_Window->State.Transform).Max + _Window->State.ScrollBarOffset + m_Style->FrameWidth * 0.5f);

            _Window->State.OuterClipAreaBox = next->State.InnerClipAreaBox; // this clipbox is already transformed to parent coordinates
        }
        else
        {
            gs_vec2f outerBorder =
                gs_vec2f(m_Style->WindowResizeAngleGizmoRadius, m_Style->WindowResizeAngleGizmoRadius);

            _Window->State.InnerClipAreaBox =
                gs_2dboxf(
                    _Window->State.ViewportBox.transform(_Window->State.Transform).Min - m_Style->FrameWidth,
                    _Window->State.ViewportBox.transform(_Window->State.Transform).Max + _Window->State.ScrollBarOffset + m_Style->FrameWidth * 0.5f);

            _Window->State.OuterClipAreaBox =
                gs_2dboxf(
                    _Window->State.WindowBox.transform(_Window->State.Transform).Min - outerBorder,
                    _Window->State.WindowBox.transform(_Window->State.Transform).Max + outerBorder);
        }
    }

    // dockarea
    {
        _Window->State.DockArea =
            gs_2dboxf(
                _Window->State.WindowBox.Min + gs_vec2f(0.f, m_Style->FontSize),
                _Window->State.WindowBox.Max);
    }

    ui_node_calculate_geometry(_Window->State.Parent);
}

int ImmedidateUserInterfaceContextLayer::ui_node_calculate_layer_depth(const ImmedidateUserInterfaceNodeLayer& _Layer) const
{
    float depth = 0.f;

    for (int layer = ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_Begin;
             layer < ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_End; ++layer)
    {
        m_RendererLayers[(ImmedidateUserInterfaceNodeLayer_)layer] = (int)depth;
        depth += m_Renderer->get_far_plane() / (ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_End - ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_Begin);
    }

    return m_RendererLayers[_Layer];
}

int ImmedidateUserInterfaceContextLayer::ui_node_calculate_child_depth_placed_in_follow(ImmedidateUserInterfaceNode* _Window, const int& _Thickness) const
{
    if(_Window == nullptr) return 0;
    
    _Window->State.Thickness += _Thickness;
    return _Window->State.LayerDepth + _Window->State.Thickness;
}

gs_vec2f ImmedidateUserInterfaceContextLayer::ui_node_vertical_cursor_direction() const
{
    return gs_vec2f(0.f, 1.f);
}

gs_vec2f ImmedidateUserInterfaceContextLayer::ui_node_horizontal_cursor_direction() const
{
    return gs_vec2f(1.f, 0.f);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_of_type(const ImmedidateUserInterfaceNode* _Node, const ImmedidateUserInterfaceNodeType& _Type) const
{
    return _Node != nullptr && (_Node->State.Type == _Type);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_vertical_scroll_bar_needed(const ImmedidateUserInterfaceNode* _Window) const
{
    if(_Window == nullptr) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_NeverVerticalScrollBar)     return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_AlwaysVerticalScrollBar)    return true;

    return _Window->Cache.ContentBox.height() > _Window->Cache.ViewportBox.height() &&
            gs_abs(_Window->Cache.ViewportBox.height() - _Window->Cache.ContentBox.height()) > _Window->Cache.ViewportBox.height() * 0.1f;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_horizontal_scroll_bar_needed(const ImmedidateUserInterfaceNode* _Window) const
{
    if(_Window == nullptr) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_NeverHorizontalScrollBar)     return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally) return false;
    if (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar)    return true;

    return _Window->Cache.ContentBox.width() > _Window->Cache.ViewportBox.width() &&
            gs_abs(_Window->Cache.ViewportBox.width() - _Window->Cache.ContentBox.width()) > _Window->Cache.ViewportBox.width() * 0.1f;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_begin_attaching_to_a_docker(const ImmedidateUserInterfaceNode* _Node, ImmedidateUserInterfaceNode* _Docker)
{
    if(_Node == nullptr || _Docker == nullptr)
        return false;

    _Node->State.Docker = _Docker;
    ++_Docker->State.DockNodesCount;

    return true;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_docked_by(const ImmedidateUserInterfaceNode* _Node, ImmedidateUserInterfaceNode* _Docker)
{
    auto docker = _Docker;

    while (docker)
    {
        if(docker == _Node->State.Docker)
            return true;
        docker = docker->State.Docker;
    }

    return false;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_docker(const ImmedidateUserInterfaceNode* _Node)
{
    return _Node != nullptr && _Node->State.DockNodesCount > 0;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_docked(const ImmedidateUserInterfaceNode* _Node)
{
    return _Node != nullptr && _Node->State.Docker != nullptr;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_end_attaching_to_a_docker(const ImmedidateUserInterfaceNode* _Node)
{
    if(_Node == nullptr || _Node->State.Docker == nullptr)
        return false;

    _Node->State.Docker->State.DockNodesCount = gs_max(++_Node->State.Docker->State.DockNodesCount - 1, 0);
    _Node->State.Docker                       = nullptr;

    return true;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_hovered(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
            (_Window->Cache.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHovered);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_mouse_down(const ImmedidateUserInterfaceNode* _Window, const ApplicationMouseButton::Button& _Button) const
{    
    return _Window != nullptr && _Window->Cache.MouseDown.has_value() && _Window->Cache.MouseDown.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_mouse_pressed(const ImmedidateUserInterfaceNode* _Window, const ApplicationMouseButton::Button& _Button) const
{
    return _Window != nullptr && _Window->Cache.MousePressed.has_value() && _Window->Cache.MousePressed.value() == _Button; 
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_mouse_clicked(const ImmedidateUserInterfaceNode* _Window, const ApplicationMouseButton::Button& _Button) const
{
    return _Window != nullptr && _Window->Cache.MouseClicked.has_value() && _Window->Cache.MouseClicked.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_mouse_double_clicked(const ImmedidateUserInterfaceNode* _Window, const ApplicationMouseButton::Button& _Button) const
{
    return _Window != nullptr && _Window->Cache.MouseDoubleClicked.has_value() && _Window->Cache.MouseDoubleClicked.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_resized_top_left(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
        (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_resized_top_right(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
            (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_resized_bottom_left(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
            (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_resized_bottom_right(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_resized_top(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_resized_left(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_resized_right(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_resized_bottom(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr &&
          (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_resized(const ImmedidateUserInterfaceNode* _Window) const
{
    return ui_node_is_being_resized_top_left(_Window)      ||
            ui_node_is_being_resized_top_right(_Window)    ||
            ui_node_is_being_resized_bottom_left(_Window)  ||
            ui_node_is_being_resized_bottom_right(_Window) ||
            ui_node_is_being_resized_top(_Window)          ||
            ui_node_is_being_resized_left(_Window)         ||
            ui_node_is_being_resized_right(_Window)        ||
            ui_node_is_being_resized_bottom(_Window);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_moved(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMoved);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_modified(const ImmedidateUserInterfaceNode* _Window) const
{
    return ui_node_is_being_moved(_Window) || ui_node_is_being_resized(_Window) || ui_node_is_being_scrolled(_Window);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_focused(const ImmedidateUserInterfaceNode* _Window) const
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

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_scrolled_vertically(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_scrolled_horizontally(const ImmedidateUserInterfaceNode* _Window) const
{
    return _Window != nullptr && (_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_is_being_scrolled(const ImmedidateUserInterfaceNode* _Window) const
{
    return ui_node_is_being_scrolled_vertically(_Window) || ui_node_is_being_scrolled_horizontally(_Window);
}

//------------------------------------------------------------------------------------------------------------------------
// event queue
//------------------------------------------------------------------------------------------------------------------------
bool ImmedidateUserInterfaceContextLayer::ui_node_event_has_been_enqueued(const ImmedidateUserInterfaceNodeChanges& _Event)
{
    bool enqueued = false;
    for (auto event : m_EventsQueue)
        enqueued = enqueued || ((event.first & _Event) && event.second.has_value() && event.second.value() != nullptr);
    return enqueued;
    
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_focused_node(ImmedidateUserInterfaceNode* _Window)
{
    if (_Window == nullptr || (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_IgnoreFocus))
        return;
    
    // pass focus to _Window
    auto parent = _Window->State.Parent;
    auto window = _Window;

    while (parent != nullptr)
    {
        window = parent;
        parent = parent->State.Parent;
    }

    m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingFocused] = window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_focused_node()
{
    auto ui_node_begin_focus = [this](ImmedidateUserInterfaceNode* _Window)
    {
        if (_Window == nullptr || (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_IgnoreFocus))
            return;

        if(!(_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_IgnoreFocus))
            _Window->State.Changes |= ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingFocused;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingFocused];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_focus(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_resize_top_left_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_resize_top_left_node()
{
    auto ui_node_begin_resize_top_left = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr                                                                                                                ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;

        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_resize_top_left(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_resize_top_right_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_resize_top_right_node()
{
    auto ui_node_begin_resize_top_right = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr                                                                                                                ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;
        
        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_resize_top_right(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_resize_bottom_left_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_resize_bottom_left_node()
{
    auto ui_node_begin_resize_bottom_left = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr                                                                                                                ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;
        
        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_resize_bottom_left(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_resize_bottom_right_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_resize_bottom_right_node()
{
    auto ui_node_begin_resize_bottom_right = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr                                                                                                                ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;
        
        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_resize_bottom_right(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_resize_top_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_resize_top_node()
{
    auto ui_node_begin_resize_top = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically)) return;
        
        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_resize_top(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_resize_left_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_resize_left_node()
{
    auto ui_node_begin_resize_left = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;

        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_resize_left(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_resize_right_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_resize_right_node()
{
    auto ui_node_begin_resize_right = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally)) return;
        
        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_resize_right(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_resize_bottom_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_resize_bottom_node()
{
    auto ui_node_begin_resize_bottom = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr ||
            (_Window->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically)) return;
        
        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_resize_bottom(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_moving_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMoved] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_moving_node()
{
    auto ui_node_begin_move = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr) return;
        
        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMoved;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMoved];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_move(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_vertical_scroll_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_vertical_scroll_node()
{
    auto ui_node_begin_scroll_vertically = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr) return;
        
        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_scroll_vertically(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_enqueue_horizontal_scroll_node(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window != nullptr)
        m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally] = _Window;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_dequeue_horizontal_scroll_node()
{
    auto ui_node_begin_scroll_horizontally = [](ImmedidateUserInterfaceNode* _Window)
    {
        if(_Window == nullptr) return;
        
        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally;
    };

    auto& event = m_EventsQueue[ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally];

    if (event.has_value() && event.value() != nullptr)
    {
        ui_node_begin_scroll_horizontally(event.value());
        ui_node_event_dequeue_finish(event.value());
        event.reset();
        return true;
    }
    
    return false;
}

void ImmedidateUserInterfaceContextLayer::ui_node_event_dequeue_finish(ImmedidateUserInterfaceNode* _Sink)
{
    if(_Sink == nullptr)
        return;

    for (auto& window : m_NodesRenderingList)
    {
        if(window == _Sink)
            continue;

        ui_node_end_move(window);
        ui_node_end_resize(window);
        ui_node_end_scroll(window);
        ui_node_end_focus(window);
    }
};

//-----

void ImmedidateUserInterfaceContextLayer::ui_node_begin_hover(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;

    // launch timer
    if(!(_Window->Cache.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHoveredStarted))
    {
        _Window->State.WindowHoverStart = Frenchie::Core::tic();

        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHoveredStarted;
    }
    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(_Window->State.WindowHoverStart, Frenchie::Core::tic()) > 5.0) // TODO: this MUST BE A SETTING !!!
    {
        for (auto window : m_NodesRenderingCache)
            ui_node_end_hover(window);

        _Window->State.Changes |=
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHovered;
    }
};

void ImmedidateUserInterfaceContextLayer::ui_node_end_hover(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;

    if(!(_Window->State.Changes & ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHoveredEnded))
    {
        _Window->State.WindowHoverEnd = Frenchie::Core::tic();

        _Window->State.Changes |= ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHoveredEnded;
    }
    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(_Window->State.WindowHoverEnd, Frenchie::Core::tic()) > 200.f) // TODO: this MUST BE A SETTING !!!!
    {
        _Window->State.Changes &= ~(
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHovered        |
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHoveredStarted |
            ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMouseHoveredEnded);
    }
}

void ImmedidateUserInterfaceContextLayer::ui_node_end_resize(ImmedidateUserInterfaceNode* _Window)
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

void ImmedidateUserInterfaceContextLayer::ui_node_end_move(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes &=
        ~ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMoved;
}

void ImmedidateUserInterfaceContextLayer::ui_node_end_focus(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;
    
    _Window->State.Changes &=
        ~ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingFocused;
}

void ImmedidateUserInterfaceContextLayer::ui_node_end_scroll(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return;

    _Window->State.Changes &= ~(
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically |
        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally);
}

bool ImmedidateUserInterfaceContextLayer::ui_node_render_clipbox(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    m_Renderer->push_clip_box(
        _Window->State.InnerClipAreaBox.clip_with(_Window->State.OuterClipAreaBox));

    return true;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_render_background(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.WindowBox.Min,
        _Window->State.WindowBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceColor],
        _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));

    return true;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_render_background_frame(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    m_Renderer->push_rectangle_rounded(
        _Window->State.WindowBox.Min,
        _Window->State.WindowBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));

    // m_Renderer->push_rectangle_rounded(
    //     _Window->Cache.ViewportBox.Min,
    //     _Window->Cache.ViewportBox.Max,
    //     m_Style->FrameRoundingRadius,
    //     m_Style->FrameWidth,
    //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
    //     _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));

    return true;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_render_vertical_scrollbar(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    // draw scroll panel
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.VerticalScrollBarBox.Min,
        _Window->State.VerticalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarColor],
        _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));

    // draw slider
    if(_Window->State.VerticalScrollBarSliderBox.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()))
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.VerticalScrollBarSliderBox.Min,
            _Window->State.VerticalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left) ?
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderPressedColor] :
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderHoveredColor],
            _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
    }
    else
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.VerticalScrollBarSliderBox.Min,
            _Window->State.VerticalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderColor],
            _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
    }
    
    // draw frame
    m_Renderer->push_rectangle_rounded(
        _Window->State.VerticalScrollBarBox.Min,
        _Window->State.VerticalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
    
    return true;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_render_horizontal_scrollbar(ImmedidateUserInterfaceNode* _Window)
{
    if(_Window == nullptr) return false;

    // draw scroll panel
    m_Renderer->push_rectangle_rounded_filled(
        _Window->State.HorizontalScrollBarBox.Min,
        _Window->State.HorizontalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarColor],
        _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));

    // draw slider
    if(_Window->State.HorizontalScrollBarSliderBox.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()))
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.HorizontalScrollBarSliderBox.Min,
            _Window->State.HorizontalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            application()->is_mouse_button_down(ApplicationMouseButton::Button::ApplicationMouseButton_Left) ?
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderPressedColor] :
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderHoveredColor],
            _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
    }
    else
    {
        m_Renderer->push_rectangle_rounded_filled(
            _Window->State.HorizontalScrollBarSliderBox.Min,
            _Window->State.HorizontalScrollBarSliderBox.Max,
            m_Style->FrameRoundingRadius,
            m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderColor],
            _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
    }

    // draw frame
    m_Renderer->push_rectangle_rounded(
        _Window->State.HorizontalScrollBarBox.Min,
        _Window->State.HorizontalScrollBarBox.Max,
        m_Style->FrameRoundingRadius,
        m_Style->FrameWidth,
        m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor],
        _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
    
    return true;
}

bool ImmedidateUserInterfaceContextLayer::ui_node_render_resize_events_gizmos(ImmedidateUserInterfaceNode* _Window)
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
        if((resizeTopLeft.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()) || ui_node_is_being_resized_top_left(_Window)) && !ui_node_is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeTopLeft.Center,
                resizeTopLeft.Radius,
                resizeTopLeft.Radius,
                0.f,
                360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
        }
        else if((resizeTopRight.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()) || ui_node_is_being_resized_top_right(_Window)) && !ui_node_is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeTopRight.Center,
                resizeTopRight.Radius,
                resizeTopRight.Radius,
                0.f,
                360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
        }
        else if((resizeBottomLeft.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()) || ui_node_is_being_resized_bottom_left(_Window)) && !ui_node_is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeBottomLeft.Center,
                resizeBottomLeft.Radius,
                resizeBottomLeft.Radius,
                0.f, 360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
        }
        else if((resizeBottomRight.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()) || ui_node_is_being_resized_bottom_right(_Window)) && !ui_node_is_being_moved(_Window))
        {
            m_Renderer->push_arc_filled(
                resizeBottomRight.Center,
                resizeBottomRight.Radius,
                resizeBottomRight.Radius,
                0.f,
                360.f,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
        }
        else if((resizeTop.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()) || ui_node_is_being_resized_top(_Window)) && !ui_node_is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeTop.Min,
                resizeTop.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
        }
        else if((resizeLeft.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()) || ui_node_is_being_resized_left(_Window)) && !ui_node_is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeLeft.Min,
                resizeLeft.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
        }
        else if((resizeRight.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()) || ui_node_is_being_resized_right(_Window)) && !ui_node_is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeRight.Min,
                resizeRight.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
        }
        else if((resizeBottom.transform(_Window->State.Transform).contains(m_Renderer->get_cursor_postion()) || ui_node_is_being_resized_bottom(_Window)) && !ui_node_is_being_moved(_Window))
        {
            m_Renderer->push_rectangle_rounded_filled(
                resizeBottom.Min,
                resizeBottom.Max,
                m_Style->FrameRoundingRadius,
                m_Style->Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor],
                _Window->State.Transform * m_Renderer->calculate_transform_matrix((float)ui_node_calculate_child_depth_placed_in_follow(_Window, 1)));
        }
    }

    return true;
}

void ImmedidateUserInterfaceContextLayer::ui_node_layout_children()
{
    // build hierarchy
    //static bool inited = false;

    m_Hierarchy.build(m_NodesRenderingList);

    // if(!inited)
    // {
    //     for (auto& window : m_NodesRenderingList)
    //     {
    //         std::cout << window->Name << "\n";

    //         auto begin = m_Hierarchy.Indexes[window->State.DrawIndex];
    //         auto end   = m_Hierarchy.Indexes[window->State.DrawIndex + 1];

    //         for (int i = begin; i < end; i++)
    //         {
    //             std::cout << "\t" << m_Hierarchy.Sorted[i]->Name << "\n";
    //         }
    //     }
    // }

    // inited = true;
    
    // layering
    for (auto& window : m_NodesRenderingList)
    {
        if(window->State.Type == ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_WindowMenu)
            window->State.Layer = ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_Popups;
        else if(ui_node_is_being_focused(window))
            window->State.Layer = ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_Focus;
        else if(ui_node_is_being_docked(window))
            window->State.Layer = ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_Docking;
        else
            window->State.Layer = ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_Main;
    }

    // layouting
    for (auto& window : m_NodesRenderingList)
    {
        if(window->State.Settings & ImmedidateUserInterfaceNodeSettings_NullParent)
        {
            window->State.WindowBox = gs_2dboxf(
                window->State.WindowBox.Min,
                window->State.WindowBox.Min + gs_clamp(
                    window->State.WindowBox.size(),
                    gs_vec2f(window->State.WindowMinimumWidth, window->State.WindowMinimumHeight),
                    gs_vec2f(window->State.WindowMaximumWidth, window->State.WindowMaximumHeight)));

            ui_node_calculate_geometry(window);

            continue;
        }
        
        ImmedidateUserInterfaceNode* parent = window->State.Parent;

        if(parent != nullptr &&
           ((parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally) ||
           (parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenVertically)))
        {
            gs_vec2f size =
                (parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutSetAllChildrenSameSize) ?
                parent->State.ViewportBox.size() / (float)parent->State.ChildCount :
                (window->State.WindowBox.size() / parent->State.LayoutTotalChildrenSize) * parent->State.WindowBox.size();
            
            if((parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally))
                size = gs_vec2f(size.x, parent->State.ViewportBox.height());
            else if((parent->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_LayoutChildrenVertically))
                size = gs_vec2f(parent->State.ViewportBox.width(), size.y);

            window->State.WindowBox = gs_2dboxf(
                window->State.WindowBox.Min,
                window->State.WindowBox.Min + gs_clamp(
                    size,
                    gs_vec2f(window->State.WindowMinimumWidth, window->State.WindowMinimumHeight),
                    gs_vec2f(window->State.WindowMaximumWidth, window->State.WindowMaximumHeight)));

            ui_node_calculate_geometry(window);
        }
    }

    // docking
    for (auto& window : m_NodesRenderingList)
    {
        if(window->State.Docker == nullptr)
            continue;

        window->State.WindowBox = window->State.Docker->State.DockArea;
        ui_node_calculate_geometry(window);
    }
}

void ImmedidateUserInterfaceContextLayer::ui_node_receive_events()
{
    if(m_NodesRenderingList.empty())
        return;

    class ImmedidateUserInterfaceContextEventsReceiver
    {
    public:

        static bool receive_hover_event(ImmedidateUserInterfaceContextLayer* _Context, bool (*_Filter)(ImmedidateUserInterfaceNode*) = nullptr)
        {
            if(_Context == nullptr)
                return true;

            // find the top most hovered node
            ImmedidateUserInterfaceNode* hoveredNode = nullptr;

            for (auto next : _Context->m_NodesRenderingList)
            {
                _Context->ui_node_end_hover(next);
                
                if(_Filter != nullptr && !_Filter(next))
                    continue;

                if(next->State.WindowBox.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()))
                {
                    if(hoveredNode == nullptr || next->State.LayerDepth > hoveredNode->State.LayerDepth)
                        hoveredNode = next;
                }
            }

            if(hoveredNode == nullptr)
                return false;

            _Context->HoveredName = hoveredNode->Name;
            _Context->ui_node_begin_hover(hoveredNode);

            // render node hover gizmo
            _Context->m_Renderer->push_rectangle_rounded(
                hoveredNode->State.WindowBox.Min,
                hoveredNode->State.WindowBox.Max,
                0.f,
                12.f,
                gs_vec4f(0.f, 255.f, 0.f, 255.f),
                hoveredNode->State.Transform * _Context->m_Renderer->calculate_transform_matrix(_Context->m_Renderer->get_far_plane()));

            return true;
        }

        static bool receive_widgets_events(ImmedidateUserInterfaceContextLayer* _Context)
        {
            if(_Context == nullptr)
                return false;

            for (int button = ApplicationMouseButton::ApplicationMouseButton_Begin;
                     button < ApplicationMouseButton::ApplicationMouseButton_End;
                     button++)
            {
                if(_Context->widget_has_been_mouse_down((ApplicationMouseButton::Button)button) ||
                    _Context->widget_has_been_pressed((ApplicationMouseButton::Button)button) ||
                    _Context->widget_has_been_clicked((ApplicationMouseButton::Button)button) ||
                    _Context->widget_has_been_double_clicked((ApplicationMouseButton::Button)button))
                {
                    return true;
                }
            }

            return false;
        }

        static bool receive_focus_event(ImmedidateUserInterfaceContextLayer* _Context, bool (*_Filter)(ImmedidateUserInterfaceNode*) = nullptr)
        {
            if(_Context == nullptr)
                return false;

            // check if any node has been queued to be focused
            if(_Context->ui_node_dequeue_focused_node())
                return true;

            // pass focus to currently top most pressed and hovered node
            for (int button = ApplicationMouseButton::ApplicationMouseButton_Begin;
                     button < ApplicationMouseButton::ApplicationMouseButton_End;
                     button++)
            {
                if(!application()->is_mouse_button_pressed((ApplicationMouseButton::Button)button))
                    continue;

                for (auto next : _Context->m_NodesRenderingList)
                {
                    if(!_Context->ui_node_is_being_hovered(next))
                        continue;

                    _Context->ui_node_enqueue_focused_node(next);
                    return true;
                }
            }

            return false;
        }

        static bool receive_mouse_events(ImmedidateUserInterfaceContextLayer* _Context, bool (*_Filter)(ImmedidateUserInterfaceNode*) = nullptr)
        {
            if(_Context == nullptr)
                return false;

            for (auto next : _Context->m_NodesRenderingList)
            {
                if((_Filter != nullptr && !_Filter(next)) ||
                    !_Context->ui_node_is_being_hovered(next))
                    continue;

                for (int button = ApplicationMouseButton::ApplicationMouseButton_Begin;
                        button < ApplicationMouseButton::ApplicationMouseButton_End;
                        button++)
                {
                    if(application()->is_mouse_button_down((ApplicationMouseButton::Button)button))
                        next->State.MouseDown = (ApplicationMouseButton::Button)button;

                    if(application()->is_mouse_button_pressed((ApplicationMouseButton::Button)button))
                        next->State.MousePressed = (ApplicationMouseButton::Button)button;

                    if(application()->is_mouse_button_clicked((ApplicationMouseButton::Button)button))
                        next->State.MouseClicked = (ApplicationMouseButton::Button)button;

                    if(application()->is_mouse_button_double_clicked((ApplicationMouseButton::Button)button))
                        next->State.MouseDoubleClicked = (ApplicationMouseButton::Button)button;
                }
            }

            return true;
        }

        static bool receive_resize_event(ImmedidateUserInterfaceContextLayer* _Context, bool (*_Filter)(ImmedidateUserInterfaceNode*) = nullptr)
        {
            if(_Context == nullptr)
                return false;

            // dequeue
            if(_Context->ui_node_dequeue_resize_top_left_node()      ||
                _Context->ui_node_dequeue_resize_top_right_node()    ||
                _Context->ui_node_dequeue_resize_bottom_left_node()  ||
                _Context->ui_node_dequeue_resize_bottom_right_node() ||
                _Context->ui_node_dequeue_resize_top_node()          ||
                _Context->ui_node_dequeue_resize_left_node()         ||
                _Context->ui_node_dequeue_resize_right_node()        ||
                _Context->ui_node_dequeue_resize_bottom_node())
            {
                return true;
            }

            // check if anything already is being resized
            for (auto next : _Context->m_NodesRenderingList)
            {
                if(_Context->ui_node_is_being_resized(next))
                    return true;
            }

            // collect event sinks
            for (auto next : _Context->m_NodesRenderingList)
            {
                if((_Filter != nullptr && !_Filter(next)))
                    continue;

                gs_2d_ellipsef resizeTopLeft     = gs_2d_ellipsef(next->State.WindowBox.Min, _Context->m_Style->WindowResizeAngleGizmoRadius);
                gs_2d_ellipsef resizeTopRight    = gs_2d_ellipsef(next->State.WindowBox.Min + gs_vec2f(next->State.WindowBox.width(), 0.f), _Context->m_Style->WindowResizeAngleGizmoRadius);
                gs_2d_ellipsef resizeBottomLeft  = gs_2d_ellipsef(next->State.WindowBox.Max - gs_vec2f(next->State.WindowBox.width(), 0.f), _Context->m_Style->WindowResizeAngleGizmoRadius);
                gs_2d_ellipsef resizeBottomRight = gs_2d_ellipsef(next->State.WindowBox.Max, _Context->m_Style->WindowResizeAngleGizmoRadius);
                gs_2dboxf      resizeTop         = gs_2dboxf(next->State.WindowBox.Min - gs_vec2f(0.f, _Context->m_Style->WindowResizeSideGizmoWidth), next->State.WindowBox.Min + gs_vec2f(next->State.WindowBox.width(), _Context->m_Style->WindowResizeSideGizmoWidth));
                gs_2dboxf      resizeLeft        = gs_2dboxf(next->State.WindowBox.Min - gs_vec2f(_Context->m_Style->WindowResizeSideGizmoWidth, 0.f), next->State.WindowBox.Min + gs_vec2f(_Context->m_Style->WindowResizeSideGizmoWidth, next->State.WindowBox.height()));
                gs_2dboxf      resizeRight       = gs_2dboxf(next->State.WindowBox.Min + gs_vec2f(next->State.WindowBox.width() - _Context->m_Style->WindowResizeSideGizmoWidth, 0.f), next->State.WindowBox.Max + gs_vec2f(_Context->m_Style->WindowResizeSideGizmoWidth, 0.f));
                gs_2dboxf      resizeBottom      = gs_2dboxf(next->State.WindowBox.Min + gs_vec2f(0.f, next->State.WindowBox.height() - _Context->m_Style->WindowResizeSideGizmoWidth), next->State.WindowBox.Max + gs_vec2f(0.f, _Context->m_Style->WindowResizeSideGizmoWidth));

                // resize
                if( application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                  &&
                    (next->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Resizable) &&
                    next->State.Docker == nullptr                                                                                &&
                    _Context->ui_node_is_being_focused(next)                                                                     &&
                    !_Context->ui_node_is_being_scrolled(next)                                                                   &&
                    !_Context->ui_node_is_being_moved(next))
                {
                    if(resizeTopLeft.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()))
                        _Context->ui_node_enqueue_resize_top_left_node(next);
                    else if(resizeTopRight.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()))
                        _Context->ui_node_enqueue_resize_top_right_node(next);
                    else if(resizeBottomLeft.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()))
                        _Context->ui_node_enqueue_resize_bottom_left_node(next);
                    else if(resizeBottomRight.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()))
                        _Context->ui_node_enqueue_resize_bottom_right_node(next);
                    else if(resizeTop.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()))
                        _Context->ui_node_enqueue_resize_top_node(next);
                    else if(resizeLeft.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()))
                        _Context->ui_node_enqueue_resize_left_node(next);
                    else if(resizeRight.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()))
                        _Context->ui_node_enqueue_resize_right_node(next);
                    else if(resizeBottom.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()))
                        _Context->ui_node_enqueue_resize_bottom_node(next);

                    if(_Context->ui_node_event_has_been_enqueued(
                        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft     |
                        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight    |
                        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft  |
                        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight |
                        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop         |
                        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft        |
                        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight       |
                        ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom))
                    {
                        if(next->State.Parent == nullptr)
                            return true;
                    }
                }
            }

            return false;
        }

        static bool receive_scroll_event(ImmedidateUserInterfaceContextLayer* _Context, bool (*_Filter)(ImmedidateUserInterfaceNode*) = nullptr)
        {
            if(_Context == nullptr)
                return false;

            // dequeue
            if(_Context->ui_node_dequeue_vertical_scroll_node() || _Context->ui_node_dequeue_horizontal_scroll_node())
                return true;

            // check if anything already scrolles
            for (auto next : _Context->m_NodesRenderingList)
            {
                if(_Context->ui_node_is_being_scrolled(next))
                    return true;
            }

            // scroll
            for (auto next : _Context->m_NodesRenderingList)
            {
                if( (_Filter != nullptr && !_Filter(next))   ||
                    !_Context->ui_node_is_being_hovered(next))
                {
                    continue;
                }

                // vertical scroll
                if( application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                                  &&
                    next->State.VerticalScrollBarSliderBox.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()) &&
                    _Context->ui_node_is_vertical_scroll_bar_needed(next)                                                                        &&
                    !_Context->ui_node_is_being_resized(next)                                                                                    &&
                    !_Context->ui_node_is_being_moved(next))
                {
                    _Context->ui_node_enqueue_vertical_scroll_node(next);
                }

                // horizontal scroll
                if( application()->is_mouse_button_pressed(ApplicationMouseButton::ApplicationMouseButton_Left)                                    &&
                    next->State.HorizontalScrollBarSliderBox.transform(next->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()) &&
                    _Context->ui_node_is_horizontal_scroll_bar_needed(next)                                                                        &&
                    !_Context->ui_node_is_being_resized(next)                                                                                      &&
                    !_Context->ui_node_is_being_moved(next))
                {
                    _Context->ui_node_enqueue_horizontal_scroll_node(next);
                }

                if(_Context->ui_node_event_has_been_enqueued(
                    ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally |
                    ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically))
                {
                    if(next->State.Parent == nullptr)
                        return true;
                }
            }

            return false;
        }

        static bool receive_move_event(ImmedidateUserInterfaceContextLayer* _Context, bool (*_Filter)(ImmedidateUserInterfaceNode*) = nullptr)
        {
            if(_Context == nullptr)
                return false;

            // dequeue
            if(_Context->ui_node_dequeue_moving_node())
                return true;

            // detect if anything is already being moved
            for (auto movedNode : _Context->m_NodesRenderingList)
            {
                if(!_Context->ui_node_is_being_moved(movedNode))
                    continue;

                return true;
            }
            
            // look for the moved node
            for (auto movedNode : _Context->m_NodesRenderingList)
            {
                if((_Filter != nullptr && !_Filter(movedNode)) || !_Context->ui_node_is_being_focused(movedNode)) continue;

                if( (movedNode->State.Settings & ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeSettings_Movable)         &&
                    application()->is_mouse_button_hold(ApplicationMouseButton::ApplicationMouseButton_Left)                                &&
                    movedNode->State.ViewportBox.transform(movedNode->State.Transform).contains(_Context->m_Renderer->get_cursor_postion()) &&
                    !_Context->ui_node_is_being_resized(movedNode)                                                                          &&
                    !_Context->ui_node_is_being_scrolled(movedNode))
                {
                    _Context->ui_node_enqueue_moving_node(movedNode);
                }

                if(_Context->ui_node_event_has_been_enqueued(ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_IsBeingMoved))
                {
                    if(movedNode->State.Parent == nullptr)
                        return true;
                }
            }

            return false;
        }
    };

    ImmedidateUserInterfaceContextEventsReceiver::receive_focus_event(this);
    ImmedidateUserInterfaceContextEventsReceiver::receive_hover_event(this);
    ImmedidateUserInterfaceContextEventsReceiver::receive_mouse_events(this);

    if(ImmedidateUserInterfaceContextEventsReceiver::receive_widgets_events(this))
        return;

    if(ImmedidateUserInterfaceContextEventsReceiver::receive_resize_event(this))
        return;

    if(ImmedidateUserInterfaceContextEventsReceiver::receive_scroll_event(this))
        return;
    
    if(ImmedidateUserInterfaceContextEventsReceiver::receive_move_event(this))
        return;
}

void ImmedidateUserInterfaceContextLayer::ui_node_process_events()
{
    for (auto window : m_NodesRenderingList)
    {
        if(ui_node_is_being_resized(window))
        {
            gs_2dboxf estimatedBox;
            gs_vec2f  cursorDelta = application()->get_window_cursor_dragdelta();

            if(ui_node_is_being_resized_top_left(window))
            {
                estimatedBox = gs_2dboxf(
                    window->Cache.WindowBox.Min + cursorDelta,
                    window->Cache.WindowBox.Max);
            }
            else if(ui_node_is_being_resized_top_right(window))
            {
                estimatedBox = gs_2dboxf(
                    window->Cache.WindowBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                    window->Cache.WindowBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
            }
            else if(ui_node_is_being_resized_bottom_left(window))
            {
                estimatedBox = gs_2dboxf(
                    window->Cache.WindowBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                    window->Cache.WindowBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
            }
            else if(ui_node_is_being_resized_bottom_right(window))
            {
                estimatedBox = gs_2dboxf(
                    window->Cache.WindowBox.Min,
                    window->Cache.WindowBox.Max + application()->get_window_cursor_dragdelta());
            }
            else if(ui_node_is_being_resized_top(window))
            {
                estimatedBox = gs_2dboxf(
                    window->Cache.WindowBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                    window->Cache.WindowBox.Max);
            }
            else if(ui_node_is_being_resized_left(window))
            {
                estimatedBox = gs_2dboxf(
                    window->Cache.WindowBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                    window->Cache.WindowBox.Max);
            }
            else if(ui_node_is_being_resized_right(window))
            {
                estimatedBox = gs_2dboxf(
                    window->Cache.WindowBox.Min,
                    window->Cache.WindowBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f));
            }
            else if(ui_node_is_being_resized_bottom(window))
            {
                estimatedBox = gs_2dboxf(
                    window->Cache.WindowBox.Min,
                    window->Cache.WindowBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y));
            }

            // handle minimum width
            float minX = estimatedBox.size().x > window->State.WindowMinimumWidth  ? estimatedBox.Min.x : window->Cache.WindowBox.Min.x + (window->State.WindowBox.Min.x - window->Cache.WindowBox.Min.x);
            float maxX = estimatedBox.size().x > window->State.WindowMinimumWidth  ? estimatedBox.Max.x : window->Cache.WindowBox.Max.x + (window->State.WindowBox.Max.x - window->Cache.WindowBox.Max.x);
            float minY = estimatedBox.size().y > window->State.WindowMinimumHeight ? estimatedBox.Min.y : window->Cache.WindowBox.Min.y + (window->State.WindowBox.Min.y - window->Cache.WindowBox.Min.y);
            float maxY = estimatedBox.size().y > window->State.WindowMinimumHeight ? estimatedBox.Max.y : window->Cache.WindowBox.Max.y + (window->State.WindowBox.Max.y - window->Cache.WindowBox.Max.y);
            
            // handle maximum width
            minX = estimatedBox.size().x < window->State.WindowMaximumWidth  ? estimatedBox.Min.x : window->Cache.WindowBox.Min.x + (window->State.WindowBox.Min.x - window->Cache.WindowBox.Min.x);
            maxX = estimatedBox.size().x < window->State.WindowMaximumWidth  ? estimatedBox.Max.x : window->Cache.WindowBox.Max.x + (window->State.WindowBox.Max.x - window->Cache.WindowBox.Max.x);
            minY = estimatedBox.size().y < window->State.WindowMaximumHeight ? estimatedBox.Min.y : window->Cache.WindowBox.Min.y + (window->State.WindowBox.Min.y - window->Cache.WindowBox.Min.y);
            maxY = estimatedBox.size().y < window->State.WindowMaximumHeight ? estimatedBox.Max.y : window->Cache.WindowBox.Max.y + (window->State.WindowBox.Max.y - window->Cache.WindowBox.Max.y);

            window->State.WindowBox = gs_2dboxf(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));
        }
        else if(ui_node_is_being_moved(window))
        {
            // move
            window->State.WindowBox = gs_2dboxf(
                window->Cache.WindowBox.Min + application()->get_window_cursor_dragdelta(),
                window->Cache.WindowBox.Max + application()->get_window_cursor_dragdelta());

            // docking
            if(window->State.Parent == nullptr && window->State.Creator == nullptr)
            {
                ui_node_end_attaching_to_a_docker(window);

                // find the top most hovered node not equal to the moved one
                ImmedidateUserInterfaceNode* docker = nullptr;

                for (auto node : m_NodesRenderingList)
                {
                    if(node->State.WindowBox.transform(node->State.Transform).contains(m_Renderer->get_cursor_postion()))
                    {
                        if(docker == nullptr || node->State.LayerDepth > docker->State.LayerDepth)
                        {
                            if(node != window && node->State.Parent == nullptr)
                                docker = node;
                        }
                    }
                }

                if(docker != nullptr)
                {
                    bool allMouseButtonsAreReleased = true;

                    for (int button = ApplicationMouseButton::Button::ApplicationMouseButton_Begin;
                             button < ApplicationMouseButton::Button::ApplicationMouseButton_End; button++)
                    {
                        allMouseButtonsAreReleased =
                            allMouseButtonsAreReleased && !application()->is_mouse_button_down((ApplicationMouseButton::Button)button);
                    }

                    if(!ui_node_is_being_docked_by(window, docker)  &&
                        !ui_node_is_being_docked_by(docker, window))
                    {
                        // render potential docker gizmo
                        m_Renderer->push_rectangle_rounded(
                            docker->State.WindowBox.Min,
                            docker->State.WindowBox.Max,
                            0.f,
                            12.f,
                            gs_vec4f(0.f, 0.f, 255.f, 255.f),
                            docker->State.Transform * m_Renderer->calculate_transform_matrix(m_Renderer->get_far_plane()));

                        // if all mouse buttons are released we attach a moved node and all it's docked nodes to a hovered node
                        if(allMouseButtonsAreReleased)
                        {
                            ui_node_begin_attaching_to_a_docker(window, docker);

                            for (auto dockChild : m_NodesRenderingList)
                            {
                                if(dockChild->State.Docker == window)
                                    ui_node_begin_attaching_to_a_docker(dockChild, docker);
                            }
                        }
                    }
                }
            }
        }
        else if(ui_node_is_being_scrolled_horizontally(window))
        {
            window->State.HorizontalScrollBar.reposition(
                window->Cache.HorizontalScrollBar.SliderPosition + application()->get_window_cursor_dragdelta().x);
        }
        else if(ui_node_is_being_scrolled_vertically(window))
        {
            window->State.VerticalScrollBar.reposition(
                window->Cache.VerticalScrollBar.SliderPosition + application()->get_window_cursor_dragdelta().y);
        }
    }

    // disable all modifications if all mouse buttons have been released
    bool allMouseButtonsAreReleased = true;

    for (int button = ApplicationMouseButton::Button::ApplicationMouseButton_Begin;
             button < ApplicationMouseButton::Button::ApplicationMouseButton_End; button++)
    {
        allMouseButtonsAreReleased =
            allMouseButtonsAreReleased && !application()->is_mouse_button_down((ApplicationMouseButton::Button)button);
    }

    if(allMouseButtonsAreReleased)
    {
        for (auto& renderedNode : m_NodesRenderingList)
        {
            ui_node_end_move(renderedNode);
            ui_node_end_resize(renderedNode);
            ui_node_end_scroll(renderedNode);
        }
    }
}

void ImmedidateUserInterfaceContextLayer::ui_node_save_state()
{
    for (auto& cachedWindow : m_NodesRenderingList)
    {
        // save state
        if(!ui_node_is_being_modified(cachedWindow))
            cachedWindow->Cache = cachedWindow->State;

        // layers
        cachedWindow->Cache.Layer       = cachedWindow->State.Layer;
        cachedWindow->Cache.LayerDepth  = cachedWindow->State.LayerDepth;
        cachedWindow->Cache.Thickness   = cachedWindow->State.Thickness;

        // events
        cachedWindow->Cache.Changes = cachedWindow->State.Changes;

        if(cachedWindow->State.MouseDown.has_value())
            cachedWindow->Cache.MouseDown = cachedWindow->State.MouseDown.value();
        else
            cachedWindow->Cache.MouseDown.reset();
        
        if(cachedWindow->State.MousePressed.has_value())
            cachedWindow->Cache.MousePressed = cachedWindow->State.MousePressed.value();
        else
            cachedWindow->Cache.MousePressed.reset();

        if(cachedWindow->State.MouseClicked.has_value())
            cachedWindow->Cache.MouseClicked = cachedWindow->State.MouseClicked.value();
        else
            cachedWindow->Cache.MouseClicked.reset();

        if(cachedWindow->State.MouseDoubleClicked.has_value())
            cachedWindow->Cache.MouseDoubleClicked = cachedWindow->State.MouseDoubleClicked.value();
        else
            cachedWindow->Cache.MouseDoubleClicked.reset();

        cachedWindow->State.MouseDown.reset();
        cachedWindow->State.MousePressed.reset();
        cachedWindow->State.MouseClicked.reset();
        cachedWindow->State.MouseDoubleClicked.reset();

        // layout
        cachedWindow->State.LayoutTotalChildrenSize = gs_vec2f(0.f);
        cachedWindow->State.LayoutCursorPositon     = gs_vec2f(0.f, 0.f);
        cachedWindow->State.LayoutCursorSize        = gs_vec2f(0.f, 0.f);
        cachedWindow->State.ScrollAreaBox           = cachedWindow->State.ViewportBox;
        cachedWindow->State.ContentBox              = gs_2dboxf(cachedWindow->State.ViewportBox.Min, cachedWindow->State.ViewportBox.Min);

        // hierarchy
        cachedWindow->State.LayerDepth = 0;
        cachedWindow->State.ChildCount = 0;
        cachedWindow->State.Thickness  = 0;
        cachedWindow->State.Creator    = nullptr;
        cachedWindow->State.Parent     = nullptr;
        cachedWindow->State.Layer      = ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_Main;
    }

    // save rendering list to cache
    m_NodesRenderingCache = m_NodesRenderingList;

    // clear hierarchy and draw lists
    GS_ASSERT(m_NodesRenderingStack.empty());
    m_NodesRenderingList.clear();
    m_NodesRenderingStack.clear();
}

bool ImmedidateUserInterfaceContextLayer::widget_is_being_hovered() const
{
    return m_WidgetIsBeingMouseHovered.has_value() &&
            m_WidgetIsBeingMouseHovered.value();
}

bool ImmedidateUserInterfaceContextLayer::widget_is_being_pressed(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetIsBeingMousePressed.has_value() &&
            m_WidgetIsBeingMousePressed.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_is_being_clicked(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetIsBeingMouseClicked.has_value() &&
            m_WidgetIsBeingMouseClicked.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_is_being_mouse_down(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetIsBeingMouseDown.has_value() &&
            m_WidgetIsBeingMouseDown.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_is_being_double_clicked(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetIsBeingMouseDoubleClicked.has_value() &&
            m_WidgetIsBeingMouseDoubleClicked.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_has_been_hovered() const
{
    return m_WidgetHasBeenMouseHovered.has_value() &&
            m_WidgetHasBeenMouseHovered.value();
}

bool ImmedidateUserInterfaceContextLayer::widget_has_been_pressed(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetHasBeenMousePressed.has_value() &&
            m_WidgetHasBeenMousePressed.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_has_been_clicked(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetHasBeenMouseClicked.has_value() &&
            m_WidgetHasBeenMouseClicked.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_has_been_mouse_down(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetHasBeenMouseDown.has_value() &&
            m_WidgetHasBeenMouseDown.value() == _Button;
}

bool ImmedidateUserInterfaceContextLayer::widget_has_been_double_clicked(const ApplicationMouseButton::Button& _Button) const
{
    return m_WidgetHasBeenMouseDoubleClicked.has_value() &&
            m_WidgetHasBeenMouseDoubleClicked.value() == _Button;
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