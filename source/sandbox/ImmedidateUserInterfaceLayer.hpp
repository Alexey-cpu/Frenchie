#pragma once

#include <FrenchieApplicationImmediate2DRenderer.hpp>

// STL
#include <type_traits>
#include <memory>
#include <chrono>
#include <stack>
#include <map>
#include <set>

namespace Frenchie
{
    namespace Application
    {
        struct ImmedidateUserInterfaceStyle;
        struct ImmedidateUserInterfaceNode;
        struct ImmedidateUserInterfaceNodeState;

        enum ImmedidateUserInterfaceColors_ : int
        {
            // application window
            ImmedidateUserInterfaceColors_WindowResizeGizmoColor,
            ImmedidateUserInterfaceColors_WindowContentSpaceColor,
            ImmedidateUserInterfaceColors_WindowDecorationFrameColor,
            ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor,
            ImmedidateUserInterfaceColors_WindowDecorationFrameFrameColor,

            // application window close buttons
            ImmedidateUserInterfaceColors_WindowCloseButtonDefaultColor,
            ImmedidateUserInterfaceColors_WindowCloseButtonHoveredColor,
            ImmedidateUserInterfaceColors_WindowCloseButtonPressedColor,

            // application window scroll bar
            ImmedidateUserInterfaceColors_WindowScrollBarColor,
            ImmedidateUserInterfaceColors_WindowScrollBarSliderColor,
            ImmedidateUserInterfaceColors_WindowScrollBarSliderHoveredColor,
            ImmedidateUserInterfaceColors_WindowScrollBarSliderPressedColor,

            // application text
            ImmedidateUserInterfaceColors_TextEnabledColor,
            ImmedidateUserInterfaceColors_TextDisabledColor,
            ImmedidateUserInterfaceColors_TextHoveredColor,

            // push button background
            ImmedidateUserInterfaceColors_PushButtonEnabledBackgroundColor,
            ImmedidateUserInterfaceColors_PushButtonEnabledHoveredBackgroundColor,
            ImmedidateUserInterfaceColors_PushButtonEnabledPressedBackgroundColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledBackgroundColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledHoveredBackgroundColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledPressedBackgroundColor,

            // push button frame
            ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor,
            ImmedidateUserInterfaceColors_PushButtonEnabledHoveredFrameColor,
            ImmedidateUserInterfaceColors_PushButtonEnabledPressedFrameColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledHoveredFrameColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledPressedFrameColor,

            // // radio buton
            // ImmedidateUserInterfaceColors_RadioButtonPressedEnabledColor,
            // ImmedidateUserInterfaceColors_RadioButtonPressedDisabledColor,

            ImmedidateUserInterfaceColors_ColorEnd,
        };

        enum ImmedidateUserInterfaceNodeType_ : int
        {
            ImmedidateUserInterfaceNodeType_Node                 = 0,
            ImmedidateUserInterfaceNodeType_Window               = 1,
            ImmedidateUserInterfaceNodeType_WindowMenu           = 2,
            ImmedidateUserInterfaceNodeType_WindowMenubar        = 3,
            ImmedidateUserInterfaceNodeType_WindowDefaultFrame   = 4
        };

        enum ImmedidateUserInterfaceNodeLayer_ : int
        {
            ImmedidateUserInterfaceNodeLayer_Begin   = 0,
            ImmedidateUserInterfaceNodeLayer_Docking = ImmedidateUserInterfaceNodeLayer_Begin,
            ImmedidateUserInterfaceNodeLayer_Main,
            ImmedidateUserInterfaceNodeLayer_DockingActive,
            ImmedidateUserInterfaceNodeLayer_Focus,
            ImmedidateUserInterfaceNodeLayer_Popups,
            ImmedidateUserInterfaceNodeLayer_End,
        };

        enum ImmedidateUserInterfaceNodeChanges_ : int
        {
            ImmedidateUserInterfaceNodeChanges_None                        = 0,
            
            // move
            ImmedidateUserInterfaceNodeChanges_IsBeingMoved                = 1 << 0,

            // resize
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft       = 1 << 1,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight      = 1 << 2,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft    = 1 << 3,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight   = 1 << 4,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop           = 1 << 5,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft          = 1 << 6,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight         = 1 << 7,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom        = 1 << 8,

