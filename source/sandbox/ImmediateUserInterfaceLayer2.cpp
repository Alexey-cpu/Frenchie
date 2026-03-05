#include <ImmediateUserInterfaceLayer2.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

// STL
#include <algorithm>
#include <typeinfo>
#include <typeindex>

//#define IMMEDIATE_USER_INTERFACE_DEBUG

using namespace Frenchie::Application;

#ifdef IMMEDIATE_USER_INTERFACE_DEBUG

#include <iostream>

void showHierarchy(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const std::string& _Delimiter)
{
    std::cout << _Delimiter << _Node->Hash << "\t" << _Node->State.Depth << "\t" << _Node->State.SelfThickness << "\t" << _Node->State.MaximumChildDepth << "\n";

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
    {
        showHierarchy(_Context, (*it), _Delimiter + "\t");
    }
}
#endif

namespace Frenchie
{
    namespace Application
    {
        namespace ImmediateUserInterfaceContextLayerHelpers
        {
            class ImmediateUserInterfaceWindowUtility
            {
            public:

                template<typename FrameProcessor>
                void process_window_frame(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Window, const FrameProcessor& _FrameProcessor)
                {
                    if(_Context == nullptr || _Window == nullptr || _Window->Docker != nullptr)
                        return;

                    // calculate total windows frames size and frames count
                    gs_vec2f totalFramesSize  = gs_vec2f(0.f, 0.f);
                    int      totalFramesCount = 0;

                    for(auto it  = _Context->m_Hierarchy.begin(_Window->DockerView);
                             it != _Context->m_Hierarchy.end(_Window->DockerView);
                             it++)
                    {
                        if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) == nullptr) continue;

                        totalFramesSize += gs_vec2f(
                            _Context->m_Renderer->calculate_bounding_box((*it)->Name.begin(), (*it)->Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).width(),
                            0.f);
                        totalFramesCount++;
                    }

                    totalFramesSize += gs_vec2f(
                        _Context->m_Renderer->calculate_bounding_box(_Window->Name.begin(), _Window->Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).width(),
                        0.f);
                    totalFramesCount++;

                    totalFramesSize = gs_vec2f(
                        gs_min(totalFramesSize.x, _Window->FrameBox.width() * 0.8f), // TODO: 0.8f MUST BE A SETTING !!!
                        _Window->FrameBox.height());

                    // calculate frame unlocking and dragging panes
                    gs_2dboxf frameBoxUnlockingPane = gs_2dboxf(_Window->FrameBox.Min, _Window->FrameBox.Min + totalFramesSize);

                    // process frameboxes
                    gs_vec2f position = frameBoxUnlockingPane.Min;
                    gs_vec2f size     = gs_vec2f((frameBoxUnlockingPane.size() / (float)totalFramesCount).x, frameBoxUnlockingPane.size().y);

                    // process self frame
                    gs_2dboxf boundingBox = gs_2dboxf(position, position + size);
                    _FrameProcessor(_Context, _Window->FrameBox, boundingBox, _Window);
                    position += gs_vec2f(size.x, 0.f);

                    // process docked windows frames
                    for(auto it  = _Context->m_Hierarchy.begin(_Window->DockerView);
                             it != _Context->m_Hierarchy.end(_Window->DockerView);
                             it++)
                    {
                        ImmediateUserInterfaceWindow* window =
                            dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                        if(window == nullptr || window->Docker == nullptr)
                            continue;

                        _FrameProcessor(
                            _Context,
                            gs_2dboxf(gs_vec2f(0.f), gs_vec2f(0.f)),
                            gs_2dboxf(position, position + size),
                            dynamic_cast<ImmediateUserInterfaceWindow*>(*it));

                        position += gs_vec2f(size.x, 0.f);
                    }
                }

                ImmediateUserInterfaceWindow* retrieve_docker_by_view(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _DockerView)
                {
                    ImmediateUserInterfaceNode* parent = _DockerView;

                    while (parent)
                    {
                        if(dynamic_cast<ImmediateUserInterfaceWindow*>(parent))
                            return dynamic_cast<ImmediateUserInterfaceWindow*>(parent);
                        parent = _Context->m_Hierarchy.get_parent(parent);
                    }
                    
                    return nullptr;
                }
            };

            class ImmedidateUserInterfaceMovedNodeSearcher
            {
            public:
                template<typename FrameProcessor>
                ImmediateUserInterfaceNode* search(ImmediateUserInterfaceContextLayer* _Context, const FrameProcessor& _Filter)
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

                template<typename FrameProcessor>
                ImmediateUserInterfaceNode* search_recursive(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Moved, const FrameProcessor& _Filter)
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

                template<typename FrameProcessor>
                ImmediateUserInterfaceNode* search(ImmediateUserInterfaceContextLayer* _Context, const FrameProcessor& _Filter)
                {
                    // find top most hovered singleton window or a snapped window not equal to the moved one
                    ImmediateUserInterfaceNode* hovered  = nullptr;

                    for(auto singleton : _Context->m_Hierarchy.Singletons)
                        search_recursive(_Context, singleton, &hovered, _Filter);

                    return hovered;
                };

            private:

                template<typename FrameProcessor>
                void search_recursive(
                    ImmediateUserInterfaceContextLayer* _Context,
                    ImmediateUserInterfaceNode*         _Next,
                    ImmediateUserInterfaceNode**        _Hovered,
                    const FrameProcessor&               _Filter)
                {
                    if(_Context == nullptr || _Next == nullptr || !_Next->State.BoundingBox.contains(_Context->m_Input.get_cusor_position()))
                        return;

                    // check self
                    if(_Filter(_Next))
                    {
                        if(*_Hovered == nullptr || _Next->Cache.Depth > (*_Hovered)->Cache.Depth)
                            *_Hovered = _Next;
                    }

                    // check children
                    for(auto it = _Context->m_Hierarchy.begin(_Next); it != _Context->m_Hierarchy.end(_Next); it++)
                        search_recursive(_Context, *it, _Hovered, _Filter);
                }
            };

            float calculate_offset(ImmediateUserInterfaceContextLayer* _Context)
            {
                return _Context != nullptr ? gs_max(4.f, _Context->m_Style.get_frames_width()) : 16.f;
            }

            gs_vec2f calculate_scrollbar_length(gs_vec2f scrollbarMinimumValue, gs_vec2f scrollbarMaximumValue, gs_vec2f totalContentSize, gs_vec2f scrollbarMinimumSize)
            {
                return gs_vec2f(
                    gs_min(gs_max(gs_abs(scrollbarMaximumValue.x - scrollbarMinimumValue.x) / gs_abs(totalContentSize.x) * scrollbarMaximumValue.x, scrollbarMinimumSize.x), scrollbarMaximumValue.x),
                    gs_min(gs_max(gs_abs(scrollbarMaximumValue.y - scrollbarMinimumValue.y) / gs_abs(totalContentSize.y) * scrollbarMaximumValue.y, scrollbarMinimumSize.y), scrollbarMaximumValue.y));
            }

            gs_vec2f calculate_scrollbar_slider_position_scale(gs_vec2f scrollbarMinimumValue, gs_vec2f scrollbarMaximumValue, gs_vec2f totalContentSize)
            {
                return gs_vec2f(
                    gs_abs(totalContentSize.x), gs_abs(totalContentSize.y)) / gs_vec2f(gs_abs(scrollbarMaximumValue.x - scrollbarMinimumValue.x),
                    gs_abs(scrollbarMaximumValue.y - scrollbarMinimumValue.y));
            }

            int calculate_depth_over_node(const ImmediateUserInterfaceNode* _Node)
            {
                if(_Node == nullptr)
                    return 0;

                return gs_max(
                    _Node->Cache.MaximumChildDepth + _Node->Cache.MaximumChildThickness - _Node->Cache.Depth,
                    _Node->Cache.MaximumChildDepth + _Node->Cache.SelfThickness + 1,
                    _Node->Cache.Depth + _Node->Cache.SelfThickness + 1);
            }

            int calculate_layer_depth(ImmediateUserInterfaceContextLayer* _Context, int _Layer)
            {
                return (int)(_Layer * _Context->m_Renderer->get_far_plane() / (ImmedidateUserInterfaceRenderingLayer_::ImmedidateUserInterfaceRenderingLayer_End - ImmedidateUserInterfaceRenderingLayer_::ImmedidateUserInterfaceRenderingLayer_Begin));
            };

