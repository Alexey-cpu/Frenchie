#include <ImmediateUserInterfaceLayer.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

// STL
#include <algorithm>
#include <typeinfo>
#include <typeindex>

//#define IMMEDIATE_USER_INTERFACE_DEBUG

using namespace Frenchie::Application;

#ifdef IMMEDIATE_USER_INTERFACE_DEBUG

#include <iostream>

void showHierarchy(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const std::string& _Delimiter)
{
    std::cout << _Delimiter << _Node->Hash << "\t" << _Node->State.Depth << "\t" << _Node->State.SelfThickness << "\t" << _Node->State.MaximumChildDepth << "\n";

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
    {
        showHierarchy(_Context, (*it), _Delimiter + "\t");
    }
}
#endif

// declaration
namespace Frenchie
{
    namespace Application
    {
        // layouts
        struct ImmediateUserInterfaceNodePanel : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceNodePanel(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNodePanel();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;

            gs_vec4f ContentPadding
            {
                0.f, // top
                0.f, // left
                0.f, // right
                0.f  // bottom
            };

            gs_vec4f ContentMargin
            {
                0.f, // top
                0.f, // left
                0.f, // right
                0.f  // bottom
            };

            // gs_vec2f ContentPadding = gs_vec2f(0.f, 0.f); // creates border inside between child elements
            // gs_vec2f ContentMargin  = gs_vec2f(0.f, 0.f); // creates border outside
        };

        struct ImmediateUserInterfaceNodeVerticalStack : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceNodeVerticalStack(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNodeVerticalStack();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceNodeHorizontalStack : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceNodeHorizontalStack(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNodeHorizontalStack();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        };
   
        // scroll area
        struct ImmediateUserInterfaceScrollAreaScrollBar;

        enum ImmediateUserInterfaceScrollAreaScrollBarType_
        {
            ImmediateUserInterfaceScrollAreaScrollBarType_Vertical,
            ImmediateUserInterfaceScrollAreaScrollBarType_Horizontal,
        };

        struct ImmediateUserInterfaceScrollArea : public ImmediateUserInterfaceNodePanel
        {
        public:

            ImmediateUserInterfaceScrollArea(const std::string& _Name);
            virtual ~ImmediateUserInterfaceScrollArea();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;

            virtual void attach_child(ImmediateUserInterfaceNode*   _Child) override;

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr) override;

            float get_horizontal_scrollbar_width(ImmediateUserInterfaceContextLayer*) const;
            float get_vertical_scrollbar_width(ImmediateUserInterfaceContextLayer*) const;

            ImmediateUserInterfaceNode*                ContentView         = nullptr;
            ImmediateUserInterfaceNode*                ContentPanel        = nullptr;
            ImmediateUserInterfaceScrollAreaScrollBar* VerticalScrollBar   = nullptr;
            ImmediateUserInterfaceScrollAreaScrollBar* HorizontalScrollBar = nullptr;

        private:
            void calculate_content_padding(ImmediateUserInterfaceContextLayer*, ImmediateUserInterfaceNode*, gs_vec2f&);
        };

        struct ImmediateUserInterfaceScrollAreaPanel : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceScrollAreaPanel(const std::string& _Name);
            virtual ~ImmediateUserInterfaceScrollAreaPanel();
        };

        struct ImmediateUserInterfaceScrollAreaContent : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceScrollAreaContent(const std::string& _Name);
            ~ImmediateUserInterfaceScrollAreaContent();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceScrollAreaScrollBar : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceScrollAreaScrollBar(const std::string& _Name);
            virtual ~ImmediateUserInterfaceScrollAreaScrollBar();

            // getters
            gs_vec2f get_scroll_offset() const;

            // setters
            void set_scroll_offset(const gs_vec2f _Value);

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;

            ImmediateUserInterfaceScrollAreaScrollBarType_ Type =
                ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical;

        private:

            // info
            gs_vec2f Size             = gs_vec2f(0.f, 0.f);
            gs_vec2f Position         = gs_vec2f(0.f, 0.f);
            gs_vec2f PositionScale    = gs_vec2f(1.f, 1.f);
            gs_vec2f PreviousPosition = gs_vec2f(0.f, 0.f);

