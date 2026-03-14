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
        // This enum contains boolean flags indicating different node events such as resize, move e.t.c
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
        // This enum contains indexes of the colors used by immediate user interface layer style
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
            ImmediateUserInterfaceNodeSettings_ForceMoveOnBackground                  = 1 << 22, // sets the node to be always on background

            ImmediateUserInterfaceNodeSettings_AllowedModificationsDefaults           = 
                ImmediateUserInterfaceNodeSettings_Movable |
                ImmediateUserInterfaceNodeSettings_Resizable,

            ImmediateUserInterfaceNodeSettings_ScrollAreaDefaults                     =
                ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar              |
                ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar            |
                ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment  |
                ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment   |
                ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment,

            ImmediateUserInterfaceNodeSettings_ContentAlignmentDefaults =
                ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter         |
                ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter,

            ImmediateUserInterfaceNodeSettings_Defaults =
                ImmediateUserInterfaceNodeSettings_AllowedModificationsDefaults           |
                ImmediateUserInterfaceNodeSettings_ScrollAreaDefaults                     |
                ImmediateUserInterfaceNodeSettings_ContentAlignmentDefaults,
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

        enum ImmediateUserInterfaceLabelSettings_ : int
        {
            ImmediateUserInterfaceLabelSettings_None        = 0,
            ImmediateUserInterfaceLabelSettings_RenderFrame = 1 << 0,
        };

        enum ImmediateUserInterfaceCheckButtonSettings_ : int
        {
            ImmediateUserInterfaceCheckButtonSettings_Checkbox     = 1 << 0, // check button is rendered as checkbox
            ImmediateUserInterfaceCheckButtonSettings_RadioButton  = 1 << 1, // check button is rendered as radio button
            ImmediateUserInterfaceCheckButtonSettings_SliderButton = 1 << 2  // check button is rendered as slider button
        };

        enum ImmediateUserInterfaceInputStringSettings_ : int
        {
            ImmediateUserInterfaceInputStringSettings_NoInput           = 1 << 0, // disables input
            ImmediateUserInterfaceInputStringSettings_Password          = 1 << 1, // all input symbols are changed on '*' while rendering
            ImmediateUserInterfaceInputStringSettings_NoClipboard       = 1 << 2, // disables copy/paste
            ImmediateUserInterfaceInputStringSettings_NoSelection       = 1 << 3, // disables selection
            ImmediateUserInterfaceInputStringSettings_NoMultiline       = 1 << 4, // disables multiline text
            ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter = 1 << 5, // appropriate input string function returns true when enter key is pressed
            ImmediateUserInterfaceInputStringSettings_Defaults          = 0
        };

        enum ImmediateUserInterfaceColorPickerSettings_ : int
        {
            ImmediateUserInterfaceColorPickerSettings_None      = 0,      // sentinel
            ImmediateUserInterfaceColorPickerSettings_EditRGB   = 1 << 0, // enables RGB   editor in color picker
            ImmediateUserInterfaceColorPickerSettings_EditHSV   = 1 << 1, // enables HSV   editor in color picker
            ImmediateUserInterfaceColorPickerSettings_EditHSL   = 1 << 2, // enables HSL   editor in color picker
            ImmediateUserInterfaceColorPickerSettings_EditAlpha = 1 << 3, // enables alpha editor in color picker

            ImmediateUserInterfaceColorPickerSettings_Defaults  =
                ImmediateUserInterfaceColorPickerSettings_EditRGB |
                ImmediateUserInterfaceColorPickerSettings_EditHSV |
                ImmediateUserInterfaceColorPickerSettings_EditHSL |
                ImmediateUserInterfaceColorPickerSettings_EditAlpha
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
                ImmedidateUserInterfaceDockingAnchor_Top    |
                ImmedidateUserInterfaceDockingAnchor_Left   |
                ImmedidateUserInterfaceDockingAnchor_Right  |
                ImmedidateUserInterfaceDockingAnchor_Bottom |
                ImmedidateUserInterfaceDockingAnchor_Center
        };

        // rendering order and rendering layer
        enum ImmedidateUserInterfaceRenderingOrder_ : int
        {
            ImmedidateUserInterfaceRenderingOrder_Begin      = 0,
            ImmedidateUserInterfaceRenderingOrder_Background = ImmedidateUserInterfaceRenderingOrder_Begin,
            ImmedidateUserInterfaceRenderingOrder_Main,
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

        typedef int ImmediateUserInterfaceNodeEvents;
        typedef int ImmediateUserInterfaceNodeMouseHover;

        typedef int ImmediateUserInterfaceNodeSettings;
        typedef int ImmediateUserInterfaceContextSettings;
        typedef int ImmediateUserInterfaceLabelSettings;
        typedef int ImmediateUserInterfaceCheckButtonSettings;
        typedef int ImmediateUserInterfaceInputStringSettings;
        typedef int ImmediateUserInterfaceColorPickerSettings;

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
            float                            FontSize             = 64.f;
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
                int                                Depth                      {0};     // depth along Z-axis
                int                                SelfThickness              {0};     // thickness of rendered content
                int                                RenderingIndex             {0};     // index of the node within context rendering list
                int                                RenderingOrder             {0};     // index of the node while rendering
                int                                MaximumChildDepth          {0};     // depth of the deepest child
                int                                MaximumChildThickness      {0};     // thickness of the 'fattest' child
                bool                               PlaceInFollow              {false}; // shows if the node places it's children in follow
                bool                               OrderChildrenWhileRendering{false}; // shows if the node sorts it's children by rendering order index while rendering

                // geometry
                gs_2dboxf                          BoundingBox                {gs_2dboxf(gs_vec2f(32.f, 32.f), gs_vec2f(1024.f, 512.f))};
                gs_vec2f                           ContentSize                {gs_vec2f(0.f)};
                gs_vec2f                           MinimumSize                {gs_vec2f(32.f)};
                gs_vec2f                           MaximumSize                {gs_vec2f((float)INT_MAX)};

                // hierarchy
                ImmediateUserInterfaceNode*        Parent                     {nullptr};
                ImmediateUserInterfaceNode*        Relative                   {nullptr};

                // visibility
                mutable Frenchie::Core::Optional<gs_2dboxf> ClippingBox;

                // settings
                ImmediateUserInterfaceNodeSettings Settings                   {ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable};

                // events
                ImmediateUserInterfaceNodeEvents   Events                     {ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None};
                bool                               Selected                   {false};

                // layout hints
                int NextLine = 0;

                // mouse hover
                ImmediateUserInterfaceNodeMouseHover           MouseHover{ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None};
                std::chrono::high_resolution_clock::time_point MouseEnterTimer;
                std::chrono::high_resolution_clock::time_point MouseLeaveTimer;
            };

            Data State;
            Data Cache;
            bool Loaded{false};

        //private:
            std::string Name  = "UINode";
            std::string Hash  = "###UINode";
            int         Count = 0;
        };

        // layouts
        struct ImmediateUserInterfaceNodePanel : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceNodePanel(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNodePanel();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;

            gs_vec2f ContentPadding = gs_vec2f(0.f, 0.f);
        };

        struct ImmediateUserInterfaceNodeVerticalStack : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceNodeVerticalStack(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNodeVerticalStack();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceNodeHorizontalStack : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceNodeHorizontalStack(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNodeHorizontalStack();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        };
      
        struct ImmediateUserInterfaceScrollAreaScrollBar;

        struct ImmediateUserInterfaceScrollArea : public ImmediateUserInterfaceNodePanel
        {
        public:

            ImmediateUserInterfaceScrollArea(const std::string& _Name);
            virtual ~ImmediateUserInterfaceScrollArea();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;

            virtual void attach_child(ImmediateUserInterfaceNode*   _Child) override;

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr) override;

            float get_horizontal_scrollbar_width(ImmediateUserInterfaceContextLayer*) const;
            float get_vertical_scrollbar_width(ImmediateUserInterfaceContextLayer*) const;

            ImmediateUserInterfaceNode*                ContentView         = nullptr;
            ImmediateUserInterfaceScrollAreaScrollBar* VerticalScrollBar   = nullptr;
            ImmediateUserInterfaceScrollAreaScrollBar* HorizontalScrollBar = nullptr;

        private:
            void calculate_content_padding(ImmediateUserInterfaceContextLayer*, ImmediateUserInterfaceNode*, gs_vec2f&);
        };

        // popups
        struct ImmediateUserInterfaceMenu : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceMenu(const std::string& _Name);
            virtual ~ImmediateUserInterfaceMenu();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override;

            ImmediateUserInterfaceScrollArea* InternalScrollArea = nullptr;
            ImmediateUserInterfaceScrollArea* ExternalScrollArea = nullptr;
        };

        // windows
        struct ImmediateUserInterfaceWindow : public ImmediateUserInterfaceNode
        {
        public:

            ImmediateUserInterfaceWindow(const std::string& _Name);
            virtual ~ImmediateUserInterfaceWindow();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void attach_child(ImmediateUserInterfaceNode*   _Child) override;

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr) override;

            virtual void load_state(ImmediateUserInterfaceContextLayer*) override;
            virtual void save_state(ImmediateUserInterfaceContextLayer*) override;

            ImmediateUserInterfaceNode* Docker            {nullptr};
            ImmediateUserInterfaceNode* TopSnapper        {nullptr};
            ImmediateUserInterfaceNode* LeftSnapper       {nullptr};
            ImmediateUserInterfaceNode* RightSnapper      {nullptr};
            ImmediateUserInterfaceNode* BottomSnapper     {nullptr};
            bool                        IsActive          {true};
            bool                        Activate          {false};
            bool                        ReattachChildren  {false};

            gs_2dboxf                   FrameBox          {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
            gs_2dboxf                   ContentBox        {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
            bool*                       Opened            {nullptr};
            int                         DockingIndex      {-1};

            // docking
            ImmediateUserInterfaceNode* DockerView        {nullptr};

            // snapping
            ImmediateUserInterfaceNode* SnapperView       {nullptr};
            ImmediateUserInterfaceNode* TopSnapperView    {nullptr};
            ImmediateUserInterfaceNode* LeftSnapperView   {nullptr};
            ImmediateUserInterfaceNode* RightSnapperView  {nullptr};
            ImmediateUserInterfaceNode* BottomSnapperView {nullptr};

            // content
            ImmediateUserInterfaceNode* ContentView       {nullptr};
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

        // controllers
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

        class ImmedidateUserInterfaceWindowController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceWindowController();
            virtual ~ImmedidateUserInterfaceWindowController();
            virtual void frame_start(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_finish(ImmediateUserInterfaceContextLayer*) override;

            std::vector<ImmediateUserInterfaceNode*>&
            retrieve_docked_windows(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Docker, const ImmedidateUserInterfaceDockingAnchor& _Anchors);

        private:

            void place_on_dockers(ImmediateUserInterfaceContextLayer* _Context);
            bool can_be_docked(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked);
            void attach_to_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked, const ImmedidateUserInterfaceDockingAnchor& _Anchor);
            void detach_from_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Detached);

            mutable std::vector<ImmediateUserInterfaceNode*>  m_WindowsDockingCache;
            mutable std::vector<ImmediateUserInterfaceNode*>  m_WindowsDockingList;
            mutable ImmediateUserInterfaceWindow*             m_WorkspaceDockArea {nullptr};
            mutable bool                                      m_DockAreaOpened    {false};
        };
    
        class ImmedidateUserInterfaceInputController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceInputController();
            virtual ~ImmedidateUserInterfaceInputController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context) override;
        };
    
        class ImmedidateUserInterfaceLayoutController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceLayoutController();
            virtual ~ImmedidateUserInterfaceLayoutController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context) override;

        private:
            void node_layout(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node);
        };

        class ImmedidateUserInterfaceRenderingController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceRenderingController();
            virtual ~ImmedidateUserInterfaceRenderingController();
            virtual void frame_render(ImmediateUserInterfaceContextLayer*) override;

        private:

            static void render_node(ImmediateUserInterfaceContextLayer*, ImmediateUserInterfaceNode*);

            mutable std::vector<ImmediateUserInterfaceNode*> m_NodesRenderingCache;
        };

        class ImmedidateUserInterfaceMenusController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceMenusController();
            virtual ~ImmedidateUserInterfaceMenusController();

            virtual void frame_finish(ImmediateUserInterfaceContextLayer* _Context) override;

            mutable std::vector<ImmediateUserInterfaceMenu*> ActiveMenus;
        };
    
        class ImmedidateUserInterfaceNextNodeController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceNextNodeController();
            virtual ~ImmedidateUserInterfaceNextNodeController();

            virtual void frame_start(ImmediateUserInterfaceContextLayer*) override;
            
            // API
            void reset();

            // info
            mutable Frenchie::Core::Optional<int>      NextLine;
            mutable Frenchie::Core::Optional<gs_vec2f> NextMaximumSize;
            mutable Frenchie::Core::Optional<gs_vec2f> NextMinimumSize;
            mutable Frenchie::Core::Optional<gs_vec2f> NextPosition;
            mutable Frenchie::Core::Optional<gs_vec2f> NextContentPadding;
        };

        class ImmediateUserInterfaceScrollBarsController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceScrollBarsController();
            virtual ~ImmediateUserInterfaceScrollBarsController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context) override;
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

            virtual bool awake()        override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_debug()  override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;

            // UI API

            // Hierarchical elements

            // scroll area
            bool begin_scrollarea(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_scrollarea();

            // layout
            bool begin_panel(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_panel();

            bool begin_vertial_stack(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_vertical_stack();

            bool begin_horizontal_stack(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_horizontal_stack();

            // menus
            bool begin_menu(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_menu();

            // widgets

            // This function renders push button
            bool push_button(const std::string& _ID);

            bool check_button(
                const std::string&                               _ID,
                bool&                                            _Checked,
                const ImmediateUserInterfaceCheckButtonSettings& _Settings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox);
            
            bool menu_action(const std::string& _ID);
            
            // This function renders text label. The minimum size of the label equals to text size, maximum size has no limit
            // on horizontal axis, but is constrained by a text height on vertical axis
            void label(
                const std::string&                         _ID,
                const std::string&                         _Text,
                const ImmediateUserInterfaceLabelSettings& _Settings = ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_None);

            // This function renders input multiline text
            bool input_string_multiline(
                const std::string&                               _ID,
                std::string&                                     _Text,
                const ImmediateUserInterfaceInputStringSettings& _Settings                             = ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Defaults,
                bool                                           (*_InputTextFilter)(const std::string&) = nullptr);

            // This function renders input singleline text
            bool input_string_singleline(
                const std::string&                               _ID,
                std::string&                                     _Text,
                const ImmediateUserInterfaceInputStringSettings& _Settings                             = ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Defaults,
                bool                                           (*_InputTextFilter)(const std::string&) = nullptr);

            // This function renders input scalar value widget.
            // Supported scalar types are: float, double, int, short, unsigned int, unsigned short
            template<typename Type>
            bool input_scalar(const std::string& _ID, Type& _Input, const Type& _Min = gs_tiny<Type>(), const Type& _Max = gs_huge<Type>());

            // This function renders classic RGBA color picker widget.
            // The widget supports RGB, HSV, HSL color formats and alpha channel value regulation.
            void color_picker_rgba(
                const std::string&                               _ID,
                gs_color&                                        _Color,
                const ImmediateUserInterfaceColorPickerSettings& _Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults);

            // This function renders rounded HSV color picker widget.
            // The widget supports RGB, HSV, HSL color formats and alpha channel value regulation.
            void color_picker_hsva(
                const std::string&                               _ID,
                gs_color&                                        _Color,
                const ImmediateUserInterfaceColorPickerSettings& _Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults);

            // This function renders square image.
            // If texture is not passed to a function default renderer white pattern image is used.
            void image(const std::string& _ID, const gs_color& _ColorMask, const ApplicationRenderingBackendTexture& _Texture = ApplicationRenderingBackendTexture());

            // windows
            bool begin_window(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
                bool*                                     _Opened = nullptr);
            void end_window();

            // next node API
            
            // If next node is pushed within scroll area this function pushes it onto a next line by inserting vertica 'next line' indent.
            // If you call this function several times it inserts several 'next line' indents before the node.
            // All changes are applied every frame.
            void next_line();

            // This function sets next node size by changing it's bounding box width, height and size constraints (minimum and maximum size).
            // All changes are applied every frame.
            void next_size(const gs_vec2f&);

            // This function changes minimum size constrain of next node.
            // All changes are applied every frame.
            void next_minimum_size(const gs_vec2f&);

            // This function changes minimum size constrain of next node.
            // All changes are applied every frame.
            void next_maximum_size(const gs_vec2f&);

            // This function changes next node position.
            // All changes are applied every frame.
            void next_position(const gs_vec2f&);

            // If the node is descendant of panel this function changes it's content padding.
            // All changes are applied every frame.
            void next_content_padding(const gs_vec2f&);

            // ID API
            // _Name - is what is going to be displayed as the node title
            // _Hash - is a unique local node identifier
            std::string next_id(const std::string& _Name, const std::string& _Hash = std::string());

            // This function retrieves controller of a given type
            template<typename Type> Type* get_controller() const
            {
                for(auto& controller : m_Controllers)
                {
                    if(dynamic_cast<Type*>(controller.get()))
                        return dynamic_cast<Type*>(controller.get());
                }

                return nullptr;
            }

            // This function retrieves rendering stack top node
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


            // This function start the new node of a given type.
            // The function creates the node or retrieves it from cache and then pushes it into rendering stack and rendered items list
            template<typename Type>
            bool begin_node(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr)
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
                    node->State.Relative = m_NodesRenderingStack[m_NodesRenderingStack.size() - 1];
                }

                // setup next rendered node parameters
                ImmedidateUserInterfaceNextNodeController* controller = get_controller<ImmedidateUserInterfaceNextNodeController>();

                if(controller != nullptr)
                {
                    // next line
                    if(!m_NodesRenderedStack.empty() && controller->NextLine.has_value())
                        m_NodesRenderedStack[m_NodesRenderedStack.size() - 1]->State.NextLine = controller->NextLine.value();

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

            // This function ends the node of a given type.
            // The function pops the node out of hierarchical rendering stack and pushes it into
            // already rendered nodes stack
            template<typename Type>
            void end_node()
            {
                if(m_NodesRenderingStack.empty())
                    return;

                // as the node can contain nested items and store pointers to them
                // we need to load state when the node finishes it's hierarchy
                if(!m_IniFileState.empty())
                    m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]->load_state(this);

                GS_ASSERT((dynamic_cast<Type*>(m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]) != nullptr));

                m_NodesRenderedStack.push_back(m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]);
                
                m_NodesRenderingStack.pop_back();
            }

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

            template<typename Type> Type* create_node(const std::string& _ID)
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