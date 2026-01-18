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
    
        struct ImmedidateUserInterfaceVerticalStack : public ImmedidateUserInterfaceNode
        {
        public:
            ImmedidateUserInterfaceVerticalStack(const std::string& _Name) : ImmedidateUserInterfaceNode(_Name){}
            virtual ~ImmedidateUserInterfaceVerticalStack(){}

            virtual void layout(ImmedidateUserInterfaceContextLayer* _Context) override
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
                    size = (node->State.BoundingBox.size() / totalsize) * parent->State.BoundingBox.size();
                    size = gs_vec2f(parent->State.BoundingBox.width(), size.y);
                    size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);

                    node->State.BoundingBox = gs_2dboxf(position, position + size);

                    // next
                    position += gs_vec2f(0.f, size.y);
                }
            }
        };

        struct ImmedidateUserInterfaceHorizontalStack : public ImmedidateUserInterfaceNode
        {
        public:
            ImmedidateUserInterfaceHorizontalStack(const std::string& _Name) : ImmedidateUserInterfaceNode(_Name){}
            virtual ~ImmedidateUserInterfaceHorizontalStack(){}

            virtual void layout(ImmedidateUserInterfaceContextLayer* _Context) override
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
                    size = (node->State.BoundingBox.size() / totalsize) * parent->State.BoundingBox.size();
                    size = gs_vec2f(size.x, parent->State.BoundingBox.height());
                    size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);

                    node->State.BoundingBox = gs_2dboxf(position, position + size);

                    // next
                    position += gs_vec2f(size.x, 0.f);
                }
            }
        };
    
        struct ImmedidateUserInterfaceWindow : public ImmedidateUserInterfaceVerticalStack
        {
        public:

            enum ImmedidateUserInterfaceWindowLayer_ : int
            {
                ImmedidateUserInterfaceWindowLayer_Begin   = 0,
                ImmedidateUserInterfaceWindowLayer_Main    = ImmedidateUserInterfaceWindowLayer_Begin,
                ImmedidateUserInterfaceWindowLayer_Focused,
                ImmedidateUserInterfaceWindowLayer_End,
            };

            ImmedidateUserInterfaceWindow(const std::string& _Name) : ImmedidateUserInterfaceVerticalStack(_Name){}
            virtual ~ImmedidateUserInterfaceWindow(){}

            void render_frame(
                ImmedidateUserInterfaceNode*         _Node,
                ImmedidateUserInterfaceContextLayer* _Context,
                const gs_2dboxf&                     _Box)
            {
                // background
                if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
                    _Box.contains(_Context->m_Renderer->get_cursor_postion()))
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Box.Min,
                        _Box.Max,
                        32.f,
                        gs_vec4f(128.f, 200.f, 200.f, 255.f),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                    if(State.MouseClicked.has_value())
                    {
                        _Node->State.InitialDepth =
                            calculate_layer_depth(_Context, ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Focused);
                    }
                }
                else
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Box.Min,
                        _Box.Max,
                        32.f,
                        gs_vec4f(128.f, 200.f, 128.f, 255.f),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }

                // frame
                _Context->m_Renderer->push_rectangle_rounded(
                    _Box.Min,
                    _Box.Max,
                    32.f,
                    8.f,
                    gs_vec4f(12.f, 128.f, 128.f, 255.f),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));


                // title
                _Context->m_Renderer->push_text(
                    _Node->Name,
                    32.f,
                    gs_vec4f(255.f, 0.f, 0.f, 255.f),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow(), _Box.Min));

            }

            virtual void render(ImmedidateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr || !is_partially_visible())
                    return;

                // background
                _Context->m_Renderer->push_rectangle_rounded_filled(
                    State.BoundingBox.Min,
                    State.BoundingBox.Max,
                    32.f,
                    gs_vec4f(128.f, 200.f, 128.f, 255.f),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // background frame
                _Context->m_Renderer->push_rectangle_rounded(
                    State.BoundingBox.Min,
                    State.BoundingBox.Max,
                    32.f,
                    8.f,
                    gs_vec4f(12.f, 128.f, 128.f, 255.f),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // title frame
                auto dockedWindowsCount = (_Context->m_DockAreas.end(this) - _Context->m_DockAreas.begin(this));

                if(dockedWindowsCount > 0)
                {
                    gs_vec2f position = State.BoundingBox.Min;
                    gs_vec2f size     = State.BoundingBox.size() / (float)(dockedWindowsCount + 1);

                    // render self frame
                    render_frame(this, _Context, gs_2dboxf(position, position + gs_vec2f(size.x, 32.f)));
                    position += gs_vec2f(size.x, 0.f);

                    // render docked children frames
                    for(auto it = _Context->m_DockAreas.begin(this); it != _Context->m_DockAreas.end(this); ++it)
                    {
                        render_frame((*it), _Context, gs_2dboxf(position, position + gs_vec2f(size.x, 32.f)));

                        position += gs_vec2f(size.x, 0.f);
                    }
                }
                else
                {
                    if(Docker == nullptr)
                        render_frame(this, _Context, gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min + gs_vec2f(State.BoundingBox.width(), 32.f)));
                }
            }

            virtual void layout(ImmedidateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr)
                    return;

                // docking
                {
                    gs_2dboxf frameBox = gs_2dboxf(
                        State.BoundingBox.Min,
                        gs_vec2f(State.BoundingBox.Max.x, State.BoundingBox.Min.y + 32.f));

                    DockingBox = gs_2dboxf(
                        (Docker == nullptr ? gs_vec2f(frameBox.Min.x, frameBox.Max.y) : State.BoundingBox.Min),
                        State.BoundingBox.Max);

                    for(auto it = _Context->m_DockAreas.begin(this); it != _Context->m_DockAreas.end(this); ++it)
                    {
                        if((*it) != nullptr)
                            (*it)->State.BoundingBox = DockingBox;
                    }
                }

                // self layouting
                {
                    gs_vec2f position   = DockingBox.Min;
                    gs_vec2f totalsize  = gs_vec2f(0.f, 0.f);

                    // compute total size
                    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
                    {
                        if((*it) != nullptr)
                            totalsize += (*it)->State.BoundingBox.size();
                    }

                    // layout children
                    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
                    {
                        if((*it) == nullptr)
                            continue;

                        // compute size
                        gs_vec2f size = gs_clamp(
                            gs_vec2f(DockingBox.width(), (((*it)->State.BoundingBox.size() / totalsize) * DockingBox.size()).y),
                            (*it)->State.MinimumSize, (*it)->State.MaximumSize);

                        (*it)->State.BoundingBox = gs_2dboxf(position, position + size);

                        // next
                        position += gs_vec2f(0.f, size.y);
                    }
                }
            }

            static int calculate_layer_depth(ImmedidateUserInterfaceContextLayer* _Context, int _Layer)
            {
                return _Layer * _Context->m_Renderer->get_far_plane() / (ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_End - ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Begin);
            }

            ImmedidateUserInterfaceNode* Docker{nullptr};
            gs_2dboxf DockingBox;
        };
    }
}