            ImmediateUserInterfaceScrollArea* retrieve_scroll_area(ImmediateUserInterfaceContextLayer* _Context);
        };

        // menu
        struct ImmediateUserInterfaceMenu : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceMenu(const std::string& _Name);
            virtual ~ImmediateUserInterfaceMenu();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override;

            ImmediateUserInterfaceScrollArea* InternalScrollArea = nullptr;
            ImmediateUserInterfaceScrollArea* ExternalScrollArea = nullptr;
        };
    
        struct ImmediateUserInterfaceMenuScrollArea : public ImmediateUserInterfaceScrollArea
        {
        public:
            ImmediateUserInterfaceMenuScrollArea(const std::string& _Name);
            virtual ~ImmediateUserInterfaceMenuScrollArea();
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceMenuAction : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceMenuAction(const std::string& _Name);
            virtual ~ImmediateUserInterfaceMenuAction();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceMenuItem : public ImmediateUserInterfaceMenuAction
        {
        public:
            ImmediateUserInterfaceMenuItem(const std::string& _Name);
            virtual ~ImmediateUserInterfaceMenuItem();
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        // combobox
        struct ImmediateUserInterfaceCombobox : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceCombobox(const std::string& _Name);
            virtual ~ImmediateUserInterfaceCombobox();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override;

            ImmediateUserInterfaceScrollArea*              ScrollArea{nullptr};
            bool                                           Active    {false};
            bool                                           Hovered   {false};
            std::chrono::high_resolution_clock::time_point HoverTime {std::chrono::high_resolution_clock::time_point()};
        };

        struct ImmediateUserInterfaceComboboxItem : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceComboboxItem(const std::string& _Name);
            virtual ~ImmediateUserInterfaceComboboxItem();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceComboboxScrollArea : public ImmediateUserInterfaceScrollArea
        {
        public:
            ImmediateUserInterfaceComboboxScrollArea(const std::string& _Name);
            virtual ~ImmediateUserInterfaceComboboxScrollArea();
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        // tree
        struct ImmediateUserInterfaceTreeNode : public ImmediateUserInterfaceNodePanel
        {
        public:
            ImmediateUserInterfaceTreeNode(const std::string& _Name);
            virtual ~ImmediateUserInterfaceTreeNode();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void measure(ImmediateUserInterfaceContextLayer* _Context) override;

            // info
            bool                               Opened        {false};
            gs_2dboxf                          TitleBox      {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
            gs_2dboxf                          IconBox       {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
            ApplicationRenderingBackendTexture TextureOpened {ApplicationRenderingBackendTexture()};
            ApplicationRenderingBackendTexture TextureClosed {ApplicationRenderingBackendTexture()};
            int                                TreeSettings  {0};
        };

        // windows
        struct ImmediateUserInterfaceWindow : public ImmediateUserInterfaceNode
        {
        public:

            ImmediateUserInterfaceWindow(const std::string& _Name);
            virtual ~ImmediateUserInterfaceWindow();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void attach_child(ImmediateUserInterfaceNode*   _Child) override;

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr) override;

            virtual void load_state(ImmediateUserInterfaceContextLayer*) override;
            virtual void save_state(ImmediateUserInterfaceContextLayer*) override;

            ImmediateUserInterfaceNode* Docker            {nullptr};
            ImmediateUserInterfaceNode* TopSnapper        {nullptr};
            ImmediateUserInterfaceNode* LeftSnapper       {nullptr};
            ImmediateUserInterfaceNode* RightSnapper      {nullptr};
            ImmediateUserInterfaceNode* BottomSnapper     {nullptr};
            bool                        IsActive          {true};
            bool                        Activate          {false};
            bool                        ReattachChildren  {false};

            gs_2dboxf                   FrameBox          {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
            gs_2dboxf                   ContentBox        {gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
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

            template<typename FrameProcessor>
            void process_window_frame(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Window, const FrameProcessor& _FrameProcessor)
            {
                if(_Context == nullptr || _Window == nullptr || _Window->Docker != nullptr)
                    return;

                // calculate total windows frames size and frames count
                gs_vec2f totalFramesSize  = gs_vec2f(0.f, 0.f);
                int      totalFramesCount = 0;

                for(auto it  = _Context->m_Hierarchy.begin(_Window->DockerView);
                            it != _Context->m_Hierarchy.end(_Window->DockerView);
                            it++)
                {
                    if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) == nullptr) continue;

                    totalFramesSize += gs_vec2f(
                        _Context->m_Renderer->calculate_bounding_box((*it)->Name.begin(), (*it)->Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).width(),
                        0.f);
                    totalFramesCount++;
                }

                totalFramesSize += gs_vec2f(
                    _Context->m_Renderer->calculate_bounding_box(_Window->Name.begin(), _Window->Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).width(),
                    0.f);
                totalFramesCount++;

                totalFramesSize = gs_vec2f(
                    gs_min(totalFramesSize.x, _Window->FrameBox.width() * 0.8f), // TODO: 0.8f MUST BE A SETTING !!!
                    _Window->FrameBox.height());

                // calculate frame unlocking and dragging panes
                gs_2dboxf frameBoxUnlockingPane = gs_2dboxf(_Window->FrameBox.Min, _Window->FrameBox.Min + totalFramesSize);

                // process frameboxes
                gs_vec2f position = frameBoxUnlockingPane.Min;
                gs_vec2f size     = gs_vec2f((frameBoxUnlockingPane.size() / (float)totalFramesCount).x, frameBoxUnlockingPane.size().y);

                // process self frame
                gs_2dboxf boundingBox = gs_2dboxf(position, position + size);
                _FrameProcessor(_Context, _Window->FrameBox, boundingBox, _Window);
                position += gs_vec2f(size.x, 0.f);

                // process docked windows frames
                for(auto it  = _Context->m_Hierarchy.begin(_Window->DockerView);
                            it != _Context->m_Hierarchy.end(_Window->DockerView);
                            it++)
                {
                    ImmediateUserInterfaceWindow* window =
                        dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

                    if(window == nullptr || window->Docker == nullptr)
                        continue;

                    _FrameProcessor(
                        _Context,
                        gs_2dboxf(gs_vec2f(0.f), gs_vec2f(0.f)),
                        gs_2dboxf(position, position + size),
                        dynamic_cast<ImmediateUserInterfaceWindow*>(*it));

                    position += gs_vec2f(size.x, 0.f);
                }
            }

            static ImmediateUserInterfaceWindow* retrieve_docker_by_view(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _DockerView)
            {
                ImmediateUserInterfaceNode* parent = _DockerView;

                while (parent)
                {
                    if(dynamic_cast<ImmediateUserInterfaceWindow*>(parent))
                        return dynamic_cast<ImmediateUserInterfaceWindow*>(parent);
                    parent = _Context->m_Hierarchy.get_parent(parent);
                }
                
                return nullptr;
            }
        };

        struct ImmediateUserInterfaceWindowDockArea : public ImmediateUserInterfaceWindow
        {
            ImmediateUserInterfaceWindowDockArea(const std::string& _Name);
            virtual ~ImmediateUserInterfaceWindowDockArea();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceWindowCentralDocker : public ImmediateUserInterfaceNodePanel
        {
            ImmediateUserInterfaceWindowCentralDocker(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
            virtual ~ImmediateUserInterfaceWindowCentralDocker(){}
        };

        struct ImmediateUserInterfaceWindowVerticalSnapper : public ImmediateUserInterfaceNodeVerticalStack
        {
            ImmediateUserInterfaceWindowVerticalSnapper(const std::string& _Name) : ImmediateUserInterfaceNodeVerticalStack(_Name){}
            virtual ~ImmediateUserInterfaceWindowVerticalSnapper(){}
        };

        struct ImmediateUserInterfaceWindowHorizontalSnapper : public ImmediateUserInterfaceNodeHorizontalStack
        {
            ImmediateUserInterfaceWindowHorizontalSnapper(const std::string& _Name) : ImmediateUserInterfaceNodeHorizontalStack(_Name){}
            virtual ~ImmediateUserInterfaceWindowHorizontalSnapper(){}

            virtual void measure(ImmediateUserInterfaceContextLayer* _Context) override
            {
                ImmediateUserInterfaceNodeHorizontalStack::measure(_Context);

                State.MinimumSize = gs_vec2f(0.f, 0.f);

                if(_Context->m_Hierarchy.size(this) <= 0)
                    State.BoundingBox = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min);
                else if(gs_min(State.BoundingBox.size().x, State.BoundingBox.size().y) <= 1.f)
                    State.BoundingBox = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min + gs_vec2f(512.f, 512.f));
            }
        };

        // controllers
        class ImmedidateUserInterfaceWindowController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceWindowController();
            virtual ~ImmedidateUserInterfaceWindowController();
            virtual void frame_start(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_finish(ImmediateUserInterfaceContextLayer*) override;

            std::vector<ImmediateUserInterfaceNode*>&
            retrieve_docked_windows(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Docker, const ImmedidateUserInterfaceDockingAnchor& _Anchors);

        private:

            void place_on_dockers(ImmediateUserInterfaceContextLayer* _Context);
            bool can_be_docked(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked);
            void attach_to_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked, const ImmedidateUserInterfaceDockingAnchor& _Anchor);
            void detach_from_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Detached);

            mutable std::vector<ImmediateUserInterfaceNode*>  m_WindowsDockingCache;
            mutable std::vector<ImmediateUserInterfaceNode*>  m_WindowsDockingList;
            mutable ImmediateUserInterfaceWindow*             m_WorkspaceDockArea {nullptr};
            mutable bool                                      m_DockAreaOpened    {false};
        };
    
        class ImmedidateUserInterfaceInputController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceInputController();
            virtual ~ImmedidateUserInterfaceInputController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context) override;
        };
    
        class ImmedidateUserInterfaceLayoutController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceLayoutController();
            virtual ~ImmedidateUserInterfaceLayoutController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context) override;

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

        class ImmedidateUserInterfaceMenusController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceMenusController();
            virtual ~ImmedidateUserInterfaceMenusController();

            virtual void frame_finish(ImmediateUserInterfaceContextLayer* _Context) override;

            mutable std::vector<ImmediateUserInterfaceMenu*> ActiveMenus;

        private:

            void detect_maximum_width(
                ImmediateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceNode*         _Node,
                float&                              _MaximumWidth);

            void setup_maximum_with(
                ImmediateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceNode*         _Node,
                float&                              _MaximumWidth);
        };
    
        class ImmedidateUserInterfaceNextNodeController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmedidateUserInterfaceNextNodeController();
            virtual ~ImmedidateUserInterfaceNextNodeController();

            virtual void frame_start(ImmediateUserInterfaceContextLayer*) override;
            
            // API
            void reset();

            // info
            mutable Frenchie::Core::Optional<int>      NextLine;
            mutable Frenchie::Core::Optional<float>    NextIndent;
            mutable Frenchie::Core::Optional<gs_vec2f> NextPosition;
            mutable Frenchie::Core::Optional<gs_vec2f> NextMaximumSize;
            mutable Frenchie::Core::Optional<gs_vec2f> NextMinimumSize;
            mutable Frenchie::Core::Optional<gs_vec4f> NextContentMargin;
            mutable Frenchie::Core::Optional<gs_vec4f> NextContentPadding;
        };

        class ImmediateUserInterfaceScrollBarsController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceScrollBarsController();
            virtual ~ImmediateUserInterfaceScrollBarsController();
            virtual void frame_debug(ImmediateUserInterfaceContextLayer* _Context) override;
        };
    
        // internal
        struct ImmedidateUserInterfaceDefaultInputTextFilter
        {
            bool operator()(const std::string&) const{return true;}
        };

        struct ImmedidateUserInterfaceDefaultInputTextCallback
        {
            void operator()(const std::string&) const{}
        };

        enum ImmediateUserInterfaceInputStringInternalSettings_ : int
        {
            ImmediateUserInterfaceInputStringInternalSettings_NoMultiline = 1 << 4,
        };

        typedef int ImmediateUserInterfaceInputStringInternalSettings;

        template<typename SymbolFilter = ImmedidateUserInterfaceDefaultInputTextFilter, typename InputTextCallback = ImmedidateUserInterfaceDefaultInputTextCallback>
        bool input_string_internal(
            ImmediateUserInterfaceContextLayer*                      _Context,
            const std::string&                                       _ID,
            std::string&                                             _Text,
            const ImmediateUserInterfaceInputStringSettings&         _InputSettings,
            const ImmediateUserInterfaceInputStringInternalSettings& _InternalSettings,
            const ImmediateUserInterfaceNodeSettings&                _NodeSettings,
            const SymbolFilter&                                      _InputTextFilter   = ImmedidateUserInterfaceDefaultInputTextFilter(),
            const InputTextCallback&                                 _InputTextCallback = ImmedidateUserInterfaceDefaultInputTextCallback())
        {
            // nested types
            struct ImmediateUserInterfaceInputString : public ImmediateUserInterfaceScrollArea
            {
            public:
                ImmediateUserInterfaceInputString(const std::string& _Name) : ImmediateUserInterfaceScrollArea(_Name)
                {
                    State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 128.f));
                }

                virtual ~ImmediateUserInterfaceInputString(){}

                virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
                {
                    if(_Context == nullptr || _Context->m_Renderer == nullptr)
                        return;

                    // outline
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        State.BoundingBox.Min,
                        State.BoundingBox.Max,
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }
            };

            struct ImmediateUserInterfaceInputStringContent : public ImmediateUserInterfaceNode
            {
            public:
                ImmediateUserInterfaceInputStringContent(const std::string& _Name): ImmediateUserInterfaceNode(_Name)
                {
                    State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
                }

                virtual ~ImmediateUserInterfaceInputStringContent(){}

                virtual void layout(ImmediateUserInterfaceContextLayer*) override{}

                static int move_cursor_left(const int& _Cursor, std::string& _Text)
                {
                    if(_Text.empty())
                        return 0;

                    auto iterator = _Text.begin() + _Cursor;
                    
                    if(iterator != _Text.begin())
                        Frenchie::Core::String::utf8_prior(iterator);
                    
                    return gs_clamp((int)(iterator - _Text.begin()), 0, gs_max((int)_Text.size(), 0));
                }

                static int move_cursor_right(const int& _Cursor, std::string& _Text)
                {
                    if(_Text.empty())
                        return 0;

                    if(_Cursor > (int)_Text.size())
                        return gs_clamp(_Cursor, 0, gs_max((int)_Text.size(), 0));

                    auto iterator = _Text.begin() + _Cursor;

                    if(iterator != _Text.end())
                        Frenchie::Core::String::utf8_next(iterator);
                    
                    return gs_clamp((int)(iterator - _Text.begin()), 0, gs_max((int)_Text.size(), 0));
                }

                static int move_cursor_up(const int& _Cursor, std::string& _Text)
                {
                    if(_Text.empty())
                        return 0;

                    int SymbolsCountTillLineStart = 0;

                    // move backward to find out how many symbols there are till the line start
                    {
                        auto iterator = _Text.begin() + _Cursor;

                        while (iterator > _Text.begin())
                        {
                            SymbolsCountTillLineStart++;
                            if(Frenchie::Core::String::utf8_prior(iterator) == '\n') break;
                        }
                    }

                    {
                        // move backward to find prvious line end
                        auto iterator = _Text.begin() + _Cursor;

                        while (iterator > _Text.begin())
                        {
                            if(Frenchie::Core::String::utf8_prior(iterator) == '\n') break;
                        }

                        // move backward to find previous line start
                        while (iterator > _Text.begin())
                        {
                            if(Frenchie::Core::String::utf8_prior(iterator) == '\n') break;
                        }

                        // move forward
                        while (iterator < _Text.end() && SymbolsCountTillLineStart > 0)
                        {
                            Frenchie::Core::String::utf8_next(iterator);
                            SymbolsCountTillLineStart--;
                        }

                        return (int)(iterator - _Text.begin());
                    }
                }

                static int move_cursor_down(const int& _Cursor, std::string& _Text)
                {
                    if(_Text.empty())
                        return 0;

                    if(_Cursor >= (int)_Text.size())
                        return gs_clamp(_Cursor, 0, gs_max((int)_Text.size(), 0));

                    int SymbolsCountTillLineStart = 0;

                    // move backward to find out how many symbols there are till the line start
                    {
                        auto iterator = _Text.begin() + _Cursor;

                        while (iterator > _Text.begin())
                        {
                            if(Frenchie::Core::String::utf8_prior(iterator) == '\n') break;
                            SymbolsCountTillLineStart++;
                        }
                    }

                    {
                        // move forward to find next line start
                        auto iterator = _Text.begin() + _Cursor;

                        while (iterator < _Text.end())
                        {
                            if(Frenchie::Core::String::utf8_next(iterator) == '\n') break;
                        }

                        // move forward to find next line end
                        while (iterator < _Text.end() && SymbolsCountTillLineStart > 0)
                        {
                            if(*iterator == '\n') break;
                            Frenchie::Core::String::utf8_next(iterator);
                            SymbolsCountTillLineStart--;
                        }

                        return (int)(iterator - _Text.begin());
                    }
                }

                int                                            Utf8LeftCursorPosition  = 0;
                int                                            Utf8RightCursorPosition = 0;
                std::chrono::high_resolution_clock::time_point CursorAnimtionTimer;
                std::chrono::high_resolution_clock::time_point CursorMovementTimer;
            };

            struct ImmediateUserInterfaceInputStringRenderingData
            {
                gs_vec2f                            CursorPosition;
                gs_2dboxf                           TextBoundingBox;
                Frenchie::Core::Optional<gs_2dboxf> HoveredSymbolBoundingBox;
                Frenchie::Core::Optional<int>       HoveredSymbolUtf8CursorPosition;
            };

            // auxiliary lambdas
            auto inputStringCharacterFilter   = [_InputTextFilter, _InternalSettings](const std::string& _Input)->bool
            {
                // internal filter first
                if((_InternalSettings & ImmediateUserInterfaceInputStringInternalSettings_::ImmediateUserInterfaceInputStringInternalSettings_NoMultiline))
                {
                    for(auto& symbol : _Input)
                    {
                        if(symbol == '\n')
                            return false;
                    }
                }

                // 
                if(!_InputTextFilter(_Input))
                    return false;

                return true;
            };

            auto inputStringCharacterChanger  = [_InputSettings](const unsigned int& _Symbol)->unsigned int
            {
                return _InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Password ? '*' : _Symbol;
            };

            auto inputStringScrollBarAdjuster = [](
                ImmediateUserInterfaceContextLayer*                   _Context,
                ImmediateUserInterfaceInputStringContent*             _Contents,
                ImmediateUserInterfaceInputString*                    _ScrollArea,
                const ImmediateUserInterfaceInputStringRenderingData& _RenderingData)
            {
                // move scroll bar if the text is behind visible area
                if(!_Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.contains(_RenderingData.CursorPosition))
                {
                    // adjust horizontal scrollbar
                    if(_ScrollArea != nullptr && _ScrollArea->HorizontalScrollBar != nullptr)
                    {
                        if(_RenderingData.CursorPosition.x > _Context->m_Hierarchy.get_parent(_Contents)->Cache.BoundingBox.Max.x)
                        {
                            _ScrollArea->HorizontalScrollBar->set_scroll_offset(
                                (_RenderingData.CursorPosition.x - _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Max.x) + _Context->m_Style.get_font_size());
                        }
                        if(_RenderingData.CursorPosition.x < _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Min.x)
                        {
                            _ScrollArea->HorizontalScrollBar->set_scroll_offset(
                                (_RenderingData.CursorPosition.x - _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Min.x) - _Context->m_Style.get_font_size());
                        }
                    }

                    // adjust vertical scrollbar position
                    if(_ScrollArea != nullptr && _ScrollArea->VerticalScrollBar != nullptr)
                    {
                        if(_RenderingData.CursorPosition.y > _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Max.y)
                        {
                            _ScrollArea->VerticalScrollBar->set_scroll_offset(
                                (_RenderingData.CursorPosition.y - _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Max.y) + _Context->m_Style.get_font_size());
                        }
                        if(_RenderingData.CursorPosition.y < _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Min.y)
                        {
                            _ScrollArea->VerticalScrollBar->set_scroll_offset(
                                (_RenderingData.CursorPosition.y - _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Min.y) - _Context->m_Style.get_font_size());
                        }
                    }
                }
            };

            // main code
            if(_Context == nullptr)
                return false;

            ImmediateUserInterfaceInputStringRenderingData inputStringRenderingData;

            // gemerate scroll area settings
            int scrollAreaSettings = _NodeSettings;
            scrollAreaSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment;
            scrollAreaSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment;
            scrollAreaSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable;
            scrollAreaSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;

            // generate scroll area contents settings
            int scrollAreaContentSettings = scrollAreaSettings;
            scrollAreaContentSettings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent;

            // begin widgets
            ImmediateUserInterfaceInputString*        scrollArea = nullptr;
            ImmediateUserInterfaceInputStringContent* widget     = nullptr;
            bool                                      edited     = false;

            _Context->next_content_margin(gs_vec4f(
                _Context->m_Style.get_frames_width(),
                _Context->m_Style.get_frames_width(),
                _Context->m_Style.get_frames_width(),
                _Context->m_Style.get_frames_width()));

            if(_Context->begin_node<ImmediateUserInterfaceInputString>(std::string(_ID).append("/ScrollArea"),
                scrollAreaSettings))
            {
                scrollArea = _Context->get_rendering_stack_top<ImmediateUserInterfaceInputString>();

                if(_Context->begin_node<ImmediateUserInterfaceInputStringContent>(
                    _ID,
                    scrollAreaContentSettings))
                {
                    widget = _Context->get_rendering_stack_top<ImmediateUserInterfaceInputStringContent>();

                    inputStringRenderingData.CursorPosition  = widget->State.BoundingBox.Min;
                    inputStringRenderingData.TextBoundingBox = gs_2dboxf(widget->State.BoundingBox.Min, widget->State.BoundingBox.Min);
                    inputStringRenderingData.HoveredSymbolUtf8CursorPosition.reset();

                    auto parent = _Context->m_Hierarchy.get_parent(widget);

                    while (parent != nullptr &&
                            dynamic_cast<ImmediateUserInterfaceInputString*>(parent) == nullptr)
                    {
                        if(parent->State.Selected)
                        {
                            widget->State.Selected = true;
                            parent->State.Selected = false;
                            break;
                        }

                        parent = _Context->m_Hierarchy.get_parent(parent);
                    }

                    // render
                    if(!widget->Dirty)
                    {
                        _Context->m_Renderer->push_clip_box(widget->get_clipping_box(_Context));

                        int depth = widget->Cache.Depth;

                        // render text
                        {
                            if(_Text.empty())
                            {
                                // restore text rendering data
                                inputStringRenderingData.HoveredSymbolBoundingBox        = inputStringRenderingData.TextBoundingBox;
                                inputStringRenderingData.HoveredSymbolUtf8CursorPosition = 0;

                                // restore widget state
                                widget->Utf8LeftCursorPosition  = 0;
                                widget->Utf8RightCursorPosition = 0;
                            }

                            if(widget->Utf8LeftCursorPosition > (int)_Text.size())
                                widget->Utf8LeftCursorPosition = (int)_Text.size();

                            if(widget->Utf8RightCursorPosition > (int)_Text.size())
                                widget->Utf8RightCursorPosition = (int)_Text.size();

                            _Context->m_Renderer->push_text(
                                widget->State.BoundingBox.Min + gs_max(_Context->m_Style.get_frames_radius() * 0.5f, 4.f),
                                _Text.begin(),
                                _Text.end(),
                                _Context->m_Style.get_font_size(),
                                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                                _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                                _Context->m_Style.get_current_font(), false,
                                RenderingQueue::DefaultSymbolProcessor(),
                                inputStringCharacterChanger);
                        }

                        // render selection bounding box
                        {
                            _Context->m_Renderer->push_text(
                                widget->State.BoundingBox.Min + gs_max(_Context->m_Style.get_frames_radius() * 0.5f, 4.f),
                                _Text.begin(),
                                _Text.end(),
                                _Context->m_Style.get_font_size(),
                                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                                _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                                _Context->m_Style.get_current_font(),
                                true,
                                [_Context, widget, &inputStringRenderingData, &depth](
                                    const gs_2dboxf&    _CurrentSymbolBoundingBox,
                                    const gs_vec2f&     _CursorPosition,
                                    const int&          _Utf8IteratorPosition,
                                    const unsigned int& _Symbol)
                                {
                                    // calculate text bounding box
                                    inputStringRenderingData.TextBoundingBox = gs_2dboxf(
                                        inputStringRenderingData.TextBoundingBox.Min,
                                        _CurrentSymbolBoundingBox.Min,
                                        inputStringRenderingData.TextBoundingBox.Max,
                                        _CurrentSymbolBoundingBox.Max);

                                    // calculate mouse hovered symbol bounding box
                                    float scale  = _Context->m_Style.get_current_font().get_scale(_Context->m_Style.get_font_size());
                                    float offset = _Context->m_Style.get_current_font().get_offset(_Context->m_Style.get_font_size());

                                    if(gs_2dboxf(
                                        _CursorPosition - gs_vec2f(4.f, offset * 0.5f),
                                        _CursorPosition + gs_vec2f(4.f, offset * 0.5f) + _CurrentSymbolBoundingBox.size()).contains(_Context->m_Renderer->get_cursor_postion()))
                                    {
                                        inputStringRenderingData.HoveredSymbolBoundingBox        = _CurrentSymbolBoundingBox;
                                        inputStringRenderingData.HoveredSymbolUtf8CursorPosition = _Utf8IteratorPosition;
                                    }

                                    // calculate cursor geometrical position
                                    if(widget->Utf8LeftCursorPosition == _Utf8IteratorPosition)
                                        inputStringRenderingData.CursorPosition = _CursorPosition;

                                    if(_Utf8IteratorPosition >= widget->Utf8LeftCursorPosition   &&
                                        _Utf8IteratorPosition <= widget->Utf8RightCursorPosition &&
                                        (widget->Utf8LeftCursorPosition != widget->Utf8RightCursorPosition))
                                    {
                                        float glyphAdvance = _Context->m_Style.get_current_font().contains_glyph(_Symbol) ? _Context->m_Style.get_current_font().retrieve_glyph(_Symbol).Advance * scale : 0.f;

                                        _Context->m_Renderer->push_rectangle_filled(
                                            _CursorPosition - gs_vec2f(gs_abs(_CurrentSymbolBoundingBox.size().x - glyphAdvance), 0.f),
                                            _CursorPosition + gs_vec2f(gs_abs(_CurrentSymbolBoundingBox.size().x - glyphAdvance), 0.f) + gs_vec2f(_CurrentSymbolBoundingBox.size().x, _Context->m_Style.get_font_size()),
                                            gs_color_rgba(
                                                gs_color_rgba_get_r(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                                                gs_color_rgba_get_g(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                                                gs_color_rgba_get_b(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                                                200),
                                            _Context->m_Renderer->calculate_transform_matrix((float)depth++));
                                    }
                                },
                                inputStringCharacterChanger);
                        }

                        // render hovered symbol bounding box
                        if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && inputStringRenderingData.HoveredSymbolBoundingBox.has_value())
                        {
                            _Context->m_Renderer->push_rectangle_filled(
                                inputStringRenderingData.HoveredSymbolBoundingBox.value().Min,
                                inputStringRenderingData.HoveredSymbolBoundingBox.value().Max,
                                gs_color_rgba(
                                    gs_color_rgba_get_r(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                                    gs_color_rgba_get_g(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                                    gs_color_rgba_get_b(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                                    200),
                                _Context->m_Renderer->calculate_transform_matrix((float)depth++));
                        }

                        // render cursor
                        if(widget->State.Selected && !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoInput))
                        {
                            if(widget->CursorAnimtionTimer.time_since_epoch().count() <= 0)
                            {
                                widget->CursorAnimtionTimer = Frenchie::Core::tic();
                            }
                            else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorAnimtionTimer, Frenchie::Core::tic()) > 300)
                            {
                                if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorAnimtionTimer, Frenchie::Core::tic()) < 700)
                                {
                                    _Context->m_Renderer->push_rectangle_filled(
                                        inputStringRenderingData.CursorPosition,
                                        inputStringRenderingData.CursorPosition + gs_vec2f(4.f, _Context->m_Style.get_font_size()),
                                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));
                                }
                                else
                                {
                                    widget->CursorAnimtionTimer = std::chrono::steady_clock::time_point();
                                }
                            }
                        }

                        _Context->m_Renderer->pop_clip_box();
                    }

                    // process events
                    {
                        // adjust scrollbar
                        if(widget->State.Selected && _Context->m_Input.is_mouse_button_hold() && !_Context->m_Input.is_mouse_button_pressed())
                        {
                            if(scrollArea != nullptr && scrollArea->HorizontalScrollBar != nullptr && (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                                scrollArea->HorizontalScrollBar->set_scroll_offset(gs_vector_normalize(_Context->m_Input.get_cusor_drag_delta()) * 4.f);
                            if(scrollArea != nullptr && scrollArea->VerticalScrollBar != nullptr && (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                                scrollArea->VerticalScrollBar->set_scroll_offset(gs_vector_normalize(_Context->m_Input.get_cusor_drag_delta()) * 4.f);
                        }

                        if(widget->State.Selected)
                        {
                            const int cursorMovementInterval = 80; // TODO: this MUST BE a setting !!!

                            // move cursor left
                            if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow) ||
                                _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow))
                            {
                                if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow))
                                {
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                }
                                else
                                {
                                    if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                                    {
                                        widget->CursorMovementTimer = Frenchie::Core::tic();
                                    }
                                    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                                    {
                                        widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                        widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                        widget->CursorMovementTimer     = std::chrono::steady_clock::time_point();
                                    }
                                }

                                inputStringScrollBarAdjuster(_Context, widget, scrollArea, inputStringRenderingData);
                            }

                            // move cursor right
                            else if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ||
                                    _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow))
                            {
                                if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow))
                                {
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_right(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                }
                                else
                                {
                                    if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                                    {
                                        widget->CursorMovementTimer = Frenchie::Core::tic();
                                    }
                                    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                                    {
                                        widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_right(widget->Utf8LeftCursorPosition, _Text);
                                        widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                        widget->CursorMovementTimer     = std::chrono::steady_clock::time_point();
                                    }
                                }

                                inputStringScrollBarAdjuster(_Context, widget, scrollArea, inputStringRenderingData);
                            }

                            // stop editing on enter
                            else if( 
                                ((_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter) && _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Enter)) ||
                                ((_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_StopEditOnEscape) && _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Escape)))
                            {
                                while (parent != nullptr &&
                                        dynamic_cast<ImmediateUserInterfaceInputString*>(parent) == nullptr)
                                {
                                    parent->State.Selected = false;
                                    parent = _Context->m_Hierarchy.get_parent(parent);
                                }

                                widget->State.Selected = false;
                            }

                            // move cursor up
                            else if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow) ||
                                    _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow))
                            {
                                if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow))
                                {
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_up(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                }
                                else
                                {
                                    if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                                    {
                                        widget->CursorMovementTimer = Frenchie::Core::tic();
                                    }
                                    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                                    {
                                        widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_up(widget->Utf8LeftCursorPosition, _Text);
                                        widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                        widget->CursorMovementTimer     = std::chrono::steady_clock::time_point();
                                    }
                                }

                                inputStringScrollBarAdjuster(_Context, widget, scrollArea, inputStringRenderingData);
                            }

                            // move cursor down
                            else if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow) ||
                                    _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow))
                            {
                                if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow))
                                {
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_down(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                }
                                else
                                {
                                    if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                                    {
                                        widget->CursorMovementTimer = Frenchie::Core::tic();
                                    }
                                    else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                                    {
                                        widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_down(widget->Utf8LeftCursorPosition, _Text);
                                        widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                        widget->CursorMovementTimer     = std::chrono::steady_clock::time_point();
                                    }
                                }

                                inputStringScrollBarAdjuster(_Context, widget, scrollArea, inputStringRenderingData);
                            }

                            // set left cursor position
                            else if(_Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Escape)     ||
                                    _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow)    ||
                                    _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow)  ||
                                    _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ||
                                    _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow)  ||
                                    _Context->m_Input.is_mouse_button_pressed())
                            {
                                if(inputStringRenderingData.HoveredSymbolUtf8CursorPosition.has_value() && _Context->m_Input.is_mouse_button_pressed())
                                {
                                    widget->Utf8LeftCursorPosition  = inputStringRenderingData.HoveredSymbolUtf8CursorPosition.value();
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                }

                                widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                            }

                            // set right cursor position
                            else if(                        
                                !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoSelection) &&
                                inputStringRenderingData.HoveredSymbolUtf8CursorPosition.has_value()                                                  &&
                                _Context->m_Input.is_mouse_button_down())
                            {
                                if(inputStringRenderingData.HoveredSymbolUtf8CursorPosition.value() > widget->Utf8LeftCursorPosition)
                                    widget->Utf8RightCursorPosition = inputStringRenderingData.HoveredSymbolUtf8CursorPosition.value();
                                else
                                    widget->Utf8LeftCursorPosition = inputStringRenderingData.HoveredSymbolUtf8CursorPosition.value();
                            }

                            // select all
                            else if(
                                    !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoSelection) &&
                                    _Context->m_Input.has_modifier(ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_Ctrl)     &&
                                    _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_A))
                            {
                                widget->Utf8LeftCursorPosition  = 0;
                                widget->Utf8RightCursorPosition = (int)_Text.size();
                            }

                            // modifications

                            // insert text
                            else if(
                                !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoInput) &&
                                _Context->m_Input.has_input_text())
                            {
                                // remove selection
                                if(gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) > 0)
                                {
                                    int position = widget->Utf8LeftCursorPosition;
                                    int count    = gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) + 1;

                                    // move cursor
                                    for (int i = 0; i < count; i++)
                                        widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                                    // erase selection
                                    _Text.erase(position, count);
                                }

                                // insert text after selection
                                if(inputStringCharacterFilter(_Context->m_Input.get_input_text()))
                                {
                                    _Text.insert(widget->Utf8LeftCursorPosition, _Context->m_Input.get_input_text());
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_right(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                }

                                inputStringScrollBarAdjuster(_Context, widget, scrollArea, inputStringRenderingData);

                                _InputTextCallback(_Text);

                                edited = true;
                            }

                            // remove text
                            else if(
                                    !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoInput) &&

                                    _Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace) ||
                                    _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace))
                            {
                                // remove selection
                                if(gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) > 0)
                                {
                                    int position = widget->Utf8LeftCursorPosition;
                                    int count    = gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) + 1;

                                    // move cursor
                                    for (int i = 0; i < count; i++)
                                        widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                                    // erase selection
                                    _Text.erase(position, count);

                                    edited = true;
                                }
                                // remove single symbol
                                else
                                {
                                    if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace))
                                    {
                                        int previousCursorPosition      = widget->Utf8LeftCursorPosition;
                                        widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                        widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                                        if(previousCursorPosition - widget->Utf8LeftCursorPosition > 0)
                                            _Text.erase(widget->Utf8LeftCursorPosition, previousCursorPosition - widget->Utf8LeftCursorPosition);
                                    }
                                    else
                                    {
                                        if(widget->CursorMovementTimer.time_since_epoch().count() <= 0)
                                        {
                                            widget->CursorMovementTimer = Frenchie::Core::tic();
                                        }
                                        else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(widget->CursorMovementTimer, Frenchie::Core::tic()) > cursorMovementInterval)
                                        {
                                            int previousCursorPosition      = widget->Utf8LeftCursorPosition;
                                            widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                            widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                                            if(previousCursorPosition - widget->Utf8LeftCursorPosition > 0)
                                                _Text.erase(widget->Utf8LeftCursorPosition, previousCursorPosition - widget->Utf8LeftCursorPosition);

                                            widget->CursorMovementTimer = std::chrono::steady_clock::time_point();
                                        }
                                    }
                                }

                                inputStringScrollBarAdjuster(_Context, widget, scrollArea, inputStringRenderingData);

                                _InputTextCallback(_Text);
                            }

                            // copy text
                            if( 
                                !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoClipboard) &&
                                _Context->m_Input.has_modifier(ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_Ctrl)     &&
                                _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_C)) // Ctrl + C
                            {
                                if(gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition))
                                {
                                    _Context->m_Input.set_clipboard_text(
                                        std::string(
                                            _Text.begin() + widget->Utf8LeftCursorPosition,
                                            _Text.begin() + gs_clamp(widget->Utf8RightCursorPosition + 1, 0, (int)_Text.size())));
                                }
                            }

                            // paste text
                            if( 
                                !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoInput)     &&
                                !(_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoClipboard) &&
                                _Context->m_Input.has_modifier(ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_Ctrl)     &&
                                _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_V)                      &&
                                _Context->m_Input.has_clipboard_text()) // Ctrl + V
                            {
                                // remove selection
                                if(gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) > 0)
                                {
                                    int position = widget->Utf8LeftCursorPosition;
                                    int count    = gs_abs(widget->Utf8RightCursorPosition - widget->Utf8LeftCursorPosition) + 1;

                                    // move cursor
                                    for (int i = 0; i < count; i++)
                                        widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                                    // erase selection
                                    _Text.erase(position, count);
                                }

                                // insert text from clipboard
                                std::string clipboardText = _Context->m_Input.get_clipboard_text();

                                _Text.insert(widget->Utf8LeftCursorPosition, clipboardText);
                                
                                for(int i = 0; i < (int)clipboardText.size(); i++)
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_right(widget->Utf8LeftCursorPosition, _Text);
                                
                                widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                                inputStringScrollBarAdjuster(_Context, widget, scrollArea, inputStringRenderingData);

                                _InputTextCallback(_Text);

                                edited = true;
                            }
                        }
                    }

                    // calculate geometry
                    {
                        if((_InternalSettings & ImmediateUserInterfaceInputStringInternalSettings_::ImmediateUserInterfaceInputStringInternalSettings_NoMultiline))
                        {
                            widget->State.MinimumSize = gs_vec2f(
                                gs_max(inputStringRenderingData.TextBoundingBox.size().x, _Context->m_Style.get_font_size()) + _Context->m_Style.get_font_size(),
                                _Context->m_Style.get_font_size() + _Context->m_Style.get_frames_width());

                            widget->State.MaximumSize = gs_vec2f((float)INT_MAX, widget->State.MinimumSize.y);
                        }
                        else
                        {
                            widget->State.MinimumSize = gs_vec2f(
                                gs_max(inputStringRenderingData.TextBoundingBox.size().x, _Context->m_Style.get_font_size()) + _Context->m_Style.get_font_size(),
                                gs_max(inputStringRenderingData.TextBoundingBox.size().y, _Context->m_Style.get_font_size()) + _Context->m_Style.get_frames_width());

                            widget->State.MaximumSize = gs_vec2f((float)INT_MAX, (float)INT_MAX);
                        }
                        
                        widget->State.BoundingBox = gs_2dboxf(
                            widget->State.BoundingBox.Min,
                            widget->State.BoundingBox.Min + gs_clamp(widget->State.MinimumSize, widget->State.MinimumSize, widget->State.MaximumSize));
                    }

                    _Context->end_node<ImmediateUserInterfaceInputStringContent>();
                }

                _Context->end_node<ImmediateUserInterfaceInputString>();
            }

            if(widget != nullptr && widget->Cache.Selected && (_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEdit))
                return edited;

            if(_Context != nullptr        &&
                   widget   != nullptr    &&
                   widget->Cache.Selected &&
                   _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Enter) &&
                   (_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter))
            {                    
                return true;
            }

            return false;
        }

        template<typename Type>
        bool input_scalar_internal(
            ImmediateUserInterfaceContextLayer*              _Context,
            const std::string&                               _ID,
            Type&                                            _Input,
            const Type&                                      _Min,
            const Type&                                      _Max,
            const std::string&                               _Format,
            const ImmediateUserInterfaceInputScalarSettings& _Settings)
        {
            // nested types
            struct ImmediateUserInterfaceInputScalarPanel : public ImmediateUserInterfaceNodePanel
            {
            public:
                ImmediateUserInterfaceInputScalarPanel(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}

                virtual ~ImmediateUserInterfaceInputScalarPanel(){}

                virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
                {
                    IsEdited = is_edited(_Context, this);

                    ImmediateUserInterfaceNodePanel::layout(_Context);
                }

                bool is_edited(ImmediateUserInterfaceContextLayer* _Context, const ImmediateUserInterfaceNode* _Node)
                {
                    if(_Node != nullptr && _Node->State.Selected)
                        return true;

                    for (auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
                    {
                        if((*it)->State.Selected || is_edited(_Context, *it))
                            return true;
                    }

                    return false;
                }

                std::string Buffer;
                bool        IsEdited = false;
            };

            // main code
            bool modified = false;

            if(_Context->begin_node<ImmediateUserInterfaceInputScalarPanel>(
                _ID,
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                ImmediateUserInterfaceInputScalarPanel* panel = _Context->get_rendering_stack_top<ImmediateUserInterfaceInputScalarPanel>();

                modified = input_string_internal(
                    _Context,
                    std::string(_ID).append("/Input"),
                    panel->Buffer,

                    // input settings
                    ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Defaults
                    | ((_Settings & ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_StopEditOnEscape) ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_StopEditOnEscape   : 0)
                    | ((_Settings & ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter) ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter : 0)
                    | ((_Settings & ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEdit)  ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEdit  : 0),
                    
                    ImmediateUserInterfaceInputStringInternalSettings_::ImmediateUserInterfaceInputStringInternalSettings_NoMultiline,

                    // node settings
                      ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar,
                    [](const std::string&)->bool
                    {
                        return true;
                    });

                // auxiliary lambdas
                auto writeValueToBuffer = [](ImmediateUserInterfaceInputScalarPanel* _Panel, const Type& _Input, const std::string& _Format)
                {
                    if(_Panel == nullptr) return;

                    std::string  currentValue = Frenchie::Core::String::format(_Format, _Input);
                    const size_t maximumSize  = 16;

                    if(currentValue.size() < maximumSize)
                        _Panel->Buffer = currentValue;
                    else
                        _Panel->Buffer = std::string(currentValue.c_str(), maximumSize);
                };

                if(modified)
                {
                    _Input = Frenchie::Core::String::from_string<Type>(panel->Buffer);
                    writeValueToBuffer(panel, gs_clamp(_Input, _Min, _Max), _Format);
                }

                if(!panel->IsEdited)
                    writeValueToBuffer(panel, _Input, _Format);

                // calculate geometry
                {
                    panel->State.MinimumSize = gs_vec2f(
                        panel->State.MinimumSize.x,
                        _Context->m_Renderer->calculate_bounding_box(
                            panel->Buffer.begin(),
                            panel->Buffer.end(),
                            _Context->m_Style.get_font_size(),
                            _Context->m_Style.get_current_font()).size().y);
                    
                    panel->State.MaximumSize = gs_vec2f(
                        panel->State.MaximumSize.x,
                        gs_max(panel->State.MinimumSize.y, _Context->m_Style.get_font_size()));

                    panel->State.BoundingBox = gs_2dboxf(
                        panel->State.BoundingBox.Min,
                        panel->State.BoundingBox.Min + gs_clamp(panel->State.BoundingBox.size(), panel->State.MinimumSize, panel->State.MaximumSize));
                }

                _Context->end_node<ImmediateUserInterfaceInputScalarPanel>();
            }

            return modified;
        }
    
        template<typename Type>
        bool input_scalar_slider_internal(
            ImmediateUserInterfaceContextLayer*              _Context,
            const std::string&                               _ID,
            Type&                                            _Input,
            const Type&                                      _Min,
            const Type&                                      _Max,
            const int&                                       _Delta,
            const ImmediateUserInterfaceInputScalarSettings& _Settings)
        {
            // nested types
            struct ImmediateUserInterfaceInputScalarSlider : public ImmediateUserInterfaceNode
            {
            public:
                ImmediateUserInterfaceInputScalarSlider(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
                {
                    State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 64.f));
                    State.MinimumSize = gs_vec2f(0.f, 64.f);
                    State.MaximumSize = gs_vec2f(gs_huge<float>(), 64.f);
                }

                virtual ~ImmediateUserInterfaceInputScalarSlider(){}

                virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
                {
                    // render slider box
                    {
                        _Context->m_Renderer->push_rectangle_rounded_filled(
                            State.BoundingBox.Min,
                            State.BoundingBox.Max,
                            _Context->m_Style.get_frames_radius(),
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                        _Context->m_Renderer->push_rectangle_rounded_filled(
                            State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
                            State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
                            _Context->m_Style.get_frames_radius(),
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                    }

                    // render slider
                    {
                        // palette slider
                        gs_2dboxf paletteSlider = gs_2dboxf(
                            State.BoundingBox.Min + gs_vec2f(SliderPosition, 0.f) * State.BoundingBox.size() * 0.9f,
                            State.BoundingBox.Min + gs_vec2f(SliderPosition, 0.f) * State.BoundingBox.size() * 0.9f + gs_vec2f(State.BoundingBox.width() * 0.1f, State.BoundingBox.height()));

                        _Context->m_Renderer->push_rectangle_rounded_filled(
                            paletteSlider.Min,
                            paletteSlider.Max,
                            _Context->m_Style.get_frames_radius(),
                            gs_color_rgba(0, 0, 0, 255),
                            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                        _Context->m_Renderer->push_rectangle_rounded_filled(
                            paletteSlider.Min + gs_vec2f(4.f),
                            paletteSlider.Max - gs_vec2f(4.f),
                            _Context->m_Style.get_frames_radius(),
                            gs_color_rgba(255, 255, 255, 255),
                            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                    }
                }

                virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override
                {
                    if(_Context == nullptr || _Context->m_Renderer == nullptr)
                        return false;

                    if(!_Context->m_Input.is_mouse_button_down())
                    {
                        SliderIsMoving = false;
                        Edited         = false;
                        return false;
                    }

                    // catch vertical color palette event
                    if((State.BoundingBox.contains(_Context->m_Input.get_cusor_position()) &&_Context->m_Input.is_mouse_button_pressed()) || SliderIsMoving)
                    {
                        if(_Context->m_Input.is_mouse_button_pressed())
                        {
                            SliderPosition = gs_clamp(
                                ceil(((_Context->m_Input.get_cusor_position() - State.BoundingBox.Min - State.BoundingBox.size() * 0.1f * 0.5f) / State.BoundingBox.size() / 0.9f).x * 100.f / (float)Delta),
                                1.f / (float)Delta,
                                100.f) * (float)Delta / 100.f;
                            
                            SliderPreviousPosition = SliderPosition;
                        }

                        SliderPosition = gs_clamp(
                            SliderPreviousPosition + gs_clamp(
                                ceil( (_Context->m_Input.get_cusor_drag_delta() / State.BoundingBox.size() / 0.9f).x * 100.f / (float)Delta),
                                -100.f,
                                +100.f) * (float)Delta / 100.f,
                            0.f,
                            1.f);
                        
                        SliderIsMoving = true;
                        Edited         = true;

                        return true;
                    }
                    
                    return false;
                }
                
                float SliderPosition         {0.f};
                float SliderPreviousPosition {0.f};
                int   Delta                  {1};
                bool  SliderIsMoving         {false};
                bool  Edited                 {false};
            };

            if(_Context->begin_node<ImmediateUserInterfaceInputScalarSlider>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                ImmediateUserInterfaceInputScalarSlider* slider =
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceInputScalarSlider>();

                slider->Delta = _Delta;

                if(slider->Edited)
                {
                    _Input = (Type)((float)_Min + (float)(_Max - _Min) * slider->SliderPosition);
                }
                else
                {
                    slider->SliderPosition = (float)_Input / (float)(_Max - _Min);
                }

                _Context->end_node<ImmediateUserInterfaceInputScalarSlider>();
            }

            return true;
        }

        // helpers
        namespace ImmediateUserInterfaceContextLayerHelpers
        {
            class ImmedidateUserInterfaceMovedNodeSearcher
            {
            public:
                template<typename FrameProcessor>
                ImmediateUserInterfaceNode* search(ImmediateUserInterfaceContextLayer* _Context, const FrameProcessor& _Filter)
                {
                    if(_Context == nullptr)
                        return nullptr;

                    for(auto singleton : _Context->m_Hierarchy.Singletons)
                    {
                        if(!_Filter(singleton))
                            continue;

                        ImmediateUserInterfaceNode* moved = search_recursive(_Context, singleton, _Filter);

                        if(moved != nullptr)
                            return moved;
                    }

                    return nullptr;
                };

            private:

                template<typename FrameProcessor>
                ImmediateUserInterfaceNode* search_recursive(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Moved, const FrameProcessor& _Filter)
                {
                    if(_Context == nullptr || _Moved == nullptr)
                        return nullptr;

                    // check self
                    if((_Moved->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved) && _Filter(_Moved))
                        return _Moved;

                    // check children
                    for(auto it = _Context->m_Hierarchy.begin(_Moved); it != _Context->m_Hierarchy.end(_Moved); it++)
                    {
                        ImmediateUserInterfaceNode* moved = search_recursive(_Context, *it, _Filter);

                        if(moved != nullptr)
                            return moved;
                    }

                    return nullptr;
                }
            };

            class ImmedidateUserInterfaceHoveredNodeSearcher
            {
            public:

                template<typename FrameProcessor>
                ImmediateUserInterfaceNode* search(ImmediateUserInterfaceContextLayer* _Context, const FrameProcessor& _Filter)
                {
                    // find top most hovered singleton window or a snapped window not equal to the moved one
                    ImmediateUserInterfaceNode* hovered  = nullptr;

                    for(auto singleton : _Context->m_Hierarchy.Singletons)
                        search_recursive(_Context, singleton, &hovered, _Filter);

                    return hovered;
                };

            private:

                template<typename FrameProcessor>
                void search_recursive(
                    ImmediateUserInterfaceContextLayer* _Context,
                    ImmediateUserInterfaceNode*         _Next,
                    ImmediateUserInterfaceNode**        _Hovered,
                    const FrameProcessor&               _Filter)
                {
                    if(_Context == nullptr || _Next == nullptr || !_Next->State.BoundingBox.contains(_Context->m_Input.get_cusor_position()))
                        return;

                    // check self
                    if(_Filter(_Next))
                    {
                        if(*_Hovered == nullptr || _Next->Cache.Depth > (*_Hovered)->Cache.Depth)
                            *_Hovered = _Next;
                    }

                    // check children
                    for(auto it = _Context->m_Hierarchy.begin(_Next); it != _Context->m_Hierarchy.end(_Next); it++)
                        search_recursive(_Context, *it, _Hovered, _Filter);
                }
            };

            // helper functions
            int calculate_depth_over_node(const ImmediateUserInterfaceNode* _Node)
            {
                if(_Node == nullptr)
                    return 0;

                return gs_max(
                    _Node->Cache.MaximumChildDepth + _Node->Cache.MaximumChildThickness - _Node->Cache.Depth,
                    _Node->Cache.MaximumChildDepth + _Node->Cache.SelfThickness + 1,
                    _Node->Cache.Depth + _Node->Cache.SelfThickness + 1);
            }

            int calculate_layer_depth(ImmediateUserInterfaceContextLayer* _Context, int _Layer)
            {
                return (int)(_Layer * _Context->m_Renderer->get_far_plane() / (ImmedidateUserInterfaceRenderingLayer_::ImmedidateUserInterfaceRenderingLayer_End - ImmedidateUserInterfaceRenderingLayer_::ImmedidateUserInterfaceRenderingLayer_Begin));
            };

            // layouting
            template<typename Type, typename FrameProcessor>
            void layout_nodes_as_panel(
                const Type&           _Begin,
                const Type&           _End,
                const gs_vec2f&       _Position,
                const gs_vec2f&       _Size,
                const gs_vec4f&       _Padding,
                const gs_vec4f&       _Margin,
                const int             _Settings,
                const FrameProcessor& _Filter)
            {
                // extract padding
                float topPadding    = _Padding.x;
                float leftPadding   = _Padding.y;
                float rightPadding  = _Padding.z;
                float bottomPadding = _Padding.w;
                
                // extract margin
                float topMargin     = _Margin.x;
                float leftMargin    = _Margin.y;
                float rightMargin   = _Margin.z;
                float bottomMargin  = _Margin.w;

                // layout children
                gs_2dboxf marginBox  = gs_2dboxf(_Position + gs_vec2f(leftMargin, topMargin), _Position - gs_vec2f(rightMargin, bottomMargin) + _Size);
                gs_2dboxf paddingBox = gs_2dboxf(marginBox.Min + gs_vec2f(leftPadding, topPadding), marginBox.Max - gs_vec2f(rightPadding, bottomPadding));

                float x = marginBox.Min.x;
                float y = marginBox.Min.y;

                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft)
                    x = marginBox.Min.x;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter)
                    x = marginBox.center().x - paddingBox.size().x * 0.5f;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight)
                    x = marginBox.Max.x - paddingBox.size().x;
                
                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop)
                    y = marginBox.Min.y;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter)
                    y = marginBox.center().y - paddingBox.size().y * 0.5f;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom)
                    y = marginBox.Max.y - paddingBox.size().y;

                gs_vec2f position = gs_vec2f(x, y) + gs_vec2f(leftPadding - rightPadding, topPadding - bottomPadding);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) == nullptr || !_Filter(*it))
                        continue;
                    
                    (*it)->State.BoundingBox = gs_2dboxf(
                        position,
                        position + gs_clamp(paddingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));
                }
            }

            template<typename Type, typename FrameProcessor>
            void layout_nodes_as_vertical_stack(
                const Type&           _Begin,
                const Type&           _End,
                const gs_vec2f&       _Position,
                const gs_vec2f&       _Size,
                const gs_vec4f&       _Padding,
                const gs_vec4f&       _Margin,
                const int             _Settings,
                const FrameProcessor& _Filter)
            {
                // extract padding
                float topPadding    = _Padding.x;
                float leftPadding   = _Padding.y;
                float rightPadding  = _Padding.z;
                float bottomPadding = _Padding.w;
                
                // extract margin
                float topMargin     = _Margin.x;
                float leftMargin    = _Margin.y;
                float rightMargin   = _Margin.z;
                float bottomMargin  = _Margin.w;

                // compute total size
                gs_vec2f totalsize  = gs_vec2f(0.f, 0.f);
                int      childCount = 0;

                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) != nullptr && _Filter(*it))
                    {
                        totalsize += (*it)->State.BoundingBox.size();
                        childCount++;
                    }
                }

                totalsize += gs_vec2f(0.f, (float)(childCount - 1) * (topPadding + bottomPadding) * 0.5f);

                // layout children
                gs_2dboxf marginBox  = gs_2dboxf(_Position + gs_vec2f(leftMargin, topMargin), _Position - gs_vec2f(rightMargin, bottomMargin) + _Size);
                gs_2dboxf paddingBox = gs_2dboxf(marginBox.Min + gs_vec2f(leftPadding, topPadding), marginBox.Max - gs_vec2f(rightPadding, bottomPadding));
                gs_vec2f  scale      = paddingBox.size() / gs_vec2f(gs_max(totalsize.x, 1.f), gs_max(totalsize.y, 1.f));

                float x = marginBox.Min.x;
                float y = marginBox.Min.y;

                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft)
                    x = marginBox.Min.x;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter)
                    x = marginBox.center().x - paddingBox.size().x * 0.5f;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight)
                    x = marginBox.Max.x - paddingBox.size().x;
                
                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop)
                    y = marginBox.Min.y;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter)
                    y = marginBox.center().y - paddingBox.size().y * 0.5f;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom)
                    y = marginBox.Max.y - paddingBox.size().y;

                gs_vec2f position = gs_vec2f(x, y) + gs_vec2f(leftPadding - rightPadding, topPadding - bottomPadding);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if(*it == nullptr || !_Filter(*it))
                        continue;
                    
                    gs_vec2f size = gs_vec2f(
                        paddingBox.width(),
                        gs_clamp(((*it)->State.BoundingBox.size() * scale).y, (*it)->State.MinimumSize.y, (*it)->State.MaximumSize.y));

                    (*it)->State.BoundingBox = gs_2dboxf(position, position + size);

                    position += gs_vec2f(0.f, size.y + (topPadding + bottomPadding) * 0.5f);
                }
            }

            template<typename Type, typename FrameProcessor>
            void layout_nodes_as_horizontal_stack(
                const Type&           _Begin,
                const Type&           _End,
                const gs_vec2f&       _Position,
                const gs_vec2f&       _Size,
                const gs_vec4f&       _Padding,
                const gs_vec4f&       _Margin,
                const int             _Settings,
                const FrameProcessor& _Filter)
            {
                // extract padding
                float topPadding    = _Padding.x;
                float leftPadding   = _Padding.y;
                float rightPadding  = _Padding.z;
                float bottomPadding = _Padding.w;
                
                // extract margin
                float topMargin     = _Margin.x;
                float leftMargin    = _Margin.y;
                float rightMargin   = _Margin.z;
                float bottomMargin  = _Margin.w;

                // compute total size
                gs_vec2f totalsize  = gs_vec2f(0.f, 0.f);
                int      childCount = 0;

                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) != nullptr && _Filter(*it))
                    {
                        totalsize += (*it)->State.BoundingBox.size();
                        childCount++;
                    }
                }

                totalsize += gs_vec2f((float)(childCount - 1) * (leftPadding + rightPadding) * 0.5f, 0.f);

                // layout children
                gs_2dboxf marginBox  = gs_2dboxf(_Position + gs_vec2f(leftMargin, topMargin), _Position - gs_vec2f(rightMargin, bottomMargin) + _Size);
                gs_2dboxf paddingBox = gs_2dboxf(marginBox.Min + gs_vec2f(leftPadding, topPadding), marginBox.Max - gs_vec2f(rightPadding, bottomPadding));
                gs_vec2f  scale      = paddingBox.size() / gs_vec2f(gs_max(totalsize.x, 1.f), gs_max(totalsize.y, 1.f));

                float x = marginBox.Min.x;
                float y = marginBox.Min.y;

                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft)
                    x = marginBox.Min.x;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter)
                    x = marginBox.center().x - paddingBox.size().x * 0.5f;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight)
                    x = marginBox.Max.x - paddingBox.size().x;
                
                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop)
                    y = marginBox.Min.y;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter)
                    y = marginBox.center().y - paddingBox.size().y * 0.5f;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom)
                    y = marginBox.Max.y - paddingBox.size().y;

                gs_vec2f position = gs_vec2f(x, y) + gs_vec2f(leftPadding - rightPadding, topPadding - bottomPadding);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if(*it == nullptr || !_Filter(*it))
                        continue;
                    
                    gs_vec2f size = gs_vec2f(
                        gs_clamp(((*it)->State.BoundingBox.size() * scale).x, (*it)->State.MinimumSize.x, (*it)->State.MaximumSize.x),
                        paddingBox.height());

                    (*it)->State.BoundingBox = gs_2dboxf(position, position + size);

                    position += gs_vec2f(size.x + (leftPadding + rightPadding) * 0.5f, 0.f);
                }
            }

            // gizmos
            gs_2d_ellipsef build_resize_top_left_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 32.f);
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Min, WindowResizeAngleGizmoRadius);
            }

            gs_2d_ellipsef build_resize_top_right_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Min + gs_vec2f(_Node->get_visible_rect(_Context).width(), 0.f), WindowResizeAngleGizmoRadius);
            }

            gs_2d_ellipsef build_resize_bottom_left_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 32.f);
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Max - gs_vec2f(_Node->get_visible_rect(_Context).width(), 0.f), WindowResizeAngleGizmoRadius);
            };

            gs_2d_ellipsef build_resize_bottom_right_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 32.f);
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Max, WindowResizeAngleGizmoRadius);
            };

            gs_2dboxf build_resize_top_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = _Context->m_Style.get_frames_width() * 2.f;

                return gs_2dboxf(
                    _Node->get_visible_rect(_Context).Min - gs_vec2f(0.f, WindowResizeSideGizmoWidth),
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(_Node->get_visible_rect(_Context).width(), WindowResizeSideGizmoWidth));
            };

            gs_2dboxf build_resize_left_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = _Context->m_Style.get_frames_width() * 2.f;

                return gs_2dboxf(
                    _Node->get_visible_rect(_Context).Min - gs_vec2f(WindowResizeSideGizmoWidth, 0.f),
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(WindowResizeSideGizmoWidth, _Node->get_visible_rect(_Context).height()));
            };

            gs_2dboxf build_resize_right_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = _Context->m_Style.get_frames_width() * 2.f;

                return gs_2dboxf(
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(_Node->get_visible_rect(_Context).width() - WindowResizeSideGizmoWidth, 0.f),
                    _Node->get_visible_rect(_Context).Max + gs_vec2f(WindowResizeSideGizmoWidth, 0.f));
            };

            gs_2dboxf build_resize_bottom_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = _Context->m_Style.get_frames_width() * 2.f;

                return gs_2dboxf(
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(0.f, _Node->get_visible_rect(_Context).height() - WindowResizeSideGizmoWidth),
                    _Node->get_visible_rect(_Context).Max + gs_vec2f(0.f, WindowResizeSideGizmoWidth));
            };
        
            void render_resize_gizmo(
                ImmediateUserInterfaceContextLayer*     _Context,
                ImmediateUserInterfaceNode*             _Node,
                const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
            {
                if(_Context == nullptr || _Node == nullptr) return;

                int depth = ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node(_Node);

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
                {
                    auto resizeTopLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Context, _Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeTopLeft.Center,
                        resizeTopLeft.Radius,
                        resizeTopLeft.Radius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
                {
                    auto resizeTopRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_right_ellipse(_Context,_Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeTopRight.Center,
                        resizeTopRight.Radius,
                        resizeTopRight.Radius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
                {
                    auto resizeBottomLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_left_ellipse(_Context,_Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeBottomLeft.Center,
                        resizeBottomLeft.Radius,
                        resizeBottomLeft.Radius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
                {
                    auto resizeBottomRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_right_ellipse(_Context,_Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeBottomRight.Center,
                        resizeBottomRight.Radius,
                        resizeBottomRight.Radius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
                {
                    auto resizeTop = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        resizeTop.Min,
                        resizeTop.Max,
                        16.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
                {
                    auto resizeLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_left_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        resizeLeft.Min,
                        resizeLeft.Max,
                        16.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
                {
                    auto resizeRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_right_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        resizeRight.Min,
                        resizeRight.Max,
                        16.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
                {
                    auto resizeBottom = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        resizeBottom.Min,
                        resizeBottom.Max,
                        16.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            };

            bool check_cursor_intersection_with_resize_gizmo(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
            {
                if(_Context == nullptr || _Node == nullptr) return false;

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Context, _Node).contains(_Context->m_Input.get_cusor_position());

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_top_right_ellipse(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_left_ellipse(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_right_ellipse(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_top_box(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_left_box(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_right_box(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
                    return ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_box(_Context, _Node).contains(_Context->m_Input.get_cusor_position());
                
                return false;
            };

            ImmediateUserInterfaceNode* find_resizable_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
            {
                if(_Context == nullptr || _Node == nullptr) return nullptr;

                // find resizable node
                ImmediateUserInterfaceNode* resizable = _Node;

                // pass event to a parent
                while (_Context->m_Hierarchy.get_parent(resizable) &&
                        check_cursor_intersection_with_resize_gizmo(_Context, _Context->m_Hierarchy.get_parent(resizable), _ResizeEventType))
                    resizable = _Context->m_Hierarchy.get_parent(resizable);

                return resizable;
            };

            void resize_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
            {
                if(_Node == nullptr)
                    return;

                gs_2dboxf box = _Node->State.BoundingBox;

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + _Context->m_Input.get_cusor_drag_delta(), _Node->Cache.BoundingBox.Max);
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y), _Node->Cache.BoundingBox.Max + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f));
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f), _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y));
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min, _Node->Cache.BoundingBox.Max + ApplicationPlatformBackend::get_window_cursor_dragdelta());
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y), _Node->Cache.BoundingBox.Max);
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f), _Node->Cache.BoundingBox.Max);
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min, _Node->Cache.BoundingBox.Max + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f));
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
                    box = gs_2dboxf(_Node->Cache.BoundingBox.Min, _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y));

                _Node->State.BoundingBox = gs_2dboxf(box.Min, box.Min + gs_clamp(box.size(), _Node->State.MinimumSize, _Node->State.MaximumSize));
            };
        }
    }
}