            gs_vec2f layout_compute_aligned_position(
                const int       _Settings,
                const gs_vec2f& _Position,
                const gs_vec2f& _Size,
                gs_vec2f        _ChildrenSize)
            {
                gs_2dboxf parentBoundingBox = gs_2dboxf(_Position, _Position + _Size);
                gs_vec2f  position = _Position;

                // vertical alignment
                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop)
                    position = gs_vec2f(position.x, _Position.y);
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom)
                    position = gs_vec2f(position.x, (position + parentBoundingBox.size() - _ChildrenSize).y);
                else // center is default
                    position = gs_vec2f(position.x, (parentBoundingBox.center() - _ChildrenSize * 0.5f).y);

                // horizontal alignment
                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft)
                    position = gs_vec2f(_Position.x, position.y);
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight)
                    position = gs_vec2f((position + parentBoundingBox.size() - _ChildrenSize).x, position.y);
                else // center is default
                    position = gs_vec2f((parentBoundingBox.center() - _ChildrenSize * 0.5f).x, position.y);

                return position;
            }

            template<typename Type, typename FrameProcessor>
            void layout_nodes_as_panel(
                const Type&           _Begin,
                const Type&           _End,
                const gs_vec2f&       _Position,
                const gs_vec2f&       _Size,
                const int             _Settings,
                const FrameProcessor& _Filter)
            {
                // layout children
                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) == nullptr || !_Filter(*it)) continue;

                    gs_vec2f size     = gs_clamp(_Size, (*it)->State.MinimumSize, (*it)->State.MaximumSize);
                    gs_vec2f position = layout_compute_aligned_position( _Settings, _Position, _Size, size);
                    (*it)->State.BoundingBox = gs_2dboxf(position, position + size);
                }
            }

            template<typename Type, typename FrameProcessor>
            void layout_nodes_as_vertical_stack(
                const Type&           _Begin,
                const Type&           _End,
                const gs_vec2f&       _Position,
                const gs_vec2f&       _Size,
                const gs_vec2f&       _Padding,
                const int             _Settings,
                const FrameProcessor& _Filter)
            {
                gs_vec2f position  = _Position + _Padding;
                gs_vec2f totalsize = gs_vec2f(0.f, 0.f);

                // compute total children size
                totalsize += _Padding * 2.f;

                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) != nullptr && _Filter(*it))
                        totalsize += (*it)->State.BoundingBox.size();
                }

                // compute children size scale
                gs_vec2f scale = _Size / gs_vec2f(gs_max(totalsize.x, 1.f), gs_max(totalsize.y, 1.f));

                // layout children and compute their bounding box
                gs_2dboxf childrenBoundingBox = gs_2dboxf(position, position);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if(*it == nullptr || !_Filter(*it))
                        continue;

                    gs_vec2f size = gs_vec2f(_Size.x, ((*it)->State.BoundingBox.size() * scale).y);
                    size = gs_clamp(size, (*it)->State.MinimumSize, (*it)->State.MaximumSize);
                    (*it)->State.BoundingBox = gs_2dboxf(position, position + size);
                    position += gs_vec2f(0.f, size.y + _Padding.y);

                    childrenBoundingBox = gs_2dboxf(
                        childrenBoundingBox.Min,
                        childrenBoundingBox.Max,
                        (*it)->State.BoundingBox.Min,
                        (*it)->State.BoundingBox.Max);
                }

                // align children within parent
                position = layout_compute_aligned_position(_Settings, _Position, _Size, childrenBoundingBox.size());

                for(auto it = _Begin; it != _End; ++it)
                {
                    if(*it == nullptr || !_Filter(*it))
                        continue;

                    gs_vec2f size = (*it)->State.BoundingBox.size();

                    (*it)->State.BoundingBox = gs_2dboxf(position, position + size);
                    position += gs_vec2f(0.f, size.y + _Padding.y);
                }
            }

            template<typename Type, typename FrameProcessor>
            void layout_nodes_as_horizontal_stack(
                const Type&           _Begin,
                const Type&           _End,
                const gs_vec2f&       _Position,
                const gs_vec2f&       _Size,
                const gs_vec2f&       _Padding,
                const int             _Settings,
                const FrameProcessor& _Filter)
            {
                gs_vec2f position   = _Position + _Padding;
                gs_vec2f totalsize  = gs_vec2f(0.f, 0.f);

                // compute total size
                totalsize += _Padding * 2.f;

                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) != nullptr && _Filter(*it))
                        totalsize += (*it)->State.BoundingBox.size() + _Padding;
                }

                // compute children scale
                gs_vec2f scale = _Size / gs_vec2f(gs_max(totalsize.x, 1.f), gs_max(totalsize.y, 1.f));

                // layout children and compute their bounding box
                gs_2dboxf childrenBoundingBox = gs_2dboxf(position, position);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if(*it == nullptr || !_Filter(*it))
                        continue;

                    gs_vec2f size = gs_vec2f(((*it)->State.BoundingBox.size() * scale).x, _Size.y);
                    size = gs_clamp(size, (*it)->State.MinimumSize, (*it)->State.MaximumSize);
                    (*it)->State.BoundingBox = gs_2dboxf(position, position + size);
                    position += gs_vec2f(size.x + _Padding.x, 0.f);

                    childrenBoundingBox = gs_2dboxf(
                        childrenBoundingBox.Min,
                        childrenBoundingBox.Max,
                        (*it)->State.BoundingBox.Min,
                        (*it)->State.BoundingBox.Max);
                }

                // align children within parent
                position = layout_compute_aligned_position(_Settings, _Position, _Size, childrenBoundingBox.size());

                for(auto it = _Begin; it != _End; ++it)
                {
                    if(*it == nullptr || !_Filter(*it))
                        continue;

                    gs_vec2f size = (*it)->State.BoundingBox.size();

                    (*it)->State.BoundingBox = gs_2dboxf(position, position + size);
                    position += gs_vec2f(size.x + _Padding.x, 0.f);
                }
            }

            gs_2d_ellipsef build_resize_top_left_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 32.f);
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Min, WindowResizeAngleGizmoRadius);
            }

            gs_2d_ellipsef build_resize_top_right_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Min + gs_vec2f(_Node->get_visible_rect(_Context).width(), 0.f), WindowResizeAngleGizmoRadius);
            }

            gs_2d_ellipsef build_resize_bottom_left_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 32.f);
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Max - gs_vec2f(_Node->get_visible_rect(_Context).width(), 0.f), WindowResizeAngleGizmoRadius);
            };

            gs_2d_ellipsef build_resize_bottom_right_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 32.f);
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Max, WindowResizeAngleGizmoRadius);
            };

            gs_2dboxf build_resize_top_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = _Context->m_Style.get_frames_width() * 2.f;

                return gs_2dboxf(
                    _Node->get_visible_rect(_Context).Min - gs_vec2f(0.f, WindowResizeSideGizmoWidth),
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(_Node->get_visible_rect(_Context).width(), WindowResizeSideGizmoWidth));
            };

            gs_2dboxf build_resize_left_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = _Context->m_Style.get_frames_width() * 2.f;

                return gs_2dboxf(
                    _Node->get_visible_rect(_Context).Min - gs_vec2f(WindowResizeSideGizmoWidth, 0.f),
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(WindowResizeSideGizmoWidth, _Node->get_visible_rect(_Context).height()));
            };

            gs_2dboxf build_resize_right_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = _Context->m_Style.get_frames_width() * 2.f;

                return gs_2dboxf(
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(_Node->get_visible_rect(_Context).width() - WindowResizeSideGizmoWidth, 0.f),
                    _Node->get_visible_rect(_Context).Max + gs_vec2f(WindowResizeSideGizmoWidth, 0.f));
            };

            gs_2dboxf build_resize_bottom_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = _Context->m_Style.get_frames_width() * 2.f;

                return gs_2dboxf(
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(0.f, _Node->get_visible_rect(_Context).height() - WindowResizeSideGizmoWidth),
                    _Node->get_visible_rect(_Context).Max + gs_vec2f(0.f, WindowResizeSideGizmoWidth));
            };
        
            void render_resize_gizmo(
                ImmediateUserInterfaceContextLayer*     _Context,
                ImmediateUserInterfaceNode*             _Node,
                const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
            {
                if(_Context == nullptr || _Node == nullptr) return;

                int depth = ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node(_Node);

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
                {
                    auto resizeTopLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Context, _Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeTopLeft.Center,
                        resizeTopLeft.Radius,
                        resizeTopLeft.Radius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
                {
                    auto resizeTopRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_right_ellipse(_Context,_Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeTopRight.Center,
                        resizeTopRight.Radius,
                        resizeTopRight.Radius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
                {
                    auto resizeBottomLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_left_ellipse(_Context,_Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeBottomLeft.Center,
                        resizeBottomLeft.Radius,
                        resizeBottomLeft.Radius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
                {
                    auto resizeBottomRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_right_ellipse(_Context,_Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeBottomRight.Center,
                        resizeBottomRight.Radius,
                        resizeBottomRight.Radius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
                {
                    auto resizeTop = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        resizeTop.Min,
                        resizeTop.Max,
                        16.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
                {
                    auto resizeLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_left_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        resizeLeft.Min,
                        resizeLeft.Max,
                        16.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
                {
                    auto resizeRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_right_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        resizeRight.Min,
                        resizeRight.Max,
                        16.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
                {
                    auto resizeBottom = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        resizeBottom.Min,
                        resizeBottom.Max,
                        16.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            };

            bool check_cursor_intersection_with_resize_gizmo(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
            {
                if(_Context == nullptr || _Node == nullptr) return false;

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Context, _Node).contains(_Context->m_Input.get_cusor_position());

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_top_right_ellipse(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_left_ellipse(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_right_ellipse(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_top_box(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_left_box(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_right_box(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_box(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
                
                return false;
            };

            ImmediateUserInterfaceNode* find_resizable_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
            {
                if(_Context == nullptr || _Node == nullptr) return nullptr;

                // find resizable node
                ImmediateUserInterfaceNode* resizable = _Node;

                // pass event to a parent
                while (_Context->m_Hierarchy.get_parent(resizable) &&
                        check_cursor_intersection_with_resize_gizmo(_Context, _Context->m_Hierarchy.get_parent(resizable), _ResizeEventType))
                    resizable = _Context->m_Hierarchy.get_parent(resizable);

                return resizable;
            };

            void resize_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
            {
                if(_Node == nullptr)
                    return;

                gs_2dboxf box = _Node->State.BoundingBox;

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + _Context->m_Input.get_cusor_drag_delta(), _Node->Cache.BoundingBox.Max);
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y), _Node->Cache.BoundingBox.Max + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f));
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f), _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y));
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min, _Node->Cache.BoundingBox.Max + ApplicationPlatformBackend::get_window_cursor_dragdelta());
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y), _Node->Cache.BoundingBox.Max);
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f), _Node->Cache.BoundingBox.Max);
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min, _Node->Cache.BoundingBox.Max + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f));
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min, _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y));

                _Node->State.BoundingBox = gs_2dboxf(box.Min, box.Min + gs_clamp(box.size(), _Node->State.MinimumSize, _Node->State.MaximumSize));
            };
        }

        // windows
        struct ImmediateUserInterfaceWindowCentralDocker : public ImmediateUserInterfaceNodePanel
        {
            ImmediateUserInterfaceWindowCentralDocker(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
            virtual ~ImmediateUserInterfaceWindowCentralDocker(){}
        };

        struct ImmediateUserInterfaceWindowVerticalSnapper : public ImmediateUserInterfaceNodeVerticalStack
        {
            ImmediateUserInterfaceWindowVerticalSnapper(const std::string& _Name) : ImmediateUserInterfaceNodeVerticalStack(_Name){}
            virtual ~ImmediateUserInterfaceWindowVerticalSnapper(){}
        };

        struct ImmediateUserInterfaceWindowHorizontalSnapper : public ImmediateUserInterfaceNodeHorizontalStack
        {
            ImmediateUserInterfaceWindowHorizontalSnapper(const std::string& _Name) : ImmediateUserInterfaceNodeHorizontalStack(_Name){}
            virtual ~ImmediateUserInterfaceWindowHorizontalSnapper(){}

            virtual void measure(ImmediateUserInterfaceContextLayer* _Context) override
            {
                ImmediateUserInterfaceNodeHorizontalStack::measure(_Context);

                State.MinimumSize = gs_vec2f(0.f, 0.f);

                if(_Context->m_Hierarchy.size(this) <= 0)
                    State.BoundingBox = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min);
                else if(gs_min(State.BoundingBox.size().x, State.BoundingBox.size().y) <= 1.f)
                    State.BoundingBox = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min + gs_vec2f(512.f, 512.f));
            }
        };
    
        struct ImmediateUserInterfaceWindowDockArea : public ImmediateUserInterfaceWindow
        {
            ImmediateUserInterfaceWindowDockArea(const std::string& _Name) : ImmediateUserInterfaceWindow(_Name){}
            virtual ~ImmediateUserInterfaceWindowDockArea(){}

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
            {
                State.BoundingBox = gs_2dboxf(
                    _Context->m_Renderer->current_viewport().Min - _Context->m_Style.get_frames_width(),
                    _Context->m_Renderer->current_viewport().Max + _Context->m_Style.get_frames_width());

                ImmediateUserInterfaceWindow::layout(_Context);
            }

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr)
                    return;

                // content background and outline frame
                _Context->m_Renderer->push_rectangle_rounded_filled(
                    State.BoundingBox.Min,
                    State.BoundingBox.Max,
                    _Context->m_Style.get_frames_radius(),
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                _Context->m_Renderer->push_rectangle_rounded_filled(
                    FrameBox.Min + _Context->m_Style.get_frames_width() * 2.f,
                    FrameBox.Max - _Context->m_Style.get_frames_width() * 2.f,
                    _Context->m_Style.get_frames_radius(),
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // frame
                ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().process_window_frame(
                    _Context,
                    this,

                    // render self and child frames
                    [this](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _FrameBox, const gs_2dboxf& _Frame, ImmediateUserInterfaceWindow* _Window)
                    {
                        // detect if we are active
                        bool active = _Window->IsActive;

                        // render frame
                        if(_Window->Docker == nullptr &&
                            _Context->get_controller<ImmedidateUserInterfaceWindowController>()->retrieve_docked_windows(
                                _Context,
                                _Window,
                                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center).empty())
                        {
                            // frame
                            if(_Frame.contains(_Context->m_Input.get_cusor_position()) &&
                                (_Window->TopSnapper    != nullptr ||
                                _Window->LeftSnapper   != nullptr ||
                                _Window->RightSnapper  != nullptr ||
                                _Window->BottomSnapper != nullptr))
                            {
                                _Context->m_Renderer->push_rectangle_rounded_filled(
                                    _Frame.Min + _Context->m_Style.get_frames_width(),
                                    _Frame.Max - _Context->m_Style.get_frames_width(),
                                    _Context->m_Style.get_frames_radius(),
                                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered),
                                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                            }
                        }
                        else
                        {
                            // frame
                            if(active)
                            {
                                _Context->m_Renderer->push_rectangle_rounded_filled(
                                    _Frame.Min + _Context->m_Style.get_frames_width(),
                                    _Frame.Max - _Context->m_Style.get_frames_width(),
                                    _Context->m_Style.get_frames_radius(),
                                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
                                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                                    true,
                                    true,
                                    false,
                                    false);
                            }
                            else if(_Frame.contains(_Context->m_Input.get_cusor_position()))
                            {
                                _Context->m_Renderer->push_rectangle_rounded_filled(
                                    _Frame.Min + _Context->m_Style.get_frames_width(),
                                    _Frame.Max - _Context->m_Style.get_frames_width(),
                                    _Context->m_Style.get_frames_radius(),
                                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered),
                                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                                    true,
                                    true,
                                    false,
                                    false);
                            }
                        }

                        // title
                        _Context->m_Renderer->push_text(
                            gs_vec2f(
                                _Frame.Min.x + 16.f + _Context->m_Style.get_frames_width(),
                                _Frame.center().y - _Context->m_Renderer->calculate_bounding_box(_Window->Name.begin(), _Window->Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).height() * 0.6f * 0.5f),
                            _Window->Name.begin(),
                            _Window->Name.end(),
                            _Context->m_Style.get_font_size() * 0.6f,
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                    }
                );
            }
        };

        // scrollarea
        struct ImmediateUserInterfaceScrollAreaPanel : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceScrollAreaPanel(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
            virtual ~ImmediateUserInterfaceScrollAreaPanel(){}
        };

        struct ImmediateUserInterfaceScrollAreaScrollBar : public ImmediateUserInterfaceNodePanel
        {
        public:
            enum ImmediateUserInterfaceScrollAreaScrollBarType_
            {
                ImmediateUserInterfaceScrollAreaScrollBarType_Vertical,
                ImmediateUserInterfaceScrollAreaScrollBarType_Horizontal,
            } Type = ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical;

            ImmediateUserInterfaceScrollAreaScrollBar(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name)
            {
                ContentPadding = gs_vec2f(4.f, 4.f);
            }

            virtual ~ImmediateUserInterfaceScrollAreaScrollBar(){}

            // getters
            gs_vec2f get_scroll_offset() const
            {
                return Position * PositionScale;
            }

            // setters
            void set_scroll_offset(const gs_vec2f _Value)
            {
                if(gs_max(State.BoundingBox.size().x, State.BoundingBox.size().y) <= 0.f) return;

                PreviousPosition = Position;
                Position         = gs_clamp(PreviousPosition + _Value, gs_vec2f(0.f, 0.f), State.BoundingBox.size() - Size);
            }

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
            {
                // auxiliary lambdas
                auto do_not_render_scroll_bar = [this](ImmediateUserInterfaceContextLayer* _Context)
                {
                    // clamp self
                    State.MaximumSize = gs_vec2f(0.f, 0.f);
                    State.MinimumSize = gs_vec2f(0.f, 0.f);
                    State.BoundingBox = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min + State.MaximumSize);

                    // reset slider
                    Position = gs_vec2f(0.f, 0.f);
                };

                ImmediateUserInterfaceScrollArea* contentArea = retrieve_scroll_area(_Context);

                if(_Context == nullptr || contentArea == nullptr) return;

                // calculate self maximum and minimum sizes
                State.MaximumSize =
                    Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical ?
                        gs_vec2f(_Context->m_Style.get_scrollbar_width(), (State.Parent != nullptr ? State.Parent->State.BoundingBox.size().y : 512.f)) :
                            gs_vec2f((State.Parent != nullptr ? State.Parent->State.BoundingBox.size().x : 512.f), _Context->m_Style.get_scrollbar_width());

                if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar) &&
                    Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Horizontal)
                {
                    State.MaximumSize.y = 0.f;
                    State.MinimumSize = State.MaximumSize;
                    State.BoundingBox = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min + State.MaximumSize);
                }
                else if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleVerticalScrollBar) &&
                    Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical)
                {
                    State.MaximumSize.x =  0.f;
                    State.MinimumSize   = State.MaximumSize;
                    State.BoundingBox   = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min + State.MaximumSize);
                }

                State.MinimumSize = State.MaximumSize;

                // calculate self bounding box
                gs_vec2f offset =
                    Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical ?
                        gs_vec2f(0.f, gs_abs(_Context->m_Style.get_frames_radius() * 2.f - _Context->m_Style.get_scrollbar_width()) * 0.5f) :
                            gs_vec2f(gs_abs(_Context->m_Style.get_frames_radius() * 2.f - _Context->m_Style.get_scrollbar_width()) * 0.5f, 0.f);

                if(_Context->m_Style.get_frames_radius() <= 0.f)
                    offset = gs_vec2f(0.f, 0.f);

                State.BoundingBox = gs_2dboxf(
                    State.BoundingBox.Min + offset,
                    State.BoundingBox.Min - (Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical ? gs_vec2f(0.f, 0.f) : offset) + State.MaximumSize);

                // calculate scrollbar metrics
                gs_vec2f scrollbarMinimumValue = gs_vec2f(0.f, 0.f);
                gs_vec2f scrollbarMaximumValue =
                    Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical ?
                        gs_vec2f(_Context->m_Style.get_scrollbar_width(), State.BoundingBox.size().y) :
                            gs_vec2f(State.BoundingBox.size().x, _Context->m_Style.get_scrollbar_width());

                // do not render scrollbar out of scrollarea
                if(contentArea == nullptr)
                {
                    do_not_render_scroll_bar(_Context);
                    return;
                }

                // manage horizontal scrollbar
                if(Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Horizontal)
                {
                    if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
                    {
                        do_not_render_scroll_bar(_Context);
                        return;
                    }

                    gs_vec2f scrollbarSliderLength = ImmediateUserInterfaceContextLayerHelpers::calculate_scrollbar_length(
                        scrollbarMinimumValue,
                        scrollbarMaximumValue,
                        contentArea->ContentView->State.ContentSize,
                        _Context->m_Style.get_scrollbar_width());

                    if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar))
                    {
                        if((int)gs_abs(scrollbarSliderLength.x - scrollbarMaximumValue.x) < _Context->m_Style.get_frames_width())
                        {
                            do_not_render_scroll_bar(_Context);
                            return;
                        }

                        if((int)scrollbarSliderLength.x >= (int)scrollbarMaximumValue.x)
                        {
                            do_not_render_scroll_bar(_Context);
                            return;
                        }
                    }
                    else if(contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverHorizontalScrollBar)
                    {
                        do_not_render_scroll_bar(_Context);
                        return;
                    }
                    else if(contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar)
                    {
                    }
                    else
                    {
                        do_not_render_scroll_bar(_Context);
                        return;
                    }
                }

                // manage vertical scrollbar
                if(Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical)
                {
                    if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
                    {
                        do_not_render_scroll_bar(_Context);
                        return;
                    }

                    gs_vec2f scrollbarSliderLength = ImmediateUserInterfaceContextLayerHelpers::calculate_scrollbar_length(
                        scrollbarMinimumValue,
                        scrollbarMaximumValue,
                        contentArea->ContentView->State.ContentSize,
                        _Context->m_Style.get_scrollbar_width());

                    if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar))
                    {
                        if((int)gs_abs(scrollbarSliderLength.y - scrollbarMaximumValue.y) < _Context->m_Style.get_frames_width())
                        {
                            do_not_render_scroll_bar(_Context);
                            return;
                        }

                        if((int)scrollbarSliderLength.y >= (int)scrollbarMaximumValue.y)
                        {
                            do_not_render_scroll_bar(_Context);
                            return;
                        }
                    }
                    else if(contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar)
                    {
                        do_not_render_scroll_bar(_Context);
                        return;
                    }
                    else if(contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysVerticalScrollBar)
                    {
                    }
                    else
                    {
                        do_not_render_scroll_bar(_Context);
                        return;
                    }
                }

                // layout child sliders
                gs_vec2f scrollbarSliderLength = ImmediateUserInterfaceContextLayerHelpers::calculate_scrollbar_length(
                    scrollbarMinimumValue,
                    scrollbarMaximumValue,
                    contentArea->ContentView->State.ContentSize + gs_min(State.BoundingBox.size().x, State.BoundingBox.size().y),
                    _Context->m_Style.get_scrollbar_width());

                gs_vec2f scrollbarSliderScale = ImmediateUserInterfaceContextLayerHelpers::calculate_scrollbar_slider_position_scale(
                    scrollbarMinimumValue,
                    scrollbarMaximumValue,
                    contentArea->ContentView->State.ContentSize + gs_min(State.BoundingBox.size().x, State.BoundingBox.size().y));

                gs_vec2f sliderPosition = gs_clamp(State.BoundingBox.Min + Position, State.BoundingBox.Min, State.BoundingBox.Max - scrollbarSliderLength);
                
                if(Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical)
                {
                    if(sliderPosition.y > State.BoundingBox.Min.y && sliderPosition.y < (State.BoundingBox.Max - scrollbarSliderLength).y)
                        PositionScale = scrollbarSliderScale;
                }
                else
                {
                    if(sliderPosition.x > State.BoundingBox.Min.x && sliderPosition.x < (State.BoundingBox.Max - scrollbarSliderLength).x)
                        PositionScale = scrollbarSliderScale;
                }

                Size = scrollbarSliderLength;
            }

            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override
            {
                // resize
                if(((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable) || _Context->m_Hierarchy.get_parent(this)) &&
                    !(State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
                {
                    auto scrollArea = retrieve_scroll_area(_Context);

                    if(Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical)
                    {
                        if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight) ||
                            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
                        {
                            ImmediateUserInterfaceNode* resizable =
                                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

                            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

                            // trigger event
                            if(resizable != nullptr)
                            {
                                if(_Context->m_Input.is_mouse_button_pressed())
                                {
                                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight;
                                    return true;
                                }

                                // execute event
                                if(_Context->m_Input.is_mouse_button_down() &&
                                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
                                {
                                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);
                                    return true;
                                }
                            }
                        }
                        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight) ||
                            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
                        {
                            ImmediateUserInterfaceNode* resizable =
                                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);

                            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);

                            // trigger event
                            if(resizable != nullptr)
                            {
                                if(_Context->m_Input.is_mouse_button_pressed())
                                {
                                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight;
                                    return true;
                                }

                                // execute event
                                if(_Context->m_Input.is_mouse_button_down() &&
                                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
                                {
                                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);
                                    return true;
                                }
                            }
                        }
                        else if((scrollArea != nullptr && (int)scrollArea->get_horizontal_scrollbar_width(_Context) <= 0.f) &&
                                ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
                        {
                            ImmediateUserInterfaceNode* resizable =
                                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

                            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

                            // trigger event
                            if(resizable != nullptr)
                            {
                                if(_Context->m_Input.is_mouse_button_pressed())
                                {
                                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight;
                                    return true;
                                }

                                // execute event
                                if(_Context->m_Input.is_mouse_button_down() &&
                                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
                                {
                                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);
                                    return true;
                                }
                            }
                        }
                    }
                    else
                    {
                        if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft) ||
                            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
                        {
                            ImmediateUserInterfaceNode* resizable =
                                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);

                            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);

                            // trigger event
                            if(resizable != nullptr)
                            {
                                if(_Context->m_Input.is_mouse_button_pressed())
                                {
                                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft;
                                    return true;
                                }

                                // execute event
                                if(_Context->m_Input.is_mouse_button_down() &&
                                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
                                {
                                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);
                                    return true;
                                }
                            }
                        }
                        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
                        {
                            ImmediateUserInterfaceNode* resizable =
                                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

                            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

                            // trigger event
                            if(resizable != nullptr)
                            {
                                if(_Context->m_Input.is_mouse_button_pressed())
                                {
                                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight;
                                    return true;
                                }

                                // execute event
                                if(_Context->m_Input.is_mouse_button_down() &&
                                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
                                {
                                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);
                                    return true;
                                }
                            }
                        }
                        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom) ||
                            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
                        {
                            ImmediateUserInterfaceNode* resizable =
                                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);

                            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);

                            // trigger event
                            if(resizable != nullptr)
                            {
                                if(_Context->m_Input.is_mouse_button_pressed())
                                {
                                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom;
                                    return true;
                                }

                                // execute event
                                if(_Context->m_Input.is_mouse_button_down() &&
                                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
                                {
                                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);
                                    return true;
                                }
                            }
                        }
                    }
                }

                // move slider on mouse press event
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    Position         = gs_clamp((_Context->m_Input.get_cusor_position() - State.BoundingBox.Min) - Size * 0.5f, gs_vec2f(0.f, 0.f), State.BoundingBox.size() - Size);
                    PreviousPosition = Position;
                    State.Events    |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_Custom;
                }

                // move slider on mouse drag event
                if(_Context->m_Input.is_mouse_button_down())
                {
                    Position      = gs_clamp(PreviousPosition + ApplicationPlatformBackend::get_window_cursor_dragdelta(), gs_vec2f(0.f, 0.f), State.BoundingBox.size() - Size);
                    State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_Custom;
                }

                return true;
            }

            void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

                // outline
                _Context->m_Renderer->push_rectangle_rounded_filled(
                    State.BoundingBox.Min,
                    State.BoundingBox.Max,
                    _Context->m_Style.get_frames_radius(),
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // background
                _Context->m_Renderer->push_rectangle_rounded_filled(
                    State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
                    State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
                    _Context->m_Style.get_frames_radius(),
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // slider
                gs_vec2f sliderPosition = gs_clamp(State.BoundingBox.Min + Position, State.BoundingBox.Min, State.BoundingBox.Max - Size);

                if(State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered)
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        sliderPosition + _Context->m_Style.get_frames_width() * 2.f,
                        sliderPosition + Size - _Context->m_Style.get_frames_width() * 2.f,
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }
                else
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        sliderPosition + _Context->m_Style.get_frames_width() * 2.f,
                        sliderPosition + Size - _Context->m_Style.get_frames_width() * 2.f,
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }
            }

        private:

            // info
            gs_vec2f Size             = gs_vec2f(0.f, 0.f);
            gs_vec2f Position         = gs_vec2f(0.f, 0.f);
            gs_vec2f PositionScale    = gs_vec2f(1.f, 1.f);
            gs_vec2f PreviousPosition = gs_vec2f(0.f, 0.f);

            ImmediateUserInterfaceScrollArea* retrieve_scroll_area(ImmediateUserInterfaceContextLayer* _Context)
            {
                if(_Context == nullptr) return nullptr;

                auto parent = _Context->m_Hierarchy.get_parent(this);

                while (parent)
                {
                    ImmediateUserInterfaceScrollArea* contentArea =
                        dynamic_cast<ImmediateUserInterfaceScrollArea*>(parent);

                    if(contentArea != nullptr)
                        return contentArea;
                        
                    parent = _Context->m_Hierarchy.get_parent(parent);
                }

                return nullptr;
            }
        };

        struct ImmediateUserInterfaceScrollAreaContent : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceScrollAreaContent(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
            ~ImmediateUserInterfaceScrollAreaContent(){}

            void layout(ImmediateUserInterfaceContextLayer* _Context)
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

                ImmediateUserInterfaceScrollArea* scrollArea = nullptr;
                ImmediateUserInterfaceNode*       parent     = _Context->m_Hierarchy.get_parent(this);

                while (parent)
                {
                    scrollArea = dynamic_cast<ImmediateUserInterfaceScrollArea*>(parent);
                    if(scrollArea != nullptr) break;
                    parent = _Context->m_Hierarchy.get_parent(parent);
                }
                
                gs_vec2f horizontalScrollBarPosition =
                    scrollArea != nullptr && scrollArea->HorizontalScrollBar != nullptr ?
                        scrollArea->HorizontalScrollBar->get_scroll_offset() :
                            gs_vec2f(0.f, 0.f);

                gs_vec2f verticalScrollBarPosition =
                    scrollArea != nullptr ?
                        scrollArea->VerticalScrollBar->get_scroll_offset() :
                            gs_vec2f(0.f, 0.f);

                gs_vec2f position  = State.BoundingBox.Min - gs_vec2f(horizontalScrollBarPosition.x, verticalScrollBarPosition.y) + ContentPadding;
                gs_vec2f start     = position;
                float    maxHeight = 0.f;

                for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
                {
                    (*it)->State.BoundingBox = gs_2dboxf(
                        position,
                        position + gs_clamp((*it)->State.BoundingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));

                    maxHeight = gs_max(maxHeight, (*it)->State.BoundingBox.height());

                    if((*it)->State.NextLine > 0)
                    {
                        position  = gs_vec2f(start.x, position.y + maxHeight * (*it)->State.NextLine + ContentPadding.y);
                        maxHeight = 0.f;
                    }
                    else
                    {
                        position += gs_vec2f((*it)->State.BoundingBox.size().x + ContentPadding.x, 0.f);
                    }
                }
            }
        };
    
        // widgets
        struct ImmediateUserInterfaceColorPickerGradientColorSelector : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceColorPickerGradientColorSelector(const std::string& _Hash) : ImmediateUserInterfaceNodePanel(_Hash){}
            virtual ~ImmediateUserInterfaceColorPickerGradientColorSelector(){}

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

                gs_color colors[7] =
                {
                    gs_rgba_color(255, 0, 0, 255),
                    gs_rgba_color(255, 255, 0, 255),
                    gs_rgba_color(0, 255, 0, 255),
                    gs_rgba_color(0, 255, 255, 255),
                    gs_rgba_color(0, 0, 255, 255),
                    gs_rgba_color(255, 0, 255, 255),
                    gs_rgba_color(255, 0, 0, 255)
                };

                // render vertical color selector
                gs_vec2f position = State.BoundingBox.Min;
                gs_vec2f size     = gs_vec2f(State.BoundingBox.width(), State.BoundingBox.height() / 6.f);

                gs_2dboxf slider = gs_2dboxf(
                    position + gs_vec2f(0.f, SliderPosition),
                    position + gs_vec2f(0.f, SliderPosition) + gs_vec2f(State.BoundingBox.width(), 16.f));

                bool caught = false;

                for (int i = 1; i < 7; i++)
                {
                    gs_color sourceColor = colors[i-1];
                    gs_color targetColor = colors[i-0];

                    _Context->m_Renderer->push_rectangle_gradient_mesh(
                        position,
                        position + size,
                        sourceColor,
                        sourceColor,
                        targetColor,
                        targetColor,
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                    // calculate color
                    if(gs_2dboxf(position, position + size).contains(slider.Min) && !caught)
                    {
                        int r1 = gs_rgba_color_get_r(sourceColor);
                        int g1 = gs_rgba_color_get_g(sourceColor);
                        int b1 = gs_rgba_color_get_b(sourceColor);

                        int r2 = gs_rgba_color_get_r(targetColor);
                        int g2 = gs_rgba_color_get_g(targetColor);
                        int b2 = gs_rgba_color_get_b(targetColor);

                        float fraction = (slider.Min.y - position.y) / size.y;

                        Color = gs_rgba_color(
                            (gs_color)(r1 + (r2 - r1) * fraction),
                            (gs_color)(g1 + (g2 - g1) * fraction),
                            (gs_color)(b1 + (b2 - b1) * fraction),
                            255);

                        caught = true;
                    }

                    position += gs_vec2f(0.f, size.y);
                }

                // render slider
                _Context->m_Renderer->push_rectangle_filled(
                    slider.Min,
                    slider.Max,
                    gs_rgba_color(255, 255, 255, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }

            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr) return false;

                if(_Context->m_Input.is_mouse_button_pressed())
                    SliderPreviousPosition = SliderPosition;

                if(!_Context->m_Input.is_mouse_button_down()) return false;

                gs_vec2f position = State.BoundingBox.Min;

                gs_2dboxf slider = gs_2dboxf(
                    position + gs_vec2f(0.f, SliderPosition),
                    position + gs_vec2f(0.f, SliderPosition) + gs_vec2f(State.BoundingBox.width(), 16.f));

                SliderPosition = gs_clamp(
                    SliderPreviousPosition + _Context->m_Input.get_cusor_drag_delta().y,
                    0.f,
                    State.BoundingBox.height() - slider.height());

                return true;
            }

            float    SliderPreviousPosition = 0.f;
            float    SliderPosition = 0.f;
            gs_color Color;
        };

        struct ImmediateUserInterfaceColorPickerGradientColorModifier : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceColorPickerGradientColorModifier(const std::string& _Hash) : ImmediateUserInterfaceNodePanel(_Hash){}
            virtual ~ImmediateUserInterfaceColorPickerGradientColorModifier(){}

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

                _Context->m_Renderer->push_rectangle_gradient_mesh(
                    State.BoundingBox.Min,
                    State.BoundingBox.Max,
                    gs_rgba_color(255, 255, 255, 255),
                    BaseColor, // this is current color
                    gs_rgba_color(0, 0, 0, 255),
                    gs_rgba_color(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }

            gs_color BaseColor;
            gs_color ModifiedColor;
        };

        // popups
        struct ImmediateUserInterfaceMenuScrollArea : public ImmediateUserInterfaceScrollArea
        {
        public:
            ImmediateUserInterfaceMenuScrollArea(const std::string& _Name) : ImmediateUserInterfaceScrollArea(_Name){}
            virtual ~ImmediateUserInterfaceMenuScrollArea(){}

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

                ImmediateUserInterfaceMenu* menu =
                    _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceMenu>(this);

                if(_Context->m_Hierarchy.get_parent(menu)) return;

                // outline
                _Context->m_Renderer->push_rectangle_rounded_filled(
                    State.BoundingBox.Min,
                    State.BoundingBox.Max,
                    _Context->m_Style.get_frames_radius(),
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuOutline),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }
        };

        struct ImmediateUserInterfaceMenuAction : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceMenuAction(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
            virtual ~ImmediateUserInterfaceMenuAction(){}

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr) return;

                gs_vec2f size =
                    _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() +
                    gs_vec2f(_Context->m_Style.get_font_size() * 2.f, _Context->m_Style.get_font_size() * 0.5f);

                State.MinimumSize = gs_vec2f(gs_min(size.x, State.MinimumSize.x), gs_min(size.y, State.MinimumSize.y));
                State.MaximumSize = gs_vec2f(gs_max(size.x, State.MaximumSize.x), gs_max(size.y, State.MaximumSize.y));
            }

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

                // background
                if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && _Context->m_Input.is_mouse_button_down())
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
                        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }
                else
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
                        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        (State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered) :
                                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }

                // title
                _Context->m_Renderer->push_text(
                    gs_vec2f(
                        State.BoundingBox.Min.x + ImmediateUserInterfaceContextLayerHelpers::calculate_offset(_Context), 
                        (State.BoundingBox.center() - _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() * 0.5f).y),
                    Name.begin(),
                    Name.end(),
                    _Context->m_Style.get_font_size(),
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    _Context->m_Renderer->calculate_transform_matrix(
                        (float)place_in_follow()));
            }
        };

        struct ImmediateUserInterfaceMenuItem : public ImmediateUserInterfaceMenuAction
        {
        public:
            ImmediateUserInterfaceMenuItem(const std::string& _Name) : ImmediateUserInterfaceMenuAction(_Name){}
            virtual ~ImmediateUserInterfaceMenuItem(){}

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
            {
                if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

                // background
                if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && _Context->m_Input.is_mouse_button_down())
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
                        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }
                else
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
                        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        (State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered) :
                                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }

                // title
                _Context->m_Renderer->push_text(
                    gs_vec2f(
                        State.BoundingBox.Min.x + ImmediateUserInterfaceContextLayerHelpers::calculate_offset(_Context), 
                        (State.BoundingBox.center() - _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() * 0.5f).y),
                    Name.begin(),
                    Name.end(),
                    _Context->m_Style.get_font_size(),
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // triangle
                float triangleWidth = _Context->m_Style.get_popup_menu_pointer_size();

                _Context->m_Renderer->push_triangle_filled(
                    gs_vec2f(0.f, 0.0),
                    gs_vec2f(0.f, triangleWidth),
                    gs_vec2f(triangleWidth * 0.5f, triangleWidth * 0.5f),
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    _Context->m_Renderer->calculate_transform_matrix(
                        (float)place_in_follow(),
                        gs_vec2f(
                            State.BoundingBox.Max.x - triangleWidth,
                            State.BoundingBox.center().y - triangleWidth * 0.5f)));
            }
        };
    }
}

