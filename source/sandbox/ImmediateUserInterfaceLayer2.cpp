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
        _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

    _Renderer->push_rectangle_rounded(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        32.f,
        8.f,
        gs_vec4f(12.f, 128.f, 128.f, 255.f),
        _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

    _Renderer->push_text(
        Name,
        32.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++), State.BoundingBox.Min));

    if((State.MouseHover & UINodeMouseHover_::UINodeMouseHover_MouseHovered))
    {
        _Renderer->push_rectangle_rounded(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            32.f,
            8.f,
            gs_vec4f(0.f, 255.f, 0.f, 255.f),
            _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));
    }

    if(State.MouseClicked.has_value())
    {
        _Renderer->push_rectangle_rounded_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            32.f,
            gs_vec4f(255.f, 0.f, 0.f, 255.f),
            _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));
    }
}

bool UINode::hover(Immediate2DRenderer* _Renderer, const UIEvent& _Event)
{
    if(!is_partially_visible())
        return false;

    // catch hover
    if(State.BoundingBox.contains(_Event.CursorPosition))
    {
        if(!(Cache.MouseHover & UINodeMouseHover_::UINodeMouseHover_MouseEntered))
        {
            State.MouseEnterTimer = Frenchie::Core::tic();
            State.MouseHover |= UINodeMouseHover_::UINodeMouseHover_MouseEntered;
        }
        else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
            State.MouseEnterTimer,
            Frenchie::Core::tic()) > 10.0) // TODO: this MUST BE A SETTING !!!
        {
            State.MouseHover |= UINodeMouseHover_::UINodeMouseHover_MouseHovered;

            UINode* parent = State.Parent;

            while (parent)
            {
                parent->State.MouseHover = UINodeMouseHover_::UINodeMouseHover_None;
                parent = parent->State.Parent;
            }
        }
    }
    else
    {
        if(!(Cache.MouseHover & UINodeMouseHover_::UINodeMouseHover_MouseLeft))
        {
            State.MouseLeaveTimer = Frenchie::Core::tic();
            State.MouseHover |= UINodeMouseHover_::UINodeMouseHover_MouseLeft;
        }
        else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
            State.MouseLeaveTimer,Frenchie::Core::tic()) > 200.f) // TODO: this MUST BE A SETTING !!!!
        {
            State.MouseHover = UINodeMouseHover_::UINodeMouseHover_None;
        }
    }

    return false;
}

bool UINode::input(Immediate2DRenderer* _Renderer, const UIEvent& _Event)
{
    if(!(State.MouseHover & UINodeMouseHover_::UINodeMouseHover_MouseHovered))
        return false;

    State.MouseDown          = _Event.MouseDown;
    State.MouseHold          = _Event.MouseHold;
    State.MousePressed       = _Event.MousePressed;
    State.MouseClicked       = _Event.MouseClicked;
    State.MouseDoubleClicked = _Event.MouseDoubleClicked;

    return
            State.MousePressed.has_value() ||
            State.MouseClicked.has_value() ||
            State.MouseDoubleClicked.has_value();
}

