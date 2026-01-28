#include <ImmediateUserInterfaceLayer2.hpp>

// STL
#include <algorithm>
#include <typeinfo>
#include <typeindex>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        enum ImmedidateUserInterfaceDockingAnchor_ : int
        {
            ImmedidateUserInterfaceDockingAnchor_Begin = 0,
            ImmedidateUserInterfaceDockingAnchor_Top   = ImmedidateUserInterfaceDockingAnchor_Begin,
            ImmedidateUserInterfaceDockingAnchor_Left,
            ImmedidateUserInterfaceDockingAnchor_Right,
            ImmedidateUserInterfaceDockingAnchor_Bottom,
            ImmedidateUserInterfaceDockingAnchor_Center,
            ImmedidateUserInterfaceDockingAnchor_End
        };

        enum ImmedidateUserInterfaceRenderingOrder_ : int
        {
            ImmedidateUserInterfaceRenderingOrder_Begin   = 0,
            ImmedidateUserInterfaceRenderingOrder_Main    = ImmedidateUserInterfaceRenderingOrder_Begin,
            ImmedidateUserInterfaceRenderingOrder_Focus,
            ImmedidateUserInterfaceRenderingOrder_Modal,
            ImmedidateUserInterfaceRenderingOrder_End,
        };

        enum ImmedidateUserInterfaceRenderingLayer_ : int
        {
            ImmedidateUserInterfaceRenderingLayer_Begin   = 0,
            ImmedidateUserInterfaceRenderingLayer_Main    = ImmedidateUserInterfaceRenderingLayer_Begin,
            ImmedidateUserInterfaceRenderingLayer_Gizmos,
            ImmedidateUserInterfaceRenderingLayer_End,
        };

        namespace ImmediateUserInterfaceContextLayerHelpers
        {
            class ImmedidateUserInterfaceMovedNodeSearcher
            {
            public:
                template<typename Filter>
                ImmediateUserInterfaceNode* search(ImmediateUserInterfaceContextLayer* _Context, const Filter& _Filter)
                {
                    if(_Context == nullptr)
                        return nullptr;

                    for(auto singleton : _Context->m_Hierarchy.Singletons)
                    {
                        if(!_Filter(singleton))
                            continue;

                        ImmediateUserInterfaceNode* moved = search_recursive(_Context, singleton, _Filter);

                        if(moved != nullptr)
                            return moved;
                    }

                    return nullptr;
                };

            private:

                template<typename Filter>
                ImmediateUserInterfaceNode* search_recursive(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Moved, const Filter& _Filter)
                {
                    if(_Context == nullptr || _Moved == nullptr)
                        return nullptr;

                    // check self
                    if((_Moved->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved) && _Filter(_Moved))
                        return _Moved;

                    // check children
                    for(auto it = _Context->m_Hierarchy.begin(_Moved); it != _Context->m_Hierarchy.end(_Moved); it++)
                    {
                        ImmediateUserInterfaceNode* moved = search_recursive(_Context, *it, _Filter);

                        if(moved != nullptr)
                            return moved;
                    }

                    return nullptr;
                }
            };

            class ImmedidateUserInterfaceHoveredNodeSearcher
            {
            public:

                template<typename Filter>
                ImmediateUserInterfaceNode* search(
                    ImmediateUserInterfaceContextLayer* _Context,
                    const ImmedidateUserInterfaceEvent& _Event,
                    const Filter&                       _Filter)
                {
                    // find top most hovered singleton window or a snapped window not equal to the moved one
                    ImmediateUserInterfaceNode* hovered  = nullptr;

                    for(auto singleton : _Context->m_Hierarchy.Singletons)
                        search_recursive(_Context, singleton, &hovered, _Event, _Filter);

                    return hovered;
                };

            private:

                template<typename Filter>
                void search_recursive(
                    ImmediateUserInterfaceContextLayer* _Context,
                    ImmediateUserInterfaceNode*         _Next,
                    ImmediateUserInterfaceNode**        _Hovered,
                    const ImmedidateUserInterfaceEvent& _Event,
                    const Filter&                       _Filter)
                {
                    if(_Context == nullptr || _Next == nullptr)
                        return;

                    // check self
                    if(_Filter(_Next))
                    {
                        gs_2dboxf dockingGizmo = gs_2dboxf(
                            _Next->State.BoundingBox.center() - gs_min(_Next->State.BoundingBox.size().x, _Next->State.BoundingBox.size().y) * 0.25f,
                            _Next->State.BoundingBox.center() + gs_min(_Next->State.BoundingBox.size().x, _Next->State.BoundingBox.size().y) * 0.25f);

                        if(dockingGizmo.contains(_Event.CursorPosition))
                        {
                            if(*_Hovered == nullptr || _Next->Cache.Depth > (*_Hovered)->Cache.Depth)
                                *_Hovered = _Next;
                        }
                    }

                    // check children
                    for(auto it = _Context->m_Hierarchy.begin(_Next); it != _Context->m_Hierarchy.end(_Next); it++)
                        search_recursive(_Context, *it, _Hovered, _Event, _Filter);
                }
            };

            ImmedidateUserInterfaceEvent construct_event(ImmediateUserInterfaceContextLayer* _Context)
            {
                if(_Context == nullptr)
                    return ImmedidateUserInterfaceEvent();

                // construct events
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

                return event;
            }

            template<typename Type, typename Filter>
            void layout_nodes_vertically(const Type& _Begin, const Type& _End, const gs_vec2f& _Position, const gs_2dboxf& _BoundingBox, const Filter& _Filter)
            {
                gs_vec2f position  = _Position;
                gs_vec2f totalsize = gs_vec2f(0.f, 0.f);

                // compute total size
                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) != nullptr && _Filter(*it))
                        totalsize += (*it)->State.BoundingBox.size();
                }

                // compute children size scale
                gs_vec2f scale = _BoundingBox.size() / totalsize;

                // compute layout
                for(auto it = _Begin; it != _End; ++it)
                {
                    auto node = *it;

                    if(node == nullptr || !_Filter(node))
                        continue;

                    gs_vec2f size = gs_vec2f(_BoundingBox.width(), (node->State.BoundingBox.size() * scale).y);
                    size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);
                    node->State.BoundingBox = gs_2dboxf(position, position + size);
                    position += gs_vec2f(0.f, size.y);
                }
            }

            template<typename Type, typename Filter>
            void layout_nodes_horizontally(const Type& _Begin, const Type& _End, const gs_vec2f& _Position, const gs_2dboxf& _BoundingBox, const Filter& _Filter)
            {
                gs_vec2f position   = _Position;
                gs_vec2f totalsize  = gs_vec2f(0.f, 0.f);

                // compute total size
                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) != nullptr && _Filter(*it))
                        totalsize += (*it)->State.BoundingBox.size();
                }

                // compute children scale
                gs_vec2f scale = _BoundingBox.size() / totalsize;

                // compute layout
                for(auto it = _Begin; it != _End; ++it)
                {
                    auto node = *it;

                    if(node == nullptr || !_Filter(node))
                        continue;

                    gs_vec2f size = gs_vec2f((node->State.BoundingBox.size() * scale).x, _BoundingBox.height());
                    size = gs_clamp(size, node->State.MinimumSize, node->State.MaximumSize);
                    node->State.BoundingBox = gs_2dboxf(position, position + size);
                    position += gs_vec2f(size.x, 0.f);
                }
            }
        
            template<typename Type, typename Filter>
            void layout_nodes_to_resize_to_box(const Type& _Begin, const Type& _End, const gs_vec2f& _Position, const gs_2dboxf& _BoundingBox, const Filter& _Filter)
            {
                // compute layout
                for(auto it = _Begin; it != _End; ++it)
                {
                    auto node = *it;

                    if(node == nullptr || !_Filter(node))
                        continue;

                    node->State.BoundingBox = gs_2dboxf(_Position, _Position + _BoundingBox.size());
                }
            }

            template<typename Type, typename Filter>
            void collect_children(const Type& _Begin, const Type& _End, const Filter& _Filter, std::vector<ImmediateUserInterfaceNode*>& _Buffer)
            {
                _Buffer.clear();

                for(auto it = _Begin; it != _End; it++)
                {
                    if(_Filter(*it))
                        _Buffer.push_back(*it);
                }
            }

            void layout_boxes_vertically(gs_2dboxf* _Boxes, int _Count, const gs_2dboxf& _Parent, const gs_vec2f& _Position)
            {
                // compute total size
                gs_vec2f totalSize = gs_vec2f(0.f, 0.f);
                gs_vec2f position  = _Position;
                
                for (int i = 0; i < _Count; i++)
                    totalSize += _Boxes[i].size();
                gs_vec2f scale = _Parent.size() / totalSize;

                // layout
                for (int i = 0; i < _Count; i++)
                {
                    _Boxes[i] = gs_2dboxf(position, position + gs_vec2f(_Parent.size().x, (_Boxes[i].size() * scale).y));
                    position += gs_vec2f(0.f, _Boxes[i].size().y);
                }
            }

            void layout_boxes_horizontally(gs_2dboxf* _Boxes, int _Count, const gs_2dboxf& _Parent, const gs_vec2f& _Position)
            {
                // compute total size
                gs_vec2f totalSize = gs_vec2f(0.f, 0.f);
                gs_vec2f position  = _Position;
                
                for (int i = 0; i < _Count; i++)
                    totalSize += _Boxes[i].size();
                gs_vec2f scale = _Parent.size() / totalSize;

                // layout
                for (int i = 0; i < _Count; i++)
                {
                    _Boxes[i] = gs_2dboxf(
                        position,
                        position + gs_vec2f((_Boxes[i].size() * scale).x, _Parent.size().y));
                    
                    position += gs_vec2f(_Boxes[i].size().x, 0.f);
                }
            }

            void clamp_bounding_box(ImmediateUserInterfaceNode* _Node, gs_2dboxf _BoundingBox)
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

            gs_2d_ellipsef build_resize_top_left_ellipse(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Min, WindowResizeAngleGizmoRadius);
            }

            gs_2d_ellipsef build_resize_top_right_ellipse(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
            }

            gs_2d_ellipsef build_resize_bottom_left_ellipse(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Max - gs_vec2f(_Node->State.BoundingBox.width(), 0.f), WindowResizeAngleGizmoRadius);
            };

            gs_2d_ellipsef build_resize_bottom_right_ellipse(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->State.BoundingBox.Max, WindowResizeAngleGizmoRadius);
            };

            gs_2dboxf build_resize_top_box(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 8.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min - gs_vec2f(0.f, WindowResizeSideGizmoWidth),
                    _Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width(), WindowResizeSideGizmoWidth));
            };

            gs_2dboxf build_resize_left_box(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 8.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min - gs_vec2f(WindowResizeSideGizmoWidth, 0.f),
                    _Node->State.BoundingBox.Min + gs_vec2f(WindowResizeSideGizmoWidth, _Node->State.BoundingBox.height()));
            };

            gs_2dboxf build_resize_right_box(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 8.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min + gs_vec2f(_Node->State.BoundingBox.width() - WindowResizeSideGizmoWidth, 0.f),
                    _Node->State.BoundingBox.Max + gs_vec2f(WindowResizeSideGizmoWidth, 0.f));
            };

            gs_2dboxf build_resize_bottom_box(ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeSideGizmoWidth = 8.f;

                return gs_2dboxf(
                    _Node->State.BoundingBox.Min + gs_vec2f(0.f, _Node->State.BoundingBox.height() - WindowResizeSideGizmoWidth),
                    _Node->State.BoundingBox.Max + gs_vec2f(0.f, WindowResizeSideGizmoWidth));
            };
        
            int calculate_layer_depth(ImmediateUserInterfaceContextLayer* _Context, int _Layer)
            {
                return (int)(_Layer * _Context->m_Renderer->get_far_plane() / (ImmedidateUserInterfaceRenderingLayer_::ImmedidateUserInterfaceRenderingLayer_End - ImmedidateUserInterfaceRenderingLayer_::ImmedidateUserInterfaceRenderingLayer_Begin));
            };
        }

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

                ImmediateUserInterfaceContextLayerHelpers::layout_nodes_vertically(
                    _Context->m_Hierarchy.begin(this),
                    _Context->m_Hierarchy.end(this),
                    State.BoundingBox.Min, State.BoundingBox,
                    [](const ImmediateUserInterfaceNode*){return true;});
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

                ImmediateUserInterfaceContextLayerHelpers::layout_nodes_horizontally(
                    _Context->m_Hierarchy.begin(this),
                    _Context->m_Hierarchy.end(this),
                    State.BoundingBox.Min,
                    State.BoundingBox,
                    [](const ImmediateUserInterfaceNode*){return true;});
            }
        };
    
        // windows
        struct ImmediateUserInterfaceWindowVerticalDocker : public ImmediateUserInterfaceNodeVerticalStack
        {
            ImmediateUserInterfaceWindowVerticalDocker(const std::string& _Name) : ImmediateUserInterfaceNodeVerticalStack(_Name){}
            virtual ~ImmediateUserInterfaceWindowVerticalDocker(){}

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                _Context->m_Renderer->push_rectangle(
                    State.BoundingBox.Min,
                    State.BoundingBox.Max,
                    8.f,
                    gs_vec4f(12.f, 128.f, 128.f, 255.f), // TODO: this MUST BE a setting
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }
        };

        struct ImmediateUserInterfaceWindowHorizontalDocker : public ImmediateUserInterfaceNodeHorizontalStack
        {
            ImmediateUserInterfaceWindowHorizontalDocker(const std::string& _Name) : ImmediateUserInterfaceNodeHorizontalStack(_Name){}
            virtual ~ImmediateUserInterfaceWindowHorizontalDocker(){}

            void measure(ImmediateUserInterfaceContextLayer* _Context) override
            {
                ImmediateUserInterfaceNodeHorizontalStack::measure(_Context);

                State.MinimumSize = gs_vec2f(0.f, 0.f);

                if(_Context->m_Hierarchy.size(this) <= 0)
                    State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
                else if(gs_min(State.BoundingBox.size().x, State.BoundingBox.size().y) <= 1.f)
                    State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(512.f, 512.f));
            }

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                _Context->m_Renderer->push_rectangle(
                    State.BoundingBox.Min,
                    State.BoundingBox.Max,
                    8.f,
                    gs_vec4f(12.f, 128.f, 128.f, 255.f), // TODO: this MUST BE a setting
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }
        };

        struct ImmediateUserInterfaceWindow : public ImmediateUserInterfaceNodeVerticalStack
        {
        public:

            class ImmediateUserInterfaceWindowActivator
            {
            public:
            
                template<typename Filter>
                void setup_as_active_docking_window(
                    ImmediateUserInterfaceContextLayer* _Context,
                    ImmediateUserInterfaceWindow*       _Docked,
                    const Filter&                       _Filter)
                {
                    if(_Context == nullptr || _Docked == nullptr)
                        return;

                    if(_Docked->CentralDocker != nullptr)
                    {
                        for(auto it  = _Context->m_Hierarchy.begin(_Docked->CentralDocker);
                                 it != _Context->m_Hierarchy.end(_Docked->CentralDocker);
                                 it++)
                        {
                            if(!_Filter(*it)) continue;

                            (*it)->State.Hidden = _Docked != (*it);
                        }
                    }
                    else
                    {
                        for(auto it  = _Context->m_Hierarchy.begin(_Docked);
                                 it != _Context->m_Hierarchy.end(_Docked);
                                 it++)
                        {
                            if(!_Filter(*it)) continue;

                            ImmediateUserInterfaceWindow* window =
                                dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                            (*it)->State.Hidden = window != nullptr && window->CentralDocker == _Docked;
                        }
                    }
                }
            };

            ImmediateUserInterfaceWindow(const std::string& _Name) : ImmediateUserInterfaceNodeVerticalStack(_Name){}
            virtual ~ImmediateUserInterfaceWindow(){}

            void render_frame(ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _BoundingBox, ImmediateUserInterfaceWindow* _Node)
            {
                // construct events
                ImmedidateUserInterfaceEvent event = ImmediateUserInterfaceContextLayerHelpers::construct_event(_Context);

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
                    _Node->Name,
                    _Context->m_Style.FontSize,
                    _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text],
                    _Context->m_Renderer->calculate_transform_matrix(
                        (float)place_in_follow(),
                        gs_vec2f(_BoundingBox.Min.x + 16.f,
                            _BoundingBox.center().y - _Context->m_Renderer->calculate_bounding_box(_Node->Name, _Context->m_Style.FontSize, _Context->m_Style.Font).height() * 0.5f)));

                // TODO: window frame MUST BE a separate UI object with it's own rendering and event loop...
                // close button
                if(_Node->Opened != nullptr)
                {
                    gs_vec4f closeButtonСolor = gs_vec4f(255.f, 0.f, 0.f, 255.f); // TODO: this MUST BE a setting

                    gs_2dboxf CloseButtonBox = gs_2dboxf(
                        gs_vec2f(_BoundingBox.Max.x - _Node->FrameBox.height() / 2.f, _BoundingBox.center().y) - _Node->FrameBox.height() / 4.f,
                        gs_vec2f(_BoundingBox.Max.x - _Node->FrameBox.height() / 2.f, _BoundingBox.center().y) + _Node->FrameBox.height() / 4.f);

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        CloseButtonBox.Min,
                        CloseButtonBox.Max,
                        _Context->m_Style.FramesRadius,
                        _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                    if(CloseButtonBox.contains(event.CursorPosition))
                    {
                        closeButtonСolor = event.MouseDown.has_value() ?
                            gs_vec4f(255.f, 230.f, 0.f, 255.f) : // TODO: this MUST BE a setting
                            gs_vec4f(255.f, 128.f, 0.f, 255.f);  // TODO: this MUST BE a setting

                        *_Node->Opened = !event.MouseClicked.has_value();

                        // undock all windows
                        if(!(*_Node->Opened))
                        {
                            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
                            {
                                ImmediateUserInterfaceWindow* window =
                                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                                if(window == nullptr)
                                    continue;

                                window->TopDocker     = nullptr;
                                window->LeftDocker    = nullptr;
                                window->RightDocker   = nullptr;
                                window->BottomDocker  = nullptr;
                                window->CentralDocker = nullptr;
                                window->State.Hidden  = false;
                            }

                            for(auto it = _Context->m_Hierarchy.begin(_Node->Top); it != _Context->m_Hierarchy.end(_Node->Top); it++)
                            {
                                ImmediateUserInterfaceWindow* window =
                                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                                if(window == nullptr)
                                    continue;

                                window->TopDocker     = nullptr;
                                window->LeftDocker    = nullptr;
                                window->RightDocker   = nullptr;
                                window->BottomDocker  = nullptr;
                                window->CentralDocker = nullptr;
                                window->State.Hidden  = false;
                            }

                            for(auto it = _Context->m_Hierarchy.begin(_Node->Left); it != _Context->m_Hierarchy.end(_Node->Left); it++)
                            {
                                ImmediateUserInterfaceWindow* window =
                                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                                if(window == nullptr)
                                    continue;

                                window->TopDocker     = nullptr;
                                window->LeftDocker    = nullptr;
                                window->RightDocker   = nullptr;
                                window->BottomDocker  = nullptr;
                                window->CentralDocker = nullptr;
                                window->State.Hidden  = false;
                            }

                            for(auto it = _Context->m_Hierarchy.begin(_Node->Right); it != _Context->m_Hierarchy.end(_Node->Right); it++)
                            {
                                ImmediateUserInterfaceWindow* window =
                                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                                if(window == nullptr)
                                    continue;

                                window->TopDocker     = nullptr;
                                window->LeftDocker    = nullptr;
                                window->RightDocker   = nullptr;
                                window->BottomDocker  = nullptr;
                                window->CentralDocker = nullptr;
                                window->State.Hidden  = false;
                            }


                            for(auto it = _Context->m_Hierarchy.begin(_Node->Bottom); it != _Context->m_Hierarchy.end(_Node->Bottom); it++)
                            {
                                ImmediateUserInterfaceWindow* window =
                                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                                if(window == nullptr)
                                    continue;

                                window->TopDocker     = nullptr;
                                window->LeftDocker    = nullptr;
                                window->RightDocker   = nullptr;
                                window->BottomDocker  = nullptr;
                                window->CentralDocker = nullptr;
                                window->State.Hidden  = false;
                            }

                            _Node->State.Hidden = false; // don't hide self !!!
                        }
                    }

                    _Context->m_Renderer->push_arc_filled(
                        gs_vec2f(_BoundingBox.Max.x - _Node->FrameBox.height() / 2.f, _BoundingBox.center().y),
                        _Node->FrameBox.height() / 4.f,
                        _Node->FrameBox.height() / 4.f,
                        0.f,
                        360.f,
                        closeButtonСolor,
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }

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
                if(CentralDocker != nullptr)
                    return;

                gs_vec2f position = FrameBox.Min;
                int dockedWindowsCount = _Context->m_Hierarchy.count(
                    this,
                    [this](const ImmediateUserInterfaceNode* _Node)
                    {
                        const ImmediateUserInterfaceWindow* window =
                            dynamic_cast<const ImmediateUserInterfaceWindow*>(_Node);

                        return window != nullptr && window->CentralDocker != nullptr;
                    }
                );
                gs_vec2f size = gs_vec2f((FrameBox.size() / (float)(dockedWindowsCount + 1)).x, FrameBox.size().y);

                // render self frame
                gs_2dboxf boundingBox = gs_2dboxf(position, position + size);
                render_frame(_Context, boundingBox, this);
                position += gs_vec2f(size.x, 0.f);

                // render docked windows frames
                for(auto it  = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
                {
                    ImmediateUserInterfaceWindow* window =
                        dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                    if(window == nullptr || window->CentralDocker == nullptr)
                        continue;

                    gs_2dboxf boundingBox = gs_2dboxf(position, position + size);
                    render_frame(_Context, boundingBox, dynamic_cast<ImmediateUserInterfaceWindow*>(*it));
                    position += gs_vec2f(size.x, 0.f);
                }
            }

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr || !is_partially_visible())
                    return;

                if(CentralDocker != nullptr)
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
                FrameBox = gs_2dboxf(
                    State.BoundingBox.Min + _Context->m_Style.FramesWidth * 0.5f,
                    gs_vec2f(
                        State.BoundingBox.Max.x,
                        State.BoundingBox.Min.y + gs_max(_Context->m_Style.FontSize, 64.f)) - _Context->m_Style.FramesWidth * 0.5f);

                ContentBox = gs_2dboxf(
                    (CentralDocker == nullptr ? gs_vec2f(FrameBox.Min.x, FrameBox.Max.y) : State.BoundingBox.Min),
                    State.BoundingBox.Max);

                // layout
                ImmediateUserInterfaceContextLayerHelpers::layout_nodes_to_resize_to_box(
                    _Context->m_Hierarchy.begin(this),
                    _Context->m_Hierarchy.end(this),
                    ContentBox.Min,
                    ContentBox,
                    [this](const ImmediateUserInterfaceNode* _Node)
                    {
                        if(_Node == Docker)
                            return true;

                        if(dynamic_cast<const ImmediateUserInterfaceWindow*>(_Node) &&
                            dynamic_cast<const ImmediateUserInterfaceWindow*>(_Node)->CentralDocker == this)
                            return true;

                        return false;
                    }
                );
            }

            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override
            {
                if(_Child == nullptr)
                    return;

                if(dynamic_cast<ImmediateUserInterfaceWindowVerticalDocker*>(_Child) ||
                    dynamic_cast<ImmediateUserInterfaceWindowHorizontalDocker*>(_Child))
                    _Child->State.Parent = this;
                else
                    _Child->State.Parent = Center;
            }

            ImmediateUserInterfaceNode* TopDocker    {nullptr};
            ImmediateUserInterfaceNode* LeftDocker   {nullptr};
            ImmediateUserInterfaceNode* RightDocker  {nullptr};
            ImmediateUserInterfaceNode* BottomDocker {nullptr};
            ImmediateUserInterfaceNode* CentralDocker{nullptr};

            gs_2dboxf                     FrameBox;
            gs_2dboxf                     ContentBox;

            bool*                         Opened        {nullptr};

            int                           DockingIndex  {-1};

            ImmediateUserInterfaceWindowHorizontalDocker* Top   {nullptr};
            ImmediateUserInterfaceWindowHorizontalDocker* Left  {nullptr};
            ImmediateUserInterfaceWindowHorizontalDocker* Right {nullptr};
            ImmediateUserInterfaceWindowHorizontalDocker* Bottom{nullptr};
            ImmediateUserInterfaceWindowVerticalDocker*   Center{nullptr};
            ImmediateUserInterfaceWindowVerticalDocker*   Docker{nullptr};
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
        ImmediateUserInterfaceContextLayer*     _Context,
        ImmediateUserInterfaceNode*             _Node,
        const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
    {
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
        {
            auto resizeTopLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Node);

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
            auto resizeTopRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_right_ellipse(_Node);

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
            auto resizeBottomLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_left_ellipse(_Node);

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
            auto resizeBottomRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_right_ellipse(_Node);

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
            auto resizeTop = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_box(_Node);

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
            auto resizeLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_left_box(_Node);

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
            auto resizeRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_right_box(_Node);

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
            auto resizeBottom = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_box(_Node);

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
            return ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Node).contains(_Event.CursorPosition);

        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
            return ImmediateUserInterfaceContextLayerHelpers::build_resize_top_right_ellipse(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
            return ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_left_ellipse(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
            return ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_right_ellipse(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
            return ImmediateUserInterfaceContextLayerHelpers::build_resize_top_box(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
            return ImmediateUserInterfaceContextLayerHelpers::build_resize_left_box(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
            return ImmediateUserInterfaceContextLayerHelpers::build_resize_right_box(_Node).contains(_Event.CursorPosition);
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
            return ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_box(_Node).contains(_Event.CursorPosition);
        
        return false;
    };

    auto find_resizable_node = [&check_cursor_intersection_with_resize_gizmo](
        ImmediateUserInterfaceContextLayer*     _Context,
        ImmediateUserInterfaceNode*             _Node,
        const ImmediateUserInterfaceNodeEvents& _ResizeEventType,
        const ImmedidateUserInterfaceEvent&     _Event)
    {
        // find resizable node
        ImmediateUserInterfaceNode* resizable = _Node;

        // pass event to a parent
        while (_Context->m_Hierarchy.get_parent(resizable) &&
                check_cursor_intersection_with_resize_gizmo(_Context->m_Hierarchy.get_parent(resizable), _Event, _ResizeEventType))
            resizable = _Context->m_Hierarchy.get_parent(resizable);

        // if this is a window then we pass event to a docker
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(resizable);

        if(window != nullptr && window->CentralDocker != nullptr)
            resizable = window->CentralDocker;

        return resizable;
    };

    auto resize_node = [](
        ImmediateUserInterfaceNode*             _Node,
        const ImmediateUserInterfaceNodeEvents& _ResizeEventType,
        const ImmedidateUserInterfaceEvent&     _Event)
    {
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
        {
            ImmediateUserInterfaceContextLayerHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + _Event.CursorDragDelta,
                _Node->Cache.BoundingBox.Max));
            return;
        }

        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
        {
            ImmediateUserInterfaceContextLayerHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Node->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
        {
            ImmediateUserInterfaceContextLayerHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
        {
            ImmediateUserInterfaceContextLayerHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min,
                _Node->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta()));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
        {
            ImmediateUserInterfaceContextLayerHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y),
                _Node->Cache.BoundingBox.Max));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
        {
            ImmediateUserInterfaceContextLayerHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f),
                _Node->Cache.BoundingBox.Max));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
        {
            ImmediateUserInterfaceContextLayerHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min,
                _Node->Cache.BoundingBox.Max + gs_vec2f(application()->get_window_cursor_dragdelta().x, 0.f)));
            return;
        }
    
        if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
        {
            ImmediateUserInterfaceContextLayerHelpers::clamp_bounding_box(_Node, gs_2dboxf(
                _Node->Cache.BoundingBox.Min,
                _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, application()->get_window_cursor_dragdelta().y)));
            return;
        }
    };

    // focus
    if(State.MousePressed.has_value())
    {
        for (auto node : _Context->m_Hierarchy.Singletons)
        {
            node->State.RenderingOrder =
                ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Main;
        }

        if(dynamic_cast<ImmediateUserInterfaceWindow*>(this) &&
            (dynamic_cast<ImmediateUserInterfaceWindow*>(this)->LeftDocker   ||
            dynamic_cast<ImmediateUserInterfaceWindow*>(this)->RightDocker   ||
            dynamic_cast<ImmediateUserInterfaceWindow*>(this)->BottomDocker  ||
            dynamic_cast<ImmediateUserInterfaceWindow*>(this)->CentralDocker))
        {
            State.RenderingOrder = ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Focus;
        }
        else
        {
            ImmediateUserInterfaceNode* focused = this;
            ImmediateUserInterfaceNode* parent  = _Context->m_Hierarchy.get_parent(this);

            while (parent)
            {
                focused = parent;
                parent  = _Context->m_Hierarchy.get_parent(parent);
            }
            
            if(focused != nullptr)
                focused->State.RenderingOrder = ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Focus;
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
        ImmediateUserInterfaceNode* movable = this;

        if(  dynamic_cast<ImmediateUserInterfaceWindow*>(movable) != nullptr &&
            (dynamic_cast<ImmediateUserInterfaceWindow*>(movable)->TopDocker     != nullptr ||
             dynamic_cast<ImmediateUserInterfaceWindow*>(movable)->LeftDocker    != nullptr ||
             dynamic_cast<ImmediateUserInterfaceWindow*>(movable)->RightDocker   != nullptr ||
             dynamic_cast<ImmediateUserInterfaceWindow*>(movable)->BottomDocker  != nullptr ||
             dynamic_cast<ImmediateUserInterfaceWindow*>(movable)->CentralDocker != nullptr))
        {
        }
        else
        {
            while (_Context->m_Hierarchy.get_parent(movable))
                movable = _Context->m_Hierarchy.get_parent(movable);
        }

        if(_Event.MousePressed.has_value())
        {
            movable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;
            return;
        }

        if(_Event.MouseDown.has_value() &&
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
        {            
            movable->State.BoundingBox = gs_2dboxf(
                movable->Cache.BoundingBox.Min + application()->get_window_cursor_dragdelta(),
                movable->Cache.BoundingBox.Max + application()->get_window_cursor_dragdelta());
            return;
        }
    }
}

void ImmediateUserInterfaceNode::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child != nullptr)
        _Child->State.Parent = this;
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

    // hierarchy
    m_Hierarchy = ImmedidateUserInterfaceNodeHierarchy(
        [](ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
        {
            if(_Node == nullptr)
                return nullptr;

            if(_Node->State.Parent != nullptr)
                return _Node->State.Parent;

            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(_Node);

            if(window != nullptr && window->CentralDocker != nullptr)
                return window->CentralDocker;

            if(window != nullptr && window->TopDocker != nullptr)
                return window->TopDocker;

            if(window != nullptr && window->LeftDocker != nullptr)
                return window->LeftDocker;

            if(window != nullptr && window->RightDocker != nullptr)
                return window->RightDocker;

            if(window != nullptr && window->BottomDocker != nullptr)
                return window->BottomDocker;

            return nullptr;
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
            for (auto it  = _Context->m_Hierarchy.begin(_Context->m_Hierarchy.get_parent(hoveredNode));
                      it != _Context->m_Hierarchy.end(_Context->m_Hierarchy.get_parent(hoveredNode));
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

            // rebuild hierarchy
            _Context->m_WindowsDockingList.clear();
            _Context->m_WindowsDockingCache.clear();
            
            for(auto node : _Context->m_NodesRenderingList)
            {
                if(dynamic_cast<ImmediateUserInterfaceWindow*>(node))
                    _Context->m_WindowsDockingList.push_back(node);
                else
                    _Context->m_WindowsDockingCache.push_back(node);
            }

            std::sort(
                _Context->m_WindowsDockingList.begin(),
                _Context->m_WindowsDockingList.end(),
                [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B) 
                {
                    return dynamic_cast<const ImmediateUserInterfaceWindow*>(_A)->DockingIndex <
                        dynamic_cast<const ImmediateUserInterfaceWindow*>(_B)->DockingIndex;
                });

            _Context->m_NodesRenderingList.clear();

            for(auto node : _Context->m_WindowsDockingList)
                _Context->m_NodesRenderingList.push_back(node);

            for(auto node : _Context->m_WindowsDockingCache)
                _Context->m_NodesRenderingList.push_back(node);

             _Context->m_Hierarchy.build(_Context->m_NodesRenderingList);
        }

    private:

        static void place_on_dockers(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event)
        {
            ImmediateUserInterfaceWindow* moved   = dynamic_cast<ImmediateUserInterfaceWindow*>(
                ImmediateUserInterfaceContextLayerHelpers::ImmedidateUserInterfaceMovedNodeSearcher().search(
                    _Context,
                    [](const ImmediateUserInterfaceNode*){return true;}));
                
            ImmediateUserInterfaceWindow* hovered =
                dynamic_cast<ImmediateUserInterfaceWindow*>(ImmediateUserInterfaceContextLayerHelpers::ImmedidateUserInterfaceHoveredNodeSearcher().search(
                    _Context,
                    _Event,
                    [moved](const ImmediateUserInterfaceNode* _Node)->bool
                    {
                        return dynamic_cast<const ImmediateUserInterfaceWindow*>(_Node) && _Node != moved;
                    }
                )
            );

            ImmedidateUserInterfaceWindowController::detach_from_docker(_Context, moved);

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

            // compute gizmos
            gs_2dboxf dockingGizmo = gs_2dboxf(
                hovered->State.BoundingBox.center() - gs_min(hovered->State.BoundingBox.size().x, hovered->State.BoundingBox.size().y) * 0.25f,
                hovered->State.BoundingBox.center() + gs_min(hovered->State.BoundingBox.size().x, hovered->State.BoundingBox.size().y) * 0.25f);

            gs_vec2f gizmoSize = dockingGizmo.size() * 0.333f;

            gs_2dboxf topDockingGizmo = gs_2dboxf(
                dockingGizmo.Min + gs_vec2f(gizmoSize.x, 0.f),
                dockingGizmo.Min + gs_vec2f(gizmoSize.x * 2.f, gizmoSize.y));

            gs_2dboxf leftDockingGizmo = gs_2dboxf(
                dockingGizmo.Min + gs_vec2f(0.f, gizmoSize.y),
                dockingGizmo.Min + gs_vec2f(gizmoSize.x, gizmoSize.y * 2.f));

            gs_2dboxf rightDockingGizmo = gs_2dboxf(
                dockingGizmo.Min + gs_vec2f(gizmoSize.x * 2.f, gizmoSize.y),
                dockingGizmo.Min + gs_vec2f(gizmoSize.x * 3.f, gizmoSize.y * 2.f));

            gs_2dboxf bottomDockingGizmo = gs_2dboxf(
                dockingGizmo.Min + gs_vec2f(gizmoSize.x, gizmoSize.y * 2.f),
                dockingGizmo.Min + gs_vec2f(gizmoSize.x * 2.f, gizmoSize.y * 3.f));

            gs_2dboxf centralDockingGizmo = gs_2dboxf(
                dockingGizmo.center() - gizmoSize * 0.5f,
                dockingGizmo.center() + gizmoSize * 0.5f);

            if(allMouseButtonsAreReleased)
            {
                if(centralDockingGizmo.contains(_Event.CursorPosition))
                {
                    ImmedidateUserInterfaceWindowController::attach_to_docker(
                        _Context,
                        hovered,
                        moved,
                        ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center);
                }
                if(topDockingGizmo.contains(_Event.CursorPosition))
                {
                    ImmedidateUserInterfaceWindowController::attach_to_docker(
                        _Context,
                        hovered,
                        moved,
                        ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Top);
                }
                else if(leftDockingGizmo.contains(_Event.CursorPosition))
                {
                    ImmedidateUserInterfaceWindowController::attach_to_docker(
                        _Context,
                        hovered,
                        moved,
                        ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Left);
                }
                else if(rightDockingGizmo.contains(_Event.CursorPosition))
                {
                    ImmedidateUserInterfaceWindowController::attach_to_docker(
                        _Context,
                        hovered,
                        moved,
                        ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Right);
                }
                else if(bottomDockingGizmo.contains(_Event.CursorPosition))
                {
                    ImmedidateUserInterfaceWindowController::attach_to_docker(
                        _Context,
                        hovered,
                        moved,
                        ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Bottom);
                }
            }
            else if(moved != hovered && moved->CentralDocker != hovered && hovered->CentralDocker != moved)
            {
                // render potential docking window gizmo
                int depth = ImmediateUserInterfaceContextLayerHelpers::calculate_layer_depth(
                    _Context,
                    ImmedidateUserInterfaceRenderingLayer_::ImmedidateUserInterfaceRenderingLayer_Gizmos);

                if(dockingGizmo.contains(_Event.CursorPosition))
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        dockingGizmo.Min,
                        dockingGizmo.Max,
                        _Context->m_Style.FramesRadius,
                        gs_vec4f(
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos].x,
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos].y,
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos].z,
                            128.f),
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        topDockingGizmo.Min,
                        topDockingGizmo.Max,
                        _Context->m_Style.FramesRadius,
                        topDockingGizmo.contains(_Event.CursorPosition) ?
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered] :
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        leftDockingGizmo.Min,
                        leftDockingGizmo.Max,
                        _Context->m_Style.FramesRadius,
                        leftDockingGizmo.contains(_Event.CursorPosition) ?
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered] :
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        rightDockingGizmo.Min,
                        rightDockingGizmo.Max,
                        _Context->m_Style.FramesRadius,
                        rightDockingGizmo.contains(_Event.CursorPosition) ?
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered] :
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        bottomDockingGizmo.Min,
                        bottomDockingGizmo.Max,
                        _Context->m_Style.FramesRadius,
                        bottomDockingGizmo.contains(_Event.CursorPosition) ?
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered] :
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        centralDockingGizmo.Min,
                        centralDockingGizmo.Max,
                        _Context->m_Style.FramesRadius,
                        centralDockingGizmo.contains(_Event.CursorPosition) ?
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered] :
                            _Context->m_Style.Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos],
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));
                }
            }
        }

        static void attach_to_docker(
            ImmediateUserInterfaceContextLayer*          _Context,
            ImmediateUserInterfaceWindow*                _Docker,
            ImmediateUserInterfaceWindow*                _Docked,
            const ImmedidateUserInterfaceDockingAnchor_& _Orientation)
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
                ImmediateUserInterfaceContextLayer*          _Context,
                ImmediateUserInterfaceNode*                  _Docker,
                const ImmedidateUserInterfaceDockingAnchor_& _Orientation)
            {
                if(_Context == nullptr || _Docker == nullptr)
                    return;

                auto& dockedWindows = retrieve_docked_windows(_Context, _Docker, _Orientation);
                for(auto it  = dockedWindows.begin(); it != dockedWindows.end(); it++)
                    _Context->m_WindowsDockingCache.push_back(*it);
            };

            // get ready
            _Context->m_WindowsDockingCache.clear();

            // attach to a central part as a tab
            if(_Orientation == ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center)
            {
                if(_Context == nullptr || _Docker == nullptr || _Docked == nullptr || _Docked->CentralDocker == _Docker || _Docker->CentralDocker == _Docked)
                    return;

                ImmediateUserInterfaceNode* docker = _Docker->CentralDocker ? _Docker->CentralDocker : _Docker;

                // move child docked windows and self to windows docking cache
                move_child_docked_windows_to_cache(_Context, docker, _Orientation);
                move_to_cache(_Context, _Docked);
                move_child_docked_windows_to_cache(_Context, _Docked, _Orientation);

                // reindex docked nodes and setup their docker
                int dockindex = 0;

                for(auto it = _Context->m_WindowsDockingCache.begin(); it != _Context->m_WindowsDockingCache.end(); it++)
                {
                    ImmediateUserInterfaceWindow* window =
                        dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                    if(window == nullptr)
                        continue;

                    window->CentralDocker = docker;
                    window->DockingIndex  = dockindex++;
                }

                // setup self as active
                ImmediateUserInterfaceWindow::ImmediateUserInterfaceWindowActivator().setup_as_active_docking_window(
                    _Context,
                    _Docked,
                    [](const ImmediateUserInterfaceNode*)->bool{return true;});

                // clear
                _Context->m_WindowsDockingCache.clear();
                _Context->m_WindowsDockingList.clear();
                return;
            }

            // attach to sides as a snapped window
            if( _Context  == nullptr ||
                _Docker == nullptr  ||
                _Docked == nullptr  ||
                (_Docked->TopDocker == _Docker->Top || _Docked->LeftDocker == _Docker->Left || _Docked->RightDocker == _Docker->Right || _Docked->BottomDocker == _Docker->Bottom) ||
                (_Docker->TopDocker == _Docked->Top || _Docker->LeftDocker == _Docked->Left || _Docker->RightDocker == _Docked->Right || _Docker->BottomDocker == _Docked->Bottom))
            {
                return;
            }

            // get ready
            _Context->m_WindowsDockingCache.clear();

            ImmediateUserInterfaceWindow* docker = _Docker;

            // move child docked windows and self to windows docking cache
            move_child_docked_windows_to_cache(_Context, docker, _Orientation);
            move_to_cache(_Context, _Docked);

            // reindex docked nodes and setup their docker
            int dockindex = 0;

            for(auto it = _Context->m_WindowsDockingCache.begin(); it != _Context->m_WindowsDockingCache.end(); it++)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                if(window == nullptr)
                    continue;

                switch (_Orientation)
                {
                    case ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Top:
                    window->TopDocker = docker->Top;
                        break;
                    
                    case ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Left:
                    window->LeftDocker = docker->Left;
                        break;

                    case ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Right:
                    window->RightDocker = docker->Right;
                        break;

                    case ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Bottom:
                    window->BottomDocker = docker->Bottom;
                        break;
                }

                window->DockingIndex = dockindex++;
            }
        }
    
        static void detach_from_docker(
            ImmediateUserInterfaceContextLayer* _Context,
            ImmediateUserInterfaceWindow*       _Detached)
        {
            if(_Detached == nullptr)
                return;

            // reindex docked nodes of the docker of the moved node and setup new currently active window
            if( _Detached->TopDocker     != nullptr ||
                _Detached->LeftDocker    != nullptr ||
                _Detached->RightDocker   != nullptr ||
                _Detached->BottomDocker  != nullptr ||
                _Detached->CentralDocker != nullptr)
            {
                ImmediateUserInterfaceWindow* active = nullptr;
                
                for (int orientation = ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Begin;
                         orientation < ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_End;
                         orientation++)
                {
                    active = dynamic_cast<ImmediateUserInterfaceWindow*>(_Context->m_Hierarchy.get_parent(_Detached));

                    auto& dockedWindows = retrieve_docked_windows(
                        _Context,
                        active,
                        (ImmedidateUserInterfaceDockingAnchor_)orientation);

                    int dockindex = 0;

                    for(auto it = dockedWindows.begin(); it != dockedWindows.end(); it++)
                    {
                        ImmediateUserInterfaceWindow* window =
                            dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                        if(window != nullptr && window != _Detached)
                        {
                            window->DockingIndex = dockindex++;

                            // find docked window with the larges docking index
                            if(window->DockingIndex > active->DockingIndex)
                                active = window;
                        }
                    }
                }

                ImmediateUserInterfaceWindow::ImmediateUserInterfaceWindowActivator().setup_as_active_docking_window(
                    _Context,
                    active,
                    [_Detached](const ImmediateUserInterfaceNode* _Node)->bool{return _Node != _Detached;});

                _Detached->TopDocker     = nullptr;
                _Detached->LeftDocker    = nullptr;
                _Detached->RightDocker   = nullptr;
                _Detached->BottomDocker  = nullptr;
                _Detached->CentralDocker = nullptr;
                _Detached->DockingIndex  = -1;
                return;
            }

            // TODO: reattach children to the first child
            _Detached->TopDocker     = nullptr;
            _Detached->LeftDocker    = nullptr;
            _Detached->RightDocker   = nullptr;
            _Detached->BottomDocker  = nullptr;
            _Detached->CentralDocker = nullptr;
            _Detached->DockingIndex  = -1;
        }

        static std::vector<ImmediateUserInterfaceNode*>& retrieve_docked_windows(
            ImmediateUserInterfaceContextLayer*          _Context,
            ImmediateUserInterfaceNode*                  _Docker,
            const ImmedidateUserInterfaceDockingAnchor_& _Orientation)
        {
            // get ready
            _Context->m_WindowsDockingList.clear();

            ImmediateUserInterfaceWindow* docker =
                dynamic_cast<ImmediateUserInterfaceWindow*>(_Docker);

            if(docker == nullptr)
                return _Context->m_WindowsDockingList;

            switch (_Orientation)
            {
                case ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Top:
                for(auto it = _Context->m_Hierarchy.begin(docker->Top); it != _Context->m_Hierarchy.end(docker->Top); it++)
                    _Context->m_WindowsDockingList.push_back(*it);
                    break;
                
                case ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Left:
                for(auto it = _Context->m_Hierarchy.begin(docker->Left); it != _Context->m_Hierarchy.end(docker->Left); it++)
                    _Context->m_WindowsDockingList.push_back(*it);
                    break;

                case ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Right:
                for(auto it = _Context->m_Hierarchy.begin(docker->Right); it != _Context->m_Hierarchy.end(docker->Right); it++)
                    _Context->m_WindowsDockingList.push_back(*it);
                    break;

                case ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Bottom:
                for(auto it = _Context->m_Hierarchy.begin(docker->Bottom); it != _Context->m_Hierarchy.end(docker->Bottom); it++)
                    _Context->m_WindowsDockingList.push_back(*it);
                    break;

                case ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center:
                for(auto it = _Context->m_Hierarchy.begin(_Docker); it != _Context->m_Hierarchy.end(_Docker); it++)
                {
                    ImmediateUserInterfaceWindow* window =
                        dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                    if(window != nullptr && window->CentralDocker == _Docker)
                        _Context->m_WindowsDockingList.push_back(window);
                }
                    break;
            }

            return _Context->m_WindowsDockingList;
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

    ImmedidateUserInterfaceWindowController::execute(this, event);
    ImmedidateUserInterfaceEventsController::execute(this, event);
    ImmedidateUserInterfaceLayoutController::execute(this, event);
}