// ImmedidateUserInterfaceStyle
ImmedidateUserInterfaceStyle::ImmedidateUserInterfaceStyle()
{
    Colors.resize(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End);

    // general ui elements
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground]                 = gs_rgba_color(28, 28, 28, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered]          = gs_rgba_color(72, 82, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground]                  = gs_rgba_color(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackgroundHovered]           = gs_rgba_color(72, 82, 72, 255);
    
    // push button
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline]                    = gs_rgba_color(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground]                 = gs_rgba_color(32, 32, 32, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered]          = gs_rgba_color(60, 72, 60, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed]          = gs_rgba_color(120, 128, 120, 255);

    // menus
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuOutline]                      = gs_rgba_color(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground]             = gs_rgba_color(32, 32, 32, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered]      = gs_rgba_color(60, 72, 60, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed]      = gs_rgba_color(120, 128, 120, 255);

    // scrollbar
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground]        = gs_rgba_color(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered] = gs_rgba_color(72, 82, 72, 255);

    // gizmos
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos]                           = gs_rgba_color(50, 50, 100, 200);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered]                    = gs_rgba_color(100, 100, 172, 255);

    // gizmos
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text]                    = gs_rgba_color(255, 255, 255, 255);
}

ImmedidateUserInterfaceStyle::~ImmedidateUserInterfaceStyle(){}

float ImmedidateUserInterfaceStyle::get_minimum_frames_radius() const
{
    return 0.f;
}

float ImmedidateUserInterfaceStyle::get_maximum_frames_radius() const
{
    return 32.f;
}

float ImmedidateUserInterfaceStyle::get_frames_radius() const
{
    return gs_clamp(FramesRadius, get_minimum_frames_radius(), get_maximum_frames_radius());
}

float ImmedidateUserInterfaceStyle::get_minimum_frames_width() const
{
    return 4.f;
}

float ImmedidateUserInterfaceStyle::get_maximum_frames_width() const
{
    return 16.f;
}

float ImmedidateUserInterfaceStyle::get_frames_width() const
{
    return gs_clamp(FramesWidth, get_minimum_frames_width(), get_maximum_frames_width());
}

float ImmedidateUserInterfaceStyle::get_minimum_font_size() const
{
    return 32.f;
}

float ImmedidateUserInterfaceStyle::get_maximum_font_size() const
{
    return 128.f;
}

float ImmedidateUserInterfaceStyle::get_font_size() const
{
    return gs_clamp(FontSize, get_minimum_font_size(), get_maximum_font_size());
}

float ImmedidateUserInterfaceStyle::get_minimum_scrollbar_width() const
{
    return 16.f;
}

float ImmedidateUserInterfaceStyle::get_maximum_scrollbar_width() const
{
    return get_maximum_frames_radius() * 2.f;
}

float ImmedidateUserInterfaceStyle::get_scrollbar_width() const
{
    return gs_clamp(ScrollBarWidth, get_minimum_scrollbar_width(), get_maximum_scrollbar_width());
}

float ImmedidateUserInterfaceStyle::get_popup_menu_pointer_size() const
{
    return gs_min(gs_max(PopupMenuPointerSize, 32.f), get_font_size() - 2.f * get_frames_width());
}

ApplicationRenderingBackendFont ImmedidateUserInterfaceStyle::get_current_font() const
{
    return Font.is_null() ? ApplicationRenderingBackend::get_default_font() : Font;
}

gs_color ImmedidateUserInterfaceStyle::get_color(const ImmediateUserInterfaceNodeColors_& _Color) const
{
    return Colors[_Color];
}

// ImmedidateUserInterfaceInput
ImmedidateUserInterfaceInput::ImmedidateUserInterfaceInput(ImmediateUserInterfaceContextLayer* _Context) : m_Context(_Context){}

gs_vec2f ImmedidateUserInterfaceInput::get_cusor_position() const
{
    return m_Context != nullptr && m_Context->m_Renderer != nullptr ?
            gs_vec2f(m_Context->m_Renderer->get_cursor_postion().x, m_Context->m_Renderer->get_cursor_postion().y) :
                gs_vec2f(0.f, 0.f);
}

gs_vec2f ImmedidateUserInterfaceInput::get_cusor_drag_delta() const
{
    return ApplicationPlatformBackend::get_window_cursor_dragdelta();
}

gs_vec2f ImmedidateUserInterfaceInput::get_cusor_scroll_offset() const
{
    return ApplicationPlatformBackend::get_mouse_scroll_offset();
}

std::string ImmedidateUserInterfaceInput::get_input_text() const
{
    return ApplicationPlatformBackend::input_text();
}

std::string ImmedidateUserInterfaceInput::get_clipboard_text() const
{
    return ApplicationPlatformBackend::get_clipboard_text();
}

bool ImmedidateUserInterfaceInput::has_input_text() const
{
    return ApplicationPlatformBackend::has_input_text();
}

bool ImmedidateUserInterfaceInput::has_clipboard_text() const
{
    return ApplicationPlatformBackend::has_clipboard_text();
}

