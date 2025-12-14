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

            // // push button background
            // ImmedidateUserInterfaceColors_PushButtonEnabledBackgroundColor,
            // ImmedidateUserInterfaceColors_PushButtonEnabledHoveredBackgroundColor,
            // ImmedidateUserInterfaceColors_PushButtonEnabledPressedBackgroundColor,
            // ImmedidateUserInterfaceColors_PushButtonDisabledBackgroundColor,
            // ImmedidateUserInterfaceColors_PushButtonDisabledHoveredBackgroundColor,
            // ImmedidateUserInterfaceColors_PushButtonDisabledPressedBackgroundColor,

            // // push button frame
            // ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor,
            // ImmedidateUserInterfaceColors_PushButtonEnabledHoveredFrameColor,
            // ImmedidateUserInterfaceColors_PushButtonEnabledPressedFrameColor,
            // ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor,
            // ImmedidateUserInterfaceColors_PushButtonDisabledHoveredFrameColor,
            // ImmedidateUserInterfaceColors_PushButtonDisabledPressedFrameColor,

            // // radio buton
            // ImmedidateUserInterfaceColors_RadioButtonPressedEnabledColor,
            // ImmedidateUserInterfaceColors_RadioButtonPressedDisabledColor,

            ImmedidateUserInterfaceColors_ColorEnd,
        };

        enum ImmedidateUserInterfaceWindowHints_ : int
        {
            // window common hints
            ImmedidateUserInterfaceWindowHints_Movable                   = 1 << 1,
            ImmedidateUserInterfaceWindowHints_Closable                  = 1 << 2,
            ImmedidateUserInterfaceWindowHints_Resizable                 = 1 << 3,

            // scrollbars
            ImmedidateUserInterfaceWindowHints_AlwaysHorizontalScrollBar = 1 << 4,
            ImmedidateUserInterfaceWindowHints_AlwaysVerticalScrollBar   = 1 << 5,
            ImmedidateUserInterfaceWindowHints_NeverHorizontalScrollBar  = 1 << 6,
            ImmedidateUserInterfaceWindowHints_NeverVerticalScrollBar    = 1 << 7,

            // child windows layouting
            // ImmedidateUserInterfaceWindowHints_LayoutChildWindowsHorizontally,
            // ImmedidateUserInterfaceWindowHints_LayoutChildWindowsVertically,

            ImmedidateUserInterfaceWindowHints_Default       =
                ImmedidateUserInterfaceWindowHints_Movable   |
                ImmedidateUserInterfaceWindowHints_Closable  |
                ImmedidateUserInterfaceWindowHints_Resizable
        };

        typedef int ImmedidateUserInterfaceWindowHints;
        typedef int ImmedidateUserInterfaceConditions;

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

                // // Push button
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledBackgroundColor        ] = gs_vec4f(4, 41, 189, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredBackgroundColor ] = gs_vec4f(60, 98, 250, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedBackgroundColor ] = gs_vec4f(87, 120, 250, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledBackgroundColor       ] = gs_vec4f(53, 53, 54, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredBackgroundColor] = gs_vec4f(87, 87, 89, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedBackgroundColor] = gs_vec4f(117, 117, 120, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor             ] = gs_vec4f(1, 16, 77, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredFrameColor      ] = gs_vec4f(0, 16, 77, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedFrameColor      ] = gs_vec4f(1, 32, 77, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor            ] = gs_vec4f(30, 30, 31, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredFrameColor     ] = gs_vec4f(30, 1, 31, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedFrameColor     ] = gs_vec4f(30, 12, 31, 255);

                // // radio button
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_RadioButtonPressedEnabledColor          ] = gs_vec4f(255, 255, 255, 255);
                // Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_RadioButtonPressedDisabledColor         ] = gs_vec4f(200, 200, 200, 255);

                // radio button

            }

            RenderingQueueFont Font;
            float              FontSize                          = 32.f;

            float              WindowResizeAngleGizmoRadius      = 32.f;
            float              WindowResizeSideGizmoWidth        = 12.f;
            float              WindowFrameCloseMinimumButtonSize = 32.f;
            float              WindowMinimumWidth                = 128.f;
            float              WindowMinimumHeight               = 128.f;
            float              WindowScrollBarSliderWidth        = 32.f;

            float              FrameWidth                        = 8.f;
            float              FrameRoundingRadius               = 16.f;

            gs_vec4f Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_ColorEnd]{};
        };

        struct ImmedidateUserInterfaceWindow
        {
            struct Scrollbar
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

            struct WindowState
            {
                // hints
                mutable ImmedidateUserInterfaceWindowHints Hints {ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Default};

                // hierarchy
                mutable int                            Depth    {0};
                mutable ImmedidateUserInterfaceWindow* Parent   {nullptr}; // parent is never nullptr as ALL windows are cached...
                mutable int                            Thickness{0};

                // layouting
                mutable gs_vec2f LayoutCursorDirection{gs_vec2f(1.f, 1.f)};
                mutable gs_vec2f LayoutCursorPositon  {gs_vec2f(0.f, 0.f)};
                mutable gs_vec2f LayoutCursorSize     {gs_vec2f(0.f, 0.f)};
                mutable float    LayoutFillWeight     {1.f};
                mutable float    LayoutTotalWeight    {1.f};

                // scrolling
                mutable Scrollbar VerticalScrollBar;
                mutable Scrollbar HorizontalScrollBar;

                // geometry
                mutable gs_2dboxf WindowBox                         {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowInnerClipAreaBox            {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowOuterClipAreaBox            {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowViewportBox                 {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowScrollAreaBox               {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowContentBox                  {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowFrameBox                    {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowTitleBox                    {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowCloseButtonBox              {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                
                mutable gs_2dboxf WindowVerticalScrollBarBox        {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowVerticalScrollBarSliderBox  {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};

                mutable gs_2dboxf WindowHorizontalScrollBarBox      {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};
                mutable gs_2dboxf WindowHorizontalScrollBarSliderBox{gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 128.f))};

                mutable gs_mat4f  WindowTransform                   {gs_mat4f(1.f)};

                mutable bool IsBeingMoved               {false};
                mutable bool IsBeingResizedTopLeft      {false};
                mutable bool IsBeingResizedTopRight     {false};
                mutable bool IsBeingResizedBottomLeft   {false};
                mutable bool IsBeingResizedBottomRight  {false};
                mutable bool IsBeingResizedTop          {false};
                mutable bool IsBeingResizedLeft         {false};
                mutable bool IsBeingResizedRight        {false};
                mutable bool IsBeingResizedBottom       {false};
                mutable bool IsFocused                  {false};
                mutable bool IsBeingScrolledHorizontally{false};
                mutable bool IsBeingScrolledVertically  {false};
            };

            mutable std::string Name {"Default"    }; // TODO: this MUST BE A HASH !!!
            mutable WindowState State{WindowState()};
            mutable WindowState Cache{WindowState()};

            // API
            int  calculate_child_depth() const;
            bool needs_vertical_scroll_bar() const;
            bool needs_horizontal_scroll_bar() const;
            bool is_being_resized_top_left() const;
            bool is_being_resized_top_right() const;
            bool is_being_resized_bottom_left() const;
            bool is_being_resized_bottom_right() const;
            bool is_being_resized_top() const;
            bool is_being_resized_left() const;
            bool is_being_resized_right() const;
            bool is_being_resized_bottom() const;
            bool is_being_resized() const;
            bool is_being_moved() const;
            bool is_being_modified() const;
            bool is_being_focused() const;
            bool is_being_scrolled_vertically() const;
            bool is_being_scrolled_horizontally() const;
            bool is_being_scrolled() const;
            void begin_resize_top_left();
            void begin_resize_top_right();
            void begin_resize_bottom_left();
            void begin_resize_bottom_right();
            void begin_resize_top();
            void begin_resize_left();
            void begin_resize_right();
            void begin_resize_bottom();
            void being_move();
            void begin_focus();
            void begin_scroll_vertically();
            void begin_scroll_horizontally();
            void end_resize();
            void end_move();
            void end_focus();
            void end_scroll();
        };

        class Immedidate2DRendererTestLayer : public Layer
        {
        public:
            Immedidate2DRendererTestLayer();
            virtual ~Immedidate2DRendererTestLayer();

            virtual bool awake()        override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

            void set_next_window_position(const gs_vec2f&);
            void set_next_window_size(const gs_vec2f&);

            void same_line();

            // windows API
            bool begin_window(
                const std::string&                 _Name,
                bool*                              _Opened = nullptr,
                ImmedidateUserInterfaceWindowHints _Hints  = ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Default);

            void end_window();

            // widgets API
            bool close_button_widget(const gs_vec2f& _Size = gs_vec2f(64.f, 64.f));


        protected:

            std::shared_ptr<ImmedidateUserInterfaceStyle> m_Style   {nullptr};
            std::shared_ptr<Immediate2DRenderer>          m_Renderer{nullptr};

            // info
            std::map<std::string, std::unique_ptr<ImmedidateUserInterfaceWindow>> m_WindowsCache    {std::map<std::string, std::unique_ptr<ImmedidateUserInterfaceWindow>>()};
            std::vector<ImmedidateUserInterfaceWindow*>                           m_WindowsDrawList {std::vector<ImmedidateUserInterfaceWindow*>()};
            std::vector<ImmedidateUserInterfaceWindow*>                           m_WindowsHierarchy{std::vector<ImmedidateUserInterfaceWindow*>()};
            
            Frenchie::Core::Optional<ImmedidateUserInterfaceConditions> m_Conditions;
            Frenchie::Core::Optional<gs_vec2f>                          m_NextWindowSize;
            Frenchie::Core::Optional<gs_vec2f>                          m_NextWindowPosition;
            Frenchie::Core::Optional<gs_vec2f>                          m_NextCursorDirection;

            // service methods

            // geometry calculation
            void      calculate_window_geometry(ImmedidateUserInterfaceWindow*);
            gs_2dboxf calculate_widget_geometry(const gs_vec2f& _Size);

            // rendering
            bool render_window_clipbox(ImmedidateUserInterfaceWindow*);
            bool render_window_background(ImmedidateUserInterfaceWindow*);
            bool render_window_classic_frame(ImmedidateUserInterfaceWindow*);
            bool render_window_vertical_scrollbar(ImmedidateUserInterfaceWindow*);
            bool render_window_horizontal_scrollbar(ImmedidateUserInterfaceWindow*);

            bool render_close_button_widget(const gs_2dboxf& _ButtonBox, const gs_2dboxf& _CursorClipBox, const gs_mat4f&  _Transform);

            void sink_window_events(ImmedidateUserInterfaceWindow*);
        };
    }
}

// bool draw_radio_button_widget(
//     const std::string& _Name,
//     const float&       _Radius,
//     bool&              _Pushed,
//     const bool&        _Enabled,
//     const float&       _Depth,
//     const gs_vec2f&    _Position = gs_vec2f(0.f, 0.f),
//     const float&       _Rotation = 0.f,
//     const gs_vec2f&    _Scale    = gs_vec2f(1.f, 1.f));

// bool draw_push_button_widget(
//     const std::string& _Name,
//     const gs_vec2f&    _Size,
//     const bool&        _Enabled,
//     const float&       _Depth,
//     const gs_vec2f&    _Position = gs_vec2f(0.f, 0.f),
//     const float&       _Rotation = 0.f,
//     const gs_vec2f&    _Scale    = gs_vec2f(1.f, 1.f));

// bool bounding_box_contains(const gs_vec2f& _Position, const gs_2dboxf& _Box, const gs_vec2f& _Point)
// {
//     return m_Renderer->calculate_bounding_box(
//             _Position,
//             0.f,
//             gs_vec2f(1.f, 1.f),
//             _Box.Min,
//             _Box.Max).contains(_Point);
// }

// gs_2dboxf calculate_text_bounding_box(const std::string& _Text)
// {
//     return m_Renderer->calculate_bounding_box(
//         gs_vec2f(0.f, 0.f),
//         0.f,
//         gs_vec2f(1.f, 1.f),
//         _Text,
//         m_Style.FontSize,
//         m_Renderer->m_RenderingQueue->get_default_font());
// }

// auxiliary lambdas
// gs_vec4f retreive_push_button_background_color(const bool& _Enabled, const bool _Hovered, const bool& _Pressed)
// {
//     if(_Pressed)
//     {
//         return _Enabled ?
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedBackgroundColor] :
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedBackgroundColor];
//     }

//     if(_Hovered)
//     {
//         return _Enabled ?
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredBackgroundColor] :
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredBackgroundColor];
//     }

//     return _Enabled ?
//         m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledBackgroundColor] :
//         m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledBackgroundColor];
// };

// gs_vec4f retreive_push_frame_color(const bool& _Enabled, const bool _Hovered, const bool& _Pressed)
// {
//     if(_Pressed)
//     {
//         return _Enabled ?
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledPressedFrameColor] :
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledPressedFrameColor];
//     }

