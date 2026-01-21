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

            virtual void layout(ImmedidateUserInterfaceContextLayer* _Context) override
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

            virtual void layout(ImmedidateUserInterfaceContextLayer* _Context) override
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

            enum ImmedidateUserInterfaceWindowLayer_ : int
            {
                ImmedidateUserInterfaceWindowLayer_Begin   = 0,
                ImmedidateUserInterfaceWindowLayer_Main    = ImmedidateUserInterfaceWindowLayer_Begin,
                ImmedidateUserInterfaceWindowLayer_Active,
                ImmedidateUserInterfaceWindowLayer_Focused,
                ImmedidateUserInterfaceWindowLayer_End,
            };

            ImmediateUserInterfaceWindow(const std::string& _Name) : ImmediateUserInterfaceNodeVerticalStack(_Name){}
            virtual ~ImmediateUserInterfaceWindow(){}

            virtual void render(ImmedidateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr || !is_partially_visible())
                    return;

                if(Docker != nullptr && !DockingActive)
                    return;

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

            virtual void layout(ImmedidateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr)
                    return;

                // docking
                gs_2dboxf frameBox = gs_2dboxf(
                    State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                    gs_vec2f(
                        State.BoundingBox.Max.x,
                        State.BoundingBox.Min.y + _Context->m_Style.FontSize) - _Context->m_Style.FramesWidth * 0.5f);

                DockingBox = gs_2dboxf(
                    (Docker == nullptr ? gs_vec2f(frameBox.Min.x, frameBox.Max.y) : State.BoundingBox.Min),
                    State.BoundingBox.Max);

                for(auto it = _Context->m_DockAreas.begin(this); it != _Context->m_DockAreas.end(this); ++it)
                {
                    if((*it) != nullptr)
                        (*it)->State.BoundingBox = DockingBox;
                }

                // self layouting
                ImmediateUserInterfaceNodeVerticalStack::layout(_Context);
            }

            static int calculate_layer_depth(ImmedidateUserInterfaceContextLayer* _Context, int _Layer)
            {
                return (int)(_Layer * _Context->m_Renderer->get_far_plane() / (ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_End - ImmedidateUserInterfaceWindowLayer_::ImmedidateUserInterfaceWindowLayer_Begin));
            }

            static void setup_as_active_docking_window(
                ImmedidateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceWindow*       _Docked)
            {
                if(_Context == nullptr || _Docked == nullptr)
                    return;

                for(auto it = _Context->m_DockAreas.begin(_Docked->Docker ? _Docked->Docker : _Docked);
                         it != _Context->m_DockAreas.end(_Docked->Docker ? _Docked->Docker : _Docked);
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
                    _Docked->Docker != nullptr || _Context->m_DockAreas.size(_Docked) > 0;
            }

            static void setup_as_active_focused_window(
                ImmedidateUserInterfaceContextLayer* _Context,
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
            gs_2dboxf                      DockingBox;
            int                            DockingIndex  {-1};
            bool                           DockingActive {false};
            bool                           DockingFocused{false};
        };
    
        struct ImmediateUserInterfaceWindowFrame : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceWindowFrame(const std::string& _ID) : ImmediateUserInterfaceNode(_ID){}
            virtual ~ImmediateUserInterfaceWindowFrame(){}

            virtual void render(ImmedidateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr || !is_partially_visible())
                    return;

                // background
                if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
                    State.BoundingBox.contains(_Context->m_Renderer->get_cursor_postion()))
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                        State.BoundingBox.Max - _Context->m_Style.FramesWidth * 0.5f,
                        _Context->m_Style.FramesRadius,
                        _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowFrameBackgroundHovered],
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                    if(State.MouseClicked.has_value())
                    {
                        // TODO: what to do on mouse click ??
                    }
                }
                else
                {
                    if(Window != nullptr && Window->DockingActive)
                    {
                        _Context->m_Renderer->push_rectangle_rounded_filled(
                            State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                            State.BoundingBox.Max - _Context->m_Style.FramesWidth * 0.5f,
                            _Context->m_Style.FramesRadius,
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowFrameBackgroundActive],
                            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                    }
                    else
                    {
                        _Context->m_Renderer->push_rectangle_rounded_filled(
                            State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                            State.BoundingBox.Max - _Context->m_Style.FramesWidth * 0.5f,
                            _Context->m_Style.FramesRadius,
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowFrameBackground],
                            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                    }
                }

                // frame
                _Context->m_Renderer->push_rectangle_rounded(
                    State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                    State.BoundingBox.Max - _Context->m_Style.FramesWidth * 0.5f,
                    _Context->m_Style.FramesRadius,
                    _Context->m_Style.FramesWidth,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowOutline],
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // title
                _Context->m_Renderer->push_text(
                    Title,
                    _Context->m_Style.FontSize,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text],
                    _Context->m_Renderer->calculate_transform_matrix(
                        (float)place_in_follow(),
                        gs_vec2f(
                            State.BoundingBox.Min.x + gs_max(_Context->m_Style.FramesWidth * 2.f, 16.f),
                            State.BoundingBox.center().y - _Context->m_Renderer->calculate_bounding_box(Title, _Context->m_Style.FontSize, _Context->m_Style.Font).height() * 0.5f)),
                    _Context->m_Style.Font);
            }

            virtual void layout(ImmedidateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr)
                    return;

                ImmediateUserInterfaceNode::layout(_Context);
                State.MinimumSize = gs_vec2f(4.f, _Context->m_Style.FontSize);
                State.MaximumSize = gs_vec2f((float)INT_MAX, _Context->m_Style.FontSize);
            }

            virtual void events(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event) override
            {
                if(_Context == nullptr || Window == nullptr) 
                    return;

                if(State.MousePressed.has_value())
                {
                    ImmediateUserInterfaceWindow::setup_as_active_focused_window(_Context, Window);
                    ImmediateUserInterfaceWindow::setup_as_active_docking_window(_Context, Window);
                }

                // pass event to the window
                if(Window->Docker == nullptr)
                {
                    Window->events(_Context, _Event);
                    return;
                }

                // resize
                if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable) &&
                    !(State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
                {
                    if((ImmedidateUserInterfaceHelpers::build_resize_top_box(this).contains(_Event.CursorPosition) ||
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)))
                    {
                        // render resize gizmo
                        auto resizeTopLeft = ImmedidateUserInterfaceHelpers::build_resize_top_box(this);

                        _Context->m_Renderer->push_rectangle_rounded_filled(
                            resizeTopLeft.Min,
                            resizeTopLeft.Max,
                            16.f,
                            gs_vec4f(5, 255, 255, 200.f),
                            _Context->m_Renderer->calculate_transform_matrix((float)(Cache.Depth + Cache.TotalThickness)));

                        // trigger event
                        if(_Event.MousePressed.has_value())
                        {
                            Window->Docker->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop;
                            return;
                        }

                        // execute event
                        if(_Event.MouseDown.has_value())
                        {
                            ImmedidateUserInterfaceHelpers::clamp_bounding_box(Window, gs_2dboxf(
                                Window->Docker->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                                Window->Docker->Cache.BoundingBox.Max));
                            return;
                        }
                    }

                    if((ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(this).contains(_Event.CursorPosition) ||
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)))
                    {
                        // render resize gizmo
                        auto resizeTopLeft = ImmedidateUserInterfaceHelpers::build_resize_top_left_ellipse(this);

                        _Context->m_Renderer->push_arc_filled(
                            resizeTopLeft.Center,
                            resizeTopLeft.Radius,
                            resizeTopLeft.Radius,
                            0.f,
                            360.f,
                            gs_vec4f(5, 255, 255, 200.f),
                            _Context->m_Renderer->calculate_transform_matrix((float)(Cache.Depth + Cache.TotalThickness)));

                        // trigger event
                        if(_Event.MousePressed.has_value())
                        {
                            Window->Docker->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft;
                            return;
                        }

                        // execute event
                        if(_Event.MouseDown.has_value())
                        {
                            ImmedidateUserInterfaceHelpers::clamp_bounding_box(Window, gs_2dboxf(
                                Window->Docker->Cache.BoundingBox.Min + _Event.CursorDragDelta,
                                Window->Docker->Cache.BoundingBox.Max));
                            return;
                        }
                    }
                    
                    if((ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(this).contains(_Event.CursorPosition) ||
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)))
                    {
                        // render resize gizmo
                        auto resizeTopRight = ImmedidateUserInterfaceHelpers::build_resize_top_right_ellipse(this);

                        _Context->m_Renderer->push_arc_filled(
                            resizeTopRight.Center,
                            resizeTopRight.Radius,
                            resizeTopRight.Radius,
                            0.f,
                            360.f,
                            gs_vec4f(5, 255, 255, 200.f),
                            _Context->m_Renderer->calculate_transform_matrix((float)(Cache.Depth + Cache.TotalThickness)));

                        // trigger event
                        if(_Event.MousePressed.has_value())
                        {
                            Window->Docker->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight;
                            return;
                        }

                        // execute event
                        if(_Event.MouseDown.has_value())
                        {
                            ImmedidateUserInterfaceHelpers::clamp_bounding_box(Window->Docker, gs_2dboxf(
                                Window->Docker->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                                Window->Docker->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
                            return;
                        }
                    }
                }

                // move
                if((Window->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable) &&
                    !((Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)           ||
                        (Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)        ||
                        (Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)       ||
                        (Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)      ||
                        (Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)  ||
                        (Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                        (Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)     ||
                        (Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)))
                {
                    if((_Event.MouseHold.has_value() && State.BoundingBox.contains(_Event.CursorPosition)) ||
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
                    {
                        if(Window->Docker != nullptr &&
                            !(Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoveStarted))
                        {
                            Window->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoveStarted;
                            Window->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;

                            float delta = State.BoundingBox.Min.y - Window->Cache.BoundingBox.Min.y; 

                            Window->Cache.BoundingBox = gs_2dboxf(
                                Window->Cache.BoundingBox.Min + gs_vec2f(0.f, delta),
                                Window->Cache.BoundingBox.Max + gs_vec2f(0.f, delta));
                        }
                        else
                        {
                            Window->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoveStarted;
                            Window->State.Events = ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;

                            Window->State.BoundingBox = gs_2dboxf(
                                Window->Cache.BoundingBox.Min + application()->get_window_cursor_dragdelta(),
                                Window->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta());
                        }

                        return;
                    }
                }
            }

            std::string                    Title {std::string()};
            ImmediateUserInterfaceWindow* Window{nullptr};
        };
        
        struct ImmediateUserInterfaceWindowFrameBox : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceWindowFrameBox(const std::string& _ID) : ImmediateUserInterfaceNode(_ID){}
            virtual ~ImmediateUserInterfaceWindowFrameBox(){}

            virtual void render(ImmedidateUserInterfaceContextLayer*) override{}

            virtual void layout(ImmedidateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr)
                    return;

                // setup min/max sizes
                State.MinimumSize = gs_vec2f(4.f, _Context->m_Style.FontSize);
                State.MaximumSize = gs_vec2f((float)INT_MAX, _Context->m_Style.FontSize);

                // layout children
                gs_vec2f position = State.BoundingBox.Min;

                for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); ++it)
                {
                    auto node = *it;

                    if(node == nullptr)
                        continue;

                    // compute size
                    gs_vec2f size = gs_vec2f(
                        (State.BoundingBox.size() / (float)(_Context->m_Hierarchy.end(this) - _Context->m_Hierarchy.begin(this))).x,
                        State.BoundingBox.height());
                    size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);

                    node->State.BoundingBox = gs_2dboxf(position, position + size);

                    position += gs_vec2f(size.x, 0.f);
                }
            }
        };
    
        // docking window gizmos
        struct ImmedaiateUserInterfaceDockingWindowGizmo : public ImmediateUserInterfaceWindow
        {
        public:
            ImmedaiateUserInterfaceDockingWindowGizmo(const std::string& _ID) : ImmediateUserInterfaceWindow(_ID){}
            virtual ~ImmedaiateUserInterfaceDockingWindowGizmo(){}

            virtual void render(ImmedidateUserInterfaceContextLayer*) override{}
            virtual void layout(ImmedidateUserInterfaceContextLayer*) override{}
            virtual void events(ImmedidateUserInterfaceContextLayer*, const ImmedidateUserInterfaceEvent&) override{}
        };

        struct ImmedaiateUserInterfaceDockingWindowFrameGizmo : public ImmediateUserInterfaceNode
        {
        public:
            ImmedaiateUserInterfaceDockingWindowFrameGizmo(const std::string& _ID) : ImmediateUserInterfaceNode(_ID){}
            virtual ~ImmedaiateUserInterfaceDockingWindowFrameGizmo(){}

            virtual void render(ImmedidateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr || !is_partially_visible())
                    return;

                // background
                _Context->m_Renderer->push_rectangle_rounded_filled(
                    State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                    State.BoundingBox.Max - _Context->m_Style.FramesWidth * 0.5f,
                    _Context->m_Style.FramesRadius,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // frame
                _Context->m_Renderer->push_rectangle_rounded(
                    State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                    State.BoundingBox.Max - _Context->m_Style.FramesWidth * 0.5f,
                    _Context->m_Style.FramesRadius,
                    _Context->m_Style.FramesWidth,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }

            virtual void layout(ImmedidateUserInterfaceContextLayer*) override
            {
            }

            virtual void events(ImmedidateUserInterfaceContextLayer*, const ImmedidateUserInterfaceEvent&) override
            {
            }
        };
    }
}