void ImmedidateUserInterfaceInput::set_clipboard_text(const std::string& _Value)
{
    ApplicationPlatformBackend::set_clipboard_text(_Value);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_down() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_down((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_hold() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_hold((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }
    
    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_pressed() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_pressed((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_released() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_released((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_clicked() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_clicked((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_double_clicked() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_double_clicked((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_down(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_hold(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_pressed(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_released(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_clicked(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_double_clicked(_Button);
}

bool ImmedidateUserInterfaceInput::is_key_down() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_down((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_hold() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_hold((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_pressed() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_pressed((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_released() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_released((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_clicked() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_clicked((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_down(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_down(_Key);
}

bool ImmedidateUserInterfaceInput::is_key_hold(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_hold(_Key);
}

bool ImmedidateUserInterfaceInput::is_key_pressed(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_pressed(_Key);
}

bool ImmedidateUserInterfaceInput::is_key_released(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_released(_Key);
}

bool ImmedidateUserInterfaceInput::is_key_clicked(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_clicked(_Key);
}

bool ImmedidateUserInterfaceInput::has_modifier(const ApplicationPlatformBackendKeyModifier::Modifier& _Modifier) const
{
    return ApplicationPlatformBackend::has_modifier(_Modifier);
}

// ImmediateUserInterfaceContextConfiguration
ImmediateUserInterfaceContextConfiguration::ImmediateUserInterfaceContextConfiguration(){}
ImmediateUserInterfaceContextConfiguration::~ImmediateUserInterfaceContextConfiguration(){}

template<> std::string ImmediateUserInterfaceContextConfiguration::get<std::string>(const std::string& _Section, const std::string& _Name)
{
    return m_Configuration[_Section][_Name];
}

template<> int ImmediateUserInterfaceContextConfiguration::get<int>(const std::string& _Section, const std::string& _Name)
{
    return Frenchie::Core::String::from_string<int>(m_Configuration[_Section][_Name]);
}

template<> float ImmediateUserInterfaceContextConfiguration::get<float>(const std::string& _Section, const std::string& _Name)
{
    return Frenchie::Core::String::from_string<float>(m_Configuration[_Section][_Name]);
}

template<> double ImmediateUserInterfaceContextConfiguration::get<double>(const std::string& _Section, const std::string& _Name)
{
    return Frenchie::Core::String::from_string<double>(m_Configuration[_Section][_Name]);
}

template<> bool ImmediateUserInterfaceContextConfiguration::get<bool>(const std::string& _Section, const std::string& _Name)
{
    return Frenchie::Core::String::from_string<bool>(m_Configuration[_Section][_Name]);
}

template<> gs_vec2f ImmediateUserInterfaceContextConfiguration::get<gs_vec2f>(const std::string& _Section, const std::string& _Name)
{
    auto& _File = m_Configuration[_Section][_Name];

    if(_File.empty())
        return gs_vec2f(0.f, 0.f);

    gs_vec2f vector;

    char* begin = &_File[0];
    char* end   = begin;

    for (; *end != ',' && *end != '\0'; end++);
    vector.x = Frenchie::Core::String::from_string<float>(std::string(begin, end));

    if(*end == '\n')
        return vector;

    begin = ++end;
    for (;*end != '\0'; end++);

    vector.y = Frenchie::Core::String::from_string<float>(std::string(begin, end));

    return vector;
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<std::string>(const std::string& _Section, const std::string& _Name, const std::string& _Value)
{
    m_Configuration[_Section][_Name] = _Value;
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<int>(const std::string& _Section, const std::string& _Name, const int& _Value)
{
    m_Configuration[_Section][_Name] = Frenchie::Core::String::to_string<int>(_Value);
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<float>(const std::string& _Section, const std::string& _Name, const float& _Value)
{
    m_Configuration[_Section][_Name] = Frenchie::Core::String::to_string<float>(_Value);
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<double>(const std::string& _Section, const std::string& _Name, const double& _Value)
{
    m_Configuration[_Section][_Name] = Frenchie::Core::String::to_string<double>(_Value);
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<bool>(const std::string& _Section, const std::string& _Name, const bool& _Value)
{
    m_Configuration[_Section][_Name] = Frenchie::Core::String::to_string<bool>(_Value);
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<gs_vec2f>(const std::string& _Section, const std::string& _Name, const gs_vec2f& _Value)
{
    m_Configuration[_Section][_Name] =
        std::string(Frenchie::Core::String::to_string<double>(_Value.x))
        .append(",")
        .append(Frenchie::Core::String::to_string<double>(_Value.y));
}

bool ImmediateUserInterfaceContextConfiguration::contains(const std::string& _Section, const std::string& _Name) const
{
    auto iterator = m_Configuration.find(_Section);
    return iterator != m_Configuration.end() && iterator->second.find(_Name) != iterator->second.end();
}

bool ImmediateUserInterfaceContextConfiguration::empty() const
{
    return m_Configuration.empty();
}

void ImmediateUserInterfaceContextConfiguration::clear()
{
    m_Configuration.clear();
}

bool ImmediateUserInterfaceContextConfiguration::read(const std::u32string& _Path)
{
    // open file
    FILE* file = std::fopen(
        Frenchie::Core::String::convert_utf32_to_utf8(_Path).c_str(),
        Frenchie::Core::String::convert_utf32_to_utf8(U"rb").c_str());
    
    if(file == nullptr)
        return false;

    // determine file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file); // Go back to the beginning

    if (file_size == -1)
    {
        fclose(file);
        return false;
    }

    // allocate memory for the content (+1 for null terminator)
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer)
    {
        fclose(file);
        return false;
    }

    // Read the entire file into string buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size)
    {
        free(buffer);
        fclose(file);
        return false;
    }

    // add the null terminator to make it a valid C string
    buffer[file_size] = '\0';

    // read file contents

    // auxiliary lambda predicates
    auto isEndOfFile    = [](const char* _Contents)->bool{return *_Contents ==  '\0';};
    auto isSectionStart = [](const char* _Contents)->bool{return *_Contents ==  '['; };
    auto isSectionEnd   = [](const char* _Contents)->bool{return *_Contents ==  ']'; };
    auto isValueStart   = [](const char* _Contents)->bool{return *_Contents ==  '='; };
    auto isKeyStart     = [](const char* _Contents)->bool{return *_Contents == '\n'; };

    char* fileContents = buffer;

    std::string currentSection;
    std::string currentSectionKey;

    for (;!isEndOfFile(fileContents); fileContents++)
    {
        // read section name
        if(isSectionStart(fileContents) && !isEndOfFile(++fileContents))
        {
            char* sectionNameBegin = fileContents;
            char* sectionNameEnd   = fileContents;

            for (;!isEndOfFile(sectionNameEnd); sectionNameEnd++)
            {
                if (isSectionEnd(sectionNameEnd))
                {
                    for (;isSectionEnd(sectionNameEnd) && !isEndOfFile(sectionNameEnd); sectionNameEnd++);
                    break;
                }
            }

            currentSection = std::string(sectionNameBegin, (sectionNameBegin != sectionNameEnd - 1 ? sectionNameEnd - 1 : sectionNameEnd));
            fileContents   = --sectionNameEnd;
        }

        // read section contents
        if(isSectionEnd(fileContents) && !currentSection.empty())
        {
            char* sectionContentsBegin = fileContents;
            char* sectionContentsEnd   = sectionContentsBegin;
            for (;!isKeyStart(sectionContentsBegin) && !isEndOfFile(sectionContentsBegin); sectionContentsBegin++);
            for (;!isSectionStart(sectionContentsEnd) && !isEndOfFile(sectionContentsEnd); sectionContentsEnd++);

            for(auto it = sectionContentsBegin; it != sectionContentsEnd && !isEndOfFile(it); it++)
            {
                if(isKeyStart(it))
                {
                    char* keyBegin = ++it;
                    char* keyEnd   = keyBegin;
                    for (;!isValueStart(keyEnd) && keyEnd != sectionContentsEnd && !isEndOfFile(keyEnd); keyEnd++);
                    currentSectionKey = std::string(keyBegin, keyEnd);
                    it = --keyEnd;
                }

                if(isValueStart(it) && !currentSectionKey.empty())
                {
                    char* valueBegin = ++it;
                    char* valueEnd   = valueBegin;
                    for (;!isKeyStart(valueEnd) && valueEnd != sectionContentsEnd && !isEndOfFile(valueEnd); valueEnd++);
                    m_Configuration[currentSection][currentSectionKey] = std::string(valueBegin, valueEnd);
                    it = --valueEnd;
                }
            }

            fileContents = --sectionContentsEnd;
        }
    }
    
    // close the file
    fclose(file);

    // free file contents buffer
    free(buffer);

    return true;
}

bool ImmediateUserInterfaceContextConfiguration::write(const std::u32string& _Path)
{
    // open file
    FILE* file = std::fopen(
        Frenchie::Core::String::convert_utf32_to_utf8(_Path).c_str(),
        Frenchie::Core::String::convert_utf32_to_utf8(U"wb").c_str());

    if(file == nullptr)
        return false;

    for(auto& settings : m_Configuration)
    {
        fprintf(file, "[%s]\n", settings.first.c_str());

        for(auto& setting : settings.second)
        {
            if(!setting.first.empty() && !setting.second.empty())
                fprintf(file, "%s=%s\n", setting.first.c_str(), setting.second.c_str());
        }
    }

    fclose(file);

    return true;
}

// ImmedidateUserInterfaceNode
ImmediateUserInterfaceNode::ImmediateUserInterfaceNode(const std::string& _Hash) : Hash(_Hash){}
ImmediateUserInterfaceNode::~ImmediateUserInterfaceNode(){}

void ImmediateUserInterfaceNode::render(ImmediateUserInterfaceContextLayer*)
{
}

void ImmediateUserInterfaceNode::layout(ImmediateUserInterfaceContextLayer*){}

void ImmediateUserInterfaceNode::measure(ImmediateUserInterfaceContextLayer* _Context)
{
    gs_2dboxf box = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min);

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

bool ImmediateUserInterfaceNode::events(ImmediateUserInterfaceContextLayer* _Context)
{
    // resize
    if(((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable)) &&
        !(State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
    {
        if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);
                    return true;
                }
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
        ImmediateUserInterfaceNode* parent  = _Context->m_Hierarchy.get_parent(movable);

        while (parent)
        {
            if(parent->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable)
                movable = parent;
            else
                break;
            parent = _Context->m_Hierarchy.get_parent(parent);
        }

        if(_Context->m_Input.is_mouse_button_pressed())
        {
            movable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;
            return true;
        }

        if(_Context->m_Input.is_mouse_button_down() &&
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
        {            
            movable->State.BoundingBox = gs_2dboxf(
                movable->Cache.BoundingBox.Min + ApplicationPlatformBackend::get_window_cursor_dragdelta(),
                movable->Cache.BoundingBox.Max + ApplicationPlatformBackend::get_window_cursor_dragdelta());
            return true;
        }
    }

    return false;
}

void ImmediateUserInterfaceNode::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child != nullptr)
        _Child->State.Parent = this;
}

bool ImmediateUserInterfaceNode::create_contents(ImmediateUserInterfaceContextLayer*, const std::string&, const ImmediateUserInterfaceNodeSettings&, bool*)
{
    return true;
}

void ImmediateUserInterfaceNode::load_state(ImmediateUserInterfaceContextLayer*){}
void ImmediateUserInterfaceNode::save_state(ImmediateUserInterfaceContextLayer*){}

gs_2dboxf ImmediateUserInterfaceNode::get_clipping_box(ImmediateUserInterfaceContextLayer* _Context) const
{
    // auxiliary lambdas
    auto calculate_clipping_box = [](ImmediateUserInterfaceContextLayer* _Context, const ImmediateUserInterfaceNode* _Node)->gs_2dboxf
    {
        if(_Node == nullptr)
        {
            return _Context != nullptr ?
                        _Context->m_Renderer->current_viewport() :
                            gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f((float)INT_MAX, (float)INT_MAX));
        }

        const ImmediateUserInterfaceNode* next   = _Node;
        ImmediateUserInterfaceNode*       parent = _Context->m_Hierarchy.get_parent(_Node);

        gs_2dboxf clippingBox = next->State.BoundingBox;

        while (parent)
        {
            clippingBox = gs_2dboxf(
                gs_vec2f(gs_max(next->State.BoundingBox.Min.x, clippingBox.Min.x), gs_max(next->State.BoundingBox.Min.y, clippingBox.Min.y)),
                gs_vec2f(gs_min(next->State.BoundingBox.Max.x, clippingBox.Max.x), gs_min(next->State.BoundingBox.Max.y, clippingBox.Max.y)));

            next   = parent;
            parent = _Context->m_Hierarchy.get_parent(parent);
        }

        return clippingBox;
    };

    // main code
    if(!State.ClippingBox.has_value())
        State.ClippingBox = calculate_clipping_box(_Context, this);

    return State.ClippingBox.value();
}

gs_2dboxf ImmediateUserInterfaceNode::get_visible_rect(ImmediateUserInterfaceContextLayer* _Context) const
{
    return State.BoundingBox.clip_with(get_clipping_box(_Context));
}

bool ImmediateUserInterfaceNode::is_partially_visible(ImmediateUserInterfaceContextLayer* _Context) const
{
    return State.BoundingBox.overlaps(get_clipping_box(_Context));
}

int ImmediateUserInterfaceNode::place_in_follow()
{
    return State.Depth + (++State.SelfThickness);
}

// ImmediateUserInterfaceNodePanel
ImmediateUserInterfaceNodePanel::ImmediateUserInterfaceNodePanel(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceNodePanel::~ImmediateUserInterfaceNodePanel(){}

void ImmediateUserInterfaceNodePanel::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min + ContentPadding,
        State.BoundingBox.size() - ContentPadding * 2.f,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

void ImmediateUserInterfaceNodePanel::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // outline
    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
}

bool ImmediateUserInterfaceNodePanel::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return false;

    if(_Context->m_Hierarchy.size(this) > 0)
        return ImmediateUserInterfaceNode::events(_Context);

    ImmediateUserInterfaceNode* parent = _Context->m_Hierarchy.get_parent(this);
    while (parent)
    {
        if(parent->events(_Context))
            return true;
        parent = _Context->m_Hierarchy.get_parent(parent);
    }
    
    return false;
}

// ImmediateUserInterfaceNodeVerticalStack
ImmediateUserInterfaceNodeVerticalStack::ImmediateUserInterfaceNodeVerticalStack(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceNodeVerticalStack::~ImmediateUserInterfaceNodeVerticalStack(){}

void ImmediateUserInterfaceNodeVerticalStack::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_vertical_stack(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min,
        State.BoundingBox.size(),
        ContentPadding,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

// ImmediateUserInterfaceNodeHorizontalStack
ImmediateUserInterfaceNodeHorizontalStack::ImmediateUserInterfaceNodeHorizontalStack(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceNodeHorizontalStack::~ImmediateUserInterfaceNodeHorizontalStack(){}

void ImmediateUserInterfaceNodeHorizontalStack::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_horizontal_stack(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min,
        State.BoundingBox.size(),
        ContentPadding,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

// ImmediateUserInterfaceScrollArea
ImmediateUserInterfaceScrollArea::ImmediateUserInterfaceScrollArea(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceScrollArea::~ImmediateUserInterfaceScrollArea(){}

void ImmediateUserInterfaceScrollArea::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    gs_vec2f padding = gs_vec2f(0.f, 0.f);
    calculate_content_padding(_Context, this, padding);

    // resize to contents
    State.MinimumSize = gs_vec2f(
        (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally) ?
            ContentView->State.ContentSize.x + padding.x * 2.f :
                State.MinimumSize.x,
        (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically) ?
            ContentView->State.ContentSize.y + padding.y :
                State.MinimumSize.y);

    // TODO: if something goes wrong --> uncomment this code
    // State.MinimumSize = gs_vec2f(
    //     (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally) ?
    //         (ContentView->State.ContentSize + get_vertical_scrollbar_width(_Context)).x + padding.x * 2.f :
    //             State.MinimumSize.x,
    //     (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically) ?
    //         (ContentView->State.ContentSize + get_horizontal_scrollbar_width(_Context)).y + padding.y :
    //             State.MinimumSize.y);
    
    State.MaximumSize = gs_vec2f(
        (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally) ? State.MinimumSize.x : State.MaximumSize.x,
        (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically) ? State.MinimumSize.y : State.MaximumSize.y);
    
    State.BoundingBox = gs_2dboxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + gs_clamp(State.BoundingBox.size(), State.MinimumSize, State.MaximumSize));

    // default layout
    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min + gs_vec2f(_Context->m_Style.get_frames_width()),
        State.BoundingBox.size() - gs_vec2f(_Context->m_Style.get_frames_width()) * 2.f,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

void ImmediateUserInterfaceScrollArea::render(ImmediateUserInterfaceContextLayer*){}

void ImmediateUserInterfaceScrollArea::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child == nullptr) return;

    if(dynamic_cast<ImmediateUserInterfaceScrollAreaPanel*>(_Child))
    {
        _Child->State.Parent = this;
        return;
    }

    if(ContentView)
        ContentView->attach_child(_Child);
}

bool ImmediateUserInterfaceScrollArea::create_contents(
    ImmediateUserInterfaceContextLayer*       _Context, 
    const std::string&                        _ID,
    const ImmediateUserInterfaceNodeSettings& _Settings,
    bool*                                     _Render)
{
    if(_Context == nullptr) return false;

    ImmediateUserInterfaceNodeSettings settings   = _Settings & ~ImmediateUserInterfaceNodeSettings_NullParent;
    ImmediateUserInterfaceScrollArea*  scrollArea = this;

    if(_Context->begin_node<ImmediateUserInterfaceScrollAreaPanel>(
        std::string(_ID).append("/Panel"),
        settings))
    {
        if(_Context->begin_vertial_stack(
            std::string(_ID).append("/Panel/VerticalStack"),
            settings))
        {
            _Context->get_rendering_stack_top<ImmediateUserInterfaceNode>()->State.PlaceInFollow = true;

            // vertical scrollbar area
            if(_Context->begin_horizontal_stack(
                std::string(_ID).append("/Panel/VerticalStack/VerticalScrollBarArea"),
                settings))
            {
                _Context->get_rendering_stack_top<ImmediateUserInterfaceNodeHorizontalStack>()->State.PlaceInFollow = true;

                // contents
                if(_Context->begin_node<ImmediateUserInterfaceScrollAreaContent>(
                    std::string(_ID).append("/Panel/VerticalStack/VerticalScrollBarArea/Contents"),
                    settings))
                {
                    scrollArea->ContentView = _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaContent>();
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaContent>()->ContentPadding      = scrollArea->ContentPadding;
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaContent>()->State.PlaceInFollow = scrollArea->State.PlaceInFollow;
                    
                    // reset self content padding as it's applied to content view
                    scrollArea->ContentPadding = gs_vec2f(0.f, 0.f);
                    
                    _Context->end_node<ImmediateUserInterfaceScrollAreaContent>();
                }

                // vertical scrollbar
                if(_Context->begin_node<ImmediateUserInterfaceScrollAreaScrollBar>(
                    std::string(_ID).append("/Panel/VerticalStack/VerticalScrollBarArea/VerticalScrollBar"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaScrollBar>()->Type =
                        ImmediateUserInterfaceScrollAreaScrollBar::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical;

                    scrollArea->VerticalScrollBar = _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaScrollBar>();

                    _Context->end_node<ImmediateUserInterfaceScrollAreaScrollBar>();
                }

                _Context->end_horizontal_stack();
            }

            // horizontal scrollbar
            if(_Context->begin_node<ImmediateUserInterfaceScrollAreaScrollBar>(
                std::string(_ID).append("/Panel/VerticalStack/HorizontalScrollBar"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaScrollBar>()->Type =
                    ImmediateUserInterfaceScrollAreaScrollBar::ImmediateUserInterfaceScrollAreaScrollBarType_Horizontal;

                scrollArea->HorizontalScrollBar = _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaScrollBar>();

                _Context->end_node<ImmediateUserInterfaceScrollAreaScrollBar>();
            }

            _Context->end_vertical_stack();
        }
    
        _Context->end_node<ImmediateUserInterfaceScrollAreaPanel>();
    }

    return true;
}

float ImmediateUserInterfaceScrollArea::get_horizontal_scrollbar_width(ImmediateUserInterfaceContextLayer* _Context) const
{
    float horizontalScrollBarSize =
        HorizontalScrollBar != nullptr ?
            gs_min(HorizontalScrollBar->State.BoundingBox.size().x, HorizontalScrollBar->State.BoundingBox.size().y) :
                0.f;

    return horizontalScrollBarSize;
}

float ImmediateUserInterfaceScrollArea::get_vertical_scrollbar_width(ImmediateUserInterfaceContextLayer* _Context) const
{
    float verticalScrollBarSize =
        VerticalScrollBar != nullptr ?
            gs_min(VerticalScrollBar->State.BoundingBox.size().x, VerticalScrollBar->State.BoundingBox.size().y) :
                0.f;
    
    return verticalScrollBarSize;
}

void ImmediateUserInterfaceScrollArea::calculate_content_padding(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, gs_vec2f& _Padding)
{
    if(_Context == nullptr || _Node == nullptr) return;

    if(dynamic_cast<ImmediateUserInterfaceNodePanel*>(_Node) != nullptr)
        _Padding += dynamic_cast<ImmediateUserInterfaceNodePanel*>(_Node)->ContentPadding;

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
        calculate_content_padding(_Context, *it, _Padding);
}

// ImmediateUserInterfaceMenu
ImmediateUserInterfaceMenu::ImmediateUserInterfaceMenu(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceMenu::~ImmediateUserInterfaceMenu(){}

void ImmediateUserInterfaceMenu::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(InternalScrollArea != nullptr)
    {
        State.BoundingBox = gs_2dboxf(
            State.BoundingBox.Min,
            State.BoundingBox.Min + InternalScrollArea->State.BoundingBox.size());
    }

    ImmediateUserInterfaceNodePanel::layout(_Context);
}

void ImmediateUserInterfaceMenu::render(ImmediateUserInterfaceContextLayer*){}

void ImmediateUserInterfaceMenu::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child == nullptr) return;

    if(dynamic_cast<ImmediateUserInterfaceMenuScrollArea*>(_Child))
    {
        _Child->State.Parent = this;
        return;
    }

    if(dynamic_cast<ImmediateUserInterfaceMenuAction*>(_Child) || dynamic_cast<ImmediateUserInterfaceMenu*>(_Child))
        _Child->State.NextLine = 1;
    
    if(State.Parent)
    {
        if(ExternalScrollArea)
            ExternalScrollArea->attach_child(_Child);
    }
    else 
        InternalScrollArea->attach_child(_Child);
}

// ImmediateUserInterfaceWindow
ImmediateUserInterfaceWindow::ImmediateUserInterfaceWindow(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceWindow::~ImmediateUserInterfaceWindow(){}

void ImmediateUserInterfaceWindow::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // auxiliary lambdas
    auto close_button_color = [](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& closeButtonBox)
    {
        if(_Context->m_Input.is_mouse_button_down() && closeButtonBox.contains(_Context->m_Input.get_cusor_position()))
            return gs_rgba_color(255, 0, 0, 255);

        return closeButtonBox.contains(_Context->m_Input.get_cusor_position()) ?
            gs_rgba_color(128, 0, 0, 255) : // TODO: this MUST BE a setting
            gs_rgba_color(64, 0, 0, 255);
    };

    auto render_close_button = [this, &close_button_color](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _Box)
    {
        gs_2dboxf closeButtonBox = gs_2dboxf(
            _Box.Min + _Context->m_Style.get_frames_width() * 2.f,
            _Box.Max - _Context->m_Style.get_frames_width() * 2.f);

        _Context->m_Renderer->push_arc_filled(
            closeButtonBox.center(),
            closeButtonBox.size().x,
            closeButtonBox.size().y,
            0.f,
            360.f,
            close_button_color(_Context, closeButtonBox),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

        _Context->m_Renderer->push_line(
            closeButtonBox.Min + gs_vec2f(+4.f, +4.f),
            closeButtonBox.Max - gs_vec2f(+4.f, +4.f),
            4.f,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

        _Context->m_Renderer->push_line(
            gs_vec2f(closeButtonBox.Max.x, closeButtonBox.Min.y) + gs_vec2f(-4.f, +4.f),
            gs_vec2f(closeButtonBox.Min.x, closeButtonBox.Max.y) + gs_vec2f(+4.f, -4.f),
            4.f,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    };

    // content background and outline frame
    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    _Context->m_Renderer->push_rectangle_rounded_filled(
        FrameBox.Min + _Context->m_Style.get_frames_width(),
        FrameBox.Max - _Context->m_Style.get_frames_width(),
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    // frame
    ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().process_window_frame(
        _Context,
        this,

        // render self and child frames
        [this, &render_close_button](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _FrameBox, const gs_2dboxf& _Frame, ImmediateUserInterfaceWindow* _Window)
        {
            ImmedidateUserInterfaceWindowController* windowsController =
                _Context->get_controller<ImmedidateUserInterfaceWindowController>();

            // detect if we are active
            bool active   = _Window->IsActive;

            bool isDocker = windowsController != nullptr && !windowsController->retrieve_docked_windows(
                _Context,
                _Window,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center).empty();

            // render frame
            if(_Window->Docker == nullptr && !isDocker)
            {
                // frame
                if(_Frame.contains(_Context->m_Input.get_cusor_position()) &&
                    (_Window->TopSnapper    != nullptr ||
                     _Window->LeftSnapper   != nullptr ||
                     _Window->RightSnapper  != nullptr ||
                     _Window->BottomSnapper != nullptr))
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Frame.Min + _Context->m_Style.get_frames_width(),
                        _Frame.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }

                // close button
                if(_Window->Opened != nullptr)
                {
                    gs_2dboxf closeButtonBox  = gs_2dboxf(
                        gs_vec2f(_FrameBox.Max.x - _FrameBox.height() / 2.f, _Frame.center().y) - _FrameBox.height() / 4.f,
                        gs_vec2f(_FrameBox.Max.x - _FrameBox.height() / 2.f, _Frame.center().y) + _FrameBox.height() / 4.f);

                    render_close_button(_Context, closeButtonBox);

                    *_Window->Opened = !(closeButtonBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_clicked());
                }
            }
            else
            {
                // frame
                if(active)
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Frame.Min + _Context->m_Style.get_frames_width(),
                        _Frame.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                        true,
                        true,
                        false,
                        false);
                }
                else if(_Frame.contains(_Context->m_Input.get_cusor_position()))
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Frame.Min + _Context->m_Style.get_frames_width(),
                        _Frame.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                        true,
                        true,
                        false,
                        isDocker);
                }

                // close button
                if(_Window->Opened != nullptr)
                {
                    if(active || _Frame.contains(_Context->m_Input.get_cusor_position()))
                    {
                        gs_2dboxf closeButtonBox  = gs_2dboxf(
                            gs_vec2f(_Frame.Max.x - _Frame.height() / 2.f, _Frame.center().y) - _Frame.height() / 4.f,
                            gs_vec2f(_Frame.Max.x - _Frame.height() / 2.f, _Frame.center().y) + _Frame.height() / 4.f);

                        render_close_button(_Context, closeButtonBox);

                        *_Window->Opened = !(closeButtonBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_clicked());
                    }
                }
            }

            // title
            _Context->m_Renderer->push_text(
                gs_vec2f(
                    _Frame.Min.x + 16.f + _Context->m_Style.get_frames_width(),
                    _Frame.center().y - _Context->m_Renderer->calculate_bounding_box(_Window->Name.begin(), _Window->Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).height() * 0.6f * 0.5f),
                _Window->Name.begin(),
                _Window->Name.end(),
                _Context->m_Style.get_font_size() * 0.6f,
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
        }
    );
}

void ImmediateUserInterfaceWindow::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    // compute self geometry
    FrameBox = gs_2dboxf(
        State.BoundingBox.Min,
        gs_vec2f(
            State.BoundingBox.Max.x,
            State.BoundingBox.Min.y + gs_max(_Context->m_Style.get_font_size(), 64.f)));

    ContentBox = gs_2dboxf(
        (Docker == nullptr ? gs_vec2f(FrameBox.Min.x, FrameBox.Max.y) : State.BoundingBox.Min),
        State.BoundingBox.Max);

    ContentBox = gs_2dboxf(
        ContentBox.Min + _Context->m_Style.get_frames_width(),
        ContentBox.Max - _Context->m_Style.get_frames_width());

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        ContentBox.Min,
        ContentBox.size(),
        State.Settings,
        [this](const ImmediateUserInterfaceNode* _Node){return true;});
}

bool ImmediateUserInterfaceWindow::events(ImmediateUserInterfaceContextLayer* _Context)
{
    bool defaultPipeline = true;

    // window frame events
    ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().process_window_frame(
        _Context,
        this,

        // catch frame events
        [this, &defaultPipeline](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _FrameBox, const gs_2dboxf& _Frame, ImmediateUserInterfaceWindow* _Window)
        {
            // focus and activity
            if(_Context->m_Input.is_mouse_button_pressed() && _Frame.contains(_Context->m_Input.get_cusor_position()))
                _Window->Activate = true;

            // move
            if((_Window->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable) &&
                !((_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)           ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)        ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)       ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)      ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)  ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)     ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)))
            {
                ImmediateUserInterfaceNode* movable = _Window;

                if(_Context->m_Input.is_mouse_button_pressed() &&
                    gs_2dboxf(
                        _Frame.Min + gs_vec2f(ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Context, this).Radius, 0.f),
                        _Frame.Max - gs_vec2f(ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Context, this).Radius, 0.f)).contains(_Context->m_Input.get_cusor_position()))
                {
                    movable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;
                    defaultPipeline = false;
                    return;
                }

                if(_Context->m_Input.is_mouse_button_down() &&
                    _Frame.contains(_Context->m_Input.get_cusor_position())   &&
                    gs_vector_length(_Context->m_Input.get_cusor_drag_delta()) > 8.f &&
                    (movable->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
                {
                    movable->State.BoundingBox = gs_2dboxf(
                        movable->Cache.BoundingBox.Min + _Context->m_Input.get_cusor_drag_delta(),
                        movable->Cache.BoundingBox.Max + _Context->m_Input.get_cusor_drag_delta());

                    if(dynamic_cast<ImmediateUserInterfaceWindow*>(movable))
                        dynamic_cast<ImmediateUserInterfaceWindow*>(movable)->ReattachChildren = true;

                    defaultPipeline = false;
                    return;
                }
            }
        });

    // default event pipeline
    if(defaultPipeline)
        return ImmediateUserInterfaceNode::events(_Context);
    
    return !defaultPipeline;
}

void ImmediateUserInterfaceWindow::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child == nullptr)
        return;

    if( dynamic_cast<ImmediateUserInterfaceWindowHorizontalSnapper*>(_Child) ||
        dynamic_cast<ImmediateUserInterfaceWindowVerticalSnapper*>(_Child)   ||
        dynamic_cast<ImmediateUserInterfaceWindowCentralDocker*>(_Child))
    {
        _Child->State.Parent = this;
    }
    else
    {
        _Child->State.Parent = ContentView;
    }
}

bool ImmediateUserInterfaceWindow::create_contents(ImmediateUserInterfaceContextLayer* _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render)
{
    if(_Context == nullptr) return false;

    ImmediateUserInterfaceNodeSettings settings = _Settings & ~ImmediateUserInterfaceNodeSettings_NullParent;

    ImmediateUserInterfaceWindow* window = this;
    window->Opened                       = _Render;

    if(_Context->begin_node<ImmediateUserInterfaceWindowCentralDocker>(
        std::string(_ID).append("/CentralDockerView"),
        settings))
    {
        if(!window->IsActive)
        {
            _Context->end_node<ImmediateUserInterfaceWindowCentralDocker>();
            _Context->end_node<ImmediateUserInterfaceWindow>();
            return false;
        }

        window->DockerView                                    = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowCentralDocker>();
        window->DockerView->State.PlaceInFollow               = true;
        window->DockerView->State.OrderChildrenWhileRendering = true;

        if(_Context->begin_node<ImmediateUserInterfaceWindowVerticalSnapper>(
            std::string(_ID).append("/SnapperView"),
            settings))
        {
            window->SnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowVerticalSnapper>();

            // top
            if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                std::string(_ID).append("/SnapperView/TopSnapperView"),
                settings))
            {
                window->TopSnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalSnapper>();
                _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
            }

            // center
            if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                std::string(_ID).append("/SnapperView/CentralSnapperView"),
                settings))
            {
                if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                    std::string(_ID).append("/SnapperView/CentralSnapperView/LeftSnapperView"),
                    settings))
                {
                    window->LeftSnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalSnapper>();
                    _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
                }

                if(_Context->begin_node<ImmediateUserInterfaceWindowVerticalSnapper>(
                    std::string(_ID).append("/SnapperView/CentralSnapperView/ContentView"),
                    settings))
                {
                    window->ContentView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowVerticalSnapper>();
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowVerticalSnapper>()->ContentPadding = _Context->m_Style.get_frames_width();
                    _Context->end_node<ImmediateUserInterfaceWindowVerticalSnapper>();
                }

                if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                    std::string(_ID).append("/SnapperView/CentralSnapperView/RightSnapperView"),
                    settings))
                {
                    window->RightSnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalSnapper>();
                    _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
                }

                _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
            }

            // bottom
            if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                std::string(_ID).append("/SnapperView/BottomSnapperView"),
                settings))
            {
                window->BottomSnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalSnapper>();
                _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
            }

            _Context->end_node<ImmediateUserInterfaceWindowVerticalSnapper>();
        }

        _Context->end_node<ImmediateUserInterfaceWindowCentralDocker>();
    }

    return true;
}

void ImmediateUserInterfaceWindow::load_state(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    // self position and size
    gs_vec2f size =
        _Context->m_IniFileState.contains(Hash, "Size") ?
            _Context->m_IniFileState.get<gs_vec2f>(Hash, "Size") :
                gs_vec2f(512.f, 512.f);
    
    gs_vec2f position =
        _Context->m_IniFileState.contains(Hash, "Position") ?
            _Context->m_IniFileState.get<gs_vec2f>(Hash, "Position") :
                gs_vec2f(0.f, 0.f);
    
    DockingIndex =
        _Context->m_IniFileState.contains(Hash, "DockingIndex") ?
            _Context->m_IniFileState.get<int>(Hash, "DockingIndex") :
                -1;

    State.BoundingBox = gs_2dboxf(position, position + gs_clamp(size, State.MinimumSize, State.MaximumSize));

    // layout
    if(_Context->m_IniFileState.contains(Hash, "TopSnapperViewSize"))
    {
        TopSnapperView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "TopSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "LeftSnapperViewSize"))
    {
        LeftSnapperView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "LeftSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "RightSnapperViewSize"))
    {
        RightSnapperView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "RightSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "BottomSnapperViewSize"))
    {
        BottomSnapperView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "BottomSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "ContentViewSize"))
    {
        ContentView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "ContentViewSize"));
    }
}

void ImmediateUserInterfaceWindow::save_state(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    // self position and size
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "Size", State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "Position", State.BoundingBox.Min);

    // layout items positions and sizes
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "TopSnapperViewSize", TopSnapperView->State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "LeftSnapperViewSize", LeftSnapperView->State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "RightSnapperViewSize", RightSnapperView->State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "BottomSnapperViewSize", BottomSnapperView->State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "ContentViewSize", ContentView->State.BoundingBox.size());

    // docking
    _Context->m_IniFileState.set<int>(Hash, "DockingIndex", DockingIndex);

    if(Docker)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "Docker",
            ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().retrieve_docker_by_view(_Context, Docker)->Hash);
    }
    
    if(TopSnapper)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "TopSnapper",
            ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().retrieve_docker_by_view(_Context, TopSnapper)->Hash);
    }
    
    if(LeftSnapper)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "LeftSnapper",
            ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().retrieve_docker_by_view(_Context, LeftSnapper)->Hash);
    }
    
    if(RightSnapper)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "RightSnapper",
            ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().retrieve_docker_by_view(_Context, RightSnapper)->Hash);
    }
    
    if(BottomSnapper)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "BottomSnapper",
            ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().retrieve_docker_by_view(_Context, BottomSnapper)->Hash);
    }

    _Context->m_IniFileState.set<bool>(Hash, "IsActive", IsActive);
}

// ImmedidateUserInterfaceWindowController
ImmedidateUserInterfaceWindowController::ImmedidateUserInterfaceWindowController(){}
ImmedidateUserInterfaceWindowController::~ImmedidateUserInterfaceWindowController(){}

void ImmedidateUserInterfaceWindowController::frame_start(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    // create worksapce dockarea
    m_DockAreaOpened =
        (_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking) &&
        (_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking);

    if(_Context->begin_node<ImmediateUserInterfaceWindowDockArea>(
        std::string(ApplicationPlatformBackend::get_window_name()).append("###").append("DockingWorkspace"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ForceMoveOnBackground,
        &m_DockAreaOpened))
    {
        // retrieve window
        m_WorkspaceDockArea         = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindow>();
        m_WorkspaceDockArea->Opened = &m_DockAreaOpened;

        _Context->end_node<ImmediateUserInterfaceWindowDockArea>();
    }
}

void ImmedidateUserInterfaceWindowController::frame_debug(ImmediateUserInterfaceContextLayer* _Context)
{
    if(!(_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking))
        return;

    place_on_dockers(_Context);

    // setup rendering order of docking area
    if(m_WorkspaceDockArea != nullptr)
        m_WorkspaceDockArea->State.RenderingOrder = ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Background;

    // rebuild hierarchy
    m_WindowsDockingList.clear();
    m_WindowsDockingCache.clear();
    
    for(auto node : _Context->m_NodesRenderingList)
    {
        if(dynamic_cast<ImmediateUserInterfaceWindow*>(node))
            m_WindowsDockingList.push_back(node);
        else
            m_WindowsDockingCache.push_back(node);
    }

    std::stable_sort(
        m_WindowsDockingList.begin(),
        m_WindowsDockingList.end(),
        [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B) 
        {
            return dynamic_cast<const ImmediateUserInterfaceWindow*>(_A)->DockingIndex <
                dynamic_cast<const ImmediateUserInterfaceWindow*>(_B)->DockingIndex;
        });

    _Context->m_NodesRenderingList.clear();

    for(auto node : m_WindowsDockingList)
        _Context->m_NodesRenderingList.push_back(node);

    for(auto node : m_WindowsDockingCache)
        _Context->m_NodesRenderingList.push_back(node);

    _Context->m_Hierarchy.build(_Context->m_NodesRenderingList);

    // activate/deactivate windows
    for(auto node : _Context->m_NodesRenderingList)
    {
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(node);

        if(window != nullptr)
        {
            // activate window
            if(window->Activate)
            {
                // deactivate docker windows
                ImmediateUserInterfaceWindow* docker =
                    ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().retrieve_docker_by_view(_Context, (window->Docker ? window->Docker : window));

                if(docker != nullptr)
                    docker->IsActive = false;

                for(auto node : retrieve_docked_windows(_Context, docker, ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center))
                {
                    if(dynamic_cast<ImmediateUserInterfaceWindow*>(node) != nullptr)
                        dynamic_cast<ImmediateUserInterfaceWindow*>(node)->IsActive = false;
                }

                window->IsActive = true;
            }

            // activate singletone window
            if((window->Docker        == nullptr &&
                window->TopSnapper    == nullptr &&
                window->LeftSnapper   == nullptr &&
                window->RightSnapper  == nullptr &&
                window->BottomSnapper == nullptr &&
                retrieve_docked_windows(_Context, window, ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_All).empty()))
            {
                window->IsActive = true;
            }

            if(window->IsActive)
            {
                // setup maximum rendering order for docked window within it's docker hierarchy
                if(window->Docker != nullptr)
                {
                    int renderingOrder = window->Docker->State.RenderingOrder;

                    for(auto it  = _Context->m_Hierarchy.begin(window->Docker);
                            it != _Context->m_Hierarchy.end(window->Docker);
                            it++)
                    {
                        (*it)->State.RenderingOrder = renderingOrder;
                    }
                    
                    window->State.RenderingOrder = ++renderingOrder;
                }
                else
                {
                    // setup maximum rendering order for docker snapper view
                    int renderingOrder = 0;

                    for(auto it  = _Context->m_Hierarchy.begin(window->DockerView);
                            it != _Context->m_Hierarchy.end(window->DockerView);
                            it++)
                    {
                        (*it)->State.RenderingOrder = renderingOrder;
                    }

                    window->SnapperView->State.RenderingOrder = ++renderingOrder;
                }
            }

            // reset all
            window->Activate         = false;
            window->ReattachChildren = false;
        }
    }
}

void ImmedidateUserInterfaceWindowController::frame_finish(ImmediateUserInterfaceContextLayer* _Context)
{
    for(auto node : _Context->m_NodesRenderingList)
    {
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(node);

        if(window == nullptr) continue;

        // detach all windows from closed window
        if(window->Opened != nullptr && !(*window->Opened))
        {
            auto& dockedWindows = _Context->get_controller<ImmedidateUserInterfaceWindowController>()->retrieve_docked_windows(
                _Context,
                window,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_All);

            for(auto dockedWindow : dockedWindows)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(dockedWindow);

                if(window == nullptr)
                    continue;

                window->Docker               = nullptr;
                window->TopSnapper           = nullptr;
                window->LeftSnapper          = nullptr;
                window->RightSnapper         = nullptr;
                window->BottomSnapper        = nullptr;
                window->DockingIndex         = -1;
                window->State.RenderingOrder = ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Main;
            }
        }
    }
}

void ImmedidateUserInterfaceWindowController::place_on_dockers(ImmediateUserInterfaceContextLayer* _Context)
{
    // read docking info
    if(!_Context->m_IniFileState.empty())
    {
        // collect all windows
        std::map<std::string, ImmediateUserInterfaceWindow*> windows;
        windows[""] = nullptr;

        for(auto node : _Context->m_NodesRenderingList)
        {
            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(node);

            if(window == nullptr) continue;

            windows[window->Hash] = window;
        }

        // restore docking
        for(auto node : _Context->m_NodesRenderingList)
        {
            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(node);

            if(window == nullptr) continue;

            ImmediateUserInterfaceWindow* docker        = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "Docker")];
            ImmediateUserInterfaceWindow* topSnapper    = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "TopSnapper")];
            ImmediateUserInterfaceWindow* LeftSnapper   = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "LeftSnapper")];
            ImmediateUserInterfaceWindow* RightSnapper  = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "RightSnapper")];
            ImmediateUserInterfaceWindow* BottomSnapper = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "BottomSnapper")];

            if(docker && can_be_docked(_Context, docker, window))
                window->Docker = docker->DockerView;

            if(topSnapper && can_be_docked(_Context, topSnapper, window))
                window->TopSnapper = topSnapper->TopSnapperView;

            if(LeftSnapper && can_be_docked(_Context, LeftSnapper, window))
                window->LeftSnapper = LeftSnapper->LeftSnapperView;

            if(RightSnapper && can_be_docked(_Context, RightSnapper, window))
                window->RightSnapper = RightSnapper->RightSnapperView;

            if(BottomSnapper && can_be_docked(_Context, BottomSnapper, window))
                window->BottomSnapper = BottomSnapper->BottomSnapperView;

            if(_Context->m_IniFileState.get<bool>(window->Hash, "IsActive"))
                window->Activate = true;
        }
    }

    // analyze moved windows
    if(gs_vector_length(_Context->m_Input.get_cusor_drag_delta()) < 8.f)
        return;

    // find moved node and detach it from a docker
    ImmediateUserInterfaceWindow* moved = dynamic_cast<ImmediateUserInterfaceWindow*>(
        ImmediateUserInterfaceContextLayerHelpers::ImmedidateUserInterfaceMovedNodeSearcher().search(
            _Context,
            [](const ImmediateUserInterfaceNode*)->bool{return true;}));

    detach_from_docker(_Context, moved);

    // find top most hovered node not equal to the moved one
    ImmediateUserInterfaceNode* hoveredNode = 
        ImmediateUserInterfaceContextLayerHelpers::ImmedidateUserInterfaceHoveredNodeSearcher().search(_Context, [moved](const ImmediateUserInterfaceNode* _Node)->bool{return _Node != moved;});

    // detect hovered window
    ImmediateUserInterfaceWindow* hovered =
        _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceWindow>(hoveredNode);

    if(hovered == nullptr || moved == nullptr)
        return;

    // dock the moved window to the hovered one if all mouse buttons are released

    // compute gizmos
    gs_2dboxf dockingGizmo = gs_2dboxf(
        hovered->ContentBox.center() - gs_min(hovered->ContentBox.size().x, hovered->ContentBox.size().y) * 0.25f,
        hovered->ContentBox.center() + gs_min(hovered->ContentBox.size().x, hovered->ContentBox.size().y) * 0.25f);

    dockingGizmo = gs_2dboxf(
        dockingGizmo.Min,
        dockingGizmo.Min + gs_min(dockingGizmo.size().x, 256.f));

    dockingGizmo = gs_2dboxf(
        hovered->ContentBox.center() - dockingGizmo.size() * 0.5f,
        hovered->ContentBox.center() + dockingGizmo.size() * 0.5f);


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

    if(!_Context->m_Input.is_mouse_button_down())
    {
        if(centralDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center);
        }
        if(topDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Top);
        }
        else if(leftDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Left);
        }
        else if(rightDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Right);
        }
        else if(bottomDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Bottom);
        }
    }
    else if(can_be_docked(_Context, moved, hovered))
    {
        // render potential docking window gizmo
        int depth = ImmediateUserInterfaceContextLayerHelpers::calculate_layer_depth(
            _Context,
            ImmedidateUserInterfaceRenderingLayer_::ImmedidateUserInterfaceRenderingLayer_Gizmos);

        if(dockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            _Context->m_Renderer->push_rectangle_rounded_filled(
                dockingGizmo.Min,
                dockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                gs_rgba_color(
                    gs_rgba_color_get_r(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    gs_rgba_color_get_g(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    gs_rgba_color_get_b(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    128),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                topDockingGizmo.Min,
                topDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                topDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                leftDockingGizmo.Min,
                leftDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                leftDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                rightDockingGizmo.Min,
                rightDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                rightDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                bottomDockingGizmo.Min,
                bottomDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                bottomDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                centralDockingGizmo.Min,
                centralDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                centralDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));
        }
    }
}

bool ImmedidateUserInterfaceWindowController::can_be_docked(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked)
{
    // general checks
    if(_Context == nullptr || _Docker  == nullptr || _Docked  == nullptr)
        return false;
    
    // check that we are already docked to a docker
    {
        ImmediateUserInterfaceNode* docker = _Context->m_Hierarchy.get_parent(_Docked);

        while (docker)
        {
            if(docker == _Docker)
                return false;
            docker = _Context->m_Hierarchy.get_parent(docker);
        }
    }

    // check that we are not trying to attach to self
    {
        ImmediateUserInterfaceNode* docker = _Docker;

        while (docker)
        {
            if(docker == _Docked)
                return false;
            docker = _Context->m_Hierarchy.get_parent(docker);
        }
    }

    return true;
}

void ImmedidateUserInterfaceWindowController::attach_to_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked, const ImmedidateUserInterfaceDockingAnchor& _Anchors)
{
    // auxiliary lambdas
    auto move_to_cache = [this](
        ImmediateUserInterfaceContextLayer* _Context,
        ImmediateUserInterfaceWindow*       _Docker)
    {
        if(_Context == nullptr || _Docker == nullptr)
            return;

        m_WindowsDockingCache.push_back(_Docker);
    };

    auto move_child_docked_windows_to_cache = [this](
        ImmediateUserInterfaceContextLayer*          _Context,
        ImmediateUserInterfaceNode*                  _Docker,
        const ImmedidateUserInterfaceDockingAnchor& _Orientation)
    {
        if(_Context == nullptr || _Docker == nullptr)
            return;

        auto& dockedWindows = retrieve_docked_windows(_Context, _Docker, _Orientation);
        for(auto it  = dockedWindows.begin(); it != dockedWindows.end(); it++)
            m_WindowsDockingCache.push_back(*it);
    };

    if(!can_be_docked(_Context, _Docker, _Docked))
        return;

    // get ready
    m_WindowsDockingCache.clear();
    m_WindowsDockingList.clear();

    // attach to a central part as a tab
    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center)
    {
        ImmediateUserInterfaceWindow * docker =
            _Docker->Docker ?
                ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().retrieve_docker_by_view(_Context, _Docker->Docker) :
                    _Docker;

        // move child docked windows and self to windows docking cache
        move_child_docked_windows_to_cache(_Context, docker, _Anchors);
        move_to_cache(_Context, _Docked);
        move_child_docked_windows_to_cache(_Context, _Docked, _Anchors);

        // reindex docked nodes and setup their docker
        int dockindex = 0;

        for(auto it = m_WindowsDockingCache.begin(); it != m_WindowsDockingCache.end(); it++)
        {
            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

            if(window == nullptr)
                continue;

            window->Docker = docker->DockerView;
            window->DockingIndex  = dockindex++;
        }

        // setup self as active
        _Docked->Activate = true;

        // clear
        m_WindowsDockingCache.clear();
        m_WindowsDockingList.clear();
        return;
    }

    // attach to top/left/right/bottom parts
    ImmediateUserInterfaceWindow* docker = _Docker;

    // move child docked windows and self to windows docking cache
    move_child_docked_windows_to_cache(_Context, docker, _Anchors);
    move_to_cache(_Context, _Docked);

    // reindex docked nodes and setup their docker
    int dockindex = 0;

    for(auto it = m_WindowsDockingCache.begin(); it != m_WindowsDockingCache.end(); it++)
    {
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

        if(window == nullptr)
            continue;

        if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Top)
            window->TopSnapper = docker->TopSnapperView;
        else if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Left)
            window->LeftSnapper = docker->LeftSnapperView;
        else if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Right)
            window->RightSnapper = docker->RightSnapperView;
        else if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Bottom)
            window->BottomSnapper = docker->BottomSnapperView;

        window->DockingIndex = dockindex++;
    }

    // clear
    m_WindowsDockingCache.clear();
    m_WindowsDockingList.clear();
}