// ImmedidateUserInterfaceStyle
ImmedidateUserInterfaceStyle::ImmedidateUserInterfaceStyle()
{
    Colors.resize(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End);

    // general ui elements
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground]                 = gs_color_rgba(28, 28, 28, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered]          = gs_color_rgba(72, 82, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground]                  = gs_color_rgba(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackgroundHovered]           = gs_color_rgba(72, 82, 72, 255);
    
    // push button
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline]                    = gs_color_rgba(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground]                 = gs_color_rgba(32, 32, 32, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered]          = gs_color_rgba(60, 72, 60, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed]          = gs_color_rgba(120, 128, 120, 255);

    // menus
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuOutline]                      = gs_color_rgba(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground]             = gs_color_rgba(32, 32, 32, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered]      = gs_color_rgba(60, 72, 60, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed]      = gs_color_rgba(120, 128, 120, 255);

    // scrollbar
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground]        = gs_color_rgba(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered] = gs_color_rgba(72, 82, 72, 255);

    // // comboboxes
    // Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ComboboxOutline]                  = gs_color_rgba(72, 72, 72, 255);
    // Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ComboboxBackground]               = gs_color_rgba(32, 32, 32, 255);

    // gizmos
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos]                           = gs_color_rgba(50, 50, 100, 200);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered]                    = gs_color_rgba(100, 100, 172, 255);

    // gizmos
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text]                             = gs_color_rgba(255, 255, 255, 255);
}

ImmedidateUserInterfaceStyle::~ImmedidateUserInterfaceStyle(){}

float ImmedidateUserInterfaceStyle::get_minimum_frames_radius() const
{
    return 0.f;
}

float ImmedidateUserInterfaceStyle::get_maximum_frames_radius() const
{
    return 32.f;
}

float ImmedidateUserInterfaceStyle::get_frames_radius() const
{
    return gs_clamp(FramesRadius, get_minimum_frames_radius(), get_maximum_frames_radius());
}

float ImmedidateUserInterfaceStyle::get_minimum_frames_width() const
{
    return 4.f;
}

float ImmedidateUserInterfaceStyle::get_maximum_frames_width() const
{
    return 16.f;
}

float ImmedidateUserInterfaceStyle::get_frames_width() const
{
    return gs_clamp(FramesWidth, get_minimum_frames_width(), get_maximum_frames_width());
}

float ImmedidateUserInterfaceStyle::get_minimum_font_size() const
{
    return 32.f + get_minimum_frames_width();
}

float ImmedidateUserInterfaceStyle::get_maximum_font_size() const
{
    return 128.f;
}

float ImmedidateUserInterfaceStyle::get_font_size() const
{
    return gs_clamp(FontSize, get_minimum_font_size(), get_maximum_font_size());
}

float ImmedidateUserInterfaceStyle::get_minimum_scrollbar_width() const
{
    return 16.f;
}

float ImmedidateUserInterfaceStyle::get_maximum_scrollbar_width() const
{
    return get_maximum_frames_radius() * 2.f;
}

float ImmedidateUserInterfaceStyle::get_scrollbar_width() const
{
    return gs_clamp(ScrollBarWidth, get_minimum_scrollbar_width(), get_maximum_scrollbar_width());
}

float ImmedidateUserInterfaceStyle::get_popup_menu_pointer_size() const
{
    return gs_min(gs_max(PopupMenuPointerSize, 32.f), get_font_size() - 2.f * get_frames_width());
}

ApplicationRenderingBackendFont ImmedidateUserInterfaceStyle::get_current_font() const
{
    return Font.is_null() ? ApplicationRenderingBackend::get_default_font() : Font;
}

gs_color ImmedidateUserInterfaceStyle::get_color(const ImmediateUserInterfaceNodeColors_& _Color) const
{
    return Colors[_Color];
}

// ImmedidateUserInterfaceInput
ImmedidateUserInterfaceInput::ImmedidateUserInterfaceInput(ImmediateUserInterfaceContextLayer* _Context) : m_Context(_Context){}

gs_vec2f ImmedidateUserInterfaceInput::get_cusor_position() const
{
    return m_Context != nullptr && m_Context->m_Renderer != nullptr ?
            gs_vec2f(m_Context->m_Renderer->get_cursor_postion().x, m_Context->m_Renderer->get_cursor_postion().y) :
                gs_vec2f(0.f, 0.f);
}

gs_vec2f ImmedidateUserInterfaceInput::get_cusor_drag_delta() const
{
    return ApplicationPlatformBackend::get_window_cursor_dragdelta();
}

gs_vec2f ImmedidateUserInterfaceInput::get_cusor_scroll_offset() const
{
    return ApplicationPlatformBackend::get_mouse_scroll_offset();
}

std::string ImmedidateUserInterfaceInput::get_input_text() const
{
    return ApplicationPlatformBackend::input_text();
}

std::string ImmedidateUserInterfaceInput::get_clipboard_text() const
{
    return ApplicationPlatformBackend::get_clipboard_text();
}

bool ImmedidateUserInterfaceInput::has_input_text() const
{
    return ApplicationPlatformBackend::has_input_text();
}

bool ImmedidateUserInterfaceInput::has_clipboard_text() const
{
    return ApplicationPlatformBackend::has_clipboard_text();
}

void ImmedidateUserInterfaceInput::set_clipboard_text(const std::string& _Value)
{
    ApplicationPlatformBackend::set_clipboard_text(_Value);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_down() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_down((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_hold() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_hold((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }
    
    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_pressed() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_pressed((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_released() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_released((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_clicked() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_clicked((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_double_clicked() const
{
    // catch mouse buttons
    for (int button = ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonBegin;
             button < ApplicationPlatformBackendMouseButton::ApplicationPlatformBackendMouseButtonEnd;
             button++)
    {
        if(ApplicationPlatformBackend::is_mouse_button_double_clicked((ApplicationPlatformBackendMouseButton::Button)button))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_down(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_hold(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_pressed(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_released(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_clicked(_Button);
}

bool ImmedidateUserInterfaceInput::is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_double_clicked(_Button);
}

bool ImmedidateUserInterfaceInput::is_key_down() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_down((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_hold() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_hold((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_pressed() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_pressed((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_released() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_released((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_clicked() const
{
    for (int key = ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_BEGIN;
             key < ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NamedKey_END;
             key++)
    {
        if(ApplicationPlatformBackend::is_key_clicked((ApplicationPlatformBackendKey::Key)key))
            return true;
    }

    return false;
}

bool ImmedidateUserInterfaceInput::is_key_down(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_down(_Key);
}

bool ImmedidateUserInterfaceInput::is_key_hold(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_hold(_Key);
}

bool ImmedidateUserInterfaceInput::is_key_pressed(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_pressed(_Key);
}

bool ImmedidateUserInterfaceInput::is_key_released(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_released(_Key);
}

bool ImmedidateUserInterfaceInput::is_key_clicked(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_clicked(_Key);
}

bool ImmedidateUserInterfaceInput::has_modifier(const ApplicationPlatformBackendKeyModifier::Modifier& _Modifier) const
{
    return ApplicationPlatformBackend::has_modifier(_Modifier);
}

// ImmediateUserInterfaceContextConfiguration
ImmediateUserInterfaceContextConfiguration::ImmediateUserInterfaceContextConfiguration(){}
ImmediateUserInterfaceContextConfiguration::~ImmediateUserInterfaceContextConfiguration(){}

template<> std::string ImmediateUserInterfaceContextConfiguration::get<std::string>(const std::string& _Section, const std::string& _Name)
{
    return m_Configuration[_Section][_Name];
}

template<> int ImmediateUserInterfaceContextConfiguration::get<int>(const std::string& _Section, const std::string& _Name)
{
    return Frenchie::Core::String::from_string<int>(m_Configuration[_Section][_Name]);
}

template<> float ImmediateUserInterfaceContextConfiguration::get<float>(const std::string& _Section, const std::string& _Name)
{
    return Frenchie::Core::String::from_string<float>(m_Configuration[_Section][_Name]);
}

template<> double ImmediateUserInterfaceContextConfiguration::get<double>(const std::string& _Section, const std::string& _Name)
{
    return Frenchie::Core::String::from_string<double>(m_Configuration[_Section][_Name]);
}

template<> bool ImmediateUserInterfaceContextConfiguration::get<bool>(const std::string& _Section, const std::string& _Name)
{
    return Frenchie::Core::String::from_string<bool>(m_Configuration[_Section][_Name]);
}

template<> gs_vec2f ImmediateUserInterfaceContextConfiguration::get<gs_vec2f>(const std::string& _Section, const std::string& _Name)
{
    auto& _File = m_Configuration[_Section][_Name];

    if(_File.empty())
        return gs_vec2f(0.f, 0.f);

    gs_vec2f vector;

    char* begin = &_File[0];
    char* end   = begin;

    for (; *end != ',' && *end != '\0'; end++);
    vector.x = Frenchie::Core::String::from_string<float>(std::string(begin, end));

    if(*end == '\n')
        return vector;

    begin = ++end;
    for (;*end != '\0'; end++);

    vector.y = Frenchie::Core::String::from_string<float>(std::string(begin, end));

    return vector;
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<std::string>(const std::string& _Section, const std::string& _Name, const std::string& _Value)
{
    m_Configuration[_Section][_Name] = _Value;
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<int>(const std::string& _Section, const std::string& _Name, const int& _Value)
{
    m_Configuration[_Section][_Name] = Frenchie::Core::String::to_string<int>(_Value);
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<float>(const std::string& _Section, const std::string& _Name, const float& _Value)
{
    m_Configuration[_Section][_Name] = Frenchie::Core::String::to_string<float>(_Value);
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<double>(const std::string& _Section, const std::string& _Name, const double& _Value)
{
    m_Configuration[_Section][_Name] = Frenchie::Core::String::to_string<double>(_Value);
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<bool>(const std::string& _Section, const std::string& _Name, const bool& _Value)
{
    m_Configuration[_Section][_Name] = Frenchie::Core::String::to_string<bool>(_Value);
}

template<>
void ImmediateUserInterfaceContextConfiguration::set<gs_vec2f>(const std::string& _Section, const std::string& _Name, const gs_vec2f& _Value)
{
    m_Configuration[_Section][_Name] =
        std::string(Frenchie::Core::String::to_string<double>(_Value.x))
        .append(",")
        .append(Frenchie::Core::String::to_string<double>(_Value.y));
}

bool ImmediateUserInterfaceContextConfiguration::contains(const std::string& _Section, const std::string& _Name) const
{
    auto iterator = m_Configuration.find(_Section);
    return iterator != m_Configuration.end() && iterator->second.find(_Name) != iterator->second.end();
}

bool ImmediateUserInterfaceContextConfiguration::empty() const
{
    return m_Configuration.empty();
}

void ImmediateUserInterfaceContextConfiguration::clear()
{
    m_Configuration.clear();
}

bool ImmediateUserInterfaceContextConfiguration::read(const std::u32string& _Path)
{
    // open file
    FILE* file = std::fopen(
        Frenchie::Core::String::convert_utf32_to_utf8(_Path).c_str(),
        Frenchie::Core::String::convert_utf32_to_utf8(U"rb").c_str());
    
    if(file == nullptr)
        return false;

    // determine file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file); // Go back to the beginning

    if (file_size == -1)
    {
        fclose(file);
        return false;
    }

    // allocate memory for the content (+1 for null terminator)
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer)
    {
        fclose(file);
        return false;
    }

    // Read the entire file into string buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size)
    {
        free(buffer);
        fclose(file);
        return false;
    }

    // add the null terminator to make it a valid C string
    buffer[file_size] = '\0';

    // read file contents

    // auxiliary lambda predicates
    auto isEndOfFile    = [](const char* _Contents)->bool{return *_Contents ==  '\0';};
    auto isSectionStart = [](const char* _Contents)->bool{return *_Contents ==  '['; };
    auto isSectionEnd   = [](const char* _Contents)->bool{return *_Contents ==  ']'; };
    auto isValueStart   = [](const char* _Contents)->bool{return *_Contents ==  '='; };
    auto isKeyStart     = [](const char* _Contents)->bool{return *_Contents == '\n'; };

    char* fileContents = buffer;

    std::string currentSection;
    std::string currentSectionKey;

    for (;!isEndOfFile(fileContents); fileContents++)
    {
        // read section name
        if(isSectionStart(fileContents) && !isEndOfFile(++fileContents))
        {
            char* sectionNameBegin = fileContents;
            char* sectionNameEnd   = fileContents;

            for (;!isEndOfFile(sectionNameEnd); sectionNameEnd++)
            {
                if (isSectionEnd(sectionNameEnd))
                {
                    for (;isSectionEnd(sectionNameEnd) && !isEndOfFile(sectionNameEnd); sectionNameEnd++);
                    break;
                }
            }

            currentSection = std::string(sectionNameBegin, (sectionNameBegin != sectionNameEnd - 1 ? sectionNameEnd - 1 : sectionNameEnd));
            fileContents   = --sectionNameEnd;
        }

        // read section contents
        if(isSectionEnd(fileContents) && !currentSection.empty())
        {
            char* sectionContentsBegin = fileContents;
            char* sectionContentsEnd   = sectionContentsBegin;
            for (;!isKeyStart(sectionContentsBegin) && !isEndOfFile(sectionContentsBegin); sectionContentsBegin++);
            for (;!isSectionStart(sectionContentsEnd) && !isEndOfFile(sectionContentsEnd); sectionContentsEnd++);

            for(auto it = sectionContentsBegin; it != sectionContentsEnd && !isEndOfFile(it); it++)
            {
                if(isKeyStart(it))
                {
                    char* keyBegin = ++it;
                    char* keyEnd   = keyBegin;
                    for (;!isValueStart(keyEnd) && keyEnd != sectionContentsEnd && !isEndOfFile(keyEnd); keyEnd++);
                    currentSectionKey = std::string(keyBegin, keyEnd);
                    it = --keyEnd;
                }

                if(isValueStart(it) && !currentSectionKey.empty())
                {
                    char* valueBegin = ++it;
                    char* valueEnd   = valueBegin;
                    for (;!isKeyStart(valueEnd) && valueEnd != sectionContentsEnd && !isEndOfFile(valueEnd); valueEnd++);
                    m_Configuration[currentSection][currentSectionKey] = std::string(valueBegin, valueEnd);
                    it = --valueEnd;
                }
            }

            fileContents = --sectionContentsEnd;
        }
    }
    
    // close the file
    fclose(file);

    // free file contents buffer
    free(buffer);

    return true;
}

bool ImmediateUserInterfaceContextConfiguration::write(const std::u32string& _Path)
{
    // open file
    FILE* file = std::fopen(
        Frenchie::Core::String::convert_utf32_to_utf8(_Path).c_str(),
        Frenchie::Core::String::convert_utf32_to_utf8(U"wb").c_str());

    if(file == nullptr)
        return false;

    for(auto& settings : m_Configuration)
    {
        fprintf(file, "[%s]\n", settings.first.c_str());

        for(auto& setting : settings.second)
        {
            if(!setting.first.empty() && !setting.second.empty())
                fprintf(file, "%s=%s\n", setting.first.c_str(), setting.second.c_str());
        }
    }

    fclose(file);

    return true;
}

// ImmedidateUserInterfaceNode
ImmediateUserInterfaceNode::ImmediateUserInterfaceNode(const std::string& _Hash) : Hash(_Hash){}
ImmediateUserInterfaceNode::~ImmediateUserInterfaceNode(){}

void ImmediateUserInterfaceNode::render(ImmediateUserInterfaceContextLayer*)
{
}

void ImmediateUserInterfaceNode::layout(ImmediateUserInterfaceContextLayer*){}

void ImmediateUserInterfaceNode::measure(ImmediateUserInterfaceContextLayer* _Context)
{
    gs_2dboxf box = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min);

    for (auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        box = gs_2dboxf(
            box.Min,
            (*it)->State.BoundingBox.Min,
            box.Max,
            (*it)->State.BoundingBox.Max);
    }

    State.ContentSize = box.size();
}

bool ImmediateUserInterfaceNode::events(ImmediateUserInterfaceContextLayer* _Context)
{
    // resize
    if(((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable)) &&
        !(State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
    {
        if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);
                    return true;
                }
            }
        }
        else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
        {
            ImmediateUserInterfaceNode* resizable =
                ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);

            ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);

            // trigger event
            if(resizable != nullptr)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom;
                    return true;
                }

                // execute event
                if(_Context->m_Input.is_mouse_button_down() &&
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
                {
                    ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);
                    return true;
                }
            }
        }
    }

    // move
    if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable) &&
        !((State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)           ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)        ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)       ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)      ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)  ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)     ||
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)))
    {
        ImmediateUserInterfaceNode* movable = this;
        ImmediateUserInterfaceNode* parent  = _Context->m_Hierarchy.get_parent(movable);

        while (parent)
        {
            if(parent->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable)
                movable = parent;
            else
                break;
            parent = _Context->m_Hierarchy.get_parent(parent);
        }

        if(_Context->m_Input.is_mouse_button_pressed())
        {
            movable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;
            return true;
        }

        if(_Context->m_Input.is_mouse_button_down() &&
            (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
        {            
            movable->State.BoundingBox = gs_2dboxf(
                movable->Cache.BoundingBox.Min + ApplicationPlatformBackend::get_window_cursor_dragdelta(),
                movable->Cache.BoundingBox.Max + ApplicationPlatformBackend::get_window_cursor_dragdelta());
            return true;
        }
    }

    return false;
}

void ImmediateUserInterfaceNode::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child != nullptr)
        _Child->State.Parent = this;
}

bool ImmediateUserInterfaceNode::create_contents(ImmediateUserInterfaceContextLayer*, const std::string&, const ImmediateUserInterfaceNodeSettings&, bool*)
{
    return true;
}

void ImmediateUserInterfaceNode::load_state(ImmediateUserInterfaceContextLayer*){}
void ImmediateUserInterfaceNode::save_state(ImmediateUserInterfaceContextLayer*){}

gs_2dboxf ImmediateUserInterfaceNode::get_clipping_box(ImmediateUserInterfaceContextLayer* _Context) const
{
    // auxiliary lambdas
    auto calculate_clipping_box = [](ImmediateUserInterfaceContextLayer* _Context, const ImmediateUserInterfaceNode* _Node)->gs_2dboxf
    {
        if(_Node == nullptr)
        {
            return _Context != nullptr ?
                        _Context->m_Renderer->current_viewport() :
                            gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f((float)INT_MAX, (float)INT_MAX));
        }

        const ImmediateUserInterfaceNode* next   = _Node;
        ImmediateUserInterfaceNode*       parent = _Context->m_Hierarchy.get_parent(_Node);

        gs_2dboxf clippingBox = next->State.BoundingBox;

        while (parent)
        {
            clippingBox = gs_2dboxf(
                gs_vec2f(gs_max(next->State.BoundingBox.Min.x, clippingBox.Min.x), gs_max(next->State.BoundingBox.Min.y, clippingBox.Min.y)),
                gs_vec2f(gs_min(next->State.BoundingBox.Max.x, clippingBox.Max.x), gs_min(next->State.BoundingBox.Max.y, clippingBox.Max.y)));

            next   = parent;
            parent = _Context->m_Hierarchy.get_parent(parent);
        }

        return clippingBox;
    };

    // main code
    if(!State.ClippingBox.has_value())
        State.ClippingBox = calculate_clipping_box(_Context, this);

    return State.ClippingBox.value();
}

gs_2dboxf ImmediateUserInterfaceNode::get_visible_rect(ImmediateUserInterfaceContextLayer* _Context) const
{
    return State.BoundingBox.clip_with(get_clipping_box(_Context));
}

bool ImmediateUserInterfaceNode::is_partially_visible(ImmediateUserInterfaceContextLayer* _Context) const
{
    return State.BoundingBox.overlaps(get_clipping_box(_Context));
}

int ImmediateUserInterfaceNode::place_in_follow()
{
    return State.Depth + (++State.SelfThickness);
}

// ImmediateUserInterfaceNodePanel
ImmediateUserInterfaceNodePanel::ImmediateUserInterfaceNodePanel(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceNodePanel::~ImmediateUserInterfaceNodePanel(){}

void ImmediateUserInterfaceNodePanel::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min,
        State.BoundingBox.size(),
        ContentPadding,
        ContentMargin,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

void ImmediateUserInterfaceNodePanel::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // outline
    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
}

bool ImmediateUserInterfaceNodePanel::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return false;

    if(_Context->m_Hierarchy.size(this) > 0)
        return ImmediateUserInterfaceNode::events(_Context);

    ImmediateUserInterfaceNode* parent = _Context->m_Hierarchy.get_parent(this);
    while (parent)
    {
        if(parent->events(_Context))
            return true;
        parent = _Context->m_Hierarchy.get_parent(parent);
    }
    
    return false;
}

// ImmediateUserInterfaceNodeVerticalStack
ImmediateUserInterfaceNodeVerticalStack::ImmediateUserInterfaceNodeVerticalStack(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceNodeVerticalStack::~ImmediateUserInterfaceNodeVerticalStack(){}

void ImmediateUserInterfaceNodeVerticalStack::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_vertical_stack(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min,
        State.BoundingBox.size(),
        ContentPadding,
        ContentMargin,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

// ImmediateUserInterfaceNodeHorizontalStack
ImmediateUserInterfaceNodeHorizontalStack::ImmediateUserInterfaceNodeHorizontalStack(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceNodeHorizontalStack::~ImmediateUserInterfaceNodeHorizontalStack(){}

void ImmediateUserInterfaceNodeHorizontalStack::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_horizontal_stack(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min,
        State.BoundingBox.size(),
        ContentPadding,
        ContentMargin,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

// ImmediateUserInterfaceScrollArea
ImmediateUserInterfaceScrollArea::ImmediateUserInterfaceScrollArea(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceScrollArea::~ImmediateUserInterfaceScrollArea(){}

void ImmediateUserInterfaceScrollArea::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    // resize to contents
    State.MinimumSize = gs_vec2f(
        (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally) ?
            ContentView->State.ContentSize.x :
                State.MinimumSize.x,
        (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically) ?
            ContentView->State.ContentSize.y :
                State.MinimumSize.y);
    
    State.MaximumSize =
     gs_vec2f(
        (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally) ? State.MinimumSize.x : State.MaximumSize.x,
        (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically) ? State.MinimumSize.y : State.MaximumSize.y);
    
    State.BoundingBox = gs_2dboxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + gs_clamp(State.BoundingBox.size(), State.MinimumSize, State.MaximumSize));

    // default layout
    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min,
        State.BoundingBox.size(),
        ContentPadding,
        ContentMargin,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

void ImmediateUserInterfaceScrollArea::render(ImmediateUserInterfaceContextLayer*){}

void ImmediateUserInterfaceScrollArea::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child == nullptr) return;

    if(dynamic_cast<ImmediateUserInterfaceScrollAreaPanel*>(_Child))
    {
        _Child->State.Parent = this;
        return;
    }

    if(ContentView)
        ContentView->attach_child(_Child);
}

bool ImmediateUserInterfaceScrollArea::create_contents(
    ImmediateUserInterfaceContextLayer*       _Context, 
    const std::string&                        _ID,
    const ImmediateUserInterfaceNodeSettings& _Settings,
    bool*                                     _Render)
{
    if(_Context == nullptr) return false;

    ImmediateUserInterfaceNodeSettings settings   = _Settings & ~ImmediateUserInterfaceNodeSettings_NullParent;
    ImmediateUserInterfaceScrollArea*  scrollArea = this;

    if(_Context->begin_node<ImmediateUserInterfaceScrollAreaPanel>(
        std::string(_ID).append("/Panel"),
        settings))
    {
        scrollArea->ContentPanel = _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaPanel>();

        if(_Context->begin_vertical_stack(
            std::string(_ID).append("/Panel/VerticalStack"),
            settings))
        {
            _Context->get_rendering_stack_top<ImmediateUserInterfaceNode>()->State.PlaceInFollow = true;

            // vertical scrollbar area
            if(_Context->begin_horizontal_stack(
                std::string(_ID).append("/Panel/VerticalStack/VerticalScrollBarArea"),
                settings))
            {
                _Context->get_rendering_stack_top<ImmediateUserInterfaceNodeHorizontalStack>()->State.PlaceInFollow = true;

                // contents
                if(_Context->begin_node<ImmediateUserInterfaceScrollAreaContent>(
                    std::string(_ID).append("/Panel/VerticalStack/VerticalScrollBarArea/Contents"),
                    settings))
                {
                    scrollArea->ContentView = _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaContent>();
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaContent>()->ContentPadding      = scrollArea->ContentPadding;
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaContent>()->State.PlaceInFollow = scrollArea->State.PlaceInFollow;
                    
                    // reset self content padding as it's applied to content view
                    scrollArea->ContentPadding = gs_vec2f(0.f, 0.f);
                    
                    _Context->end_node<ImmediateUserInterfaceScrollAreaContent>();
                }

                // vertical scrollbar
                if(_Context->begin_node<ImmediateUserInterfaceScrollAreaScrollBar>(
                    std::string(_ID).append("/Panel/VerticalStack/VerticalScrollBarArea/VerticalScrollBar"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaScrollBar>()->Type =
                        ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical;

                    scrollArea->VerticalScrollBar = _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaScrollBar>();

                    _Context->end_node<ImmediateUserInterfaceScrollAreaScrollBar>();
                }

                _Context->end_horizontal_stack();
            }

            // horizontal scrollbar
            if(_Context->begin_node<ImmediateUserInterfaceScrollAreaScrollBar>(
                std::string(_ID).append("/Panel/VerticalStack/HorizontalScrollBar"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaScrollBar>()->Type =
                    ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Horizontal;

                scrollArea->HorizontalScrollBar = _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollAreaScrollBar>();

                _Context->end_node<ImmediateUserInterfaceScrollAreaScrollBar>();
            }

            _Context->end_vertical_stack();
        }
    
        _Context->end_node<ImmediateUserInterfaceScrollAreaPanel>();
    }

    return true;
}

float ImmediateUserInterfaceScrollArea::get_horizontal_scrollbar_width(ImmediateUserInterfaceContextLayer* _Context) const
{
    float horizontalScrollBarSize =
        HorizontalScrollBar != nullptr ?
            gs_min(HorizontalScrollBar->State.BoundingBox.size().x, HorizontalScrollBar->State.BoundingBox.size().y) :
                0.f;

    return horizontalScrollBarSize;
}

float ImmediateUserInterfaceScrollArea::get_vertical_scrollbar_width(ImmediateUserInterfaceContextLayer* _Context) const
{
    float verticalScrollBarSize =
        VerticalScrollBar != nullptr ?
            gs_min(VerticalScrollBar->State.BoundingBox.size().x, VerticalScrollBar->State.BoundingBox.size().y) :
                0.f;
    
    return verticalScrollBarSize;
}

void ImmediateUserInterfaceScrollArea::calculate_content_padding(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, gs_vec2f& _Padding)
{
    if(_Context == nullptr || _Node == nullptr) return;

    if(dynamic_cast<ImmediateUserInterfaceNodePanel*>(_Node) != nullptr)
        _Padding += dynamic_cast<ImmediateUserInterfaceNodePanel*>(_Node)->ContentPadding;

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
        calculate_content_padding(_Context, *it, _Padding);
}

ImmediateUserInterfaceScrollAreaPanel::ImmediateUserInterfaceScrollAreaPanel(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceScrollAreaPanel::~ImmediateUserInterfaceScrollAreaPanel(){}

// ImmediateUserInterfaceScrollAreaContent
ImmediateUserInterfaceScrollAreaContent::ImmediateUserInterfaceScrollAreaContent(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceScrollAreaContent::~ImmediateUserInterfaceScrollAreaContent(){}

void ImmediateUserInterfaceScrollAreaContent::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // extract scroll area
    ImmediateUserInterfaceScrollArea* scrollArea = nullptr;
    ImmediateUserInterfaceNode*       parent     = _Context->m_Hierarchy.get_parent(this);

    while (parent)
    {
        scrollArea = dynamic_cast<ImmediateUserInterfaceScrollArea*>(parent);
        if(scrollArea != nullptr) break;
        parent = _Context->m_Hierarchy.get_parent(parent);
    }
    
    // extract horizontal and vertical scrollbars
    gs_vec2f horizontalScrollBarPosition =
        scrollArea != nullptr && scrollArea->HorizontalScrollBar != nullptr ?
            scrollArea->HorizontalScrollBar->get_scroll_offset() :
                gs_vec2f(0.f, 0.f);

    gs_vec2f verticalScrollBarPosition =
        scrollArea != nullptr ?
            scrollArea->VerticalScrollBar->get_scroll_offset() :
                gs_vec2f(0.f, 0.f);

    float indent =
        scrollArea != nullptr && scrollArea->ContentPanel != nullptr ?
            scrollArea->ContentPanel->State.Indent :
                0.f; 

    // layout

    // extract padding
    float topPadding    = ContentPadding.x;
    float leftPadding   = ContentPadding.y;
    float rightPadding  = ContentPadding.z;
    float bottomPadding = ContentPadding.w;
    
    // extract margin
    float topMargin     = ContentMargin.x;
    float leftMargin    = ContentMargin.y;
    float rightMargin   = ContentMargin.z;
    float bottomMargin  = ContentMargin.w;

    gs_2dboxf marginBox = gs_2dboxf(State.BoundingBox.Min + gs_vec2f(leftMargin, topMargin), State.BoundingBox.Min + gs_vec2f(leftMargin, topMargin) - gs_vec2f(rightMargin, bottomMargin) + State.BoundingBox.size());
    gs_vec2f  position  = State.BoundingBox.Min - gs_vec2f(horizontalScrollBarPosition.x, verticalScrollBarPosition.y) + gs_vec2f(leftMargin - rightMargin, topMargin - bottomMargin) + gs_vec2f(indent, 0.f);
    gs_vec2f  start     = State.BoundingBox.Min - gs_vec2f(horizontalScrollBarPosition.x, verticalScrollBarPosition.y) + gs_vec2f(leftMargin - rightMargin, topMargin - bottomMargin);
    float     maxHeight = 0.f;

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        (*it)->State.BoundingBox = gs_2dboxf(
            position,
            position + gs_clamp((*it)->State.BoundingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));

        maxHeight = gs_max(maxHeight, (*it)->State.BoundingBox.height());

        if((*it)->State.NextLine > 0)
        {
            position = gs_vec2f(
                start.x + (*it)->State.Indent,
                position.y + maxHeight * (*it)->State.NextLine + (topPadding - bottomPadding));
            
            maxHeight = 0.f;
        }
        else
        {
            position += gs_vec2f(
                (*it)->State.BoundingBox.size().x + (leftPadding - rightPadding) + (*it)->State.Indent,
                0.f);
        }
    }
}

// ImmediateUserInterfaceScrollAreaScrollBar
ImmediateUserInterfaceScrollAreaScrollBar::ImmediateUserInterfaceScrollAreaScrollBar(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceScrollAreaScrollBar::~ImmediateUserInterfaceScrollAreaScrollBar(){}

gs_vec2f ImmediateUserInterfaceScrollAreaScrollBar::get_scroll_offset() const
{
    return Position * PositionScale;
}

void ImmediateUserInterfaceScrollAreaScrollBar::set_scroll_offset(const gs_vec2f _Value)
{
    if(gs_max(State.BoundingBox.size().x, State.BoundingBox.size().y) <= 0.f) return;

    PreviousPosition = Position;
    Position         = gs_clamp(PreviousPosition + _Value, gs_vec2f(0.f, 0.f), State.BoundingBox.size() - Size);
}

void ImmediateUserInterfaceScrollAreaScrollBar::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    // auxiliary lambdas
    auto do_not_render_scroll_bar = [this](ImmediateUserInterfaceContextLayer* _Context)
    {
        // clamp self
        State.MaximumSize = gs_vec2f(0.f, 0.f);
        State.MinimumSize = gs_vec2f(0.f, 0.f);
        State.BoundingBox = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min + State.MaximumSize);

        // reset slider
        Position = gs_vec2f(0.f, 0.f);
    };

    auto calculate_scrollbar_length = [](gs_vec2f scrollbarMinimumValue, gs_vec2f scrollbarMaximumValue, gs_vec2f totalContentSize, gs_vec2f scrollbarMinimumSize)->gs_vec2f
    {
        return gs_vec2f(
            gs_min(gs_max(gs_abs(scrollbarMaximumValue.x - scrollbarMinimumValue.x) / gs_abs(totalContentSize.x) * scrollbarMaximumValue.x, scrollbarMinimumSize.x), scrollbarMaximumValue.x),
            gs_min(gs_max(gs_abs(scrollbarMaximumValue.y - scrollbarMinimumValue.y) / gs_abs(totalContentSize.y) * scrollbarMaximumValue.y, scrollbarMinimumSize.y), scrollbarMaximumValue.y));
    };

    auto calculate_scrollbar_slider_position_scale = [](gs_vec2f scrollbarMinimumValue, gs_vec2f scrollbarMaximumValue, gs_vec2f totalContentSize)->gs_vec2f
    {
        return gs_vec2f(
            gs_abs(totalContentSize.x), gs_abs(totalContentSize.y)) / gs_vec2f(gs_abs(scrollbarMaximumValue.x - scrollbarMinimumValue.x),
            gs_abs(scrollbarMaximumValue.y - scrollbarMinimumValue.y));
    };

    // main code
    ImmediateUserInterfaceScrollArea* contentArea = retrieve_scroll_area(_Context);

    if(_Context == nullptr || contentArea == nullptr)
        return;

    // calculate self maximum and minimum sizes
    State.MaximumSize =
        Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical ?
            gs_vec2f(_Context->m_Style.get_scrollbar_width(), (State.Parent != nullptr ? State.Parent->State.BoundingBox.size().y : 512.f)) :
                gs_vec2f((State.Parent != nullptr ? State.Parent->State.BoundingBox.size().x : 512.f), _Context->m_Style.get_scrollbar_width());

    if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar) &&
        Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Horizontal)
    {
        State.MaximumSize.y = 0.f;
        State.MinimumSize = State.MaximumSize;
        State.BoundingBox = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min + State.MaximumSize);
    }
    else if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleVerticalScrollBar) &&
        Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical)
    {
        State.MaximumSize.x =  0.f;
        State.MinimumSize   = State.MaximumSize;
        State.BoundingBox   = gs_2dboxf(State.BoundingBox.Min, State.BoundingBox.Min + State.MaximumSize);
    }

    State.MinimumSize = State.MaximumSize;

    // calculate self bounding box
    gs_vec2f offset =
        Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical ?
            gs_vec2f(0.f, gs_abs(_Context->m_Style.get_frames_radius() * 2.f - _Context->m_Style.get_scrollbar_width()) * 0.5f) :
                gs_vec2f(gs_abs(_Context->m_Style.get_frames_radius() * 2.f - _Context->m_Style.get_scrollbar_width()) * 0.5f, 0.f);

    if(_Context->m_Style.get_frames_radius() <= 0.f)
        offset = gs_vec2f(0.f, 0.f);

    State.BoundingBox = gs_2dboxf(
        State.BoundingBox.Min + offset,
        State.BoundingBox.Min - (Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical ? gs_vec2f(0.f, 0.f) : offset) + State.MaximumSize);

    // calculate scrollbar metrics
    gs_vec2f scrollbarMinimumValue = gs_vec2f(0.f, 0.f);
    gs_vec2f scrollbarMaximumValue =
        Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical ?
            gs_vec2f(_Context->m_Style.get_scrollbar_width(), State.BoundingBox.size().y) :
                gs_vec2f(State.BoundingBox.size().x, _Context->m_Style.get_scrollbar_width());

    // do not render scrollbar out of scrollarea
    if(contentArea == nullptr)
    {
        do_not_render_scroll_bar(_Context);
        return;
    }

    // manage horizontal scrollbar
    if(Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Horizontal)
    {
        if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
        {
            do_not_render_scroll_bar(_Context);
            return;
        }

        gs_vec2f scrollbarSliderLength = calculate_scrollbar_length(
            scrollbarMinimumValue,
            scrollbarMaximumValue,
            contentArea->ContentView->State.ContentSize,
            _Context->m_Style.get_scrollbar_width());

        if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar))
        {
            if((int)gs_abs(scrollbarSliderLength.x - scrollbarMaximumValue.x) < _Context->m_Style.get_frames_width())
            {
                do_not_render_scroll_bar(_Context);
                return;
            }

            if((int)scrollbarSliderLength.x >= (int)scrollbarMaximumValue.x)
            {
                do_not_render_scroll_bar(_Context);
                return;
            }
        }
        else if(contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverHorizontalScrollBar)
        {
            do_not_render_scroll_bar(_Context);
            return;
        }
        else if(contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar)
        {
        }
        else
        {
            do_not_render_scroll_bar(_Context);
            return;
        }
    }

    // manage vertical scrollbar
    if(Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical)
    {
        if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
        {
            do_not_render_scroll_bar(_Context);
            return;
        }

        gs_vec2f scrollbarSliderLength = calculate_scrollbar_length(
            scrollbarMinimumValue,
            scrollbarMaximumValue,
            contentArea->ContentView->State.ContentSize,
            _Context->m_Style.get_scrollbar_width());

        if((contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar))
        {
            if((int)gs_abs(scrollbarSliderLength.y - scrollbarMaximumValue.y) < _Context->m_Style.get_frames_width())
            {
                do_not_render_scroll_bar(_Context);
                return;
            }

            if((int)scrollbarSliderLength.y >= (int)scrollbarMaximumValue.y)
            {
                do_not_render_scroll_bar(_Context);
                return;
            }
        }
        else if(contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar)
        {
            do_not_render_scroll_bar(_Context);
            return;
        }
        else if(contentArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysVerticalScrollBar)
        {
        }
        else
        {
            do_not_render_scroll_bar(_Context);
            return;
        }
    }

    // layout child sliders
    gs_vec2f scrollbarSliderLength = calculate_scrollbar_length(
        scrollbarMinimumValue,
        scrollbarMaximumValue,
        contentArea->ContentView->State.ContentSize + gs_min(State.BoundingBox.size().x, State.BoundingBox.size().y),
        _Context->m_Style.get_scrollbar_width());

    gs_vec2f scrollbarSliderScale = calculate_scrollbar_slider_position_scale(
        scrollbarMinimumValue,
        scrollbarMaximumValue,
        contentArea->ContentView->State.ContentSize + gs_min(State.BoundingBox.size().x, State.BoundingBox.size().y));

    gs_vec2f sliderPosition = gs_clamp(State.BoundingBox.Min + Position, State.BoundingBox.Min, State.BoundingBox.Max - scrollbarSliderLength);
    
    if(Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical)
    {
        if(sliderPosition.y > State.BoundingBox.Min.y && sliderPosition.y < (State.BoundingBox.Max - scrollbarSliderLength).y)
            PositionScale = scrollbarSliderScale;
    }
    else
    {
        if(sliderPosition.x > State.BoundingBox.Min.x && sliderPosition.x < (State.BoundingBox.Max - scrollbarSliderLength).x)
            PositionScale = scrollbarSliderScale;
    }

    Size = scrollbarSliderLength;
}

bool ImmediateUserInterfaceScrollAreaScrollBar::events(ImmediateUserInterfaceContextLayer* _Context)
{
    // resize
    if(((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable) || _Context->m_Hierarchy.get_parent(this)) &&
        !(State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
    {
        auto scrollArea = retrieve_scroll_area(_Context);

        if(Type == ImmediateUserInterfaceScrollAreaScrollBarType_::ImmediateUserInterfaceScrollAreaScrollBarType_Vertical)
        {
            if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight) ||
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
            {
                ImmediateUserInterfaceNode* resizable =
                    ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

                ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);

                // trigger event
                if(resizable != nullptr)
                {
                    if(_Context->m_Input.is_mouse_button_pressed())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight;
                        return true;
                    }

                    // execute event
                    if(_Context->m_Input.is_mouse_button_down() &&
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight))
                    {
                        ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight);
                        return true;
                    }
                }
            }
            else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight) ||
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
            {
                ImmediateUserInterfaceNode* resizable =
                    ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);

                ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);

                // trigger event
                if(resizable != nullptr)
                {
                    if(_Context->m_Input.is_mouse_button_pressed())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight;
                        return true;
                    }

                    // execute event
                    if(_Context->m_Input.is_mouse_button_down() &&
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight))
                    {
                        ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight);
                        return true;
                    }
                }
            }
            else if((scrollArea != nullptr && (int)scrollArea->get_horizontal_scrollbar_width(_Context) <= 0.f) &&
                    ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                    (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
            {
                ImmediateUserInterfaceNode* resizable =
                    ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

                ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

                // trigger event
                if(resizable != nullptr)
                {
                    if(_Context->m_Input.is_mouse_button_pressed())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight;
                        return true;
                    }

                    // execute event
                    if(_Context->m_Input.is_mouse_button_down() &&
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
                    {
                        ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);
                        return true;
                    }
                }
            }
        }
        else
        {
            if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft) ||
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
            {
                ImmediateUserInterfaceNode* resizable =
                    ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);

                ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);

                // trigger event
                if(resizable != nullptr)
                {
                    if(_Context->m_Input.is_mouse_button_pressed())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft;
                        return true;
                    }

                    // execute event
                    if(_Context->m_Input.is_mouse_button_down() &&
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft))
                    {
                        ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft);
                        return true;
                    }
                }
            }
            else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
            {
                ImmediateUserInterfaceNode* resizable =
                    ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

                ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);

                // trigger event
                if(resizable != nullptr)
                {
                    if(_Context->m_Input.is_mouse_button_pressed())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight;
                        return true;
                    }

                    // execute event
                    if(_Context->m_Input.is_mouse_button_down() &&
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight))
                    {
                        ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight);
                        return true;
                    }
                }
            }
            else if(ImmediateUserInterfaceContextLayerHelpers::check_cursor_intersection_with_resize_gizmo(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom) ||
                (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
            {
                ImmediateUserInterfaceNode* resizable =
                    ImmediateUserInterfaceContextLayerHelpers::find_resizable_node(_Context, this, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);

                ImmediateUserInterfaceContextLayerHelpers::render_resize_gizmo(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);

                // trigger event
                if(resizable != nullptr)
                {
                    if(_Context->m_Input.is_mouse_button_pressed())
                    {
                        resizable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom;
                        return true;
                    }

                    // execute event
                    if(_Context->m_Input.is_mouse_button_down() &&
                        (State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom))
                    {
                        ImmediateUserInterfaceContextLayerHelpers::resize_node(_Context, resizable, ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom);
                        return true;
                    }
                }
            }
        }
    }

    // move slider on mouse press event
    if(_Context->m_Input.is_mouse_button_pressed())
    {
        Position         = gs_clamp((_Context->m_Input.get_cusor_position() - State.BoundingBox.Min) - Size * 0.5f, gs_vec2f(0.f, 0.f), State.BoundingBox.size() - Size);
        PreviousPosition = Position;
        State.Events    |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_Custom;
    }

    // move slider on mouse drag event
    if(_Context->m_Input.is_mouse_button_down())
    {
        Position      = gs_clamp(PreviousPosition + ApplicationPlatformBackend::get_window_cursor_dragdelta(), gs_vec2f(0.f, 0.f), State.BoundingBox.size() - Size);
        State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_Custom;
    }

    return true;
}

