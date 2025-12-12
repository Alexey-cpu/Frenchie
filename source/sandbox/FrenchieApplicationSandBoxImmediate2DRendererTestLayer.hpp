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

            ImmedidateUserInterfaceWindowHints_Default       =
                ImmedidateUserInterfaceWindowHints_Movable   |
                ImmedidateUserInterfaceWindowHints_Closable  |
                ImmedidateUserInterfaceWindowHints_Resizable
        };

        typedef int ImmedidateUserInterfaceWindowHints;
        typedef int ImmedidateUserInterfaceConditions;

        struct ImmedidateUserInterfaceStyle;
        struct ImmedidateUserInterfaceWindow;

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

            // API
            bool push_close_button_widget();

            bool begin_window(
                const std::string&                 _Name,
                bool*                              _Opened = nullptr,
                ImmedidateUserInterfaceWindowHints _Hints  = ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Default);

            void end_window();

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