            // focus
            ImmedidateUserInterfaceNodeChanges_IsBeingFocused              = 1 << 9,

            // scroll
            ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally = 1 << 10,
            ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically   = 1 << 11,

            // mouse hover
            ImmedidateUserInterfaceNodeChanges_IsBeingMouseHoveredStarted  = 1 << 12,
            ImmedidateUserInterfaceNodeChanges_IsBeingMouseHovered         = 1 << 13,
            ImmedidateUserInterfaceNodeChanges_IsBeingMouseHoveredEnded    = 1 << 14,
        };

        enum ImmedidateUserInterfaceNodeSettings_ : int
        {
            ImmedidateUserInterfaceNodeSettings_None                               = 0,

            // common
            ImmedidateUserInterfaceNodeSettings_Movable                            = 1 << 0,
            ImmedidateUserInterfaceNodeSettings_Resizable                          = 1 << 1,

            // scrollbars
            ImmedidateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar          = 1 << 2,
            ImmedidateUserInterfaceNodeSettings_AlwaysVerticalScrollBar            = 1 << 3,
            ImmedidateUserInterfaceNodeSettings_NeverHorizontalScrollBar           = 1 << 4,
            ImmedidateUserInterfaceNodeSettings_NeverVerticalScrollBar             = 1 << 5,

            // layouting
            ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally         = 1 << 6,
            ImmedidateUserInterfaceNodeSettings_LayoutChildrenVertically           = 1 << 7,
            ImmedidateUserInterfaceNodeSettings_LayoutSetAllChildrenSameSize       = 1 << 8,

            // size hints
            ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally       = 1 << 9,
            ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically         = 1 << 10,

            // clipping
            ImmedidateUserInterfaceNodeSettings_IgnoreClipping                     = 1 << 11,
            
            // focus
            ImmedidateUserInterfaceNodeSettings_IgnoreFocus                        = 1 << 12,

            // parenting
            ImmedidateUserInterfaceNodeSettings_NullParent                         = 1 << 13,

            ImmedidateUserInterfaceNodeSettings_ResizeToContents =
                ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically |
                ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally,

            ImmedidateUserInterfaceNodeSettings_AlwaysScrollBar =
                ImmedidateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar |
                ImmedidateUserInterfaceNodeSettings_AlwaysVerticalScrollBar,

            ImmedidateUserInterfaceNodeSettings_NeverScrollBar =
                ImmedidateUserInterfaceNodeSettings_NeverHorizontalScrollBar |
                ImmedidateUserInterfaceNodeSettings_NeverVerticalScrollBar,

            ImmedidateUserInterfaceNodeHints_Default       =
                ImmedidateUserInterfaceNodeSettings_Movable   |
                ImmedidateUserInterfaceNodeSettings_Resizable |
                ImmedidateUserInterfaceNodeSettings_LayoutChildrenVertically
        };

        typedef int ImmedidateUserInterfaceNodeType;
        typedef int ImmedidateUserInterfaceNodeLayer;
        typedef int ImmedidateUserInterfaceNodeChanges;
        typedef int ImmedidateUserInterfaceNodeSettings;

        struct ImmedidateUserInterfaceStyle
        {
            ImmedidateUserInterfaceStyle()
            {
                // setup push button color scheme

                // applciation windows
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowResizeGizmoColor                  ] = gs_vec4f(5, 255, 255, 200.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceColor                 ] = gs_vec4f(125, 204, 250, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameColor              ] = gs_vec4f(5, 126, 255, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor            ] = gs_vec4f(15, 47, 66, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowDecorationFrameFrameColor         ] = gs_vec4f(15, 47, 66, 255.f);

                // application windows close button
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonDefaultColor           ] = gs_vec4f(80.f, 0.f, 0.f, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonHoveredColor           ] = gs_vec4f(120.f, 0.f, 0.f, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowCloseButtonPressedColor           ] = gs_vec4f(40.f, 0.f, 0.f, 255.f);

