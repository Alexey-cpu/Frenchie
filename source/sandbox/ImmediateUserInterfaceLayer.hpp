#pragma once

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationLayerRenderingQueue.hpp>

// STL
#include <type_traits>
#include <functional>
#include <memory>
#include <chrono>
#include <stack>
#include <map>
#include <set>

namespace Frenchie
{
    namespace Application
    {
        // Events
        enum ImmediateUserInterfaceNodeEvents_ : int
        {
            // sentinel
            ImmediateUserInterfaceNodeEvents_None                 = 0,
            
            // move
            ImmediateUserInterfaceNodeEvents_IsMoved              = 1 << 0, // indicates that node is being moved
            
            // resize
            ImmediateUserInterfaceNodeEvents_IsResizedTop         = 1 << 1, // indicates that node is being resized by dragging top part of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedLeft        = 1 << 2, // indicates that node is being resized by dragging left part of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedRight       = 1 << 3, // indicates that node is being resized by dragging right part of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedBottom      = 1 << 4, // indicates that node is being resized by dragging bottom part of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedTopLeft     = 1 << 5, // indicates that node is being resized by dragging top left corner of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedTopRight    = 1 << 6, // indicates that node is being resized by dragging top right corner of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft  = 1 << 7, // indicates that node is being resized by dragging bottom left corner of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedBottomRight = 1 << 8, // indicates that node is being resized by dragging bottom right corner of it's bounding box

            // custom event
            ImmediateUserInterfaceNodeEvents_Custom               = 1 << 9, // is used for user defined events
        };

        // Colors
        enum ImmediateUserInterfaceNodeColors_ : int
        {
            ImmediateUserInterfaceNodeColors_Begin            = 0,

            // layouts/windows e.t.c
            ImmediateUserInterfaceNodeColors_ParentBackground = ImmediateUserInterfaceNodeColors_Begin, // parent UI elements background color
            ImmediateUserInterfaceNodeColors_ParentBackgroundHovered,                                   // hovered parent UI elements background color
            ImmediateUserInterfaceNodeColors_ChildBackground,                                           // child UI elements background color
            ImmediateUserInterfaceNodeColors_ChildBackgroundHovered,                                    // hovered child UI elements background color

            // buttons
            ImmediateUserInterfaceNodeColors_ButtonOutline,                                             // push button, check button, radio button, slider button e.t.c outline color
            ImmediateUserInterfaceNodeColors_ButtonBackground,                                          // push button, check button, radio button, slider button e.t.c background color
            ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered,                                   // hovered push button, check button, radio button, slider button e.t.c background color
            ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed,                                   // pressed push button, check button, radio button, slider button e.t.c background color

            // scrollbar
            ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground,                                 // scroll bar slider background color
            ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered,                          // hovered scroll bar slider background color

            // menus
            ImmediateUserInterfaceNodeColors_MenuOutline,                                               // menu outline
            ImmediateUserInterfaceNodeColors_MenuActionBackground,                                      // menu action background
            ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered,                               // hovered menu action background
            ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed,                               // hovered menu action background

            // gizmos
            ImmediateUserInterfaceNodeColors_Gizmos,                                                    // gizmos background
            ImmediateUserInterfaceNodeColors_GizmosHovered,                                             // hovered gizmos background

            // text
            ImmediateUserInterfaceNodeColors_Text,
            
            ImmediateUserInterfaceNodeColors_End
        };

        // Settings
        enum ImmediateUserInterfaceNodeSettings_ : int
        {
            // sentinel
            ImmediateUserInterfaceNodeSettings_None                                   = 0,

            // modifications
            ImmediateUserInterfaceNodeSettings_Movable                                = 1 << 0, // make node movable
            ImmediateUserInterfaceNodeSettings_Resizable                              = 1 << 1, // make node resizable
            ImmediateUserInterfaceNodeSettings_NullParent                             = 1 << 2, // make node ignore it's parent within hierarchy

            // content alignment
            ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop            = 1 << 3, // content is aligned top on vertical axis
            ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter         = 1 << 4, // content is aligned center on vertical axis
            ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom         = 1 << 5, // content is aligned bottom on vertical axis
            ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft         = 1 << 6, // content is aligned left on horizontal axis
            ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter       = 1 << 7, // content is aligned center on horizontal axis
            ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight        = 1 << 8, // content is aligned right on horizontal axis

            // scrollbars
            ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar                 = 1 << 9,  // vertical scrollbar will always be disabled
            ImmediateUserInterfaceNodeSettings_AlwaysVerticalScrollBar                = 1 << 10, // vertical scrollbar will always be enabled
            ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar              = 1 << 11, // vertical scrollbar will be enabled when needed
            ImmediateUserInterfaceNodeSettings_NeverHorizontalScrollBar               = 1 << 12, // horizontal scrollbar will always be disabled
            ImmediateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar              = 1 << 13, // horizontal scrollbar will always be enabled
            ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar            = 1 << 14, // horizontal scrollbar will be enabled when needed
            ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically             = 1 << 15, // node with scrollarea will be resized to it's contents vertically
            ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally           = 1 << 16, // node with scrollarea will be resized to it's contents horizontally

            ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment   = 1 << 17, // vertical scroll bar position will be adjusted by up/down keyboard arrows
            ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment = 1 << 18, // horizontal scroll bar position will be adjusted by left/right keyboard arrows
            ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment  = 1 << 19, // vertical scroll bar position will be adjusted by a mouse wheel

