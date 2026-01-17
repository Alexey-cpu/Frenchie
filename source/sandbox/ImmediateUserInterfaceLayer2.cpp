#include <ImmediateUserInterfaceLayer2.hpp>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        class ImmedidateUserInterfaceHelpers
        {
        public:

            // auxiliary lambdas
            static void clamp_bounding_box(ImmedidateUserInterfaceNode* _Node, gs_2dboxf _BoundingBox)
            {
                // handle minimum width
                float minX = _BoundingBox.size().x > _Node->State.MinimumSize.x ? _BoundingBox.Min.x : _Node->Cache.BoundingBox.Min.x + (_Node->State.BoundingBox.Min.x - _Node->Cache.BoundingBox.Min.x);
                float maxX = _BoundingBox.size().x > _Node->State.MinimumSize.x ? _BoundingBox.Max.x : _Node->Cache.BoundingBox.Max.x + (_Node->State.BoundingBox.Max.x - _Node->Cache.BoundingBox.Max.x);
                float minY = _BoundingBox.size().y > _Node->State.MinimumSize.y ? _BoundingBox.Min.y : _Node->Cache.BoundingBox.Min.y + (_Node->State.BoundingBox.Min.y - _Node->Cache.BoundingBox.Min.y);
                float maxY = _BoundingBox.size().y > _Node->State.MinimumSize.y ? _BoundingBox.Max.y : _Node->Cache.BoundingBox.Max.y + (_Node->State.BoundingBox.Max.y - _Node->Cache.BoundingBox.Max.y);
                
                // handle maximum width
                minX = _BoundingBox.size().x < _Node->State.MaximumSize.x ? _BoundingBox.Min.x : _Node->Cache.BoundingBox.Min.x + (_Node->State.BoundingBox.Min.x - _Node->Cache.BoundingBox.Min.x);
                maxX = _BoundingBox.size().x < _Node->State.MaximumSize.x ? _BoundingBox.Max.x : _Node->Cache.BoundingBox.Max.x + (_Node->State.BoundingBox.Max.x - _Node->Cache.BoundingBox.Max.x);
                minY = _BoundingBox.size().y < _Node->State.MaximumSize.y ? _BoundingBox.Min.y : _Node->Cache.BoundingBox.Min.y + (_Node->State.BoundingBox.Min.y - _Node->Cache.BoundingBox.Min.y);
                maxY = _BoundingBox.size().y < _Node->State.MaximumSize.y ? _BoundingBox.Max.y : _Node->Cache.BoundingBox.Max.y + (_Node->State.BoundingBox.Max.y - _Node->Cache.BoundingBox.Max.y);
                _Node->State.BoundingBox = gs_2dboxf(gs_vec2f(minX, minY), gs_vec2f(maxX, maxY));
            }

            static gs_2d_ellipsef build_resize_top_left_ellipse(ImmedidateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Min, WindowResizeAngleGizmoRadius);
            }

            static gs_2d_ellipsef build_resize_top_right_ellipse(ImmedidateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
            }

            static gs_2d_ellipsef build_resize_bottom_left_ellipse(ImmedidateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Max - gs_vec2f(_Node->State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
            };

            static gs_2d_ellipsef build_resize_bottom_right_ellipse(ImmedidateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Max, WindowResizeAngleGizmoRadius);
            };

            static gs_2dboxf build_resize_top_box(ImmedidateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 16.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min - gs_vec2f(0.f, WindowResizeSideGizmoWidth),
                    _Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width(), WindowResizeSideGizmoWidth));
            };

            static gs_2dboxf build_resize_left_box(ImmedidateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 16.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min - gs_vec2f(WindowResizeSideGizmoWidth, 0.f),
                    _Node->State.BoundingBox.Min + gs_vec2f(WindowResizeSideGizmoWidth, _Node->State.BoundingBox.height()));
            };

            static gs_2dboxf build_resize_right_box(ImmedidateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 16.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width() - WindowResizeSideGizmoWidth, 0.f),
                    _Node->State.BoundingBox.Max + gs_vec2f(WindowResizeSideGizmoWidth, 0.f));
            };

            static gs_2dboxf build_resize_bottom_box(ImmedidateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 16.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min + gs_vec2f(0.f, _Node->State.BoundingBox.height() - WindowResizeSideGizmoWidth),
                    _Node->State.BoundingBox.Max + gs_vec2f(0.f, WindowResizeSideGizmoWidth));
            };
        };
    }
}