//     if(_Hovered)
//     {
//         return _Enabled ?
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredFrameColor] :
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredFrameColor];
//     }

//     return _Enabled ?
//         m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor] :
//         m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor];
// };

// gs_vec4f retreive_text_color(const bool& _Enabled, const bool _Hovered, const bool& _Pressed)
// {
//     if(_Hovered || _Pressed)
//     {
//         return _Enabled ?
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextHoveredColor] :
//             m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextDisabledColor];
//     }

//     return _Enabled ?
//         m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextEnabledColor] :
//         m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextDisabledColor];
// };

// gs_vec4f retrieve_radio_button_pressed_state_color(const bool& _Enabled, const bool& _Pressed)
// {
//     return _Enabled ?
//         m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_RadioButtonPressedEnabledColor] :
//         m_Style.Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_RadioButtonPressedDisabledColor];
// }

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

// class ImmedidateUserInterfaceEntry
// {
//     ImmedidateUserInterfaceEntry(const gs_rectf& _BoundingBox) :
//         BoundingBox(_BoundingBox){}
    
//     gs_rectf BoundingBox  {gs_rectf(0.f, 0.f, 128.f, 128.f)};
//     gs_mat4f Transform    {gs_mat4f(1.f)};
//     bool     Hovered      {false};
//     bool     Clicked      {false};
//     bool     DoubleClicked{false};

//     void (*DrawFunction)(
//         const char*     _Name,
//         const gs_vec2f& _Position,
//         const gs_vec2f& _Size,
//         const gs_mat4f& _Transform) = nullptr;
// };