bool UINode::event(Immediate2DRenderer* _Renderer, const UIEvent& _Event)
{
    if(!is_visible())
        return false;

    // auxiliary lambdas
    auto clamp_size = [this](UINode* window, gs_2dboxf estimatedBox)
    {
        // handle minimum width
        float minX = estimatedBox.size().x > window->State.MinimumSize.x  ? estimatedBox.Min.x : window->Cache.BoundingBox.Min.x + (window->State.BoundingBox.Min.x - window->Cache.BoundingBox.Min.x);
        float maxX = estimatedBox.size().x > window->State.MinimumSize.x  ? estimatedBox.Max.x : window->Cache.BoundingBox.Max.x + (window->State.BoundingBox.Max.x - window->Cache.BoundingBox.Max.x);
        float minY = estimatedBox.size().y > window->State.MinimumSize.y ? estimatedBox.Min.y : window->Cache.BoundingBox.Min.y + (window->State.BoundingBox.Min.y - window->Cache.BoundingBox.Min.y);
        float maxY = estimatedBox.size().y > window->State.MinimumSize.y ? estimatedBox.Max.y : window->Cache.BoundingBox.Max.y + (window->State.BoundingBox.Max.y - window->Cache.BoundingBox.Max.y);
        
        // handle maximum width
        minX = estimatedBox.size().x < window->State.MaximumSize.x  ? estimatedBox.Min.x : window->Cache.BoundingBox.Min.x + (window->State.BoundingBox.Min.x - window->Cache.BoundingBox.Min.x);
        maxX = estimatedBox.size().x < window->State.MaximumSize.x  ? estimatedBox.Max.x : window->Cache.BoundingBox.Max.x + (window->State.BoundingBox.Max.x - window->Cache.BoundingBox.Max.x);
        minY = estimatedBox.size().y < window->State.MaximumSize.y ? estimatedBox.Min.y : window->Cache.BoundingBox.Min.y + (window->State.BoundingBox.Min.y - window->Cache.BoundingBox.Min.y);
        maxY = estimatedBox.size().y < window->State.MaximumSize.y ? estimatedBox.Max.y : window->Cache.BoundingBox.Max.y + (window->State.BoundingBox.Max.y - window->Cache.BoundingBox.Max.y);
        window->State.BoundingBox = gs_2dboxf(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));
    };

    // resize
    if((State.Settings & UINodeSettings_::UINodeSettings_Resizable) &&
        !(State.Events & UINodeEvents_::UINodeEvents_IsMoved))
    {
        float WindowResizeAngleGizmoRadius = 32.f;
        float WindowResizeSideGizmoWidth   = 8.f;

        gs_2d_ellipsef resizeTopLeft     = gs_2d_ellipsef(State.BoundingBox.Min, WindowResizeAngleGizmoRadius);
        gs_2d_ellipsef resizeTopRight    = gs_2d_ellipsef(State.BoundingBox.Min + gs_vec2f(State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
        gs_2d_ellipsef resizeBottomLeft  = gs_2d_ellipsef(State.BoundingBox.Max - gs_vec2f(State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
        gs_2d_ellipsef resizeBottomRight = gs_2d_ellipsef(State.BoundingBox.Max, WindowResizeAngleGizmoRadius);
        gs_2dboxf      resizeTop         = gs_2dboxf(State.BoundingBox.Min - gs_vec2f(0.f, WindowResizeSideGizmoWidth), State.BoundingBox.Min + gs_vec2f(State.BoundingBox.width(), WindowResizeSideGizmoWidth));
        gs_2dboxf      resizeLeft        = gs_2dboxf(State.BoundingBox.Min - gs_vec2f(WindowResizeSideGizmoWidth, 0.f), State.BoundingBox.Min + gs_vec2f(WindowResizeSideGizmoWidth, State.BoundingBox.height()));
        gs_2dboxf      resizeRight       = gs_2dboxf(State.BoundingBox.Min + gs_vec2f(State.BoundingBox.width() - WindowResizeSideGizmoWidth, 0.f), State.BoundingBox.Max + gs_vec2f(WindowResizeSideGizmoWidth, 0.f));
        gs_2dboxf      resizeBottom      = gs_2dboxf(State.BoundingBox.Min + gs_vec2f(0.f, State.BoundingBox.height() - WindowResizeSideGizmoWidth), State.BoundingBox.Max + gs_vec2f(0.f, WindowResizeSideGizmoWidth));

        if(resizeTopLeft.contains(_Event.CursorPosition) ||
            (State.Events & UINodeEvents_::UINodeEvents_IsResizedTopLeft))
        {
            _Renderer->push_arc_filled(
                resizeTopLeft.Center,
                resizeTopLeft.Radius,
                resizeTopLeft.Radius,
                0.f,
                360.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

            if(_Event.MouseDown.has_value())
            {
                State.Events |= UINodeEvents_::UINodeEvents_IsResizedTopLeft;
                clamp_size(this, gs_2dboxf(Cache.BoundingBox.Min + _Event.CursorDragDelta, Cache.BoundingBox.Max));
                return true;
            }
        }
        else if(resizeTopRight.contains(_Event.CursorPosition) ||
            (State.Events & UINodeEvents_::UINodeEvents_IsResizedTopRight))
        {
            _Renderer->push_arc_filled(
                resizeTopRight.Center,
                resizeTopRight.Radius,
                resizeTopRight.Radius,
                0.f,
                360.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

            if(_Event.MouseDown.has_value())
            {
                State.Events |= UINodeEvents_::UINodeEvents_IsResizedTopRight;
                clamp_size(this, gs_2dboxf(
                    Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                    Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
                return true;
            }
        }
        else if(resizeBottomLeft.contains(_Event.CursorPosition) ||
            (State.Events & UINodeEvents_::UINodeEvents_IsResizedTopBottomLeft))
        {
            _Renderer->push_arc_filled(
                resizeBottomLeft.Center,
                resizeBottomLeft.Radius,
                resizeBottomLeft.Radius,
                0.f,
                360.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

            if(_Event.MouseDown.has_value())
            {
                State.Events |= UINodeEvents_::UINodeEvents_IsResizedTopBottomLeft;
                clamp_size(this, gs_2dboxf(
                    Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                    Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
                return true;
            }
        }
        else if(resizeBottomRight.contains(_Event.CursorPosition) ||
            (State.Events & UINodeEvents_::UINodeEvents_IsResizedTopBottomRight))
        {
            _Renderer->push_arc_filled(
                resizeBottomRight.Center,
                resizeBottomRight.Radius,
                resizeBottomRight.Radius,
                0.f,
                360.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

            if(_Event.MouseDown.has_value())
            {
                State.Events |= UINodeEvents_::UINodeEvents_IsResizedTopBottomRight;
                clamp_size(this, gs_2dboxf(
                    Cache.BoundingBox.Min,
                    Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta()));
                return true;
            }
        }
        else if(resizeTop.contains(_Event.CursorPosition) ||
            (State.Events & UINodeEvents_::UINodeEvents_IsResizedTop))
        {
            _Renderer->push_rectangle_rounded_filled(
                resizeTop.Min,
                resizeTop.Max,
                16.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

            if(_Event.MouseDown.has_value())
            {
                State.Events |= UINodeEvents_::UINodeEvents_IsResizedTop;
                clamp_size(this, gs_2dboxf(
                    Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                    Cache.BoundingBox.Max));
                return true;
            }
        }
        else if(resizeLeft.contains(_Event.CursorPosition) ||
            (State.Events & UINodeEvents_::UINodeEvents_IsResizedLeft))
        {
            _Renderer->push_rectangle_rounded_filled(
                resizeLeft.Min,
                resizeLeft.Max,
                16.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

            if(_Event.MouseDown.has_value())
            {
                State.Events |= UINodeEvents_::UINodeEvents_IsResizedLeft;
                clamp_size(this, gs_2dboxf(
                    Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                    Cache.BoundingBox.Max));
                return true;
            }
        }
        else if(resizeRight.contains(_Event.CursorPosition) ||
            (State.Events & UINodeEvents_::UINodeEvents_IsResizedRight))
        {
            _Renderer->push_rectangle_rounded_filled(
                resizeRight.Min,
                resizeRight.Max,
                16.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

            if(_Event.MouseDown.has_value())
            {
                State.Events |= UINodeEvents_::UINodeEvents_IsResizedRight;
                clamp_size(this, gs_2dboxf(
                    Cache.BoundingBox.Min,
                    Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
                return true;
            }
        }
        else if(resizeBottom.contains(_Event.CursorPosition) ||
            (State.Events & UINodeEvents_::UINodeEvents_IsResizedBottom))
        {
            _Renderer->push_rectangle_rounded_filled(
                resizeBottom.Min,
                resizeBottom.Max,
                16.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));

            if(_Event.MouseDown.has_value())
            {
                State.Events |= UINodeEvents_::UINodeEvents_IsResizedBottom;
                clamp_size(this, gs_2dboxf(
                    Cache.BoundingBox.Min,
                    Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
                return true;
            }
        }
    }

    // move
    if((State.Settings & UINodeSettings_::UINodeSettings_Movable) &&
        !((State.Events & UINodeEvents_::UINodeEvents_IsResizedTop)          ||
        (State.Events & UINodeEvents_::UINodeEvents_IsResizedLeft)           ||
        (State.Events & UINodeEvents_::UINodeEvents_IsResizedRight)          ||
        (State.Events & UINodeEvents_::UINodeEvents_IsResizedBottom)         ||
        (State.Events & UINodeEvents_::UINodeEvents_IsResizedTopBottomLeft)  ||
        (State.Events & UINodeEvents_::UINodeEvents_IsResizedTopBottomRight) ||
        (State.Events & UINodeEvents_::UINodeEvents_IsResizedTopLeft)        ||
        (State.Events & UINodeEvents_::UINodeEvents_IsResizedTopRight)))
    {
        if((_Event.MouseDown.has_value() && State.BoundingBox.contains(_Event.CursorPosition)) ||
            (State.Events & UINodeEvents_::UINodeEvents_IsMoved))
        {
            State.Events |= UINodeEvents_::UINodeEvents_IsMoved;
            State.BoundingBox = gs_2dboxf(
                Cache.BoundingBox.Min + application()->get_window_cursor_dragdelta(),
                Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta());
            return true;
        }
    }

    return false;
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
    static bool start = true;

    if(begin_node("Root",
        UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
    {
        // if(start)
        // {
        //     std::cout << "START !!! \n";
        //     m_NodesRenderingStack[m_NodesRenderingStack.size()-1]->State.WindowBox =
        //         gs_2dboxf(gs_vec2f(12.f, 12.f), gs_vec2f(255.f, 255.f));
        // }

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

    if(begin_node("Root-1",
        UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally |
        UINodeSettings_::UINodeSettings_Resizable |
        UINodeSettings_::UINodeSettings_Movable | UINodeSettings_::UINodeSettings_LayoutChildrenWithNativeSize
    )
)
    {
        // if(start)
        // {
        //     m_NodesRenderingStack[m_NodesRenderingStack.size()-1]->State.WindowBox =
        //         gs_2dboxf(gs_vec2f(255.f, 255.f) + gs_vec2f(12.f, 12.f), gs_vec2f(512.f, 512.f) + gs_vec2f(12.f, 12.f));
        // }

        if(begin_node("Root-1/Child-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
        {
            if(begin_node("Root-1/Child-1/Child-1-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();
            if(begin_node("Root-1/Child-1/Child-1-2", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();
            if(begin_node("Root-1/Child-1/Child-1-3", UINodeSettings_::UINodeSettings_LayoutChildrenVertically | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable)) end_node();

            end_node();
        }

        if(begin_node("Root-1/Child-2", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
        {
            end_node();
        }

        if(begin_node("Root-1/Child-3", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally | UINodeSettings_::UINodeSettings_Resizable | UINodeSettings_::UINodeSettings_Movable))
        {
            end_node();
        }

        end_node();
    }

    start = false;
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

                // compute depth
                node->State.Depth = parent->State.Depth + parent->State.Thickness + 1;

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
            if(catch_hover(_Context, _Event) &&
                catch_input(_Context, _Event)) return;
            catch_event(_Context, _Event);
        }

    private:
        static bool node_hover(ImmedidateUserInterfaceContextLayer2* _Context, UINode* _Node, const UIEvent& _Event)
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr)
                return false;

            if(_Node->hover(_Context->m_Renderer.get(), _Event))
                return true;

            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
            {
                if(node_hover(_Context, *it, _Event))
                    return true;
            }

            return false;
        }

        static bool node_input(ImmedidateUserInterfaceContextLayer2* _Context, UINode* _Node, const UIEvent& _Event)
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr)
                return false;

            if(_Node->input(_Context->m_Renderer.get(), _Event))
                return true;

            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
            {
                if(node_input(_Context, *it, _Event))
                    return true;
            }

            return false;
        }
    
        static bool node_event(ImmedidateUserInterfaceContextLayer2* _Context, UINode* _Node, const UIEvent& _Event)
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr)
                return false;

            for (auto node : _Context->m_NodesRenderingList)
            {
                if(node->State.Events == UINodeEvents_::UINodeEvents_None)
                    continue;

                node->event(_Context->m_Renderer.get(), _Event);
                return true;
            }

            if(_Node->event(_Context->m_Renderer.get(), _Event))
                return true;

            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
            {
                if(node_event(_Context, *it, _Event))
                    return true;
            }

            return false;
        }

        static bool catch_hover(ImmedidateUserInterfaceContextLayer2* _Context, const UIEvent& _Event)
        {
            if(_Context == nullptr)
                return false;

            for (auto& singleton : _Context->m_Hierarchy.Singletons)
            {
                if(UINodeEventsCatcher::node_hover(_Context, singleton, _Event))
                    return true;
            }

            for (auto& node : _Context->m_NodesRenderingList)
            {
                if((node->State.MouseHover & UINodeMouseHover_::UINodeMouseHover_MouseHovered))
                    return true;
            }

            return false;
        }

        static bool catch_input(ImmedidateUserInterfaceContextLayer2* _Context, const UIEvent& _Event)
        {
            if(_Context == nullptr)
                return false;
            
            for (auto& singleton : _Context->m_Hierarchy.Singletons)
            {
                if(UINodeEventsCatcher::node_input(_Context, singleton, _Event))
                    return true;
            }

            return false;
        }

        static bool catch_event(ImmedidateUserInterfaceContextLayer2* _Context, const UIEvent& _Event)
        {
            if(_Context == nullptr)
                return false;

            for (auto& singleton : _Context->m_Hierarchy.Singletons)
            {
                if(UINodeEventsCatcher::node_event(_Context, singleton, _Event))
                    return true;
            }

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
        static void execute(ImmedidateUserInterfaceContextLayer2* _Context, UINode* _Node)
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

            _Node->render(_Context->m_Renderer.get());

            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
                execute(_Context, (*it));

            _Context->m_Renderer->pop_clip_box();
        }
    };

    // render
    for (auto& singleton : m_Hierarchy.Singletons)
        UINodeRenderer::execute(this, singleton);
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
        node->Cache.Events            = node->State.Events;
        node->Cache.MouseHover         = node->State.MouseHover;
        node->Cache.MouseDown          = node->State.MouseDown;
        node->Cache.MouseHold          = node->State.MouseHold;
        node->Cache.MousePressed       = node->State.MousePressed;
        node->Cache.MouseClicked       = node->State.MouseClicked;
        node->Cache.MouseDoubleClicked = node->State.MouseDoubleClicked;

        // save cache
        if(node->State.Events == UINodeEvents_::UINodeEvents_None)
            node->Cache = node->State;

        // clean-up
        node->State.Depth          = 0;
        node->State.Thickness      = 1;
        node->State.RenderingIndex = 0;
        node->State.Parent         = nullptr;
        node->State.Settings       = 0;
    }

    m_NodesRenderingCache = m_NodesRenderingList;
    m_NodesRenderingList.clear();
    m_NodesRenderingStack.clear();
}