void ImmedidateUserInterfaceWindowController::detach_from_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Detached)
{
    if(_Detached == nullptr)
        return;

    // reattach docked windows of detached window
    if(_Detached->ReattachChildren)
    {
        auto& dockedWindows = retrieve_docked_windows(
            _Context,
            _Detached,
            ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center);

        if(!dockedWindows.empty())
        {
            ImmediateUserInterfaceWindow* newDocker =
                dynamic_cast<ImmediateUserInterfaceWindow*>(dockedWindows[0]);

            int index = 0;
            for(auto it = dockedWindows.begin(); it != dockedWindows.end(); it++)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);
                
                if(window == nullptr || window == newDocker)
                    continue;
                    
                window->Docker       = newDocker->DockerView;
                window->DockingIndex = index++;
            }

            newDocker->Docker        = nullptr;
            newDocker->TopSnapper    = nullptr;
            newDocker->LeftSnapper   = nullptr;
            newDocker->RightSnapper  = nullptr;
            newDocker->BottomSnapper = nullptr;
            newDocker->DockingIndex  = -1;

            if(_Detached->TopSnapper != nullptr)
                newDocker->TopSnapper = _Detached->TopSnapper;
            else if(_Detached->LeftSnapper != nullptr)
                newDocker->LeftSnapper = _Detached->LeftSnapper;
            else if(_Detached->RightSnapper != nullptr)
                newDocker->RightSnapper = _Detached->RightSnapper;
            else if(_Detached->BottomSnapper != nullptr)
                newDocker->BottomSnapper = _Detached->BottomSnapper;

            _Detached->Activate = true;
            newDocker->Activate = true;
        }
    }

    // setup active window within self
    auto& dockedWindows = retrieve_docked_windows(
        _Context,
        _Detached->Docker,
        ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center);

    if (dockedWindows.size() > 1)
    {
        dynamic_cast<ImmediateUserInterfaceWindow*>(dockedWindows[0])->IsActive = true;
    }
    else
    {
        ImmediateUserInterfaceWindow* docker =
            ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().retrieve_docker_by_view(_Context, _Detached->Docker);

        if(docker != nullptr)
            docker->IsActive = true;
    }

    // detach from docker
    _Detached->TopSnapper    = nullptr;
    _Detached->LeftSnapper   = nullptr;
    _Detached->RightSnapper  = nullptr;
    _Detached->BottomSnapper = nullptr;
    _Detached->Docker        = nullptr;
    _Detached->DockingIndex  = -1;
}

