#pragma once

#include <FrenchieApplicationImmediate2DRenderer.hpp>

// STL
#include <type_traits>
#include <memory>
#include <chrono>
#include <stack>
#include <map>


namespace Frenchie
{
    namespace Application
    {
        struct ImmedidateUserInterfaceStyle;
        struct ImmedidateUserInterfaceWindow;
        struct ImmedidateUserInterfaceWindowState;

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

        enum ImmedidateUserInterfaceWindowHints_ : int
        {
            // window common hints
            ImmedidateUserInterfaceWindowHints_Movable                   = 1 << 0,
            ImmedidateUserInterfaceWindowHints_Closable                  = 1 << 1,
            ImmedidateUserInterfaceWindowHints_Resizable                 = 1 << 2,

            // scrollbars
            ImmedidateUserInterfaceWindowHints_AlwaysHorizontalScrollBar = 1 << 3,
            ImmedidateUserInterfaceWindowHints_AlwaysVerticalScrollBar   = 1 << 4,
            ImmedidateUserInterfaceWindowHints_NeverHorizontalScrollBar  = 1 << 5,
            ImmedidateUserInterfaceWindowHints_NeverVerticalScrollBar    = 1 << 6,

            // child windows layouting
            ImmedidateUserInterfaceWindowHints_LayoutChildrenHorizontally = 1 << 7,
            ImmedidateUserInterfaceWindowHints_LayoutChildrenVertically   = 1 << 8,

            // size hints
            ImmedidateUserInterfaceWindowHints_ResizeToContents = 1 << 9,
            ImmedidateUserInterfaceWindowHints_NoParent         = 1 << 10,
            ImmedidateUserInterfaceWindowHints_NoClipping       = 1 << 11,

            // type
            ImmedidateUserInterfaceWindowHints_TypeMenu = 1 << 12,

            ImmedidateUserInterfaceWindowHints_Default       =
                ImmedidateUserInterfaceWindowHints_Movable   |
                ImmedidateUserInterfaceWindowHints_Closable  |
                ImmedidateUserInterfaceWindowHints_Resizable |
                ImmedidateUserInterfaceWindowHints_LayoutChildrenVertically
        };

        enum ImmedidateUserInterfaceWindowStateChangeHints_ : int
        {
            ImmedidateUserInterfaceWindowStateChangeHints_None                        = 0,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMoved                = 1 << 0,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTopLeft       = 1 << 1,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTopRight      = 1 << 2,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottomLeft    = 1 << 3,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottomRight   = 1 << 4,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedTop           = 1 << 5,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedLeft          = 1 << 6,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedRight         = 1 << 7,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingResizedBottom        = 1 << 8,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingFocused              = 1 << 9,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingScrolledHorizontally = 1 << 10,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingScrolledVertically   = 1 << 11,

            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMouseHovered       = 1 << 12,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMousePressed       = 1 << 13,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMouseDown          = 1 << 14,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMouseClicked       = 1 << 15,
            ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMouseDoubleClicked = 1 << 16,
        };

        typedef int ImmedidateUserInterfaceWindowHints;
        typedef int ImmedidateUserInterfaceWindowStateChangeHints;
        typedef int ImmedidateUserInterfaceWindowAttributes;

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

        struct ImmedidateUserInterfaceWindowScrollbar
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

        struct ImmedidateUserInterfaceWindowState
        {
            // hints
            mutable ImmedidateUserInterfaceWindowHints            Settings   {ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Default};
            mutable ImmedidateUserInterfaceWindowStateChangeHints Changes    {ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_None};

            // hierarchy
            mutable int                            Depth     {0};
            mutable ImmedidateUserInterfaceWindow* Parent    {nullptr}; // parent is never nullptr as ALL windows are cached...
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
            mutable ImmedidateUserInterfaceWindowScrollbar VerticalScrollBar;
            mutable ImmedidateUserInterfaceWindowScrollbar HorizontalScrollBar;