// UINode
ImmedidateUserInterfaceNode::ImmedidateUserInterfaceNode(const std::string _Name) : Name(_Name){}
ImmedidateUserInterfaceNode::~ImmedidateUserInterfaceNode(){}

void ImmedidateUserInterfaceNode::render(ImmedidateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr || !is_partially_visible())
        return;

    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        32.f,
        gs_vec4f(128.f, 200.f, 128.f, 255.f),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    _Context->m_Renderer->push_rectangle_rounded(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        32.f,
        8.f,
        gs_vec4f(12.f, 128.f, 128.f, 255.f),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    _Context->m_Renderer->push_text(
        Name,
        32.f,
        gs_vec4f(255.f, 0.f, 0.f, 255.f),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow(), State.BoundingBox.Min));

    if((State.MouseHover & ImmediateUserInterfaceMouseHover_::ImmediateUserInterfaceMouseHover_MouseHovered))
    {
        _Context->m_Renderer->push_rectangle_rounded(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            32.f,
            8.f,
            gs_vec4f(0.f, 255.f, 0.f, 255.f),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }

    if(State.MouseClicked.has_value())
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            32.f,
            gs_vec4f(255.f, 0.f, 0.f, 255.f),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }
}

void ImmedidateUserInterfaceNode::layout(ImmedidateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    gs_vec2f position   = State.BoundingBox.Min;
    gs_vec2f totalsize  = gs_vec2f(0.f, 0.f);

    // compute total size and maximum size delta
    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
    {
        if((*it) != nullptr)
            totalsize += (*it)->State.BoundingBox.size();
    }

    // layout children
    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
    {
        // compute self
        auto node   = *it;
        auto parent = this;

        if(node == nullptr || parent == nullptr)
            continue;

        // compute size
        gs_vec2f size = node->State.BoundingBox.size();

        if(!(parent->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenWithNativeSize))
        {
            size = (node->State.BoundingBox.size() / totalsize) * parent->State.BoundingBox.size();

            if((parent->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenHorizontally))
                size = gs_vec2f(size.x, parent->State.BoundingBox.height());
            else if((parent->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenVertically))
                size = gs_vec2f(parent->State.BoundingBox.width(), size.y);
        }

        size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);

        node->State.BoundingBox = gs_2dboxf(position, position + size);

        // // compute child
        // node->layout(_Context);

        // next
        if((parent->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenHorizontally))
            position += gs_vec2f(size.x, 0.f);
        else if((parent->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenVertically))
            position += gs_vec2f(0.f, size.y);
    }
}

bool ImmedidateUserInterfaceNode::is_visible() const
{
    if(State.Parent == nullptr)
        return true;

    gs_2dboxf box = gs_2dboxf(
        State.Parent->State.BoundingBox.Min - gs_vec2f(12.f),
        State.Parent->State.BoundingBox.Max + gs_vec2f(12.f)
    );

    return box.contains(State.BoundingBox);
}

bool ImmedidateUserInterfaceNode::is_partially_visible() const
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
ImmedidateUserInterfaceContextLayer::ImmedidateUserInterfaceContextLayer(){}
ImmedidateUserInterfaceContextLayer::~ImmedidateUserInterfaceContextLayer(){}

bool ImmedidateUserInterfaceContextLayer::awake()
{
    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

    return m_Renderer != nullptr;
}

void ImmedidateUserInterfaceContextLayer::frame_start()
{
}