            ImmediateUserInterfaceNodeSettings_InvisibleVerticalScrollBar             = 1 << 20, // vertical scrollbar will be invisible
            ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar           = 1 << 21, // horizontal scrollbar will be invisible

            // ordering
            ImmediateUserInterfaceNodeSettings_ManualRenderingOrderSetup              = 1 << 22, // declares that the rendering order of the node is setup manually so it won't be changed by focus pass and other events

            ImmediateUserInterfaceNodeSettings_AllowedModificationsDefaults           = 
                ImmediateUserInterfaceNodeSettings_Movable |
                ImmediateUserInterfaceNodeSettings_Resizable,

            ImmediateUserInterfaceNodeSettings_ScrollAreaDefaults                     =
                  ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar
                | ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
                | ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment
                | ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment
                | ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment,

            ImmediateUserInterfaceNodeSettings_ContentAlignmentDefaults =
                  ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
                | ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter,

            ImmediateUserInterfaceNodeSettings_Defaults =
                  ImmediateUserInterfaceNodeSettings_AllowedModificationsDefaults
                | ImmediateUserInterfaceNodeSettings_ScrollAreaDefaults
                | ImmediateUserInterfaceNodeSettings_ContentAlignmentDefaults,
        };

        enum ImmediateUserInterfaceLabelSettings_ : int
        {
            ImmediateUserInterfaceLabelSettings_None        = 0,
            ImmediateUserInterfaceLabelSettings_RenderFrame = 1 << 0,
        };

        enum ImmediateUserInterfaceTreeNodeSettings_ : int
        {
            ImmediateUserInterfaceTreeNodeSettings_OpenOnClick           = 1 << 0, // tree node opened on mouse click
            ImmediateUserInterfaceTreeNodeSettings_OpenOnDoubleClick     = 1 << 1, // tree node opened on mouse double click
            ImmediateUserInterfaceTreeNodeSettings_RenderConnectionLines = 1 << 2, // tree node siblings connection lines are drawn

            ImmediateUserInterfaceTreeNodeSettings_Defaults              =
                  ImmediateUserInterfaceTreeNodeSettings_OpenOnDoubleClick
                | ImmediateUserInterfaceTreeNodeSettings_RenderConnectionLines
        };

        enum ImmediateUserInterfaceCheckButtonSettings_ : int
        {
            ImmediateUserInterfaceCheckButtonSettings_Checkbox     = 1 << 0, // check button is rendered as checkbox
            ImmediateUserInterfaceCheckButtonSettings_RadioButton  = 1 << 1, // check button is rendered as radio button
            ImmediateUserInterfaceCheckButtonSettings_SliderButton = 1 << 2, // check button is rendered as slider button
            ImmediateUserInterfaceCheckButtonSettings_Checkable    = 1 << 3, // check button changes boolean variable passed to it

            ImmediateUserInterfaceCheckButtonSettings_Defaults     =
                ImmediateUserInterfaceCheckButtonSettings_Checkbox
                | ImmediateUserInterfaceCheckButtonSettings_Checkable,
        };

        enum ImmediateUserInterfaceInputStringSettings_ : int
        {
            ImmediateUserInterfaceInputStringSettings_NoInput           = 1 << 0, // disables input
            ImmediateUserInterfaceInputStringSettings_Password          = 1 << 1, // all input symbols are changed on '*' while rendering
            ImmediateUserInterfaceInputStringSettings_NoClipboard       = 1 << 2, // disables copy/paste
            ImmediateUserInterfaceInputStringSettings_NoSelection       = 1 << 3, // disables selection
            ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter = 1 << 4, // appropriate input string function returns true when enter key is pressed
            ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEdit  = 1 << 5, // appropriate input string function returns true when text is being edited
            ImmediateUserInterfaceInputStringSettings_Defaults          = 0
        };

        enum ImmediateUserInterfaceInputScalarSettings_ : int
        {
            ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter = 1 << 0, // appropriate input scalar function returns true when enter key is pressed
            ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEdit  = 1 << 1, // appropriate input scalar function returns true when text is being edited
            ImmediateUserInterfaceInputScalarSettings_Defaults          = 0
        };

        enum ImmediateUserInterfaceColorPickerSettings_ : int
        {
            ImmediateUserInterfaceColorPickerSettings_None         = 0,      // sentinel
            ImmediateUserInterfaceColorPickerSettings_EditRGB      = 1 << 0, // enables RGB   editor in color picker
            ImmediateUserInterfaceColorPickerSettings_EditHSV      = 1 << 1, // enables HSV   editor in color picker
            ImmediateUserInterfaceColorPickerSettings_EditHSL      = 1 << 2, // enables HSL   editor in color picker
            ImmediateUserInterfaceColorPickerSettings_EditAlpha    = 1 << 3, // enables alpha editor in color picker
            ImmediateUserInterfaceColorPickerSettings_PreviewColor = 1 << 4, // enables color preview image

