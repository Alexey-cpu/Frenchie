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
            
            // resize
            ImmediateUserInterfaceNodeEvents_IsResizedTop         = 1 << 1,
            ImmediateUserInterfaceNodeEvents_IsResizedLeft        = 1 << 2,
            ImmediateUserInterfaceNodeEvents_IsResizedRight       = 1 << 3,
            ImmediateUserInterfaceNodeEvents_IsResizedBottom      = 1 << 4,
            ImmediateUserInterfaceNodeEvents_IsResizedTopLeft     = 1 << 5,
            ImmediateUserInterfaceNodeEvents_IsResizedTopRight    = 1 << 6,
            ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft  = 1 << 7,
            ImmediateUserInterfaceNodeEvents_IsResizedBottomRight = 1 << 8,
        };

        enum ImmediateUserInterfaceNodeColors_ : int
        {
            ImmediateUserInterfaceNodeColors_Begin         = 0,

            // windows
            ImmediateUserInterfaceNodeColors_ChildBackground = ImmediateUserInterfaceNodeColors_Begin,
            ImmediateUserInterfaceNodeColors_ChildBackgroundHovered,
            ImmediateUserInterfaceNodeColors_ParentBackground,
            ImmediateUserInterfaceNodeColors_ParentBackgroundHovered,

            // push button
            ImmediateUserInterfaceNodeColors_ButtonOutline,
            ImmediateUserInterfaceNodeColors_ButtonBackground,
            ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered,
            ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed,

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
            ImmediateUserInterfaceNodeSettings_Movable     = 1 << 0,
            ImmediateUserInterfaceNodeSettings_Resizable   = 1 << 1,

            ImmediateUserInterfaceNodeSettings_UserDefined = 1 << 2,

            ImmediateUserInterfaceNodeSettings_Defaults  =
                ImmediateUserInterfaceNodeSettings_Movable |
                ImmediateUserInterfaceNodeSettings_Resizable,
        };

        enum ImmediateUserInterfaceContextSettings_ : int
        {
            // docking
            ImmediateUserInterfaceContextSettings_DisableDocking         = 1 << 0,
            ImmediateUserInterfaceContextSettings_EnableWindowsDocking   = 1 << 1,
            ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking = 1 << 2,
        };

        enum ImmediateUserInterfaceScrollAreaSettings_ : int
        {
            ImmediateUserInterfaceScrollAreaSettings_NeverVerticalScrollBar      = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined,
            ImmediateUserInterfaceScrollAreaSettings_AlwaysVerticalScrollBar     = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 1,
            ImmediateUserInterfaceScrollAreaSettings_AdaptiveVerticalScrollBar   = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 2,

            ImmediateUserInterfaceScrollAreaSettings_NeverHorizontalScrollBar    = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 3,
            ImmediateUserInterfaceScrollAreaSettings_AlwaysHorizontalScrollBar   = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 4,
            ImmediateUserInterfaceScrollAreaSettings_AdaptiveHorizontalScrollBar = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_UserDefined << 5,

            ImmediateUserInterfaceScrollAreaSettings_Defaults                    =
                ImmediateUserInterfaceScrollAreaSettings_AdaptiveVerticalScrollBar |
                ImmediateUserInterfaceScrollAreaSettings_AdaptiveHorizontalScrollBar,
        };

        enum ImmediateUserInterfaceNodeMouseHover_ : int
        {
            ImmediateUserInterfaceNodeMouseHover_None         = 0,
            ImmediateUserInterfaceNodeMouseHover_MouseLeft    = 1 << 0,
            ImmediateUserInterfaceNodeMouseHover_MouseHovered = 1 << 1,
            ImmediateUserInterfaceNodeMouseHover_MouseEntered = 1 << 2,
        };

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
        typedef int ImmediateUserInterfaceNodeSettings;
        typedef int ImmediateUserInterfaceContextSettings;
        typedef int ImmediateUserInterfaceNodeMouseHover;

        typedef int ImmedidateUserInterfaceDockingAnchor;
        typedef int ImmedidateUserInterfaceRenderingOrder;
        typedef int ImmedidateUserInterfaceRenderingLayer;

        class ImmediateUserInterfaceContextLayer;

        // style and events
        struct ImmedidateUserInterfaceStyle
        {
            ImmedidateUserInterfaceStyle()
            {
                Colors.resize(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End);

                // window
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground]                = RenderingQueueGraphicsApi::construct_rgba_color(72, 72, 72, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground]               = RenderingQueueGraphicsApi::construct_rgba_color(28, 28, 28, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered]        = RenderingQueueGraphicsApi::construct_rgba_color(72, 82, 72, 255);
                
                // push button
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline]                  = RenderingQueueGraphicsApi::construct_rgba_color(28, 28, 28, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground]               = RenderingQueueGraphicsApi::construct_rgba_color(72, 72, 72, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered]        = RenderingQueueGraphicsApi::construct_rgba_color(60, 72, 60, 255);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed]        = RenderingQueueGraphicsApi::construct_rgba_color(120, 128, 120, 255);

                // gizmos
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos]                         = RenderingQueueGraphicsApi::construct_rgba_color(50, 50, 100, 200);
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered]                  = RenderingQueueGraphicsApi::construct_rgba_color(100, 100, 172, 255);

                // gizmos
                Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text]                           = RenderingQueueGraphicsApi::construct_rgba_color(255, 255, 255, 255);
            }

            ~ImmedidateUserInterfaceStyle(){}

            // getters
            float get_frames_radius() const
            {
                return gs_max(FramesRadius, 0.f);
            }

            float get_frames_width() const
            {
                return gs_max(FramesWidth, 4.f);
            }

            float get_font_size() const
            {
                return gs_max(32.f, FontSize);
            }

            float get_scrollbar_width() const
            {
                return gs_max(get_frames_width(), ScrollBarWidth);
            }

            RenderingQueueColor get_color(const ImmediateUserInterfaceNodeColors_& _Color) const
            {
                return Colors[_Color];
            }

            // font
            RenderingQueueFont Font;

        private:

            // infos
            float                            FramesRadius   = 32.f;
            float                            FramesWidth    = 0.f;
            float                            FontSize       = 64.f;
            float                            ScrollBarWidth = 64.f;
            std::vector<RenderingQueueColor> Colors;
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

        // nodes
        struct ImmediateUserInterfaceNode
        {
            ImmediateUserInterfaceNode(const std::string _Name);
            virtual ~ImmediateUserInterfaceNode();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context);
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context);
            virtual void measure(ImmediateUserInterfaceContextLayer* _Context);
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event);
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
                int            Depth                      {0};
                int            InitialDepth               {0};
                int            SelfThickness              {0}; // thickness of self rendered content
                int            RenderingIndex             {0}; // index of the node within context rendering list
                int            RenderingOrder             {0};
                int            MaximumChildDepth          {0};
                int            MaximumChildThickness      {0};
                bool           PlaceInFollow              {false};
                bool           OrderChildrenWhileRendering{false};

                // geometry
                gs_2dboxf      BoundingBox                {gs_2dboxf(gs_vec2f(32.f, 32.f), gs_vec2f(1024.f, 512.f))};
                gs_vec2f       ContentSize                {gs_vec2f(0.f)};
                gs_vec2f       MinimumSize                {gs_vec2f(32.f)};
                gs_vec2f       MaximumSize                {gs_vec2f((float)INT_MAX)};

                // hierarchy
                ImmediateUserInterfaceNode*        Parent{nullptr};

                // settings
                ImmediateUserInterfaceNodeSettings Settings{ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable};

                // events
                ImmediateUserInterfaceNodeEvents   Events{ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None};

                // layout hints
                bool PushNextLine{false};

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
            bool Loaded{false};

        //private:
            std::string Name  = "UINode";
            std::string Hash  = "###UINode";
            int         Count = 0;
        };

        // windows
        struct ImmediateUserInterfaceWindow : public ImmediateUserInterfaceNode
        {
        public:

            ImmediateUserInterfaceWindow(const std::string& _Name);
            virtual ~ImmediateUserInterfaceWindow();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event) override;
            virtual void attach_child(ImmediateUserInterfaceNode*   _Child) override;
            virtual void load_state(ImmediateUserInterfaceContextLayer*) override;
            virtual void save_state(ImmediateUserInterfaceContextLayer*) override;

            ImmediateUserInterfaceNode* Docker        {nullptr};
            ImmediateUserInterfaceNode* TopSnapper    {nullptr};
            ImmediateUserInterfaceNode* LeftSnapper   {nullptr};
            ImmediateUserInterfaceNode* RightSnapper  {nullptr};
            ImmediateUserInterfaceNode* BottomSnapper {nullptr};

            gs_2dboxf                   FrameBox;

            gs_2dboxf                   ContentBox;

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

        // layouts
        struct ImmediateUserInterfaceNodePanel : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceNodePanel(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNodePanel();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event) override;

            gs_vec2f ContentPadding = gs_vec2f(0.f, 0.f);
        };

        struct ImmediateUserInterfaceNodeVerticalStack : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceNodeVerticalStack(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNodeVerticalStack();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceNodeHorizontalStack : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceNodeHorizontalStack(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNodeHorizontalStack();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        // scroll area
        struct ImmediateUserInterfaceScrollAreaScrollBarSlider : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceScrollAreaScrollBarSlider(const std::string& _Name);
            virtual ~ImmediateUserInterfaceScrollAreaScrollBarSlider();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event);

            gs_vec2f Position      = gs_vec2f(0.f, 0.f);
            gs_vec2f PositionScale = gs_vec2f(1.f, 1.f);
        private:
            gs_vec2f PreviousPosition = gs_vec2f(0.f, 0.f);
        };
        
        struct ImmediateUserInterfaceScrollArea : public ImmediateUserInterfaceNodePanel
        {
        public:

            ImmediateUserInterfaceScrollArea(const std::string& _Name);
            virtual ~ImmediateUserInterfaceScrollArea();
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void attach_child(ImmediateUserInterfaceNode*   _Child) override;

            ImmediateUserInterfaceNode*                      ContentView         = nullptr;
            ImmediateUserInterfaceScrollAreaScrollBarSlider* VerticalScrollBar   = nullptr;
            ImmediateUserInterfaceScrollAreaScrollBarSlider* HorizontalScrollBar = nullptr;
        };

        // widgets
        struct ImmediateUserInterfacePushButton : public ImmediateUserInterfaceNode
        {
            ImmediateUserInterfacePushButton(const std::string& _Name);
            virtual ~ImmediateUserInterfacePushButton();
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
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
            mutable std::map<const ImmediateUserInterfaceNode*, int>                              RenderingIndexesCache;
            mutable std::function<ImmediateUserInterfaceNode*(const ImmediateUserInterfaceNode*)> GetParent;

            std::vector<ImmediateUserInterfaceNode*>::iterator begin(const ImmediateUserInterfaceNode* _Node) const
            {
                if(_Node == nullptr || RenderingIndexesCache.empty())
                    return Sorted.end();

                auto iterator = RenderingIndexesCache.find(_Node);

                if(iterator == RenderingIndexesCache.end())
                    return Sorted.end();

                if( iterator->second          >= (int)Indexes.size() ||
                    Indexes[iterator->second] >= (int)Sorted.size())
                {
                    return Sorted.end();
                }

                return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[iterator->second];
            }

            std::vector<ImmediateUserInterfaceNode*>::iterator end(const ImmediateUserInterfaceNode* _Node) const
            {
                if(_Node == nullptr || RenderingIndexesCache.empty())
                    return Sorted.end();

                auto iterator = RenderingIndexesCache.find(_Node);

                if(iterator == RenderingIndexesCache.end())
                    return Sorted.end();

                if(_Node == nullptr                                      ||
                    iterator->second + 1          >= (int)Indexes.size() ||
                    Indexes[iterator->second + 1] >= (int)Sorted.size())
                {
                    return Sorted.end();
                }

                return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[iterator->second + 1];
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

                    RenderingIndexesCache[_Nodes[i]] = _Nodes[i]->State.RenderingIndex;

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
            virtual void frame_debug(ImmediateUserInterfaceContextLayer*, const ImmedidateUserInterfaceEvent&){}
            virtual void frame_render(ImmediateUserInterfaceContextLayer*){}
            virtual void frame_finish(ImmediateUserInterfaceContextLayer*){}
        };

        class ImmedidateUserInterfaceWindowController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceWindowController();
            virtual ~ImmedidateUserInterfaceWindowController();
            virtual void frame_start(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event) override;

            void push_event(std::function<void(ImmediateUserInterfaceContextLayer*)> _Event);

            std::vector<ImmediateUserInterfaceNode*>&
            retrieve_docked_windows(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Docker, const ImmedidateUserInterfaceDockingAnchor& _Anchors);

        private:

            void place_on_dockers(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event);
            bool can_be_docked(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked);
            void attach_to_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked, const ImmedidateUserInterfaceDockingAnchor& _Anchor);
            void detach_from_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Detached);

            mutable std::vector<ImmediateUserInterfaceNode*>                             m_WindowsDockingCache;
            mutable std::vector<ImmediateUserInterfaceNode*>                             m_WindowsDockingList;
            mutable std::stack<std::function<void(ImmediateUserInterfaceContextLayer*)>> m_DockingEventsStack;
            mutable ImmediateUserInterfaceNode*                                          m_WorkspaceDockArea      {nullptr};
            mutable bool                                                                 m_WorkspaceDockAreaOpened{true};
        };
    
        class ImmedidateUserInterfaceEventsController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceEventsController();
            virtual ~ImmedidateUserInterfaceEventsController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event) override;

        private:

            void catch_hover(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event);
            void catch_input(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event);
            void catch_event(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent& _Event);
        };
    
        class ImmedidateUserInterfaceLayoutController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceLayoutController();
            virtual ~ImmedidateUserInterfaceLayoutController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context, const ImmedidateUserInterfaceEvent&) override;

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

        class ImmedidateUserInterfaceNextNodeController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceNextNodeController();
            virtual ~ImmedidateUserInterfaceNextNodeController();
            virtual void frame_start(ImmediateUserInterfaceContextLayer*);

            void set_next_line()
            {
                PushNextLine = true;
            }

            bool is_next_line() const
            {
                bool value = PushNextLine.has_value();
                PushNextLine.reset();
                return value;
            }

        private:
            // info
            mutable Frenchie::Core::Optional<bool> PushNextLine;
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

            // windows
            bool begin_window(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, bool* _Opened = nullptr);
            void end_window();
            
            // layout
            bool begin_vertial_stack(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_vertical_stack();

            bool begin_horizontal_stack(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void end_horizontal_stack();

            // scroll area
            bool begin_scrollarea(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings =
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults            |
                ImmediateUserInterfaceScrollAreaSettings_::ImmediateUserInterfaceScrollAreaSettings_Defaults);
            void end_scrollarea();

            // widgets
            bool push_button(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            void next_line();

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

                // create node (output is never nullptr)
                ImmediateUserInterfaceNode* node = create_node<Type>(_ID);

                // setup node parameters
                node->State.Settings       = _Settings;
                node->State.RenderingIndex = (int)m_NodesRenderingList.size();
                node->State.PushNextLine   = get_controller<ImmedidateUserInterfaceNextNodeController>()->is_next_line();

                // build nodes hierarchy
                if(!m_NodesRenderingStack.empty())
                    m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]->attach_child(node);

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

            mutable std::map<uint32_t, std::unique_ptr<ImmediateUserInterfaceNode>> m_Cache;
            mutable ImmedidateUserInterfaceStyle                                    m_Style;
            mutable ImmedidateUserInterfaceHierarchy                                m_Hierarchy;

            // rendering
            mutable std::shared_ptr<Immediate2DRenderer>                            m_Renderer{nullptr};
            mutable std::vector<ImmediateUserInterfaceNode*>                        m_NodesRenderingList;
            mutable std::vector<ImmediateUserInterfaceNode*>                        m_NodesRenderingCache;
            mutable std::vector<ImmediateUserInterfaceNode*>                        m_NodesRenderingStack;

            // ini file
            ImmediateUserInterfaceContextConfiguration                              m_IniFileState;

            // settings
            ImmediateUserInterfaceContextSettings                                   m_Settings = ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking | ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking;

        private:

            // info
            std::vector<std::unique_ptr<ImmediateUserInterfaceContextController>> m_Controllers;
            std::u32string                                                        m_IniFilePath = U"Frenchie.ini";

            // service methods
            template<typename Type> Type* create_node(const std::string& _ID)
            {
                // FNV‑1a
                auto hashFunction = [](const void* _Data, size_t _Length)->uint32_t
                {
                    const uint8_t *bytes = (const uint8_t*)_Data;
                    uint32_t       hash  = 2166136261u;

                    for (size_t i = 0; i < _Length; i++)
                    {
                        hash ^= bytes[i];
                        hash *= 16777619u;
                    }
                    return hash;
                };

                // determine hashable part of the _ID
                int hashable = 0;

                for (;hashable < (int)_ID.size(); hashable++)
                {
                    // TODO: here we should hash only if there are ### but now it is #
                    if(_ID[hashable] == '#') break;
                }
                
                // compute hash
                unsigned int hash =
                    hashable < _ID.size() ?
                        hashFunction(_ID.c_str() + hashable, _ID.size()) :
                            hashFunction(_ID.c_str(), _ID.size());

                // create node
                if(m_Cache.find(hash) == m_Cache.end())
                {
                    m_Cache[hash] = std::make_unique<Type>(
                        hashable < _ID.size() ?
                            std::string(_ID.c_str() + hashable, _ID.c_str() + _ID.size()) :
                                std::string(_ID.c_str(), _ID.c_str() + hashable));
                }
                ImmediateUserInterfaceNode* node = m_Cache[hash].get();
                GS_ASSERT((++node->Count) <= 1);

                // TODO: optimize this !!!
                if(node->Name != std::string(_ID.c_str(), _ID.c_str() + hashable))
                    node->Name = std::string(_ID.c_str(), _ID.c_str() + hashable);

                return dynamic_cast<Type*>(node);
            }
        };
    };
}