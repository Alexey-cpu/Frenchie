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
        enum UINodeSettings_ : int
        {
            UINodeSettings_Default                    = 0,
            UINodeSettings_LayoutChildrenVertically   = 1 << 0,
            UINodeSettings_LayoutChildrenHorizontally = 1 << 1,
        };

        typedef int UINodeSettings;

        struct UINode;
        struct UIEvent;

        struct UIEvent
        {
            gs_vec2f CursorPosition{gs_vec2f(0.f, 0.f)};
            gs_vec2f CursorDragDelta{gs_vec2f(0.f, 0.f)};

            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseDown;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseHold;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MousePressed;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseClicked;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseDoubleClicked;
        };
        
        struct UINodeState
        {
            int Depth    {0};
            int Thickness{1};
            int DrawIndex{0};
            UINode*        Parent        {nullptr};
            gs_2dboxf      WindowBox   {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(255.f, 255.f))};
            UINodeSettings Settings      {UINodeSettings_::UINodeSettings_LayoutChildrenVertically};
        };

        struct UINode
        {
            UINodeState State;
            UINodeState Cache;

            UINode(const std::string _Name) : Name(_Name){}
            virtual ~UINode(){}

            virtual void render(Immediate2DRenderer* _Renderer)
            {
                if(_Renderer == nullptr)
                    return;

                _Renderer->push_rectangle_rounded(
                    State.WindowBox.Min,
                    State.WindowBox.Max,
                    32.f,
                    8.f,
                    gs_vec4f(0.f, 0.f, 255.f, 255.f),
                    _Renderer->calculate_transform_matrix((float)State.Depth + (State.Thickness++)));
            }

            virtual bool event(const UIEvent&)
            {
                // resize
                // TODO: process resize event here

                // move
                // TODO: process move event here

                return false;
            }

        private:
            std::string Name = "UINode";
        };

        class ImmedidateUserInterfaceContextLayer2 : public Layer
        {
        public:
            ImmedidateUserInterfaceContextLayer2();
            virtual ~ImmedidateUserInterfaceContextLayer2();

            virtual bool awake()        override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_debug()  override;
            virtual void frame_render() override;
            virtual void frame_finish() override;

            bool begin_node(const std::string& _Name, const UINodeSettings& _Settings)
            {
                // create node
                if(m_Cache.find(_Name) == m_Cache.end())
                    m_Cache[_Name] = std::make_unique<UINode>(_Name);
                UINode* node = m_Cache[_Name].get();

                node->State.Settings  = _Settings;
                node->State.DrawIndex = m_NodesRenderingList.size();

                // build nodes hierarchy
                if(!m_NodesRenderingStack.empty())
                {
                    node->State.Parent = m_NodesRenderingStack[m_NodesRenderingStack.size() - 1];
                    node->State.Depth  = node->State.Parent->State.Depth + (node->State.Parent->State.Thickness++);
                }
                else
                {
                    node->State.Depth = 0;

                    for (auto& node : m_NodesRenderingList)
                        node->State.Depth = gs_max(node->State.Depth + node->State.Thickness + 1, node->State.Depth);
                }

                m_NodesRenderingList.push_back(node);
                m_NodesRenderingStack.push_back(node);

                return true;
            }

            void end_node()
            {
                if(m_NodesRenderingStack.empty())
                    return;

                m_NodesRenderingStack.pop_back();
            }

        protected:

            void compute_geometry(UINode* _Node);
            void render(UINode* _Node);

            mutable std::map<std::string, std::unique_ptr<UINode>> m_Cache;
            mutable std::vector<UINode*>                           m_NodesRenderingList;
            mutable std::vector<UINode*>                           m_NodesRenderingCache;
            mutable std::vector<UINode*>                           m_NodesRenderingStack;

            // nested types
            struct UINodeHierarchy
            {
                UINodeHierarchy(const std::function<UINode*(UINode*)> _GetParent =
                    [](UINode* _Node)->UINode*
                    {
                        return _Node != nullptr ? _Node->State.Parent : nullptr;
                    }) : GetParent(_GetParent){}

                ~UINodeHierarchy(){}

                std::vector<int>                Indexes;
                std::vector<int>                Entries;
                std::vector<UINode*>            Singletons;
                std::vector<UINode*>            Sorted;
                std::function<UINode*(UINode*)> GetParent;

                std::vector<UINode*>::iterator begin(const UINode* _Node)
                {
                    return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.DrawIndex];
                }

                std::vector<UINode*>::iterator end(const UINode* _Node)
                {
                    return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.DrawIndex + 1];
                }

                void build(const std::vector<UINode*>& _Nodes)
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

                        if(_Nodes[i]->State.Parent == nullptr)
                            Singletons.push_back(_Nodes[i]);
                    }

                    // count items
                    for (int i = 0; i < (int)_Nodes.size(); i++)
                    {
                        if(get_parent(_Nodes[i]) == nullptr)
                            continue;

                        ++Entries[get_parent(_Nodes[i])->State.DrawIndex];
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

                    for(int i = 0; i < _Nodes.size(); i++ )
                    {
                        if(get_parent(_Nodes[i]) == nullptr)
                            continue;

                        Sorted[workspace[get_parent(_Nodes[i])->State.DrawIndex]++] = _Nodes[i];
                    }
                }

            private:

                UINode* get_parent(UINode* _Node)
                {
                    return GetParent != nullptr ? GetParent(_Node) : nullptr;
                }
            } mutable m_Hierarchy;

            mutable std::shared_ptr<Immediate2DRenderer> m_Renderer{nullptr};
        };
    };
}