            ImmediateUserInterfaceColorPickerSettings_Defaults  =
                  ImmediateUserInterfaceColorPickerSettings_EditRGB
                | ImmediateUserInterfaceColorPickerSettings_EditHSV
                | ImmediateUserInterfaceColorPickerSettings_EditHSL
                | ImmediateUserInterfaceColorPickerSettings_EditAlpha
                | ImmediateUserInterfaceColorPickerSettings_PreviewColor
        };

        enum ImmediateUserInterfaceContextSettings_ : int
        {
            // docking
            ImmediateUserInterfaceContextSettings_DisableDocking         = 1 << 0, // disables all docking features
            ImmediateUserInterfaceContextSettings_EnableWindowsDocking   = 1 << 1, // enables windows mutual docking
            ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking = 1 << 2, // enables workspace dock space

            // highlighting
            ImmediateUserInterfaceContextSettings_HighlighHoveredNodes   = 1 << 3, // enables hovered node highligting by a semi-transparent rectangle
        };

        // mouse hover
        enum ImmediateUserInterfaceNodeMouseHover_ : int
        {
            ImmediateUserInterfaceNodeMouseHover_None         = 0,
            ImmediateUserInterfaceNodeMouseHover_MouseLeft    = 1 << 0, // is set when mouse leaves bounding box of previously hovered node
            ImmediateUserInterfaceNodeMouseHover_MouseHovered = 1 << 1, // is set if mouse is within bounding box of a node
            ImmediateUserInterfaceNodeMouseHover_MouseEntered = 1 << 2, // is set when mouse enters bounding box of node
        };

        // windows docking anchors
        enum ImmedidateUserInterfaceDockingAnchor_ : int
        {
            ImmedidateUserInterfaceDockingAnchor_Top    = 1 << 0, // docked window is snapped to the top part of it's docker
            ImmedidateUserInterfaceDockingAnchor_Left   = 1 << 1, // docked window is snapped to the left part of it's docker
            ImmedidateUserInterfaceDockingAnchor_Right  = 1 << 2, // docked window is snapped to the right part of it's docker
            ImmedidateUserInterfaceDockingAnchor_Bottom = 1 << 3, // docked window is snapped to the bottom part of it's docker
            ImmedidateUserInterfaceDockingAnchor_Center = 1 << 4, // docked window is docked as tab to it's docker

            ImmedidateUserInterfaceDockingAnchor_All    =
                  ImmedidateUserInterfaceDockingAnchor_Top
                | ImmedidateUserInterfaceDockingAnchor_Left
                | ImmedidateUserInterfaceDockingAnchor_Right
                | ImmedidateUserInterfaceDockingAnchor_Bottom
                | ImmedidateUserInterfaceDockingAnchor_Center
        };

        // rendering order and rendering layer
        enum ImmedidateUserInterfaceRenderingOrder_ : int
        {
            ImmedidateUserInterfaceRenderingOrder_Begin      = 0,
            ImmedidateUserInterfaceRenderingOrder_Background = ImmedidateUserInterfaceRenderingOrder_Begin,
            ImmedidateUserInterfaceRenderingOrder_Main,
            ImmedidateUserInterfaceRenderingOrder_Focus,
            ImmedidateUserInterfaceRenderingOrder_Modal,
            ImmedidateUserInterfaceRenderingOrder_Popup,
            ImmedidateUserInterfaceRenderingOrder_End,
        };

        enum ImmedidateUserInterfaceRenderingLayer_ : int
        {
            ImmedidateUserInterfaceRenderingLayer_Begin   = 0,
            ImmedidateUserInterfaceRenderingLayer_Main    = ImmedidateUserInterfaceRenderingLayer_Begin,
            ImmedidateUserInterfaceRenderingLayer_Gizmos,
            ImmedidateUserInterfaceRenderingLayer_End,
        };

        typedef int ImmediateUserInterfaceNodeEvents;
        typedef int ImmediateUserInterfaceNodeMouseHover;

        typedef int ImmediateUserInterfaceNodeSettings;
        typedef int ImmediateUserInterfaceLabelSettings;
        typedef int ImmediateUserInterfaceTreeNodeSettings;
        typedef int ImmediateUserInterfaceCheckButtonSettings;
        typedef int ImmediateUserInterfaceInputStringSettings;
        typedef int ImmediateUserInterfaceInputScalarSettings;
        typedef int ImmediateUserInterfaceColorPickerSettings;

        typedef int ImmediateUserInterfaceContextSettings;

        typedef int ImmedidateUserInterfaceDockingAnchor;
        typedef int ImmedidateUserInterfaceRenderingOrder;
        typedef int ImmedidateUserInterfaceRenderingLayer;

        class ImmediateUserInterfaceContextLayer;

        // style and events
        struct ImmedidateUserInterfaceStyle final
        {
            ImmedidateUserInterfaceStyle();
            ~ImmedidateUserInterfaceStyle();

            // getters

            // frames radius
            float get_minimum_frames_radius() const;
            float get_maximum_frames_radius() const;
            float get_frames_radius() const;

            // frames width
            float get_minimum_frames_width() const;
            float get_maximum_frames_width() const;
            float get_frames_width() const;

            // font size
            float get_minimum_font_size() const;
            float get_maximum_font_size() const;
            float get_font_size() const;

            // scrollbar width
            float get_minimum_scrollbar_width() const;
            float get_maximum_scrollbar_width() const;
            float get_scrollbar_width() const;

