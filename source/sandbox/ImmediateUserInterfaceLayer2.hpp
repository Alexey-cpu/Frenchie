#pragma once

#include <FrenchieApplicationImmediate2DRenderer.hpp>

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
        enum ImmediateUserInterfaceNodeEvents_ : int
        {
            // sentinel
            ImmediateUserInterfaceNodeEvents_None                 = 0,
            
            // move
            ImmediateUserInterfaceNodeEvents_IsMoved              = 1 << 0,
            ImmediateUserInterfaceNodeEvents_IsMoveStarted        = 1 << 1,
            
            // resize
            ImmediateUserInterfaceNodeEvents_IsResizedTop         = 1 << 2,
            ImmediateUserInterfaceNodeEvents_IsResizedLeft        = 1 << 3,
            ImmediateUserInterfaceNodeEvents_IsResizedRight       = 1 << 4,
            ImmediateUserInterfaceNodeEvents_IsResizedBottom      = 1 << 5,
            ImmediateUserInterfaceNodeEvents_IsResizedTopLeft     = 1 << 6,
            ImmediateUserInterfaceNodeEvents_IsResizedTopRight    = 1 << 7,
            ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft  = 1 << 8,
            ImmediateUserInterfaceNodeEvents_IsResizedBottomRight = 1 << 9,
        };

        enum ImmediateUserInterfaceNodeColors_ : int
        {
            ImmediateUserInterfaceNodeColors_Begin         = 0,

            // window background
            ImmediateUserInterfaceNodeColors_WindowOutline = ImmediateUserInterfaceNodeColors_Begin,
            ImmediateUserInterfaceNodeColors_WindowBackground,
            
            // window frame
            ImmediateUserInterfaceNodeColors_WindowFrameBackground,
            ImmediateUserInterfaceNodeColors_WindowFrameBackgroundActive,
            ImmediateUserInterfaceNodeColors_WindowFrameBackgroundHovered,

            // gizmos
            ImmediateUserInterfaceNodeColors_Gizmos,
            ImmediateUserInterfaceNodeColors_GizmosHovered,

            // text
            ImmediateUserInterfaceNodeColors_Text,
            
            ImmediateUserInterfaceNodeColors_End
        };

        enum ImmediateUserInterfaceNodeSettings_ : int
        {
            // sentinel
            ImmediateUserInterfaceNodeSettings_None      = 0,

            // settings
            ImmediateUserInterfaceNodeSettings_Movable   = 1 << 0,
            ImmediateUserInterfaceNodeSettings_Resizable = 1 << 1
        };

        enum ImmediateUserInterfaceNodeMouseHover_ : int
        {
            ImmediateUserInterfaceNodeMouseHover_None         = 0,
            ImmediateUserInterfaceNodeMouseHover_MouseLeft    = 1 << 0,
            ImmediateUserInterfaceNodeMouseHover_MouseHovered = 1 << 1,
            ImmediateUserInterfaceNodeMouseHover_MouseEntered = 1 << 2,
        };

        typedef int ImmediateUserInterfaceNodeEvents;
        typedef int ImmediateUserInterfaceNodeSettings;
        typedef int ImmediateUserInterfaceNodeMouseHover;
        class ImmediateUserInterfaceContextLayer;

        struct ImmedidateUserInterfaceStyle
        {
            ImmedidateUserInterfaceStyle()
            {
                Colors.resize(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End);

                // window
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowOutline]                      = gs_vec4f(12.f, 64.f, 128.f, 255.f);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowBackground]                   = gs_vec4f(12.f, 128.f, 200.f, 255.f);
                
                // window frame
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowFrameBackground]              = gs_vec4f(12.f, 128.f, 200.f, 255.f);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowFrameBackgroundActive]        = gs_vec4f(32.f, 175.f, 255.f, 255.f);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_WindowFrameBackgroundHovered]       = gs_vec4f(32.f, 175.f, 255.f, 255.f);
                
                // gizmos
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos]                             = gs_vec4f(32.f, 200.f, 200.f, 200.f);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered]                      = gs_vec4f(32.f, 230.f, 200.f, 200.f);

                // gizmos
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text]                               = gs_vec4f(255.f, 255.f, 255.f, 255.f);
            }
            ~ImmedidateUserInterfaceStyle(){}

            std::vector<gs_vec4f> Colors;

            // font
            RenderingQueueFont Font;
            float              FontSize = 64.f;

            // frames of windows, buttons, child windows e.t.c
            float FramesRadius = 32.f;
            float FramesWidth  = 8.f;
        };

        struct ImmedidateUserInterfaceEvent
        {
            gs_vec2f             CursorPosition {gs_vec2f(0.f, 0.f)};
            gs_vec2f             CursorDragDelta{gs_vec2f(0.f, 0.f)};

            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseDown;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseHold;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MousePressed;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseClicked;
            Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseDoubleClicked;
        };

        struct ImmediateUserInterfaceNode
        {
            ImmediateUserInterfaceNode(const std::string _Name);
            virtual ~ImmediateUserInterfaceNode();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context);
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context);
            virtual void measure(ImmediateUserInterfaceContextLayer* _Context);
            virtual void events(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event);

            bool is_visible() const;
            bool is_partially_visible() const;

            int place_in_follow();

            struct Data
            {
                // rendering
                int            Depth         {0};
                int            InitialDepth  {0};
                int            SelfThickness {0}; // thickness of self rendered content
                int            TotalThickness{0}; // thickness of self rendered content plus contented rendered by children
                int            RenderingIndex{0}; // index of the node within context rendering list
                int            RenderingOrder{0};
                bool           RenderChildren{1}; // defines if the children of this node are drawn
                bool           RenderedAlways{0}; // defines if this node is always drawn ignoring 'RenderChildren' setting

                // geimetry
                gs_2dboxf      BoundingBox{gs_2dboxf(gs_vec2f(32.f, 32.f), gs_vec2f(1024.f, 512.f))};
                gs_vec2f       ContentSize{gs_vec2f(0.f)};
                gs_vec2f       MinimumSize{gs_vec2f(32.f)};
                gs_vec2f       MaximumSize{gs_vec2f((float)INT_MAX)};

                // hierarchy
                ImmediateUserInterfaceNode*       Parent{nullptr};

                // settings
                ImmediateUserInterfaceNodeSettings Settings{ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable};

                // events
                ImmediateUserInterfaceNodeEvents   Events{ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None};

                // mouse hover
                ImmediateUserInterfaceNodeMouseHover           MouseHover{ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None};
                std::chrono::high_resolution_clock::time_point MouseEnterTimer;
                std::chrono::high_resolution_clock::time_point MouseLeaveTimer;

                // mouse and keyboard input
                // TODO: implement keyboard input !!!
                Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseDown;
                Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseHold;
                Frenchie::Core::Optional<ApplicationMouseButton::Button> MousePressed;
                Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseClicked;
                Frenchie::Core::Optional<ApplicationMouseButton::Button> MouseDoubleClicked;
            };

            Data State;
            Data Cache;

        //private:
            std::string Name = "UINode";
        };

        struct ImmedidateUserInterfaceNodeHierarchy
        {
            ImmedidateUserInterfaceNodeHierarchy(const std::function<ImmediateUserInterfaceNode*(ImmediateUserInterfaceNode*)> _GetParent =
                [](ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
                {
                    return _Node != nullptr ? _Node->State.Parent : nullptr;
                }) : GetParent(_GetParent){}

            ~ImmedidateUserInterfaceNodeHierarchy(){}

            std::vector<int>                                                        Indexes;
            std::vector<int>                                                        Entries;
            std::vector<ImmediateUserInterfaceNode*>                                Singletons;
            std::vector<ImmediateUserInterfaceNode*>                                Sorted;
            std::function<ImmediateUserInterfaceNode*(ImmediateUserInterfaceNode*)> GetParent;

            std::vector<ImmediateUserInterfaceNode*>::iterator begin(const ImmediateUserInterfaceNode* _Node)
            {
                if(_Node == nullptr                                             ||
                    _Node->State.RenderingIndex          >= (int)Indexes.size() ||
                    Indexes[_Node->State.RenderingIndex] >= (int)Sorted.size())
                {
                    return Sorted.end();
                }

                return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.RenderingIndex];
            }

            std::vector<ImmediateUserInterfaceNode*>::iterator end(const ImmediateUserInterfaceNode* _Node)
            {
                if(_Node == nullptr                                                 ||
                    _Node->State.RenderingIndex + 1          >= (int)Indexes.size() ||
                    Indexes[_Node->State.RenderingIndex + 1] >= (int)Sorted.size())
                {
                    return Sorted.end();
                }

                return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.RenderingIndex + 1];
            }

            int size(const ImmediateUserInterfaceNode* _Node)
            {
                return (int)(end(_Node) - begin(_Node));
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

                    if(_Nodes[i]->State.Parent == nullptr)
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

        private:

            ImmediateUserInterfaceNode* get_parent(ImmediateUserInterfaceNode* _Node)
            {
                return GetParent != nullptr ? GetParent(_Node) : nullptr;
            }
        };

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

            // API
            bool begin_window(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, bool* _Opened = nullptr);
            void end_window();
            
            bool begin_vertial_stack(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings);
            void end_vertical_stack();

            bool begin_horizontal_stack(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings);
            void end_horizontal_stack();

            // hierarchy
            template<typename Type> Type* get_rendering_stack_top() const
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

                // create node
                if(m_Cache.find(_ID) == m_Cache.end())
                {
                    m_Duplicates.insert(_ID);
                    GS_ASSERT(m_Duplicates.count(_ID) <= 1);
                    m_Cache[_ID] = std::make_unique<Type>(_ID);
                }
                ImmediateUserInterfaceNode* node = m_Cache[_ID].get();

                if(node->Cache.Parent != nullptr              &&
                    !node->Cache.Parent->Cache.RenderChildren &&
                    !node->Cache.RenderedAlways)
                    return false;

                node->State.Settings       = _Settings;
                node->State.RenderingIndex = (int)m_NodesRenderingList.size();

                // build nodes hierarchy
                if(!m_NodesRenderingStack.empty())
                    node->State.Parent = m_NodesRenderingStack[m_NodesRenderingStack.size() - 1];

                m_NodesRenderingList.push_back(node);
                m_NodesRenderingStack.push_back(node);

                return true;
            }

            template<typename Type>
            void end_node()
            {
                if(m_NodesRenderingStack.empty())
                    return;

                GS_ASSERT((dynamic_cast<Type*>(m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]) != nullptr));

                m_NodesRenderingStack.pop_back();
            }

            mutable std::map<std::string, std::unique_ptr<ImmediateUserInterfaceNode>> m_Cache;
            mutable ImmedidateUserInterfaceStyle                                       m_Style;
            mutable std::multiset<std::string>                                         m_Duplicates;
            mutable ImmedidateUserInterfaceNodeHierarchy                               m_Hierarchy;

            // rendering
            mutable std::shared_ptr<Immediate2DRenderer>                               m_Renderer{nullptr};
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingList;
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingCache;
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingStack;

            // docking and snapping
            mutable ImmedidateUserInterfaceNodeHierarchy     m_WindowsDockingHierarchy;

            mutable ImmedidateUserInterfaceNodeHierarchy     m_WindowsTopSnappingHierarchy;
            mutable ImmedidateUserInterfaceNodeHierarchy     m_WindowsLeftSnappingHierarchy;
            mutable ImmedidateUserInterfaceNodeHierarchy     m_WindowsRightSnappingHierarchy;
            mutable ImmedidateUserInterfaceNodeHierarchy     m_WindowsBottomSnappingHierarchy;

            mutable std::vector<ImmediateUserInterfaceNode*> m_WindowsDockingCache;
            mutable std::vector<ImmediateUserInterfaceNode*> m_WindowsDockingList;
        };
    };
}