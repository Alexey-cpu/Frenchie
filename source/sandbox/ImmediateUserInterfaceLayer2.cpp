#include <ImmediateUserInterfaceLayer2.hpp>

// STL
#include <algorithm>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        class ImmedidateUserInterfaceHelpers
        {
        public:

            // auxiliary lambdas
            static void clamp_bounding_box(ImmediateUserInterfaceNode* _Node, gs_2dboxf _BoundingBox)
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

            static gs_2d_ellipsef build_resize_top_left_ellipse(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Min, WindowResizeAngleGizmoRadius);
            }

            static gs_2d_ellipsef build_resize_top_right_ellipse(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
            }

            static gs_2d_ellipsef build_resize_bottom_left_ellipse(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Max - gs_vec2f(_Node->State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
            };

            static gs_2d_ellipsef build_resize_bottom_right_ellipse(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Max, WindowResizeAngleGizmoRadius);
            };

            static gs_2dboxf build_resize_top_box(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 8.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min - gs_vec2f(0.f, WindowResizeSideGizmoWidth),
                    _Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width(), WindowResizeSideGizmoWidth));
            };

            static gs_2dboxf build_resize_left_box(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 8.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min - gs_vec2f(WindowResizeSideGizmoWidth, 0.f),
                    _Node->State.BoundingBox.Min + gs_vec2f(WindowResizeSideGizmoWidth, _Node->State.BoundingBox.height()));
            };

            static gs_2dboxf build_resize_right_box(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 8.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width() - WindowResizeSideGizmoWidth, 0.f),
                    _Node->State.BoundingBox.Max + gs_vec2f(WindowResizeSideGizmoWidth, 0.f));
            };

            static gs_2dboxf build_resize_bottom_box(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 8.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min + gs_vec2f(0.f, _Node->State.BoundingBox.height() - WindowResizeSideGizmoWidth),
                    _Node->State.BoundingBox.Max + gs_vec2f(0.f, WindowResizeSideGizmoWidth));
            };
        };
    
        // layout
        struct ImmediateUserInterfaceNodeVerticalStack : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceNodeVerticalStack(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
            virtual ~ImmediateUserInterfaceNodeVerticalStack(){}

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr)
                    return;

                gs_vec2f position  = State.BoundingBox.Min;
                gs_vec2f totalsize = gs_vec2f(0.f, 0.f);

                // compute total size and children size scale
                for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
                {
                    if((*it) != nullptr)
                        totalsize += (*it)->State.BoundingBox.size();
                }

                gs_vec2f scale = State.BoundingBox.size() / totalsize;

                // layout children
                for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
                {
                    auto node = *it;

                    if(node == nullptr)
                        continue;

                    gs_vec2f size = gs_vec2f(State.BoundingBox.width(), (node->State.BoundingBox.size() * scale).y);
                    size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);
                    node->State.BoundingBox = gs_2dboxf(position, position + size);
                    position += gs_vec2f(0.f, size.y);
                }
            }
        };

        struct ImmediateUserInterfaceNodeHorizontalStack : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceNodeHorizontalStack(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
            virtual ~ImmediateUserInterfaceNodeHorizontalStack(){}

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr)
                    return;

                gs_vec2f position   = State.BoundingBox.Min;
                gs_vec2f totalsize  = gs_vec2f(0.f, 0.f);

                // compute total size and children size scale
                for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
                {
                    if((*it) != nullptr)
                        totalsize += (*it)->State.BoundingBox.size();
                }

                gs_vec2f scale = State.BoundingBox.size() / totalsize;

                // layout children
                for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
                {
                    auto node = *it;

                    if(node == nullptr)
                        continue;

                    gs_vec2f size = gs_vec2f((node->State.BoundingBox.size() * scale).x, State.BoundingBox.height());
                    size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);
                    node->State.BoundingBox = gs_2dboxf(position, position + size);
                    position += gs_vec2f(size.x, 0.f);
                }
            }
        };
    
        // windows
        struct ImmediateUserInterfaceWindow : public ImmediateUserInterfaceNodeVerticalStack
        {
        public:

            ImmediateUserInterfaceWindow(const std::string& _Name) : ImmediateUserInterfaceNodeVerticalStack(_Name){}
            virtual ~ImmediateUserInterfaceWindow(){}

            void render_frame(ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _BoundingBox, const std::string& _Title)
            {
                // background
                _Context->m_Renderer->push_rectangle_rounded_filled(
                    _BoundingBox.Min,
                    _BoundingBox.Max,
                    _Context->m_Style.FramesRadius,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowFrameBackground],
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // outline
                _Context->m_Renderer->push_rectangle_rounded(
                    _BoundingBox.Min,
                    _BoundingBox.Max,
                    _Context->m_Style.FramesRadius,
                    _Context->m_Style.FramesWidth,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowOutline],
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // title
                _Context->m_Renderer->push_text(
                    _Title,
                    _Context->m_Style.FontSize,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text],
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow(), _BoundingBox.Min));

                if(_BoundingBox.contains(_Context->m_Renderer->get_cursor_postion()))
                {
                    _Context->m_Renderer->push_rectangle_rounded(
                        _BoundingBox.Min,
                        _BoundingBox.Max,
                        _Context->m_Style.FramesRadius,
                        _Context->m_Style.FramesWidth,
                        _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }
            }

            void render_frame(ImmediateUserInterfaceContextLayer* _Context)
            {
                if(Docker != nullptr)
                    return;

                ImmedidateUserInterfaceEvent event;
                event.CursorPosition  = _Context->m_Renderer->get_cursor_postion();
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

                gs_vec2f position = FrameBox.Min;
                gs_vec2f size     = gs_vec2f((FrameBox.size() / (float)(_Context->m_WindowsDockingHierarchy.size(this) + 1)).x, FrameBox.size().y);

                // render self frame
                {
                    gs_2dboxf boundingBox = gs_2dboxf(position, position + size);

                    // render frame
                    render_frame(_Context, boundingBox, Name);

                    // pass focus and activity
                    if(boundingBox.contains(_Context->m_Renderer->get_cursor_postion()))
                    {
                        if(event.MousePressed.has_value())
                        {
                            setup_as_active_docking_window(_Context, this);
                            setup_as_active_focused_window(_Context, this);
                        }
                    }

                    position += gs_vec2f(size.x, 0.f);
                }

                // render docked windows frames
                for(auto it = _Context->m_WindowsDockingHierarchy.begin(this);
                         it != _Context->m_WindowsDockingHierarchy.end(this);
                         it++)
                {
                    gs_2dboxf boundingBox = gs_2dboxf(position, position + size);

                    // render frame
                    render_frame(_Context, boundingBox, (*it)->Name);

                    // some simple event processing
                    if(boundingBox.contains(_Context->m_Renderer->get_cursor_postion()))
                    {
                        // pass focus and activity
                        if(event.MousePressed.has_value())
                        {
                            setup_as_active_docking_window(_Context, dynamic_cast<ImmediateUserInterfaceWindow*>(*it));
                            setup_as_active_focused_window(_Context, dynamic_cast<ImmediateUserInterfaceWindow*>(*it));
                        }

                        // move
                        if( dynamic_cast<ImmediateUserInterfaceWindow*>(*it)->DockingActive                                                      &&
                            ((*it)->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable)            &&
                            !(((*it)->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)           ||
                                ((*it)->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)        ||
                                ((*it)->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)       ||
                                ((*it)->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)      ||
                                ((*it)->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)  ||
                                ((*it)->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                                ((*it)->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)     ||
                                ((*it)->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)))
                        {
                            if((event.MouseHold.has_value()))
                            {
                                ImmediateUserInterfaceWindow* movable =
                                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                                if(movable->Docker != nullptr &&
                                    !(movable->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoveStarted))
                                {
                                    movable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoveStarted;
                                    movable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;

                                    float delta = State.BoundingBox.Min.y - movable->Cache.BoundingBox.Min.y;

                                    movable->Cache.BoundingBox = gs_2dboxf(
                                        movable->Cache.BoundingBox.Min + gs_vec2f(0.f, delta),
                                        movable->Cache.BoundingBox.Max + gs_vec2f(0.f, delta));
                                }

                                State.Events = ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None;
                            }
                        }
                    }

                    position += gs_vec2f(size.x, 0.f);
                }
            }

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr || !is_partially_visible())
                    return;

                if(Docker != nullptr && !DockingActive)
                    return;

                // content background and outline frame
                {
                    // background
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                        State.BoundingBox.Max - _Context->m_Style.FramesWidth * 0.5f,
                        _Context->m_Style.FramesRadius,
                        _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowBackground],
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                    // background frame
                    _Context->m_Renderer->push_rectangle_rounded(
                        State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                        State.BoundingBox.Max - _Context->m_Style.FramesWidth * 0.5f,
                        _Context->m_Style.FramesRadius,
                        _Context->m_Style.FramesWidth,
                        _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowOutline],
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }

                // frame
                render_frame(_Context);
            }

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr)
                    return;

                // compute self geometry
                {
                    FrameBox = gs_2dboxf(
                        State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                        gs_vec2f(
                            State.BoundingBox.Max.x,
                            State.BoundingBox.Min.y + _Context->m_Style.FontSize) - _Context->m_Style.FramesWidth * 0.5f);

                    ContentBox = gs_2dboxf(
                        (Docker == nullptr ? gs_vec2f(FrameBox.Min.x, FrameBox.Max.y) : State.BoundingBox.Min),
                        State.BoundingBox.Max);
                }

                // layout docked windows
                {
                    for(auto it = _Context->m_WindowsDockingHierarchy.begin(this);
                             it != _Context->m_WindowsDockingHierarchy.end(this);
                             it++)
                    {
                        if((*it) != nullptr)
                            (*it)->State.BoundingBox = ContentBox;
                    }
                }

                // layout children vertically
                {
                    gs_vec2f position  = ContentBox.Min;
                    gs_vec2f totalsize = gs_vec2f(0.f, 0.f);

                    // compute total size and children size scale
                    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
                    {
                        if((*it) != nullptr)
                            totalsize += (*it)->State.BoundingBox.size();
                    }

                    gs_vec2f scale = ContentBox.size() / totalsize;

                    // layout children
                    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
                    {
                        auto node = *it;

                        if(node == nullptr)
                            continue;

                        gs_vec2f size = gs_vec2f(ContentBox.width(), (node->State.BoundingBox.size() * scale).y);
                        size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);
                        node->State.BoundingBox = gs_2dboxf(position, position + size);
                        position += gs_vec2f(0.f, size.y);
                    }
                }
            }

            static void setup_as_active_docking_window(
                ImmediateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceWindow*       _Docked)
            {
                if(_Context == nullptr || _Docked == nullptr)
                    return;

                for(auto it = _Context->m_WindowsDockingHierarchy.begin(_Docked->Docker ? _Docked->Docker : _Docked);
                         it != _Context->m_WindowsDockingHierarchy.end(_Docked->Docker ? _Docked->Docker : _Docked);
                         it++)
                {
                    ImmediateUserInterfaceWindow* dockable =
                        dynamic_cast<ImmediateUserInterfaceWindow*>((*it));

                    if(dockable != nullptr)
                        dockable->DockingActive = false;
                }

                if(_Docked->Docker != nullptr)
                    _Docked->Docker->DockingActive = false;

                _Docked->DockingActive =
                    _Docked->Docker != nullptr || _Context->m_WindowsDockingHierarchy.size(_Docked) > 0;
            }

            static void setup_as_active_focused_window(
                ImmediateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceWindow*       _Docked)
            {
                if(_Context == nullptr || _Docked == nullptr)
                    return;

                for (auto node : _Context->m_Hierarchy.Singletons)
                {
                    ImmediateUserInterfaceWindow* window =
                        dynamic_cast<ImmediateUserInterfaceWindow*>(node);

                    if(window != nullptr)
                        window->DockingFocused = false;
                }

                _Docked->DockingFocused = true;
            }

            ImmediateUserInterfaceWindow* Docker{nullptr};
            
            gs_2dboxf                     FrameBox;
            gs_2dboxf                     ContentBox;

            int                           DockingIndex         {-1};
            bool                          DockingActive        {false};
            bool                          DockingFocused       {false};
            bool                          ReattachDockedWindows{false};
        };
    }
}