void ImmediateUserInterfaceScrollAreaScrollBar::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // outline
    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    // background
    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    // slider
    gs_vec2f sliderPosition = gs_clamp(State.BoundingBox.Min + Position, State.BoundingBox.Min, State.BoundingBox.Max - Size);

    if(State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered)
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            sliderPosition + _Context->m_Style.get_frames_width() * 2.f,
            sliderPosition + Size - _Context->m_Style.get_frames_width() * 2.f,
            _Context->m_Style.get_frames_radius(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }
    else
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            sliderPosition + _Context->m_Style.get_frames_width() * 2.f,
            sliderPosition + Size - _Context->m_Style.get_frames_width() * 2.f,
            _Context->m_Style.get_frames_radius(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }
}

ImmediateUserInterfaceScrollArea* ImmediateUserInterfaceScrollAreaScrollBar::retrieve_scroll_area(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return nullptr;

    auto parent = _Context->m_Hierarchy.get_parent(this);

    while (parent)
    {
        ImmediateUserInterfaceScrollArea* contentArea =
            dynamic_cast<ImmediateUserInterfaceScrollArea*>(parent);

        if(contentArea != nullptr)
            return contentArea;
            
        parent = _Context->m_Hierarchy.get_parent(parent);
    }

    return nullptr;
}

// ImmediateUserInterfaceMenu
ImmediateUserInterfaceMenu::ImmediateUserInterfaceMenu(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceMenu::~ImmediateUserInterfaceMenu(){}

void ImmediateUserInterfaceMenu::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(InternalScrollArea != nullptr)
    {
        State.BoundingBox = gs_2dboxf(
            State.BoundingBox.Min,
            State.BoundingBox.Min + InternalScrollArea->State.BoundingBox.size());
    }

    ImmediateUserInterfaceNodePanel::layout(_Context);
}

void ImmediateUserInterfaceMenu::render(ImmediateUserInterfaceContextLayer*){}

void ImmediateUserInterfaceMenu::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child == nullptr) return;

    if(dynamic_cast<ImmediateUserInterfaceMenuScrollArea*>(_Child))
    {
        _Child->State.Parent = this;
        return;
    }

    if(dynamic_cast<ImmediateUserInterfaceMenuAction*>(_Child) || dynamic_cast<ImmediateUserInterfaceMenu*>(_Child))
        _Child->State.NextLine = 1;
    
    if(State.Parent)
    {
        if(ExternalScrollArea)
            ExternalScrollArea->attach_child(_Child);
    }
    else 
        InternalScrollArea->attach_child(_Child);
}

// ImmediateUserInterfaceMenuScrollArea
ImmediateUserInterfaceMenuScrollArea::ImmediateUserInterfaceMenuScrollArea(const std::string& _Name) : ImmediateUserInterfaceScrollArea(_Name){}
ImmediateUserInterfaceMenuScrollArea::~ImmediateUserInterfaceMenuScrollArea(){}

void ImmediateUserInterfaceMenuScrollArea::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    ImmediateUserInterfaceMenu* menu =
        _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceMenu>(this);

    if(_Context->m_Hierarchy.get_parent(menu)) return;

    // outline
    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuOutline),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
}

// ImmediateUserInterfaceMenuAction
ImmediateUserInterfaceMenuAction::ImmediateUserInterfaceMenuAction(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceMenuAction::~ImmediateUserInterfaceMenuAction(){}

void ImmediateUserInterfaceMenuAction::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    gs_vec2f size =
        _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() +
        gs_vec2f(_Context->m_Style.get_font_size() * 2.f, _Context->m_Style.get_font_size() * 0.5f);

    State.MinimumSize = gs_vec2f(gs_min(size.x, State.MinimumSize.x), gs_min(size.y, State.MinimumSize.y));
    State.MaximumSize = gs_vec2f(gs_max(size.x, State.MaximumSize.x), gs_max(size.y, State.MaximumSize.y));
}

void ImmediateUserInterfaceMenuAction::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // background
    if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && _Context->m_Input.is_mouse_button_down())
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
            State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_frames_radius(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }
    else
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
            State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_frames_radius(),
            (State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }

    // title
    _Context->m_Renderer->push_text(
        gs_vec2f(
            State.BoundingBox.Min.x, 
            (State.BoundingBox.center() - _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() * 0.5f).y),
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix(
            (float)place_in_follow()));
}

// ImmediateUserInterfaceMenuItem
ImmediateUserInterfaceMenuItem::ImmediateUserInterfaceMenuItem(const std::string& _Name) : ImmediateUserInterfaceMenuAction(_Name){}
ImmediateUserInterfaceMenuItem::~ImmediateUserInterfaceMenuItem(){}

void ImmediateUserInterfaceMenuItem::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // background
    if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && _Context->m_Input.is_mouse_button_down())
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
            State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_frames_radius(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }
    else
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
            State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_frames_radius(),
            (State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }

    // title
    _Context->m_Renderer->push_text(
        gs_vec2f(
            State.BoundingBox.Min.x, 
            (State.BoundingBox.center() - _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() * 0.5f).y),
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    // triangle
    float triangleWidth = _Context->m_Style.get_popup_menu_pointer_size();

    _Context->m_Renderer->push_triangle_filled(
        gs_vec2f(0.f, 0.0),
        gs_vec2f(0.f, triangleWidth),
        gs_vec2f(triangleWidth * 0.5f, triangleWidth * 0.5f),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix(
            (float)place_in_follow(),
            gs_vec2f(
                State.BoundingBox.Max.x - triangleWidth,
                State.BoundingBox.center().y - triangleWidth * 0.5f)));
}

// ImmediateUserInterfaceCombobox
ImmediateUserInterfaceCombobox::ImmediateUserInterfaceCombobox(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
{
    State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 128.f));
}

ImmediateUserInterfaceCombobox::~ImmediateUserInterfaceCombobox(){}

void ImmediateUserInterfaceCombobox::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // layout self
    State.BoundingBox = gs_2dboxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + gs_vec2f(State.BoundingBox.width(), _Context->m_Style.get_font_size()));

    if(ScrollArea == nullptr || ScrollArea->ContentView == nullptr)
        return;

    // resize children
    float MaximumWidth = 0.f;

    for(auto it = _Context->m_Hierarchy.begin(ScrollArea->ContentView); it != _Context->m_Hierarchy.end(ScrollArea->ContentView); it++)
    {
        ImmediateUserInterfaceComboboxItem* comboboxItem =
            dynamic_cast<ImmediateUserInterfaceComboboxItem*>(*it);

        if(comboboxItem == nullptr)
            continue;

        MaximumWidth = _Context->m_Renderer->calculate_bounding_box(
            comboboxItem->Name.begin(),
            comboboxItem->Name.end(),
            _Context->m_Style.get_font_size(),
            _Context->m_Style.get_current_font()).size().x + _Context->m_Style.get_font_size();
    }

    for(auto it = _Context->m_Hierarchy.begin(ScrollArea->ContentView); it != _Context->m_Hierarchy.end(ScrollArea->ContentView); it++)
    {
        ImmediateUserInterfaceComboboxItem* comboboxItem =
            dynamic_cast<ImmediateUserInterfaceComboboxItem*>(*it);

        if(comboboxItem != nullptr)
        {
            comboboxItem->State.MinimumSize = gs_vec2f(MaximumWidth, _Context->m_Style.get_font_size());
            comboboxItem->State.MaximumSize = comboboxItem->State.MinimumSize;

            comboboxItem->State.BoundingBox = gs_2dboxf(
                comboboxItem->State.BoundingBox.Min,
                comboboxItem->State.BoundingBox.Min + gs_vec2f(MaximumWidth, _Context->m_Style.get_font_size()));
        }
    }
}

bool ImmediateUserInterfaceCombobox::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return false;

    // activate self
    if(_Context->m_Input.is_mouse_button_pressed() && State.BoundingBox.contains(_Context->m_Input.get_cusor_position()))
    {
        if(Active)
        {
            Active  = false;
            Hovered = false;
        }
        else
        {
            Active  = true;
        }
    }

    return true;
}

void ImmediateUserInterfaceCombobox::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(ScrollArea != nullptr)
        ScrollArea->attach_child(_Child);
}

// ImmediateUserInterfaceComboboxItem
ImmediateUserInterfaceComboboxItem::ImmediateUserInterfaceComboboxItem(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceComboboxItem::~ImmediateUserInterfaceComboboxItem(){}

void ImmediateUserInterfaceComboboxItem::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    gs_vec2f size =
        _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() +
        gs_vec2f(_Context->m_Style.get_font_size() * 2.f, _Context->m_Style.get_font_size() * 0.5f);

    State.MinimumSize = gs_vec2f(gs_min(size.x, State.MinimumSize.x), size.y);
    State.MaximumSize = gs_vec2f(gs_max(size.x, State.MaximumSize.x), size.y);
}

void ImmediateUserInterfaceComboboxItem::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // background
    if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && _Context->m_Input.is_mouse_button_down())
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
            State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_frames_radius(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }
    else
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
            State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_frames_radius(),
            (State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }

    // title
    _Context->m_Renderer->push_text(
        gs_vec2f(
            State.BoundingBox.Min.x, 
            (State.BoundingBox.center() - _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() * 0.5f).y),
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix(
            (float)place_in_follow()));
}

// ImmediateUserInterfaceComboboxItemScrollArea
ImmediateUserInterfaceComboboxScrollArea::ImmediateUserInterfaceComboboxScrollArea(const std::string& _Name) : ImmediateUserInterfaceScrollArea(_Name){}
ImmediateUserInterfaceComboboxScrollArea::~ImmediateUserInterfaceComboboxScrollArea(){}
void ImmediateUserInterfaceComboboxScrollArea::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // outline
    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
}

// ImmediateUserInterfaceTreeNode
ImmediateUserInterfaceTreeNode::ImmediateUserInterfaceTreeNode(const std::string& _Name) : ImmediateUserInterfaceNodePanel(_Name){}
ImmediateUserInterfaceTreeNode::~ImmediateUserInterfaceTreeNode(){}

void ImmediateUserInterfaceTreeNode::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;
    
    // background
    if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        TitleBox.contains(_Context->m_Input.get_cusor_position()))
    {
        _Context->m_Renderer->push_rectangle_rounded_filled(
            TitleBox.Min,
            TitleBox.Max,
            _Context->m_Style.get_frames_radius(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }

    // lines
    int childrenCount = 0;

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        childrenCount++;

        if(!(TreeSettings & ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_RenderConnectionLines))
            continue;

        ImmediateUserInterfaceTreeNode* treeNode =
            dynamic_cast<ImmediateUserInterfaceTreeNode*>(*it);

        if(treeNode == nullptr)
            continue;

        _Context->m_Renderer->push_line(
            gs_vec2f(IconBox.center().x, IconBox.Max.y),
            gs_vec2f(IconBox.center().x, treeNode->IconBox.center().y),
            _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

        _Context->m_Renderer->push_line(
            gs_vec2f(IconBox.center().x, treeNode->IconBox.center().y),
            gs_vec2f(treeNode->IconBox.Min.x, treeNode->IconBox.center().y),
            _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    }

    // user-defined icons
    if(!TextureOpened.is_null() && !TextureClosed.is_null())
    {
        if(Opened && childrenCount > 0)
        {
            _Context->m_Renderer->push_rectangle_filled(
                IconBox.Min,
                IconBox.Max,
                gs_color_rgba(255, 255, 255, 255),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                TextureOpened);
        }
        else
        {
            _Context->m_Renderer->push_rectangle_filled(
                IconBox.Min,
                IconBox.Max,
                gs_color_rgba(255, 255, 255, 255),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                TextureClosed);
        }
    }
    
    // default icons
    else
    {
        // TODO: dots ???
        
        // triangles
        if(Opened && childrenCount > 0)
        {
            _Context->m_Renderer->push_triangle_filled(
                IconBox.center() + gs_vec2f(-IconBox.width() * 0.25f, -IconBox.height() * 0.25f),
                IconBox.center() + gs_vec2f(+IconBox.width() * 0.25f, -IconBox.height() * 0.25f),
                IconBox.center() + gs_vec2f(0.f, IconBox.height() * 0.25f * 0.5f),
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
        }
        else
        {
            _Context->m_Renderer->push_triangle_filled(
                IconBox.center() + gs_vec2f(0.f, -IconBox.height() * 0.25f),
                IconBox.center() + gs_vec2f(0.f, +IconBox.height() * 0.25f),
                IconBox.center() + gs_vec2f(+IconBox.width() * 0.25f, 0.f),
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
        }
    }

    // title
    _Context->m_Renderer->push_text(
        gs_vec2f(IconBox.Max.x, IconBox.Min.y),
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
}

void ImmediateUserInterfaceTreeNode::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // layout self
    State.BoundingBox = gs_2dboxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + gs_clamp(State.ContentSize, State.MinimumSize, State.MaximumSize));

    TitleBox = gs_2dboxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + gs_vec2f(State.BoundingBox.width(), _Context->m_Style.get_font_size()));

    IconBox = gs_2dboxf(
        TitleBox.Min,
        TitleBox.Min + gs_vec2f(_Context->m_Style.get_font_size(), _Context->m_Style.get_font_size()));

    // layout children
    gs_vec2f position  = State.BoundingBox.Min + gs_vec2f(0.f, _Context->m_Style.get_font_size()) + gs_vec2f(IconBox.width(), 0.f);
    gs_vec2f start     = State.BoundingBox.Min + gs_vec2f(0.f, _Context->m_Style.get_font_size()) + gs_vec2f(IconBox.width(), 0.f);
    float    maxHeight = 0.f;

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        (*it)->State.BoundingBox = gs_2dboxf(
            position,
            position + gs_clamp((*it)->State.BoundingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));

        maxHeight = gs_max(maxHeight, (*it)->State.BoundingBox.height());

        if((*it)->State.NextLine > 0)
        {
            position = gs_vec2f(
                start.x + (*it)->State.Indent,
                position.y + maxHeight * (*it)->State.NextLine);
            
            maxHeight = 0.f;
        }
        else
        {
            position += gs_vec2f(
                (*it)->State.BoundingBox.size().x + (*it)->State.Indent,
                0.f);
        }
    }
}

bool ImmediateUserInterfaceTreeNode::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return false;

    if((TreeSettings & ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_OpenOnClick))
    {
        if(_Context->m_Input.is_mouse_button_clicked() && TitleBox.contains(_Context->m_Input.get_cusor_position()))
            Opened = !Opened;
    }
    else if((TreeSettings & ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_OpenOnDoubleClick))
    {
        if(_Context->m_Input.is_mouse_button_double_clicked() && TitleBox.contains(_Context->m_Input.get_cusor_position()))
            Opened = !Opened;
    }

    return ImmediateUserInterfaceNode::events(_Context);
}

void ImmediateUserInterfaceTreeNode::measure(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // this is the title box
    gs_2dboxf box = gs_2dboxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + _Context->m_Renderer->calculate_bounding_box(
            Name.begin(),
            Name.end(),
            _Context->m_Style.get_font_size(),
            _Context->m_Style.get_current_font()).size() + gs_vec2f(_Context->m_Style.get_font_size(), _Context->m_Style.get_font_size()));

    // this are children
    for (auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        box = gs_2dboxf(
            box.Min,
            (*it)->State.BoundingBox.Min,
            box.Max,
            (*it)->State.BoundingBox.Max);
    }

    State.ContentSize = box.size();
}