std::vector<ImmediateUserInterfaceNode*>& ImmedidateUserInterfaceWindowController::retrieve_docked_windows(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Docker, const ImmedidateUserInterfaceDockingAnchor& _Anchors)
{
    // get ready
    m_WindowsDockingList.clear();

    ImmediateUserInterfaceWindow* docker =
        dynamic_cast<ImmediateUserInterfaceWindow*>(
            ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceWindowUtility().retrieve_docker_by_view(_Context, _Docker));

    if(docker == nullptr)
        return m_WindowsDockingList;

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Top)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->TopSnapperView); it != _Context->m_Hierarchy.end(docker->TopSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Left)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->LeftSnapperView); it != _Context->m_Hierarchy.end(docker->LeftSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Right)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->RightSnapperView); it != _Context->m_Hierarchy.end(docker->RightSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Bottom)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->BottomSnapperView); it != _Context->m_Hierarchy.end(docker->BottomSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->DockerView); it != _Context->m_Hierarchy.end(docker->DockerView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    return m_WindowsDockingList;
}

// ImmedidateUserInterfaceEventsController
ImmedidateUserInterfaceInputController::ImmedidateUserInterfaceInputController(){}
ImmedidateUserInterfaceInputController::~ImmedidateUserInterfaceInputController(){}

void ImmedidateUserInterfaceInputController::frame_debug(ImmediateUserInterfaceContextLayer* _Context)
{
    // main code
    if(_Context == nullptr)
        return;

    // find the top most node catching the mouse cursor
    ImmediateUserInterfaceNode* hoveredNode  = nullptr;
    ImmediateUserInterfaceNode* eventNode    = nullptr;
    int                         maximumDepth = INT_MIN;

    for (auto& node : _Context->m_NodesRenderingList)
    {
        if(_Context->m_Input.is_mouse_button_pressed())
        {
            // deselect node on mouse press
            node->State.Selected = false;

            // setup default rendering order
            if((node->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ForceMoveOnBackground))
            {
                node->State.RenderingOrder =
                    ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Background;
            }
            else
            {
                node->State.RenderingOrder =
                    ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Main;
            }
        }

        // unhover invisible node
        if(!node->is_partially_visible(_Context))
        {
            node->State.MouseHover = ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None;
            continue;
        }

        // find node catching events
        if(eventNode == nullptr && node->State.Events != ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
            eventNode = node;

        // hover end logic
        if(!node->get_visible_rect(_Context).contains(_Context->m_Input.get_cusor_position()))
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

    // process hovered node
    if(hoveredNode != nullptr)
    {
        // highlight hovered node
        if((_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes))
        {
            int depth = ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node(hoveredNode);

            _Context->m_Renderer->push_rectangle_rounded(
                hoveredNode->get_visible_rect(_Context).Min,
                hoveredNode->get_visible_rect(_Context).Max,
                _Context->m_Style.get_frames_radius(),
                _Context->m_Style.get_frames_width(),
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth));
        }

        // start hover node
        if(!(hoveredNode->Cache.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseEntered))
        {
            hoveredNode->State.MouseEnterTimer = Frenchie::Core::tic();
            hoveredNode->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseEntered;
        }
        else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
            hoveredNode->State.MouseEnterTimer,
            Frenchie::Core::tic()) > 10.0) // TODO: this MUST BE A SETTING !!!
        {
            // make this node hovered
            hoveredNode->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered;

            // the only one node can be hovered
            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(node != hoveredNode)
                    node->State.MouseHover = ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None;
            }

            // select this node on mouse click
            if(_Context->m_Input.is_mouse_button_pressed())
                hoveredNode->State.Selected = true;
        }
    }

    // catch events
    ImmediateUserInterfaceNode* eventCatcher = eventNode != nullptr ? eventNode : hoveredNode;

    if(eventCatcher != nullptr)
    {
        eventCatcher->events(_Context);

        // check in-parent intersection and process events of intersected nodes
        for (auto it  = _Context->m_Hierarchy.begin(_Context->m_Hierarchy.get_parent(eventCatcher));
                    it != _Context->m_Hierarchy.end(_Context->m_Hierarchy.get_parent(eventCatcher));
                    it++)
        {
            if((*it) == eventCatcher)
                continue;

            if(gs_2dboxf(
                (*it)->get_visible_rect(_Context).Min - ImmediateUserInterfaceContextLayerHelpers::calculate_offset(_Context),
                (*it)->get_visible_rect(_Context).Max + ImmediateUserInterfaceContextLayerHelpers::calculate_offset(_Context)).contains(_Context->m_Input.get_cusor_position()))
            {
                // process events
                (*it)->events(_Context);

                // reset event loop if events of parent and it's children are different
                if((*it)->State.Events != eventCatcher->State.Events)
                    (*it)->State.Events = ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None;
            }
        }

        // pass focus
        if(_Context->m_Input.is_mouse_button_pressed() ||
            eventCatcher->State.Events != ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
        {
            ImmediateUserInterfaceNode* focused = eventCatcher;
            ImmediateUserInterfaceNode* parent  = _Context->m_Hierarchy.get_parent(eventCatcher);

            while (parent)
            {
                focused = parent;
                parent  = _Context->m_Hierarchy.get_parent(parent);
            }
            
            if(!(focused->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ForceMoveOnBackground))
                focused->State.RenderingOrder = ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Focus;
        }
    }
}

// ImmedidateUserInterfaceLayoutController
ImmedidateUserInterfaceLayoutController::ImmedidateUserInterfaceLayoutController(){}
ImmedidateUserInterfaceLayoutController::~ImmedidateUserInterfaceLayoutController(){}

void ImmedidateUserInterfaceLayoutController::frame_debug(ImmediateUserInterfaceContextLayer* _Context)
{
    // layout
    for (auto& singleton : _Context->m_Hierarchy.Singletons)
        node_layout(_Context, singleton);
}

void ImmedidateUserInterfaceLayoutController::node_layout(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
{
    if(_Context == nullptr || _Node == nullptr)
        return;

    _Node->layout(_Context);
    _Node->measure(_Context);

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
        node_layout(_Context, (*it));
}

ImmedidateUserInterfaceRenderingController::ImmedidateUserInterfaceRenderingController(){}
ImmedidateUserInterfaceRenderingController::~ImmedidateUserInterfaceRenderingController(){}
void ImmedidateUserInterfaceRenderingController::frame_render(ImmediateUserInterfaceContextLayer* _Context)
{
    // get ready
    m_NodesRenderingCache.clear();

    std::stable_sort(
        _Context->m_Hierarchy.Singletons.begin(),
        _Context->m_Hierarchy.Singletons.end(),
        [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
        {
            return _A->State.RenderingOrder < _B->State.RenderingOrder;
        });
    
    // render singletones
    for (auto& singleton : _Context->m_Hierarchy.Singletons)
    {
        for (auto& renderedNode : m_NodesRenderingCache)
        {
            int depth =
                gs_max(
                    renderedNode->State.MaximumChildDepth + renderedNode->State.MaximumChildThickness - renderedNode->State.Depth,
                    renderedNode->State.MaximumChildDepth + renderedNode->State.SelfThickness + 1);

            singleton->State.Depth =
                gs_max(
                    depth,
                    singleton->State.Depth);
        }

        ImmedidateUserInterfaceRenderingController::render_node(_Context, singleton);
        m_NodesRenderingCache.push_back(singleton);
    }

    #ifdef IMMEDIATE_USER_INTERFACE_DEBUG

    std::cout << "\n\n\n";
    std::cout << "------------------------------------------------------------------------------------------------\n";
    std::cout << "HIERARCHY:\n";
    std::cout << "------------------------------------------------------------------------------------------------\n";
    for (auto& singleton : _Context->m_Hierarchy.Singletons)
        showHierarchy(_Context, singleton, "\t");
    #endif

    // clean-up
    m_NodesRenderingCache.clear();
}

void ImmedidateUserInterfaceRenderingController::render_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
{
    if(_Node == nullptr || !_Node->is_partially_visible(_Context)) return;

    // calculate clippingbox
    _Context->m_Renderer->push_clip_box(_Node->get_clipping_box(_Context));

    // render self
    _Node->render(_Context);

    // render children
    if(_Node->State.OrderChildrenWhileRendering)
    {
        std::stable_sort(
            _Context->m_Hierarchy.begin(_Node),
            _Context->m_Hierarchy.end(_Node),
            [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
            {
                return _A->State.RenderingOrder < _B->State.RenderingOrder;
            });
    }

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
    {
        int depth = gs_max(
            _Node->State.MaximumChildDepth + _Node->State.MaximumChildThickness - _Node->State.Depth,
            _Node->State.MaximumChildDepth + _Node->State.SelfThickness + 1,
            _Node->State.Depth + _Node->State.SelfThickness + 1);

        (*it)->State.Depth = _Node->State.PlaceInFollow ? depth : _Node->State.Depth + _Node->State.SelfThickness + 1;

        render_node(_Context, (*it));

        _Node->State.MaximumChildDepth     = gs_max(_Node->State.MaximumChildDepth, (*it)->State.Depth);
        _Node->State.MaximumChildThickness = gs_max(_Node->State.MaximumChildThickness, (*it)->State.SelfThickness);
    }

    _Node->State.MaximumChildThickness = gs_max(_Node->State.MaximumChildThickness, _Node->State.SelfThickness);

    // update parent maximum child depth and maximum child thickness
    auto parent = _Context->m_Hierarchy.get_parent(_Node);

    while (parent)
    {
        parent->State.MaximumChildDepth     = gs_max(parent->State.MaximumChildDepth, _Node->State.MaximumChildDepth);
        parent->State.MaximumChildThickness = gs_max(parent->State.MaximumChildThickness, _Node->State.MaximumChildThickness);
        parent = _Context->m_Hierarchy.get_parent(parent);
    }            

    // remove clipping
    _Context->m_Renderer->pop_clip_box();
}

ImmedidateUserInterfaceMenusController::ImmedidateUserInterfaceMenusController(){}
ImmedidateUserInterfaceMenusController::~ImmedidateUserInterfaceMenusController(){}

void detect_maximum_width(
    ImmediateUserInterfaceContextLayer* _Context,
    ImmediateUserInterfaceNode*         _Node,
    float&                              _MaximumWidth)
{
    ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Node);

    if(scrollArea != nullptr && scrollArea->ContentView != nullptr)
    {
        for(auto it = _Context->m_Hierarchy.begin(scrollArea->ContentView); it != _Context->m_Hierarchy.end(scrollArea->ContentView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceMenuAction*>(*it))
            {
                gs_vec2f size =
                    _Context->m_Renderer->calculate_bounding_box(
                        (*it)->Name.begin(),
                        (*it)->Name.end(),
                        _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() +
                        gs_vec2f(_Context->m_Style.get_font_size() * 2.f, _Context->m_Style.get_font_size() * 0.5f);

                _MaximumWidth = gs_max(_MaximumWidth, size.x);
            }
        }
    }

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
        detect_maximum_width(_Context, *it, _MaximumWidth);
}

void setup_maximum_with(
    ImmediateUserInterfaceContextLayer* _Context,
    ImmediateUserInterfaceNode*         _Node,
    float&                              _MaximumWidth)
{
    ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Node);

    if(scrollArea != nullptr && scrollArea->ContentView != nullptr)
    {
        for(auto it = _Context->m_Hierarchy.begin(scrollArea->ContentView); it != _Context->m_Hierarchy.end(scrollArea->ContentView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceMenuAction*>(*it))
            {
                gs_vec2f size =
                    _Context->m_Renderer->calculate_bounding_box(
                        (*it)->Name.begin(),
                        (*it)->Name.end(),
                        _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() +
                        gs_vec2f(_Context->m_Style.get_font_size() * 2.f, _Context->m_Style.get_font_size() * 0.5f);

                dynamic_cast<ImmediateUserInterfaceMenuAction*>(*it)->State.BoundingBox =
                    gs_2dboxf(
                        dynamic_cast<ImmediateUserInterfaceMenuAction*>(*it)->State.BoundingBox.Min,
                        dynamic_cast<ImmediateUserInterfaceMenuAction*>(*it)->State.BoundingBox.Min + gs_vec2f(_MaximumWidth, size.y));
            }
        }
    }

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
        setup_maximum_with(_Context, *it, _MaximumWidth);
}

void ImmedidateUserInterfaceMenusController::frame_finish(ImmediateUserInterfaceContextLayer* _Context)
{
    ActiveMenus.clear();

    // layout menu actions
    for(auto node : _Context->m_NodesRenderingList)
    {
        ImmediateUserInterfaceMenu* menu =
            dynamic_cast<ImmediateUserInterfaceMenu*>(node);

        if(menu != nullptr)
        {
            // look for parental scroll bar
            ImmediateUserInterfaceScrollArea* scroll = nullptr;
            ImmediateUserInterfaceScrollArea* parent = _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceScrollArea>(menu);

            while (parent)
            {
                scroll = parent;
                parent = _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceScrollArea>(parent);
            }
            
            if(scroll)
            {
                // manage internal scroll area
                float internal = 0.f;
                detect_maximum_width(_Context, scroll, internal);
                detect_maximum_width(_Context, menu->InternalScrollArea, internal);
                setup_maximum_with(_Context, scroll, internal);
                setup_maximum_with(_Context, menu->InternalScrollArea, internal);

                // manage external scroll area
                float external = 0.f;
                detect_maximum_width(_Context, menu->ExternalScrollArea, external);
                setup_maximum_with(_Context, menu->ExternalScrollArea, external);
            }
            else
            {
                // manage internal scroll area
                float internal = 0.f;
                detect_maximum_width(_Context, menu->InternalScrollArea, internal);
                setup_maximum_with(_Context, menu->InternalScrollArea, internal);

                // manage external scroll area
                float external = 0.f;
                detect_maximum_width(_Context, menu->ExternalScrollArea, external);
                setup_maximum_with(_Context, menu->ExternalScrollArea, external);
            }
        }

        // collect active menus
        if(!(node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered)) continue;

        auto relative = node;

        while (relative)
        {
            if(dynamic_cast<ImmediateUserInterfaceMenu*>(relative))
                ActiveMenus.push_back(dynamic_cast<ImmediateUserInterfaceMenu*>(relative));
            relative = relative->State.Relative;
        }
    }
}

// ImmedidateUserInterfaceNextNodeController
ImmedidateUserInterfaceNextNodeController::ImmedidateUserInterfaceNextNodeController(){}
ImmedidateUserInterfaceNextNodeController::~ImmedidateUserInterfaceNextNodeController(){}

void ImmedidateUserInterfaceNextNodeController::frame_start(ImmediateUserInterfaceContextLayer*)
{
    // reset all
    reset();
}

void ImmedidateUserInterfaceNextNodeController::reset()
{
    // reset all
    NextLine.reset();
    NextMinimumSize.reset();
    NextMaximumSize.reset();
    NextPosition.reset();
    NextContentPadding.reset();
}

// ImmediateUserInterfaceScrollBarsController
ImmediateUserInterfaceScrollBarsController::ImmediateUserInterfaceScrollBarsController(){}
ImmediateUserInterfaceScrollBarsController::~ImmediateUserInterfaceScrollBarsController(){}
void ImmediateUserInterfaceScrollBarsController::frame_debug(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    ImmediateUserInterfaceNode* hoveredNode =
        ImmediateUserInterfaceContextLayerHelpers::ImmedidateUserInterfaceHoveredNodeSearcher().search(
            _Context,
            [](const ImmediateUserInterfaceNode* _Node)->bool{return dynamic_cast<const ImmediateUserInterfaceScrollArea*>(_Node);});

    ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(hoveredNode);
    
    if(scrollArea == nullptr ||
        !scrollArea->State.BoundingBox.contains(_Context->m_Input.get_cusor_position())) return;

    // adjust vertical scroll bar by mouse wheel
    if((scrollArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment))
    {
        ImmediateUserInterfaceScrollAreaScrollBar* verticalScrollBar =
            scrollArea != nullptr ? scrollArea->VerticalScrollBar : nullptr; 

        if(verticalScrollBar != nullptr)
        {
            if(gs_vector_length(_Context->m_Input.get_cusor_scroll_offset()) > 0.f)
            {
                verticalScrollBar->set_scroll_offset(
                    _Context->m_Input.get_cusor_scroll_offset() * (-1.f) * gs_min(scrollArea->State.ContentSize.y, scrollArea->State.BoundingBox.size().y) * 0.05f);
            }
        }
    }

    // adjust vertical scrollbar by arrow keys
    if((scrollArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment))
    {
        ImmediateUserInterfaceScrollAreaScrollBar* verticalScrollBar =
            scrollArea != nullptr ? scrollArea->VerticalScrollBar : nullptr; 

        if(verticalScrollBar != nullptr)
        {
            if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow) ||
                _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow))
            {
                gs_vec2f offset =
                    !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow) ?
                        (-1.f) * gs_min(scrollArea->State.ContentSize.y, scrollArea->State.BoundingBox.size().y) * 0.05f :
                            (-4.f);

                verticalScrollBar->set_scroll_offset(offset);
            }
            else if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow) ||
                    _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow))
            {
                gs_vec2f offset =
                    !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow) ?
                        (+1.f) * gs_min(scrollArea->State.ContentSize.y, scrollArea->State.BoundingBox.size().y) * 0.05f :
                            (+4.f);

                verticalScrollBar->set_scroll_offset(offset);
            }
        }
    }

    // adjust horizontal scrollbar by arrow keys
    if((scrollArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment))
    {
        ImmediateUserInterfaceScrollAreaScrollBar* horizontalScrollBar =
            scrollArea != nullptr ? scrollArea->HorizontalScrollBar : nullptr; 

        if(horizontalScrollBar != nullptr)
        {
            if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow) ||
                _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow))
            {
                gs_vec2f offset =
                    !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow) ?
                        (-1.f) * gs_min(scrollArea->State.ContentSize.x, scrollArea->State.BoundingBox.size().x ) * 0.05f :
                            (-4.f);

                horizontalScrollBar->set_scroll_offset(offset);
            }
            else if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ||
                    _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow))
            {
                gs_vec2f offset =
                    !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ?
                        (+1.f) * gs_min(scrollArea->State.ContentSize.x, scrollArea->State.BoundingBox.size().x) * 0.05f :
                            (+4.f);

                horizontalScrollBar->set_scroll_offset(offset);
            }
        }
    }
}

// ImmediateUserInterfaceContextLayer2
ImmediateUserInterfaceContextLayer::ImmediateUserInterfaceContextLayer(){}
ImmediateUserInterfaceContextLayer::~ImmediateUserInterfaceContextLayer(){}

bool ImmediateUserInterfaceContextLayer::awake()
{
    // launch renderer
    if(m_Renderer == nullptr)
        m_Renderer = application()->push_layer<RenderingQueue>();

    // create hierarchy
    m_Hierarchy = ImmedidateUserInterfaceHierarchy(
        [](const ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
        {
            if(_Node == nullptr)
                return nullptr;

            if(_Node->State.Parent != nullptr)
                return _Node->State.Parent;

            const ImmediateUserInterfaceWindow* window =
                dynamic_cast<const ImmediateUserInterfaceWindow*>(_Node);

            if(window != nullptr && window->Docker != nullptr)
                return window->Docker;

            if(window != nullptr && window->TopSnapper != nullptr)
                return window->TopSnapper;

            if(window != nullptr && window->LeftSnapper != nullptr)
                return window->LeftSnapper;

            if(window != nullptr && window->RightSnapper != nullptr)
                return window->RightSnapper;

            if(window != nullptr && window->BottomSnapper != nullptr)
                return window->BottomSnapper;

            return nullptr;
        });

    // create controllers
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceWindowController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceInputController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceScrollBarsController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceLayoutController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceRenderingController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceNextNodeController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceMenusController>());

    // awake controllers
    for(auto& controller : m_Controllers)
        GS_ASSERT(controller->awake(this));

    // load .ini file
    m_IniFileState.read(m_IniFilePath);

    // create input handler
    m_Input = ImmedidateUserInterfaceInput(this);

    return m_Renderer != nullptr;
}

