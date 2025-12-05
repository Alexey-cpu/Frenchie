#pragma once

#include <FrenchieApplicationImmediate2DRenderer.hpp>

// STL
#include <chrono>
#include <stack>
#include <map>

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

namespace Frenchie
{
    namespace Application
    {
        struct ImmedidateUserInterfaceWindow
        {
            int ChildrenCount = 0;
            int ChildIndex    = 0;
            int StackIndex    = 0;

            gs_mat4f  Transform{gs_mat4f(1.f)};
            gs_2dboxf PreviousBox;
            gs_2dboxf CurrentBox;
            float     Depth;

            void set_dirty(const bool& _Value) 
            {
                IsDirty = _Value;
            }

            bool is_dirty() const
            {
                return IsDirty;
            }

            bool is_being_moved() const
            {
                return IsBeingMoved && !is_dirty();
            }

            bool is_being_resized_top_left() const
            {
                return IsBeingResizedTopLeft && !is_dirty();
            }

            bool is_being_resized_top_right() const
            {
                return IsBeingResizedTopRight && !is_dirty();
            }

            bool is_being_resized_bottom_left() const
            {
                return IsBeingResizedBottomLeft && !is_dirty();
            }

            bool is_being_resized_bottom_right() const
            {
                return IsBeingResizedBottomRight && !is_dirty();
            }

            bool is_being_resized_top() const
            {
                return IsBeingResizedTop && !is_dirty();
            }

            bool is_being_resized_left() const
            {
                return IsBeingResizedLeft && !is_dirty();
            }

            bool is_being_resized_right() const
            {
                return IsBeingResizedRight && !is_dirty();
            }

            bool is_being_resized_bottom() const
            {
                return IsBeingResizedBottom && !is_dirty();
            }

            void being_move()
            {
                IsBeingMoved = true;
            }

            void begin_resize_top_left()
            {
                IsBeingResizedTopLeft = true;
            }

            void begin_resize_top_right()
            {
                IsBeingResizedTopRight = true;
            }

            void begin_resize_bottom_left()
            {
                IsBeingResizedBottomLeft = true;
            }
            
            void begin_resize_bottom_right()
            {
                IsBeingResizedBottomRight = true;
            }

            void begin_resize_top()
            {
                IsBeingResizedTop = true;
            }

            void begin_resize_left()
            {
                IsBeingResizedLeft = true;
            }

            void begin_resize_right()
            {
                IsBeingResizedRight = true;
            }

            void begin_resize_bottom()
            {
                IsBeingResizedBottom = true;
            }
            
            bool is_being_resized() const
            {
                return (IsBeingResizedTopLeft    ||
                       IsBeingResizedTopRight    ||
                       IsBeingResizedBottomLeft  ||
                       IsBeingResizedBottomRight ||
                       IsBeingResizedTop         ||
                       IsBeingResizedLeft        ||
                       IsBeingResizedRight       ||
                       IsBeingResizedBottom) && !is_dirty();
            }

            void end_move()
            {
                IsBeingMoved = false;
            }

            void end_resize()
            {
                IsBeingResizedTopLeft     = false;
                IsBeingResizedTopRight    = false;
                IsBeingResizedBottomLeft  = false;
                IsBeingResizedBottomRight = false;
                IsBeingResizedTop         = false;
                IsBeingResizedLeft        = false;
                IsBeingResizedRight       = false;
                IsBeingResizedBottom      = false;
            }

        protected:

            bool IsBeingMoved             {false};
            bool IsBeingResizedTopLeft    {false};
            bool IsBeingResizedTopRight   {false};
            bool IsBeingResizedBottomLeft {false};
            bool IsBeingResizedBottomRight{false};
            bool IsBeingResizedTop        {false};
            bool IsBeingResizedLeft       {false};
            bool IsBeingResizedRight      {false};
            bool IsBeingResizedBottom     {false};
            bool IsDirty                  {true };
        };

        enum ImmedidateUserInterfaceColors_ : int
        {
            // application window
            ImmedidateUserInterfaceColors_WindowResizeGizmoColor,
            ImmedidateUserInterfaceColors_WindowContentSpaceColor,
            ImmedidateUserInterfaceColors_WindowDecorationFrameColor,
            ImmedidateUserInterfaceColors_WindowContentSpaceFrameColor,
            ImmedidateUserInterfaceColors_WindowDecorationFrameFrameColor,

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

            // radio buton
            ImmedidateUserInterfaceColors_RadioButtonPressedEnabledColor,
            ImmedidateUserInterfaceColors_RadioButtonPressedDisabledColor,

            ImmedidateUserInterfaceColors_ColorEnd,
        };

        enum ImmedidateUserInterfaceWindowHints_
        {
            ImmedidateUserInterfaceWindowHints_Movable,
            ImmedidateUserInterfaceWindowHints_Resizable,
            ImmedidateUserInterfaceWindowHints_Default = ImmedidateUserInterfaceWindowHints_Movable | ImmedidateUserInterfaceWindowHints_Resizable
        };

        typedef int ImmedidateUserInterfaceWindowHints;

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

                // application text
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextEnabledColor                        ] = gs_vec4f(255.f, 255.f, 255.f, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextDisabledColor                       ] = gs_vec4f(200.f, 200.f, 200.f, 255.f);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_TextHoveredColor                        ] = gs_vec4f(255.f, 32.f, 32.f, 255.f);

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

                // radio button
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_RadioButtonPressedEnabledColor          ] = gs_vec4f(255, 255, 255, 255);
                Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_RadioButtonPressedDisabledColor         ] = gs_vec4f(200, 200, 200, 255);

                // radio button

            }

            RenderingQueueFont Font;
            float              FontSize                     = 32.f;
            float              WindowResizeAngleGizmoRadius = 32.f;
            float              WindowResizeSideGizmoWidth   = 12.f;
            float              WindowFrameCloseButtonSize   = 32.f;
            float              WindowMinimumWidth           = 128.f;
            float              WindowMinimumHeight          = 128.f;
            float              FrameWidth                   = 8.f;
            float              FrameRoundingRadius          = 32.f;

            gs_vec4f Colors[ImmedidateUserInterfaceColors_::ImmedidateUserInterfaceColors_ColorEnd]{};
        };

        class Immedidate2DRendererTestLayer : public Layer
        {
        public:
            Immedidate2DRendererTestLayer(){}
            virtual ~Immedidate2DRendererTestLayer(){}

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

            bool push_window(
                const std::string&                        _Name,
                const ImmedidateUserInterfaceWindowHints& _Hints = ImmedidateUserInterfaceWindowHints_::ImmedidateUserInterfaceWindowHints_Default);
            void pop_window();

        protected:

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

            std::map<std::string, ImmedidateUserInterfaceWindow> m_WindowsCache{std::map<std::string, ImmedidateUserInterfaceWindow>()};
            std::vector<ImmedidateUserInterfaceWindow>           m_WindowsStack{std::vector<ImmedidateUserInterfaceWindow>()};

            ImmedidateUserInterfaceStyle         m_Style   {ImmedidateUserInterfaceStyle()};
            std::shared_ptr<Immediate2DRenderer> m_Renderer{nullptr};

        };
    }
}