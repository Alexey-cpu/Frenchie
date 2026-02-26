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
        // events
        enum ImmediateUserInterfaceNodeEvents_ : int
        {
            // sentinel
            ImmediateUserInterfaceNodeEvents_None                 = 0,
            
            // move
            ImmediateUserInterfaceNodeEvents_IsMoved              = 1 << 0,
            
            // resize
            ImmediateUserInterfaceNodeEvents_IsResizedTop         = 1 << 1,
            ImmediateUserInterfaceNodeEvents_IsResizedLeft        = 1 << 2,
            ImmediateUserInterfaceNodeEvents_IsResizedRight       = 1 << 3,
            ImmediateUserInterfaceNodeEvents_IsResizedBottom      = 1 << 4,
            ImmediateUserInterfaceNodeEvents_IsResizedTopLeft     = 1 << 5,
            ImmediateUserInterfaceNodeEvents_IsResizedTopRight    = 1 << 6,
            ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft  = 1 << 7,
            ImmediateUserInterfaceNodeEvents_IsResizedBottomRight = 1 << 8,
        };

        // colors
        enum ImmediateUserInterfaceNodeColors_ : int
        {
            ImmediateUserInterfaceNodeColors_Begin         = 0,

            // windows
            ImmediateUserInterfaceNodeColors_ChildBackground = ImmediateUserInterfaceNodeColors_Begin,
            ImmediateUserInterfaceNodeColors_ChildBackgroundHovered,
            ImmediateUserInterfaceNodeColors_ParentBackground,
            ImmediateUserInterfaceNodeColors_ParentBackgroundHovered,

            // push button
            ImmediateUserInterfaceNodeColors_ButtonOutline,
            ImmediateUserInterfaceNodeColors_ButtonBackground,
            ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered,
            ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed,

            // gizmos
            ImmediateUserInterfaceNodeColors_Gizmos,
            ImmediateUserInterfaceNodeColors_GizmosHovered,

            // text
            ImmediateUserInterfaceNodeColors_Text,
            
            ImmediateUserInterfaceNodeColors_End
        };

        // settings
        enum ImmediateUserInterfaceNodeSettings_ : int
        {
            // sentinel
            ImmediateUserInterfaceNodeSettings_None        = 0,

            // settings
            ImmediateUserInterfaceNodeSettings_Movable     = 1 << 0,
            ImmediateUserInterfaceNodeSettings_Resizable   = 1 << 1,
            ImmediateUserInterfaceNodeSettings_NullParent  = 1 << 2,

            ImmediateUserInterfaceNodeSettings_UserDefined = 1 << 3,

            ImmediateUserInterfaceNodeSettings_Defaults  =
                ImmediateUserInterfaceNodeSettings_Movable |
                ImmediateUserInterfaceNodeSettings_Resizable,
        };

        enum ImmediateUserInterfaceContextSettings_ : int
        {
            // docking
            ImmediateUserInterfaceContextSettings_DisableDocking         = 1 << 0,
            ImmediateUserInterfaceContextSettings_EnableWindowsDocking   = 1 << 1,
            ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking = 1 << 2,
        };

        enum ImmediateUserInterfaceScrollAreaSettings_ : int
        {
            ImmediateUserInterfaceScrollAreaSettings_NeverVerticalScrollBar             = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 1,
            ImmediateUserInterfaceScrollAreaSettings_AlwaysVerticalScrollBar            = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 2,
            ImmediateUserInterfaceScrollAreaSettings_AdaptiveVerticalScrollBar          = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 3,

            ImmediateUserInterfaceScrollAreaSettings_NeverHorizontalScrollBar           = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 4,
            ImmediateUserInterfaceScrollAreaSettings_AlwaysHorizontalScrollBar          = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 5,
            ImmediateUserInterfaceScrollAreaSettings_AdaptiveHorizontalScrollBar        = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 6,

            ImmediateUserInterfaceScrollAreaSettings_ResizeToContentsVertically         = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 7,
            ImmediateUserInterfaceScrollAreaSettings_ResizeToContentsHorizontally       = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 8,

            ImmediateUserInterfaceScrollAreaSettings_MouseWheelAdjustsVerticalScrollBar = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 9,
            ImmediateUserInterfaceScrollAreaSettings_ArrowKeysAdjustVerticalScrollBar   = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 10,
            ImmediateUserInterfaceScrollAreaSettings_ArrowKeysAdjustHorizontalScrollBar = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 11,

            ImmediateUserInterfaceScrollAreaSettings_Defaults                    =
                ImmediateUserInterfaceScrollAreaSettings_AdaptiveVerticalScrollBar          |
                ImmediateUserInterfaceScrollAreaSettings_AdaptiveHorizontalScrollBar        |
                ImmediateUserInterfaceScrollAreaSettings_MouseWheelAdjustsVerticalScrollBar |
                ImmediateUserInterfaceScrollAreaSettings_ArrowKeysAdjustVerticalScrollBar   |
                ImmediateUserInterfaceScrollAreaSettings_ArrowKeysAdjustHorizontalScrollBar,
        };

        enum ImmediateUserInterfaceCheckButtonSettings_ : int
        {
            ImmediateUserInterfaceCheckButtonSettings_Checkbox     = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 12,
            ImmediateUserInterfaceCheckButtonSettings_RadioButton  = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 13,
            ImmediateUserInterfaceCheckButtonSettings_SliderButton = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 14
        };

        enum ImmediateUserInterfaceLayoutAlignmentSettings_ : int
        {
            ImmediateUserInterfaceLayoutAlignmentSettings_VerticalAlignTop      = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 15,
            ImmediateUserInterfaceLayoutAlignmentSettings_VerticalAlignCenter   = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 16,
            ImmediateUserInterfaceLayoutAlignmentSettings_VerticalAlignBottom   = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 17,

            ImmediateUserInterfaceLayoutAlignmentSettings_HorizontalAlignLeft   = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 18,
            ImmediateUserInterfaceLayoutAlignmentSettings_HorizontalAlignCenter = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 19,
            ImmediateUserInterfaceLayoutAlignmentSettings_HorizontalAlignRight  = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 20,

            ImmediateUserInterfaceLayoutAlignmentSettings_Defaults =
                ImmediateUserInterfaceLayoutAlignmentSettings_VerticalAlignCenter |
                ImmediateUserInterfaceLayoutAlignmentSettings_HorizontalAlignCenter,
        };

        // mouse hover
        enum ImmediateUserInterfaceNodeMouseHover_ : int
        {
            ImmediateUserInterfaceNodeMouseHover_None         = 0,
            ImmediateUserInterfaceNodeMouseHover_MouseLeft    = 1 << 0,
            ImmediateUserInterfaceNodeMouseHover_MouseHovered = 1 << 1,
            ImmediateUserInterfaceNodeMouseHover_MouseEntered = 1 << 2,
        };

        // windows docking anchors
        enum ImmedidateUserInterfaceDockingAnchor_ : int
        {
            ImmedidateUserInterfaceDockingAnchor_Top    = 1 << 0,
            ImmedidateUserInterfaceDockingAnchor_Left   = 1 << 2,
            ImmedidateUserInterfaceDockingAnchor_Right  = 1 << 3,
            ImmedidateUserInterfaceDockingAnchor_Bottom = 1 << 4,
            ImmedidateUserInterfaceDockingAnchor_Center = 1 << 5,
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
        typedef int ImmediateUserInterfaceNodeSettings;
        typedef int ImmediateUserInterfaceContextSettings;
        typedef int ImmediateUserInterfaceNodeMouseHover;

        typedef int ImmedidateUserInterfaceDockingAnchor;
        typedef int ImmedidateUserInterfaceRenderingOrder;
        typedef int ImmedidateUserInterfaceRenderingLayer;

        class ImmediateUserInterfaceContextLayer;

        // style and events
        struct ImmedidateUserInterfaceStyle
        {
            ImmedidateUserInterfaceStyle()
            {
                Colors.resize(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End);

                // window
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground]         = gs_rgba_color(72, 72, 72, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground]        = gs_rgba_color(28, 28, 28, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered] = gs_rgba_color(72, 82, 72, 255);
                
                // push button
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline]           = gs_rgba_color(28, 28, 28, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground]        = gs_rgba_color(72, 72, 72, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered] = gs_rgba_color(60, 72, 60, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed] = gs_rgba_color(120, 128, 120, 255);

                // gizmos
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos]                  = gs_rgba_color(50, 50, 100, 200);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered]           = gs_rgba_color(100, 100, 172, 255);

                // gizmos
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text]                    = gs_rgba_color(255, 255, 255, 255);
            }

            ~ImmedidateUserInterfaceStyle(){}

            // getters
            float get_frames_radius() const
            {
                return gs_max(FramesRadius, 0.f);
            }

            float get_frames_width() const
            {
                return gs_max(FramesWidth, 4.f);
            }

            float get_font_size() const
            {
                return gs_max(32.f, FontSize);
            }

            float get_scrollbar_width() const
            {
                return gs_max(32.f, get_frames_radius() * 2.f, get_frames_width(), ScrollBarWidth);
            }

            float get_popup_menu_pointer_size() const
            {
                return gs_min(gs_max(PopupMenuPointerSize, 32.f), get_font_size() - 2.f * get_frames_width());
            }

            ApplicationRenderingBackendFont get_current_font() const
            {
                return Font;
            }

            gs_color get_color(const ImmediateUserInterfaceNodeColors_& _Color) const
            {
                return Colors[_Color];
            }

            // font

        private:

            // infos
            float                            FramesRadius         = 0.f;
            float                            FramesWidth          = 0.f;
            float                            FontSize             = 64.f;
            float                            ScrollBarWidth       = 16.f;
            float                            PopupMenuPointerSize = 32.f;
            std::vector<gs_color>            Colors;
            ApplicationRenderingBackendFont  Font;
        };

        struct ImmedidateUserInterfaceInput final
        {
            ImmedidateUserInterfaceInput(ImmediateUserInterfaceContextLayer* _Context = nullptr);

            gs_vec2f get_cusor_position() const;
            gs_vec2f get_cusor_drag_delta() const;
            gs_vec2f get_cusor_scroll_offset() const;

            bool is_mouse_button_down() const;
            bool is_mouse_button_hold() const;
            bool is_mouse_button_pressed() const;
            bool is_mouse_button_released() const;
            bool is_mouse_button_clicked() const;
            bool is_mouse_button_double_clicked() const;

            bool is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button& _Button) const;
            bool is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button& _Button) const;
            bool is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button) const;
            bool is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button& _Button) const;
            bool is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const;
            bool is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            bool is_key_down() const;
            bool is_key_hold() const;
            bool is_key_pressed() const;
            bool is_key_released() const;
            bool is_key_clicked() const;

            bool is_key_down(const ApplicationPlatformBackendKey::Key& _Button) const;
            bool is_key_hold(const ApplicationPlatformBackendKey::Key& _Button) const;
            bool is_key_pressed(const ApplicationPlatformBackendKey::Key& _Button) const;
            bool is_key_released(const ApplicationPlatformBackendKey::Key& _Button) const;
            bool is_key_clicked(const ApplicationPlatformBackendKey::Key& _Button) const;

        private:

            gs_vec2f CursorPosition   {gs_vec2f(0.f, 0.f)}; // cursor position projected onto UI surface
            gs_vec2f CursorDragDelta  {gs_vec2f(0.f, 0.f)}; // delta between cursor press position and it's current position
            gs_vec2f MouseScrollOffset{gs_vec2f(0.f, 0.f)}; // normalized mouse scroll offset

            // last accepted mouse
            // This variables show if any mouse button has been down, hold, pressed e.t.c
            Frenchie::Core::Optional<ApplicationPlatformBackendMouseButton::Button> MouseDown;
            Frenchie::Core::Optional<ApplicationPlatformBackendMouseButton::Button> MouseHold;
            Frenchie::Core::Optional<ApplicationPlatformBackendMouseButton::Button> MousePressed;
            Frenchie::Core::Optional<ApplicationPlatformBackendMouseButton::Button> MouseReleased;
            Frenchie::Core::Optional<ApplicationPlatformBackendMouseButton::Button> MouseClicked;
            Frenchie::Core::Optional<ApplicationPlatformBackendMouseButton::Button> MouseDoubleClicked;

            // last accepted key
            // This variables show if any mouse keyboard key has been down, hold, pressed e.t.c
            Frenchie::Core::Optional<ApplicationPlatformBackendKey::Key>            KeyDown;
            Frenchie::Core::Optional<ApplicationPlatformBackendKey::Key>            KeyHold;
            Frenchie::Core::Optional<ApplicationPlatformBackendKey::Key>            KeyPressed;
            Frenchie::Core::Optional<ApplicationPlatformBackendKey::Key>            KeyReleased;
            Frenchie::Core::Optional<ApplicationPlatformBackendKey::Key>            KeyClicked;
        };

        // nodes
        struct ImmediateUserInterfaceNode
        {
            ImmediateUserInterfaceNode(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNode();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context);
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context);
            virtual void measure(ImmediateUserInterfaceContextLayer* _Context);
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput& _Event);
            virtual void attach_child(ImmediateUserInterfaceNode* _Child);
            virtual void load_state(ImmediateUserInterfaceContextLayer*);
            virtual void save_state(ImmediateUserInterfaceContextLayer*);

            // getters
            gs_2dboxf get_visible_rect(ImmediateUserInterfaceContextLayer* _Context) const;
            bool is_partially_visible(ImmediateUserInterfaceContextLayer* _Context) const;

            int place_in_follow();

            struct Data
            {
                // rendering
                int                                Depth                      {0};
                int                                SelfThickness              {0}; // thickness of self rendered content
                int                                RenderingIndex             {0}; // index of the node within context rendering list
                int                                RenderingOrder             {0};
                int                                MaximumChildDepth          {0};
                int                                MaximumChildThickness      {0};
                bool                               PlaceInFollow              {false};
                bool                               OrderChildrenWhileRendering{false};

                // geometry
                gs_2dboxf                          BoundingBox                {gs_2dboxf(gs_vec2f(32.f, 32.f), gs_vec2f(1024.f, 512.f))};
                gs_vec2f                           ContentSize                {gs_vec2f(0.f)};
                gs_vec2f                           MinimumSize                {gs_vec2f(32.f)};
                gs_vec2f                           MaximumSize                {gs_vec2f((float)INT_MAX)};

                // hierarchy
                ImmediateUserInterfaceNode*        Parent                     {nullptr};
                ImmediateUserInterfaceNode*        Relative                   {nullptr};

                // settings
                ImmediateUserInterfaceNodeSettings Settings                   {ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable};

                // events
                ImmediateUserInterfaceNodeEvents   Events                     {ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None};

                // layout hints
                int NextLine = 0;

                // mouse hover
                ImmediateUserInterfaceNodeMouseHover           MouseHover{ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None};
                std::chrono::high_resolution_clock::time_point MouseEnterTimer;
                std::chrono::high_resolution_clock::time_point MouseLeaveTimer;

                // mouse and keyboard input
                ImmedidateUserInterfaceInput Input;
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
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput& _Event) override;

            virtual void create_contents(){} // place contents creation function here...

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

            float get_horizontal_scrollbar_width(ImmediateUserInterfaceContextLayer*) const;
            float get_vertical_scrollbar_width(ImmediateUserInterfaceContextLayer*) const;

            ImmediateUserInterfaceNode*                ContentView         = nullptr;
            ImmediateUserInterfaceScrollAreaScrollBar* VerticalScrollBar   = nullptr;
            ImmediateUserInterfaceScrollAreaScrollBar* HorizontalScrollBar = nullptr;

        private:
            void calculate_content_padding(ImmediateUserInterfaceContextLayer*, ImmediateUserInterfaceNode*, gs_vec2f&);
        };

        // widgets
        struct ImmediateUserInterfaceColorPickerGradientColorSelector;
        struct ImmediateUserInterfaceColorPickerGradientColorModifier;

        struct ImmediateUserInterfaceColorPicker : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceColorPicker(const std::string& _Hash) : ImmediateUserInterfaceNodePanel(_Hash){}
            virtual ~ImmediateUserInterfaceColorPicker(){}

            ImmediateUserInterfaceColorPickerGradientColorSelector*        GradientColorSelector        = nullptr;
            ImmediateUserInterfaceColorPickerGradientColorModifier* GradientSurfaceColorModifier = nullptr;
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
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput& _Event) override;
            virtual void attach_child(ImmediateUserInterfaceNode*   _Child) override;
            virtual void load_state(ImmediateUserInterfaceContextLayer*) override;
            virtual void save_state(ImmediateUserInterfaceContextLayer*) override;

            ImmediateUserInterfaceNode* Docker        {nullptr};
            ImmediateUserInterfaceNode* TopSnapper    {nullptr};
            ImmediateUserInterfaceNode* LeftSnapper   {nullptr};
            ImmediateUserInterfaceNode* RightSnapper  {nullptr};
            ImmediateUserInterfaceNode* BottomSnapper {nullptr};

            gs_2dboxf                   FrameBox;

            gs_2dboxf                   ContentBox;

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
            virtual void frame_debug(ImmediateUserInterfaceContextLayer*, const ImmedidateUserInterfaceInput&){}
            virtual void frame_render(ImmediateUserInterfaceContextLayer*){}
            virtual void frame_finish(ImmediateUserInterfaceContextLayer*){}
        };

        class ImmedidateUserInterfaceWindowController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceWindowController();
            virtual ~ImmedidateUserInterfaceWindowController();
            virtual void frame_start(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput& _Event) override;

            void push_event(std::function<void(ImmediateUserInterfaceContextLayer*)> _Event);

            std::vector<ImmediateUserInterfaceNode*>&
            retrieve_docked_windows(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Docker, const ImmedidateUserInterfaceDockingAnchor& _Anchors);

        private:

            void place_on_dockers(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput& _Event);
            bool can_be_docked(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked);
            void attach_to_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked, const ImmedidateUserInterfaceDockingAnchor& _Anchor);
            void detach_from_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Detached);

            mutable std::vector<ImmediateUserInterfaceNode*>                             m_WindowsDockingCache;
            mutable std::vector<ImmediateUserInterfaceNode*>                             m_WindowsDockingList;
            mutable std::stack<std::function<void(ImmediateUserInterfaceContextLayer*)>> m_DockingEventsStack;
            mutable ImmediateUserInterfaceNode*                                          m_WorkspaceDockArea      {nullptr};
            mutable bool                                                                 m_WorkspaceDockAreaOpened{true};
        };
    
        class ImmedidateUserInterfaceInputController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceInputController();
            virtual ~ImmedidateUserInterfaceInputController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput& _Event) override;

        private:

            void catch_hover(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput& _Event);
            void catch_input(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput& _Event);
            void catch_event(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput& _Event);
        };
    
        class ImmedidateUserInterfaceLayoutController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceLayoutController();
            virtual ~ImmedidateUserInterfaceLayoutController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput&) override;

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
            mutable Frenchie::Core::Optional<gs_vec2f> NextSize;
            mutable Frenchie::Core::Optional<gs_vec2f> NextPosition;
            mutable Frenchie::Core::Optional<gs_vec2f> NextContentPadding;
        };

        class ImmediateUserInterfaceScrollBarsController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceScrollBarsController();
            virtual ~ImmediateUserInterfaceScrollBarsController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceInput&) override;
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

            // scroll area
            bool begin_scrollarea(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings =
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults            |
                ImmediateUserInterfaceScrollAreaSettings_::ImmediateUserInterfaceScrollAreaSettings_Defaults);
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
            bool push_button(const std::string& _ID);
            
            bool check_button(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox,
                bool*                                     _Checked  = nullptr);
            
            void label(const std::string& _ID, const std::string& _Text);

            void color_picker(const std::string& _ID);
            bool menu_action(const std::string& _ID);

            // next node API
            void next_line();
            void next_size(const gs_vec2f&);
            void next_position(const gs_vec2f&);
            void next_content_padding(const gs_vec2f&);

            // windows
            bool begin_window(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
                bool*                                     _Opened = nullptr);
            void end_window();

            // auxiliary API
            template<typename Type> Type* get_controller() const
            {
                for(auto& controller : m_Controllers)
                {
                    if(dynamic_cast<Type*>(controller.get()))
                        return dynamic_cast<Type*>(controller.get());
                }

                return nullptr;
            }

            template<typename Type = ImmediateUserInterfaceNode> Type* get_rendering_stack_top() const
            {
                if(m_NodesRenderingStack.empty())
                    return nullptr;

                return dynamic_cast<Type*>(m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]);
            }

        // private: // TODO: make this private when finished

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
                    if(!m_NodesRenderingList.empty() && controller->NextLine.has_value())
                        m_NodesRenderingList[m_NodesRenderingList.size() - 1]->State.NextLine = controller->NextLine.value();

                    // next size
                    if(controller->NextSize.has_value())
                        node->State.BoundingBox = gs_2dboxf(node->State.BoundingBox.Min, node->State.BoundingBox.Min + controller->NextSize.value());

                    // next position
                    if(controller->NextPosition.has_value())
                        node->State.BoundingBox = gs_2dboxf(controller->NextPosition.value(), controller->NextPosition.value() + node->State.BoundingBox.size());

                    // next content padding
                    if(dynamic_cast<ImmediateUserInterfaceNodePanel*>(node) && controller->NextContentPadding.has_value())
                        dynamic_cast<ImmediateUserInterfaceNodePanel*>(node)->ContentPadding = controller->NextContentPadding.value();

                    // reset next item controller
                    controller->reset();
                }

                m_NodesRenderingList.push_back(node);
                m_NodesRenderingStack.push_back(node);

                return true;
            }

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

                m_NodesRenderingStack.pop_back();
            }

            mutable std::map<std::string, std::unique_ptr<ImmediateUserInterfaceNode>> m_Cache;
            mutable ImmedidateUserInterfaceStyle                                       m_Style;
            mutable ImmedidateUserInterfaceHierarchy                                   m_Hierarchy;

            // rendering
            mutable std::shared_ptr<RenderingQueue>                                    m_Renderer{nullptr};
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingList;
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingStack;

            // ini file
            ImmediateUserInterfaceContextConfiguration                              m_IniFileState;

            // settings
            ImmediateUserInterfaceContextSettings                                   m_Settings = ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking | ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking;

        private:

            // info
            std::vector<std::unique_ptr<ImmediateUserInterfaceContextController>> m_Controllers;
            std::string                                                           m_CurrentHash;
            std::string                                                           m_CurrentName;
            std::u32string                                                        m_IniFilePath = U"Frenchie.ini";

            // service methods
            template<typename Type> Type* create_node(const std::string& _ID)
            {
                // clean-up hash and name buffers
                m_CurrentHash.clear();
                m_CurrentName.clear();

                // determine hashable part of input id
                int hashable = 0;

                for (;hashable < (int)_ID.size(); hashable++)
                {
                    // TODO: here we should hash only if there are ### but now it is #
                    if(_ID[hashable] == '#')
                    {
                        int sharpCount = 1;
                        if(hashable + 1 < (int)_ID.size() && _ID[hashable + 1] == '#') ++sharpCount;
                        if(hashable + 2 < (int)_ID.size() && _ID[hashable + 2] == '#') ++sharpCount;
                        if(sharpCount >= 3) break;
                    }
                }
                
                // create node
                m_CurrentHash.append(
                    (hashable < _ID.size() ? _ID.c_str() + hashable : _ID.c_str()),
                    (hashable < _ID.size() ? _ID.size() - hashable : _ID.size()));

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