void ImmediateUserInterfaceContextLayer::frame_start()
{
    for(auto& controller : m_Controllers)
        controller->frame_start(this);
}

void ImmediateUserInterfaceContextLayer::frame_update()
{
}

void ImmediateUserInterfaceContextLayer::frame_debug()
{
    // build hierarchy
    m_Hierarchy.build(m_NodesRenderingList);

    // execute controllers
    for(auto& controller : m_Controllers)
        controller->frame_debug(this);
}

void ImmediateUserInterfaceContextLayer::frame_render()
{
    for(auto& controller : m_Controllers)
        controller->frame_render(this);
}

void ImmediateUserInterfaceContextLayer::frame_finish()
{
    // save state
    if(application()->is_closed())
    {
        for (auto node : m_NodesRenderingList)
            node->save_state(this);
        m_IniFileState.write(m_IniFilePath);
    }

    // process controllers
    for(auto& controller : m_Controllers)
        controller->frame_finish(this);

    // save state
    for (auto& node : m_NodesRenderingList)
    {
        // stop all modifications
        if(!m_Input.is_mouse_button_down())
            node->State.Events = ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None;

        // save cache
        node->Cache.Events        = node->State.Events;
        node->Cache.MouseHover    = node->State.MouseHover;
        node->Cache.SelfThickness = node->State.SelfThickness;
        node->Cache.Depth         = node->State.Depth;
        node->Cache.Parent        = node->State.Parent;
        
        if(node->State.Events == ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
            node->Cache = node->State;

        // restore
        node->State.ClippingBox.reset();

        node->State.Depth                 = 0;
        node->State.SelfThickness         = 0;
        node->State.RenderingIndex        = 0;
        node->State.MaximumChildDepth     = 0;
        node->State.MaximumChildThickness = 0;
        node->State.Settings              = 0;
        node->Count                       = 0;
    }

    // check rendering stack
    GS_ASSERT(m_NodesRenderingStack.empty());

    // clean-up rendering data
    m_NodesRenderingList.clear();
    m_NodesRenderingStack.clear();
    m_NodesRenderedStack.clear();

    // clear ini file state
    m_IniFileState.clear();
}

void ImmediateUserInterfaceContextLayer::finish()
{
    // close renderer
    if(m_Renderer != nullptr)
        m_Renderer->close();
}

bool ImmediateUserInterfaceContextLayer::begin_scrollarea(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceScrollArea>(_ID, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_scrollarea()
{
    end_node<ImmediateUserInterfaceScrollArea>();
}

bool ImmediateUserInterfaceContextLayer::begin_panel(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceNodePanel>(_ID, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_panel()
{
    end_node<ImmediateUserInterfaceNodePanel>();
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

bool ImmediateUserInterfaceContextLayer::push_button(const std::string& _ID)
{
    struct ImmediateUserInterfacePushButton : public ImmediateUserInterfaceNode
    {
    public:
        ImmediateUserInterfacePushButton(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
        {
            State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 128.f));
            State.MinimumSize = gs_vec2f(gs_vec2f(128.f, 64.f));
            State.MaximumSize = gs_vec2f(gs_vec2f(256.f, 128.f));
        }
        
        virtual ~ImmediateUserInterfacePushButton(){}

        virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override{}
    };

    if(begin_node<ImmediateUserInterfacePushButton>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfacePushButton* widget = 
            get_rendering_stack_top<ImmediateUserInterfacePushButton>();

        // render
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            int depth = widget->Cache.Depth;

            // background
            m_Renderer->push_rectangle_rounded_filled(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Max,
                m_Style.get_frames_radius(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                m_Renderer->calculate_transform_matrix((float)depth++));

            if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down())
            {
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }
            else
            {
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                    m_Style.get_frames_radius(),
                    (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }

            // title
            m_Renderer->push_text(
                gs_vec2f(widget->State.BoundingBox.center() - m_Renderer->calculate_bounding_box(widget->Name.begin(), widget->Name.end(), m_Style.get_font_size(), m_Style.get_current_font()).size() * 0.5f),
                widget->Name.begin(),
                widget->Name.end(),
                m_Style.get_font_size(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                m_Renderer->calculate_transform_matrix((float)depth++));
        
            m_Renderer->pop_clip_box();
        }

        end_node<ImmediateUserInterfacePushButton>();

        return (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_clicked();
    }

    return false;
}

bool ImmediateUserInterfaceContextLayer::menu_action(const std::string& _ID)
{
    if(begin_node<ImmediateUserInterfaceMenuAction>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceMenuAction* widget =
            get_rendering_stack_top<ImmediateUserInterfaceMenuAction>();

        end_node<ImmediateUserInterfaceMenuAction>();

        return (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_clicked();
    }

    return false;
}

bool ImmediateUserInterfaceContextLayer::check_button(
    const std::string&                               _ID,
    const ImmediateUserInterfaceCheckButtonSettings& _Settings,
    bool*                                            _Checked)
{
    struct ImmediateUserInterfaceCheckButton : public ImmediateUserInterfaceNode
    {
    public:
        ImmediateUserInterfaceCheckButton(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
        virtual ~ImmediateUserInterfaceCheckButton(){}

        void layout(ImmediateUserInterfaceContextLayer* _Context)
        {
        }
    };

    if(begin_node<ImmediateUserInterfaceCheckButton>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        // setup
        ImmediateUserInterfaceCheckButton* widget =  get_rendering_stack_top<ImmediateUserInterfaceCheckButton>();
        
        // event processing
        {
            if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_clicked() && _Checked != nullptr)
                *_Checked = !(*_Checked);
        }

        // render
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            int depth = widget->Cache.Depth;

            // render checkbox
            if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox)
            {
                // background
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Max,
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                    m_Renderer->calculate_transform_matrix((float)depth++));

                if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down())
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
                else
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }

                // tick
                if(_Checked != nullptr && (*_Checked))
                {
                    gs_vec2f start = gs_vec2f(
                        widget->State.BoundingBox.center().x,
                        widget->State.BoundingBox.center().y + widget->State.BoundingBox.height() * 0.5f * 0.5f);

                    m_Renderer->push_line(
                        start,
                        gs_vec2f(
                            widget->State.BoundingBox.center().x - widget->State.BoundingBox.width() * 0.5f * 0.7f,
                            widget->State.BoundingBox.center().y - widget->State.BoundingBox.height() * 0.5f * 0.25f),
                        m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));

                    m_Renderer->push_line(
                        start,
                        gs_vec2f(
                            widget->State.BoundingBox.center().x + widget->State.BoundingBox.width() * 0.5f * 0.7f,
                            widget->State.BoundingBox.center().y - widget->State.BoundingBox.height() * 0.5f * 0.9f),
                        m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
            }
            
            // render radio button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton)
            {
                // background
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Max,
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                    m_Renderer->calculate_transform_matrix((float)depth++));

                if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down())
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
                else
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }

                if(_Checked != nullptr && (*_Checked))
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width() * 3.f,
                        widget->State.BoundingBox.Max - m_Style.get_frames_width() * 3.f,
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
            }
            
            // render slider button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton)
            {
                // background
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Max,
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                    m_Renderer->calculate_transform_matrix((float)depth++));

                if(_Checked != nullptr && (*_Checked))
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min,
                        widget->State.BoundingBox.Max,
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                        m_Renderer->calculate_transform_matrix((float)depth++));

                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Max - m_Style.get_font_size() + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
                else
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Min + m_Style.get_font_size() - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
            }
        
            m_Renderer->pop_clip_box();
        }

        // layout geometry
        {
            // layout checkbox
            if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox)
            {
                widget->State.MinimumSize = m_Style.get_font_size();
                widget->State.MaximumSize = widget->State.MinimumSize;

                widget->State.BoundingBox = gs_2dboxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + widget->State.MaximumSize);
            }
            // layout radio button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton)
            {
                widget->State.MinimumSize = m_Style.get_font_size();
                widget->State.MaximumSize = widget->State.MinimumSize;

                widget->State.BoundingBox = gs_2dboxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + widget->State.MaximumSize);
            }
            // layout slider button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton)
            {
                widget->State.MinimumSize = gs_vec2f(m_Style.get_font_size() * 2.f, m_Style.get_font_size());
                widget->State.MaximumSize = widget->State.MinimumSize;

                widget->State.BoundingBox = gs_2dboxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + widget->State.MaximumSize);
            }
        }

        end_node<ImmediateUserInterfaceCheckButton>();
        
        return (_Checked != nullptr && *_Checked);
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::label(const std::string& _ID, const std::string& _Text)
{
    struct ImmediateUserInterfaceLabel : public ImmediateUserInterfaceNode
    {
    public:
        ImmediateUserInterfaceLabel(const std::string& _Name): ImmediateUserInterfaceNode(_Name){}
        virtual ~ImmediateUserInterfaceLabel(){}
        virtual void layout(ImmediateUserInterfaceContextLayer*) override{}
    };

    if(begin_node<ImmediateUserInterfaceLabel>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        // setup
        ImmediateUserInterfaceLabel* button = get_rendering_stack_top<ImmediateUserInterfaceLabel>();

        // render
        {
            m_Renderer->push_clip_box(button->get_clipping_box(this));

            int depth = button->Cache.Depth;

            // title
            gs_2dboxf textBoundingBox = m_Renderer->calculate_bounding_box(
                _Text.begin(),
                _Text.end(),
                m_Style.get_font_size(), m_Style.get_current_font());

            m_Renderer->push_text(
                button->State.BoundingBox.center() - textBoundingBox.size() * 0.5f,
                _Text.begin(),
                _Text.end(),
                m_Style.get_font_size(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                m_Renderer->calculate_transform_matrix((float)depth++));

            m_Renderer->pop_clip_box();
        }

        // calculate geometry
        {
            gs_vec2f size = m_Renderer->calculate_bounding_box(
                _Text.begin(),
                _Text.end(),
                m_Style.get_font_size(), m_Style.get_current_font()).size() + m_Style.get_font_size() * 0.25f;

            button->State.MaximumSize = size;
            button->State.BoundingBox = gs_2dboxf(
                button->State.BoundingBox.Min,
                button->State.BoundingBox.Min + gs_clamp(size, button->State.MinimumSize, button->State.MaximumSize));
        }

        end_node<ImmediateUserInterfaceLabel>();
    }
}

void ImmediateUserInterfaceContextLayer::input_string(
    const std::string&                               _ID,
    std::string&                                     _Text,
    const ImmediateUserInterfaceInputStringSettings& _InputSettings,
    const ImmediateUserInterfaceNodeSettings&        _NodeSettings,
    bool                                           (*_InputTextFilter)(const std::string&))
{
    // nested types
    struct ImmediateUserInterfaceStringContent : public ImmediateUserInterfaceNode
    {
    public:
        ImmediateUserInterfaceStringContent(const std::string& _Name): ImmediateUserInterfaceNode(_Name){}
        virtual ~ImmediateUserInterfaceStringContent(){}
        virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override{}

        static int move_cursor_left(const int& _Cursor, std::string& _Text)
        {
            if(_Text.empty()) return 0;

            auto iterator = _Text.begin() + _Cursor;
            
            if(iterator != _Text.begin())
                Frenchie::Core::String::utf8_prior(iterator);
            
            return gs_clamp((int)(iterator - _Text.begin()), 0, gs_max((int)_Text.size(), 0));
        }

        static int move_cursor_right(const int& _Cursor, std::string& _Text)
        {
            if(_Text.empty()) return 0;

            auto iterator = _Text.begin() + _Cursor;

            if(iterator != _Text.end())
                Frenchie::Core::String::utf8_next(iterator);
            
            return gs_clamp((int)(iterator - _Text.begin()), 0, gs_max((int)_Text.size(), 0));
        }

        static int move_cursor_up(const int& _Cursor, std::string& _Text)
        {
            int SymbolsCountTillLineStart = 0;

            // move backward to find out how many symbols there are till the line start
            {
                auto iterator = _Text.begin() + _Cursor;

                while (iterator > _Text.begin())
                {
                    SymbolsCountTillLineStart++;
                    if(Frenchie::Core::String::utf8_prior(iterator) == '\n') break;
                }
            }

            {
                // move backward
                auto iterator = _Text.begin() + _Cursor;

                while (iterator > _Text.begin())
                {
                    if(Frenchie::Core::String::utf8_prior(iterator) == '\n') break;
                }

                while (iterator > _Text.begin())
                {
                    if(Frenchie::Core::String::utf8_prior(iterator) == '\n') break;
                }

                // move forward
                while (iterator < _Text.end() && SymbolsCountTillLineStart > 0)
                {
                    Frenchie::Core::String::utf8_next(iterator);
                    SymbolsCountTillLineStart--;
                }

                return (int)(iterator - _Text.begin());
            }
        }

        static int move_cursor_down(const int& _Cursor, std::string& _Text)
        {
            int SymbolsCountTillLineStart = 0;

            // move backward to find out how many symbols there are till the line start
            {
                auto iterator = _Text.begin() + _Cursor;

                while (iterator > _Text.begin())
                {
                    if(Frenchie::Core::String::utf8_prior(iterator) == '\n') break;
                    SymbolsCountTillLineStart++;
                }
            }

            {
                // move forward
                auto iterator = _Text.begin() + _Cursor;

                while (iterator < _Text.end())
                {
                    if(Frenchie::Core::String::utf8_next(iterator) == '\n') break;
                }

                // move forward
                while (iterator < _Text.end() && SymbolsCountTillLineStart > 0)
                {
                    if(*iterator == '\n') break;
                    Frenchie::Core::String::utf8_next(iterator);
                    SymbolsCountTillLineStart--;
                }

                return (int)(iterator - _Text.begin());
            }
        }

        int                                            Utf8LeftCursorPosition  = 0;
        int                                            Utf8RightCursorPosition = 0;
        std::chrono::high_resolution_clock::time_point CursorAnimtionTimer;
        std::chrono::high_resolution_clock::time_point CursorMovementTimer;

        struct ImmediateUserInterfaceInputwidgetTextRenderingData
        {
            gs_vec2f                            CursorPosition;
            gs_2dboxf                           TextBoundingBox;
            Frenchie::Core::Optional<gs_2dboxf> HoveredSymbolBoundingBox;
            Frenchie::Core::Optional<int>       HoveredSymbolUtf8CursorPosition;
        } TextRenderingData;
    };

    struct ImmediateUserInterfaceStringScrollArea : public ImmediateUserInterfaceScrollArea
    {
    public:
        ImmediateUserInterfaceStringScrollArea(const std::string& _Name) : ImmediateUserInterfaceScrollArea(_Name)
        {
            State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 128.f));
        }

        virtual ~ImmediateUserInterfaceStringScrollArea(){}

        virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

            ImmediateUserInterfaceMenu* menu =
                _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceMenu>(this);

            if(_Context->m_Hierarchy.get_parent(menu)) return;

            // outline
            _Context->m_Renderer->push_rectangle_rounded_filled(
                State.BoundingBox.Min,
                State.BoundingBox.Max,
                _Context->m_Style.get_frames_radius(),
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuOutline),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
        }
    };

    // auxiliary lambdas
    auto internalFilter = [_InputTextFilter, _InputSettings](const std::string& _Input)->bool
    {
        // internal filter first
        if((_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoMultiline))
        {
            for(auto& symbol : _Input)
            {
                if(symbol == '\n')
                    return false;
            }
        }

        // 
        if(_InputTextFilter != nullptr && !_InputTextFilter(_Input))
            return false;

        return true;
    };

    auto symbolChanger = [_InputSettings](const unsigned int& _Symbol)->unsigned int
    {
        return (_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_InsertPassword) && 
                (_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoMultiline) ? '*' : _Symbol;
    };

    // main code
    int scrollAreaSettings = _NodeSettings;
    scrollAreaSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment;
    scrollAreaSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment;
    scrollAreaSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable;
    scrollAreaSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;

    int scrollAreaContentSettings = scrollAreaSettings;
    scrollAreaContentSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent;

    if(begin_node<ImmediateUserInterfaceStringScrollArea>(std::string(_ID).append("/ScrollArea"),
        scrollAreaSettings))
    {
        ImmediateUserInterfaceStringScrollArea* scrollArea =
            get_rendering_stack_top<ImmediateUserInterfaceStringScrollArea>();

        if(begin_node<ImmediateUserInterfaceStringContent>(
            _ID,
            scrollAreaContentSettings))
        {
            ImmediateUserInterfaceStringContent* widget =
                get_rendering_stack_top<ImmediateUserInterfaceStringContent>();

            widget->TextRenderingData.CursorPosition  = widget->State.BoundingBox.Min;
            widget->TextRenderingData.TextBoundingBox = gs_2dboxf(widget->State.BoundingBox.Min, widget->State.BoundingBox.Min);
            widget->TextRenderingData.HoveredSymbolUtf8CursorPosition.reset();

            if(m_Hierarchy.get_parent(widget)->State.Selected)
            {
                widget->State.Selected                         = true;
                m_Hierarchy.get_parent(widget)->State.Selected = false;
            }

            // render
            {
                m_Renderer->push_clip_box(widget->get_clipping_box(this));

                int depth = widget->Cache.Depth;

                // render text
                if(_Text.empty())
                {
                    widget->TextRenderingData.HoveredSymbolBoundingBox        = widget->TextRenderingData.TextBoundingBox;
                    widget->TextRenderingData.HoveredSymbolUtf8CursorPosition = 0;
                }

                m_Renderer->push_text(
                    widget->State.BoundingBox.Min + gs_max(m_Style.get_frames_radius() * 0.5f, 4.f),
                    _Text.begin(),
                    _Text.end(),
                    m_Style.get_font_size(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_current_font(), false,
                    RenderingQueue::DefaultSymbolProcessor(),
                    symbolChanger);

                // render selection bounding box
                {
                    m_Renderer->push_text(
                        widget->State.BoundingBox.Min + gs_max(m_Style.get_frames_radius() * 0.5f, 4.f),
                        _Text.begin(),
                        _Text.end(),
                        m_Style.get_font_size(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_current_font(),
                        true,
                        [this, widget, &depth](
                            const gs_2dboxf&    _CurrentSymbolBoundingBox,
                            const gs_vec2f&     _CursorPosition,
                            const int&          _Utf8IteratorPosition,
                            const unsigned int& _Symbol)
                        {
                            // calculate text bounding box
                            widget->TextRenderingData.TextBoundingBox = gs_2dboxf(
                                widget->TextRenderingData.TextBoundingBox.Min,
                                _CurrentSymbolBoundingBox.Min,
                                widget->TextRenderingData.TextBoundingBox.Max,
                                _CurrentSymbolBoundingBox.Max);

                            // calculate mouse hovered symbol bounding box
                            float scale  = m_Style.get_current_font().get_scale(m_Style.get_font_size());
                            float offset = m_Style.get_current_font().get_offset(m_Style.get_font_size());

                            if(gs_2dboxf(
                                _CursorPosition - gs_vec2f(4.f, offset * 0.5f),
                                _CursorPosition + gs_vec2f(4.f, offset * 0.5f) + _CurrentSymbolBoundingBox.size()).contains(m_Renderer->get_cursor_postion()))
                            {
                                widget->TextRenderingData.HoveredSymbolBoundingBox        = _CurrentSymbolBoundingBox;
                                widget->TextRenderingData.HoveredSymbolUtf8CursorPosition = _Utf8IteratorPosition;
                            }

                            // calculate cursor geometrical position
                            if(widget->Utf8LeftCursorPosition == _Utf8IteratorPosition)
                                widget->TextRenderingData.CursorPosition = _CursorPosition;

                            if(_Utf8IteratorPosition >= widget->Utf8LeftCursorPosition   &&
                                _Utf8IteratorPosition <= widget->Utf8RightCursorPosition &&
                                (widget->Utf8LeftCursorPosition != widget->Utf8RightCursorPosition))
                            {
                                float glyphAdvance = m_Style.get_current_font().contains_glyph(_Symbol) ? m_Style.get_current_font().retrieve_glyph(_Symbol).Advance * scale : 0.f;

                                m_Renderer->push_rectangle_filled(
                                    _CursorPosition - gs_vec2f(gs_abs(_CurrentSymbolBoundingBox.size().x - glyphAdvance), 0.f),
                                    _CursorPosition + gs_vec2f(gs_abs(_CurrentSymbolBoundingBox.size().x - glyphAdvance), 0.f) + gs_vec2f(_CurrentSymbolBoundingBox.size().x, m_Style.get_font_size()),
                                    gs_rgba_color(
                                        gs_rgba_color_get_r(m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                                        gs_rgba_color_get_g(m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                                        gs_rgba_color_get_b(m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                                        200),
                                    m_Renderer->calculate_transform_matrix((float)depth++));
                            }
                        },
                        symbolChanger);
                }

                // render hovered symbol bounding box
                if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
                    widget->TextRenderingData.HoveredSymbolBoundingBox.has_value())
                {
                    m_Renderer->push_rectangle_filled(
                        widget->TextRenderingData.HoveredSymbolBoundingBox.value().Min,
                        widget->TextRenderingData.HoveredSymbolBoundingBox.value().Max,
                        gs_rgba_color(
                            gs_rgba_color_get_r(m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                            gs_rgba_color_get_g(m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                            gs_rgba_color_get_b(m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                            200),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }

                // render cursor
                if(
                    widget->State.Selected                                                                                            &&
                    !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoInput) &&
                    gs_vector_length(widget->TextRenderingData.CursorPosition) > 0.f)
                {
                    if(widget->CursorAnimtionTimer.time_since_epoch().count() <= 0)
                    {
                        widget->CursorAnimtionTimer = Frenchie::Core::tic();
                    }
                    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorAnimtionTimer, Frenchie::Core::tic()) > 300)
                    {
                        if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorAnimtionTimer, Frenchie::Core::tic()) < 700)
                        {
                            m_Renderer->push_rectangle_filled(
                                widget->TextRenderingData.CursorPosition,
                                widget->TextRenderingData.CursorPosition + gs_vec2f(4.f, m_Style.get_font_size()),
                                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                                m_Renderer->calculate_transform_matrix((float)depth++));
                        }
                        else
                        {
                            widget->CursorAnimtionTimer = std::chrono::steady_clock::time_point();
                        }
                    }
                }

                m_Renderer->pop_clip_box();
            }

            // process events
            {
                // adjust scrollbar
                if(widget->State.Selected && m_Input.is_mouse_button_hold() &&
                    !m_Input.is_mouse_button_pressed()                      &&
                    !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoScrollWhenDrag))
                {
                    if(scrollArea != nullptr && scrollArea->HorizontalScrollBar != nullptr && (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                        scrollArea->HorizontalScrollBar->set_scroll_offset(gs_vector_normalize(m_Input.get_cusor_drag_delta()) * 4.f);
                    if(scrollArea != nullptr && scrollArea->VerticalScrollBar != nullptr && (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                        scrollArea->VerticalScrollBar->set_scroll_offset(gs_vector_normalize(m_Input.get_cusor_drag_delta()) * 4.f);
                }

                if(widget->State.Selected)
                {
                    const int cursorMovementInterval = 80; // TODO: this MUST BE a setting !!!

                    auto adjust_scroll_bar_position  = [this, widget, scrollArea]()
                    {
                        // move scroll bar if the text is behind visible area
                        if(!m_Hierarchy.get_parent(widget)->State.BoundingBox.contains(widget->TextRenderingData.CursorPosition))
                        {
                            // adjust horizontal scrollbar
                            if(scrollArea != nullptr && scrollArea->HorizontalScrollBar != nullptr)
                            {
                                if(widget->TextRenderingData.CursorPosition.x > m_Hierarchy.get_parent(widget)->Cache.BoundingBox.Max.x)
                                    scrollArea->HorizontalScrollBar->set_scroll_offset((widget->TextRenderingData.CursorPosition.x - m_Hierarchy.get_parent(widget)->State.BoundingBox.Max.x) + m_Style.get_font_size());
                                if(widget->TextRenderingData.CursorPosition.x < m_Hierarchy.get_parent(widget)->State.BoundingBox.Min.x)
                                    scrollArea->HorizontalScrollBar->set_scroll_offset((widget->TextRenderingData.CursorPosition.x - m_Hierarchy.get_parent(widget)->State.BoundingBox.Min.x) - m_Style.get_font_size());
                            }

                            // adjust vertical scrollbar position
                            if(scrollArea != nullptr && scrollArea->VerticalScrollBar != nullptr)
                            {
                                if(widget->TextRenderingData.CursorPosition.y > m_Hierarchy.get_parent(widget)->State.BoundingBox.Max.y)
                                    scrollArea->VerticalScrollBar->set_scroll_offset((widget->TextRenderingData.CursorPosition.y - m_Hierarchy.get_parent(widget)->State.BoundingBox.Max.y) + m_Style.get_font_size());
                                if(widget->TextRenderingData.CursorPosition.y < m_Hierarchy.get_parent(widget)->State.BoundingBox.Min.y)
                                    scrollArea->VerticalScrollBar->set_scroll_offset((widget->TextRenderingData.CursorPosition.y - m_Hierarchy.get_parent(widget)->State.BoundingBox.Min.y) - m_Style.get_font_size());
                            }
                        }
                    };

                    // move cursor left
                    if(m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow) ||
                        m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow))
                    {
                        if(m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow))
                        {
                            widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                        }
                        else
                        {
                            if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                            {
                                widget->CursorMovementTimer = Frenchie::Core::tic();
                            }
                            else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                            {
                                widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                widget->CursorMovementTimer     = std::chrono::steady_clock::time_point();
                            }
                        }

                        adjust_scroll_bar_position();
                    }

                    // move cursor right
                    else if(m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ||
                            m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow))
                    {
                        if(m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow))
                        {
                            widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_right(widget->Utf8LeftCursorPosition, _Text);
                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                        }
                        else
                        {
                            if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                            {
                                widget->CursorMovementTimer = Frenchie::Core::tic();
                            }
                            else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                            {
                                widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_right(widget->Utf8LeftCursorPosition, _Text);
                                widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                widget->CursorMovementTimer     = std::chrono::steady_clock::time_point();
                            }
                        }

                        adjust_scroll_bar_position();
                    }

                    // move cursor up
                    else if(m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow) ||
                            m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow))
                    {
                        if(m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow))
                        {
                            widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_up(widget->Utf8LeftCursorPosition, _Text);
                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                        }
                        else
                        {
                            if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                            {
                                widget->CursorMovementTimer = Frenchie::Core::tic();
                            }
                            else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                            {
                                widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_up(widget->Utf8LeftCursorPosition, _Text);
                                widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                widget->CursorMovementTimer     = std::chrono::steady_clock::time_point();
                            }
                        }

                        adjust_scroll_bar_position();
                    }

                    // move cursor down
                    else if(m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow) ||
                            m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow))
                    {
                        if(m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow))
                        {
                            widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_down(widget->Utf8LeftCursorPosition, _Text);
                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                        }
                        else
                        {
                            if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                            {
                                widget->CursorMovementTimer = Frenchie::Core::tic();
                            }
                            else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                            {
                                widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_down(widget->Utf8LeftCursorPosition, _Text);
                                widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                widget->CursorMovementTimer     = std::chrono::steady_clock::time_point();
                            }
                        }

                        adjust_scroll_bar_position();
                    }

                    // set left cursor position
                    else if(m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Escape)     ||
                            m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow)    ||
                            m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow)  ||
                            m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ||
                            m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow)  ||
                            m_Input.is_mouse_button_pressed())
                    {
                        if(widget->TextRenderingData.HoveredSymbolUtf8CursorPosition.has_value() && m_Input.is_mouse_button_pressed())
                        {
                            widget->Utf8LeftCursorPosition  = widget->TextRenderingData.HoveredSymbolUtf8CursorPosition.value();
                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                        }

                        widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                    }

                    // set right cursor position
                    else if(                        
                        !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoSelection) &&
                        widget->TextRenderingData.HoveredSymbolUtf8CursorPosition.has_value()                                                 &&
                        m_Input.is_mouse_button_down())
                    {
                        if(widget->TextRenderingData.HoveredSymbolUtf8CursorPosition.value() > widget->Utf8LeftCursorPosition)
                            widget->Utf8RightCursorPosition = widget->TextRenderingData.HoveredSymbolUtf8CursorPosition.value();
                        else
                            widget->Utf8LeftCursorPosition = widget->TextRenderingData.HoveredSymbolUtf8CursorPosition.value();
                    }

                    // select all
                    else if(
                            !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoSelection) &&
                            m_Input.has_modifier(ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_Ctrl)               &&
                            m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_A))
                    {
                        widget->Utf8LeftCursorPosition  = 0;
                        widget->Utf8RightCursorPosition = (int)_Text.size();
                    }

                    // modifications

                    // insert text
                    else if(
                        !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoInput) &&
                        m_Input.has_input_text())
                    {
                        // remove selection
                        if(gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) > 0)
                        {
                            int position = widget->Utf8LeftCursorPosition;
                            int count    = gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) + 1;

                            // move cursor
                            for (int i = 0; i < count; i++)
                                widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                            // erase selection
                            _Text.erase(position, count);
                        }

                        // insert text after selection
                        if(internalFilter(m_Input.get_input_text()))
                        {
                            _Text.insert(widget->Utf8LeftCursorPosition, m_Input.get_input_text());
                            widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_right(widget->Utf8LeftCursorPosition, _Text);
                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                        }

                        adjust_scroll_bar_position();
                    }

                    // remove text
                    else if(
                            !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoInput) &&

                            m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace) ||
                            m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace))
                    {
                        // remove selection
                        if(gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) > 0)
                        {
                            int position = widget->Utf8LeftCursorPosition;
                            int count    = gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) + 1;

                            // move cursor
                            for (int i = 0; i < count; i++)
                                widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                            // erase selection
                            _Text.erase(position, count);
                        }
                        // remove single symbol
                        else
                        {
                            if(m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace))
                            {
                                int previousCursorPosition      = widget->Utf8LeftCursorPosition;
                                widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                                if(previousCursorPosition - widget->Utf8LeftCursorPosition > 0)
                                    _Text.erase(widget->Utf8LeftCursorPosition, previousCursorPosition - widget->Utf8LeftCursorPosition);
                            }
                            else
                            {
                                if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                                {
                                    widget->CursorMovementTimer = Frenchie::Core::tic();
                                }
                                else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                                {
                                    int previousCursorPosition      = widget->Utf8LeftCursorPosition;
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                                    if(previousCursorPosition - widget->Utf8LeftCursorPosition > 0)
                                        _Text.erase(widget->Utf8LeftCursorPosition, previousCursorPosition - widget->Utf8LeftCursorPosition);

                                    widget->CursorMovementTimer = std::chrono::steady_clock::time_point();
                                }
                            }
                        }

                        adjust_scroll_bar_position();
                    }

                    // copy text
                    if( 
                        !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoClipboard) &&
                         m_Input.has_modifier(ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_Ctrl)              &&
                         m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_C)) // Ctrl + C
                    {
                        if(gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition))
                        {
                            m_Input.set_clipboard_text(
                                std::string(
                                    _Text.begin() + widget->Utf8LeftCursorPosition,
                                    _Text.begin() + gs_clamp(widget->Utf8RightCursorPosition + 1, 0, gs_max(0, (int)_Text.size() - 1))));
                        }
                    }

                    // paste text
                    if( 
                        !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoInput)     &&
                        !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoClipboard) &&
                        m_Input.has_modifier(ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_Ctrl)               &&
                        m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_V)) // Ctrl + V
                    {
                        if(m_Input.has_clipboard_text())
                        {
                            // insert text from clipboard
                            std::string clipboardText = m_Input.get_clipboard_text();

                            _Text.insert(widget->Utf8LeftCursorPosition, clipboardText);
                            
                            for(int i = 0; i < (int)clipboardText.size(); i++)
                                widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceStringContent::move_cursor_right(widget->Utf8LeftCursorPosition, _Text);
                            
                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                            adjust_scroll_bar_position();
                        }
                    }
                }
            }

            // calculate geometry
            {
                if((_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoMultiline))
                {
                    widget->State.MaximumSize = gs_vec2f(
                        gs_max(widget->TextRenderingData.TextBoundingBox.size().x, m_Style.get_font_size()) + m_Style.get_font_size(),
                        m_Style.get_font_size());
                }
                else
                {
                    widget->State.MaximumSize = gs_vec2f(
                        gs_max(widget->TextRenderingData.TextBoundingBox.size().x, m_Style.get_font_size()) + m_Style.get_font_size(),
                        gs_max(widget->TextRenderingData.TextBoundingBox.size().y, m_Style.get_font_size()));
                }
                
                widget->State.MinimumSize = widget->State.MaximumSize;
                
                widget->State.BoundingBox = gs_2dboxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + gs_clamp(widget->TextRenderingData.TextBoundingBox.size(), widget->State.MinimumSize, widget->State.MaximumSize));
            }

            end_node<ImmediateUserInterfaceStringContent>();
        }

        end_node<ImmediateUserInterfaceStringScrollArea>();
    }
}