void ImmedidateUserInterfaceContextLayer::frame_update()
{
    if(begin_node("Root",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenHorizontally | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        if(begin_node("Root/Child-1", ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenVertically |ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
        {
            if(begin_node("Root/Child-1/Child-1-1", ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenHorizontally |ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable)) end_node();
            if(begin_node("Root/Child-1/Child-1-2", ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenHorizontally |ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable)) end_node();
            if(begin_node("Root/Child-1/Child-1-3", ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenHorizontally |ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable)) end_node();

            end_node();
        }

        if(begin_node("Root/Child-2", ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenHorizontally |ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
        {
            end_node();
        }

        if(begin_node("Root/Child-3", ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenHorizontally |ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
        {
            end_node();
        }

        if(begin_node("Root/Child-4", ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutChildrenHorizontally |ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
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

void ImmedidateUserInterfaceContextLayer::frame_debug()
{
    class UINodeGeometryComputer
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer* _Context)
        {
            for (auto& singleton : _Context->m_Hierarchy.Singletons)
                UINodeGeometryComputer::node_geometry(_Context, singleton);
        }

    private:
        static void node_geometry(ImmedidateUserInterfaceContextLayer* _Context, ImmedidateUserInterfaceNode* _Node)
        {
            if(_Context == nullptr || _Node == nullptr)
                return;

            _Node->layout(_Context);
            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
                (*it)->layout(_Context);
        }
    };

    class UINodeEventsCatcher
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            catch_hover(_Context, _Event);
            catch_input(_Context, _Event);
            catch_event(_Context, _Event);
        }

    private:

        static void catch_hover(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            if(_Context == nullptr)
                return;

            // find the top most node catching the mouse cursor
            ImmedidateUserInterfaceNode* hoveredNode  = nullptr;
            float   maximumDepth = INT_MIN;

            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(!node->is_partially_visible())
                {
                    node->State.MouseHover = ImmediateUserInterfaceMouseHover_::ImmediateUserInterfaceMouseHover_None;
                    continue;
                }

                // hover end logic
                if(!node->State.BoundingBox.contains(_Event.CursorPosition))
                {
                    if(!(node->Cache.MouseHover & ImmediateUserInterfaceMouseHover_::ImmediateUserInterfaceMouseHover_MouseLeft))
                    {
                        node->State.MouseLeaveTimer = Frenchie::Core::tic();
                        node->State.MouseHover |= ImmediateUserInterfaceMouseHover_::ImmediateUserInterfaceMouseHover_MouseLeft;
                    }
                    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
                        node->State.MouseLeaveTimer,Frenchie::Core::tic()) > 200.f) // TODO: this MUST BE A SETTING !!!!
                    {
                        node->State.MouseHover = ImmediateUserInterfaceMouseHover_::ImmediateUserInterfaceMouseHover_None;
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
                if(!(hoveredNode->Cache.MouseHover & ImmediateUserInterfaceMouseHover_::ImmediateUserInterfaceMouseHover_MouseEntered))
                {
                    hoveredNode->State.MouseEnterTimer = Frenchie::Core::tic();
                    hoveredNode->State.MouseHover |= ImmediateUserInterfaceMouseHover_::ImmediateUserInterfaceMouseHover_MouseEntered;
                }
                else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
                    hoveredNode->State.MouseEnterTimer,
                    Frenchie::Core::tic()) > 10.0) // TODO: this MUST BE A SETTING !!!
                {
                    hoveredNode->State.MouseHover |= ImmediateUserInterfaceMouseHover_::ImmediateUserInterfaceMouseHover_MouseHovered;
                }
            }
        }

        static void catch_input(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            if(_Context == nullptr)
                return;
            
            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(!(node->State.MouseHover & ImmediateUserInterfaceMouseHover_::ImmediateUserInterfaceMouseHover_MouseHovered))
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

        static void catch_event(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            if(_Context == nullptr)
                return;

            // check if anything is already catchint event
            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(node->State.Events == ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
                    continue;

                UINodeEventsCatcher::event(_Context->m_Renderer.get(), node, _Event);
                return;
            }

            // find the top most hovered node
            ImmedidateUserInterfaceNode* hoveredNode  = nullptr;
            float                        maximumDepth = INT_MIN;

            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(!node->is_visible() || !node->State.BoundingBox.contains(_Event.CursorPosition))
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

        static bool event(Immediate2DRenderer* _Renderer, ImmedidateUserInterfaceNode* _Node, const ImmedidateUserInterfaceEvent& _Event)
        {
            // resize
            if((_Node->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable) &&
                !(_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
            {
                if(ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(_Node).contains(_Event.CursorPosition) ||
                    (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft))
                {
                    auto resizable = _Node;
                    while (resizable->State.Parent &&
                            ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(resizable->State.Parent).contains(_Event.CursorPosition))
                    {
                        resizable = resizable->State.Parent;
                    }

                    auto resizeTopLeft = ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(resizable);

                    _Renderer->push_arc_filled(
                        resizeTopLeft.Center,
                        resizeTopLeft.Radius,
                        resizeTopLeft.Radius,
                        0.f,
                        360.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Renderer->get_far_plane()));

                    if(_Event.MouseDown.has_value())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft;
                        ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                            resizable->Cache.BoundingBox.Min + _Event.CursorDragDelta,
                            resizable->Cache.BoundingBox.Max));
                        return true;
                    }
                }
                else if(ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(_Node).contains(_Event.CursorPosition) ||
                    (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
                {
                    auto resizable = _Node;
                    while (resizable->State.Parent &&
                            ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(resizable->State.Parent).contains(_Event.CursorPosition))
                    {
                        resizable = resizable->State.Parent;
                    }

                    auto resizeTopRight = ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(resizable);

                    _Renderer->push_arc_filled(
                        resizeTopRight.Center,
                        resizeTopRight.Radius,
                        resizeTopRight.Radius,
                        0.f,
                        360.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Renderer->get_far_plane()));

                    if(_Event.MouseDown.has_value())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight;
                        
                        ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                            resizable->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                            resizable->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
                        return true;
                    }
                }
                else if(ImmedidateUserInterfaceHelpers::build_resize_bottom_left_ellipse(_Node).contains(_Event.CursorPosition) ||
                    (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
                {
                    auto resizable = _Node;
                    while (resizable->State.Parent &&
                            ImmedidateUserInterfaceHelpers::build_resize_bottom_left_ellipse(resizable->State.Parent).contains(_Event.CursorPosition))
                    {
                        resizable = resizable->State.Parent;
                    }

                    auto resizeBottomLeft = ImmedidateUserInterfaceHelpers::build_resize_bottom_left_ellipse(resizable);

                    _Renderer->push_arc_filled(
                        resizeBottomLeft.Center,
                        resizeBottomLeft.Radius,
                        resizeBottomLeft.Radius,
                        0.f,
                        360.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Renderer->get_far_plane()));

                    if(_Event.MouseDown.has_value())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft;
                        ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                            resizable->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                            resizable->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
                        return true;
                    }
                }
                else if(ImmedidateUserInterfaceHelpers::build_resize_bottom_right_ellipse(_Node).contains(_Event.CursorPosition) ||
                    (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
                {
                    auto resizable = _Node;
                    while (resizable->State.Parent &&
                            ImmedidateUserInterfaceHelpers::build_resize_bottom_right_ellipse(resizable->State.Parent).contains(_Event.CursorPosition))
                    {
                        resizable = resizable->State.Parent;
                    }

                    auto resizeBottomRight = ImmedidateUserInterfaceHelpers::build_resize_bottom_right_ellipse(resizable);

                    _Renderer->push_arc_filled(
                        resizeBottomRight.Center,
                        resizeBottomRight.Radius,
                        resizeBottomRight.Radius,
                        0.f,
                        360.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Renderer->get_far_plane()));

                    if(_Event.MouseDown.has_value())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight;
                        ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                            resizable->Cache.BoundingBox.Min,
                            resizable->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta()));
                        return true;
                    }
                }
                else if(ImmedidateUserInterfaceHelpers::build_resize_top_box(_Node).contains(_Event.CursorPosition) ||
                    (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop))
                {
                    auto resizable = _Node;
                    while (resizable->State.Parent &&
                            ImmedidateUserInterfaceHelpers::build_resize_top_box(resizable->State.Parent).contains(_Event.CursorPosition))
                    {
                        resizable = resizable->State.Parent;
                    }

                    auto resizeTop = ImmedidateUserInterfaceHelpers::build_resize_top_box(resizable);

                    _Renderer->push_rectangle_rounded_filled(
                        resizeTop.Min,
                        resizeTop.Max,
                        16.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Renderer->get_far_plane()));

                    if(_Event.MouseDown.has_value())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop;
                        ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                            resizable->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                            resizable->Cache.BoundingBox.Max));
                        return true;
                    }
                }
                else if(ImmedidateUserInterfaceHelpers::build_resize_left_box(_Node).contains(_Event.CursorPosition) ||
                    (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft))
                {
                    auto resizable = _Node;
                    while (resizable->State.Parent &&
                            ImmedidateUserInterfaceHelpers::build_resize_left_box(resizable->State.Parent).contains(_Event.CursorPosition))
                    {
                        resizable = resizable->State.Parent;
                    }

                    auto resizeLeft = ImmedidateUserInterfaceHelpers::build_resize_left_box(resizable);

                    _Renderer->push_rectangle_rounded_filled(
                        resizeLeft.Min,
                        resizeLeft.Max,
                        16.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Renderer->get_far_plane()));

                    if(_Event.MouseDown.has_value())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft;
                        ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                            resizable->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                            resizable->Cache.BoundingBox.Max));
                        return true;
                    }
                }
                else if(ImmedidateUserInterfaceHelpers::build_resize_right_box(_Node).contains(_Event.CursorPosition) ||
                    (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
                {
                    auto resizable = _Node;
                    while (resizable->State.Parent &&
                            ImmedidateUserInterfaceHelpers::build_resize_right_box(resizable->State.Parent).contains(_Event.CursorPosition))
                    {
                        resizable = resizable->State.Parent;
                    }

                    auto resizeRight = ImmedidateUserInterfaceHelpers::build_resize_right_box(resizable);

                    _Renderer->push_rectangle_rounded_filled(
                        resizeRight.Min,
                        resizeRight.Max,
                        16.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Renderer->get_far_plane()));

                    if(_Event.MouseDown.has_value())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight;
                        ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                            resizable->Cache.BoundingBox.Min,
                            resizable->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
                        return true;
                    }
                }
                else if(ImmedidateUserInterfaceHelpers::build_resize_bottom_box(_Node).contains(_Event.CursorPosition) ||
                    (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
                {
                    auto resizable = _Node;
                    while (resizable->State.Parent &&
                            ImmedidateUserInterfaceHelpers::build_resize_bottom_box(resizable->State.Parent).contains(_Event.CursorPosition))
                    {
                        resizable = resizable->State.Parent;
                    }

                    auto resizeBottom = ImmedidateUserInterfaceHelpers::build_resize_bottom_box(resizable);

                    _Renderer->push_rectangle_rounded_filled(
                        resizeBottom.Min,
                        resizeBottom.Max,
                        16.f,
                        gs_vec4f(5, 255, 255, 200.f),
                        _Renderer->calculate_transform_matrix((float)_Renderer->get_far_plane()));

                    if(_Event.MouseDown.has_value())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom;
                        ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                            resizable->Cache.BoundingBox.Min,
                            resizable->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
                        return true;
                    }
                }
            }

            // move
            if((_Node->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable) &&
                !((_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)         ||
                  (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)        ||
                  (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)       ||
                  (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)      ||
                  (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)  ||
                  (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                  (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)     ||
                  (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)))
            {
                if((_Event.MouseDown.has_value() && _Node->State.BoundingBox.contains(_Event.CursorPosition)) ||
                    (_Node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
                {
                    auto movable = _Node;
                    while (movable->State.Parent)
                        movable = movable->State.Parent;

                    movable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;
                    movable->State.BoundingBox = gs_2dboxf(
                        movable->Cache.BoundingBox.Min + application()->get_window_cursor_dragdelta(),
                        movable->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta());
                    return true;
                }
            }

            return false;
        }
    };

    // build hierarchy
    m_Hierarchy.build(m_NodesRenderingList);

    // process events
    ImmedidateUserInterfaceEvent event;
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

void ImmedidateUserInterfaceContextLayer::frame_render()
{
    class UINodeRenderer
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer* _Context)
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

        static void render_node(ImmedidateUserInterfaceContextLayer* _Context, ImmedidateUserInterfaceNode* _Node)
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
            _Node->render(_Context);

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

void ImmedidateUserInterfaceContextLayer::frame_finish()
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
            node->State.Events = ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None;
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
        if(node->State.Events == ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
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