            // menu pointer size
            float get_popup_menu_pointer_size() const;

            // current font
            ApplicationRenderingBackendFont get_current_font() const;

            // color
            gs_color get_color(const ImmediateUserInterfaceNodeColors_& _Color) const;

        private:

            // infos
            float                            FramesRadius         = 0.f;
            float                            FramesWidth          = 0.f;
            float                            FontSize             = 32.f;
            float                            ScrollBarWidth       = 64.f;
            float                            PopupMenuPointerSize = 32.f;
            std::vector<gs_color>            Colors;
            ApplicationRenderingBackendFont  Font;
        };

        struct ImmedidateUserInterfaceInput final
        {
            ImmedidateUserInterfaceInput(ImmediateUserInterfaceContextLayer* _Context = nullptr);

            // getters
            gs_vec2f    get_cusor_position() const;
            gs_vec2f    get_cusor_drag_delta() const;
            gs_vec2f    get_cusor_scroll_offset() const;
            std::string get_input_text() const;
            std::string get_clipboard_text() const;
            
            // predicates
            bool has_input_text() const;
            bool has_clipboard_text() const;

            // setters
            void set_clipboard_text(const std::string&);

            bool is_mouse_button_down() const;
            bool is_mouse_button_hold() const;
            bool is_mouse_button_pressed() const;
            bool is_mouse_button_released() const;
            bool is_mouse_button_clicked() const;
            bool is_mouse_button_double_clicked() const;