void ImmediateUserInterfaceContextLayer::input_string_multiline(
    const std::string&                               _ID,
    std::string&                                     _Text,
    const ImmediateUserInterfaceInputStringSettings& _InputSettings,
    bool                                           (*_InputTextFilter)(const std::string&))
{
    input_string(
        _ID,
        _Text,
        (_InputSettings & ~(
            ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoMultiline |
            ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_InsertPassword)),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar             |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar,
        _InputTextFilter);
}

void ImmediateUserInterfaceContextLayer::input_string_singleline(
    const std::string&                               _ID,
    std::string&                                     _Text,
    const ImmediateUserInterfaceInputStringSettings& _InputSettings,
    bool                                           (*_InputTextFilter)(const std::string&))
{
    input_string(
        _ID,
        _Text,
        (int)(_InputSettings | ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoMultiline),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically   |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar       |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar       |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar  |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar,
        _InputTextFilter);
}


//------------------------------------------------------------------------------------------
// color picker
//------------------------------------------------------------------------------------------

void ImmediateUserInterfaceContextLayer::color_picker(const std::string& _ID)
{
    if(begin_node<ImmediateUserInterfaceColorPicker>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        ImmediateUserInterfaceColorPicker* colorPicker =
            get_rendering_stack_top<ImmediateUserInterfaceColorPicker>();

        if(begin_horizontal_stack(std::string(_ID).append("/ColorPickers")))
        {
            // gradient color modifier
            if(begin_node<ImmediateUserInterfaceColorPickerGradientColorModifier>(
                std::string(_ID).append("/ColorPickers/GradientColorModifier"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                colorPicker->GradientSurfaceColorModifier =
                    get_rendering_stack_top<ImmediateUserInterfaceColorPickerGradientColorModifier>();

                if(colorPicker->GradientColorSelector != nullptr)
                    colorPicker->GradientSurfaceColorModifier->BaseColor = colorPicker->GradientColorSelector->Color;

                end_node<ImmediateUserInterfaceColorPickerGradientColorModifier>();
            }

            // spacer
            if(begin_node<ImmediateUserInterfaceNode>(
                std::string(_ID).append("/ColorPickers/Spacer"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                get_rendering_stack_top<ImmediateUserInterfaceNode>()->State.MinimumSize = gs_vec2f(32.f, 64.f);
                get_rendering_stack_top<ImmediateUserInterfaceNode>()->State.MaximumSize = gs_vec2f(32.f, 32.f);

                end_node<ImmediateUserInterfaceNode>();
            }

            // gradient color selector
            if(begin_node<ImmediateUserInterfaceColorPickerGradientColorSelector>(
                std::string(_ID).append("/ColorPickers/GradientColorSelector"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                colorPicker->GradientColorSelector =
                    get_rendering_stack_top<ImmediateUserInterfaceColorPickerGradientColorSelector>();

                colorPicker->GradientColorSelector->State.MinimumSize = gs_vec2f(64.f, colorPicker->GradientColorSelector->State.MinimumSize.y);
                colorPicker->GradientColorSelector->State.MaximumSize = gs_vec2f(64.f, colorPicker->GradientColorSelector->State.MaximumSize.y);

                end_node<ImmediateUserInterfaceColorPickerGradientColorSelector>();
            }

            end_horizontal_stack();
        }

        end_node<ImmediateUserInterfaceColorPicker>();
    }
}
//------------------------------------------------------------------------------------------

bool ImmediateUserInterfaceContextLayer::begin_menu(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    ImmediateUserInterfaceMenu*       menu      = nullptr;
    ImmediateUserInterfaceMenuAction* menuItem  = nullptr;
    bool                              hasParent = false;
    bool                              isHovered = false;

    // retrieve controller
    ImmedidateUserInterfaceMenusController* menusController =
        get_controller<ImmedidateUserInterfaceMenusController>();

    if(begin_node<ImmediateUserInterfaceMenu>(std::string(_ID).append("/Menu"), _Settings))
    {
        menu      = get_rendering_stack_top<ImmediateUserInterfaceMenu>();
        hasParent = m_Hierarchy.get_parent(menu) != nullptr;

        if(begin_node<ImmediateUserInterfaceMenuScrollArea>(std::string(_ID).append("/Menu/InternalScrollArea"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults                                     |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar                    |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar                  |
            (hasParent ? ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically : 0) |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally                 |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment        |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment         |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment       |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop                  |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft))
        {
            menu->InternalScrollArea = get_rendering_stack_top<ImmediateUserInterfaceScrollArea>();
            menu->InternalScrollArea->State.PlaceInFollow = true;

            // if menu is inside another menu we draw menu item
            if(hasParent)
            {
                if(begin_node<ImmediateUserInterfaceMenuItem>(
                    _ID,
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    menuItem = get_rendering_stack_top<ImmediateUserInterfaceMenuItem>();
                    isHovered = (menuItem->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered);
                    end_node<ImmediateUserInterfaceMenuItem>();
                }
            }

            end_node<ImmediateUserInterfaceMenuScrollArea>();
        }
    }

    // 
    if(menusController != nullptr &&
        std::find(menusController->ActiveMenus.begin(), menusController->ActiveMenus.end(), menu) == menusController->ActiveMenus.end())
    {
        if(isHovered)
            menusController->ActiveMenus.push_back(menu);
    }

    isHovered = (menusController != nullptr && std::find(menusController->ActiveMenus.begin(), menusController->ActiveMenus.end(), menu) != menusController->ActiveMenus.end()) || isHovered;

    if(hasParent)
    {
        if(hasParent && isHovered && menuItem != nullptr)
        {
            if(begin_node<ImmediateUserInterfaceMenuScrollArea>(std::string(_ID).append("/Main/ExternalScrollArea"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent                             |
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar              |
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar            |
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment  |
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment   |
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment |
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
            {
                menu->ExternalScrollArea = get_rendering_stack_top<ImmediateUserInterfaceScrollArea>();

                menu->ExternalScrollArea->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered;

                menu->ExternalScrollArea->State.PlaceInFollow  = true;

                ImmediateUserInterfaceNode* topMost = menuItem;
                ImmediateUserInterfaceNode* parent  = m_Hierarchy.get_parent(menuItem);

                while (parent)
                {
                    topMost = parent;
                    parent  = m_Hierarchy.get_parent(parent);
                }

                menu->ExternalScrollArea->State.RenderingOrder = topMost->State.RenderingOrder + 1;

                // calculate rect
                menu->ExternalScrollArea->State.BoundingBox = gs_2dboxf(
                    gs_vec2f(menuItem->State.BoundingBox.Max.x, menuItem->State.BoundingBox.Min.y),
                    gs_vec2f(menuItem->State.BoundingBox.Max.x, menuItem->State.BoundingBox.Min.y) + menu->ExternalScrollArea->State.BoundingBox.size());

                end_node<ImmediateUserInterfaceMenuScrollArea>();
            }

            return true;
        }

        // do not render children
        end_menu();
        return false;
    }

    return true;
}

void ImmediateUserInterfaceContextLayer::end_menu()
{
    end_node<ImmediateUserInterfaceMenu>();
}

bool ImmediateUserInterfaceContextLayer::begin_window(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, bool* _Opened)
{
    return begin_node<ImmediateUserInterfaceWindow>(_ID, _Settings, _Opened);
}

void ImmediateUserInterfaceContextLayer::end_window()
{
    end_node<ImmediateUserInterfaceWindow>();
}

void ImmediateUserInterfaceContextLayer::next_line()
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextLine = controller->NextLine.has_value() ? controller->NextLine.value() + 1 : 1;
}

void ImmediateUserInterfaceContextLayer::next_size(const gs_vec2f& _Value)
{
    next_minimum_size(_Value);
    next_maximum_size(_Value);
}

void ImmediateUserInterfaceContextLayer::next_minimum_size(const gs_vec2f& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextMinimumSize = _Value;
}

void ImmediateUserInterfaceContextLayer::next_maximum_size(const gs_vec2f& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextMaximumSize = _Value;
}

void ImmediateUserInterfaceContextLayer::next_position(const gs_vec2f& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextPosition = _Value;
}

void ImmediateUserInterfaceContextLayer::next_content_padding(const gs_vec2f& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextContentPadding = _Value;
}

std::string ImmediateUserInterfaceContextLayer::next_id(const std::string& _Name, const std::string& _Hash)
{
    ImmediateUserInterfaceNode* top = get_rendering_stack_top();

    if(top == nullptr)
        return !_Hash.empty() ? std::string(_Name).append("###").append(_Hash) : _Name;
        
    return !_Hash.empty() ?
                std::string(_Name).append("###").append(top->Hash).append("/").append(_Hash) :
                    std::string(top->Hash).append("/").append(_Name);
}