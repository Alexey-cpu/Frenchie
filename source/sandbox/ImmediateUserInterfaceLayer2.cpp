#include <ImmediateUserInterfaceLayer2.hpp>

using namespace Frenchie::Application;

// UINode
UINode::UINode(const std::string _Name) : Name(_Name){}
UINode::~UINode(){}

void UINode::render(Immediate2DRenderer* _Renderer)
{
    if(_Renderer == nullptr || !is_partially_visible())
        return;

    _Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        32.f,
        gs_vec4f(128.f, 200.f, 128.f, 255.f),
        _Renderer->calculate_transform_matrix((float)place_in_follow()));

    _Renderer->push_rectangle_rounded(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        32.f,
        8.f,
        gs_vec4f(12.f, 128.f, 128.f, 255.f),
        _Renderer->calculate_transform_matrix((float)place_in_follow()));

    _Renderer->push_text(
        Name,
        32.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        _Renderer->calculate_transform_matrix((float)place_in_follow(), State.BoundingBox.Min));

    if((State.MouseHover & UINodeMouseHover_::UINodeMouseHover_MouseHovered))
    {
        _Renderer->push_rectangle_rounded(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            32.f,
            8.f,
            gs_vec4f(0.f, 255.f, 0.f, 255.f),
            _Renderer->calculate_transform_matrix((float)place_in_follow()));
    }

    if(State.MouseClicked.has_value())
    {
        _Renderer->push_rectangle_rounded_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            32.f,
            gs_vec4f(255.f, 0.f, 0.f, 255.f),
            _Renderer->calculate_transform_matrix((float)place_in_follow()));
    }
}

bool UINode::is_visible() const
{
    if(State.Parent == nullptr)
        return true;

    gs_2dboxf box = gs_2dboxf(
        State.Parent->State.BoundingBox.Min - gs_vec2f(12.f),
        State.Parent->State.BoundingBox.Max + gs_vec2f(12.f)
    );

    return box.contains(State.BoundingBox);
}

bool UINode::is_partially_visible() const
{
    if(State.Parent == nullptr)
        return true;

    gs_2dboxf box = gs_2dboxf(
        State.Parent->State.BoundingBox.Min - gs_vec2f(12.f),
        State.Parent->State.BoundingBox.Max + gs_vec2f(12.f)
    );

    return box.overlaps(State.BoundingBox);
}

// ImmedidateUserInterfaceContextLayer2
ImmedidateUserInterfaceContextLayer2::ImmedidateUserInterfaceContextLayer2(){}
ImmedidateUserInterfaceContextLayer2::~ImmedidateUserInterfaceContextLayer2(){}

bool ImmedidateUserInterfaceContextLayer2::awake()
{
    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

    return m_Renderer != nullptr;
}

void ImmedidateUserInterfaceContextLayer2::frame_start()
{
}