            // geometry
            mutable gs_2dboxf WindowBox                         {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))}; 
            mutable gs_2dboxf WindowInnerClipAreaBox            {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
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

        struct ImmedidateUserInterfaceWindow
        {
            mutable std::string                        Name {"Default"                           }; // TODO: this MUST BE A HASH !!!
            mutable ImmedidateUserInterfaceWindowState State{ImmedidateUserInterfaceWindowState()};
            mutable ImmedidateUserInterfaceWindowState Cache{ImmedidateUserInterfaceWindowState()};
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

            void set_next_window_position(const gs_vec2f&);
            void set_next_window_size(const gs_vec2f&);

            void same_line();

            // windows API
            ImmedidateUserInterfaceWindow* current_window() const;

            bool begin_window(
                const std::string&                 _Name,
                bool*                              _Opened = nullptr,
                ImmedidateUserInterfaceWindowHints _Hints  = ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Default);

            void end_window();

            bool begin_menu(const std::string& _Name);
            bool menu_item(const std::string& _Name);
            void end_menu();

            // widgets API
            bool close_button_widget(const gs_vec2f& _Size = gs_vec2f(64.f, 64.f));
            bool default_button_widget(const std::string& _Name);

        protected:

            std::shared_ptr<ImmedidateUserInterfaceStyle> m_Style   {nullptr};
            std::shared_ptr<Immediate2DRenderer>          m_Renderer{nullptr};

            // info
            std::map<std::string, std::unique_ptr<ImmedidateUserInterfaceWindow>> m_WindowsCache    {std::map<std::string, std::unique_ptr<ImmedidateUserInterfaceWindow>>()};
            std::vector<ImmedidateUserInterfaceWindow*>                           m_WindowsDrawList {std::vector<ImmedidateUserInterfaceWindow*>()};
            std::vector<ImmedidateUserInterfaceWindow*>                           m_WindowsHierarchy{std::vector<ImmedidateUserInterfaceWindow*>()};

            Frenchie::Core::Optional<gs_vec2f> m_NextWindowSize;
            Frenchie::Core::Optional<gs_vec2f> m_NextWindowPosition;
            Frenchie::Core::Optional<gs_vec2f> m_NextCursorDirection;

            // widgets events
            Frenchie::Core::Optional<bool>                           m_WidgetMouseHovered;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetMouseDown;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetMousePressed;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetMouseClicked;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> m_WidgetMouseDoubleClicked;

            // service methods

            // window API
            void window_calculate_geometry(ImmedidateUserInterfaceWindow*) const;
            int  window_calculate_child_depth(ImmedidateUserInterfaceWindow*) const;
            
            gs_vec2f window_vertical_cursor_direction() const;
            gs_vec2f window_horizontal_cursor_direction() const;
            
            bool window_is_vertical_scroll_bar_needed(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_horizontal_scroll_bar_needed(const ImmedidateUserInterfaceWindow*) const;

            bool window_is_being_mouse_hover(ImmedidateUserInterfaceWindow* _Window)
            {
                return _Window != nullptr &&
                        (_Window->State.Changes & ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMouseHovered);
            }

            bool window_is_being_resized_top_left(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_resized_top_right(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_resized_bottom_left(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_resized_bottom_right(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_resized_top(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_resized_left(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_resized_right(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_resized_bottom(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_resized(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_moved(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_modified(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_focused(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_scrolled(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_scrolled_horizontally(const ImmedidateUserInterfaceWindow*) const;
            bool window_is_being_scrolled_vertically(const ImmedidateUserInterfaceWindow*) const;
            
            void window_begin_mouse_hover(ImmedidateUserInterfaceWindow* _Window)
            {
                if(_Window == nullptr) return;

                _Window->State.Changes |=
                    ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMouseHovered;

                _Window->State.WindowHoverStart = std::chrono::high_resolution_clock::now();
            }
            // void window_begin_mouse_press(ImmedidateUserInterfaceWindow*);
            // void window_begin_mouse_down(ImmedidateUserInterfaceWindow*);
            // void window_begin_mouse_click(ImmedidateUserInterfaceWindow*);
            // void window_begin_mouse_double_click(ImmedidateUserInterfaceWindow*);

            void window_begin_resize_top_left(ImmedidateUserInterfaceWindow*);
            void window_begin_resize_top_right(ImmedidateUserInterfaceWindow*);
            void window_begin_resize_bottom_left(ImmedidateUserInterfaceWindow*);
            void window_begin_resize_bottom_right(ImmedidateUserInterfaceWindow*);
            void window_begin_resize_top(ImmedidateUserInterfaceWindow*);
            void window_begin_resize_left(ImmedidateUserInterfaceWindow*);
            void window_begin_resize_right(ImmedidateUserInterfaceWindow*);
            void window_begin_resize_bottom(ImmedidateUserInterfaceWindow*);
            void window_begin_move(ImmedidateUserInterfaceWindow*);
            void window_begin_focus(ImmedidateUserInterfaceWindow*);
            void window_begin_scroll_vertically(ImmedidateUserInterfaceWindow*);
            void window_begin_scroll_horizontally(ImmedidateUserInterfaceWindow*);

            void window_end_mouse_hover(ImmedidateUserInterfaceWindow* _Window)
            {
                if(_Window == nullptr) return;

                _Window->State.Changes &= ~(ImmedidateUserInterfaceWindowStateChangeHints_::ImmedidateUserInterfaceWindowStateChangeHints_IsBeingMouseHovered);
            }
            
            void window_end_resize(ImmedidateUserInterfaceWindow*);
            void window_end_move(ImmedidateUserInterfaceWindow*);
            void window_end_focus(ImmedidateUserInterfaceWindow*);
            void window_end_scroll(ImmedidateUserInterfaceWindow*);

            bool window_render_clipbox(ImmedidateUserInterfaceWindow*);
            bool window_render_background(ImmedidateUserInterfaceWindow*);
            bool window_render_background_frame(ImmedidateUserInterfaceWindow*);
            bool window_render_vertical_scrollbar(ImmedidateUserInterfaceWindow*);
            bool window_render_horizontal_scrollbar(ImmedidateUserInterfaceWindow*);
            bool window_render_resize_events_gizmos(ImmedidateUserInterfaceWindow*);

            void window_receive_events();
            void window_process_events(ImmedidateUserInterfaceWindow*);

            // widget API
            gs_2dboxf widget_calculate_geometry(const gs_vec2f&);
            gs_2dboxf widget_calculate_geometry(const std::string&);

            void widget_begin_receive_events();
            void widget_end_receive_events(const gs_2dboxf& _Box, const gs_2dboxf& _ClipBox, const gs_mat4f& _Transform);

            bool widget_is_mouse_hovered() const;
            bool widget_is_mouse_pressed(const ApplicationMouseButton::Button& _Button) const;
            bool widget_is_mouse_down(const ApplicationMouseButton::Button& _Button) const;
            bool widget_is_mouse_clicked(const ApplicationMouseButton::Button& _Button) const;
            bool widget_is_mouse_double_clicked(const ApplicationMouseButton::Button& _Button) const;

            bool widget_render_close_button_widget(const gs_2dboxf& _ButtonBox, const gs_2dboxf& _ClipBox, const gs_mat4f&  _Transform);
            
            bool widget_render_default_button_widget(
                const std::string&             _Text,
                const gs_2dboxf&               _Box,
                const gs_2dboxf&               _ClipBox,
                ImmedidateUserInterfaceWindow* _Context);
        };
    }
}