// ImmediateUserInterfaceWindow
ImmediateUserInterfaceWindow::ImmediateUserInterfaceWindow(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceWindow::~ImmediateUserInterfaceWindow(){}

void ImmediateUserInterfaceWindow::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // auxiliary lambdas
    auto close_button_color = [](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& closeButtonBox)
    {
        if(_Context->m_Input.is_mouse_button_down() && closeButtonBox.contains(_Context->m_Input.get_cusor_position()))
            return gs_color_rgba(255, 0, 0, 255);

        return closeButtonBox.contains(_Context->m_Input.get_cusor_position()) ?
            gs_color_rgba(128, 0, 0, 255) : // TODO: this MUST BE a setting
            gs_color_rgba(64, 0, 0, 255);
    };

    auto render_close_button = [this, &close_button_color](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _Box)
    {
        gs_2dboxf closeButtonBox = gs_2dboxf(
            _Box.Min + _Context->m_Style.get_frames_width() * 2.f,
            _Box.Max - _Context->m_Style.get_frames_width() * 2.f);

        _Context->m_Renderer->push_arc_filled(
            closeButtonBox.center(),
            closeButtonBox.size().x,
            closeButtonBox.size().y,
            0.f,
            360.f,
            close_button_color(_Context, closeButtonBox),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

        _Context->m_Renderer->push_line(
            closeButtonBox.Min + gs_vec2f(+4.f, +4.f),
            closeButtonBox.Max - gs_vec2f(+4.f, +4.f),
            4.f,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

        _Context->m_Renderer->push_line(
            gs_vec2f(closeButtonBox.Max.x, closeButtonBox.Min.y) + gs_vec2f(-4.f, +4.f),
            gs_vec2f(closeButtonBox.Min.x, closeButtonBox.Max.y) + gs_vec2f(+4.f, -4.f),
            4.f,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
    };

    // content background and outline frame
    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    _Context->m_Renderer->push_rectangle_rounded_filled(
        FrameBox.Min + _Context->m_Style.get_frames_width(),
        FrameBox.Max - _Context->m_Style.get_frames_width(),
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    // frame
    process_window_frame(
        _Context,
        this,

        // render self and child frames
        [this, &render_close_button](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _FrameBox, const gs_2dboxf& _Frame, ImmediateUserInterfaceWindow* _Window)
        {
            ImmedidateUserInterfaceWindowController* windowsController =
                _Context->get_controller<ImmedidateUserInterfaceWindowController>();

            // detect if we are active
            bool active   = _Window->IsActive;

            bool isDocker = windowsController != nullptr && !windowsController->retrieve_docked_windows(
                _Context,
                _Window,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center).empty();

            // render frame
            if(_Window->Docker == nullptr && !isDocker)
            {
                // frame
                if(_Frame.contains(_Context->m_Input.get_cusor_position()) &&
                    (_Window->TopSnapper    != nullptr ||
                     _Window->LeftSnapper   != nullptr ||
                     _Window->RightSnapper  != nullptr ||
                     _Window->BottomSnapper != nullptr))
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Frame.Min + _Context->m_Style.get_frames_width(),
                        _Frame.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }

                // close button
                if(_Window->Opened != nullptr)
                {
                    gs_2dboxf closeButtonBox  = gs_2dboxf(
                        gs_vec2f(_FrameBox.Max.x - _FrameBox.height() / 2.f, _Frame.center().y) - _FrameBox.height() / 4.f,
                        gs_vec2f(_FrameBox.Max.x - _FrameBox.height() / 2.f, _Frame.center().y) + _FrameBox.height() / 4.f);

                    render_close_button(_Context, closeButtonBox);

                    *_Window->Opened = !(closeButtonBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_clicked());
                }
            }
            else
            {
                // frame
                if(active)
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Frame.Min + _Context->m_Style.get_frames_width(),
                        _Frame.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                        true,
                        true,
                        false,
                        false);
                }
                else if(_Frame.contains(_Context->m_Input.get_cusor_position()))
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Frame.Min + _Context->m_Style.get_frames_width(),
                        _Frame.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                        true,
                        true,
                        false,
                        isDocker);
                }

                // close button
                if(_Window->Opened != nullptr)
                {
                    if(active || _Frame.contains(_Context->m_Input.get_cusor_position()))
                    {
                        gs_2dboxf closeButtonBox  = gs_2dboxf(
                            gs_vec2f(_Frame.Max.x - _Frame.height() / 2.f, _Frame.center().y) - _Frame.height() / 4.f,
                            gs_vec2f(_Frame.Max.x - _Frame.height() / 2.f, _Frame.center().y) + _Frame.height() / 4.f);

                        render_close_button(_Context, closeButtonBox);

                        *_Window->Opened = !(closeButtonBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_clicked());
                    }
                }
            }

            // title
            _Context->m_Renderer->push_text(
                gs_vec2f(
                    _Frame.Min.x + 16.f + _Context->m_Style.get_frames_width(),
                    _Frame.center().y - _Context->m_Renderer->calculate_bounding_box(_Window->Name.begin(), _Window->Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).height() * 0.6f * 0.5f),
                _Window->Name.begin(),
                _Window->Name.end(),
                _Context->m_Style.get_font_size() * 0.6f,
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
        }
    );
}

void ImmediateUserInterfaceWindow::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    // compute self geometry
    FrameBox = gs_2dboxf(
        State.BoundingBox.Min,
        gs_vec2f(
            State.BoundingBox.Max.x,
            State.BoundingBox.Min.y + gs_max(_Context->m_Style.get_font_size(), 64.f)));

    ContentBox = gs_2dboxf(
        (Docker == nullptr ? gs_vec2f(FrameBox.Min.x, FrameBox.Max.y) : State.BoundingBox.Min),
        State.BoundingBox.Max);

    ContentBox = gs_2dboxf(
        ContentBox.Min + _Context->m_Style.get_frames_width(),
        ContentBox.Max - _Context->m_Style.get_frames_width());

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        ContentBox.Min,
        ContentBox.size(),
        gs_vec4f(0.f),
        gs_vec4f(0.f),
        State.Settings,
        [this](const ImmediateUserInterfaceNode* _Node){return true;});
}

bool ImmediateUserInterfaceWindow::events(ImmediateUserInterfaceContextLayer* _Context)
{
    bool defaultPipeline = true;

    // window frame events
    process_window_frame(
        _Context,
        this,

        // catch frame events
        [this, &defaultPipeline](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _FrameBox, const gs_2dboxf& _Frame, ImmediateUserInterfaceWindow* _Window)
        {
            // focus and activity
            if(_Context->m_Input.is_mouse_button_pressed() && _Frame.contains(_Context->m_Input.get_cusor_position()))
                _Window->Activate = true;

            // move
            if((_Window->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable) &&
                !((_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)           ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)        ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)       ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)      ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)  ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight) ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)     ||
                    (_Window->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)))
            {
                ImmediateUserInterfaceNode* movable = _Window;

                if(_Context->m_Input.is_mouse_button_pressed() &&
                    gs_2dboxf(
                        _Frame.Min + gs_vec2f(ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Context, this).Radius, 0.f),
                        _Frame.Max - gs_vec2f(ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Context, this).Radius, 0.f)).contains(_Context->m_Input.get_cusor_position()))
                {
                    movable->State.Events |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;
                    defaultPipeline = false;
                    return;
                }

                if(_Context->m_Input.is_mouse_button_down() &&
                    _Frame.contains(_Context->m_Input.get_cusor_position())   &&
                    gs_vector_length(_Context->m_Input.get_cusor_drag_delta()) > 8.f &&
                    (movable->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
                {
                    movable->State.BoundingBox = gs_2dboxf(
                        movable->Cache.BoundingBox.Min + _Context->m_Input.get_cusor_drag_delta(),
                        movable->Cache.BoundingBox.Max + _Context->m_Input.get_cusor_drag_delta());

                    if(dynamic_cast<ImmediateUserInterfaceWindow*>(movable))
                        dynamic_cast<ImmediateUserInterfaceWindow*>(movable)->ReattachChildren = true;

                    defaultPipeline = false;
                    return;
                }
            }
        });

    // default event pipeline
    if(defaultPipeline)
        return ImmediateUserInterfaceNode::events(_Context);
    
    return !defaultPipeline;
}

void ImmediateUserInterfaceWindow::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child == nullptr)
        return;

    if( dynamic_cast<ImmediateUserInterfaceWindowHorizontalSnapper*>(_Child) ||
        dynamic_cast<ImmediateUserInterfaceWindowVerticalSnapper*>(_Child)   ||
        dynamic_cast<ImmediateUserInterfaceWindowCentralDocker*>(_Child))
    {
        _Child->State.Parent = this;
    }
    else
    {
        _Child->State.Parent = ContentView;
    }
}

bool ImmediateUserInterfaceWindow::create_contents(ImmediateUserInterfaceContextLayer* _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render)
{
    if(_Context == nullptr) return false;

    ImmediateUserInterfaceNodeSettings settings = _Settings & ~ImmediateUserInterfaceNodeSettings_NullParent;

    ImmediateUserInterfaceWindow* window = this;
    window->Opened                       = _Render;

    if(_Context->begin_node<ImmediateUserInterfaceWindowCentralDocker>(
        std::string(_ID).append("/CentralDockerView"),
        settings))
    {
        if(!window->IsActive)
        {
            _Context->end_node<ImmediateUserInterfaceWindowCentralDocker>();
            _Context->end_node<ImmediateUserInterfaceWindow>();
            return false;
        }

        window->DockerView                                    = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowCentralDocker>();
        window->DockerView->State.PlaceInFollow               = true;
        window->DockerView->State.OrderChildrenWhileRendering = true;

        if(_Context->begin_node<ImmediateUserInterfaceWindowVerticalSnapper>(
            std::string(_ID).append("/SnapperView"),
            settings))
        {
            window->SnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowVerticalSnapper>();

            // top
            if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                std::string(_ID).append("/SnapperView/TopSnapperView"),
                settings))
            {
                window->TopSnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalSnapper>();
                _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
            }

            // center
            if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                std::string(_ID).append("/SnapperView/CentralSnapperView"),
                settings))
            {
                if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                    std::string(_ID).append("/SnapperView/CentralSnapperView/LeftSnapperView"),
                    settings))
                {
                    window->LeftSnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalSnapper>();
                    _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
                }

                if(_Context->begin_node<ImmediateUserInterfaceWindowVerticalSnapper>(
                    std::string(_ID).append("/SnapperView/CentralSnapperView/ContentView"),
                    settings))
                {
                    window->ContentView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowVerticalSnapper>();
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowVerticalSnapper>()->ContentPadding = _Context->m_Style.get_frames_width();
                    _Context->end_node<ImmediateUserInterfaceWindowVerticalSnapper>();
                }

                if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                    std::string(_ID).append("/SnapperView/CentralSnapperView/RightSnapperView"),
                    settings))
                {
                    window->RightSnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalSnapper>();
                    _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
                }

                _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
            }

            // bottom
            if(_Context->begin_node<ImmediateUserInterfaceWindowHorizontalSnapper>(
                std::string(_ID).append("/SnapperView/BottomSnapperView"),
                settings))
            {
                window->BottomSnapperView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowHorizontalSnapper>();
                _Context->end_node<ImmediateUserInterfaceWindowHorizontalSnapper>();
            }

            _Context->end_node<ImmediateUserInterfaceWindowVerticalSnapper>();
        }

        _Context->end_node<ImmediateUserInterfaceWindowCentralDocker>();
    }

    return true;
}

void ImmediateUserInterfaceWindow::load_state(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    // self position and size
    gs_vec2f size =
        _Context->m_IniFileState.contains(Hash, "Size") ?
            _Context->m_IniFileState.get<gs_vec2f>(Hash, "Size") :
                gs_vec2f(512.f, 512.f);
    
    gs_vec2f position =
        _Context->m_IniFileState.contains(Hash, "Position") ?
            _Context->m_IniFileState.get<gs_vec2f>(Hash, "Position") :
                gs_vec2f(0.f, 0.f);
    
    DockingIndex =
        _Context->m_IniFileState.contains(Hash, "DockingIndex") ?
            _Context->m_IniFileState.get<int>(Hash, "DockingIndex") :
                -1;

    State.BoundingBox = gs_2dboxf(position, position + gs_clamp(size, State.MinimumSize, State.MaximumSize));

    // layout
    if(_Context->m_IniFileState.contains(Hash, "TopSnapperViewSize"))
    {
        TopSnapperView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "TopSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "LeftSnapperViewSize"))
    {
        LeftSnapperView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "LeftSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "RightSnapperViewSize"))
    {
        RightSnapperView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "RightSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "BottomSnapperViewSize"))
    {
        BottomSnapperView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "BottomSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "ContentViewSize"))
    {
        ContentView->State.BoundingBox =
            gs_2dboxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "ContentViewSize"));
    }
}

void ImmediateUserInterfaceWindow::save_state(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    // self position and size
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "Size", State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "Position", State.BoundingBox.Min);

    // layout items positions and sizes
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "TopSnapperViewSize", TopSnapperView->State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "LeftSnapperViewSize", LeftSnapperView->State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "RightSnapperViewSize", RightSnapperView->State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "BottomSnapperViewSize", BottomSnapperView->State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "ContentViewSize", ContentView->State.BoundingBox.size());

    // docking
    _Context->m_IniFileState.set<int>(Hash, "DockingIndex", DockingIndex);

    if(Docker)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "Docker",
            retrieve_docker_by_view(_Context, Docker)->Hash);
    }
    
    if(TopSnapper)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "TopSnapper",
            retrieve_docker_by_view(_Context, TopSnapper)->Hash);
    }
    
    if(LeftSnapper)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "LeftSnapper",
            retrieve_docker_by_view(_Context, LeftSnapper)->Hash);
    }
    
    if(RightSnapper)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "RightSnapper",
            retrieve_docker_by_view(_Context, RightSnapper)->Hash);
    }
    
    if(BottomSnapper)
    {
        _Context->m_IniFileState.set<std::string>(
            Hash,
            "BottomSnapper",
            retrieve_docker_by_view(_Context, BottomSnapper)->Hash);
    }

    _Context->m_IniFileState.set<bool>(Hash, "IsActive", IsActive);
}

// ImmediateUserInterfaceWindowDockArea
ImmediateUserInterfaceWindowDockArea::ImmediateUserInterfaceWindowDockArea(const std::string& _Name) : ImmediateUserInterfaceWindow(_Name){}
ImmediateUserInterfaceWindowDockArea::~ImmediateUserInterfaceWindowDockArea(){}

void ImmediateUserInterfaceWindowDockArea::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    State.BoundingBox = gs_2dboxf(
        _Context->m_Renderer->current_viewport().Min - _Context->m_Style.get_frames_width(),
        _Context->m_Renderer->current_viewport().Max + _Context->m_Style.get_frames_width());

    ImmediateUserInterfaceWindow::layout(_Context);
}

void ImmediateUserInterfaceWindowDockArea::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // content background and outline frame
    _Context->m_Renderer->push_rectangle_rounded_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    _Context->m_Renderer->push_rectangle_rounded_filled(
        FrameBox.Min + _Context->m_Style.get_frames_width() * 2.f,
        FrameBox.Max - _Context->m_Style.get_frames_width() * 2.f,
        _Context->m_Style.get_frames_radius(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

    // frame
    process_window_frame(
        _Context,
        this,

        // render self and child frames
        [this](ImmediateUserInterfaceContextLayer* _Context, const gs_2dboxf& _FrameBox, const gs_2dboxf& _Frame, ImmediateUserInterfaceWindow* _Window)
        {
            // detect if we are active
            bool active = _Window->IsActive;

            // render frame
            if(_Window->Docker == nullptr &&
                _Context->get_controller<ImmedidateUserInterfaceWindowController>()->retrieve_docked_windows(
                    _Context,
                    _Window,
                    ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center).empty())
            {
                // frame
                if(_Frame.contains(_Context->m_Input.get_cusor_position()) &&
                    (_Window->TopSnapper    != nullptr ||
                    _Window->LeftSnapper   != nullptr ||
                    _Window->RightSnapper  != nullptr ||
                    _Window->BottomSnapper != nullptr))
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Frame.Min + _Context->m_Style.get_frames_width(),
                        _Frame.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
                }
            }
            else
            {
                // frame
                if(active)
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Frame.Min + _Context->m_Style.get_frames_width(),
                        _Frame.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                        true,
                        true,
                        false,
                        false);
                }
                else if(_Frame.contains(_Context->m_Input.get_cusor_position()))
                {
                    _Context->m_Renderer->push_rectangle_rounded_filled(
                        _Frame.Min + _Context->m_Style.get_frames_width(),
                        _Frame.Max - _Context->m_Style.get_frames_width(),
                        _Context->m_Style.get_frames_radius(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered),
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                        true,
                        true,
                        false,
                        false);
                }
            }

            // title
            _Context->m_Renderer->push_text(
                gs_vec2f(
                    _Frame.Min.x + 16.f + _Context->m_Style.get_frames_width(),
                    _Frame.center().y - _Context->m_Renderer->calculate_bounding_box(_Window->Name.begin(), _Window->Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).height() * 0.6f * 0.5f),
                _Window->Name.begin(),
                _Window->Name.end(),
                _Context->m_Style.get_font_size() * 0.6f,
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
        }
    );
}

// ImmedidateUserInterfaceWindowController
ImmedidateUserInterfaceWindowController::ImmedidateUserInterfaceWindowController(){}
ImmedidateUserInterfaceWindowController::~ImmedidateUserInterfaceWindowController(){}

void ImmedidateUserInterfaceWindowController::frame_start(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    // create worksapce dockarea
    m_DockAreaOpened =
        (_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking) &&
        (_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking);

    if(_Context->begin_node<ImmediateUserInterfaceWindowDockArea>(
        std::string(ApplicationPlatformBackend::get_window_name()).append("###").append("DockingWorkspace"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults
        | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ManualRenderingOrderSetup,
        &m_DockAreaOpened))
    {
        // retrieve window
        m_WorkspaceDockArea         = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindow>();
        m_WorkspaceDockArea->Opened = &m_DockAreaOpened;

        m_WorkspaceDockArea->State.RenderingOrder =
            ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Background;

        _Context->end_node<ImmediateUserInterfaceWindowDockArea>();
    }
}

void ImmedidateUserInterfaceWindowController::frame_debug(ImmediateUserInterfaceContextLayer* _Context)
{
    if(!(_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking))
        return;

    place_on_dockers(_Context);

    // rebuild hierarchy
    m_WindowsDockingList.clear();
    m_WindowsDockingCache.clear();
    
    for(auto node : _Context->m_NodesRenderingList)
    {
        if(dynamic_cast<ImmediateUserInterfaceWindow*>(node))
            m_WindowsDockingList.push_back(node);
        else
            m_WindowsDockingCache.push_back(node);
    }

    std::stable_sort(
        m_WindowsDockingList.begin(),
        m_WindowsDockingList.end(),
        [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B) 
        {
            return dynamic_cast<const ImmediateUserInterfaceWindow*>(_A)->DockingIndex <
                dynamic_cast<const ImmediateUserInterfaceWindow*>(_B)->DockingIndex;
        });

    _Context->m_NodesRenderingList.clear();

    for(auto node : m_WindowsDockingList)
        _Context->m_NodesRenderingList.push_back(node);

    for(auto node : m_WindowsDockingCache)
        _Context->m_NodesRenderingList.push_back(node);

    _Context->m_Hierarchy.build(_Context->m_NodesRenderingList);

    // activate/deactivate windows
    for(auto node : _Context->m_NodesRenderingList)
    {
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(node);

        if(window != nullptr)
        {
            // activate window
            if(window->Activate)
            {
                // deactivate docker windows
                ImmediateUserInterfaceWindow* docker =
                    ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, (window->Docker ? window->Docker : window));

                if(docker != nullptr)
                    docker->IsActive = false;

                for(auto node : retrieve_docked_windows(_Context, docker, ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center))
                {
                    if(dynamic_cast<ImmediateUserInterfaceWindow*>(node) != nullptr)
                        dynamic_cast<ImmediateUserInterfaceWindow*>(node)->IsActive = false;
                }

                window->IsActive = true;
            }

            // activate singletone window
            if((window->Docker        == nullptr &&
                window->TopSnapper    == nullptr &&
                window->LeftSnapper   == nullptr &&
                window->RightSnapper  == nullptr &&
                window->BottomSnapper == nullptr &&
                retrieve_docked_windows(_Context, window, ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_All).empty()))
            {
                window->IsActive = true;
            }

            if(window->IsActive)
            {
                // setup maximum rendering order for docked window within it's docker hierarchy
                if(window->Docker != nullptr)
                {
                    int renderingOrder = window->Docker->State.RenderingOrder;

                    for(auto it  = _Context->m_Hierarchy.begin(window->Docker);
                            it != _Context->m_Hierarchy.end(window->Docker);
                            it++)
                    {
                        (*it)->State.RenderingOrder = renderingOrder;
                    }
                    
                    window->State.RenderingOrder = ++renderingOrder;
                }
                else
                {
                    // setup maximum rendering order for docker snapper view
                    int renderingOrder = 0;

                    for(auto it  = _Context->m_Hierarchy.begin(window->DockerView);
                             it != _Context->m_Hierarchy.end(window->DockerView);
                             it++)
                    {
                        (*it)->State.RenderingOrder = renderingOrder;
                    }

                    window->SnapperView->State.RenderingOrder = ++renderingOrder;
                }
            }

            // reset all
            window->Activate         = false;
            window->ReattachChildren = false;
        }
    }
}

void ImmedidateUserInterfaceWindowController::frame_finish(ImmediateUserInterfaceContextLayer* _Context)
{
    for(auto node : _Context->m_NodesRenderingList)
    {
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(node);

        if(window == nullptr) continue;

        // detach all windows from closed window
        if(window->Opened != nullptr && !(*window->Opened))
        {
            auto& dockedWindows = _Context->get_controller<ImmedidateUserInterfaceWindowController>()->retrieve_docked_windows(
                _Context,
                window,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_All);

            for(auto dockedWindow : dockedWindows)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(dockedWindow);

                if(window == nullptr)
                    continue;

                window->Docker               = nullptr;
                window->TopSnapper           = nullptr;
                window->LeftSnapper          = nullptr;
                window->RightSnapper         = nullptr;
                window->BottomSnapper        = nullptr;
                window->DockingIndex         = -1;
                
                if(!(window->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ManualRenderingOrderSetup))
                    window->State.RenderingOrder = ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Main;
            }
        }
    }
}

void ImmedidateUserInterfaceWindowController::place_on_dockers(ImmediateUserInterfaceContextLayer* _Context)
{
    // read docking info
    if(!_Context->m_IniFileState.empty())
    {
        // collect all windows
        std::map<std::string, ImmediateUserInterfaceWindow*> windows;
        windows[""] = nullptr;

        for(auto node : _Context->m_NodesRenderingList)
        {
            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(node);

            if(window == nullptr) continue;

            windows[window->Hash] = window;
        }

        // restore docking
        for(auto node : _Context->m_NodesRenderingList)
        {
            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(node);

            if(window == nullptr) continue;

            ImmediateUserInterfaceWindow* docker        = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "Docker")];
            ImmediateUserInterfaceWindow* topSnapper    = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "TopSnapper")];
            ImmediateUserInterfaceWindow* LeftSnapper   = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "LeftSnapper")];
            ImmediateUserInterfaceWindow* RightSnapper  = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "RightSnapper")];
            ImmediateUserInterfaceWindow* BottomSnapper = windows[_Context->m_IniFileState.get<std::string>(window->Hash, "BottomSnapper")];

            if(docker && can_be_docked(_Context, docker, window))
                window->Docker = docker->DockerView;

            if(topSnapper && can_be_docked(_Context, topSnapper, window))
                window->TopSnapper = topSnapper->TopSnapperView;

            if(LeftSnapper && can_be_docked(_Context, LeftSnapper, window))
                window->LeftSnapper = LeftSnapper->LeftSnapperView;

            if(RightSnapper && can_be_docked(_Context, RightSnapper, window))
                window->RightSnapper = RightSnapper->RightSnapperView;

            if(BottomSnapper && can_be_docked(_Context, BottomSnapper, window))
                window->BottomSnapper = BottomSnapper->BottomSnapperView;

            if(_Context->m_IniFileState.get<bool>(window->Hash, "IsActive"))
                window->Activate = true;
        }
    }

    // analyze moved windows
    if(gs_vector_length(_Context->m_Input.get_cusor_drag_delta()) < 8.f)
        return;

    // find moved node and detach it from a docker
    ImmediateUserInterfaceWindow* moved = dynamic_cast<ImmediateUserInterfaceWindow*>(
        ImmediateUserInterfaceContextLayerHelpers::ImmedidateUserInterfaceMovedNodeSearcher().search(
            _Context,
            [](const ImmediateUserInterfaceNode*)->bool{return true;}));

    detach_from_docker(_Context, moved);

    // find top most hovered node not equal to the moved one
    ImmediateUserInterfaceNode* hoveredNode = 
        ImmediateUserInterfaceContextLayerHelpers::ImmedidateUserInterfaceHoveredNodeSearcher().search(_Context, [moved](const ImmediateUserInterfaceNode* _Node)->bool{return _Node != moved;});

    // detect hovered window
    ImmediateUserInterfaceWindow* hovered =
        _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceWindow>(hoveredNode);

    if(hovered == nullptr || moved == nullptr)
        return;

    // dock the moved window to the hovered one if all mouse buttons are released

    // compute gizmos
    gs_2dboxf dockingGizmo = gs_2dboxf(
        hovered->ContentBox.center() - gs_min(hovered->ContentBox.size().x, hovered->ContentBox.size().y) * 0.25f,
        hovered->ContentBox.center() + gs_min(hovered->ContentBox.size().x, hovered->ContentBox.size().y) * 0.25f);

    dockingGizmo = gs_2dboxf(
        dockingGizmo.Min,
        dockingGizmo.Min + gs_min(dockingGizmo.size().x, 256.f));

    dockingGizmo = gs_2dboxf(
        hovered->ContentBox.center() - dockingGizmo.size() * 0.5f,
        hovered->ContentBox.center() + dockingGizmo.size() * 0.5f);


    gs_vec2f gizmoSize = dockingGizmo.size() * 0.333f;

    gs_2dboxf topDockingGizmo = gs_2dboxf(
        dockingGizmo.Min + gs_vec2f(gizmoSize.x, 0.f),
        dockingGizmo.Min + gs_vec2f(gizmoSize.x * 2.f, gizmoSize.y));

    gs_2dboxf leftDockingGizmo = gs_2dboxf(
        dockingGizmo.Min + gs_vec2f(0.f, gizmoSize.y),
        dockingGizmo.Min + gs_vec2f(gizmoSize.x, gizmoSize.y * 2.f));

    gs_2dboxf rightDockingGizmo = gs_2dboxf(
        dockingGizmo.Min + gs_vec2f(gizmoSize.x * 2.f, gizmoSize.y),
        dockingGizmo.Min + gs_vec2f(gizmoSize.x * 3.f, gizmoSize.y * 2.f));

    gs_2dboxf bottomDockingGizmo = gs_2dboxf(
        dockingGizmo.Min + gs_vec2f(gizmoSize.x, gizmoSize.y * 2.f),
        dockingGizmo.Min + gs_vec2f(gizmoSize.x * 2.f, gizmoSize.y * 3.f));

    gs_2dboxf centralDockingGizmo = gs_2dboxf(
        dockingGizmo.center() - gizmoSize * 0.5f,
        dockingGizmo.center() + gizmoSize * 0.5f);

    if(!_Context->m_Input.is_mouse_button_down())
    {
        if(centralDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center);
        }
        if(topDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Top);
        }
        else if(leftDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Left);
        }
        else if(rightDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Right);
        }
        else if(bottomDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Bottom);
        }
    }
    else if(can_be_docked(_Context, moved, hovered))
    {
        // render potential docking window gizmo
        int depth = ImmediateUserInterfaceContextLayerHelpers::calculate_layer_depth(
            _Context,
            ImmedidateUserInterfaceRenderingLayer_::ImmedidateUserInterfaceRenderingLayer_Gizmos);

        if(dockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            _Context->m_Renderer->push_rectangle_rounded_filled(
                dockingGizmo.Min,
                dockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                gs_color_rgba(
                    gs_color_rgba_get_r(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    gs_color_rgba_get_g(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    gs_color_rgba_get_b(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    128),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                topDockingGizmo.Min,
                topDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                topDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                leftDockingGizmo.Min,
                leftDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                leftDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                rightDockingGizmo.Min,
                rightDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                rightDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                bottomDockingGizmo.Min,
                bottomDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                bottomDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));

            _Context->m_Renderer->push_rectangle_rounded_filled(
                centralDockingGizmo.Min,
                centralDockingGizmo.Max,
                _Context->m_Style.get_frames_radius(),
                centralDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++));
        }
    }
}

bool ImmedidateUserInterfaceWindowController::can_be_docked(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked)
{
    // general checks
    if(_Context == nullptr || _Docker  == nullptr || _Docked  == nullptr)
        return false;
    
    // check that we are already docked to a docker
    {
        ImmediateUserInterfaceNode* docker = _Context->m_Hierarchy.get_parent(_Docked);

        while (docker)
        {
            if(docker == _Docker)
                return false;
            docker = _Context->m_Hierarchy.get_parent(docker);
        }
    }

    // check that we are not trying to attach to self
    {
        ImmediateUserInterfaceNode* docker = _Docker;

        while (docker)
        {
            if(docker == _Docked)
                return false;
            docker = _Context->m_Hierarchy.get_parent(docker);
        }
    }

    return true;
}

void ImmedidateUserInterfaceWindowController::attach_to_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked, const ImmedidateUserInterfaceDockingAnchor& _Anchors)
{
    // auxiliary lambdas
    auto move_to_cache = [this](
        ImmediateUserInterfaceContextLayer* _Context,
        ImmediateUserInterfaceWindow*       _Docker)
    {
        if(_Context == nullptr || _Docker == nullptr)
            return;

        m_WindowsDockingCache.push_back(_Docker);
    };

    auto move_child_docked_windows_to_cache = [this](
        ImmediateUserInterfaceContextLayer*          _Context,
        ImmediateUserInterfaceNode*                  _Docker,
        const ImmedidateUserInterfaceDockingAnchor& _Orientation)
    {
        if(_Context == nullptr || _Docker == nullptr)
            return;

        auto& dockedWindows = retrieve_docked_windows(_Context, _Docker, _Orientation);
        for(auto it  = dockedWindows.begin(); it != dockedWindows.end(); it++)
            m_WindowsDockingCache.push_back(*it);
    };

    if(!can_be_docked(_Context, _Docker, _Docked))
        return;

    // get ready
    m_WindowsDockingCache.clear();
    m_WindowsDockingList.clear();

    // attach to a central part as a tab
    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center)
    {
        ImmediateUserInterfaceWindow * docker =
            _Docker->Docker ?
                ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, _Docker->Docker) :
                    _Docker;

        // move child docked windows and self to windows docking cache
        move_child_docked_windows_to_cache(_Context, docker, _Anchors);
        move_to_cache(_Context, _Docked);
        move_child_docked_windows_to_cache(_Context, _Docked, _Anchors);

        // reindex docked nodes and setup their docker
        int dockindex = 0;

        for(auto it = m_WindowsDockingCache.begin(); it != m_WindowsDockingCache.end(); it++)
        {
            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

            if(window == nullptr)
                continue;

            window->Docker = docker->DockerView;
            window->DockingIndex  = dockindex++;
        }

        // setup self as active
        _Docked->Activate = true;

        // clear
        m_WindowsDockingCache.clear();
        m_WindowsDockingList.clear();
        return;
    }

    // attach to top/left/right/bottom parts
    ImmediateUserInterfaceWindow* docker = _Docker;

    // move child docked windows and self to windows docking cache
    move_child_docked_windows_to_cache(_Context, docker, _Anchors);
    move_to_cache(_Context, _Docked);

    // reindex docked nodes and setup their docker
    int dockindex = 0;

    for(auto it = m_WindowsDockingCache.begin(); it != m_WindowsDockingCache.end(); it++)
    {
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

        if(window == nullptr)
            continue;

        if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Top)
            window->TopSnapper = docker->TopSnapperView;
        else if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Left)
            window->LeftSnapper = docker->LeftSnapperView;
        else if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Right)
            window->RightSnapper = docker->RightSnapperView;
        else if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Bottom)
            window->BottomSnapper = docker->BottomSnapperView;

        window->DockingIndex = dockindex++;
    }

    // clear
    m_WindowsDockingCache.clear();
    m_WindowsDockingList.clear();
}

void ImmedidateUserInterfaceWindowController::detach_from_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Detached)
{
    if(_Detached == nullptr)
        return;

    // reattach docked windows of detached window
    if(_Detached->ReattachChildren)
    {
        auto& dockedWindows = retrieve_docked_windows(
            _Context,
            _Detached,
            ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center);

        if(!dockedWindows.empty())
        {
            ImmediateUserInterfaceWindow* newDocker =
                dynamic_cast<ImmediateUserInterfaceWindow*>(dockedWindows[0]);

            int index = 0;
            for(auto it = dockedWindows.begin(); it != dockedWindows.end(); it++)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(*it);
                
                if(window == nullptr || window == newDocker)
                    continue;
                    
                window->Docker       = newDocker->DockerView;
                window->DockingIndex = index++;
            }

            newDocker->Docker        = nullptr;
            newDocker->TopSnapper    = nullptr;
            newDocker->LeftSnapper   = nullptr;
            newDocker->RightSnapper  = nullptr;
            newDocker->BottomSnapper = nullptr;
            newDocker->DockingIndex  = -1;

            if(_Detached->TopSnapper != nullptr)
                newDocker->TopSnapper = _Detached->TopSnapper;
            else if(_Detached->LeftSnapper != nullptr)
                newDocker->LeftSnapper = _Detached->LeftSnapper;
            else if(_Detached->RightSnapper != nullptr)
                newDocker->RightSnapper = _Detached->RightSnapper;
            else if(_Detached->BottomSnapper != nullptr)
                newDocker->BottomSnapper = _Detached->BottomSnapper;

            _Detached->Activate = true;
            newDocker->Activate = true;
        }
    }

    // setup active window within self
    auto& dockedWindows = retrieve_docked_windows(
        _Context,
        _Detached->Docker,
        ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center);

    if (dockedWindows.size() > 1)
    {
        dynamic_cast<ImmediateUserInterfaceWindow*>(dockedWindows[0])->IsActive = true;
    }
    else
    {
        ImmediateUserInterfaceWindow* docker =
            ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, _Detached->Docker);

        if(docker != nullptr)
            docker->IsActive = true;
    }

    // detach from docker
    _Detached->TopSnapper    = nullptr;
    _Detached->LeftSnapper   = nullptr;
    _Detached->RightSnapper  = nullptr;
    _Detached->BottomSnapper = nullptr;
    _Detached->Docker        = nullptr;
    _Detached->DockingIndex  = -1;
}