// ImmedidateUserInterfaceNode
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

    if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
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

void ImmedidateUserInterfaceNode::layout(ImmedidateUserInterfaceContextLayer* _Context){}

void ImmedidateUserInterfaceNode::events(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceNodeEvent& _Event)
{
    // resize
    if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable) &&
        !(State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
    {
        if(ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(this).contains(_Event.CursorPosition) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft))
        {
            auto resizable = this;
            while (resizable->State.Parent &&
                    ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(resizable->State.Parent).contains(_Event.CursorPosition))
            {
                resizable = resizable->State.Parent;
            }

            auto resizeTopLeft = ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(resizable);

            _Context->m_Renderer->push_arc_filled(
                resizeTopLeft.Center,
                resizeTopLeft.Radius,
                resizeTopLeft.Radius,
                0.f,
                360.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Context->m_Renderer->calculate_transform_matrix((float)_Context->m_Renderer->get_far_plane()));

            if(_Event.MouseDown.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft;
                ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                    resizable->Cache.BoundingBox.Min + _Event.CursorDragDelta,
                    resizable->Cache.BoundingBox.Max));
                return;
            }
        }
        else if(ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(this).contains(_Event.CursorPosition) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
        {
            auto resizable = this;
            while (resizable->State.Parent &&
                    ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(resizable->State.Parent).contains(_Event.CursorPosition))
            {
                resizable = resizable->State.Parent;
            }

            auto resizeTopRight = ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(resizable);

            _Context->m_Renderer->push_arc_filled(
                resizeTopRight.Center,
                resizeTopRight.Radius,
                resizeTopRight.Radius,
                0.f,
                360.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Context->m_Renderer->calculate_transform_matrix((float)_Context->m_Renderer->get_far_plane()));

            if(_Event.MouseDown.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight;
                
                ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                    resizable->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                    resizable->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
                return;
            }
        }
        else if(ImmedidateUserInterfaceHelpers::build_resize_bottom_left_ellipse(this).contains(_Event.CursorPosition) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
        {
            auto resizable = this;
            while (resizable->State.Parent &&
                    ImmedidateUserInterfaceHelpers::build_resize_bottom_left_ellipse(resizable->State.Parent).contains(_Event.CursorPosition))
            {
                resizable = resizable->State.Parent;
            }

            auto resizeBottomLeft = ImmedidateUserInterfaceHelpers::build_resize_bottom_left_ellipse(resizable);

            _Context->m_Renderer->push_arc_filled(
                resizeBottomLeft.Center,
                resizeBottomLeft.Radius,
                resizeBottomLeft.Radius,
                0.f,
                360.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Context->m_Renderer->calculate_transform_matrix((float)_Context->m_Renderer->get_far_plane()));

            if(_Event.MouseDown.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft;
                ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                    resizable->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                    resizable->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
                return;
            }
        }
        else if(ImmedidateUserInterfaceHelpers::build_resize_bottom_right_ellipse(this).contains(_Event.CursorPosition) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
        {
            auto resizable = this;
            while (resizable->State.Parent &&
                    ImmedidateUserInterfaceHelpers::build_resize_bottom_right_ellipse(resizable->State.Parent).contains(_Event.CursorPosition))
            {
                resizable = resizable->State.Parent;
            }

            auto resizeBottomRight = ImmedidateUserInterfaceHelpers::build_resize_bottom_right_ellipse(resizable);

            _Context->m_Renderer->push_arc_filled(
                resizeBottomRight.Center,
                resizeBottomRight.Radius,
                resizeBottomRight.Radius,
                0.f,
                360.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Context->m_Renderer->calculate_transform_matrix((float)_Context->m_Renderer->get_far_plane()));

            if(_Event.MouseDown.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight;
                ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                    resizable->Cache.BoundingBox.Min,
                    resizable->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta()));
                return;
            }
        }
        else if(ImmedidateUserInterfaceHelpers::build_resize_top_box(this).contains(_Event.CursorPosition) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop))
        {
            auto resizable = this;
            while (resizable->State.Parent &&
                    ImmedidateUserInterfaceHelpers::build_resize_top_box(resizable->State.Parent).contains(_Event.CursorPosition))
            {
                resizable = resizable->State.Parent;
            }

            auto resizeTop = ImmedidateUserInterfaceHelpers::build_resize_top_box(resizable);

            _Context->m_Renderer->push_rectangle_rounded_filled(
                resizeTop.Min,
                resizeTop.Max,
                16.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Context->m_Renderer->calculate_transform_matrix((float)_Context->m_Renderer->get_far_plane()));

            if(_Event.MouseDown.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop;
                ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                    resizable->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                    resizable->Cache.BoundingBox.Max));
                return;
            }
        }
        else if(ImmedidateUserInterfaceHelpers::build_resize_left_box(this).contains(_Event.CursorPosition) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft))
        {
            auto resizable = this;
            while (resizable->State.Parent &&
                    ImmedidateUserInterfaceHelpers::build_resize_left_box(resizable->State.Parent).contains(_Event.CursorPosition))
            {
                resizable = resizable->State.Parent;
            }

            auto resizeLeft = ImmedidateUserInterfaceHelpers::build_resize_left_box(resizable);

            _Context->m_Renderer->push_rectangle_rounded_filled(
                resizeLeft.Min,
                resizeLeft.Max,
                16.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Context->m_Renderer->calculate_transform_matrix((float)_Context->m_Renderer->get_far_plane()));

            if(_Event.MouseDown.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft;
                ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                    resizable->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                    resizable->Cache.BoundingBox.Max));
                return;
            }
        }
        else if(ImmedidateUserInterfaceHelpers::build_resize_right_box(this).contains(_Event.CursorPosition) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
        {
            auto resizable = this;
            while (resizable->State.Parent &&
                    ImmedidateUserInterfaceHelpers::build_resize_right_box(resizable->State.Parent).contains(_Event.CursorPosition))
            {
                resizable = resizable->State.Parent;
            }

            auto resizeRight = ImmedidateUserInterfaceHelpers::build_resize_right_box(resizable);

            _Context->m_Renderer->push_rectangle_rounded_filled(
                resizeRight.Min,
                resizeRight.Max,
                16.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Context->m_Renderer->calculate_transform_matrix((float)_Context->m_Renderer->get_far_plane()));

            if(_Event.MouseDown.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight;
                ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                    resizable->Cache.BoundingBox.Min,
                    resizable->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
                return;
            }
        }
        else if(ImmedidateUserInterfaceHelpers::build_resize_bottom_box(this).contains(_Event.CursorPosition) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
        {
            auto resizable = this;
            while (resizable->State.Parent &&
                    ImmedidateUserInterfaceHelpers::build_resize_bottom_box(resizable->State.Parent).contains(_Event.CursorPosition))
            {
                resizable = resizable->State.Parent;
            }

            auto resizeBottom = ImmedidateUserInterfaceHelpers::build_resize_bottom_box(resizable);

            _Context->m_Renderer->push_rectangle_rounded_filled(
                resizeBottom.Min,
                resizeBottom.Max,
                16.f,
                gs_vec4f(5, 255, 255, 200.f),
                _Context->m_Renderer->calculate_transform_matrix((float)_Context->m_Renderer->get_far_plane()));

            if(_Event.MouseDown.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom;
                ImmedidateUserInterfaceHelpers::clamp_bounding_box(resizable, gs_2dboxf(
                    resizable->Cache.BoundingBox.Min,
                    resizable->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
                return;
            }
        }
    }

    // move
    if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable) &&
        !((State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)           ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)        ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)       ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)      ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)  ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)     ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)))
    {
        if((_Event.MousePressed.has_value() && State.BoundingBox.contains(_Event.CursorPosition)) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
        {
            auto movable = this;
            while (movable->State.Parent)
                movable = movable->State.Parent;

            movable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;
            movable->State.BoundingBox = gs_2dboxf(
                movable->Cache.BoundingBox.Min + application()->get_window_cursor_dragdelta(),
                movable->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta());
            return;
        }
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

int ImmedidateUserInterfaceNode::place_in_follow()
{
    State.TotalThickness += (++State.SelfThickness);
    return State.Depth + State.SelfThickness;
}

// ImmedidateUserInterfaceContextLayer2
ImmedidateUserInterfaceContextLayer::ImmedidateUserInterfaceContextLayer(){}
ImmedidateUserInterfaceContextLayer::~ImmedidateUserInterfaceContextLayer(){}

bool ImmedidateUserInterfaceContextLayer::awake()
{
    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

    m_Hierarchy = ImmedidateUserInterfaceNodeHierarchy(
        [](ImmedidateUserInterfaceNode* _Node)->ImmedidateUserInterfaceNode*
        {
            return _Node != nullptr ? _Node->State.Parent : nullptr;
        });

    m_DockAreas = ImmedidateUserInterfaceNodeHierarchy(
        [](ImmedidateUserInterfaceNode* _Node)->ImmedidateUserInterfaceNode*
        {
            ImmedidateUserInterfaceWindow* window =
                dynamic_cast<ImmedidateUserInterfaceWindow*>(_Node);

            return window != nullptr ? window->Docker : nullptr;
        });

    return m_Renderer != nullptr;
}

void ImmedidateUserInterfaceContextLayer::frame_start(){}

void ImmedidateUserInterfaceContextLayer::frame_update()
{
}

void showHierarchy(ImmedidateUserInterfaceContextLayer* _Context, ImmedidateUserInterfaceNode* _Node, const std::string& _Delimiter)
{
    std::cout << _Delimiter << _Node->Name << "\n";

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
    {
        showHierarchy(_Context, (*it), _Delimiter + "\t");
    }
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
                node_geometry(_Context, (*it));
        }
    };

    class UINodeEventsCatcher
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceNodeEvent& _Event)
        {
            catch_hover(_Context, _Event);
            catch_input(_Context, _Event);
            catch_event(_Context, _Event);
        }

    private:

        static void catch_hover(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceNodeEvent& _Event)
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
                    node->State.MouseHover = ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None;
                    continue;
                }

                // hover end logic
                if(!node->State.BoundingBox.contains(_Event.CursorPosition))
                {
                    if(!(node->Cache.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseLeft))
                    {
                        node->State.MouseLeaveTimer = Frenchie::Core::tic();
                        node->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseLeft;
                    }
                    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
                        node->State.MouseLeaveTimer,Frenchie::Core::tic()) > 200.f) // TODO: this MUST BE A SETTING !!!!
                    {
                        node->State.MouseHover = ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None;
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
                if(!(hoveredNode->Cache.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseEntered))
                {
                    hoveredNode->State.MouseEnterTimer = Frenchie::Core::tic();
                    hoveredNode->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseEntered;
                }
                else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
                    hoveredNode->State.MouseEnterTimer,
                    Frenchie::Core::tic()) > 10.0) // TODO: this MUST BE A SETTING !!!
                {
                    hoveredNode->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered;
                }
            }
        }

        static void catch_input(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceNodeEvent& _Event)
        {
            if(_Context == nullptr)
                return;
            
            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(!(node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
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

        static void catch_event(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceNodeEvent& _Event)
        {
            if(_Context == nullptr)
                return;

            // check if anything is already catching event
            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(node->State.Events == ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
                    continue;

                node->events(_Context, _Event);
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

            if (hoveredNode != nullptr)
                hoveredNode->events(_Context, _Event);
        }
    };

    class UIWindowsController
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceNodeEvent& _Event)
        {
            // pass focus
            for (auto node : _Context->m_NodesRenderingList)
            {
                bool modified = (node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved)  || 
                    (node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)         ||
                    (node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)        ||
                    (node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)       ||
                    (node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)      ||
                    (node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)  ||
                    (node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                    (node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)     ||
                    (node->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

                if(!node->State.MousePressed.has_value() && !modified)
                    continue;

                // setup focus to this node
                ImmedidateUserInterfaceNode* focused = node;
                ImmedidateUserInterfaceNode* parent  = focused->State.Parent;

                while(parent)
                {
                    ImmedidateUserInterfaceWindow* window =
                        dynamic_cast<ImmedidateUserInterfaceWindow*>(node);

                    focused = parent;
                    parent  = focused->State.Parent;
                }

                if(focused == nullptr)
                    continue;

                for (auto node : _Context->m_NodesRenderingList)
                {
                    node->State.InitialDepth =
                        ImmedidateUserInterfaceWindow::calculate_layer_depth(
                            _Context,
                            ImmedidateUserInterfaceWindow::ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Main);
                }

                focused->State.InitialDepth =
                    ImmedidateUserInterfaceWindow::calculate_layer_depth(
                        _Context,
                        ImmedidateUserInterfaceWindow::ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Focused);

                break;
            }

            // docking

            // find moving singleton window
            ImmedidateUserInterfaceWindow* moved = nullptr;

            for(auto singleton : _Context->m_Hierarchy.Singletons)
            {
                if(dynamic_cast<ImmedidateUserInterfaceWindow*>(singleton) &&
                    (singleton->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
                {
                    moved         = dynamic_cast<ImmedidateUserInterfaceWindow*>(singleton);
                    moved->Docker = nullptr;
                    break;
                }
            }

            // find hovered singleton window not equal to the moved one
            ImmedidateUserInterfaceWindow* hovered = nullptr;

            for(auto singleton : _Context->m_Hierarchy.Singletons)
            {
                if(dynamic_cast<ImmedidateUserInterfaceWindow*>(singleton)       &&
                    singleton->State.BoundingBox.contains(_Event.CursorPosition) &&
                    singleton != moved)
                {
                    hovered = dynamic_cast<ImmedidateUserInterfaceWindow*>(singleton);
                    break;
                }
            }

            // docking
            if(hovered == nullptr || moved == nullptr)
                return;

            // _Context->m_Renderer->push_rectangle_rounded_filled(
            //     hovered->DockingBox.Min,
            //     hovered->DockingBox.Max,
            //     32.f,
            //     gs_vec4f(255.f, 0.f, 0.f, 128.f),
            //     _Context->m_Renderer->calculate_transform_matrix((float)hovered->place_in_follow()));

            bool allMouseButtonsAreReleased = true;

            for (int button = ApplicationMouseButton::Button::ApplicationMouseButton_Begin;
                    button < ApplicationMouseButton::Button::ApplicationMouseButton_End;
                    button++)
            {
                allMouseButtonsAreReleased =
                    allMouseButtonsAreReleased && !application()->is_mouse_button_down((ApplicationMouseButton::Button)button);
            }

            if(!allMouseButtonsAreReleased || moved->Docker == hovered || hovered->Docker == moved)
                return;

            moved->Docker = hovered;

            for(auto it = _Context->m_DockAreas.begin(moved); it != _Context->m_DockAreas.end(moved); ++it)
            {
                ImmedidateUserInterfaceWindow* dockable =
                    dynamic_cast<ImmedidateUserInterfaceWindow*>((*it));

                if(dockable != nullptr)
                    dockable->Docker = hovered;
            }
        }
    };

    // build hierarchy
    m_Hierarchy.build(m_NodesRenderingList);
    m_DockAreas.build(m_NodesRenderingList);

    // static bool start = true;

    // if(start)
    // {
    //     for (auto& singleton : m_Hierarchy.Singletons)
    //         showHierarchy(this, singleton, "");
    // }

    // start = false;

    // construct events
    ImmedidateUserInterfaceNodeEvent event;
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
    UIWindowsController::execute(this, event);
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
                singleton->State.Depth = singleton->State.InitialDepth;
                
                for (auto& renderedNode : _Context->m_NodesRenderingCache)
                {
                    if(renderedNode->State.InitialDepth == singleton->State.InitialDepth)
                    {
                        singleton->State.Depth =
                            gs_max(
                                renderedNode->State.Depth + renderedNode->State.TotalThickness + 1,
                                singleton->State.Depth);
                    }
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

    GS_ASSERT(m_NodesRenderingStack.empty());

    m_NodesRenderingList.clear();
    m_NodesRenderingCache.clear();
    m_NodesRenderingStack.clear();
}

void ImmedidateUserInterfaceContextLayer::finish()
{
    if(m_Renderer != nullptr)
        m_Renderer->close();
}

bool ImmedidateUserInterfaceContextLayer::begin_window(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmedidateUserInterfaceWindow>(_ID, _Settings);
}

void ImmedidateUserInterfaceContextLayer::end_window()
{
    end_node<ImmedidateUserInterfaceWindow>();
}

bool ImmedidateUserInterfaceContextLayer::begin_vertial_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmedidateUserInterfaceVerticalStack>(_Name, _Settings);
}

void ImmedidateUserInterfaceContextLayer::end_vertical_stack()
{
    end_node<ImmedidateUserInterfaceVerticalStack>();
}

bool ImmedidateUserInterfaceContextLayer::begin_horizontal_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmedidateUserInterfaceHorizontalStack>(_Name, _Settings);
}

void ImmedidateUserInterfaceContextLayer::end_horizontal_stack()
{
    end_node<ImmedidateUserInterfaceHorizontalStack>();
}