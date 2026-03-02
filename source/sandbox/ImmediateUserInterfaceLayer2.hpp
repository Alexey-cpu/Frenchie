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

            // custom event
            ImmediateUserInterfaceNodeEvents_Custom               = 1 << 9,
        };

        // colors
        enum ImmediateUserInterfaceNodeColors_ : int
        {
            ImmediateUserInterfaceNodeColors_Begin         = 0,

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

            // scroll area
            ImmediateUserInterfaceNodeColors_ScrollAreaOutline,                                         // scroll area outline
            ImmediateUserInterfaceNodeColors_ScrollAreaBackground,                                      // scroll area background

            // scrollbar
            ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground,                                 // scroll bar slider background color
            ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered,                          // hovered scroll bar slider background color

            // menus
            ImmediateUserInterfaceNodeColors_MenuActionBackground,                                      // menu action background
            ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered,                               // hovered menu action background
            ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed,                               // hovered menu action background

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
            ImmediateUserInterfaceNodeSettings_None                                   = 0,

            // modifications
            ImmediateUserInterfaceNodeSettings_Movable                                = 1 << 0,
            ImmediateUserInterfaceNodeSettings_Resizable                              = 1 << 1,
            ImmediateUserInterfaceNodeSettings_NullParent                             = 1 << 2,

            // content alignment
            ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop            = 1 << 3,
            ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter         = 1 << 4,
            ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom         = 1 << 5,
            ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft         = 1 << 6,
            ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter       = 1 << 7,
            ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight        = 1 << 8,

            // scrollbars
            ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar                 = 1 << 9,
            ImmediateUserInterfaceNodeSettings_AlwaysVerticalScrollBar                = 1 << 10,
            ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar              = 1 << 11,
            ImmediateUserInterfaceNodeSettings_NeverHorizontalScrollBar               = 1 << 12,
            ImmediateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar              = 1 << 13,
            ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar            = 1 << 14,
            ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically             = 1 << 15,
            ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally           = 1 << 16,
            ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment  = 1 << 17,
            ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment   = 1 << 18,
            ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment = 1 << 19,

            ImmediateUserInterfaceNodeSettings_InvisibleVerticalScrollBar             = 1 << 20,
            ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar           = 1 << 21,

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
            ImmediateUserInterfaceContextSettings_DisableDocking         = 1 << 0,
            ImmediateUserInterfaceContextSettings_EnableWindowsDocking   = 1 << 1,
            ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking = 1 << 2,

            // highlighting
            ImmediateUserInterfaceContextSettings_HighlighHoveredNodes   = 1 << 3,
        };

        enum ImmediateUserInterfaceCheckButtonSettings_ : int
        {
            ImmediateUserInterfaceCheckButtonSettings_Checkbox     = 1 << 0,
            ImmediateUserInterfaceCheckButtonSettings_RadioButton  = 1 << 1,
            ImmediateUserInterfaceCheckButtonSettings_SliderButton = 1 << 2
        };

        enum ImmediateUserInterfaceInputStringSettings_ : int
        {
            ImmediateUserInterfaceInputStringSettings_NoInput     = 1 << 0, // disables input
            ImmediateUserInterfaceInputStringSettings_NoClipboard = 1 << 1, // disables copy/paste
            ImmediateUserInterfaceInputStringSettings_NoSelection = 1 << 2, // disables selection capabilities

            ImmediateUserInterfaceInputStringSettings_Defaults    = 0
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
        typedef int ImmediateUserInterfaceNodeMouseHover;

        typedef int ImmediateUserInterfaceNodeSettings;
        typedef int ImmediateUserInterfaceContextSettings;
        typedef int ImmediateUserInterfaceCheckButtonSettings;
        typedef int ImmediateUserInterfaceInputStringSettings;

        typedef int ImmedidateUserInterfaceDockingAnchor;
        typedef int ImmedidateUserInterfaceRenderingOrder;
        typedef int ImmedidateUserInterfaceRenderingLayer;

        class ImmediateUserInterfaceContextLayer;

        // style and events
        struct ImmedidateUserInterfaceStyle
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
            float                            FramesRadius         = 32.f;
            float                            FramesWidth          = 0.f;
            float                            FontSize             = 64.f;
            float                            ScrollBarWidth       = 32.f;
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
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void attach_child(ImmediateUserInterfaceNode*   _Child) override;
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

            mutable std::vector<ImmediateUserInterfaceNode*>                             m_WindowsDockingCache;
            mutable std::vector<ImmediateUserInterfaceNode*>                             m_WindowsDockingList;
            mutable ImmediateUserInterfaceNode*                                          m_WorkspaceDockArea      {nullptr};
            mutable bool                                                                 m_WorkspaceDockAreaOpened{true};
        };
    
        class ImmedidateUserInterfaceInputController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceInputController();
            virtual ~ImmedidateUserInterfaceInputController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context) override;

        private:

            void catch_hover(ImmediateUserInterfaceContextLayer* _Context);
            void catch_event(ImmediateUserInterfaceContextLayer* _Context);
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
            mutable Frenchie::Core::Optional<gs_vec2f> NextSize;
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
            bool push_button(const std::string& _ID);

            bool menu_action(const std::string& _ID);
            
            bool check_button(
                const std::string&                               _ID,
                const ImmediateUserInterfaceCheckButtonSettings& _Settings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox,
                bool*                                            _Checked  = nullptr);
            
            void label(const std::string& _ID, const std::string& _Text);

            void input_string(
                const std::string&                               _ID,
                std::string&                                     _Text,
                const ImmediateUserInterfaceInputStringSettings& _InputSettings = ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Defaults,
                const ImmediateUserInterfaceNodeSettings&        _NodeSettings  = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);

            void color_picker(const std::string& _ID);

            // windows
            bool begin_window(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
                bool*                                     _Opened = nullptr);
            void end_window();

            // next node API
            void next_line();
            void next_size(const gs_vec2f&);
            void next_position(const gs_vec2f&);
            void next_content_padding(const gs_vec2f&);

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

            // hierarchy and cache
            mutable std::map<std::string, std::unique_ptr<ImmediateUserInterfaceNode>> m_Cache;
            mutable ImmedidateUserInterfaceHierarchy                                   m_Hierarchy;

            // rendering
            mutable std::shared_ptr<RenderingQueue>                                    m_Renderer{nullptr};
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingList;
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingStack;

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