std::vector<ImmediateUserInterfaceNode*>& ImmedidateUserInterfaceWindowController::retrieve_docked_windows(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Docker, const ImmedidateUserInterfaceDockingAnchor& _Anchors)
{
    // get ready
    m_WindowsDockingList.clear();

    ImmediateUserInterfaceWindow* docker =
        dynamic_cast<ImmediateUserInterfaceWindow*>(
            ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, _Docker));

    if(docker == nullptr)
        return m_WindowsDockingList;

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Top)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->TopSnapperView); it != _Context->m_Hierarchy.end(docker->TopSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Left)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->LeftSnapperView); it != _Context->m_Hierarchy.end(docker->LeftSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Right)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->RightSnapperView); it != _Context->m_Hierarchy.end(docker->RightSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Bottom)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->BottomSnapperView); it != _Context->m_Hierarchy.end(docker->BottomSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    if(_Anchors & ImmedidateUserInterfaceDockingAnchor_::ImmedidateUserInterfaceDockingAnchor_Center)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->DockerView); it != _Context->m_Hierarchy.end(docker->DockerView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                m_WindowsDockingList.push_back(*it);
        }
    }

    return m_WindowsDockingList;
}

// ImmedidateUserInterfaceEventsController
ImmedidateUserInterfaceInputController::ImmedidateUserInterfaceInputController(){}
ImmedidateUserInterfaceInputController::~ImmedidateUserInterfaceInputController(){}

void ImmedidateUserInterfaceInputController::frame_debug(ImmediateUserInterfaceContextLayer* _Context)
{
    // main code
    if(_Context == nullptr)
        return;

    // find the top most node catching the mouse cursor
    ImmediateUserInterfaceNode* hoveredNode  = nullptr;
    ImmediateUserInterfaceNode* eventNode    = nullptr;
    int                         maximumDepth = INT_MIN;

    for (auto& node : _Context->m_NodesRenderingList)
    {
        if(_Context->m_Input.is_mouse_button_pressed())
        {
            // deselect node on mouse press
            node->State.Selected = false;

            // setup default rendering order for all singletone nodes
            if(!(node->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ManualRenderingOrderSetup) &&
                _Context->m_Hierarchy.get_parent(node) == nullptr)
            {
                node->State.RenderingOrder =
                    ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Main;
            }
        }

        // unhover invisible node
        if(!node->is_partially_visible(_Context))
        {
            node->State.MouseHover = ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None;
            continue;
        }

        // find node catching events
        if(eventNode == nullptr && node->State.Events != ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
            eventNode = node;

        // hover end logic
        if(!node->get_visible_rect(_Context).contains(_Context->m_Input.get_cusor_position()))
        {
            if(!(node->Cache.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseLeft))
            {
                node->State.MouseLeaveTimer = Frenchie::Core::tic();
                node->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseLeft;
            }
            else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
                node->State.MouseLeaveTimer,Frenchie::Core::tic()) > 200.f) // TODO: this MUST BE A SETTING !!!!
            {
                node->State.MouseHover = ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None;
            }

            continue;
        }

        if(node->Cache.Depth > maximumDepth)
        {
            maximumDepth = node->Cache.Depth;
            hoveredNode  = node;
        }
    }

    // process hovered node
    if(hoveredNode != nullptr)
    {
        // highlight hovered node
        if((_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes))
        {
            int depth = ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node(hoveredNode);

            _Context->m_Renderer->push_rectangle_rounded(
                hoveredNode->get_visible_rect(_Context).Min,
                hoveredNode->get_visible_rect(_Context).Max,
                _Context->m_Style.get_frames_radius(),
                _Context->m_Style.get_frames_width(),
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth));
        }

        // start hover node
        if(!(hoveredNode->Cache.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseEntered))
        {
            hoveredNode->State.MouseEnterTimer = Frenchie::Core::tic();
            hoveredNode->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseEntered;
        }
        else if(Frenchie::Core::elapsed<std::chrono::milliseconds>(
            hoveredNode->State.MouseEnterTimer,
            Frenchie::Core::tic()) > 10.0) // TODO: this MUST BE A SETTING !!!
        {
            // make this node hovered
            hoveredNode->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered;

            // the only one node can be hovered
            for (auto& node : _Context->m_NodesRenderingList)
            {
                if(node != hoveredNode)
                    node->State.MouseHover = ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None;
            }

            // select this node on mouse click
            if(_Context->m_Input.is_mouse_button_pressed())
                hoveredNode->State.Selected = true;
        }
    }

    // catch events
    if(eventNode != nullptr)
        eventNode->Dirty = true;

    ImmediateUserInterfaceNode* eventCatcher = eventNode != nullptr ? eventNode : hoveredNode;

    if(eventCatcher != nullptr)
    {
        eventCatcher->events(_Context);

        // check in-parent intersection and process events of intersected nodes
        for (auto it  = _Context->m_Hierarchy.begin(_Context->m_Hierarchy.get_parent(eventCatcher));
                  it != _Context->m_Hierarchy.end(_Context->m_Hierarchy.get_parent(eventCatcher));
                  it++)
        {
            if((*it) == eventCatcher)
                continue;

            float offset = gs_max(_Context->m_Renderer->get_minimum_line_width(), _Context->m_Style.get_frames_width());

            if(gs_2dboxf(
                (*it)->get_visible_rect(_Context).Min - gs_vec2f(offset, offset),
                (*it)->get_visible_rect(_Context).Max + gs_vec2f(offset, offset)).contains(_Context->m_Input.get_cusor_position()))
            {
                // process events
                (*it)->events(_Context);

                // reset event loop if events of parent and it's children are different
                if((*it)->State.Events != eventCatcher->State.Events)
                    (*it)->State.Events = ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None;
            }
        }

        // pass focus
        if(_Context->m_Input.is_mouse_button_pressed() || eventCatcher->State.Events != ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
        {
            // find top most relative of event catcher node
            ImmediateUserInterfaceNode* relative = eventCatcher;
            ImmediateUserInterfaceNode* focused  = eventCatcher;
            ImmediateUserInterfaceNode* parent   = eventCatcher->State.Scope;

            while (parent)
            {
                relative = parent;
                parent   = parent->State.Scope;
            }

            // find top most parent of event catcher relative node
            focused = relative;
            parent  = _Context->m_Hierarchy.get_parent(relative);

            while (parent)
            {
                focused = parent;
                parent  = _Context->m_Hierarchy.get_parent(parent);
            }

            if(!(focused->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ManualRenderingOrderSetup))
                focused->State.RenderingOrder = ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Focus;
        }
    }
}

// ImmedidateUserInterfaceLayoutController
ImmedidateUserInterfaceLayoutController::ImmedidateUserInterfaceLayoutController(){}
ImmedidateUserInterfaceLayoutController::~ImmedidateUserInterfaceLayoutController(){}

void ImmedidateUserInterfaceLayoutController::frame_debug(ImmediateUserInterfaceContextLayer* _Context)
{
    // layout
    for (auto& singleton : _Context->m_Hierarchy.Singletons)
        node_layout(_Context, singleton);
}

void ImmedidateUserInterfaceLayoutController::node_layout(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
{
    if(_Context == nullptr || _Node == nullptr)
        return;

    _Node->layout(_Context);
    _Node->measure(_Context);

    _Node->Dirty = false;

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
        node_layout(_Context, (*it));
}

ImmedidateUserInterfaceRenderingController::ImmedidateUserInterfaceRenderingController(){}
ImmedidateUserInterfaceRenderingController::~ImmedidateUserInterfaceRenderingController(){}
void ImmedidateUserInterfaceRenderingController::frame_render(ImmediateUserInterfaceContextLayer* _Context)
{
    // get ready
    m_NodesRenderingCache.clear();

    std::stable_sort(
        _Context->m_Hierarchy.Singletons.begin(),
        _Context->m_Hierarchy.Singletons.end(),
        [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
        {
            return _A->State.RenderingOrder < _B->State.RenderingOrder;
        });
    
    // render singletones
    for (auto& singleton : _Context->m_Hierarchy.Singletons)
    {
        for (auto& renderedNode : m_NodesRenderingCache)
        {
            int depth =
                gs_max(
                    renderedNode->State.MaximumChildDepth + renderedNode->State.MaximumChildThickness - renderedNode->State.Depth,
                    renderedNode->State.MaximumChildDepth + renderedNode->State.SelfThickness + 1);

            singleton->State.Depth =
                gs_max(
                    depth,
                    singleton->State.Depth);
        }

        ImmedidateUserInterfaceRenderingController::render_node(_Context, singleton);
        m_NodesRenderingCache.push_back(singleton);
    }

    #ifdef IMMEDIATE_USER_INTERFACE_DEBUG

    std::cout << "\n\n\n";
    std::cout << "------------------------------------------------------------------------------------------------\n";
    std::cout << "HIERARCHY:\n";
    std::cout << "------------------------------------------------------------------------------------------------\n";
    for (auto& singleton : _Context->m_Hierarchy.Singletons)
        showHierarchy(_Context, singleton, "\t");
    #endif

    // clean-up
    m_NodesRenderingCache.clear();
}

void ImmedidateUserInterfaceRenderingController::render_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
{
    if(_Node == nullptr || !_Node->is_partially_visible(_Context)) return;

    // calculate clippingbox
    _Context->m_Renderer->push_clip_box(_Node->get_clipping_box(_Context));

    // render self
    _Node->render(_Context);

    // render children
    if(_Node->State.OrderChildrenWhileRendering)
    {
        std::stable_sort(
            _Context->m_Hierarchy.begin(_Node),
            _Context->m_Hierarchy.end(_Node),
            [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
            {
                return _A->State.RenderingOrder < _B->State.RenderingOrder;
            });
    }

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
    {
        int depth = gs_max(
            _Node->State.MaximumChildDepth + _Node->State.MaximumChildThickness - _Node->State.Depth,
            _Node->State.MaximumChildDepth + _Node->State.SelfThickness + 1,
            _Node->State.Depth + _Node->State.SelfThickness + 1);

        (*it)->State.Depth = _Node->State.PlaceInFollow ? depth : _Node->State.Depth + _Node->State.SelfThickness + 1;

        render_node(_Context, (*it));

        _Node->State.MaximumChildDepth     = gs_max(_Node->State.MaximumChildDepth, (*it)->State.Depth);
        _Node->State.MaximumChildThickness = gs_max(_Node->State.MaximumChildThickness, (*it)->State.SelfThickness);
    }

    _Node->State.MaximumChildThickness = gs_max(_Node->State.MaximumChildThickness, _Node->State.SelfThickness);

    // update parent maximum child depth and maximum child thickness
    auto parent = _Context->m_Hierarchy.get_parent(_Node);

    while (parent)
    {
        parent->State.MaximumChildDepth     = gs_max(parent->State.MaximumChildDepth, _Node->State.MaximumChildDepth);
        parent->State.MaximumChildThickness = gs_max(parent->State.MaximumChildThickness, _Node->State.MaximumChildThickness);
        parent = _Context->m_Hierarchy.get_parent(parent);
    }            

    // remove clipping
    _Context->m_Renderer->pop_clip_box();
}

ImmedidateUserInterfaceMenusController::ImmedidateUserInterfaceMenusController(){}
ImmedidateUserInterfaceMenusController::~ImmedidateUserInterfaceMenusController(){}

void ImmedidateUserInterfaceMenusController::frame_finish(ImmediateUserInterfaceContextLayer* _Context)
{
    ActiveMenus.clear();

    // layout menu actions
    for(auto node : _Context->m_NodesRenderingList)
    {
        ImmediateUserInterfaceMenu* menu =
            dynamic_cast<ImmediateUserInterfaceMenu*>(node);

        if(menu != nullptr)
        {
            // look for parental scroll bar
            ImmediateUserInterfaceScrollArea* scroll = nullptr;
            ImmediateUserInterfaceScrollArea* parent = _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceScrollArea>(menu);

            while (parent != nullptr)
            {
                scroll = parent;
                parent = _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceScrollArea>(parent);
            }
            
            if(scroll != nullptr)
            {
                // manage internal scroll area
                float internal = 0.f;
                detect_maximum_width(_Context, scroll, internal);
                detect_maximum_width(_Context, menu->InternalScrollArea, internal);
                setup_maximum_with(_Context, scroll, internal);
                setup_maximum_with(_Context, menu->InternalScrollArea, internal);

                // manage external scroll area
                float external = 0.f;
                detect_maximum_width(_Context, menu->ExternalScrollArea, external);
                setup_maximum_with(_Context, menu->ExternalScrollArea, external);
            }
            else
            {
                // manage internal scroll area
                float internal = 0.f;
                detect_maximum_width(_Context, menu->InternalScrollArea, internal);
                setup_maximum_with(_Context, menu->InternalScrollArea, internal);

                // manage external scroll area
                float external = 0.f;
                detect_maximum_width(_Context, menu->ExternalScrollArea, external);
                setup_maximum_with(_Context, menu->ExternalScrollArea, external);
            }
        }

        // collect active menus
        if(!(node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered)) continue;

        ImmediateUserInterfaceNode* relative = node;

        while (relative != nullptr)
        {
            ImmediateUserInterfaceMenu* menu =
                dynamic_cast<ImmediateUserInterfaceMenu*>(relative);

            if(menu != nullptr)
                ActiveMenus.push_back(menu);

            relative = relative->State.Scope;
        }
    }
}

void ImmedidateUserInterfaceMenusController::detect_maximum_width(
    ImmediateUserInterfaceContextLayer* _Context,
    ImmediateUserInterfaceNode*         _Node,
    float&                              _MaximumWidth)
{
    if(_Context == nullptr) return;

    ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Node);

    if(scrollArea != nullptr && scrollArea->ContentView != nullptr)
    {
        for(auto it = _Context->m_Hierarchy.begin(scrollArea->ContentView); it != _Context->m_Hierarchy.end(scrollArea->ContentView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceMenuAction*>(*it) != nullptr)
            {
                gs_vec2f size =
                    _Context->m_Renderer->calculate_bounding_box(
                        (*it)->Name.begin(),
                        (*it)->Name.end(),
                        _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() + gs_vec2f(_Context->m_Style.get_font_size() * 2.f, _Context->m_Style.get_font_size() * 0.5f);

                _MaximumWidth = gs_max(_MaximumWidth, size.x);
            }
        }
    }

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
        detect_maximum_width(_Context, *it, _MaximumWidth);
}

void ImmedidateUserInterfaceMenusController::setup_maximum_with(
    ImmediateUserInterfaceContextLayer* _Context,
    ImmediateUserInterfaceNode*         _Node,
    float&                              _MaximumWidth)
{
    if(_Context == nullptr) return;

    ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Node);

    if(scrollArea != nullptr && scrollArea->ContentView != nullptr)
    {
        for(auto it = _Context->m_Hierarchy.begin(scrollArea->ContentView); it != _Context->m_Hierarchy.end(scrollArea->ContentView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceMenuAction*>(*it) != nullptr)
            {
                gs_vec2f size =
                    _Context->m_Renderer->calculate_bounding_box(
                        (*it)->Name.begin(),
                        (*it)->Name.end(),
                        _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() + gs_vec2f(_Context->m_Style.get_font_size() * 2.f, _Context->m_Style.get_font_size() * 0.5f);

                (*it)->State.BoundingBox =
                    gs_2dboxf(
                        (*it)->State.BoundingBox.Min,
                        (*it)->State.BoundingBox.Min + gs_vec2f(_MaximumWidth, size.y));
            }
        }
    }

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
        setup_maximum_with(_Context, *it, _MaximumWidth);
}

// ImmedidateUserInterfaceNextNodeController
ImmedidateUserInterfaceNextNodeController::ImmedidateUserInterfaceNextNodeController(){}
ImmedidateUserInterfaceNextNodeController::~ImmedidateUserInterfaceNextNodeController(){}

void ImmedidateUserInterfaceNextNodeController::frame_start(ImmediateUserInterfaceContextLayer*)
{
    // reset all
    reset();
}

void ImmedidateUserInterfaceNextNodeController::reset()
{
    // reset all
    NextLine.reset();
    NextIndent.reset();
    NextPosition.reset();
    NextMinimumSize.reset();
    NextMaximumSize.reset();
    NextContentMargin.reset();
    NextContentPadding.reset();
}

// ImmediateUserInterfaceScrollBarsController
ImmediateUserInterfaceScrollBarsController::ImmediateUserInterfaceScrollBarsController(){}
ImmediateUserInterfaceScrollBarsController::~ImmediateUserInterfaceScrollBarsController(){}
void ImmediateUserInterfaceScrollBarsController::frame_debug(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    ImmediateUserInterfaceNode* hoveredNode =
        ImmediateUserInterfaceContextLayerHelpers::ImmedidateUserInterfaceHoveredNodeSearcher().search(
            _Context,
            [](const ImmediateUserInterfaceNode* _Node)->bool{return dynamic_cast<const ImmediateUserInterfaceScrollArea*>(_Node);});

    ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(hoveredNode);
    
    if(scrollArea == nullptr ||
        !scrollArea->State.BoundingBox.contains(_Context->m_Input.get_cusor_position())) return;

    // adjust vertical scroll bar by mouse wheel
    if((scrollArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment))
    {
        ImmediateUserInterfaceScrollAreaScrollBar* verticalScrollBar =
            scrollArea != nullptr ? scrollArea->VerticalScrollBar : nullptr; 

        if(verticalScrollBar != nullptr)
        {
            if(gs_vector_length(_Context->m_Input.get_cusor_scroll_offset()) > 0.f)
            {
                verticalScrollBar->set_scroll_offset(
                    _Context->m_Input.get_cusor_scroll_offset() * (-1.f) * gs_min(scrollArea->State.ContentSize.y, scrollArea->State.BoundingBox.size().y) * 0.05f);
            }
        }
    }

    // adjust vertical scrollbar by arrow keys
    if((scrollArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment))
    {
        ImmediateUserInterfaceScrollAreaScrollBar* verticalScrollBar =
            scrollArea != nullptr ? scrollArea->VerticalScrollBar : nullptr; 

        if(verticalScrollBar != nullptr)
        {
            if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow) ||
                _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow))
            {
                gs_vec2f offset =
                    !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow) ?
                        (-1.f) * gs_min(scrollArea->State.ContentSize.y, scrollArea->State.BoundingBox.size().y) * 0.05f :
                            (-4.f);

                verticalScrollBar->set_scroll_offset(offset);
            }
            else if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow) ||
                    _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow))
            {
                gs_vec2f offset =
                    !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow) ?
                        (+1.f) * gs_min(scrollArea->State.ContentSize.y, scrollArea->State.BoundingBox.size().y) * 0.05f :
                            (+4.f);

                verticalScrollBar->set_scroll_offset(offset);
            }
        }
    }

    // adjust horizontal scrollbar by arrow keys
    if((scrollArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment))
    {
        ImmediateUserInterfaceScrollAreaScrollBar* horizontalScrollBar =
            scrollArea != nullptr ? scrollArea->HorizontalScrollBar : nullptr; 

        if(horizontalScrollBar != nullptr)
        {
            if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow) ||
                _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow))
            {
                gs_vec2f offset =
                    !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow) ?
                        (-1.f) * gs_min(scrollArea->State.ContentSize.x, scrollArea->State.BoundingBox.size().x ) * 0.05f :
                            (-4.f);

                horizontalScrollBar->set_scroll_offset(offset);
            }
            else if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ||
                    _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow))
            {
                gs_vec2f offset =
                    !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ?
                        (+1.f) * gs_min(scrollArea->State.ContentSize.x, scrollArea->State.BoundingBox.size().x) * 0.05f :
                            (+4.f);

                horizontalScrollBar->set_scroll_offset(offset);
            }
        }
    }
}

// ImmediateUserInterfaceContextLayer2
ImmediateUserInterfaceContextLayer::ImmediateUserInterfaceContextLayer(){}
ImmediateUserInterfaceContextLayer::~ImmediateUserInterfaceContextLayer(){}

bool ImmediateUserInterfaceContextLayer::awake()
{
    // launch renderer
    if(m_Renderer == nullptr)
        m_Renderer = application()->push_layer<RenderingQueue>();

    // create hierarchy
    m_Hierarchy = ImmedidateUserInterfaceHierarchy(
        [](const ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
        {
            if(_Node == nullptr)
                return nullptr;

            if(_Node->State.Parent != nullptr)
                return _Node->State.Parent;

            const ImmediateUserInterfaceWindow* window =
                dynamic_cast<const ImmediateUserInterfaceWindow*>(_Node);

            if(window != nullptr && window->Docker != nullptr)
                return window->Docker;

            if(window != nullptr && window->TopSnapper != nullptr)
                return window->TopSnapper;

            if(window != nullptr && window->LeftSnapper != nullptr)
                return window->LeftSnapper;

            if(window != nullptr && window->RightSnapper != nullptr)
                return window->RightSnapper;

            if(window != nullptr && window->BottomSnapper != nullptr)
                return window->BottomSnapper;

            return nullptr;
        });

    // create controllers
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceWindowController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceInputController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceScrollBarsController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceLayoutController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceRenderingController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceNextNodeController>());
    m_Controllers.push_back(std::make_unique<ImmedidateUserInterfaceMenusController>());

    // awake controllers
    for(auto& controller : m_Controllers)
        GS_ASSERT(controller->awake(this));

    // load .ini file
    m_IniFileState.read(m_IniFilePath);

    // create input handler
    m_Input = ImmedidateUserInterfaceInput(this);

    return m_Renderer != nullptr;
}

void ImmediateUserInterfaceContextLayer::frame_start()
{
    for(auto& controller : m_Controllers)
        controller->frame_start(this);
}

void ImmediateUserInterfaceContextLayer::frame_update()
{
}

void ImmediateUserInterfaceContextLayer::frame_input()
{
    // build hierarchy
    m_Hierarchy.build(m_NodesRenderingList);

    // execute controllers
    for(auto& controller : m_Controllers)
        controller->frame_debug(this);
}

void ImmediateUserInterfaceContextLayer::frame_render()
{
    for(auto& controller : m_Controllers)
        controller->frame_render(this);
}

void ImmediateUserInterfaceContextLayer::frame_finish()
{
    // save state
    if(application()->is_closed())
    {
        for (auto node : m_NodesRenderingList)
            node->save_state(this);
        m_IniFileState.write(m_IniFilePath);
    }

    // process controllers
    for(auto& controller : m_Controllers)
        controller->frame_finish(this);

    // save state
    for (auto& node : m_NodesRenderingList)
    {
        // stop all modifications
        if(!m_Input.is_mouse_button_down())
            node->State.Events = ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None;

        // save cache
        node->Cache.Events        = node->State.Events;
        node->Cache.MouseHover    = node->State.MouseHover;
        node->Cache.SelfThickness = node->State.SelfThickness;
        node->Cache.Depth         = node->State.Depth;
        node->Cache.Parent        = node->State.Parent;
        
        if(node->State.Events == ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
            node->Cache = node->State;

        // restore
        node->State.ClippingBox.reset();

        node->State.Depth                 = 0;
        node->State.SelfThickness         = 0;
        node->State.RenderingIndex        = 0;
        node->State.MaximumChildDepth     = 0;
        node->State.MaximumChildThickness = 0;
        node->State.Settings              = 0;
        node->Count                       = 0;
    }

    // check rendering stack
    GS_ASSERT(m_NodesRenderingStack.empty());

    // clean-up rendering data
    m_NodesRenderingList.clear();
    m_NodesRenderingStack.clear();
    m_NodesRenderedStack.clear();

    // clear ini file state
    m_IniFileState.clear();
}

void ImmediateUserInterfaceContextLayer::finish()
{
    // close renderer
    if(m_Renderer != nullptr)
        m_Renderer->close();
}

bool ImmediateUserInterfaceContextLayer::begin_scrollarea(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceScrollArea>(_ID, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_scrollarea()
{
    end_node<ImmediateUserInterfaceScrollArea>();
}

bool ImmediateUserInterfaceContextLayer::begin_panel(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceNodePanel>(_ID, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_panel()
{
    end_node<ImmediateUserInterfaceNodePanel>();
}

bool ImmediateUserInterfaceContextLayer::begin_vertical_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceNodeVerticalStack>(_Name, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_vertical_stack()
{
    end_node<ImmediateUserInterfaceNodeVerticalStack>();
}

bool ImmediateUserInterfaceContextLayer::begin_horizontal_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceNodeHorizontalStack>(_Name, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_horizontal_stack()
{
    end_node<ImmediateUserInterfaceNodeHorizontalStack>();
}

bool ImmediateUserInterfaceContextLayer::push_button(const std::string& _ID)
{
    struct ImmediateUserInterfacePushButton : public ImmediateUserInterfaceNode
    {
    public:
        ImmediateUserInterfacePushButton(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
        {
            State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 128.f));
        }
        
        virtual ~ImmediateUserInterfacePushButton(){}
    };

    if(begin_node<ImmediateUserInterfacePushButton>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfacePushButton* widget = get_rendering_stack_top<ImmediateUserInterfacePushButton>();
        gs_vec2f textSize = m_Renderer->calculate_bounding_box(widget->Name.begin(), widget->Name.end(), m_Style.get_font_size(), m_Style.get_current_font()).size();

        // render
        if(!widget->Dirty)
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            int depth = widget->Cache.Depth;

            // background
            m_Renderer->push_rectangle_rounded_filled(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Max,
                m_Style.get_frames_radius(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                m_Renderer->calculate_transform_matrix((float)depth++));

            if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down())
            {
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }
            else
            {
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                    m_Style.get_frames_radius(),
                    (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }

            // title
            m_Renderer->push_text(

                widget->State.BoundingBox.center() - textSize * 0.5f, // text is aligned on center of the push button

                widget->Name.begin(),
                widget->Name.end(),
                m_Style.get_font_size(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                m_Renderer->calculate_transform_matrix((float)depth++));
        
            m_Renderer->pop_clip_box();
        }

        // calculate geometry
        {
            widget->State.MinimumSize = gs_vec2f(textSize.x, gs_max(textSize.y, m_Style.get_font_size()));
            widget->State.MaximumSize = gs_vec2f((float)INT_MAX, (float)INT_MAX);

            widget->State.BoundingBox = gs_2dboxf(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Min + gs_clamp(widget->State.BoundingBox.size(), widget->State.MinimumSize, widget->State.MaximumSize));
        }

        end_node<ImmediateUserInterfacePushButton>();

        return (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_clicked();
    }

    return false;
}

bool ImmediateUserInterfaceContextLayer::check_button(
    const std::string&                               _ID,
    bool&                                            _Checked,
    const ImmediateUserInterfaceCheckButtonSettings& _Settings)
{
    if(begin_node<ImmediateUserInterfaceNode>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        // setup
        ImmediateUserInterfaceNode* widget =
            get_rendering_stack_top<ImmediateUserInterfaceNode>();
        
        // event processing
        {
            if( (_Settings                & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable) &&
                (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered)        &&
                m_Input.is_mouse_button_clicked())
            {
                _Checked = !_Checked;
            }
        }

        // render
        if(!widget->Dirty)
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            int depth = widget->Cache.Depth;

            // render checkbox
            if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox)
            {
                // background
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Max,
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                    m_Renderer->calculate_transform_matrix((float)depth++));

                if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down())
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
                else
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }

                // tick
                if(_Checked)
                {
                    gs_vec2f start = gs_vec2f(
                        widget->State.BoundingBox.center().x,
                        widget->State.BoundingBox.center().y + widget->State.BoundingBox.height() * 0.5f * 0.5f);

                    m_Renderer->push_line(
                        start,
                        gs_vec2f(
                            widget->State.BoundingBox.center().x - widget->State.BoundingBox.width() * 0.5f * 0.7f,
                            widget->State.BoundingBox.center().y - widget->State.BoundingBox.height() * 0.5f * 0.25f),
                        m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));

                    m_Renderer->push_line(
                        start,
                        gs_vec2f(
                            widget->State.BoundingBox.center().x + widget->State.BoundingBox.width() * 0.5f * 0.7f,
                            widget->State.BoundingBox.center().y - widget->State.BoundingBox.height() * 0.5f * 0.9f),
                        m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
            }
            
            // render radio button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton)
            {
                // background
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Max,
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                    m_Renderer->calculate_transform_matrix((float)depth++));

                if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down())
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
                else
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }

                if(_Checked)
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width() * 3.f,
                        widget->State.BoundingBox.Max - m_Style.get_frames_width() * 3.f,
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
            }
            
            // render slider button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton)
            {
                // background
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Max,
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                    m_Renderer->calculate_transform_matrix((float)depth++));

                if(_Checked)
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min,
                        widget->State.BoundingBox.Max,
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                        m_Renderer->calculate_transform_matrix((float)depth++));

                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Max - m_Style.get_font_size() + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
                else
                {
                    m_Renderer->push_rectangle_rounded_filled(
                        widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                        widget->State.BoundingBox.Min + m_Style.get_font_size() - m_Style.get_frames_width(),
                        m_Style.get_frames_radius(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
            }
        
            m_Renderer->pop_clip_box();
        }

        // layout geometry
        {
            // layout checkbox
            if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox)
            {
                widget->State.MinimumSize = m_Style.get_font_size() + m_Style.get_frames_width();
                widget->State.MaximumSize = widget->State.MinimumSize;

                widget->State.BoundingBox = gs_2dboxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + widget->State.MaximumSize);
            }
            // layout radio button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton)
            {
                widget->State.MinimumSize = m_Style.get_font_size() + m_Style.get_frames_width();
                widget->State.MaximumSize = widget->State.MinimumSize;

                widget->State.BoundingBox = gs_2dboxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + widget->State.MaximumSize);
            }
            // layout slider button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton)
            {
                widget->State.MinimumSize = gs_vec2f((m_Style.get_font_size() + m_Style.get_frames_width()) * 2.f, m_Style.get_font_size() + m_Style.get_frames_width());
                widget->State.MaximumSize = widget->State.MinimumSize;

                widget->State.BoundingBox = gs_2dboxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + widget->State.MaximumSize);
            }
        }

        end_node<ImmediateUserInterfaceNode>();
        
        return _Checked;
    }

    return false;
}