            bool is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button&) const;
            bool is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button&) const;
            bool is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button&) const;
            bool is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button&) const;
            bool is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button&) const;
            bool is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button&) const;

            bool is_key_down() const;
            bool is_key_hold() const;
            bool is_key_pressed() const;
            bool is_key_released() const;
            bool is_key_clicked() const;

            bool is_key_down(const ApplicationPlatformBackendKey::Key&) const;
            bool is_key_hold(const ApplicationPlatformBackendKey::Key&) const;
            bool is_key_pressed(const ApplicationPlatformBackendKey::Key&) const;
            bool is_key_released(const ApplicationPlatformBackendKey::Key&) const;
            bool is_key_clicked(const ApplicationPlatformBackendKey::Key&) const;

            bool has_modifier(const ApplicationPlatformBackendKeyModifier::Modifier&) const;

        private:
            ImmediateUserInterfaceContextLayer* m_Context = nullptr;
        };

        // nodes
        struct ImmediateUserInterfaceNode
        {
            ImmediateUserInterfaceNode(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNode();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context);
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context);
            virtual void measure(ImmediateUserInterfaceContextLayer* _Context);
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context);
            virtual void attach_child(ImmediateUserInterfaceNode* _Child);

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr);
            
            virtual void load_state(ImmediateUserInterfaceContextLayer*);
            virtual void save_state(ImmediateUserInterfaceContextLayer*);

            // getters
            gs_2dboxf get_clipping_box(ImmediateUserInterfaceContextLayer* _Context) const;
            gs_2dboxf get_visible_rect(ImmediateUserInterfaceContextLayer* _Context) const;
            bool is_partially_visible(ImmediateUserInterfaceContextLayer* _Context) const;

            int place_in_follow();

            struct Data
            {
                // rendering
                int                                            Depth                       {0};     // depth along Z-axis
                int                                            SelfThickness               {0};     // thickness of rendered content
                int                                            RenderingIndex              {0};     // index of the node within context rendering list
                int                                            RenderingOrder              {0};     // index of the node while rendering
                int                                            MaximumChildDepth           {0};     // depth of the deepest child
                int                                            MaximumChildThickness       {0};     // thickness of the 'fattest' child
                bool                                           PlaceInFollow               {false}; // shows if the node places it's children in follow along Z-axis
                bool                                           OrderChildrenWhileRendering {false}; // shows if the node sorts  it's children by rendering order index while rendering

                // geometry
                gs_2dboxf                                      BoundingBox                 {gs_2dboxf(gs_vec2f(32.f, 32.f), gs_vec2f(1024.f, 512.f))}; // node bounding box
                gs_vec2f                                       ContentSize                 {gs_vec2f(0.f)};                                            // node contents size
                gs_vec2f                                       MinimumSize                 {gs_vec2f(32.f)};                                           // node minimum size
                gs_vec2f                                       MaximumSize                 {gs_vec2f((float)INT_MAX)};                                 // node maximum size

                // hierarchy
                ImmediateUserInterfaceNode*                    Parent                      {nullptr}; // node hierarchical parent
                ImmediateUserInterfaceNode*                    Scope                       {nullptr}; // node from which scope this node was created

                // visibility
                mutable Frenchie::Core::Optional<gs_2dboxf>    ClippingBox;

                // settings
                ImmediateUserInterfaceNodeSettings             Settings                    {ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable};

                // events
                ImmediateUserInterfaceNodeEvents               Events                      {ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None};
                bool                                           Selected                    {false};

                // layout hints
                int                                            NextLine                    {1  }; // vertical indents count which need to be placed after this node within scrollarea
                float                                          Indent                      {0.f}; // horizontal indents count which  need to be placed after this node within scrollarea

                // mouse hover
                ImmediateUserInterfaceNodeMouseHover           MouseHover                  {ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None};
                std::chrono::high_resolution_clock::time_point MouseEnterTimer;
                std::chrono::high_resolution_clock::time_point MouseLeaveTimer;
            };

            Data State;
            Data Cache;
            bool Loaded{false};
            bool Dirty {true }; // indicates that geometry has not been yet computed

        //private:
            std::string Name  = "UINode";
            std::string Hash  = "###UINode";
            int         Count = 0;
        };

        // hierarchy
        struct ImmedidateUserInterfaceHierarchy
        {
            ImmedidateUserInterfaceHierarchy(const std::function<ImmediateUserInterfaceNode*(const ImmediateUserInterfaceNode*)> _GetParent =
                [](const ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
                {
                    return _Node != nullptr ? _Node->State.Parent : nullptr;
                }) : GetParent(_GetParent){}

            ~ImmedidateUserInterfaceHierarchy(){}

            mutable std::vector<int>                                                              Indexes;
            mutable std::vector<int>                                                              Entries;
            mutable std::vector<ImmediateUserInterfaceNode*>                                      Singletons;
            mutable std::vector<ImmediateUserInterfaceNode*>                                      Sorted;
            mutable std::function<ImmediateUserInterfaceNode*(const ImmediateUserInterfaceNode*)> GetParent;

            std::vector<ImmediateUserInterfaceNode*>::iterator begin(const ImmediateUserInterfaceNode* _Node) const
            {
                if( _Node == nullptr                                            ||
                    _Node->State.RenderingIndex          >= (int)Indexes.size() ||
                    Indexes[_Node->State.RenderingIndex] >= (int)Sorted.size())
                {
                    return Sorted.end();
                }

                return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.RenderingIndex];
            }

            std::vector<ImmediateUserInterfaceNode*>::iterator end(const ImmediateUserInterfaceNode* _Node) const
            {
                if(_Node == nullptr                                                 ||
                    _Node->State.RenderingIndex + 1          >= (int)Indexes.size() ||
                    Indexes[_Node->State.RenderingIndex + 1] >= (int)Sorted.size())
                {
                    return Sorted.end();
                }

                return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.RenderingIndex + 1];
            }

            int size(const ImmediateUserInterfaceNode* _Node) const
            {
                return (int)(end(_Node) - begin(_Node));
            }

            template<typename FrameProcessor>
            int count(const ImmediateUserInterfaceNode* _Node, const FrameProcessor& _Filter) const
            {
                int counter = 0;

                for(auto it = begin(_Node); it != end(_Node); it++)
                {
                    if(_Filter(*it))
                        counter++;
                }

                return counter;
            }

            void build(const std::vector<ImmediateUserInterfaceNode*>& _Nodes)
            {
                std::vector<int> workspace(_Nodes.size()+1);

                Indexes.resize(_Nodes.size() + 1);
                Entries.resize(_Nodes.size());
                Sorted.resize(_Nodes.size());
                Singletons.clear();

                for(int i = 0; i < (int)Entries.size(); i++)
                {
                    Entries[i] = 0;
                    Indexes[i] = 0;
                    Sorted [i] = nullptr;

                    if(get_parent(_Nodes[i]) == nullptr)
                        Singletons.push_back(_Nodes[i]);
                }

                // count items
                for (int i = 0; i < (int)_Nodes.size(); i++)
                {
                    if(get_parent(_Nodes[i]) == nullptr)
                        continue;

                    ++Entries[get_parent(_Nodes[i])->State.RenderingIndex];
                }

                // cumulative sum
                int sum = 0;
                for (int i = 0; i < _Nodes.size(); i++)
                {
                    Indexes  [i] = sum;
                    workspace[i] = sum;
                    sum += Entries[i];
                }
                Indexes[_Nodes.size()] = sum;

                bool allIsNull = true;

                for(int i = 0; i < _Nodes.size(); i++ )
                {
                    if(get_parent(_Nodes[i]) == nullptr)
                        continue;

                    Sorted[workspace[get_parent(_Nodes[i])->State.RenderingIndex]++] = _Nodes[i];
                    allIsNull = false;
                }

                if(allIsNull) Sorted.clear();
            }

        //private:

            template<typename Type = ImmediateUserInterfaceNode>
            Type* get_parent(const ImmediateUserInterfaceNode* _Node) const
            {
                auto parent = (GetParent != nullptr ? GetParent(_Node) : nullptr);

                while (parent)
                {
                    if(dynamic_cast<Type*>(parent))
                        return dynamic_cast<Type*>(parent);

                    parent = (GetParent != nullptr ? GetParent(parent) : nullptr);
                }
                

                return nullptr;
            }
        };

        class ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceContextController(){}
            virtual ~ImmediateUserInterfaceContextController(){}

            virtual bool awake(ImmediateUserInterfaceContextLayer*){return true;}
            virtual void frame_start(ImmediateUserInterfaceContextLayer*){}
            virtual void frame_debug(ImmediateUserInterfaceContextLayer*){}
            virtual void frame_render(ImmediateUserInterfaceContextLayer*){}
            virtual void frame_finish(ImmediateUserInterfaceContextLayer*){}
        };

        // context configuration
        class ImmediateUserInterfaceContextConfiguration final
        {
        public:
            ImmediateUserInterfaceContextConfiguration();
            ~ImmediateUserInterfaceContextConfiguration();

            template<typename Type>
            Type get(const std::string& _Section, const std::string& _Name);

            template<typename Type>
            void set(const std::string& _Section, const std::string& _Name, const Type& _Value);

            bool contains(const std::string& _Section, const std::string& _Name) const;
            bool empty() const;
            void clear();

            bool read(const std::u32string& _Path);
            bool write(const std::u32string& _Path);

        private:

        std::map<
            std::string,     // section
            std::map<
                std::string, // key
                std::string  // value
                >
                > m_Configuration; 
        };

        // context layer
        class ImmediateUserInterfaceContextLayer : public Layer
        {
        public:
            ImmediateUserInterfaceContextLayer();
            virtual ~ImmediateUserInterfaceContextLayer();

            // Layer API
            virtual bool awake()        override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_input()  override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;

            // UI API
            template<typename Type>
            bool begin_node(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, bool* _Render = nullptr)
            {
                // check if we need to render the node
                if(_Render != nullptr && !(*_Render))
                    return false;

                // create node (output is never nullptr)
                ImmediateUserInterfaceNode* node = create_node<Type>(_ID);

                // setup node parameters
                node->State.Settings       = _Settings;
                node->State.RenderingIndex = (int)m_NodesRenderingList.size();

                // build nodes hierarchy
                if(!m_NodesRenderingStack.empty())
                {
                    if(!(node->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent))
                        m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]->attach_child(node);
                    node->State.Scope = m_NodesRenderingStack[m_NodesRenderingStack.size() - 1];
                }

                // setup next rendered node parameters
                ImmedidateUserInterfaceNextNodeController* controller = get_controller<ImmedidateUserInterfaceNextNodeController>();

                if(controller != nullptr)
                {
                    // next line
                    if(!m_NodesRenderedStack.empty() && controller->NextLine.has_value())
                        m_NodesRenderedStack[m_NodesRenderedStack.size() - 1]->State.NextLine = controller->NextLine.value();

                    // next indent
                    if(!m_NodesRenderedStack.empty() && controller->NextIndent.has_value())
                        m_NodesRenderedStack[m_NodesRenderedStack.size() - 1]->State.Indent = controller->NextIndent.value();

                    // next minimum size
                    if(controller->NextMinimumSize.has_value())
                        node->State.MinimumSize = controller->NextMinimumSize.value();

                    // next maximum size
                    if(controller->NextMaximumSize.has_value())
                        node->State.MaximumSize = controller->NextMaximumSize.value();

                    // next position
                    if(controller->NextPosition.has_value())
                    {
                        node->State.BoundingBox = gs_2dboxf(
                            controller->NextPosition.value(),
                            controller->NextPosition.value() + gs_clamp(node->State.BoundingBox.size(), node->State.MinimumSize, node->State.MaximumSize));
                    }

                    // next content margin
                    if(dynamic_cast<ImmediateUserInterfaceNodePanel*>(node) && controller->NextContentMargin.has_value())
                        dynamic_cast<ImmediateUserInterfaceNodePanel*>(node)->ContentMargin = controller->NextContentMargin.value();

                    // next content padding
                    if(dynamic_cast<ImmediateUserInterfaceNodePanel*>(node) && controller->NextContentPadding.has_value())
                        dynamic_cast<ImmediateUserInterfaceNodePanel*>(node)->ContentPadding = controller->NextContentPadding.value();

                    // reset next item controller
                    controller->reset();
                }

                m_NodesRenderingList.push_back(node);
                m_NodesRenderingStack.push_back(node);

                return node->create_contents(this, _ID, _Settings, _Render);
            }

            template<typename Type>
            void end_node()
            {
                // reset next node controller
                ImmedidateUserInterfaceNextNodeController* controller =
                    get_controller<ImmedidateUserInterfaceNextNodeController>();

                if(controller != nullptr)
                    controller->reset();

                // clean up rendering stack and filled rendered nodes stack
                if(m_NodesRenderingStack.empty())
                    return;

                ImmediateUserInterfaceNode* node =
                    m_NodesRenderingStack[m_NodesRenderingStack.size() - 1];

                // as the node can contain nested items and store pointers to them
                // we need to load state when the node finishes it's hierarchy
                if(!m_IniFileState.empty())
                    node->load_state(this);

                GS_ASSERT((dynamic_cast<Type*>(node) != nullptr));

                m_NodesRenderedStack.push_back(node);
                
                m_NodesRenderingStack.pop_back();
            }

            // This function creates window
            // _ID       - unique ID
            // _Settings - settings
            // _Opened   - if this pointer is passed the close button is activated and window becomes closable
            bool begin_window(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
                bool*                                     _Opened   = nullptr);
            void end_window();

            // This function creates scrollable area that places it's contents one by one vertically unless
            // same_line(), next_line() or indent() functions are called (see appropriate functions descriptions)
            // _ID       - unique ID
            // _Settings - settings
            bool begin_scrollarea(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_scrollarea();

            // This function creates UI panel that stretches it's content to fill all available space
            // _ID       - unique ID
            // _Settings - settings
            bool begin_panel(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_panel();

            // This function creates layout box that places it's elements one by one vertically. The width of all elements equals to vertical stack width.
            // The height of each element is calculated as a fill rate height relative to the all elements heights summ.
            // _ID       - unique ID
            // _Settings - settings
            bool begin_vertical_stack(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_vertical_stack();

            // This function creates layout box that places it's elements one by one horizontally. The height of all elements equals to horizontal stack height.
            // The width of each element is calculated as a fill rate height relative to the all elements widths summ.
            // _ID       - unique ID
            // _Settings - settings
            bool begin_horizontal_stack(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_horizontal_stack();

            // This fuction creates popup menu that can be called by a right mouse click and that can play role of window menu.
            // _Settings - settings
            bool begin_menu(const std::string& _ID);
            void end_menu();

            // this function creates combobox widget.
            // _ID      - unique ID
            // _Preview - preview text of combobox widget
            bool begin_combobox(const std::string& _ID, const std::string& _Preview = "None");
            void end_combobox();

            // This function creates tree node. Tree node supports custom opened/closed state textures.
            // When no custom opened/closed state textures provided simple triangle is rendered.
            // You also can optionally render connection lines between tree nodes.
            // _ID            - unique ID
            // _Settings      - settings
            // _TextureOpened - texture displayed on the left side of a tree node when it's opened
            // _TextureClosed - texture displayed on the left side of a tree node when it's closed
            bool begin_tree_node(
                const std::string&                            _ID,
                const ImmediateUserInterfaceTreeNodeSettings& _Settings      = ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_Defaults,
                const ApplicationRenderingBackendTexture&     _TextureOpened = ApplicationRenderingBackendTexture(),
                const ApplicationRenderingBackendTexture&     _TextureClosed = ApplicationRenderingBackendTexture());
            void end_tree_node();

            // Widgets API

            // This function creates a simple push button widget
            // _ID - unique ID
            bool push_button(const std::string& _ID);

            // This function creates checkbutton that can be redered as checkbox, radiobutton or slider button depending on the settings.
            //  _ID      - unique ID
            // _Checked  - input boolean that defines checkbutton state, checkbutton may or may not change this variable depending on settings
            // _Settings - checkbutton settings
            bool check_button(
                const std::string&                               _ID,
                bool&                                            _Checked,
                const ImmediateUserInterfaceCheckButtonSettings& _Settings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Defaults);

            // This function creates menu action button
            // _ID - unique ID
            bool menu_action(const std::string& _ID);

            // This function creates combobox action button
            // _ID - unique ID
            bool combobox_item(const std::string& _ID);

            // This function creates a simple textual label
            // _ID       - unique ID
            // _Text     - text string to display
            // _Settings - label settings
            void label(
                const std::string&                         _ID,
                const std::string&                         _Text,
                const ImmediateUserInterfaceLabelSettings& _Settings = ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_None);

            // This function creates editable multiline text widget
            // _ID              - unique ID
            // _Text            - text string to display and edit
            // _Settings        - settings
            // _InputTextFilter - input text filtering function
            bool input_string_multiline(
                const std::string&                               _ID,
                std::string&                                     _Text,
                const ImmediateUserInterfaceInputStringSettings& _Settings                             = ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Defaults,
                bool                                           (*_InputTextFilter)(const std::string&) = nullptr);

            // This function creates editable singleline text widget
            // _ID              - unique ID
            // _Text            - text string to display and edit
            // _Settings        - settings
            // _InputTextFilter - input text filtering function
            bool input_string_singleline(
                const std::string&                               _ID,
                std::string&                                     _Text,
                const ImmediateUserInterfaceInputStringSettings& _Settings                             = ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Defaults,
                bool                                           (*_InputTextFilter)(const std::string&) = nullptr);

            // This function creates widget aimed at scalar values editing
            // The following scalar data types supported: float, double, long double, int, short, unsigned int, unsigned short
            // _ID       - unique ID
            // _Input    - input scalar value
            // _Min      - input scalar value minimum value
            // _Max      - input scalar value maximum value
            // _Settings - settings
            template<typename Type>
            bool input_scalar(
                const std::string&                               _ID,
                Type&                                            _Input,
                const Type&                                      _Min      = gs_tiny<Type>(),
                const Type&                                      _Max      = gs_huge<Type>(),
                const ImmediateUserInterfaceInputScalarSettings& _Settings = ImmediateUserInterfaceInputScalarSettings_Defaults);

            // This function creates a slider that can change input scalar value
            // The following scalar data types supported: float, double, long double, int, short, unsigned int, unsigned short
            // _ID       - unique ID
            // _Input    - input scalar value
            // _Min      - input scalar value minimum value
            // _Max      - input scalar value maximum value
            // _Delta    - input scalar value step in % of (_Max - Min)
            // _Settings - settings
            template<typename Type>
            bool input_scalar_slider(
                const std::string&                               _ID,
                Type&                                            _Input,
                const Type&                                      _Min      = gs_tiny<Type>(),
                const Type&                                      _Max      = gs_huge<Type>(),
                const int&                                       _Delta    = 1,
                const ImmediateUserInterfaceInputScalarSettings& _Settings = ImmediateUserInterfaceInputScalarSettings_Defaults);

            // This function creates widget for color eiditing
            // _ID       - unique ID
            // _Color    - input color
            // _Settings - settings
            bool input_color(
                const std::string&                               _ID,
                gs_color&                                        _Color,
                const ImmediateUserInterfaceColorPickerSettings& _Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults);

            // This function creates classic RGBA color picker widget
            // _ID       - unique ID
            // _Color    - input color
            // _Settings - settings
            void color_picker_rgba(
                const std::string&                               _ID,
                gs_color&                                        _Color,
                const ImmediateUserInterfaceColorPickerSettings& _Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults);

            // This function creates classic HSVA color picker widget
            // _ID       - unique ID
            // _Color    - input color
            // _Settings - settings
            void color_picker_hsva(
                const std::string&                               _ID,
                gs_color&                                        _Color,
                const ImmediateUserInterfaceColorPickerSettings& _Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults);

            // This function creates image display widget
            // _ID       - unique ID
            // _Color    - image mask color
            // _Texture  - texture
            void image(
                const std::string&                        _ID,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture = ApplicationRenderingBackendTexture());

            // next node API

            // This function generates a unique ID for UI element. It constructs ID as {Name}###{Hash} where Name is what is going to be diplayed and Hash is what is going to be used as a widget unique ID.
            // If no Hash passed to a function it generates unique ID as {Name}
            // _Name       - UI element name
            // _Hash       - UI element hash
            std::string next_id(const std::string& _Name, const std::string& _Hash = std::string());

            // 
            void next_line();
            void same_line();
            void indent(const float& = 32.f);
            void next_size(const gs_vec2f&);
            void next_position(const gs_vec2f&);
            void next_minimum_size(const gs_vec2f&);
            void next_maximum_size(const gs_vec2f&);
            void next_content_margin(const gs_vec4f&);
            void next_content_padding(const gs_vec4f&);

            // Controllers API
            template<typename Type> Type* get_controller() const
            {
                for(auto& controller : m_Controllers)
                {
                    if(dynamic_cast<Type*>(controller.get()))
                        return dynamic_cast<Type*>(controller.get());
                }

                return nullptr;
            }

            // Rendering stack API
            template<typename Type = ImmediateUserInterfaceNode>
            Type* get_rendering_stack_top() const
            {
                if(m_NodesRenderingStack.empty())
                    return nullptr;

                return dynamic_cast<Type*>(m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]);
            }

            template<typename Type = ImmediateUserInterfaceNode>
            Type* get_rendered_stack_top() const
            {
                if(m_NodesRenderedStack.empty())
                    return nullptr;

                return dynamic_cast<Type*>(m_NodesRenderedStack[m_NodesRenderedStack.size() - 1]);
            }

            // info

            // hierarchy and cache
            mutable std::map<std::string, std::unique_ptr<ImmediateUserInterfaceNode>> m_Cache;
            mutable ImmedidateUserInterfaceHierarchy                                   m_Hierarchy;

            // rendering
            mutable std::shared_ptr<RenderingQueue>                                    m_Renderer{nullptr};
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingList;
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingStack;
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderedStack;

            // style
            mutable ImmedidateUserInterfaceStyle                                       m_Style;

            // ini file
            ImmediateUserInterfaceContextConfiguration                                 m_IniFileState;

            // input
            ImmedidateUserInterfaceInput                                               m_Input;

            // settings
            ImmediateUserInterfaceContextSettings                                      m_Settings =
                ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking |
                ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking   |
                ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes;

        private:

            // info
            std::vector<std::unique_ptr<ImmediateUserInterfaceContextController>> m_Controllers;
            std::string                                                           m_CurrentHash;
            std::string                                                           m_CurrentName;
            std::u32string                                                        m_IniFilePath = U"Frenchie.ini";

            template<typename Type>
            Type* create_node(const std::string& _ID)
            {
                // clean-up hash and name buffers
                m_CurrentHash.clear();
                m_CurrentName.clear();

                // determine hashable part of input id
                int hashable   = 0;
                int sharpCount = 0;

                for (;hashable < (int)_ID.size(); hashable++)
                {
                    if(_ID[hashable] == '#')
                    {
                        sharpCount = 1;
                        if(hashable + 1 < (int)_ID.size() && _ID[hashable + 1] == '#') ++sharpCount;
                        if(hashable + 2 < (int)_ID.size() && _ID[hashable + 2] == '#') ++sharpCount;
                        
                        if(sharpCount >= 3)
                            break;
                    }
                }
                
                // create node
                m_CurrentHash.append(
                    ((hashable + sharpCount) < _ID.size() ? _ID.c_str() + (hashable + sharpCount) : _ID.c_str()),
                    ((hashable + sharpCount) < _ID.size() ? _ID.size()  - (hashable + sharpCount) : _ID.size()));

                if(m_Cache.find(m_CurrentHash) == m_Cache.end())
                    m_Cache[m_CurrentHash] = std::make_unique<Type>(m_CurrentHash);
                ImmediateUserInterfaceNode* node = m_Cache[m_CurrentHash].get();
                GS_ASSERT((++node->Count) <= 1);

                // setup node name
                m_CurrentName.append(_ID.c_str(), _ID.c_str() + hashable);
                if(node->Name != m_CurrentName)
                    node->Name = m_CurrentName;

                return dynamic_cast<Type*>(node);
            }
        
        };
    };
}