#pragma once

#include <FrenchieApplicationImmediate2DRenderer.hpp>

// STL
#include <chrono>

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
        enum ImmedidateUserInterfaceColors : int
        {
            ImmedidateUserInterface_PushButtonColorBegin = 0,

            // push button
            ImmedidateUserInterfaceColors_PushButtonEnabledColor = ImmedidateUserInterface_PushButtonColorBegin,
            ImmedidateUserInterfaceColors_PushButtonEnabledHoveredColor,
            ImmedidateUserInterfaceColors_PushButtonEnabledPressedColor,
            ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledHoveredColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledPressedColor,
            ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor,

            ImmedidateUserInterfaceColors_ColorEnd,
        };

        struct ImmedidateUserInterfaceStyle
        {
            ImmedidateUserInterfaceStyle()
            {
                // setup color scheme

                // push button
                Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledColor        ] = gs_vec4f(4, 41, 189, 255);
                Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledHoveredColor ] = gs_vec4f(60, 98, 250, 255);
                Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledPressedColor ] = gs_vec4f(87, 120, 250, 255);
                Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonEnabledFrameColor   ] = gs_vec4f(1, 16, 77, 255);
                Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledColor       ] = gs_vec4f(53, 53, 54, 255);
                Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledHoveredColor] = gs_vec4f(87, 87, 89, 255);
                Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledPressedColor] = gs_vec4f(117, 117, 120, 255);
                Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_PushButtonDisabledFrameColor  ] = gs_vec4f(30, 30, 31, 255);
            }

            gs_vec4f Colors[ImmedidateUserInterfaceColors::ImmedidateUserInterfaceColors_ColorEnd]{};
            float    PushButtonFrameWidth     = 8.f;
            float    PushButtonRoundingRadius = 16.f;
        };

        class Immedidate2DRendererTestLayer : public Layer
        {
        public:
            Immedidate2DRendererTestLayer(){}
            virtual ~Immedidate2DRendererTestLayer(){}

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;

            bool push_button_widget(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const bool&     _Enabled,
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

        protected:

            ImmedidateUserInterfaceStyle m_Style;

            std::shared_ptr<Immediate2DRenderer> m_Renderer{nullptr};

        };
    }
}