bool ImmediateUserInterfaceContextLayer::menu_action(const std::string& _ID)
{
    if(begin_node<ImmediateUserInterfaceMenuAction>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceMenuAction* widget =
            get_rendering_stack_top<ImmediateUserInterfaceMenuAction>();

        end_node<ImmediateUserInterfaceMenuAction>();

        return (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_clicked();
    }

    return false;
}

bool ImmediateUserInterfaceContextLayer::combobox_item(const std::string& _ID)
{
    if(begin_node<ImmediateUserInterfaceComboboxItem>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceComboboxItem* widget =
            get_rendering_stack_top<ImmediateUserInterfaceComboboxItem>();

        end_node<ImmediateUserInterfaceComboboxItem>();

        return (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_clicked();
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::label(
    const std::string&                         _ID,
    const std::string&                         _Text,
    const ImmediateUserInterfaceLabelSettings& _Settings)
{
    // auxiliary types
    struct ImmediateUserInterfaceLabel : public ImmediateUserInterfaceNode
    {
    public:
        ImmediateUserInterfaceLabel(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
        {
            State.BoundingBox = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
        }

        virtual ~ImmediateUserInterfaceLabel(){}

        virtual void layout(ImmediateUserInterfaceContextLayer*) override{}
        virtual void render(ImmediateUserInterfaceContextLayer*) override{}
    };

    if(begin_node<ImmediateUserInterfaceLabel>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        // setup
        ImmediateUserInterfaceLabel* widget   = get_rendering_stack_top<ImmediateUserInterfaceLabel>();
        gs_vec2f                     textSize = m_Renderer->calculate_bounding_box(_Text.begin(), _Text.end(), m_Style.get_font_size(), m_Style.get_current_font()).size();

        // render
        if(!widget->Dirty)
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            int depth = widget->Cache.Depth;

            if((_Settings & ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_RenderFrame))
            {
                // outline
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Max,
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
                    m_Renderer->calculate_transform_matrix((float)depth++));

                // background
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }
            else
            {
                // background
                m_Renderer->push_rectangle_rounded_filled(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Max,
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }

            m_Renderer->push_text(
                gs_vec2f(widget->State.BoundingBox.Min.x, widget->State.BoundingBox.center().y - textSize.y * 0.5f),
                _Text.begin(),
                _Text.end(),
                m_Style.get_font_size(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                m_Renderer->calculate_transform_matrix((float)depth++),
                m_Style.get_current_font());

            m_Renderer->pop_clip_box();
        }

        // calculate geometry
        {
            widget->State.MinimumSize = gs_vec2f(gs_max(textSize.x, widget->State.MinimumSize.x), gs_max(textSize.y, m_Style.get_font_size()));
            widget->State.MaximumSize = gs_vec2f(gs_max(widget->State.MaximumSize.x, widget->State.MinimumSize.x), widget->State.MinimumSize.y);

            widget->State.BoundingBox = gs_2dboxf(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Min + gs_clamp(widget->State.BoundingBox.size(), widget->State.MinimumSize, widget->State.MaximumSize));
        }

        end_node<ImmediateUserInterfaceLabel>();
    }
}

bool ImmediateUserInterfaceContextLayer::input_string_multiline(
    const std::string&                               _ID,
    std::string&                                     _Text,
    const ImmediateUserInterfaceInputStringSettings& _Settings,
    bool                                           (*_InputTextFilter)(const std::string&))
{
    return input_string_internal(
        this,
        _ID,
        _Text,

        // text input settings
        _Settings,

        // interanl settings
        0,

        // widget settings
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar             |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar           |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment,
        [_InputTextFilter](const std::string& _Input)->bool{return _InputTextFilter == nullptr || _InputTextFilter(_Input);});
}

bool ImmediateUserInterfaceContextLayer::input_string_singleline(
    const std::string&                               _ID,
    std::string&                                     _Text,
    const ImmediateUserInterfaceInputStringSettings& _Settings,
    bool                                           (*_InputTextFilter)(const std::string&))
{
    return input_string_internal(
        this,
        _ID,
        _Text,

        // text input settings
        _Settings,
        
        // interanl settings
        ImmediateUserInterfaceInputStringInternalSettings_::ImmediateUserInterfaceInputStringInternalSettings_NoMultiline,

        // widget settgins
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically   |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar       |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar       |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar  |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar,
        [_InputTextFilter](const std::string& _Input)->bool{ return _InputTextFilter == nullptr || _InputTextFilter(_Input);});
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar<float>(const std::string& _ID, float& _Input, const float& _Min, const float& _Max, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_internal<float>(this, _ID, _Input, _Min, _Max, "%.5f", _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar<double>(const std::string& _ID, double& _Input, const double& _Min, const double& _Max, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_internal<double>(this, _ID, _Input, _Min, _Max, "%.5f", _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar<long double>(const std::string& _ID, long double& _Input, const long double& _Min, const long double& _Max, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_internal<long double>(this, _ID, _Input, _Min, _Max, "%.5f", _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar<int>(const std::string& _ID, int& _Input, const int& _Min, const int& _Max, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_internal<int>(this, _ID, _Input, _Min, _Max, "%d", _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar<short>(const std::string& _ID, short& _Input, const short& _Min, const short& _Max, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_internal<short>(this, _ID, _Input, _Min, _Max, "%d", _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar<unsigned short>(const std::string& _ID, unsigned short& _Input, const unsigned short& _Min, const unsigned short& _Max, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_internal<unsigned short>(this, _ID, _Input, _Min, _Max, "%d", _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar<unsigned int>(const std::string& _ID, unsigned int& _Input, const unsigned int& _Min, const unsigned int& _Max, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_internal<unsigned int>(this, _ID, _Input, _Min, _Max, "%u", _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar_slider<float>(const std::string& _ID, float& _Input, const float& _Min, const float& _Max, const int& _Delta, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_slider_internal<float>(this, _ID, _Input, _Min, _Max, _Delta, _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar_slider<double>(const std::string& _ID, double& _Input, const double& _Min, const double& _Max, const int& _Delta, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_slider_internal<double>(this, _ID, _Input, _Min, _Max, _Delta, _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar_slider<long double>(const std::string& _ID, long double& _Input, const long double& _Min, const long double& _Max, const int& _Delta, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_slider_internal<long double>(this, _ID, _Input, _Min, _Max, _Delta, _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar_slider<int>(const std::string& _ID, int& _Input, const int& _Min, const int& _Max, const int& _Delta, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_slider_internal<int>(this, _ID, _Input, _Min, _Max, _Delta, _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar_slider<short>(const std::string& _ID, short& _Input, const short& _Min, const short& _Max, const int& _Delta, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_slider_internal<short>(this, _ID, _Input, _Min, _Max, _Delta, _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar_slider<unsigned short>(const std::string& _ID, unsigned short& _Input, const unsigned short& _Min, const unsigned short& _Max, const int& _Delta, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_slider_internal<unsigned short>(this, _ID, _Input, _Min, _Max, _Delta, _Settings);
}

template<> bool ImmediateUserInterfaceContextLayer::input_scalar_slider<unsigned int>(const std::string& _ID, unsigned int& _Input, const unsigned int& _Min, const unsigned int& _Max, const int& _Delta, const ImmediateUserInterfaceInputScalarSettings& _Settings)
{
    return input_scalar_slider_internal<unsigned int>(this, _ID, _Input, _Min, _Max, _Delta, _Settings);
}

bool ImmediateUserInterfaceContextLayer::input_color(const std::string& _ID, gs_color& _Color, const ImmediateUserInterfaceColorPickerSettings& _Settings)
{
    struct ImmediateUserInterfaceInputColor : public ImmediateUserInterfaceNodeHorizontalStack
    {
    public:
        ImmediateUserInterfaceInputColor(const std::string& _Hash) : ImmediateUserInterfaceNodeHorizontalStack(_Hash){}
        virtual ~ImmediateUserInterfaceInputColor(){}

        gs_vec3ui RGB   = {255, 255, 255};
        gs_vec3ui HSV   = {255, 255, 255};
        gs_vec3ui HSL   = {255, 255, 255};
        gs_color  Alpha = 255;
    };

    ImmediateUserInterfaceInputScalarSettings settings =
        ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEdit |
        ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter;

    if(begin_node<ImmediateUserInterfaceInputColor>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceInputColor* picker =
            get_rendering_stack_top<ImmediateUserInterfaceInputColor>();

        float height =
            ((float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB) +
             (float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV) +
             (float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL) +
             (float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)) * m_Style.get_font_size();

        picker->State.MinimumSize = gs_vec2f((float)picker->State.MinimumSize.x, height);
        picker->State.MaximumSize = gs_vec2f((float)picker->State.MaximumSize.x, height);

        gs_vec2f parentSize = get_rendering_stack_top()->State.BoundingBox.size();

        float weight = 1.f / ((float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor) + 1.f);

        gs_vec4f padding = gs_vec4f(0.f, m_Style.get_frames_width() * 2.f, 0.f, 0.f);

        // editors
        next_size(gs_vec2f(parentSize.x * weight, parentSize.y));

        if(begin_vertical_stack(std::string(_ID).append("/Stack")))
        {
            char longestLabel[] = "Alpha\t";

            float labelWidth    = m_Renderer->calculate_bounding_box(
                &longestLabel[0],
                &longestLabel[sizeof(longestLabel) / sizeof(char)],
                m_Style.get_font_size(),
                m_Style.get_current_font()).width();

            // RGB
            if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB)
            {
                next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                next_content_padding(padding);

                if(begin_horizontal_stack(std::string(_ID).append("/Editors/RGB"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    label(std::string(_ID).append("/Contents/Editors/RGB/Label"), "RGB");

                    if(input_scalar<gs_color>(std::string(_ID).append("VerticalStack/RGB/RedValue"), picker->RGB.x, 0, 255, settings))
                        _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                    else
                        picker->RGB.x = gs_color_rgba_get_r(_Color);
                    
                    if(input_scalar<gs_color>(std::string(_ID).append("VerticalStack/RGB/GreenValue"), picker->RGB.y, 0, 255, settings))
                        _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                    else
                        picker->RGB.y = gs_color_rgba_get_g(_Color);
                    
                    if(input_scalar<gs_color>(std::string(_ID).append("/Contents/Editors/RGB/BlueValue"), picker->RGB.z, 0, 255, settings))
                        _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                    else
                        picker->RGB.z = gs_color_rgba_get_b(_Color);

                    end_horizontal_stack();
                }
            }

            // HSV
            if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV)
            {
                next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                next_content_padding(padding);

                if(begin_horizontal_stack(std::string(_ID).append("/Editors/HSV"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));   
                    label(std::string(_ID).append("/Editors/HSV/Label"), "HSV");

                    bool hsvChanged = false;

                    if(input_scalar<gs_color>(std::string(_ID).append("/Editors/HSV/HueValue"), picker->HSV.x, 0, 360, settings))
                        hsvChanged = true;
                    else
                        picker->HSV.x = (gs_color)((float)gs_color_hsv_get_h(gs_color_rgb_to_hsv(_Color)) / 255.f * 360.f);
                    
                    if(input_scalar<gs_color>(std::string(_ID).append("/Editors/HSV/SaturationValue"), picker->HSV.y, 0, 100, settings))
                        hsvChanged = true;
                    else
                        picker->HSV.y = (gs_color)((float)gs_color_hsv_get_s(gs_color_rgb_to_hsv(_Color)) / 255.f * 100.f);
                    
                    if(input_scalar<gs_color>(std::string(_ID).append("/Editors/HSV/BrightnessValue"), picker->HSV.z, 0, 100, settings))
                        hsvChanged = true;
                    else
                        picker->HSV.z = (gs_color)((float)gs_color_hsv_get_v(gs_color_rgb_to_hsv(_Color)) / 255.f * 100.f);

                    if(hsvChanged)
                    {
                        gs_color rgb = gs_color_hsv_to_rgb(
                            gs_color_hsv(
                                (gs_color)((float)picker->HSV.x / 360.f * 255.f),
                                (gs_color)((float)picker->HSV.y / 100.f * 255.f),
                                (gs_color)((float)picker->HSV.z / 100.f * 255.f)));

                        _Color = gs_color_rgba(gs_color_rgba_get_r(rgb), gs_color_rgba_get_g(rgb), gs_color_rgba_get_b(rgb), picker->Alpha);
                    }

                    end_horizontal_stack();
                }
            }

            // HSL
            if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL)
            {
                next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                next_content_padding(padding);

                if(begin_horizontal_stack(std::string(_ID).append("/Editors/HSL"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    label(std::string(_ID).append("/Editors/HSL/Label"), "HSL");

                    bool hsvChanged = false;

                    if(input_scalar<gs_color>(std::string(_ID).append("/Editors/HSL/HueValue"), picker->HSL.x, 0, 360, settings))
                        hsvChanged = true;
                    else
                        picker->HSL.x = (gs_color)((float)gs_color_hsl_get_h(gs_color_rgb_to_hsl(_Color)) / 255.f * 360.f);
                    
                    if(input_scalar<gs_color>(std::string(_ID).append("/Editors/HSL/SaturationValue"), picker->HSL.y, 0, 100, settings))
                        hsvChanged = true;
                    else
                        picker->HSL.y = (gs_color)((float)gs_color_hsl_get_s(gs_color_rgb_to_hsl(_Color)) / 255.f * 100.f);
                    
                    if(input_scalar<gs_color>(std::string(_ID).append("/Editors/HSL/BrightnessValue"), picker->HSL.z, 0, 100, settings))
                        hsvChanged = true;
                    else
                        picker->HSL.z = (gs_color)((float)gs_color_hsl_get_l(gs_color_rgb_to_hsl(_Color)) / 255.f * 100.f);

                    if(hsvChanged)
                    {
                        gs_color rgb = gs_color_hsl_to_rgb(
                            gs_color_hsl(
                                (gs_color)((float)picker->HSL.x / 360.f * 255.f),
                                (gs_color)((float)picker->HSL.y / 100.f * 255.f),
                                (gs_color)((float)picker->HSL.z / 100.f * 255.f)));

                        _Color = gs_color_rgba(gs_color_rgba_get_r(rgb), gs_color_rgba_get_g(rgb), gs_color_rgba_get_b(rgb), picker->Alpha);
                    }

                    end_horizontal_stack();
                }
            }

            // Alpha
            if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)
            {
                next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                next_content_padding(padding);

                if(begin_horizontal_stack(std::string(_ID).append("/Editors/Alpha"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    label(std::string(_ID).append("/Editors/Alpha/Label"), "Alpha");

                    if(input_scalar<gs_color>(std::string(_ID).append("/Editors/Alpha/AlphaValue"), picker->Alpha, 0, 255, settings))
                        _Color = gs_color_rgba(gs_color_rgba_get_r(_Color), gs_color_rgba_get_g(_Color), gs_color_rgba_get_b(_Color), picker->Alpha);
                    else
                        picker->Alpha = gs_color_rgba_get_a(_Color);
                    
                    end_horizontal_stack();
                }
            }

            end_vertical_stack();
        }

        // preview color
        next_size(gs_vec2f(parentSize.x * weight, parentSize.y));

        if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor)
            image(std::string(_ID).append("/Image"), _Color);

        end_node<ImmediateUserInterfaceInputColor>();
    }

    return true;
}

void ImmediateUserInterfaceContextLayer::color_picker_rgba(const std::string& _ID, gs_color& _Color, const ImmediateUserInterfaceColorPickerSettings& _Settings)
{
    struct ImmediateUserInterfaceColorPickerRGBA : public ImmediateUserInterfaceNodePanel
    {
    public:
        ImmediateUserInterfaceColorPickerRGBA(const std::string& _Hash) : ImmediateUserInterfaceNodePanel(_Hash){}
        virtual ~ImmediateUserInterfaceColorPickerRGBA(){}

        virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
        {
            ImmediateUserInterfaceNodePanel::layout(_Context);

            gs_vec2f gradientBoxSize = gs_vec2f(256.f, 256.f);
            gs_vec2f paletteBoxSize  = gs_vec2f(32.f, 256.f);
            gs_vec2f alphaBoxSize    = gs_vec2f(((Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)    ? 32.f : 0.f), 256.f);

            gs_vec2f padding   = gs_vec2f(8.f);
            gs_vec2f position  = State.BoundingBox.Min;
            gs_vec2f totalSize = gradientBoxSize + paletteBoxSize + alphaBoxSize + padding;

            // calculate gradient box
            {
                gs_vec2f size = gs_vec2f((gradientBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height());
                GradientBox   = gs_2dboxf(position, position + size);
                position     += gs_vec2f(size.x + padding.x, 0.f);
            }

            // calculate palette box
            {
                gs_vec2f size = gs_vec2f((paletteBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height());
                PaletteBox    = gs_2dboxf(position, position + size);
                position     += gs_vec2f(size.x + padding.x, 0.f);
            }

            // calculate alpha box
            {
                gs_vec2f size = gs_vec2f((alphaBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height());
                AlphaBox    = gs_2dboxf(position, position + size);
                position     += gs_vec2f(size.x + padding.x, 0.f);
            }
        }

        virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr)
                return;

            // render color palette box
            {
                int      sectors  = (int)(PaletteMaximumHue / PaletteHueStep);
                gs_vec2f position = PaletteBox.Min;
                gs_vec2f size     = gs_vec2f(PaletteBox.width(), PaletteBox.height() / (sectors - 1));
                
                for (int i = 1; i < sectors; i++)
                {
                    gs_color sourceColor = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)((float)(i - 1) * PaletteHueStep * 255.f), 255, 255));
                    gs_color targetColor = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)((float)(i - 0) * PaletteHueStep * 255.f), 255, 255));

                    _Context->m_Renderer->push_rectangle_gradient_mesh(
                        position,
                        position + size,
                        sourceColor,
                        sourceColor,
                        targetColor,
                        targetColor,
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                    position += gs_vec2f(0.f, size.y);
                }
                
                // palette slider
                gs_2dboxf paletteSlider = gs_2dboxf(
                    PaletteBox.Min + gs_vec2f(0.f, PaletteBoxSliderPosition) * PaletteBox.size() * 0.9f,
                    PaletteBox.Min + gs_vec2f(0.f, PaletteBoxSliderPosition) * PaletteBox.size() * 0.9f + gs_vec2f(PaletteBox.width(), PaletteBox.height() * 0.1f));

                _Context->m_Renderer->push_rectangle_filled(
                    paletteSlider.Min,
                    paletteSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                _Context->m_Renderer->push_rectangle_filled(
                    paletteSlider.Min + gs_vec2f(4.f),
                    paletteSlider.Max - gs_vec2f(4.f),
                    gs_color_rgba(255, 255, 255, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }

            // render color gradient box
            {
                float h = PaletteMaximumHue * PaletteBoxSliderPosition;

                gs_color c1 = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)(h * 255.f), 0, 255));
                gs_color c2 = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)(h * 255.f), 255, 255));
                gs_color c3 = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)(h * 255.f), 255, 0));

                // gradient box
                _Context->m_Renderer->push_rectangle_gradient_mesh(
                    GradientBox.Min,
                    GradientBox.Max,
                    c1,
                    c2,
                    c3,
                    c3,
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // gradient box slider
                gs_2dboxf gradientBoxSlider = gs_2dboxf(
                    GradientBox.Min + GradientBoxSliderPosition * GradientBox.size() * 0.9f,
                    GradientBox.Min + GradientBoxSliderPosition * GradientBox.size() * 0.9f + GradientBox.size() * 0.1f);

                _Context->m_Renderer->push_rectangle_filled(
                    gradientBoxSlider.Min,
                    gradientBoxSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                _Context->m_Renderer->push_rectangle_filled(
                    gradientBoxSlider.Min + gs_vec2f(4.f),
                    gradientBoxSlider.Max - gs_vec2f(4.f),
                    gs_color_rgb(gs_color_rgba_get_r(Color), gs_color_rgba_get_g(Color), gs_color_rgba_get_b(Color)),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }

            // render alpha editor
            if(Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)
            {
                // alpha box
                _Context->m_Renderer->push_rectangle_gradient_mesh(
                    AlphaBox.Min,
                    AlphaBox.Max,
                    gs_color_rgba(255, 255, 255, 255),
                    gs_color_rgba(255, 255, 255, 255),
                    gs_color_rgba(255, 255, 255, 0),
                    gs_color_rgba(255, 255, 255, 0),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // alpha box slider
                gs_2dboxf aphaSlider = gs_2dboxf(
                    AlphaBox.Min + gs_vec2f(0.f, AlphaBoxSliderPosition) * AlphaBox.size() * 0.9f,
                    AlphaBox.Min + gs_vec2f(0.f, AlphaBoxSliderPosition) * AlphaBox.size() * 0.9f + gs_vec2f(AlphaBox.width(), AlphaBox.height() * 0.1f));

                _Context->m_Renderer->push_rectangle_filled(
                    aphaSlider.Min,
                    aphaSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                _Context->m_Renderer->push_rectangle_filled(
                    aphaSlider.Min + gs_vec2f(4.f),
                    aphaSlider.Max - gs_vec2f(4.f),
                    gs_color_rgba(255, 255, 255, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }

            // calculate color
            {
                float h = PaletteMaximumHue * PaletteBoxSliderPosition;
                float s = GradientBoxSliderPosition.x;
                float v = 1.f - GradientBoxSliderPosition.y;
                float a = 1.f - AlphaBoxSliderPosition;

                Color = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)(h * 255.f), (gs_color)(s * 255.f), (gs_color)(v * 255.f)));
                Color = gs_color_rgba(gs_color_rgba_get_r(Color), gs_color_rgba_get_g(Color), gs_color_rgba_get_b(Color), (gs_color)roundf(255.f * a));
            }
        }

        virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr)
                return false;

            // stop catching
            if(!_Context->m_Input.is_mouse_button_down())
            {
                AlphaBoxSliderIsMoving    = false;
                PaletteBoxSliderIsMoving  = false;
                GradientBoxSliderIsMoving = false;
                Edited                    = false;
                return false;
            }

            // catch vertical color palette event
            if(((PaletteBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_pressed()) || PaletteBoxSliderIsMoving) && !GradientBoxSliderIsMoving && !AlphaBoxSliderIsMoving)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    PaletteBoxSliderPosition         = ((_Context->m_Input.get_cusor_position() - PaletteBox.Min - PaletteBox.size() * 0.1f * 0.5f) / PaletteBox.size() / 0.9f).y;
                    PaletteBoxSliderPreviousPosition = PaletteBoxSliderPosition;
                }

                PaletteBoxSliderPosition = gs_clamp(PaletteBoxSliderPreviousPosition + (_Context->m_Input.get_cusor_drag_delta() / PaletteBox.size() / 0.9f).y, 0.f, 1.f);
                PaletteBoxSliderIsMoving = true;
                Edited                   = true;

                return true;
            }

            // catch gradient color modifier event
            if(((GradientBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_pressed()) || GradientBoxSliderIsMoving) && !PaletteBoxSliderIsMoving && !AlphaBoxSliderIsMoving)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    GradientBoxSliderPosition         = (_Context->m_Input.get_cusor_position() - GradientBox.Min - GradientBox.size() * 0.1f * 0.5f) / GradientBox.size() / 0.9f;
                    GradientBoxSliderPreviousPosition = GradientBoxSliderPosition;
                }

                GradientBoxSliderPosition = gs_clamp(GradientBoxSliderPreviousPosition + _Context->m_Input.get_cusor_drag_delta() / GradientBox.size() / 0.9f, gs_vec2f(0.f, 0.f), gs_vec2f(1.f, 1.f));
                GradientBoxSliderIsMoving = true;
                Edited                    = true;

                return true;
            }

            // catch
            if(((AlphaBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_pressed()) || AlphaBoxSliderIsMoving) && !PaletteBoxSliderIsMoving && !GradientBoxSliderIsMoving)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    AlphaBoxSliderPosition         = ((_Context->m_Input.get_cusor_position() - AlphaBox.Min - AlphaBox.size() * 0.1f * 0.5f) / AlphaBox.size() / 0.9f).y;
                    AlphaBoxSliderPreviousPosition = AlphaBoxSliderPosition;
                }

                AlphaBoxSliderPosition = gs_clamp(AlphaBoxSliderPreviousPosition + (_Context->m_Input.get_cusor_drag_delta() / AlphaBox.size() / 0.9f).y, 0.f, 1.f);
                AlphaBoxSliderIsMoving = true;
                Edited                 = true;

                return true;
            }

            return false;
        }

        void force_rgba_color(const gs_color& _Color)
        {
            gs_color HSV = gs_color_rgb_to_hsv(_Color);
            float    h   = (float)(gs_color_hsv_get_h(HSV) / 255.f);
            float    s   = (float)(gs_color_hsv_get_s(HSV) / 255.f);
            float    v   = (float)(gs_color_hsv_get_v(HSV) / 255.f);
            float    a   = (float)(gs_color_rgba_get_a(_Color) / 255.f);

            // setup palette slider position
            PaletteBoxSliderPosition         = gs_clamp(h / PaletteMaximumHue, 0.f, 1.f);
            PaletteBoxSliderPreviousPosition = PaletteBoxSliderPosition;

            // setup grdient slider position
            GradientBoxSliderPosition         = gs_clamp(gs_vec2f(s, 1.f - v), gs_vec2f(0.f, 0.f), gs_vec2f(1.f, 1.f));
            GradientBoxSliderPreviousPosition = GradientBoxSliderPosition;

            // setup alpha slider position
            AlphaBoxSliderPosition         = gs_clamp(1.f - a, 0.f, 1.f);
            AlphaBoxSliderPreviousPosition = AlphaBoxSliderPosition;
        }
        
        // slider attributes
        gs_color                                  Color    = 1;
        gs_vec3ui                                 RGB      = {0, 0, 0};
        gs_vec3ui                                 HSV      = {0, 0, 0};
        gs_vec3ui                                 HSL      = {0, 0, 0};
        gs_color                                  Alpha    = 255;
        bool                                      Edited   = false;
        ImmediateUserInterfaceColorPickerSettings Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults;

    private:

        // gradient box
        gs_2dboxf GradientBox                       = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
        gs_vec2f  GradientBoxSliderPosition         = gs_vec2f(0.f, 0.f);
        gs_vec2f  GradientBoxSliderPreviousPosition = gs_vec2f(0.f, 0.f);
        bool      GradientBoxSliderIsMoving         = false;

        // palette box
        float     PaletteMaximumHue                 = 1.00f;
        float     PaletteHueStep                    = 0.05f;
        
        gs_2dboxf PaletteBox                        = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
        float     PaletteBoxSliderPosition          = 0.f;
        float     PaletteBoxSliderPreviousPosition  = 0.f;
        bool      PaletteBoxSliderIsMoving          = false;
        
        // alpha box
        gs_2dboxf AlphaBox                          = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
        float     AlphaBoxSliderPosition            = 0.f;
        float     AlphaBoxSliderPreviousPosition    = 0.f;
        bool      AlphaBoxSliderIsMoving            = false;
    };

    next_content_margin(gs_vec4f(4.f));

    if(begin_vertical_stack(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        // color picker
        if(begin_node<ImmediateUserInterfaceColorPickerRGBA>(std::string(_ID).append("/ColorPicker"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            end_node<ImmediateUserInterfaceColorPickerRGBA>();

        ImmediateUserInterfaceColorPickerRGBA* picker =
            get_rendered_stack_top<ImmediateUserInterfaceColorPickerRGBA>();

        picker->Settings = _Settings;

        char longestLabel[] = "Alpha\t";

        float labelWidth = m_Renderer->calculate_bounding_box(
            &longestLabel[0],
            &longestLabel[sizeof(longestLabel) / sizeof(char)],
            m_Style.get_font_size(),
            m_Style.get_current_font()).width();

        ImmediateUserInterfaceInputScalarSettings settings =
            ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter;

        float height =
            ((float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB) +
             (float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV) +
             (float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL) +
             (float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)) * m_Style.get_font_size();

        gs_vec4f padding = gs_vec4f(0.f, m_Style.get_frames_width() * 2.f, 0.f, 0.f);

        next_maximum_size(gs_vec2f(gs_huge<float>(), height));
        next_minimum_size(gs_vec2f(0.f, height));

        if(begin_horizontal_stack(std::string(_ID).append("/Panel")))
        {
            gs_vec2f parentSize = get_rendering_stack_top()->State.BoundingBox.size();

            float weight = 1.f / ((float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor) + 1.f);

            // editors
            next_size(gs_vec2f(parentSize.x * weight, parentSize.y));

            if(begin_vertical_stack(std::string(_ID).append("/Panel/Editors")))
            {
                // RGB
                if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB)
                {
                    next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                    next_content_padding(padding);

                    if(begin_horizontal_stack(std::string(_ID).append("/Panel/Editors/RGB"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                        label(std::string(_ID).append("/Panel/Editors/RGB/Label"), "RGB");
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/RGB/RedValue"), picker->RGB.x, 0, 255, settings))
                            _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                        else
                            picker->RGB.x = gs_color_rgba_get_r(_Color);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/RGB/GreenValue"), picker->RGB.y, 0, 255, settings))
                            _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                        else
                            picker->RGB.y = gs_color_rgba_get_g(_Color);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/RGB/BlueValue"), picker->RGB.z, 0, 255, settings))
                            _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                        else
                            picker->RGB.z = gs_color_rgba_get_b(_Color);

                        end_horizontal_stack();
                    }
                }

                // HSV
                if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV)
                {
                    next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                    next_content_padding(padding);

                    if(begin_horizontal_stack(std::string(_ID).append("/Panel/Editors/HSV"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));   
                        label(std::string(_ID).append("/Panel/Editors/HSV/Label"), "HSV");

                        bool hsvChanged = false;

                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/HSV/HueValue"), picker->HSV.x, 0, 360, settings))
                            hsvChanged = true;
                        else
                            picker->HSV.x = (gs_color)((float)gs_color_hsv_get_h(gs_color_rgb_to_hsv(_Color)) / 255.f * 360.f, settings);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/HSV/SaturationValue"), picker->HSV.y, 0, 100, settings))
                            hsvChanged = true;
                        else
                            picker->HSV.y = (gs_color)((float)gs_color_hsv_get_s(gs_color_rgb_to_hsv(_Color)) / 255.f * 100.f);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/HSV/BrightnessValue"), picker->HSV.z, 0, 100, settings))
                            hsvChanged = true;
                        else
                            picker->HSV.z = (gs_color)((float)gs_color_hsv_get_v(gs_color_rgb_to_hsv(_Color)) / 255.f * 100.f);

                        if(hsvChanged)
                        {
                            gs_color rgb = gs_color_hsv_to_rgb(
                                gs_color_hsv(
                                    (gs_color)((float)picker->HSV.x / 360.f * 255.f),
                                    (gs_color)((float)picker->HSV.y / 100.f * 255.f),
                                    (gs_color)((float)picker->HSV.z / 100.f * 255.f)));

                            _Color = gs_color_rgba(gs_color_rgba_get_r(rgb), gs_color_rgba_get_g(rgb), gs_color_rgba_get_b(rgb), picker->Alpha);
                        }

                        end_horizontal_stack();
                    }
                }

                // HSL
                if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL)
                {
                    next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                    next_content_padding(padding);

                    if(begin_horizontal_stack(std::string(_ID).append("/Panel/Editors/HSL"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                        label(std::string(_ID).append("/Panel/Editors/HSL/Label"), "HSL");

                        bool hsvChanged = false;

                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/HSL/HueValue"), picker->HSL.x, 0, 360, settings))
                            hsvChanged = true;
                        else
                            picker->HSL.x = (gs_color)((float)gs_color_hsl_get_h(gs_color_rgb_to_hsl(_Color)) / 255.f * 360.f);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/HSL/SaturationValue"), picker->HSL.y, 0, 100, settings))
                            hsvChanged = true;
                        else
                            picker->HSL.y = (gs_color)((float)gs_color_hsl_get_s(gs_color_rgb_to_hsl(_Color)) / 255.f * 100.f);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/HSL/BrightnessValue"), picker->HSL.z, 0, 100, settings))
                            hsvChanged = true;
                        else
                            picker->HSL.z = (gs_color)((float)gs_color_hsl_get_l(gs_color_rgb_to_hsl(_Color)) / 255.f * 100.f);

                        if(hsvChanged)
                        {
                            gs_color rgb = gs_color_hsl_to_rgb(
                                gs_color_hsl(
                                    (gs_color)((float)picker->HSL.x / 360.f * 255.f),
                                    (gs_color)((float)picker->HSL.y / 100.f * 255.f),
                                    (gs_color)((float)picker->HSL.z / 100.f * 255.f)));

                            _Color = gs_color_rgba(gs_color_rgba_get_r(rgb), gs_color_rgba_get_g(rgb), gs_color_rgba_get_b(rgb), picker->Alpha);
                        }

                        end_horizontal_stack();
                    }
                }

                // Alpha
                if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)
                {
                    next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                    next_content_padding(padding);

                    if(begin_horizontal_stack(std::string(_ID).append("/Panel/Editors/Alpha"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                        label(std::string(_ID).append("/Panel/Editors/Alpha/Label"), "Alpha");

                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/Alpha/AlphaValue"), picker->Alpha, 0, 255, settings))
                            _Color = gs_color_rgba(gs_color_rgba_get_r(picker->Color), gs_color_rgba_get_g(picker->Color), gs_color_rgba_get_b(picker->Color), picker->Alpha);
                        else
                            picker->Alpha = gs_color_rgba_get_a(_Color);
                        
                        end_horizontal_stack();
                    }
                }

                end_vertical_stack();
            }

            // preview color
            next_size(gs_vec2f(parentSize.x * weight, parentSize.y));

            if((_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor))
                image(std::string(_ID).append("/Panel/Editors/Color"), _Color);

            end_horizontal_stack();
        }

        // setup output color value
        if(!picker->Edited)
            picker->force_rgba_color(_Color);
        else
            _Color = picker->Color;

        end_vertical_stack();
    }
}

void ImmediateUserInterfaceContextLayer::color_picker_hsva(const std::string& _ID, gs_color& _Color, const ImmediateUserInterfaceColorPickerSettings& _Settings)
{
    struct ImmediateUserInterfaceColorPickerHSVA : public ImmediateUserInterfaceNodePanel
    {
    public:
        ImmediateUserInterfaceColorPickerHSVA(const std::string& _Hash) : ImmediateUserInterfaceNodePanel(_Hash){}
        virtual ~ImmediateUserInterfaceColorPickerHSVA(){}

        virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

            gs_vec2f ellpseBoxSize       = gs_vec2f(256.f, 256.f);
            gs_vec2f brightnessBoxSize   = gs_vec2f(32.f, 256.f);
            gs_vec2f transparencyBoxSize = gs_vec2f(((Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)    ? 32.f : 0.f), 256.f);
            gs_vec2f padding             = gs_vec2f(8.f);
            gs_vec2f totalSize           = ellpseBoxSize + brightnessBoxSize + transparencyBoxSize + padding;
            gs_vec2f position            = State.BoundingBox.Min;

            // ellipse
            {
                gs_2dboxf ellipseBox = gs_2dboxf(position, position + gs_vec2f((ellpseBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height()));

                Ellipse = gs_2d_ellipsef(
                    ellipseBox.center(),
                    gs_min(ellipseBox.width(), ellipseBox.height()) * 0.4f);

                EllipseSlider = gs_2d_ellipsef(
                    Ellipse.Center + EllipseSliderPosition * Ellipse.Radius,
                    Ellipse.Radius * 0.1f);

                position += gs_vec2f(ellipseBox.width() + padding.x, 0.f);
            }

            // brightness box
            {
                BrightnessBox = gs_2dboxf(position, position + gs_vec2f((brightnessBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height()));
                position     += gs_vec2f(BrightnessBox.width() + padding.x, 0.f);
            }

            // transparency box
            {
                TransparencyBox = gs_2dboxf(position, position + gs_vec2f((transparencyBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height()));
                position       += gs_vec2f(TransparencyBox.width() + padding.x, 0.f);
            }
        }

        virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

            float s = (float)gs_vector_length(EllipseSliderPosition);
            float h = (float)gs_vector_argument(EllipseSliderPosition);
            if(h < 0.f) h += (float)PI2;

            gs_color transparency = (gs_color)((1.f - TransparencySliderPosition) * 255.f);
            gs_color brightness   = (gs_color)((1.f - BrightnessSliderPosition) * 255.f);
            gs_color saturation   = (gs_color)(s * 255.f);
            gs_color hue          = (gs_color)(h / PI2 * 255.f);

            // render ellipse
            {
                // ellipse
                const float sourceAngle = 0.f;
                const float targetAngle = 360.f;
                const float delta       = 360.f / 64.f;

                for (float angle = sourceAngle; angle < targetAngle; angle += delta)
                {
                    gs_color c1 = gs_color_rgba(255, 255, 255, 255);
                    gs_color c2 = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)(angle / 360.f * 255.f), 255, brightness));
                    gs_color c3 = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)((angle + delta) / 360.f * 255.f), 255, brightness));

                    _Context->m_Renderer->build_triangle_gradient_mesh(
                        Ellipse.Center,
                        gs_vec2f(Ellipse.Center.x + Ellipse.Radius * cos(gs_to_radians(angle)), Ellipse.Center.y + Ellipse.Radius * sin(gs_to_radians(angle))),
                        gs_vec2f(Ellipse.Center.x + Ellipse.Radius * cos(gs_to_radians(angle + delta)), Ellipse.Center.y + Ellipse.Radius * sin(gs_to_radians(angle + delta))),
                        c1,
                        c2,
                        c3);
                }

                _Context->m_Renderer->push_rendering_command(
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // slider
                _Context->m_Renderer->push_arc_filled(
                    EllipseSlider.Center,
                    EllipseSlider.Radius,
                    EllipseSlider.Radius,
                    0.f,
                    360.f,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                _Context->m_Renderer->push_arc_filled(
                    EllipseSlider.Center,
                    EllipseSlider.Radius * 0.8f,
                    EllipseSlider.Radius * 0.8f,
                    0.f,
                    360.f,
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, brightness)),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }

            // render brightness box
            {
                // box
                _Context->m_Renderer->push_rectangle_gradient_mesh(
                    BrightnessBox.Min,
                    BrightnessBox.Max,
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, 255)),
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, 255)),
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, 0)),
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, 0)),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // slider
                gs_2dboxf brightnessBoxSlider = gs_2dboxf(
                    BrightnessBox.Min + gs_vec2f(0.f, BrightnessSliderPosition * BrightnessBox.height() * 0.9f),
                    BrightnessBox.Min + gs_vec2f(0.f, BrightnessSliderPosition * BrightnessBox.height() * 0.9f) + gs_vec2f(BrightnessBox.width(), BrightnessBox.height() * 0.1f));

                _Context->m_Renderer->push_rectangle_filled(
                    brightnessBoxSlider.Min,
                    brightnessBoxSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                _Context->m_Renderer->push_rectangle_filled(
                    brightnessBoxSlider.Min + gs_vec2f(4.f),
                    brightnessBoxSlider.Max - gs_vec2f(4.f),
                    gs_color_rgba(255, 255, 255, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }

            // render transparency box
            if(Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)
            {
                // box
                _Context->m_Renderer->push_rectangle_gradient_mesh(
                    TransparencyBox.Min,
                    TransparencyBox.Max,
                    gs_color_rgba(255, 255, 255, 255),
                    gs_color_rgba(255, 255, 255, 255),
                    gs_color_rgba(255, 255, 255, 0),
                    gs_color_rgba(255, 255, 255, 0),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // slider
                gs_2dboxf transparencyBoxSlider = gs_2dboxf(
                    TransparencyBox.Min + gs_vec2f(0.f, TransparencySliderPosition * TransparencyBox.height() * 0.9f),
                    TransparencyBox.Min + gs_vec2f(0.f, TransparencySliderPosition * TransparencyBox.height() * 0.9f) + gs_vec2f(TransparencyBox.width(), TransparencyBox.height() * 0.1f));

                _Context->m_Renderer->push_rectangle_filled(
                    transparencyBoxSlider.Min,
                    transparencyBoxSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                _Context->m_Renderer->push_rectangle_filled(
                    transparencyBoxSlider.Min + gs_vec2f(4.f),
                    transparencyBoxSlider.Max - gs_vec2f(4.f),
                    gs_color_rgba(255, 255, 255, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // calculate color
                gs_color RGB = gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, brightness));
                Color = gs_color_rgba(gs_color_rgba_get_r(RGB), gs_color_rgba_get_g(RGB), gs_color_rgba_get_b(RGB), transparency);
            }
        }

        virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr)
                return false;

            if(!_Context->m_Input.is_mouse_button_down())
            {
                EllipseSliderIsMoving      = false;
                BrightnessSliderIsMoving   = false;
                TransparencySliderIsMoving = false;
                Edited                     = false;
                return false;
            }

            // catch ellipse slider event
            if(((Ellipse.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_pressed()) || EllipseSliderIsMoving) && !BrightnessSliderIsMoving && !TransparencySliderIsMoving)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    EllipseSliderPosition         = (_Context->m_Input.get_cusor_position() - Ellipse.Center) / Ellipse.Radius;
                    EllipseSliderPreviousPosition = EllipseSliderPosition;
                }

                gs_vec2f radiusVector = (EllipseSliderPreviousPosition + _Context->m_Input.get_cusor_drag_delta() / Ellipse.Radius);
                EllipseSliderPosition = gs_vector_normalize(radiusVector) * gs_clamp((float)gs_vector_length(radiusVector), 0.f, 1.f);

                EllipseSliderIsMoving = true;
                Edited                = true;

                return true;
            }

            // catch brightness slider event
            if(((BrightnessBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_pressed()) || BrightnessSliderIsMoving) && !EllipseSliderIsMoving && !TransparencySliderIsMoving)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    BrightnessSliderPosition         = ((_Context->m_Input.get_cusor_position() - BrightnessBox.Min - BrightnessBox.size() * 0.1f * 0.5f) / BrightnessBox.size() / 0.9f).y;
                    BrightnessSliderPreviousPosition = BrightnessSliderPosition;
                }

                BrightnessSliderPosition = gs_clamp(BrightnessSliderPreviousPosition + (_Context->m_Input.get_cusor_drag_delta() / BrightnessBox.size() / 0.9f).y, 0.f, 1.f);
                BrightnessSliderIsMoving = true;
                Edited                   = true;

                return true;
            }

            // catch transparency slider event
            if(((TransparencyBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_pressed()) || TransparencySliderIsMoving) && !EllipseSliderIsMoving && !BrightnessSliderIsMoving)
            {
                if(_Context->m_Input.is_mouse_button_pressed())
                {
                    TransparencySliderPosition         = ((_Context->m_Input.get_cusor_position() - TransparencyBox.Min - TransparencyBox.size() * 0.1f * 0.5f) / TransparencyBox.size() / 0.9f).y;
                    TransparencySliderPreviousPosition = TransparencySliderPosition;
                }

                TransparencySliderPosition = gs_clamp(TransparencySliderPreviousPosition + (_Context->m_Input.get_cusor_drag_delta() / TransparencyBox.size() / 0.9f).y, 0.f, 1.f);
                TransparencySliderIsMoving = true;
                Edited                     = true;

                return true;
            }

            return false;
        }

        void force_rgba_color(const gs_color& _Color)
        {
            gs_color HSV            = gs_color_rgb_to_hsv(_Color);
            gs_color Alpha          = gs_color_rgba_get_a(_Color);
            float    vectorLength   = (float)gs_color_hsv_get_s(HSV) / 255.f;
            float    vectorArgument = (float)gs_color_hsv_get_h(HSV) / 255.f * (float)PI2;

            EllipseSliderPosition      = gs_vec2f(cos(vectorArgument), sin(vectorArgument)) * vectorLength;
            BrightnessSliderPosition   = 1.f - (float)gs_color_hsv_get_v(HSV) / 255.f;
            TransparencySliderPosition = 1.f - (float)(Alpha / 255.f);
        }

        // public attributes
        gs_color                                  Color    = 1;
        gs_vec3ui                                 RGB      = {0, 0, 0};
        gs_vec3ui                                 HSV      = {0, 0, 0};
        gs_vec3ui                                 HSL      = {0, 0, 0};
        gs_color                                  Alpha    = 255;
        bool                                      Edited   = false;
        ImmediateUserInterfaceColorPickerSettings Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults;

    private:
        // info
        
        // ellipse
        gs_2d_ellipsef Ellipse                            = gs_2d_ellipsef(0.f, 0.f);
        gs_2d_ellipsef EllipseSlider                      = gs_2d_ellipsef(0.f, 0.f);
        gs_vec2f       EllipseSliderPosition              = gs_vec2f(0.f, 0.f);
        gs_vec2f       EllipseSliderPreviousPosition      = gs_vec2f(0.f, 0.f);
        bool           EllipseSliderIsMoving              = false;

        // brightness
        gs_2dboxf      BrightnessBox                      = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.0, 0.f));
        float          BrightnessSliderPosition           = 0.f;
        float          BrightnessSliderPreviousPosition   = 0.f;
        bool           BrightnessSliderIsMoving           = false;

        // transparency
        gs_2dboxf      TransparencyBox                    = gs_2dboxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.0, 0.f));
        float          TransparencySliderPosition         = 0.f;
        float          TransparencySliderPreviousPosition = 0.f;
        bool           TransparencySliderIsMoving         = false;
    };

    next_content_padding(gs_vec2f(2.f));

    if(begin_vertical_stack(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        // color picker
        if(begin_node<ImmediateUserInterfaceColorPickerHSVA>(std::string(_ID).append("/ColorPicker"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            end_node<ImmediateUserInterfaceColorPickerHSVA>();

        ImmediateUserInterfaceColorPickerHSVA* picker =
            get_rendered_stack_top<ImmediateUserInterfaceColorPickerHSVA>();

        picker->Settings = _Settings;

        char longestLabel[] = "Alpha\t";

        float labelWidth = m_Renderer->calculate_bounding_box(
            &longestLabel[0],
            &longestLabel[sizeof(longestLabel) / sizeof(char)],
            m_Style.get_font_size(),
            m_Style.get_current_font()).width();

        ImmediateUserInterfaceInputScalarSettings settings =
            ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEdit;

        float height =
            ((float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB) +
             (float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV) +
             (float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL) +
             (float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)) * m_Style.get_font_size();

        gs_vec4f padding = gs_vec4f(0.f, m_Style.get_frames_width() * 2.f, 0.f, 0.f);

        next_maximum_size(gs_vec2f(gs_huge<float>(), height));
        next_minimum_size(gs_vec2f(0.f, height));

        if(begin_horizontal_stack(std::string(_ID).append("/Panel"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
        {
            gs_vec2f parentSize = get_rendering_stack_top()->State.BoundingBox.size();

            float weight = 1.f / ((float)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor) + 1.f);

            // editors
            next_size(gs_vec2f(parentSize.x * weight, parentSize.y));

            if(begin_vertical_stack(std::string(_ID).append("/Panel/Editors"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                // RGB
                if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB)
                {
                    next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                    next_content_padding(padding);

                    if(begin_horizontal_stack(std::string(_ID).append("/Panel/Editors/RGB"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                        label(std::string(_ID).append("/Panel/Editors/RGB/Label"), "RGB");
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/RGB/RedValue"), picker->RGB.x, 0, 255, settings))
                            _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                        else
                            picker->RGB.x = gs_color_rgba_get_r(_Color);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/RGB/GreenValue"), picker->RGB.y, 0, 255, settings))
                            _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                        else
                            picker->RGB.y = gs_color_rgba_get_g(_Color);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("/Panel/Editors/RGB/BlueValue"), picker->RGB.z, 0, 255, settings))
                            _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                        else
                            picker->RGB.z = gs_color_rgba_get_b(_Color);

                        end_horizontal_stack();
                    }
                }

                // HSV
                if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV)
                {
                    next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                    next_content_padding(padding);

                    if(begin_horizontal_stack(std::string(_ID).append("Panel/Editors/HSV"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));   
                        label(std::string(_ID).append("Panel/Editors/HSV/Label"), "HSV");

                        bool hsvChanged = false;

                        if(input_scalar<gs_color>(std::string(_ID).append("Panel/Editors/HSV/HueValue"), picker->HSV.x, 0, 360, settings))
                            hsvChanged = true;
                        else
                            picker->HSV.x = (gs_color)((float)gs_color_hsv_get_h(gs_color_rgb_to_hsv(_Color)) / 255.f * 360.f);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("Panel/Editors/HSV/SaturationValue"), picker->HSV.y, 0, 100, settings))
                            hsvChanged = true;
                        else
                            picker->HSV.y = (gs_color)((float)gs_color_hsv_get_s(gs_color_rgb_to_hsv(_Color)) / 255.f * 100.f);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("Panel/Editors/HSV/BrightnessValue"), picker->HSV.z, 0, 100, settings))
                            hsvChanged = true;
                        else
                            picker->HSV.z = (gs_color)((float)gs_color_hsv_get_v(gs_color_rgb_to_hsv(_Color)) / 255.f * 100.f);

                        if(hsvChanged)
                        {
                            gs_color rgb = gs_color_hsv_to_rgb(
                                gs_color_hsv(
                                    (gs_color)((float)picker->HSV.x / 360.f * 255.f),
                                    (gs_color)((float)picker->HSV.y / 100.f * 255.f),
                                    (gs_color)((float)picker->HSV.z / 100.f * 255.f)));

                            _Color = gs_color_rgba(gs_color_rgba_get_r(rgb), gs_color_rgba_get_g(rgb), gs_color_rgba_get_b(rgb), picker->Alpha);
                        }

                        end_horizontal_stack();
                    }
                }

                // HSL
                if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL)
                {
                    next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                    next_content_padding(padding);

                    if(begin_horizontal_stack(std::string(_ID).append("Panel/Editors/HSL"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                        label(std::string(_ID).append("Panel/Editors/HSL/Label"), "HSL");

                        bool hsvChanged = false;

                        if(input_scalar<gs_color>(std::string(_ID).append("Panel/Editors/HSL/HueValue"), picker->HSL.x, 0, 360, settings))
                            hsvChanged = true;
                        else
                            picker->HSL.x = (gs_color)((float)gs_color_hsl_get_h(gs_color_rgb_to_hsl(_Color)) / 255.f * 360.f);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("Panel/Editors/HSL/SaturationValue"), picker->HSL.y, 0, 100, settings))
                            hsvChanged = true;
                        else
                            picker->HSL.y = (gs_color)((float)gs_color_hsl_get_s(gs_color_rgb_to_hsl(_Color)) / 255.f * 100.f);
                        
                        if(input_scalar<gs_color>(std::string(_ID).append("Panel/Editors/HSL/BrightnessValue"), picker->HSL.z, 0, 100, settings))
                            hsvChanged = true;
                        else
                            picker->HSL.z = (gs_color)((float)gs_color_hsl_get_l(gs_color_rgb_to_hsl(_Color)) / 255.f * 100.f);

                        if(hsvChanged)
                        {
                            gs_color rgb = gs_color_hsl_to_rgb(
                                gs_color_hsl(
                                    (gs_color)((float)picker->HSL.x / 360.f * 255.f),
                                    (gs_color)((float)picker->HSL.y / 100.f * 255.f),
                                    (gs_color)((float)picker->HSL.z / 100.f * 255.f)));

                            _Color = gs_color_rgba(gs_color_rgba_get_r(rgb), gs_color_rgba_get_g(rgb), gs_color_rgba_get_b(rgb), picker->Alpha);
                        }

                        end_horizontal_stack();
                    }
                }

                // Alpha
                if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)
                {
                    next_minimum_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                    next_maximum_size(gs_vec2f((float)INT_MAX, m_Style.get_font_size()));
                    next_content_padding(padding);

                    if(begin_horizontal_stack(std::string(_ID).append("Panel/Editors/Alpha"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        next_size(gs_vec2f(labelWidth, m_Style.get_font_size()));
                        label(std::string(_ID).append("Panel/Editors/Alpha/Label"), "Alpha");

                        if(input_scalar<gs_color>(std::string(_ID).append("Panel/Editors/Alpha/AlphaValue"), picker->Alpha, 0, 255, settings))
                            _Color = gs_color_rgba(gs_color_rgba_get_r(picker->Color), gs_color_rgba_get_g(picker->Color), gs_color_rgba_get_b(picker->Color), picker->Alpha);
                        else
                            picker->Alpha = gs_color_rgba_get_a(_Color);
                        
                        end_horizontal_stack();
                    }
                }

                end_vertical_stack();
            }

            // preview color
            next_size(gs_vec2f(parentSize.x * weight, parentSize.y));

            if((_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColor))
                image(std::string(_ID).append("/Panel/Editors/Color"), _Color);

            end_horizontal_stack();
        }

        // setup output color value
        if(!picker->Edited)
            picker->force_rgba_color(_Color);
        else
            _Color = picker->Color;

        end_vertical_stack();
    }
}

void ImmediateUserInterfaceContextLayer::image(const std::string& _ID, const gs_color& _ColorMask, const ApplicationRenderingBackendTexture& _Texture)
{
    struct ImmediateUserInterfaceNodeImage : public ImmediateUserInterfaceNode
    {
    public:
        ImmediateUserInterfaceNodeImage(const std::string& _Hash) : ImmediateUserInterfaceNode(_Hash){}
        virtual ~ImmediateUserInterfaceNodeImage(){}

        virtual void render(ImmediateUserInterfaceContextLayer* _Context) override
        {
            if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

            _Context->m_Renderer->push_rectangle_filled(
                State.BoundingBox.Min,
                State.BoundingBox.Max,
                ColorMask,
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                Texture);
        }

        ApplicationRenderingBackendTexture Texture   = ApplicationRenderingBackendTexture();
        gs_color                           ColorMask = gs_color_rgba(255, 255, 255, 255);
    };

    if(begin_node<ImmediateUserInterfaceNodeImage>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        get_rendering_stack_top<ImmediateUserInterfaceNodeImage>()->Texture   = _Texture;
        get_rendering_stack_top<ImmediateUserInterfaceNodeImage>()->ColorMask = _ColorMask;
        end_node<ImmediateUserInterfaceNodeImage>();
    }
}

bool ImmediateUserInterfaceContextLayer::begin_combobox(const std::string& _ID, const std::string& _Preview)
{
    if(begin_node<ImmediateUserInterfaceCombobox>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceCombobox* widget = get_rendering_stack_top<ImmediateUserInterfaceCombobox>();

        // render
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            int depth = widget->Cache.Depth;

            // outline
            m_Renderer->push_rectangle_rounded_filled(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Max,
                m_Style.get_frames_radius(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                m_Renderer->calculate_transform_matrix((float)depth++));

            // background
            m_Renderer->push_rectangle_rounded_filled(
                widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                m_Style.get_frames_radius(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                m_Renderer->calculate_transform_matrix((float)depth++));

            // open button
            //  + m_Style.get_frames_width()

            gs_2dboxf openButtonBox = gs_2dboxf(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Min + gs_vec2f(m_Style.get_font_size() + m_Style.get_frames_width(), widget->State.BoundingBox.height()));

            m_Renderer->push_rectangle_rounded_filled(
                openButtonBox.Min,
                openButtonBox.Max,
                m_Style.get_frames_radius(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                m_Renderer->calculate_transform_matrix((float)depth++));

            if(openButtonBox.contains(m_Input.get_cusor_position()) && m_Input.is_mouse_button_down())
            {
                m_Renderer->push_rectangle_rounded_filled(
                    openButtonBox.Min + m_Style.get_frames_width(),
                    openButtonBox.Max - m_Style.get_frames_width(),
                    m_Style.get_frames_radius(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }
            else
            {
                m_Renderer->push_rectangle_rounded_filled(
                    openButtonBox.Min + m_Style.get_frames_width(),
                    openButtonBox.Max - m_Style.get_frames_width(),
                    m_Style.get_frames_radius(),
                        openButtonBox.contains(m_Input.get_cusor_position()) ?
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }

            if(widget->Active)
            {
                m_Renderer->push_triangle_filled(
                    openButtonBox.center() + gs_vec2f(-openButtonBox.height() * 0.25f, -openButtonBox.height() * 0.25f),
                    openButtonBox.center() + gs_vec2f(+openButtonBox.height() * 0.25f, -openButtonBox.height() * 0.25f),
                    openButtonBox.center() + gs_vec2f(0.f, openButtonBox.height() * 0.25f * 0.5f),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }
            else
            {
                m_Renderer->push_triangle_filled(
                    openButtonBox.center() + gs_vec2f(0.f, -openButtonBox.height() * 0.25f),
                    openButtonBox.center() + gs_vec2f(0.f * 0.25f, +openButtonBox.height() * 0.25f),
                    openButtonBox.center() + gs_vec2f(+openButtonBox.height() * 0.25f, 0.f),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix((float)depth++));
            }

            // preview text
            m_Renderer->push_text(
                gs_vec2f(openButtonBox.Max.x, openButtonBox.center().y - m_Renderer->calculate_bounding_box(_Preview.begin(), _Preview.end(), m_Style.get_font_size(), m_Style.get_current_font()).height() * 0.5f),
                _Preview.begin(),
                _Preview.end(),
                m_Style.get_font_size(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                m_Renderer->calculate_transform_matrix((float)depth++),
                m_Style.get_current_font());

            m_Renderer->pop_clip_box();
        }

        // activate
        if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
        {
            widget->HoverTime = Frenchie::Core::tic();
        }
        else if(
             widget->ScrollArea != nullptr                                                &&
            !widget->ScrollArea->State.BoundingBox.contains(m_Input.get_cusor_position()) &&
            Frenchie::Core::elapsed<std::chrono::microseconds>(widget->HoverTime, Frenchie::Core::tic()) > 100)
        {
            widget->Active  = false;
            widget->Hovered = false;
        }

        if(!widget->Active)
        {
            end_combobox();
            return false;
        }

        if(begin_node<ImmediateUserInterfaceComboboxScrollArea>(std::string(_ID).append("/ScrollArea"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ManualRenderingOrderSetup
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
        {
            widget->ScrollArea                       = get_rendering_stack_top<ImmediateUserInterfaceScrollArea>();
            widget->ScrollArea->State.MaximumSize    = gs_vec2f(256.f, 256.f);
            widget->ScrollArea->State.PlaceInFollow  = true;
            widget->ScrollArea->State.RenderingOrder = ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Popup;

            // calculate rect
            gs_2dboxf box = widget->get_visible_rect(this);
            
            widget->ScrollArea->State.BoundingBox = gs_2dboxf(
                gs_vec2f(box.Min.x, box.Max.y) - gs_vec2f(0.f, m_Style.get_frames_width()),
                gs_vec2f(box.Min.x, box.Max.y) - gs_vec2f(0.f, m_Style.get_frames_width()) + widget->ScrollArea->State.BoundingBox.size());

            if(widget->ScrollArea->State.BoundingBox.contains(m_Input.get_cusor_position()))
            {
                widget->Active  = true;
                widget->Hovered = true;
            }
            else if(widget->Hovered && widget->Active)
            {
                widget->Active  = false;
                widget->Hovered = false;
            }

            end_node<ImmediateUserInterfaceComboboxScrollArea>();
        }
    }

    return true;
}

void ImmediateUserInterfaceContextLayer::end_combobox()
{
    end_node<ImmediateUserInterfaceCombobox>();
}

bool ImmediateUserInterfaceContextLayer::begin_tree_node(
    const std::string&                            _ID,
    const ImmediateUserInterfaceTreeNodeSettings& _Settings,
    const ApplicationRenderingBackendTexture&     _TextureOpened,
    const ApplicationRenderingBackendTexture&     _TextureClosed)
{
    if(begin_node<ImmediateUserInterfaceTreeNode>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceTreeNode* treeNode =
            get_rendering_stack_top<ImmediateUserInterfaceTreeNode>();

        treeNode->TreeSettings  = _Settings;
        treeNode->TextureOpened = _TextureOpened;
        treeNode->TextureClosed = _TextureClosed;

        if(!treeNode->Opened)
        {
            end_node<ImmediateUserInterfaceTreeNode>();
            return false;
        }

        return true;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::end_tree_node()
{
    end_node<ImmediateUserInterfaceTreeNode>();
}

bool ImmediateUserInterfaceContextLayer::begin_menu(const std::string& _ID)
{
    ImmediateUserInterfaceMenu*       menu      = nullptr;
    ImmediateUserInterfaceMenuAction* menuItem  = nullptr;
    bool                              hasParent = false;
    bool                              isHovered = false;

    // retrieve controller
    ImmedidateUserInterfaceMenusController* menusController =
        get_controller<ImmedidateUserInterfaceMenusController>();

    if(begin_node<ImmediateUserInterfaceMenu>(
        std::string(_ID).append("/Menu"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        menu      = get_rendering_stack_top<ImmediateUserInterfaceMenu>();
        hasParent = m_Hierarchy.get_parent(menu) != nullptr;

        if(begin_node<ImmediateUserInterfaceMenuScrollArea>(std::string(_ID).append("/Menu/InternalScrollArea"),
              ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ManualRenderingOrderSetup
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
            | (hasParent ? ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically : 0)
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft))
        {
            menu->InternalScrollArea = get_rendering_stack_top<ImmediateUserInterfaceScrollArea>();
            menu->InternalScrollArea->State.PlaceInFollow = true;

            // if menu is inside another menu we draw menu item
            if(hasParent)
            {
                if(begin_node<ImmediateUserInterfaceMenuItem>(
                    _ID,
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    menuItem = get_rendering_stack_top<ImmediateUserInterfaceMenuItem>();
                    isHovered = (menuItem->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered);
                    end_node<ImmediateUserInterfaceMenuItem>();
                }
            }

            end_node<ImmediateUserInterfaceMenuScrollArea>();
        }
    }

    // 
    if(menusController != nullptr &&
        std::find(menusController->ActiveMenus.begin(), menusController->ActiveMenus.end(), menu) == menusController->ActiveMenus.end())
    {
        if(isHovered)
            menusController->ActiveMenus.push_back(menu);
    }

    isHovered = (menusController != nullptr && std::find(menusController->ActiveMenus.begin(), menusController->ActiveMenus.end(), menu) != menusController->ActiveMenus.end()) || isHovered;

    if(hasParent)
    {
        if(hasParent && isHovered && menuItem != nullptr)
        {
            if(begin_node<ImmediateUserInterfaceMenuScrollArea>(std::string(_ID).append("/Main/ExternalScrollArea"),
                  ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ManualRenderingOrderSetup
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
            {
                menu->ExternalScrollArea = get_rendering_stack_top<ImmediateUserInterfaceScrollArea>();
                menu->ExternalScrollArea->State.MouseHover    |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered;
                menu->ExternalScrollArea->State.PlaceInFollow  = true;
                menu->ExternalScrollArea->State.RenderingOrder = ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Popup;

                // calculate rect
                gs_2dboxf box = menuItem->get_visible_rect(this);
                
                menu->ExternalScrollArea->State.BoundingBox = gs_2dboxf(
                    gs_vec2f(box.Max.x, box.Min.y),
                    gs_vec2f(box.Max.x, box.Min.y) + menu->ExternalScrollArea->State.BoundingBox.size());

                end_node<ImmediateUserInterfaceMenuScrollArea>();
            }

            return true;
        }

        // do not render children
        end_menu();
        return false;
    }

    return true;
}

void ImmediateUserInterfaceContextLayer::end_menu()
{
    end_node<ImmediateUserInterfaceMenu>();
}

bool ImmediateUserInterfaceContextLayer::begin_window(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, bool* _Opened)
{
    return begin_node<ImmediateUserInterfaceWindow>(_ID, _Settings, _Opened);
}

void ImmediateUserInterfaceContextLayer::end_window()
{
    end_node<ImmediateUserInterfaceWindow>();
}

std::string ImmediateUserInterfaceContextLayer::next_id(const std::string& _Name, const std::string& _Hash)
{
    ImmediateUserInterfaceNode* top = get_rendering_stack_top();

    if(top == nullptr)
        return !_Hash.empty() ? std::string(_Name).append("###").append(_Hash) : _Name;
        
    return !_Hash.empty() ?
                std::string(_Name).append("###").append(top->Hash).append("/").append(_Hash) :
                    std::string(top->Hash).append("/").append(_Name);
}

void ImmediateUserInterfaceContextLayer::next_line()
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextLine = controller->NextLine.has_value() ? controller->NextLine.value() + 1 : 1;
}

void ImmediateUserInterfaceContextLayer::same_line()
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextLine = 0;
}

void ImmediateUserInterfaceContextLayer::indent(const float& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextIndent = controller->NextIndent.has_value() ? controller->NextIndent.value() + _Value : _Value;
}

void ImmediateUserInterfaceContextLayer::next_size(const gs_vec2f& _Value)
{
    next_minimum_size(_Value);
    next_maximum_size(_Value);
}

void ImmediateUserInterfaceContextLayer::next_position(const gs_vec2f& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextPosition = _Value;
}

void ImmediateUserInterfaceContextLayer::next_minimum_size(const gs_vec2f& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextMinimumSize = _Value;
}

void ImmediateUserInterfaceContextLayer::next_maximum_size(const gs_vec2f& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextMaximumSize = _Value;
}

void ImmediateUserInterfaceContextLayer::next_content_margin(const gs_vec4f& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextContentMargin = _Value;
}

void ImmediateUserInterfaceContextLayer::next_content_padding(const gs_vec4f& _Value)
{
    ImmedidateUserInterfaceNextNodeController* controller =
        get_controller<ImmedidateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextContentPadding = _Value;
}