void ImmedidateUserInterfaceContextLayer2::frame_update()
{
    if(begin_node("Root",
        UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
    {
        if(begin_node("Root/Child-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically |UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
        {
            if(begin_node("Root/Child-1/Child-1-1", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally |UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();
            if(begin_node("Root/Child-1/Child-1-2", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally |UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();
            if(begin_node("Root/Child-1/Child-1-3", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally |UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();

            end_node();
        }

        if(begin_node("Root/Child-2", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally |UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
        {
            end_node();
        }

        if(begin_node("Root/Child-3", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally |UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
        {
            end_node();
        }

        if(begin_node("Root/Child-4", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally |UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
        {
            end_node();
        }

        end_node();
    }

    // if(begin_node("Root-1",
    //     UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally |
    //     UINodeSettings_::UINodeSettings_Resizable |
    //     UINodeSettings_::UINodeSettings_Movable | UINodeSettings_::UINodeSettings_LayoutChildrenWithNativeSize))
    // {
    //     if(begin_node("Root-1/Child-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
    //     {
    //         if(begin_node("Root-1/Child-1/Child-1-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();
    //         if(begin_node("Root-1/Child-1/Child-1-2", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();
    //         if(begin_node("Root-1/Child-1/Child-1-3", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();

    //         end_node();
    //     }

    //     if(begin_node("Root-1/Child-2", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
    //     {
    //         end_node();
    //     }

    //     if(begin_node("Root-1/Child-3", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
    //     {
    //         end_node();
    //     }

    //     end_node();
    // }

    // if(begin_node("Root-2",
    //     UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally |
    //     UINodeSettings_::UINodeSettings_Resizable |
    //     UINodeSettings_::UINodeSettings_Movable | UINodeSettings_::UINodeSettings_LayoutChildrenWithNativeSize))
    // {
    //     if(begin_node("Root-2/Child-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
    //     {
    //         if(begin_node("Root-2/Child-1/Child-1-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();
    //         if(begin_node("Root-2/Child-1/Child-1-2", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();
    //         if(begin_node("Root-2/Child-1/Child-1-3", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();

    //         end_node();
    //     }

    //     if(begin_node("Root-2/Child-2", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
    //     {
    //         end_node();
    //     }

    //     if(begin_node("Root-2/Child-3", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
    //     {
    //         end_node();
    //     }

    //     end_node();
    // }
}

void ImmedidateUserInterfaceContextLayer2::frame_debug()
{
    class UINodeGeometryComputer
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer2* _Context)
        {
            for (auto& singleton : _Context->m_Hierarchy.Singletons)
                UINodeGeometryComputer::node_geometry(_Context, singleton);
        }

    private:
        static void node_geometry(ImmedidateUserInterfaceContextLayer2* _Context, UINode* _Node)
        {
            if(_Context == nullptr || _Node == nullptr)
                return;

            gs_vec2f position   = _Node->State.BoundingBox.Min;
            gs_vec2f totalsize  = gs_vec2f(0.f, 0.f);
            float    childCount = _Context->m_Hierarchy.end(_Node) - _Context->m_Hierarchy.begin(_Node);

            // compute total size and maximum size delta
            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
            {
                if((*it) != nullptr)
                    totalsize += (*it)->State.BoundingBox.size();
            }

            // layout children
            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
            {
                // compute self
                auto node   = *it;
                auto parent = _Node;

                if(node == nullptr || parent == nullptr)
                    continue;

                // compute size
                //gs_vec2f size = parent->State.WindowBox.size() / (float)childCount;
                gs_vec2f size = node->State.BoundingBox.size();

                if(!(parent->State.Settings & UINodeSettings_::UINodeSettings_LayoutChildrenWithNativeSize))
                {
                    size = (node->State.BoundingBox.size() / totalsize) * parent->State.BoundingBox.size();

                    if((parent->State.Settings & UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
                        size = gs_vec2f(size.x, parent->State.BoundingBox.height());
                    else if((parent->State.Settings & UINodeSettings_::UINodeSettings_LayoutChildrenVertically))
                        size = gs_vec2f(parent->State.BoundingBox.width(), size.y);
                }

                size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);

                node->State.BoundingBox = gs_2dboxf(position, position + size);

                // compute child
                node_geometry(_Context, node);

                // next
                if((parent->State.Settings & UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
                    position += gs_vec2f(size.x, 0.f);
                else if((parent->State.Settings & UINodeSettings_::UINodeSettings_LayoutChildrenVertically))
                    position += gs_vec2f(0.f, size.y);
            }
        }
    };

    class UINodeEventsCatcher
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer2* _Context, const UIEvent& _Event)
        {
            catch_hover(_Context, _Event);
            catch_input(_Context, _Event);
            catch_event(_Context, _Event);
        }

    private:
        // static bool node_hover(ImmedidateUserInterfaceContextLayer2* _Context, UINode* _Node, const UIEvent& _Event)
        // {
        //     if(_Context == nullptr || _Context->m_Renderer == nullptr)
        //         return false;

        //     if(_Node->hover(_Context->m_Renderer.get(), _Event))
        //         return true;

        //     for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
        //     {
        //         if(node_hover(_Context, *it, _Event))
        //             return true;
        //     }

        //     return false;
        // }

        // static bool node_input(ImmedidateUserInterfaceContextLayer2* _Context, UINode* _Node, const UIEvent& _Event)
        // {
        //     if(_Context == nullptr || _Context->m_Renderer == nullptr)
        //         return false;

        //     if(_Node->input(_Context->m_Renderer.get(), _Event))
        //         return true;

        //     for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
        //     {
        //         if(node_input(_Context, *it, _Event))
        //             return true;
        //     }

        //     return false;
        // }
    
        // static bool node_event(ImmedidateUserInterfaceContextLayer2* _Context, UINode* _Node, const UIEvent& _Event)
        // {
        //     if(_Context == nullptr || _Context->m_Renderer == nullptr)
        //         return false;

        //     for (auto node : _Context->m_NodesRenderingList)
        //     {
        //         if(node->State.Events == UINodeEvents_::UINodeEvents_None)
        //             continue;

        //         node->event(_Context->m_Renderer.get(), _Event);
        //         return true;
        //     }

        //     if(_Node->event(_Context->m_Renderer.get(), _Event))
        //         return true;

        //     for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
        //     {
        //         if(node_event(_Context, *it, _Event))
        //             return true;
        //     }

        //     return false;
        // }

        static void catch_hover(ImmedidateUserInterfaceContextLayer2* _Context, const UIEvent& _Event)
        {
            if(_Context == nullptr)
                return;

            // find the top most node catching the mouse cursor
            UINode* hoveredNode  = nullptr;
            float   maximumDepth = INT_MIN;

            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(!node->is_partially_visible())
                {
                    node->State.MouseHover = UINodeMouseHover_::UINodeMouseHover_None;
                    continue;
                }

                // hover end logic
                if(!node->State.BoundingBox.contains(_Event.CursorPosition))
                {
                    if(!(node->Cache.MouseHover & UINodeMouseHover_::UINodeMouseHover_MouseLeft))
                    {
                        node->State.MouseLeaveTimer = Frenchie::Core::tic();
                        node->State.MouseHover |= UINodeMouseHover_::UINodeMouseHover_MouseLeft;
                    }
                    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
                        node->State.MouseLeaveTimer,Frenchie::Core::tic()) > 200.f) // TODO: this MUST BE A SETTING !!!!
                    {
                        node->State.MouseHover = UINodeMouseHover_::UINodeMouseHover_None;
                    }

                    continue;
                }

                if(node->Cache.Depth > maximumDepth)
                {
                    maximumDepth = node->Cache.Depth;
                    hoveredNode  = node;
                }
            }

            // hover start logic
            if(hoveredNode)
            {
                if(!(hoveredNode->Cache.MouseHover & UINodeMouseHover_::UINodeMouseHover_MouseEntered))
                {
                    hoveredNode->State.MouseEnterTimer = Frenchie::Core::tic();
                    hoveredNode->State.MouseHover |= UINodeMouseHover_::UINodeMouseHover_MouseEntered;
                }
                else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
                    hoveredNode->State.MouseEnterTimer,
                    Frenchie::Core::tic()) > 10.0) // TODO: this MUST BE A SETTING !!!
                {
                    hoveredNode->State.MouseHover |= UINodeMouseHover_::UINodeMouseHover_MouseHovered;
                }
            }
        }

        static void catch_input(ImmedidateUserInterfaceContextLayer2* _Context, const UIEvent& _Event)
        {
            if(_Context == nullptr)
                return;
            
            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(!(node->State.MouseHover & UINodeMouseHover_::UINodeMouseHover_MouseHovered))
                {
                    node->State.MouseDown.reset();
                    node->State.MouseHold.reset();
                    node->State.MousePressed.reset();
                    node->State.MouseClicked.reset();
                    node->State.MouseDoubleClicked.reset();
                    continue;
                }

                node->State.MouseDown          = _Event.MouseDown;
                node->State.MouseHold          = _Event.MouseHold;
                node->State.MousePressed       = _Event.MousePressed;
                node->State.MouseClicked       = _Event.MouseClicked;
                node->State.MouseDoubleClicked = _Event.MouseDoubleClicked;
            }
        }

        static void catch_event(ImmedidateUserInterfaceContextLayer2* _Context, const UIEvent& _Event)
        {
            if(_Context == nullptr)
                return;

            // check if anything is already catchint event
            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(node->State.Events == UINodeEvents_::UINodeEvents_None)
                    continue;

                UINodeEventsCatcher::event(_Context->m_Renderer.get(), node, _Event);
                return;
            }

            // find the top most hovered node with null parent
            UINode* hoveredNode  = nullptr;
            float   maximumDepth = INT_MIN;

            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(!node->is_visible() || !node->State.BoundingBox.contains(_Event.CursorPosition) || node->State.Parent != nullptr)
                    continue;

                if(node->Cache.Depth > maximumDepth)
                {
                    maximumDepth = node->Cache.Depth;
                    hoveredNode  = node;
                }
            }

            if (hoveredNode == nullptr)
                return;

            UINodeEventsCatcher::event(_Context->m_Renderer.get(), hoveredNode, _Event);
        }

        static bool event(Immediate2DRenderer* _Renderer, UINode* _Node, const UIEvent& _Event)
        {
            // auxiliary lambdas
            auto clamp_bounding_box = [](UINode* window, gs_2dboxf estimatedBox)
            {
                // handle minimum width
                float minX = estimatedBox.size().x > window->State.MinimumSize.x ? estimatedBox.Min.x : window->Cache.BoundingBox.Min.x + (window->State.BoundingBox.Min.x - window->Cache.BoundingBox.Min.x);
                float maxX = estimatedBox.size().x > window->State.MinimumSize.x ? estimatedBox.Max.x : window->Cache.BoundingBox.Max.x + (window->State.BoundingBox.Max.x - window->Cache.BoundingBox.Max.x);
                float minY = estimatedBox.size().y > window->State.MinimumSize.y ? estimatedBox.Min.y : window->Cache.BoundingBox.Min.y + (window->State.BoundingBox.Min.y - window->Cache.BoundingBox.Min.y);
                float maxY = estimatedBox.size().y > window->State.MinimumSize.y ? estimatedBox.Max.y : window->Cache.BoundingBox.Max.y + (window->State.BoundingBox.Max.y - window->Cache.BoundingBox.Max.y);
                
                // handle maximum width
                minX = estimatedBox.size().x < window->State.MaximumSize.x ? estimatedBox.Min.x : window->Cache.BoundingBox.Min.x + (window->State.BoundingBox.Min.x - window->Cache.BoundingBox.Min.x);
                maxX = estimatedBox.size().x < window->State.MaximumSize.x ? estimatedBox.Max.x : window->Cache.BoundingBox.Max.x + (window->State.BoundingBox.Max.x - window->Cache.BoundingBox.Max.x);
                minY = estimatedBox.size().y < window->State.MaximumSize.y ? estimatedBox.Min.y : window->Cache.BoundingBox.Min.y + (window->State.BoundingBox.Min.y - window->Cache.BoundingBox.Min.y);
                maxY = estimatedBox.size().y < window->State.MaximumSize.y ? estimatedBox.Max.y : window->Cache.BoundingBox.Max.y + (window->State.BoundingBox.Max.y - window->Cache.BoundingBox.Max.y);
                window->State.BoundingBox = gs_2dboxf(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));
            };

            // resize
            if((_Node->State.Settings & UINodeSettings_::UINodeSettings_Resizable) &&
                !(_Node->State.Events & UINodeEvents_::UINodeEvents_IsMoved))
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                float WindowResizeSideGizmoWidth   = 8.f;

                gs_2d_ellipsef resizeTopLeft     = gs_2d_ellipsef(_Node->State.BoundingBox.Min, WindowResizeAngleGizmoRadius);
                gs_2d_ellipsef resizeTopRight    = gs_2d_ellipsef(_Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
                gs_2d_ellipsef resizeBottomLeft  = gs_2d_ellipsef(_Node->State.BoundingBox.Max - gs_vec2f(_Node->State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
                gs_2d_ellipsef resizeBottomRight = gs_2d_ellipsef(_Node->State.BoundingBox.Max, WindowResizeAngleGizmoRadius);
                gs_2dboxf      resizeTop         = gs_2dboxf(_Node->State.BoundingBox.Min - gs_vec2f(0.f, WindowResizeSideGizmoWidth), _Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width(), WindowResizeSideGizmoWidth));
                gs_2dboxf      resizeLeft        = gs_2dboxf(_Node->State.BoundingBox.Min - gs_vec2f(WindowResizeSideGizmoWidth, 0.f), _Node->State.BoundingBox.Min + gs_vec2f(WindowResizeSideGizmoWidth, _Node->State.BoundingBox.height()));
                gs_2dboxf      resizeRight       = gs_2dboxf(_Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width() - WindowResizeSideGizmoWidth, 0.f), _Node->State.BoundingBox.Max + gs_vec2f(WindowResizeSideGizmoWidth, 0.f));
                gs_2dboxf      resizeBottom      = gs_2dboxf(_Node->State.BoundingBox.Min + gs_vec2f(0.f, _Node->State.BoundingBox.height() - WindowResizeSideGizmoWidth), _Node->State.BoundingBox.Max + gs_vec2f(0.f, WindowResizeSideGizmoWidth));

                if(resizeTopLeft.contains(_Event.CursorPosition) ||
                    (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTopLeft))
                {
                    _Renderer->push_arc_filled(
                        resizeTopLeft.Center,
                        resizeTopLeft.Radius,
                        resizeTopLeft.Radius,
                        0.f,
                        360.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                    if(_Event.MouseDown.has_value())
                    {
                        _Node->State.Events |= UINodeEvents_::UINodeEvents_IsResizedTopLeft;
                        clamp_bounding_box(_Node, gs_2dboxf(_Node->Cache.BoundingBox.Min + _Event.CursorDragDelta, _Node->Cache.BoundingBox.Max));
                        return true;
                    }
                }
                else if(resizeTopRight.contains(_Event.CursorPosition) ||
                    (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTopRight))
                {
                    _Renderer->push_arc_filled(
                        resizeTopRight.Center,
                        resizeTopRight.Radius,
                        resizeTopRight.Radius,
                        0.f,
                        360.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                    if(_Event.MouseDown.has_value())
                    {
                        _Node->State.Events |= UINodeEvents_::UINodeEvents_IsResizedTopRight;
                        clamp_bounding_box(_Node, gs_2dboxf(
                            _Node->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                            _Node->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
                        return true;
                    }
                }
                else if(resizeBottomLeft.contains(_Event.CursorPosition) ||
                    (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTopBottomLeft))
                {
                    _Renderer->push_arc_filled(
                        resizeBottomLeft.Center,
                        resizeBottomLeft.Radius,
                        resizeBottomLeft.Radius,
                        0.f,
                        360.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                    if(_Event.MouseDown.has_value())
                    {
                        _Node->State.Events |= UINodeEvents_::UINodeEvents_IsResizedTopBottomLeft;
                        clamp_bounding_box(_Node, gs_2dboxf(
                            _Node->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                            _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
                        return true;
                    }
                }
                else if(resizeBottomRight.contains(_Event.CursorPosition) ||
                    (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTopBottomRight))
                {
                    _Renderer->push_arc_filled(
                        resizeBottomRight.Center,
                        resizeBottomRight.Radius,
                        resizeBottomRight.Radius,
                        0.f,
                        360.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                    if(_Event.MouseDown.has_value())
                    {
                        _Node->State.Events |= UINodeEvents_::UINodeEvents_IsResizedTopBottomRight;
                        clamp_bounding_box(_Node, gs_2dboxf(
                            _Node->Cache.BoundingBox.Min,
                            _Node->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta()));
                        return true;
                    }
                }
                else if(resizeTop.contains(_Event.CursorPosition) ||
                    (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTop))
                {
                    _Renderer->push_rectangle_rounded_filled(
                        resizeTop.Min,
                        resizeTop.Max,
                        16.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                    if(_Event.MouseDown.has_value())
                    {
                        _Node->State.Events |= UINodeEvents_::UINodeEvents_IsResizedTop;
                        clamp_bounding_box(_Node, gs_2dboxf(
                            _Node->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                            _Node->Cache.BoundingBox.Max));
                        return true;
                    }
                }
                else if(resizeLeft.contains(_Event.CursorPosition) ||
                    (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedLeft))
                {
                    _Renderer->push_rectangle_rounded_filled(
                        resizeLeft.Min,
                        resizeLeft.Max,
                        16.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                    if(_Event.MouseDown.has_value())
                    {
                        _Node->State.Events |= UINodeEvents_::UINodeEvents_IsResizedLeft;
                        clamp_bounding_box(_Node, gs_2dboxf(
                            _Node->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                            _Node->Cache.BoundingBox.Max));
                        return true;
                    }
                }
                else if(resizeRight.contains(_Event.CursorPosition) ||
                    (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedRight))
                {
                    _Renderer->push_rectangle_rounded_filled(
                        resizeRight.Min,
                        resizeRight.Max,
                        16.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                    if(_Event.MouseDown.has_value())
                    {
                        _Node->State.Events |= UINodeEvents_::UINodeEvents_IsResizedRight;
                        clamp_bounding_box(_Node, gs_2dboxf(
                            _Node->Cache.BoundingBox.Min,
                            _Node->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
                        return true;
                    }
                }
                else if(resizeBottom.contains(_Event.CursorPosition) ||
                    (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedBottom))
                {
                    _Renderer->push_rectangle_rounded_filled(
                        resizeBottom.Min,
                        resizeBottom.Max,
                        16.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                    if(_Event.MouseDown.has_value())
                    {
                        _Node->State.Events |= UINodeEvents_::UINodeEvents_IsResizedBottom;
                        clamp_bounding_box(_Node, gs_2dboxf(
                            _Node->Cache.BoundingBox.Min,
                            _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
                        return true;
                    }
                }
            }

            // // move
            // if((_Node->State.Settings & UINodeSettings_::UINodeSettings_Movable) &&
            //     !((_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTop)          ||
            //     (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedLeft)           ||
            //     (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedRight)          ||
            //     (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedBottom)         ||
            //     (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTopBottomLeft)  ||
            //     (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTopBottomRight) ||
            //     (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTopLeft)        ||
            //     (_Node->State.Events & UINodeEvents_::UINodeEvents_IsResizedTopRight)))
            // {
            //     if((_Event.MouseDown.has_value() && _Node->State.BoundingBox.contains(_Event.CursorPosition)) ||
            //         (_Node->State.Events & UINodeEvents_::UINodeEvents_IsMoved))
            //     {
            //         _Node->State.Events |= UINodeEvents_::UINodeEvents_IsMoved;
            //         _Node->State.BoundingBox = gs_2dboxf(
            //             _Node->Cache.BoundingBox.Min + application()->get_window_cursor_dragdelta(),
            //             _Node->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta());
            //         return true;
            //     }
            // }

            return false;
        }
    };

    // build hierarchy
    m_Hierarchy.build(m_NodesRenderingList);

    // process events
    UIEvent event;
    event.CursorPosition  = m_Renderer->get_cursor_postion();
    event.CursorDragDelta = Frenchie::Application::application()->get_window_cursor_dragdelta();

    for (int button = ApplicationMouseButton::ApplicationMouseButton_Begin;
             button < ApplicationMouseButton::ApplicationMouseButton_End;
             button++)
    {
        if(Frenchie::Application::application()->is_mouse_button_down((ApplicationMouseButton::Button)button))
            event.MouseDown = (ApplicationMouseButton::Button)button;

        if(Frenchie::Application::application()->is_mouse_button_hold((ApplicationMouseButton::Button)button))
            event.MouseHold = (ApplicationMouseButton::Button)button;

        if(Frenchie::Application::application()->is_mouse_button_pressed((ApplicationMouseButton::Button)button))
            event.MousePressed = (ApplicationMouseButton::Button)button;

        if(Frenchie::Application::application()->is_mouse_button_clicked((ApplicationMouseButton::Button)button))
            event.MouseClicked = (ApplicationMouseButton::Button)button;

        if(Frenchie::Application::application()->is_mouse_button_double_clicked((ApplicationMouseButton::Button)button))
            event.MouseDoubleClicked = (ApplicationMouseButton::Button)button;
    }

    UINodeEventsCatcher::execute(this, event);
    UINodeGeometryComputer::execute(this);
}

void ImmedidateUserInterfaceContextLayer2::frame_render()
{
    class UINodeRenderer
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer2* _Context)
        {
            // compute initial depth of singletons
            for (auto& singleton : _Context->m_Hierarchy.Singletons)
            {
                singleton->State.Depth = 0.f;
                
                for (auto& renderedNode : _Context->m_NodesRenderingCache)
                {
                    singleton->State.Depth =
                        gs_max(
                            renderedNode->State.Depth + renderedNode->State.TotalThickness + 1,
                            singleton->State.Depth);
                }

                UINodeRenderer::render_node(_Context, singleton);

                _Context->m_NodesRenderingCache.push_back(singleton);
            }
        }

    private:

        static void render_node(ImmedidateUserInterfaceContextLayer2* _Context, UINode* _Node)
        {
            // calculate clippingbox
            {
                auto next   = _Node;
                auto parent = _Node->State.Parent;

                while (parent != nullptr)
                {
                    next   = parent;
                    parent = parent->State.Parent;
                }

                _Context->m_Renderer->push_clip_box(
                    _Node->State.BoundingBox.clip_with(next->State.BoundingBox));
            }

            // setup depth
            if(_Node->State.Parent != nullptr)
            {
                _Node->State.Depth = 
                    _Node->State.Parent->State.Depth +
                    _Node->State.Parent->State.SelfThickness + 1;
            }

            // render self and children
            _Node->render(_Context->m_Renderer.get());

            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
                render_node(_Context, (*it));

            // update parental bandwidth
            if(_Node->State.Parent)
                _Node->State.Parent->State.TotalThickness += _Node->State.SelfThickness;

            // remove clipping
            _Context->m_Renderer->pop_clip_box();
        }
    };

    UINodeRenderer::execute(this);
}

void ImmedidateUserInterfaceContextLayer2::frame_finish()
{
    // save state
    bool allMouseButtonsAreReleased = true;

    for (int button = ApplicationMouseButton::Button::ApplicationMouseButton_Begin;
             button < ApplicationMouseButton::Button::ApplicationMouseButton_End;
             button++)
    {
        allMouseButtonsAreReleased =
            allMouseButtonsAreReleased && !application()->is_mouse_button_down((ApplicationMouseButton::Button)button);
    }

    for (auto& node : m_NodesRenderingList)
    {
        // stop all modifications
        if(allMouseButtonsAreReleased)
            node->State.Events = UINodeEvents_::UINodeEvents_None;
        node->Cache.Events             = node->State.Events;
        node->Cache.MouseHover         = node->State.MouseHover;
        node->Cache.MouseDown          = node->State.MouseDown;
        node->Cache.MouseHold          = node->State.MouseHold;
        node->Cache.MousePressed       = node->State.MousePressed;
        node->Cache.MouseClicked       = node->State.MouseClicked;
        node->Cache.MouseDoubleClicked = node->State.MouseDoubleClicked;
        node->Cache.SelfThickness      = node->State.SelfThickness;
        node->Cache.TotalThickness     = node->State.TotalThickness;
        node->Cache.Depth              = node->State.Depth;
        node->Cache.Parent             = node->State.Parent;
        
        // save cache
        if(node->State.Events == UINodeEvents_::UINodeEvents_None)
            node->Cache = node->State;

        // clean-up
        node->State.Depth          = 0;
        node->State.SelfThickness  = 0;
        node->State.TotalThickness = 0;
        node->State.RenderingIndex = 0;
        node->State.Parent         = nullptr;
        node->State.Settings       = 0;
    }

    m_NodesRenderingList.clear();
    m_NodesRenderingCache.clear();
    m_NodesRenderingStack.clear();
}