// ImmedidateUserInterfaceNode
ImmediateUserInterfaceNode::ImmediateUserInterfaceNode(const std::string _Name) : Name(_Name){}
ImmediateUserInterfaceNode::~ImmediateUserInterfaceNode(){}

void ImmediateUserInterfaceNode::render(ImmediateUserInterfaceContextLayer* _Context)
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

void ImmediateUserInterfaceNode::layout(ImmediateUserInterfaceContextLayer* _Context){}

void ImmediateUserInterfaceNode::measure(ImmediateUserInterfaceContextLayer* _Context)
{
    gs_2dboxf box = State.BoundingBox;
    for (auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        box = gs_2dboxf(
            box.Min,
            (*it)->State.BoundingBox.Min,
            box.Max,
            (*it)->State.BoundingBox.Max);
    }

    State.ContentSize = box.size();
}

void ImmediateUserInterfaceNode::events(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
{
    // auxiliary lambdas
    auto render_resize_gizmo = [](
        ImmediateUserInterfaceContextLayer*    _Context,
        ImmediateUserInterfaceNode*             _Node,
        const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
    {
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
        {
            auto resizeTopLeft = ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(_Node);

            _Context->m_Renderer->push_arc_filled(
                resizeTopLeft.Center,
                resizeTopLeft.Radius,
                resizeTopLeft.Radius,
                0.f,
                360.f,
                _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                _Context->m_Renderer->calculate_transform_matrix((float)(_Node->Cache.Depth + _Node->Cache.TotalThickness)));
            return;
        }

        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
        {
            auto resizeTopRight = ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(_Node);

            _Context->m_Renderer->push_arc_filled(
                resizeTopRight.Center,
                resizeTopRight.Radius,
                resizeTopRight.Radius,
                0.f,
                360.f,
                _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                _Context->m_Renderer->calculate_transform_matrix((float)(_Node->Cache.Depth + _Node->Cache.TotalThickness)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
        {
            auto resizeBottomLeft = ImmedidateUserInterfaceHelpers::build_resize_bottom_left_ellipse(_Node);

            _Context->m_Renderer->push_arc_filled(
                resizeBottomLeft.Center,
                resizeBottomLeft.Radius,
                resizeBottomLeft.Radius,
                0.f,
                360.f,
                _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                _Context->m_Renderer->calculate_transform_matrix((float)(_Node->Cache.Depth + _Node->Cache.TotalThickness)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
        {
            auto resizeBottomRight = ImmedidateUserInterfaceHelpers::build_resize_bottom_right_ellipse(_Node);

            _Context->m_Renderer->push_arc_filled(
                resizeBottomRight.Center,
                resizeBottomRight.Radius,
                resizeBottomRight.Radius,
                0.f,
                360.f,
                _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                _Context->m_Renderer->calculate_transform_matrix((float)(_Node->Cache.Depth + _Node->Cache.TotalThickness)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
        {
            auto resizeTop = ImmedidateUserInterfaceHelpers::build_resize_top_box(_Node);

            _Context->m_Renderer->push_rectangle_rounded_filled(
                resizeTop.Min,
                resizeTop.Max,
                16.f,
                _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                _Context->m_Renderer->calculate_transform_matrix((float)(_Node->Cache.Depth + _Node->Cache.TotalThickness)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
        {
            auto resizeLeft = ImmedidateUserInterfaceHelpers::build_resize_left_box(_Node);

            _Context->m_Renderer->push_rectangle_rounded_filled(
                resizeLeft.Min,
                resizeLeft.Max,
                16.f,
                _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                _Context->m_Renderer->calculate_transform_matrix((float)(_Node->Cache.Depth + _Node->Cache.TotalThickness)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
        {
            auto resizeRight = ImmedidateUserInterfaceHelpers::build_resize_right_box(_Node);

            _Context->m_Renderer->push_rectangle_rounded_filled(
                resizeRight.Min,
                resizeRight.Max,
                16.f,
                _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                _Context->m_Renderer->calculate_transform_matrix((float)(_Node->Cache.Depth + _Node->Cache.TotalThickness)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
        {
            auto resizeBottom = ImmedidateUserInterfaceHelpers::build_resize_bottom_box(_Node);

            _Context->m_Renderer->push_rectangle_rounded_filled(
                resizeBottom.Min,
                resizeBottom.Max,
                16.f,
                _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                _Context->m_Renderer->calculate_transform_matrix((float)(_Node->Cache.Depth + _Node->Cache.TotalThickness)));
            return;
        }
    };

    auto check_cursor_intersection_with_resize_gizmo = [](
        ImmediateUserInterfaceNode*             _Node,
        const ImmedidateUserInterfaceEvent&     _Event,
        const ImmediateUserInterfaceNodeEvents& _ResizeEventType)->bool
    {
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
            return ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(_Node).contains(_Event.CursorPosition);

        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
            return ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
            return ImmedidateUserInterfaceHelpers::build_resize_bottom_left_ellipse(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
            return ImmedidateUserInterfaceHelpers::build_resize_bottom_right_ellipse(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
            return ImmedidateUserInterfaceHelpers::build_resize_top_box(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
            return ImmedidateUserInterfaceHelpers::build_resize_left_box(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
            return ImmedidateUserInterfaceHelpers::build_resize_right_box(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
            return ImmedidateUserInterfaceHelpers::build_resize_bottom_box(_Node).contains(_Event.CursorPosition);
        
        return false;
    };

    auto find_resizable_node = [&check_cursor_intersection_with_resize_gizmo](
        ImmediateUserInterfaceContextLayer*    _Context,
        ImmediateUserInterfaceNode*             _Node,
        const ImmediateUserInterfaceNodeEvents& _ResizeEventType,
        const ImmedidateUserInterfaceEvent&     _Event)
    {
        // find resizable node
        ImmediateUserInterfaceNode* resizable = _Node;
        
        if(resizable->State.Parent != nullptr)
        {
            // pass event to a parent
            while (resizable->State.Parent &&
                    check_cursor_intersection_with_resize_gizmo(resizable->State.Parent, _Event, _ResizeEventType))
                resizable = resizable->State.Parent;
        }

        // if this is a window then we pass event to a docker
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(resizable);

        if(window != nullptr && window->Docker != nullptr)
            resizable = window->Docker;

        return resizable;
    };

    auto resize_node = [](
        ImmediateUserInterfaceNode*             _Node,
        const ImmediateUserInterfaceNodeEvents& _ResizeEventType,
        const ImmedidateUserInterfaceEvent&     _Event)
    {
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
        {
            ImmedidateUserInterfaceHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + _Event.CursorDragDelta,
                _Node->Cache.BoundingBox.Max));
            return;
        }

        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
        {
            ImmedidateUserInterfaceHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Node->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
        {
            ImmedidateUserInterfaceHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
        {
            ImmedidateUserInterfaceHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min,
                _Node->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta()));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
        {
            ImmedidateUserInterfaceHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Node->Cache.BoundingBox.Max));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
        {
            ImmedidateUserInterfaceHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Node->Cache.BoundingBox.Max));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
        {
            ImmedidateUserInterfaceHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min,
                _Node->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
        {
            ImmedidateUserInterfaceHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min,
                _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
            return;
        }
    };

    // focus
    if(State.MousePressed.has_value())
    {
        ImmediateUserInterfaceNode* focused = nullptr;
        ImmediateUserInterfaceNode* parent  = State.Parent;

        while (parent)
        {
            focused = parent;
            parent  = parent->State.Parent;
        }
        
        if(dynamic_cast<ImmediateUserInterfaceWindow*>(focused))
        {
            ImmediateUserInterfaceWindow::setup_as_active_focused_window(_Context, dynamic_cast<ImmediateUserInterfaceWindow*>(focused));
            ImmediateUserInterfaceWindow::setup_as_active_docking_window(_Context, dynamic_cast<ImmediateUserInterfaceWindow*>(focused));
        }
    }

    // resize
    if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable) &&
        !(State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
    {
        if(check_cursor_intersection_with_resize_gizmo(this, _Event, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft))
        {
            ImmediateUserInterfaceNode* resizable =
                find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft, _Event);

            render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft);

            // trigger event
            if(_Event.MousePressed.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft;
                return;
            }

            // execute event
            if(_Event.MouseDown.has_value() &&
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft))
            {
                resize_node(resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft, _Event);
                return;
            }
        }
        else if(check_cursor_intersection_with_resize_gizmo(this, _Event, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
        {
            ImmediateUserInterfaceNode* resizable =
                find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight, _Event);

            render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

            // trigger event
            if(_Event.MousePressed.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight;
                return;
            }

            // execute event
            if(_Event.MouseDown.has_value() &&
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
            {
                resize_node(resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight, _Event);
                return;
            }
        }
        else if(check_cursor_intersection_with_resize_gizmo(this, _Event, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
        {
            ImmediateUserInterfaceNode* resizable =
                find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft, _Event);

            render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);

            // trigger event
            if(_Event.MousePressed.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft;
                return;
            }

            // execute event
            if(_Event.MouseDown.has_value() &&
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
            {
                resize_node(resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft, _Event);
                return;
            }
        }
        else if(check_cursor_intersection_with_resize_gizmo(this, _Event, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
        {
            ImmediateUserInterfaceNode* resizable =
                find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight, _Event);

            render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

            // trigger event
            if(_Event.MousePressed.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight;
                return;
            }

            // execute event
            if(_Event.MouseDown.has_value() &&
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
            {
                resize_node(resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight, _Event);
                return;
            }
        }
        else if(check_cursor_intersection_with_resize_gizmo(this, _Event, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop))
        {
            ImmediateUserInterfaceNode* resizable =
                find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop, _Event);

            render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop);

            // trigger event
            if(_Event.MousePressed.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop;
                return;
            }

            // execute event
            if(_Event.MouseDown.has_value() &&
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop))
            {
                resize_node(resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop, _Event);
                return;
            }
        }
        else if(check_cursor_intersection_with_resize_gizmo(this, _Event, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft))
        {
            ImmediateUserInterfaceNode* resizable =
                find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft, _Event);

            render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft);

            // trigger event
            if(_Event.MousePressed.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft;
                return;
            }

            // execute event
            if(_Event.MouseDown.has_value() &&
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft))
            {
                resize_node(resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft, _Event);
                return;
            }
        }
        else if(check_cursor_intersection_with_resize_gizmo(this, _Event, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
        {
            ImmediateUserInterfaceNode* resizable =
                find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight, _Event);

            render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);

            // trigger event
            if(_Event.MousePressed.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight;
                return;
            }

            // execute event
            if(_Event.MouseDown.has_value() &&
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
            {
                resize_node(resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight, _Event);
                return;
            }
        }
        else if(check_cursor_intersection_with_resize_gizmo(this, _Event, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
        {
            ImmediateUserInterfaceNode* resizable =
                find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom, _Event);

            render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);

            // trigger event
            if(_Event.MousePressed.has_value())
            {
                resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom;
                return;
            }

            // execute event
            if(_Event.MouseDown.has_value() &&
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
            {
                resize_node(resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom, _Event);
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
        if((_Event.MouseHold.has_value() && State.BoundingBox.contains(_Event.CursorPosition)) ||
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

bool ImmediateUserInterfaceNode::is_visible() const
{
    if(State.Parent == nullptr)
        return true;

    gs_2dboxf box = gs_2dboxf(
        State.Parent->State.BoundingBox.Min - gs_vec2f(12.f),
        State.Parent->State.BoundingBox.Max + gs_vec2f(12.f)
    );

    return box.contains(State.BoundingBox);
}

bool ImmediateUserInterfaceNode::is_partially_visible() const
{
    if(State.Parent == nullptr)
        return true;

    gs_2dboxf box = gs_2dboxf(
        State.Parent->State.BoundingBox.Min - gs_vec2f(12.f),
        State.Parent->State.BoundingBox.Max + gs_vec2f(12.f)
    );

    return box.overlaps(State.BoundingBox);
}

int ImmediateUserInterfaceNode::place_in_follow()
{
    State.TotalThickness += (++State.SelfThickness);
    return State.Depth + State.SelfThickness;
}

// ImmediateUserInterfaceContextLayer2
ImmediateUserInterfaceContextLayer::ImmediateUserInterfaceContextLayer(){}
ImmediateUserInterfaceContextLayer::~ImmediateUserInterfaceContextLayer(){}

bool ImmediateUserInterfaceContextLayer::awake()
{
    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

    m_Hierarchy = ImmedidateUserInterfaceNodeHierarchy(
        [](ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
        {
            return _Node != nullptr ? _Node->State.Parent : nullptr;
        });

    m_WindowsDockingHierarchy = ImmedidateUserInterfaceNodeHierarchy(
        [](ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
        {
            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(_Node);

            return window != nullptr ? window->Docker : nullptr;
        });

    return m_Renderer != nullptr;
}

void ImmediateUserInterfaceContextLayer::frame_start(){}

void ImmediateUserInterfaceContextLayer::frame_update()
{
}

// void showHierarchy(ImmediateUserInterfaceContextLayer* _Context, ImmedidateUserInterfaceNode* _Node, const std::string& _Delimiter)
// {
//     std::cout << _Delimiter << _Node->Name << "\n";

//     for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
//     {
//         showHierarchy(_Context, (*it), _Delimiter + "\t");
//     }
// }

void ImmediateUserInterfaceContextLayer::frame_debug()
{
    class ImmedidateUserInterfaceLayoutController
    {
    public:
        static void execute(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent&)
        {
            for (auto& singleton : _Context->m_Hierarchy.Singletons)
                ImmedidateUserInterfaceLayoutController::node_layout(_Context, singleton);
        }

    private:
        static void node_layout(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
        {
            if(_Context == nullptr || _Node == nullptr)
                return;

            _Node->layout(_Context);
            _Node->measure(_Context);

            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
                node_layout(_Context, (*it));
        }
    };

    class ImmedidateUserInterfaceEventsController
    {
    public:
        static void execute(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            catch_hover(_Context, _Event);
            catch_input(_Context, _Event);
            catch_event(_Context, _Event);
        }

    private:

        static void catch_hover(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            if(_Context == nullptr)
                return;

            // find the top most node catching the mouse cursor
            ImmediateUserInterfaceNode* hoveredNode  = nullptr;
            int                          maximumDepth = INT_MIN;

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

        static void catch_input(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
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

        static void catch_event(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            if(_Context == nullptr)
                return;

            // check if anything is already catching event
            bool catchingEvent = false;

            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(node->State.Events == ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
                    continue;

                node->events(_Context, _Event);
                catchingEvent = true;
            }

            if(catchingEvent)
                return;

            // find the top most hovered node
            ImmediateUserInterfaceNode* hoveredNode  = nullptr;
            int                         maximumDepth = INT_MIN;

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
            
            // process hovered node events
            hoveredNode->events(_Context, _Event);

            // check in-parent intersection and process events of intersected nodes
            for (auto it  = _Context->m_Hierarchy.begin(hoveredNode->State.Parent);
                      it != _Context->m_Hierarchy.end(hoveredNode->State.Parent);
                      it++)
            {
                if((*it) == hoveredNode)
                    continue;

                if(gs_2dboxf(
                    (*it)->State.BoundingBox.Min - gs_vec2f(16.f), // TODO: may be, this should be a setting ???
                    (*it)->State.BoundingBox.Max + gs_vec2f(16.f)).contains(_Event.CursorPosition))
                {
                    (*it)->events(_Context, _Event);
                }
            }
        }
    
    };

    class ImmedidateUserInterfaceWindowController
    {
    public:
        static void execute(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            place_on_dockers(_Context, _Event);
            place_on_layers(_Context, _Event);
            build_docking_hierarchy(_Context);
        }

    private:

        enum ImmedidateUserInterfaceWindowLayer_ : int
        {
            ImmedidateUserInterfaceWindowLayer_Begin   = 0,
            ImmedidateUserInterfaceWindowLayer_Main    = ImmedidateUserInterfaceWindowLayer_Begin,
            ImmedidateUserInterfaceWindowLayer_Active,
            ImmedidateUserInterfaceWindowLayer_Focused,
            ImmedidateUserInterfaceWindowLayer_End,
        };

        static void place_on_layers(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent&)
        {
            // auxiliary lambdas
            auto calculate_layer_depth = [](ImmediateUserInterfaceContextLayer* _Context, int _Layer)->int
            {
                return (int)(_Layer * _Context->m_Renderer->get_far_plane() / (ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_End - ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Begin));
            };

            // main code
            for(auto singleton : _Context->m_Hierarchy.Singletons)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(singleton);

                if(window == nullptr)
                    continue;

                // if the window is not being docked and is not a docker then we check only focus
                // otherwise we check both focus and activity
                if(window->Docker == nullptr &&
                    (retrieve_docked_windows(_Context, window).empty()))
                {
                    if(window->DockingFocused)
                    {
                        window->State.InitialDepth = calculate_layer_depth(
                            _Context,
                            ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Focused);
                    }
                    else
                    {
                        window->State.InitialDepth = calculate_layer_depth(
                            _Context,
                            ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Main);
                    }
                }
                else
                {
                    if(window->DockingFocused)
                    {
                        window->State.InitialDepth = calculate_layer_depth(
                            _Context,
                            ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Focused);
                    }
                    else if(window->DockingActive)
                    {
                        window->State.InitialDepth = calculate_layer_depth(
                            _Context,
                            ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Active);
                    }
                    else
                    {
                        window->State.InitialDepth = calculate_layer_depth(
                            _Context,
                            ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Main);
                    }
                }
            }
        }

        static void place_on_dockers(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            // find moving singleton window
            ImmediateUserInterfaceWindow* moved = nullptr;

            for(auto singleton : _Context->m_Hierarchy.Singletons)
            {
                if(dynamic_cast<ImmediateUserInterfaceWindow*>(singleton) &&
                    (singleton->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
                {
                    moved = dynamic_cast<ImmediateUserInterfaceWindow*>(singleton);
                    ImmedidateUserInterfaceWindowController::detach_from_docker(_Context, moved);
                    break;
                }
            }

            // find top most hovered singleton window not equal to the moved one
            ImmediateUserInterfaceWindow* hovered  = nullptr;

            for(auto singleton : _Context->m_Hierarchy.Singletons)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(singleton);

                if( window != nullptr                                         &&
                    window->State.BoundingBox.contains(_Event.CursorPosition) &&
                    window != moved)
                {
                    if(hovered == nullptr || window->Cache.Depth > hovered->Cache.Depth)
                        hovered  = window;
                }
            }

            if(hovered == nullptr || moved == nullptr)
                return;

            // dock the moved window to the hovered one if all mouse buttons are released
            bool allMouseButtonsAreReleased = true;

            for (int button = ApplicationMouseButton::Button::ApplicationMouseButton_Begin;
                     button < ApplicationMouseButton::Button::ApplicationMouseButton_End;
                     button++)
            {
                allMouseButtonsAreReleased =
                    allMouseButtonsAreReleased && !application()->is_mouse_button_down((ApplicationMouseButton::Button)button);
            }

            if(allMouseButtonsAreReleased)
            {
                ImmedidateUserInterfaceWindowController::attach_to_docker(_Context, hovered, moved);
            }
            else if(moved != hovered && moved->Docker != hovered && hovered->Docker != moved)
            {
                // render potential docking window gizmo
                int depth = hovered->Cache.Depth + hovered->Cache.TotalThickness + 1;

                _Context->m_Renderer->push_rectangle_rounded_filled(
                    hovered->ContentBox.Min,
                    hovered->ContentBox.Max,
                    _Context->m_Style.FramesRadius,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                    _Context->m_Renderer->calculate_transform_matrix((float)depth));
            }
        }

        static void build_docking_hierarchy(ImmediateUserInterfaceContextLayer* _Context)
        {
            // build hierarchy
            _Context->m_WindowsDockingHierarchy.build(_Context->m_NodesRenderingList);

            // sort docked windows by their docking index within hierarchy
            for(auto singleton : _Context->m_Hierarchy.Singletons)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(singleton);

                if(window == nullptr)
                    continue;

                std::sort(
                    _Context->m_WindowsDockingHierarchy.begin(window),
                    _Context->m_WindowsDockingHierarchy.end(window),
                    [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
                    {
                        return dynamic_cast<const ImmediateUserInterfaceWindow*>(_A)->DockingIndex <
                                dynamic_cast<const ImmediateUserInterfaceWindow*>(_B)->DockingIndex;
                    });
            }
        }

        static std::vector<ImmediateUserInterfaceNode*>& retrieve_docked_windows(
            ImmediateUserInterfaceContextLayer* _Context,
            ImmediateUserInterfaceWindow*        _Docker)
        {
            // get ready
            _Context->m_WindowsDockingList.clear();

            // retrieve windows docked by a docker
            for(auto node : _Context->m_Hierarchy.Singletons)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(node);

                if(window != nullptr && window->Docker == _Docker)
                    _Context->m_WindowsDockingList.push_back(window);
            }

            // sort windows docked by a docker by their docking indexes
            std::sort(
                _Context->m_WindowsDockingList.begin(),
                _Context->m_WindowsDockingList.end(),
                [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
                {
                    return dynamic_cast<const ImmediateUserInterfaceWindow*>(_A)->DockingIndex <
                            dynamic_cast<const ImmediateUserInterfaceWindow*>(_B)->DockingIndex;
                });

            return _Context->m_WindowsDockingList;
        }

        static void attach_to_docker(
            ImmediateUserInterfaceContextLayer* _Context,
            ImmediateUserInterfaceWindow*        _Docker,
            ImmediateUserInterfaceWindow*        _Docked)
        {
            // auxiliary lambdas
            auto move_to_cache = [](
                ImmediateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceWindow*        _Docker)
            {
                if(_Context == nullptr || _Docker == nullptr)
                    return;

                 _Context->m_WindowsDockingCache.push_back(_Docker);
            };

            auto move_child_docked_windows_to_cache = [](
                ImmediateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceWindow*        _Docker)
            {
                if(_Context == nullptr || _Docker == nullptr)
                    return;

                // retrieve docked windows
                auto& dockedWindows = retrieve_docked_windows(_Context, _Docker);

                // move to cache
                for(auto it  = dockedWindows.begin();
                         it != dockedWindows.end();
                         it++)
                {
                    _Context->m_WindowsDockingCache.push_back(*it);
                }
            };

            if(_Context == nullptr || _Docker == nullptr || _Docked == nullptr || _Docked->Docker == _Docker || _Docker->Docker == _Docked)
                return;

            // get ready
            _Context->m_WindowsDockingCache.clear();

            auto docker = _Docker->Docker ? _Docker->Docker : _Docker;

            // move child docked windows and self to windows docking cache
            move_child_docked_windows_to_cache(_Context, docker);
            move_to_cache(_Context, _Docked);
            move_child_docked_windows_to_cache(_Context, _Docked);

            // reindex docked nodes and setup their docker
            int dockindex = 0;

            for(auto it  = _Context->m_WindowsDockingCache.begin();
                     it != _Context->m_WindowsDockingCache.end();
                     it++)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                if(window == nullptr)
                    continue;

                window->Docker       = docker;
                window->DockingIndex = dockindex++;
            }

            // setup self as active
            ImmediateUserInterfaceWindow::setup_as_active_docking_window(_Context, _Docked);

            // clear
            _Context->m_WindowsDockingCache.clear();
            _Context->m_WindowsDockingList.clear();
        }
    
        static void detach_from_docker(
            ImmediateUserInterfaceContextLayer* _Context,
            ImmediateUserInterfaceWindow*        _Detached)
        {
            if(_Detached == nullptr)
                return;

            // reindex docked nodes of the docker of the moved node
            if(_Detached->Docker != nullptr)
            {
                auto& dockedWindows = retrieve_docked_windows(_Context, _Detached->Docker);

                int dockindex = 0;

                for(auto it  = dockedWindows.begin();
                         it != dockedWindows.end();
                         it++)
                {
                    ImmediateUserInterfaceWindow* window =
                        dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                    if(window != nullptr && window != _Detached)
                        window->DockingIndex = dockindex++;
                }

                _Detached->Docker        = nullptr;
                _Detached->DockingActive = false;
                _Detached->DockingIndex  = -1;
                return;
            }

            // TODO: reattach children to the first child
            _Detached->Docker        = nullptr;
            _Detached->DockingIndex  = -1;
        }
    
    };

    // build hierarchy
    m_Hierarchy.build(m_NodesRenderingList);

    // construct events
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

    ImmedidateUserInterfaceEventsController::execute(this, event);
    ImmedidateUserInterfaceWindowController::execute(this, event);
    ImmedidateUserInterfaceLayoutController::execute(this, event);
}

void ImmediateUserInterfaceContextLayer::frame_render()
{
    class UINodeRenderer
    {
    public:
        static void execute(ImmediateUserInterfaceContextLayer* _Context)
        {
            // get ready
            _Context->m_NodesRenderingCache.clear();

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

        static void render_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
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
                _Node->State.Parent->State.TotalThickness += _Node->State.TotalThickness;

            // remove clipping
            _Context->m_Renderer->pop_clip_box();
        }
    };

    UINodeRenderer::execute(this);
}

void ImmediateUserInterfaceContextLayer::frame_finish()
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

    // docking
    m_WindowsDockingCache.clear();
    m_WindowsDockingList.clear();
}

void ImmediateUserInterfaceContextLayer::finish()
{
    if(m_Renderer != nullptr)
        m_Renderer->close();
}

bool ImmediateUserInterfaceContextLayer::begin_window(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, bool* _Opened)
{
    return begin_node<ImmediateUserInterfaceWindow>(_ID, _Settings, _Opened);
}

void ImmediateUserInterfaceContextLayer::end_window()
{
    end_node<ImmediateUserInterfaceWindow>();
}

bool ImmediateUserInterfaceContextLayer::begin_vertial_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceNodeVerticalStack>(_Name, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_vertical_stack()
{
    end_node<ImmediateUserInterfaceNodeVerticalStack>();
}

bool ImmediateUserInterfaceContextLayer::begin_horizontal_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceNodeHorizontalStack>(_Name, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_horizontal_stack()
{
    end_node<ImmediateUserInterfaceNodeHorizontalStack>();
}