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

        enum ImmedidateUserInterfaceNodeType_
        {
            ImmedidateUserInterfaceNodeType_Node          = 1 << 0,
            ImmedidateUserInterfaceNodeType_Window        = 1 << 1,
            
            ImmedidateUserInterfaceNodeType_WindowFrame   = 1 << 2,
            ImmedidateUserInterfaceNodeType_WindowMenubar = 1 << 3,
            ImmedidateUserInterfaceNodeType_WindowContent = 1 << 4,
            ImmedidateUserInterfaceNodeType_WindowMain    = 1 << 5,

            ImmedidateUserInterfaceNodeType_Menu          = 1 << 6,
        };

        enum ImmedidateUserInterfaceNodeChanges_ : int
        {
            ImmedidateUserInterfaceNodeChanges_None                        = 0,
            ImmedidateUserInterfaceNodeChanges_IsBeingMoved                = 1 << 0,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopLeft       = 1 << 1,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedTopRight      = 1 << 2,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomLeft    = 1 << 3,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottomRight   = 1 << 4,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedTop           = 1 << 5,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedLeft          = 1 << 6,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedRight         = 1 << 7,
            ImmedidateUserInterfaceNodeChanges_IsBeingResizedBottom        = 1 << 8,
            ImmedidateUserInterfaceNodeChanges_IsBeingFocused              = 1 << 9,
            ImmedidateUserInterfaceNodeChanges_IsBeingScrolledHorizontally = 1 << 10,
            ImmedidateUserInterfaceNodeChanges_IsBeingScrolledVertically   = 1 << 11,
            ImmedidateUserInterfaceNodeChanges_IsBeingMouseHovered         = 1 << 12,
        };

        enum ImmedidateUserInterfaceNodeSettings_ : int
        {
            ImmedidateUserInterfaceNodeSettings_None                         = 0,

            // window common hints
            ImmedidateUserInterfaceNodeSettings_Movable                      = 1 << 0,
            ImmedidateUserInterfaceNodeSettings_Resizable                    = 1 << 1,

            // scrollbars
            ImmedidateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar    = 1 << 2,
            ImmedidateUserInterfaceNodeSettings_AlwaysVerticalScrollBar      = 1 << 3,
            ImmedidateUserInterfaceNodeSettings_NeverHorizontalScrollBar     = 1 << 4,
            ImmedidateUserInterfaceNodeSettings_NeverVerticalScrollBar       = 1 << 5,

            // child windows layouting
            ImmedidateUserInterfaceNodeSettings_LayoutChildrenHorizontally   = 1 << 6,
            ImmedidateUserInterfaceNodeSettings_LayoutChildrenVertically     = 1 << 7,

            // size hints
            ImmedidateUserInterfaceNodeSettings_ResizeToContentsHorizontally        = 1 << 8,
            ImmedidateUserInterfaceNodeSettings_ResizeToContentsVertically          = 1 << 9,

            // hierarchy & clipping
            ImmedidateUserInterfaceNodeSettings_IgnoreParent                 = 1 << 10,
            ImmedidateUserInterfaceNodeSettings_IgnoreClipping               = 1 << 11,

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
            void setup(const float& _Min, const float& _Max, const float  _Total, const float& _Track)
            {
                Min          = _Min;
                Max          = _Max;
                SliderScale  = gs_abs(_Total) / gs_abs(_Max - _Min);
                SliderLength = gs_abs(_Max - _Min) / gs_abs(_Total) * _Track;
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
            mutable ImmedidateUserInterfaceNodeType     Type     {ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Node      };
            mutable ImmedidateUserInterfaceNodeSettings Settings {ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeHints_Default};
            mutable ImmedidateUserInterfaceNodeChanges  Changes  {ImmedidateUserInterfaceNodeChanges_::ImmedidateUserInterfaceNodeChanges_None  };

            // hierarchy
            mutable int                            Depth     {0};
            mutable ImmedidateUserInterfaceNode*   Parent    {nullptr}; // parent is never nullptr as ALL windows are cached...
            mutable int                            Thickness {0};
            mutable int                            FirstChild{0};
            mutable int                            LastChild {0};

            // layouting
            mutable gs_vec2f LayoutCursorDirection{gs_vec2f(0.f, 1.f)};
            mutable gs_vec2f LayoutCursorPositon  {gs_vec2f(0.f, 0.f)};
            mutable gs_vec2f LayoutCursorSize     {gs_vec2f(0.f, 0.f)};
            mutable float    LayoutFillWeight     {1.f};
            mutable float    LayoutTotalWeight    {1.f};
            mutable float    WindowMinimumWidth   {128.f};
            mutable float    WindowMinimumHeight  {128.f};
            mutable float    WindowMaximumWidth   {(float)INT_MAX};
            mutable float    WindowMaximumHeight  {(float)INT_MAX};

            // scrolling
            mutable ImmedidateUserInterfaceNodeScroll VerticalScrollBar;
            mutable ImmedidateUserInterfaceNodeScroll HorizontalScrollBar;

            // geometry
            mutable gs_vec2f  ScrollBarOffset                   {gs_vec2f(0.f, 0.f)};
            mutable gs_2dboxf WindowBox                         {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))}; 
            mutable gs_2dboxf WindowInnerClipAreaBox            {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(512.f, 512.f))};
            mutable gs_2dboxf WindowOuterClipAreaBox            {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf WindowViewportBox                 {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf WindowScrollAreaBox               {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf WindowContentBox                  {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf WindowVerticalScrollBarBox        {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf WindowVerticalScrollBarSliderBox  {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf WindowHorizontalScrollBarBox      {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_2dboxf WindowHorizontalScrollBarSliderBox{gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
            mutable gs_mat4f  WindowTransform                   {gs_mat4f(1.f)};

            std::chrono::high_resolution_clock::time_point WindowHoverStart;
        };

        struct ImmedidateUserInterfaceNode
        {
            mutable std::string                      Name {"Default"                         }; // TODO: this MUST BE A HASH !!!
            mutable ImmedidateUserInterfaceNodeState State{ImmedidateUserInterfaceNodeState()};
            mutable ImmedidateUserInterfaceNodeState Cache{ImmedidateUserInterfaceNodeState()};
        };

        class ImmedidateUserInterfaceContextLayer : public Layer
        {
        public:
            ImmedidateUserInterfaceContextLayer();
            virtual ~ImmedidateUserInterfaceContextLayer();

            virtual bool awake()        override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

            void set_next_node_maximum_size(const gs_vec2f&);
            void set_next_node_position(const gs_vec2f&);
            void set_next_node_size(const gs_vec2f&);

            void same_line();

            // hierarchy
            bool                         node_hierarchy_is_empty() const;
            ImmedidateUserInterfaceNode* node_hierarchy_top()      const;

            // cache
            bool                         node_cache_is_empty() const;
            ImmedidateUserInterfaceNode* node_cache_request(const std::string&, const ImmedidateUserInterfaceNodeType&) const;

            // hierarchical elements
            bool begin_window(
                const std::string&                  _Name,
                bool*                               _Rendered = nullptr,
                ImmedidateUserInterfaceNodeSettings _Settings = ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeHints_Default);

            void end_window();
            
            bool begin_menu(const std::string& _Name);
            void end_menu();

            // widgets API
            bool widget_push_button(const std::string& _Name);
            bool widget_menu_button(const std::string& _Name);

        protected:

            std::shared_ptr<ImmedidateUserInterfaceStyle> m_Style   {nullptr};
            std::shared_ptr<Immediate2DRenderer>          m_Renderer{nullptr};

            // info
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
            
            mutable std::vector<ImmedidateUserInterfaceNode*> m_NodesHierarchy         {std::vector<ImmedidateUserInterfaceNode*>()};
            mutable std::vector<gs_2dboxf>                    m_NodesClipBoxes         {std::vector<gs_2dboxf>()                   };
            mutable std::vector<ImmedidateUserInterfaceNode*> m_NodesDrawList          {std::vector<ImmedidateUserInterfaceNode*>()};

            mutable Frenchie::Core::Optional<gs_vec2f>        m_NextNodeSize           {Frenchie::Core::Optional<gs_vec2f>()       };
            mutable Frenchie::Core::Optional<gs_vec2f>        m_NextNodePosition       {Frenchie::Core::Optional<gs_vec2f>()       };
            mutable Frenchie::Core::Optional<gs_vec2f>        m_NextNodeMaximumSize    {Frenchie::Core::Optional<gs_vec2f>()       };
            mutable Frenchie::Core::Optional<gs_vec2f>        m_NextNodeCursorDirection{Frenchie::Core::Optional<gs_vec2f>()       };

            // widgets events
            Frenchie::Core::Optional<bool>                           m_WidgetMouseHovered;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetMouseDown;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetMousePressed;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetMouseClicked;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetMouseDoubleClicked;

            // service methods
            std::vector<ImmedidateUserInterfaceNode*> m_HoveredMenus;

            // node API
            bool begin_node(
                const std::string&                  _Name,
                bool*                               _Rendered = nullptr,
                ImmedidateUserInterfaceNodeSettings _Settings = ImmedidateUserInterfaceNodeSettings_::ImmedidateUserInterfaceNodeHints_Default,
                ImmedidateUserInterfaceNodeType     _Type     = ImmedidateUserInterfaceNodeType_::ImmedidateUserInterfaceNodeType_Node);

            void end_node();

            void node_calculate_geometry(ImmedidateUserInterfaceNode*) const;
            int  node_calculate_depth(ImmedidateUserInterfaceNode*) const;
            
            gs_vec2f node_vertical_cursor_direction() const;
            gs_vec2f node_horizontal_cursor_direction() const;
            
            bool node_is_of_type(const ImmedidateUserInterfaceNode*, const ImmedidateUserInterfaceNodeType&) const;

            bool node_is_vertical_scroll_bar_needed(const ImmedidateUserInterfaceNode*) const;
            bool node_is_horizontal_scroll_bar_needed(const ImmedidateUserInterfaceNode*) const;

            bool node_is_being_hovered(const ImmedidateUserInterfaceNode*);
            bool node_is_being_resized_top_left(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_resized_top_right(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_resized_bottom_left(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_resized_bottom_right(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_resized_top(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_resized_left(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_resized_right(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_resized_bottom(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_resized(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_moved(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_modified(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_focused(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_scrolled(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_scrolled_horizontally(const ImmedidateUserInterfaceNode*) const;
            bool node_is_being_scrolled_vertically(const ImmedidateUserInterfaceNode*) const;
            
            void node_begin_hover(ImmedidateUserInterfaceNode*);
            void node_begin_resize_top_left(ImmedidateUserInterfaceNode*);
            void node_begin_resize_top_right(ImmedidateUserInterfaceNode*);
            void node_begin_resize_bottom_left(ImmedidateUserInterfaceNode*);
            void node_begin_resize_bottom_right(ImmedidateUserInterfaceNode*);
            void node_begin_resize_top(ImmedidateUserInterfaceNode*);
            void node_begin_resize_left(ImmedidateUserInterfaceNode*);
            void node_begin_resize_right(ImmedidateUserInterfaceNode*);
            void node_begin_resize_bottom(ImmedidateUserInterfaceNode*);
            void node_begin_move(ImmedidateUserInterfaceNode*);
            void node_begin_focus(ImmedidateUserInterfaceNode*);
            void node_begin_scroll_vertically(ImmedidateUserInterfaceNode*);
            void node_begin_scroll_horizontally(ImmedidateUserInterfaceNode*);

            void node_end_hover(ImmedidateUserInterfaceNode* _Window);
            void node_end_resize(ImmedidateUserInterfaceNode*);
            void node_end_move(ImmedidateUserInterfaceNode*);
            void node_end_focus(ImmedidateUserInterfaceNode*);
            void node_end_scroll(ImmedidateUserInterfaceNode*);

            bool node_render_clipbox(ImmedidateUserInterfaceNode*);
            bool node_render_background(ImmedidateUserInterfaceNode*);
            bool node_render_background_frame(ImmedidateUserInterfaceNode*);
            bool node_render_vertical_scrollbar(ImmedidateUserInterfaceNode*);
            bool node_render_horizontal_scrollbar(ImmedidateUserInterfaceNode*);
            bool node_render_resize_events_gizmos(ImmedidateUserInterfaceNode*);

            void node_receive_events();
            void node_process_events(ImmedidateUserInterfaceNode*);

            // widget API
            auto widget_prepare_for_rendering(const gs_vec2f&);
            auto widget_prepare_for_rendering(const std::string&);

            bool widget_is_mouse_hovered() const;
            bool widget_is_mouse_pressed(const ApplicationMouseButton::Button& _Button) const;
            bool widget_is_mouse_down(const ApplicationMouseButton::Button& _Button) const;
            bool widget_is_mouse_clicked(const ApplicationMouseButton::Button& _Button) const;
            bool widget_is_mouse_double_clicked(const ApplicationMouseButton::Button& _Button) const;

            //bool widget_render_close_button_widget(const gs_2dboxf& _ButtonBox, const gs_2dboxf& _ClipBox, const gs_mat4f&  _Transform);
            
            bool widget_render_default_button_widget(
                const std::string&             _Text,
                const gs_2dboxf&               _Box,
                const gs_2dboxf&               _ClipBox,
                ImmedidateUserInterfaceNode* _Context);
        };
    }
}