                // application windows scroll bar
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarColor                    ] = gs_vec4f(125, 10, 250, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderColor              ] = gs_vec4f(125, 200, 250, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderHoveredColor       ] = gs_vec4f(150, 250, 250, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_WindowScrollBarSliderPressedColor       ] = gs_vec4f(170, 170, 250, 255.f);
                // application text
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextEnabledColor                         ] = gs_vec4f(255.f, 255.f, 255.f, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextDisabledColor                        ] = gs_vec4f(200.f, 200.f, 200.f, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextHoveredColor                         ] = gs_vec4f(255.f, 32.f, 32.f, 255.f);

                // Push button
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledBackgroundColor        ] = gs_vec4f(4, 41, 189, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredBackgroundColor ] = gs_vec4f(60, 98, 250, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedBackgroundColor ] = gs_vec4f(87, 120, 250, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledBackgroundColor       ] = gs_vec4f(53, 53, 54, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredBackgroundColor] = gs_vec4f(87, 87, 89, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedBackgroundColor] = gs_vec4f(117, 117, 120, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor             ] = gs_vec4f(1, 16, 77, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredFrameColor      ] = gs_vec4f(0, 16, 77, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedFrameColor      ] = gs_vec4f(1, 32, 77, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor            ] = gs_vec4f(30, 30, 31, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredFrameColor     ] = gs_vec4f(30, 1, 31, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedFrameColor     ] = gs_vec4f(30, 12, 31, 255);

                // // radio button
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_RadioButtonPressedEnabledColor          ] = gs_vec4f(255, 255, 255, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_RadioButtonPressedDisabledColor         ] = gs_vec4f(200, 200, 200, 255);

                // radio button

            }

            RenderingQueueFont Font;
            float              FontSize                          = 64.f;

            float              WindowResizeAngleGizmoRadius      = 32.f;
            float              WindowResizeSideGizmoWidth        = 12.f;
            float              WindowFrameCloseMinimumButtonSize = 32.f;
            float              WindowScrollBarSliderWidth        = 32.f;

            float              FrameWidth                        = 8.f;
            float              FrameRoundingRadius               = 16.f;

            gs_vec4f Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_ColorEnd]{};
        };

        struct ImmedidateUserInterfaceNodeScroll
        {
            void setup(
                const float& _Min,
                const float& _Max,
                const float  _Total,
                const float& _Track,
                const float&  _MinSize = 64.f // TODO: this MUST BE a setting !!!!
            )
            {
                Min          = _Min;
                Max          = _Max;
                SliderScale  = gs_abs(_Total) / gs_abs(_Max - _Min);
                SliderLength = gs_max(gs_abs(_Max - _Min) / gs_abs(_Total) * _Track, _MinSize);
            }

            void reposition(const float& _Position)
            {
                SliderPosition = gs_clamp(_Position, 0.f, gs_abs(Max - SliderLength));
            }

            float Min;
            float Max;
            float SliderScale;
            float SliderLength;
            float SliderPosition;
        };

        struct ImmedidateUserInterfaceNodeState
        {
            // hints
            mutable ImmedidateUserInterfaceNodeType     Type     {ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Node        };
            mutable ImmedidateUserInterfaceNodeLayer    Layer    {ImmedidateUserInterfaceNodeLayer_::ImmedidateUserInterfaceNodeLayer_Main     };
            mutable ImmedidateUserInterfaceNodeSettings Settings {ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeHints_Default};
            mutable ImmedidateUserInterfaceNodeChanges  Changes  {ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_None  };

            // hierarchy
            mutable ImmedidateUserInterfaceNode* Parent     {nullptr};
            mutable ImmedidateUserInterfaceNode* Creator    {nullptr};
            mutable int                          Thickness  {0};
            mutable int                          LayerDepth {0};
            mutable int                          ChildCount {0};
            mutable int                          ChildIndex {0};
            mutable int                          DrawIndex  {0};

            // layouting
            mutable gs_vec2f LayoutTotalChildrenSize{gs_vec2f(0.f, 0.f)};
            mutable gs_vec2f LayoutCursorDirection  {gs_vec2f(0.f, 1.f)};
            mutable gs_vec2f LayoutCursorPositon    {gs_vec2f(0.f, 0.f)};
            mutable gs_vec2f LayoutCursorSize       {gs_vec2f(0.f, 0.f)};
            mutable float    WindowMinimumWidth     {4.f};
            mutable float    WindowMinimumHeight    {4.f};
            mutable float    WindowMaximumWidth     {(float)INT_MAX};
            mutable float    WindowMaximumHeight    {(float)INT_MAX};

            // docking
            mutable ImmedidateUserInterfaceNode* Docker        {nullptr};
            mutable gs_2dboxf                    DockArea      {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(512.f, 512.f))};
            mutable int                          DockNodesCount{0};

            // scrolling
            mutable ImmedidateUserInterfaceNodeScroll VerticalScrollBar;
            mutable ImmedidateUserInterfaceNodeScroll HorizontalScrollBar;

            // geometry
            mutable gs_vec2f  ScrollBarOffset             {gs_vec2f(0.f, 0.f)};
            mutable gs_2dboxf WindowBox                   {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(512.f, 512.f))}; 
            mutable gs_2dboxf InnerClipAreaBox            {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(512.f, 512.f))};
            mutable gs_2dboxf OuterClipAreaBox            {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf ViewportBox                 {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf ScrollAreaBox               {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf ContentBox                  {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf VerticalScrollBarBox        {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf VerticalScrollBarSliderBox  {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf HorizontalScrollBarBox      {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf HorizontalScrollBarSliderBox{gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_mat4f  Transform                   {gs_mat4f(1.f)};

            // timers
            std::chrono::high_resolution_clock::time_point WindowHoverStart;
            std::chrono::high_resolution_clock::time_point WindowHoverEnd;

            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseDown;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseHold;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MousePressed;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseClicked;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseDoubleClicked;
        };

        struct ImmedidateUserInterfaceNode
        {
            mutable std::string                      Name {"Default"                         }; // TODO: this MUST BE A HASH !!!
            mutable ImmedidateUserInterfaceNodeState State{ImmedidateUserInterfaceNodeState()};
            mutable ImmedidateUserInterfaceNodeState Cache{ImmedidateUserInterfaceNodeState()};
        };

        struct ImmedidateUserInterfaceMenu
        {
            ImmedidateUserInterfaceNode* Self      {nullptr};
            ImmedidateUserInterfaceNode* Observable{nullptr};
        };

        class ImmedidateUserInterfaceContextLayer : public Layer
        {
        public:
            ImmedidateUserInterfaceContextLayer();
            virtual ~ImmedidateUserInterfaceContextLayer();

            virtual bool awake()        override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;

            // setters
            void set_next_ui_node_maximum_size(const gs_vec2f&);
            void set_next_ui_node_position(const gs_vec2f&);
            void set_next_ui_node_size(const gs_vec2f&);
            void set_next_ui_node_cursor_same_line();

            // cache
            bool                         ui_node_cache_is_empty() const;
            ImmedidateUserInterfaceNode* ui_node_cache_request(const std::string&, const ImmedidateUserInterfaceNodeType&) const;

            // hierarchy
            bool                         ui_node_hierarchy_is_empty() const;
            ImmedidateUserInterfaceNode* ui_node_hierarchy_top()      const;

            // hierarchical elements
            bool begin_window(const std::string& _Name, bool* _Rendered = nullptr);
            void end_window();

            bool begin_menu(const std::string& _Name);
            void end_menu();

            bool begin_menubar(const std::string& _Name);
            void end_menubar();

            // widgets API
            bool widget_push_button(const std::string& _Name);
            bool widget_menu_button(const std::string& _Name);

        protected:

            // style
            mutable std::shared_ptr<ImmedidateUserInterfaceStyle> m_Style   {nullptr};
            
            std::string HoveredName;

            // rendering
            mutable std::shared_ptr<Immediate2DRenderer>            m_Renderer      {nullptr};
            mutable std::map<ImmedidateUserInterfaceNodeLayer, int> m_RendererLayers{std::map<ImmedidateUserInterfaceNodeLayer, int>()};

            // cache
            mutable std::map<
                ImmedidateUserInterfaceNodeType,
                std::map<
                    std::string,
                    std::unique_ptr<ImmedidateUserInterfaceNode>>> m_NodesCache
            {
                std::map<
                ImmedidateUserInterfaceNodeType,
                std::map<
                    std::string,
                    std::unique_ptr<ImmedidateUserInterfaceNode>>>()
            };
            
            // nodes
            mutable std::vector<ImmedidateUserInterfaceNode*>                m_NodesRenderingStack     {std::vector<ImmedidateUserInterfaceNode*>()};
            mutable std::vector<ImmedidateUserInterfaceNode*>                m_NodesRenderingList      {std::vector<ImmedidateUserInterfaceNode*>()};
            mutable std::vector<ImmedidateUserInterfaceNode*>                m_NodesRenderingCache     {std::vector<ImmedidateUserInterfaceNode*>()};

            mutable Frenchie::Core::Optional<gs_vec2f>                       m_NextNodeSize           {Frenchie::Core::Optional<gs_vec2f>()};
            mutable Frenchie::Core::Optional<gs_vec2f>                       m_NextNodePosition       {Frenchie::Core::Optional<gs_vec2f>()};
            mutable Frenchie::Core::Optional<gs_vec2f>                       m_NextNodeMaximumSize    {Frenchie::Core::Optional<gs_vec2f>()};
            mutable Frenchie::Core::Optional<gs_vec2f>                       m_NextNodeCursorDirection{Frenchie::Core::Optional<gs_vec2f>()};

            // events
            mutable std::map<
                ImmedidateUserInterfaceNodeChanges_,
                Frenchie::Core::Optional<ImmedidateUserInterfaceNode*>> m_EventsQueue;

            // popups
            mutable std::list<ImmedidateUserInterfaceMenu>              m_ActiveMenusRenderingList{std::list<ImmedidateUserInterfaceMenu>()};

            // widgets
            mutable Frenchie::Core::Optional<bool>                           m_WidgetIsBeingMouseHovered      {Frenchie::Core::Optional<bool>()};
            mutable Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetIsBeingMouseDown         {Frenchie::Core::Optional<ApplicationMouseButton::Button>()};
            mutable Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetIsBeingMousePressed      {Frenchie::Core::Optional<ApplicationMouseButton::Button>()};
            mutable Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetIsBeingMouseClicked      {Frenchie::Core::Optional<ApplicationMouseButton::Button>()};
            mutable Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetIsBeingMouseDoubleClicked{Frenchie::Core::Optional<ApplicationMouseButton::Button>()};

            mutable Frenchie::Core::Optional<bool>                           m_WidgetHasBeenMouseHovered      {Frenchie::Core::Optional<bool>()};
            mutable Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetHasBeenMouseDown         {Frenchie::Core::Optional<ApplicationMouseButton::Button>()};
            mutable Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetHasBeenMousePressed      {Frenchie::Core::Optional<ApplicationMouseButton::Button>()};
            mutable Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetHasBeenMouseClicked      {Frenchie::Core::Optional<ApplicationMouseButton::Button>()};
            mutable Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetHasBeenMouseDoubleClicked{Frenchie::Core::Optional<ApplicationMouseButton::Button>()};
            
            // user interface nodes API
            bool begin_node(
                const std::string&                  _Name,
                bool*                               _Rendered = nullptr,
                ImmedidateUserInterfaceNodeSettings _Settings = ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeHints_Default,
                ImmedidateUserInterfaceNodeType     _Type     = ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Node);

            void end_node();

            void ui_node_calculate_geometry(ImmedidateUserInterfaceNode*) const;
            int  ui_node_calculate_layer_depth(const ImmedidateUserInterfaceNodeLayer&) const;
            int  ui_node_calculate_child_depth_placed_in_follow(ImmedidateUserInterfaceNode* _Where, const int& _ChildThickness) const;
            
            gs_vec2f ui_node_vertical_cursor_direction() const;
            gs_vec2f ui_node_horizontal_cursor_direction() const;
            
            bool ui_node_is_of_type(const ImmedidateUserInterfaceNode*, const ImmedidateUserInterfaceNodeType&) const;
            bool ui_node_is_vertical_scroll_bar_needed(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_horizontal_scroll_bar_needed(const ImmedidateUserInterfaceNode*) const;

            // docking
            bool ui_node_begin_attaching_to_a_docker(const ImmedidateUserInterfaceNode* _Node, ImmedidateUserInterfaceNode* _Docker);
            bool ui_node_is_being_docked_by(const ImmedidateUserInterfaceNode* _Node, ImmedidateUserInterfaceNode* _Docker);
            bool ui_node_is_being_docker(const ImmedidateUserInterfaceNode* _Node);
            bool ui_node_is_being_docked(const ImmedidateUserInterfaceNode* _Node);
            bool ui_node_end_attaching_to_a_docker(const ImmedidateUserInterfaceNode* _Node);

            // modification
            bool ui_node_is_being_hovered(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_mouse_down(const ImmedidateUserInterfaceNode*, const ApplicationMouseButton::Button&) const;
            bool ui_node_is_being_mouse_hold(const ImmedidateUserInterfaceNode*, const ApplicationMouseButton::Button&) const;
            bool ui_node_is_being_mouse_pressed(const ImmedidateUserInterfaceNode*, const ApplicationMouseButton::Button&) const;
            bool ui_node_is_being_mouse_clicked(const ImmedidateUserInterfaceNode*, const ApplicationMouseButton::Button&) const;
            bool ui_node_is_being_mouse_double_clicked(const ImmedidateUserInterfaceNode*, const ApplicationMouseButton::Button&) const;

            bool ui_node_is_being_resized_top_left(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_resized_top_right(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_resized_bottom_left(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_resized_bottom_right(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_resized_top(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_resized_left(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_resized_right(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_resized_bottom(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_resized(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_moved(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_modified(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_focused(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_scrolled(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_scrolled_horizontally(const ImmedidateUserInterfaceNode*) const;
            bool ui_node_is_being_scrolled_vertically(const ImmedidateUserInterfaceNode*) const;

            //------------------------------------------------------------------------------------------------
            // events query
            //------------------------------------------------------------------------------------------------
            bool ui_node_event_has_been_enqueued(const ImmedidateUserInterfaceNodeChanges&);

            void ui_node_enqueue_focused_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_focused_node();

            void ui_node_enqueue_resize_top_left_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_resize_top_left_node();

            void ui_node_enqueue_resize_top_right_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_resize_top_right_node();

            void ui_node_enqueue_resize_bottom_left_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_resize_bottom_left_node();

            void ui_node_enqueue_resize_bottom_right_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_resize_bottom_right_node();

            void ui_node_enqueue_resize_top_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_resize_top_node();

            void ui_node_enqueue_resize_left_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_resize_left_node();

            void ui_node_enqueue_resize_right_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_resize_right_node();

            void ui_node_enqueue_resize_bottom_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_resize_bottom_node();

            void ui_node_enqueue_moving_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_moving_node();

            void ui_node_enqueue_vertical_scroll_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_vertical_scroll_node();

            void ui_node_enqueue_horizontal_scroll_node(ImmedidateUserInterfaceNode*);
            bool ui_node_dequeue_horizontal_scroll_node();

            void ui_node_event_dequeue_finish(ImmedidateUserInterfaceNode* _Sink);
            //------------------------------------------------------------------------------------------------

            void ui_node_begin_hover(ImmedidateUserInterfaceNode*);

            void ui_node_end_hover(ImmedidateUserInterfaceNode* _Window);
            void ui_node_end_resize(ImmedidateUserInterfaceNode*);
            void ui_node_end_move(ImmedidateUserInterfaceNode*);
            void ui_node_end_focus(ImmedidateUserInterfaceNode*);
            void ui_node_end_scroll(ImmedidateUserInterfaceNode*);

            // rendering
            bool ui_node_render_clipbox(ImmedidateUserInterfaceNode*);
            bool ui_node_render_background(ImmedidateUserInterfaceNode*);
            bool ui_node_render_background_frame(ImmedidateUserInterfaceNode*);
            bool ui_node_render_vertical_scrollbar(ImmedidateUserInterfaceNode*);
            bool ui_node_render_horizontal_scrollbar(ImmedidateUserInterfaceNode*);
            bool ui_node_render_resize_events_gizmos(ImmedidateUserInterfaceNode*);

            void ui_node_layout_children();
            void ui_node_receive_events();
            void ui_node_process_events();
            void ui_node_save_state();
            
            // user interface widgets API
            auto widget_prepare_for_rendering(const gs_vec2f&,    bool = true, bool = true);
            auto widget_prepare_for_rendering(const std::string&, bool = true, bool = true);

            bool widget_is_being_hovered() const;
            bool widget_is_being_pressed(const ApplicationMouseButton::Button&) const;
            bool widget_is_being_clicked(const ApplicationMouseButton::Button&) const;
            bool widget_is_being_mouse_down(const ApplicationMouseButton::Button&) const;
            bool widget_is_being_double_clicked(const ApplicationMouseButton::Button&) const;

            bool widget_has_been_hovered() const;
            bool widget_has_been_pressed(const ApplicationMouseButton::Button&) const;
            bool widget_has_been_clicked(const ApplicationMouseButton::Button&) const;
            bool widget_has_been_mouse_down(const ApplicationMouseButton::Button&) const;
            bool widget_has_been_double_clicked(const ApplicationMouseButton::Button&) const;

            friend class ImmedidateUserInterfaceContextEventsReceiver;

            struct Hierarchy
            {
                std::vector<int>                          Indexes;
                std::vector<int>                          Entries;
                std::vector<ImmedidateUserInterfaceNode*> Sorted;

                void build(const std::vector<ImmedidateUserInterfaceNode*>& Nodes)
                {
                    std::vector<int> workspace(Nodes.size()+1);

                    Indexes.resize(Nodes.size() + 1);
                    Entries.resize(Nodes.size());
                    Sorted.resize(Nodes.size());

                    for(int i = 0; i < (int)Entries.size(); i++)
                    {
                        Entries[i] = 0;
                        Indexes[i] = 0;
                        Sorted [i] = Nodes[i];
                    }

                    // count items
                    for (int i = 0; i < (int)Nodes.size(); i++)
                    {
                        if(Nodes[i]->State.Parent == nullptr)
                            continue;

                        ++Entries[Nodes[i]->State.Parent->State.DrawIndex];
                    }

                    // cumulative sum
                    int sum = 0;
                    for (int i = 0; i < Nodes.size(); i++)
                    {
                        Indexes  [i] = sum;
                        workspace[i] = sum;
                        sum += Entries[i];
                    }
                    Indexes[Nodes.size()] = sum;

                    for(int i = 0; i < Nodes.size(); i++ )
                    {
                        if(Nodes[i]->State.Parent == nullptr)
                            continue;

                        Sorted[workspace[Nodes[i]->State.Parent->State.DrawIndex]++] = Nodes[i];
                    }
                }
            } m_Hierarchy;
        };
    }
}

// template<typename Type> struct Tree;

// template<typename Type>
// struct Node
// {
//     int               Parent{-1};
//     int               Index {-1};
//     Type              Data  {Type()};
//     const Tree<Type>* Tree  {nullptr};
// };

// template<typename Type>
// struct Tree
// {
//     mutable std::vector<Node<Type>> Nodes  {std::vector<Node<Type>>()};
//     mutable std::vector<int>        Indexes{std::vector<int>()};
//     mutable std::vector<int>        Entries{std::vector<int>()};
//     mutable bool                    Dirty  {true};

//     template<typename ... Args>
//     Node<Type> construct_node(const Node<Type>& _Parent, Args ... _Args)
//     {
//         Node<Type> node;
//         node.Parent = _Parent.Index;
//         node.Index  = (int)Nodes.size();
//         node.Data   = Type(_Args ...);
//         node.Tree   = this;
//         Nodes.push_back(node);
//         Dirty = true;
//         return node;
//     }

//     void clear()
//     {
//         Nodes.clear();
//     }

//     void sort() const
//     {
//         std::vector<Node<Type>> nodes(Nodes.size());
//         std::vector<int> workspace(Nodes.size()+1);

//         Indexes.resize(Nodes.size() + 1);
//         Entries.resize(Nodes.size());

//         for(int i = 0; i < Entries.size(); i++)
//         {
//             Entries[i] = 0;
//             Indexes[i] = 0;
//         }

//         // count items
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             if(Nodes[i].Parent < 0) continue;
//             ++Entries[Nodes[i].Parent];
//         }

//         // cumulative sum
//         int sum = 0;
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             Indexes  [i] = sum;
//             workspace[i] = sum;
//             sum += Entries[i];
//         }
//         Indexes[Nodes.size()] = sum;

//         for(int i = 0; i < Nodes.size(); i++ )
//         {
//             if(Nodes[i].Parent < 0) continue;
//             nodes[workspace[Nodes[i].Parent]++] = Nodes[i];
//         }

//         Nodes = nodes;
//         Dirty = false;
//     }
// };