// ImmedidateUserInterfaceNode
ImmediateUserInterfaceNode::ImmediateUserInterfaceNode(const std::string _Name) : Name(_Name){}
ImmediateUserInterfaceNode::~ImmediateUserInterfaceNode(){}

void ImmediateUserInterfaceNode::render(ImmedidateUserInterfaceContextLayer* _Context)
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

void ImmediateUserInterfaceNode::layout(ImmedidateUserInterfaceContextLayer* _Context){}

void ImmediateUserInterfaceNode::measure(ImmedidateUserInterfaceContextLayer* _Context)
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

void ImmediateUserInterfaceNode::events(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
{
    // auxiliary lambdas
    auto render_resize_gizmo = [](
        ImmedidateUserInterfaceContextLayer*    _Context,
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
        ImmedidateUserInterfaceContextLayer*    _Context,
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

// ImmedidateUserInterfaceContextLayer2
ImmedidateUserInterfaceContextLayer::ImmedidateUserInterfaceContextLayer(){}
ImmedidateUserInterfaceContextLayer::~ImmedidateUserInterfaceContextLayer(){}

bool ImmedidateUserInterfaceContextLayer::awake()
{
    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

    m_Hierarchy = ImmedidateUserInterfaceNodeHierarchy(
        [](ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
        {
            return _Node != nullptr ? _Node->State.Parent : nullptr;
        });

    m_DockAreas = ImmedidateUserInterfaceNodeHierarchy(
        [](ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
        {
            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(_Node);

            return window != nullptr ? window->Docker : nullptr;
        });

    return m_Renderer != nullptr;
}

void ImmedidateUserInterfaceContextLayer::frame_start(){}

void ImmedidateUserInterfaceContextLayer::frame_update()
{
}

// void showHierarchy(ImmedidateUserInterfaceContextLayer* _Context, ImmedidateUserInterfaceNode* _Node, const std::string& _Delimiter)
// {
//     std::cout << _Delimiter << _Node->Name << "\n";

//     for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
//     {
//         showHierarchy(_Context, (*it), _Delimiter + "\t");
//     }
// }

void ImmedidateUserInterfaceContextLayer::frame_debug()
{
    class ImmedidateUserInterfaceGeometryComputer
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer* _Context)
        {
            for (auto& singleton : _Context->m_Hierarchy.Singletons)
            {
                ImmedidateUserInterfaceGeometryComputer::node_layout(_Context, singleton);
            }
        }

    private:
        static void node_layout(ImmedidateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
        {
            if(_Context == nullptr || _Node == nullptr)
                return;

            _Node->layout(_Context);
            _Node->measure(_Context);

            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
                node_layout(_Context, (*it));
        }
    };

    class ImmedidateUserInterfaceEventsCatcher
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

        static void catch_input(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
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

        static void catch_event(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            if(_Context == nullptr)
                return;

            // check if anything is already catching event
            bool anythingIsAlreadyProcessing = false;
            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(node->State.Events == ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
                    continue;

                anythingIsAlreadyProcessing = true;
                node->events(_Context, _Event);
            }

            if(anythingIsAlreadyProcessing) return;

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
            for (auto it = _Context->m_Hierarchy.begin(hoveredNode->State.Parent);
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

    class ImmedidateUserInterfaceWindowsController
    {
    public:
        static void execute(ImmedidateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            // layering
            for(auto singleton : _Context->m_Hierarchy.Singletons)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(singleton);

                if(window == nullptr)
                    continue;

                // if the window is not being docked and is not a docker then we check only focus
                // otherwise we check both focus and activity
                if(window->Docker == nullptr &&
                    (_Context->m_DockAreas.end(window) - _Context->m_DockAreas.begin(window) <= 0))
                {
                    if(window->DockingFocused)
                    {
                        window->State.InitialDepth = ImmediateUserInterfaceWindow::calculate_layer_depth(
                            _Context,
                            ImmediateUserInterfaceWindow::ImmedidateUserInterfaceWindowLayer_Focused);
                    }
                    else
                    {
                        window->State.InitialDepth = ImmediateUserInterfaceWindow::calculate_layer_depth(
                            _Context,
                            ImmediateUserInterfaceWindow::ImmedidateUserInterfaceWindowLayer_Main);
                    }
                }
                else
                {
                    if(window->DockingFocused)
                    {
                        window->State.InitialDepth = ImmediateUserInterfaceWindow::calculate_layer_depth(
                            _Context,
                            ImmediateUserInterfaceWindow::ImmedidateUserInterfaceWindowLayer_Focused);
                    }
                    else if(window->DockingActive)
                    {
                        window->State.InitialDepth = ImmediateUserInterfaceWindow::calculate_layer_depth(
                            _Context,
                            ImmediateUserInterfaceWindow::ImmedidateUserInterfaceWindowLayer_Active);
                    }
                    else
                    {
                        window->State.InitialDepth = ImmediateUserInterfaceWindow::calculate_layer_depth(
                            _Context,
                            ImmediateUserInterfaceWindow::ImmedidateUserInterfaceWindowLayer_Main);
                    }
                }
            }

            // docking

            // find moving singleton window
            ImmediateUserInterfaceWindow* moved = nullptr;

            for(auto singleton : _Context->m_Hierarchy.Singletons)
            {
                if(dynamic_cast<ImmediateUserInterfaceWindow*>(singleton) &&
                    (singleton->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
                {
                    moved = dynamic_cast<ImmediateUserInterfaceWindow*>(singleton);
                    ImmedidateUserInterfaceWindowsController::detach_from_docker(_Context, moved);
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

            // docking
            if(hovered == nullptr || moved == nullptr)
                return;

            // dock the moved window to the hovered one
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
                // detach all gizmos from docker
                // TODO: this MUST BE done on end of drawing of gizmo
                for(auto singleton : _Context->m_Hierarchy.Singletons)
                {
                    ImmedaiateUserInterfaceDockingWindowGizmo* gizmo =
                        dynamic_cast<ImmedaiateUserInterfaceDockingWindowGizmo*>(singleton);

                    if(gizmo != nullptr)
                        ImmedidateUserInterfaceWindowsController::detach_from_docker(_Context, gizmo);
                }

                // attach node
                ImmedidateUserInterfaceWindowsController::attach_to_docker(_Context, hovered, moved);
            }
            else if(moved != hovered && moved->Docker != hovered && hovered->Docker != moved)
            {

                // render potential docking window gizmo
                int depth = hovered->Cache.Depth + hovered->Cache.TotalThickness + 1;

                _Context->m_Renderer->push_rectangle_rounded_filled(
                    hovered->DockingBox.Min,
                    hovered->DockingBox.Max,
                    _Context->m_Style.FramesRadius,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                    _Context->m_Renderer->calculate_transform_matrix((float)depth));

                // attach docking window gizmo to a docker
                if(_Context->begin_node<ImmedaiateUserInterfaceDockingWindowGizmo>(
                    "DockingWindowGizmo", // this is not a bug as docked window gizmos have the only instance in UI
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    // rebuild docking hierarchy
                    _Context->m_DockAreas.build(_Context->m_NodesRenderingList);

                    ImmedidateUserInterfaceWindowsController::attach_to_docker(
                        _Context,
                        hovered,
                        _Context->get_rendering_stack_top<ImmediateUserInterfaceWindow>());

                    _Context->end_node<ImmedaiateUserInterfaceDockingWindowGizmo>();
                }
            }
        }

    private:

        static void attach_to_docker(
            ImmedidateUserInterfaceContextLayer* _Context,
            ImmediateUserInterfaceWindow*        _Docker,
            ImmediateUserInterfaceWindow*        _Docked)
        {
            // auxiliary lambdas
            auto move_to_cache = [](
                ImmedidateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceWindow*        _Docker)
            {
                if(_Context == nullptr || _Docker == nullptr)
                    return;

                 _Context->m_WindowsDockingCache.push_back(_Docker);
            };

            auto move_child_docked_windows_to_cache = [](
                ImmedidateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceWindow*        _Docker)
            {
                if(_Context == nullptr || _Docker == nullptr)
                    return;

                std::sort(
                    _Context->m_DockAreas.begin(_Docker),
                    _Context->m_DockAreas.end(_Docker),
                    [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
                    {
                        return dynamic_cast<const ImmediateUserInterfaceWindow*>(_A)->DockingIndex <
                                dynamic_cast<const ImmediateUserInterfaceWindow*>(_B)->DockingIndex;
                    });

                for(auto it  = _Context->m_DockAreas.begin(_Docker);
                         it != _Context->m_DockAreas.end(_Docker);
                         it++)
                {
                    _Context->m_WindowsDockingCache.push_back(*it);
                }
            };

            if(_Context == nullptr || _Docker == nullptr || _Docked == nullptr || _Docked->Docker == _Docker || _Docker->Docker == _Docked)
                return;

            // get ready
            _Context->m_WindowsDockingCache.clear();

            // move child docked windows and self to windows docking cache
            move_child_docked_windows_to_cache(_Context, _Docker);
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
                
                window->Docker       = _Docker->Docker ? _Docker->Docker : _Docker;
                window->DockingIndex = dockindex++;
            }

            // clear cache
            _Context->m_WindowsDockingCache.clear();

            // setup self as active
            ImmediateUserInterfaceWindow::setup_as_active_docking_window(_Context, _Docked);
        }
    
        static void detach_from_docker(
            ImmedidateUserInterfaceContextLayer* _Context,
            ImmediateUserInterfaceWindow*        _Detached)
        {
            if(_Detached == nullptr)
                return;

            // reindex docked nodes of the docker of the moved node
            if(_Detached->Docker != nullptr)
            {
                int dockindex = 0;

                for(auto it  = _Context->m_DockAreas.begin(_Detached->Docker);
                         it != _Context->m_DockAreas.end(_Detached->Docker);
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
            
            if(_Context->m_DockAreas.size(_Detached) <= 0)
                return;

            // reattach children to the first child
            _Detached->Docker        = nullptr;
            _Detached->DockingIndex  = -1;
        }
    };

    // build hierarchy
    m_Hierarchy.build(m_NodesRenderingList);

    // build dockareas
    m_DockAreas.build(m_NodesRenderingList);

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

    ImmedidateUserInterfaceEventsCatcher::execute(this, event);
    ImmedidateUserInterfaceWindowsController::execute(this, event);
    ImmedidateUserInterfaceGeometryComputer::execute(this);
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

        static void render_node(ImmedidateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
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
    m_WindowsDockingCache.clear();
}

void ImmedidateUserInterfaceContextLayer::finish()
{
    if(m_Renderer != nullptr)
        m_Renderer->close();
}

bool ImmedidateUserInterfaceContextLayer::begin_window(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, bool* _Opened)
{
    if(begin_node<ImmediateUserInterfaceWindow>(_ID, _Settings, _Opened))
    {
        ImmediateUserInterfaceWindow* window =
            get_rendering_stack_top<ImmediateUserInterfaceWindow>();

        if(m_DockAreas.size(window) > 0 || window->Docker != nullptr)
            window->State.RenderChildren = window->DockingActive;
        else
            window->State.RenderChildren = true;
            
        if(window->Docker != nullptr || window->State.Parent != nullptr)
            return true;

        if(begin_node<ImmediateUserInterfaceWindowFrameBox>(
            std::string(_ID).append("/FrameBox"),
            _Settings))
        {
            get_rendering_stack_top<ImmediateUserInterfaceWindowFrameBox>()->State.RenderedAlways = true;

            if(begin_node<ImmediateUserInterfaceWindowFrame>(
                std::string(_ID).append("/Frame"),
                _Settings))
            {
                get_rendering_stack_top<ImmediateUserInterfaceWindowFrame>()->Title                = window->Name;
                get_rendering_stack_top<ImmediateUserInterfaceWindowFrame>()->Window               = window;
                get_rendering_stack_top<ImmediateUserInterfaceWindowFrame>()->State.RenderedAlways = true;
                end_node<ImmediateUserInterfaceWindowFrame>();
            }

            std::sort(
                m_DockAreas.begin(window),
                m_DockAreas.end(window),
                [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
                {
                    return dynamic_cast<const ImmediateUserInterfaceWindow*>(_A)->DockingIndex <
                            dynamic_cast<const ImmediateUserInterfaceWindow*>(_B)->DockingIndex;
                });

            for (auto it = m_DockAreas.begin(window); it != m_DockAreas.end(window); it++)
            {
                if(dynamic_cast<ImmedaiateUserInterfaceDockingWindowGizmo*>(*it))
                {
                    if(begin_node<ImmedaiateUserInterfaceDockingWindowFrameGizmo>(
                        "DockingWindowFrameGizmo", // this is not a bug as docked window gizmos have the only instance in UI
                        _Settings))
                    {
                        get_rendering_stack_top<ImmedaiateUserInterfaceDockingWindowFrameGizmo>()->State.RenderedAlways = true;

                        end_node<ImmedaiateUserInterfaceDockingWindowFrameGizmo>();
                    }
                }
                else
                {
                    if(begin_node<ImmediateUserInterfaceWindowFrame>(
                        std::string((*it)->Name).append("/Frame"),
                        _Settings))
                    {
                        get_rendering_stack_top<ImmediateUserInterfaceWindowFrame>()->Title                = (*it)->Name;
                        get_rendering_stack_top<ImmediateUserInterfaceWindowFrame>()->Window               = dynamic_cast<ImmediateUserInterfaceWindow*>(*it);
                        get_rendering_stack_top<ImmediateUserInterfaceWindowFrame>()->State.RenderedAlways = true;
                        end_node<ImmediateUserInterfaceWindowFrame>();
                    }
                }
            }

            end_node<ImmediateUserInterfaceWindowFrameBox>();
        }

        return true;
    }

    return false;
}

void ImmedidateUserInterfaceContextLayer::end_window()
{
    end_node<ImmediateUserInterfaceWindow>();
}

bool ImmedidateUserInterfaceContextLayer::begin_vertial_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceNodeVerticalStack>(_Name, _Settings);
}

void ImmedidateUserInterfaceContextLayer::end_vertical_stack()
{
    end_node<ImmediateUserInterfaceNodeVerticalStack>();
}

bool ImmedidateUserInterfaceContextLayer::begin_horizontal_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceNodeHorizontalStack>(_Name, _Settings);
}

void ImmedidateUserInterfaceContextLayer::end_horizontal_stack()
{
    end_node<ImmediateUserInterfaceNodeHorizontalStack>();
}