void ImmediateUserInterfaceContextLayer::frame_render()
{
    class ImmediateUserInterfaceNodeRenderer
    {
    public:
        static void execute(ImmediateUserInterfaceContextLayer* _Context)
        {
            // get ready
            _Context->m_NodesRenderingCache.clear();

            // compute initial depth of singletons
            std::sort(
                _Context->m_Hierarchy.Singletons.begin(),
                _Context->m_Hierarchy.Singletons.end(),
                [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
                {
                    return _A->State.RenderingOrder < _B->State.RenderingOrder;
                }
            );

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

                ImmediateUserInterfaceNodeRenderer::render_node(_Context, singleton);
                _Context->m_NodesRenderingCache.push_back(singleton);
            }
        }

    private:

        static void render_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
        {
            if(_Node->State.Hidden)
                return;

            // calculate clippingbox
            {
                auto next   = _Node;
                auto parent = _Context->m_Hierarchy.get_parent(_Node);

                while (parent != nullptr)
                {
                    next   = parent;
                    parent = _Context->m_Hierarchy.get_parent(parent);
                }

                _Context->m_Renderer->push_clip_box(
                    _Node->State.BoundingBox.clip_with(next->State.BoundingBox));
            }

            // setup depth
            if(_Context->m_Hierarchy.get_parent(_Node) != nullptr)
            {
                _Node->State.Depth = 
                    _Context->m_Hierarchy.get_parent(_Node)->State.Depth +
                    _Context->m_Hierarchy.get_parent(_Node)->State.SelfThickness + 1;
            }

            // render self and children
            _Node->render(_Context);

            for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
                render_node(_Context, (*it));

            // update parental bandwidth
            if(_Context->m_Hierarchy.get_parent(_Node))
                _Context->m_Hierarchy.get_parent(_Node)->State.TotalThickness += _Node->State.TotalThickness;

            // remove clipping
            _Context->m_Renderer->pop_clip_box();
        }
    };

    ImmediateUserInterfaceNodeRenderer::execute(this);
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

        // save cache
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
        
        if(node->State.Events == ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
            node->Cache = node->State;

        node->State.Depth          = 0;
        node->State.SelfThickness  = 0;
        node->State.TotalThickness = 0;
        node->State.RenderingIndex = 0;
        node->State.Parent         = nullptr;
        node->State.Settings       = 0;
    }

    GS_ASSERT(m_NodesRenderingStack.empty());

    // rendering
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
    if(begin_node<ImmediateUserInterfaceWindow>(_ID, _Settings, _Opened))
    {
        ImmediateUserInterfaceWindow* window = get_rendering_stack_top<ImmediateUserInterfaceWindow>();
        window->Opened = _Opened;

        if(begin_node<ImmediateUserInterfaceWindowVerticalDocker>(
            std::string(_ID).append("/VerticalDocker"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
        {
            window->Docker = get_rendering_stack_top<ImmediateUserInterfaceWindowVerticalDocker>();

            // top
            if(begin_node<ImmediateUserInterfaceWindowHorizontalDocker>(
                std::string(_ID).append("/VerticalDockerTop"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
            {
                window->Top = get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalDocker>();
                end_node<ImmediateUserInterfaceWindowHorizontalDocker>();
            }

            // center
            if(begin_node<ImmediateUserInterfaceWindowHorizontalDocker>(
                std::string(_ID).append("/VerticalDockerCenter"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
            {
                if(begin_node<ImmediateUserInterfaceWindowHorizontalDocker>(
                    std::string(_ID).append("/HorizontalDockerLeft"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
                {
                    window->Left = get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalDocker>();
                    end_node<ImmediateUserInterfaceWindowHorizontalDocker>();
                }

                if(begin_node<ImmediateUserInterfaceWindowVerticalDocker>(
                    std::string(_ID).append("/HorizontalDockerCenter123"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
                {
                    window->Center = get_rendering_stack_top<ImmediateUserInterfaceWindowVerticalDocker>();
                    end_node<ImmediateUserInterfaceWindowVerticalDocker>();
                }

                if(begin_node<ImmediateUserInterfaceWindowHorizontalDocker>(
                    std::string(_ID).append("/HorizontalDockerRight"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
                {
                    window->Right = get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalDocker>();
                    end_node<ImmediateUserInterfaceWindowHorizontalDocker>();
                }

                end_node<ImmediateUserInterfaceWindowHorizontalDocker>();
            }

            // bottom
            if(begin_node<ImmediateUserInterfaceWindowHorizontalDocker>(
                std::string(_ID).append("/VerticalDockerBottom"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
            {
                window->Bottom = get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalDocker>();
                end_node<ImmediateUserInterfaceWindowHorizontalDocker>();
            }

            end_node<ImmediateUserInterfaceWindowVerticalDocker>();
        }

        return true;
    }

    return false;
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