#include <FrenchieImmediateUserInterfaceLayer.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

// STL
#include <algorithm>
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
        enum ImmediateUserInterfaceRenderingLayer_ : int
        {
            ImmediateUserInterfaceRenderingLayer_Begin   = 0,
            ImmediateUserInterfaceRenderingLayer_Main    = ImmediateUserInterfaceRenderingLayer_Begin,
            ImmediateUserInterfaceRenderingLayer_Gizmos,
            ImmediateUserInterfaceRenderingLayer_End,
        };

        enum ImmediateUserInterfaceDockingAnchor_ : int
        {
            ImmediateUserInterfaceDockingAnchor_Top    = 1 << 0,
            ImmediateUserInterfaceDockingAnchor_Left   = 1 << 1,
            ImmediateUserInterfaceDockingAnchor_Right  = 1 << 2,
            ImmediateUserInterfaceDockingAnchor_Bottom = 1 << 3,
            ImmediateUserInterfaceDockingAnchor_Center = 1 << 4,

            ImmediateUserInterfaceDockingAnchor_All    =
                  ImmediateUserInterfaceDockingAnchor_Top
                | ImmediateUserInterfaceDockingAnchor_Left
                | ImmediateUserInterfaceDockingAnchor_Right
                | ImmediateUserInterfaceDockingAnchor_Bottom
                | ImmediateUserInterfaceDockingAnchor_Center
        };

        enum ImmediateUserInterfaceWindowEvents_ : int
        {
            ImmediateUserInterfaceWindowEvents_DragStarted  = ImmediateUserInterfaceNodeEvents_Custom << 1,
        };

        // layouts
        struct ImmediateUserInterfacePanel : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfacePanel(const std::string& _Name);
            virtual ~ImmediateUserInterfacePanel();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void measure(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void restore() override;

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

            std::optional<gs_vec2f> LastSize;
            std::optional<gs_vec2f> LastMinimumSize;
            std::optional<gs_vec2f> LastMaximumSize;

            std::optional<gs_vec2f> MinimumSizeBeforeResizeToContents;
            std::optional<gs_vec2f> MaximumSizeBeforeResizeToContents;
        };

        struct ImmediateUserInterfaceVerticalStack : public ImmediateUserInterfacePanel
        {
        public:
            ImmediateUserInterfaceVerticalStack(const std::string& _Name);
            virtual ~ImmediateUserInterfaceVerticalStack();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceHorizontalStack : public ImmediateUserInterfacePanel
        {
        public:
            ImmediateUserInterfaceHorizontalStack(const std::string& _Name);
            virtual ~ImmediateUserInterfaceHorizontalStack();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        };
   
        struct ImmediateUserInterfaceGrid : public ImmediateUserInterfacePanel
        {
            ImmediateUserInterfaceGrid(const std::string& _Hash);
            virtual ~ImmediateUserInterfaceGrid();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;

            std::vector<gs_2d_boxf> Cells;
            std::vector<gs_2d_boxf> CellsCache;
        };

        struct ImmediateUserInterfaceGridPlace : public ImmediateUserInterfacePanel
        {
            ImmediateUserInterfaceGridPlace(const std::string& _Hash);
            virtual ~ImmediateUserInterfaceGridPlace();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        
            int Row    = 0;
            int Column = 0;
        };

        // scroll area
        struct IImmediateUserInterfaceScrollArea
        {
            virtual gs_vec2f get_scroll_offset(const bool& _Scaled = true) const = 0;
        };

        struct ImmediateUserInterfaceScrollArea : public ImmediateUserInterfacePanel, public IImmediateUserInterfaceScrollArea
        {
        public:

            // nested types
            struct ScrollBar
            {
                void recompute(gs_vec2f _ScrollbarMinimumValue, gs_vec2f _ScrollbarMaximumValue, gs_vec2f _TotalContentSize, gs_vec2f _ScrollbarMinimumSize)
                {
                    PositionScale     = calculate_scrollbar_slider_position_scale(_ScrollbarMinimumValue, _ScrollbarMaximumValue, _TotalContentSize);
                    ConstrainedSize   = calculate_scrollbar_length(_ScrollbarMinimumValue, _ScrollbarMaximumValue, _TotalContentSize, _ScrollbarMinimumSize);
                    UnconstrainedSize = calculate_scrollbar_length(_ScrollbarMinimumValue, _ScrollbarMaximumValue, _TotalContentSize, gs_vec2f(0.f, 0.f));
                }

                void reset()
                {
                    Position           = gs_vec2f(0.f, 0.f);
                    PositionScale      = gs_vec2f(1.f, 1.f);
                    PreviousPosition   = gs_vec2f(0.f, 0.f);
                    ConstrainedSize    = gs_vec2f(0.f, 0.f);
                    UnconstrainedSize  = gs_vec2f(0.f, 0.f);
                }

                // info
                gs_vec2f  Position           = gs_vec2f(0.f, 0.f);
                gs_vec2f  PositionScale      = gs_vec2f(1.f, 1.f);
                gs_vec2f  PreviousPosition   = gs_vec2f(0.f, 0.f);
                gs_vec2f  ConstrainedSize    = gs_vec2f(0.f, 0.f);
                gs_vec2f  UnconstrainedSize  = gs_vec2f(0.f, 0.f);

            private:

                gs_vec2f calculate_scrollbar_length(gs_vec2f _ScrollbarMinimumValue, gs_vec2f _ScrollbarMaximumValue, gs_vec2f _TotalContentSize, gs_vec2f _ScrollbarMinimumSize)
                {
                    gs_vec2f totalContentSize = gs_vec2f(gs_max(_TotalContentSize.x, 0.01f), gs_max(_TotalContentSize.y, 0.01f));

                    return gs_vec2f(
                        gs_min(gs_max(gs_abs(_ScrollbarMaximumValue.x - _ScrollbarMinimumValue.x) / gs_abs(totalContentSize.x) * _ScrollbarMaximumValue.x, _ScrollbarMinimumSize.x), _ScrollbarMaximumValue.x),
                        gs_min(gs_max(gs_abs(_ScrollbarMaximumValue.y - _ScrollbarMinimumValue.y) / gs_abs(totalContentSize.y) * _ScrollbarMaximumValue.y, _ScrollbarMinimumSize.y), _ScrollbarMaximumValue.y));
                };

                gs_vec2f calculate_scrollbar_slider_position_scale(gs_vec2f _ScrollbarMinimumValue, gs_vec2f _ScrollbarMaximumValue, gs_vec2f _TotalContentSize)
                {
                    gs_vec2f totalContentSize = gs_vec2f(gs_max(_TotalContentSize.x, 0.01f), gs_max(_TotalContentSize.y, 0.01f));
                    
                    gs_vec2f scale = gs_vec2f(
                        gs_abs(totalContentSize.x), gs_abs(totalContentSize.y)) / gs_vec2f(gs_abs(_ScrollbarMaximumValue.x - _ScrollbarMinimumValue.x),
                        gs_abs(_ScrollbarMaximumValue.y - _ScrollbarMinimumValue.y));

                    return gs_vec2f(gs_max(scale.x, 1.f), gs_max(scale.y, 1.f));
                };
            };
            
            // API
            ImmediateUserInterfaceScrollArea(const std::string& _Name);
            
            virtual ~ImmediateUserInterfaceScrollArea();

            // getters
            virtual gs_vec2f get_scroll_offset(const bool& _Scaled = true) const override;
            virtual gs_2d_boxf get_visible_rect(ImmediateUserInterfaceContextLayer* _Context) const override;
            virtual bool is_catching_event(ImmediateUserInterfaceContextLayer* _Context) const override;

            // setters
            void set_vertical_scroll_offset(const gs_vec2f _Value, const bool& _Relative = true);
            void set_horizontal_scroll_offset(const gs_vec2f _Value, const bool& _Relative = true);

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;

            virtual void render_background(ImmediateUserInterfaceContextLayer* _Context);

            ScrollBar VerticalScrollBar;
            ScrollBar HorizontalScrollBar;

            bool VerticalScrollIsMoving   = false;
            bool HorizontalScrollIsMoving = false;

            bool ResetVerticalScrollBar   = false;
            bool ResetHorizontalScrollBar = false;

            gs_2d_boxf ContentBox;
            gs_2d_boxf VerticalScrollBarBox;
            gs_2d_boxf HorizontalScrollBarBox;
        };

        // what is it popup
        struct ImmediateUserInterfacePopupScrollArea : public ImmediateUserInterfaceScrollArea
        {
        public:
            ImmediateUserInterfacePopupScrollArea(const std::string& _Name);
            virtual ~ImmediateUserInterfacePopupScrollArea();
            virtual void render_background(ImmediateUserInterfaceContextLayer* _Context) override;

            bool WantsToBeDisabled = false;
        };

        // menu
        struct ImmediateUserInterfaceMenu : public ImmediateUserInterfacePanel
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
            virtual void render_background(ImmediateUserInterfaceContextLayer* _Context) override;
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

        // menubar
        struct ImmediateUserInterfaceMenuBar : public ImmediateUserInterfaceScrollArea
        {
        public:
            ImmediateUserInterfaceMenuBar(const std::string& _Name);
            virtual ~ImmediateUserInterfaceMenuBar();

            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override;
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
            Frenchie::Core::Clock::TimePoint HoverTime {Frenchie::Core::Clock::TimePoint()};
        };

        struct ImmediateUserInterfaceComboboxScrollArea : public ImmediateUserInterfaceScrollArea
        {
        public:
            ImmediateUserInterfaceComboboxScrollArea(const std::string& _Name);
            virtual ~ImmediateUserInterfaceComboboxScrollArea();
            virtual void render_background(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceComboboxItem : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceComboboxItem(const std::string& _Name);
            virtual ~ImmediateUserInterfaceComboboxItem();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        // tree
        struct ImmediateUserInterfaceTreeNode : public ImmediateUserInterfacePanel
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
            gs_2d_boxf                         TitleBox      {gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
            gs_2d_boxf                         IconBox       {gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
            ApplicationRenderingBackendTexture TextureOpened {ApplicationRenderingBackendTexture()};
            ApplicationRenderingBackendTexture TextureClosed {ApplicationRenderingBackendTexture()};
            int                                TreeSettings  {0};
        };

        // table
        struct ImmediateUserInterfaceTable;
        struct ImmediateUserInterfaceTableGrid;
        struct ImmediateUserInterfaceTableGridCell;

        struct ImmediateUserInterfaceTable : public ImmediateUserInterfacePanel
        {
        public:
            ImmediateUserInterfaceTable(const std::string& _Name);
            virtual ~ImmediateUserInterfaceTable();

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr) override;

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;

            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override;

            gs_vec2f                          GridCellSize   {gs_vec2f(256.f, 128.f)};
            int                               GridRowsCount  {0};
            int                               GridColsCount  {0};

            ImmediateUserInterfaceVerticalClipper   VerticalClipper   {ImmediateUserInterfaceVerticalClipper()};
            ImmediateUserInterfaceHorizontalClipper HorizontalClipper {ImmediateUserInterfaceHorizontalClipper()};

        private:

            ImmediateUserInterfaceTableGrid*  DataCells     {nullptr};
            ImmediateUserInterfacePanel*      CorenerHeader {nullptr};
            ImmediateUserInterfaceTableGrid*  RowHeaders    {nullptr};
            ImmediateUserInterfaceTableGrid*  ColumnHeaders {nullptr};

            int ColumnHeadersCount{0};
            int CornerHeaderCount {0};
            int RowHeadersCount   {0};
        };

        struct ImmediateUserInterfaceTableGrid : public ImmediateUserInterfacePanel
        {
        public:
            ImmediateUserInterfaceTableGrid(const std::string& _Name);
            virtual ~ImmediateUserInterfaceTableGrid();
            
            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override;
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void measure(ImmediateUserInterfaceContextLayer* _Context) override;

            gs_vec2f* CellSize  = nullptr;
            int       RowsCount = 0;
            int       ColsCount = 0;
        };

        struct ImmediateUserInterfaceTableGridCell : public ImmediateUserInterfacePanel
        {
        public:
            ImmediateUserInterfaceTableGridCell(const std::string& _Name);
            virtual ~ImmediateUserInterfaceTableGridCell();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;

            int Row    = 0;
            int Column = 0;
        };
        
        struct ImmediateUserInterfaceTableCornerPanel : public ImmediateUserInterfacePanel
        {
        public:
            ImmediateUserInterfaceTableCornerPanel(const std::string& _Name);
            virtual ~ImmediateUserInterfaceTableCornerPanel();
        };

        struct ImmediateUserInterfaceTableVerticalHeader : public ImmediateUserInterfaceTableGridCell
        {
        public:
            ImmediateUserInterfaceTableVerticalHeader(const std::string& _Name);
            virtual ~ImmediateUserInterfaceTableVerticalHeader();
        };

        struct ImmediateUserInterfaceTableHorizontalHeader : public ImmediateUserInterfaceTableGridCell
        {
        public:
            ImmediateUserInterfaceTableHorizontalHeader(const std::string& _Name);
            virtual ~ImmediateUserInterfaceTableHorizontalHeader();
        };

        struct ImmediateUserInterfaceTableCornerHeader : public ImmediateUserInterfacePanel
        {
        public:
            ImmediateUserInterfaceTableCornerHeader(const std::string& _Name);
            virtual ~ImmediateUserInterfaceTableCornerHeader(); 
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        };
        
        // window
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

            static ImmediateUserInterfaceWindow* retrieve_docker_by_view(
                ImmediateUserInterfaceContextLayer* _Context,
                ImmediateUserInterfaceNode*         _DockerView);

            ImmediateUserInterfaceNode*              Docker            {nullptr};
            ImmediateUserInterfaceNode*              TopSnapper        {nullptr};
            ImmediateUserInterfaceNode*              LeftSnapper       {nullptr};
            ImmediateUserInterfaceNode*              RightSnapper      {nullptr};
            ImmediateUserInterfaceNode*              BottomSnapper     {nullptr};
            bool                                     Activate          {false  };
            bool                                     ReattachChildren  {false  };
            bool                                     IsActive          {true   };
            bool*                                    Opened            {nullptr};
            int                                      DockingIndex      {-1     };

            // content
            ImmediateUserInterfaceNode*              RootView          {nullptr};

            // docking
            ImmediateUserInterfaceNode*              DockerView        {nullptr};

            // snapping
            ImmediateUserInterfaceNode*              SnapperView       {nullptr};
            ImmediateUserInterfaceNode*              TopSnapperView    {nullptr};
            ImmediateUserInterfaceNode*              LeftSnapperView   {nullptr};
            ImmediateUserInterfaceNode*              RightSnapperView  {nullptr};
            ImmediateUserInterfaceNode*              BottomSnapperView {nullptr};

            // content
            ImmediateUserInterfaceNode*              ContentView       {nullptr};

            // other auxiliary variables
            std::vector<ImmediateUserInterfaceNode*> DockedWindowsCache{std::vector<ImmediateUserInterfaceNode*>()};
            gs_2d_boxf                                DockedWindowsBox  {gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
        };

        struct ImmediateUserInterfaceWindowDockArea : public ImmediateUserInterfaceWindow
        {
            ImmediateUserInterfaceWindowDockArea(const std::string& _Name);
            virtual ~ImmediateUserInterfaceWindowDockArea();
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceWindowDockGizmo : public ImmediateUserInterfaceWindow
        {
            ImmediateUserInterfaceWindowDockGizmo(const std::string& _Name);
            virtual ~ImmediateUserInterfaceWindowDockGizmo();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override;

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr) override;

            virtual void load_state(ImmediateUserInterfaceContextLayer*) override;
            virtual void save_state(ImmediateUserInterfaceContextLayer*) override;
        };

        struct ImmediateUserInterfaceWindowRoot : public ImmediateUserInterfaceVerticalStack
        {
            ImmediateUserInterfaceWindowRoot(const std::string& _Name);
            virtual ~ImmediateUserInterfaceWindowRoot();
        };

        struct ImmediateUserInterfaceWindowFrame : public ImmediateUserInterfaceScrollArea
        {
        public:
            ImmediateUserInterfaceWindowFrame(const std::string& _Name);
            virtual ~ImmediateUserInterfaceWindowFrame();

            virtual void render_background(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceWindowFrameButton : public ImmediateUserInterfaceNode
        {
        public:
            ImmediateUserInterfaceWindowFrameButton(const std::string& _Name);
            virtual ~ImmediateUserInterfaceWindowFrameButton();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;

            ImmediateUserInterfaceWindow* Window         {nullptr};
            bool                          Pressed        {false};
            gs_2d_boxf                    CloseButtonBox {gs_2d_boxf()};
        };

        // dialogs
        struct ImmediateUserInterfaceDialogContent : public ImmediateUserInterfaceNode
        {
            ImmediateUserInterfaceDialogContent(const std::string& _Name);
            virtual ~ImmediateUserInterfaceDialogContent();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;

            gs_2d_boxf FrameBox   {gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
            gs_2d_boxf ContentBox {gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f))};
        };

        struct ImmediateUserInterfaceDialog : public ImmediateUserInterfacePanel
        {
            ImmediateUserInterfaceDialog(const std::string& _Name);
            virtual ~ImmediateUserInterfaceDialog();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override;

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr) override;

            ImmediateUserInterfaceDialogContent* Contents {nullptr};
            bool*                                Opened   {nullptr};
        };

        // plots
        struct ImmediateUserInterfacePlotAxis : public ImmediateUserInterfaceNode, public IImmediateUserInterfaceScrollArea
        {
            ImmediateUserInterfacePlotAxis(const std::string& _Hash);
            virtual ~ImmediateUserInterfacePlotAxis();

            virtual gs_vec2f get_scroll_offset(const bool& _Scaled = true) const override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            
            gs_vec2f  PreviousOffset {gs_vec2f(0.f, 0.f)};
            gs_vec2f  CurrentOffset  {gs_vec2f(0.f, 0.f)};

            gs_vec2f  MinReference   {gs_vec2f(0.f, -1.f)};
            gs_vec2f  MaxReference   {gs_vec2f(0.f, +1.f)};
            int       TicksCount     {10};

            gs_vec2f  MinScaled      {gs_vec2f(0.f, -1.f)};
            gs_vec2f  MaxScaled      {gs_vec2f(0.f, +1.f)};

            gs_vec2f  ZoomScale      {gs_vec2f(1.f, 1.f)};
            gs_vec2f  MinZoomScale   {gs_vec2f(1e-3, 1e-3)};
            gs_vec2f  MaxZoomScale   {gs_vec2f(1e+3, 1e+3)};

            bool      Edited         {false};
            int       Settings       {0};

        protected:
            gs_vec2f    LabelSize   {gs_vec2f(0.f, 0.f)};
            std::string LabelFormat {"0.00"};
        };

        struct ImmediateUserInterfaceVerticalPlotAxis : public ImmediateUserInterfacePlotAxis
        {
            ImmediateUserInterfaceVerticalPlotAxis(const std::string& _Hash);
            virtual ~ImmediateUserInterfaceVerticalPlotAxis();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfaceHorizontalPlotAxis : public ImmediateUserInterfacePlotAxis
        {
            ImmediateUserInterfaceHorizontalPlotAxis(const std::string& _Hash);
            virtual ~ImmediateUserInterfaceHorizontalPlotAxis();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfacePlot : public ImmediateUserInterfaceNode
        {
            ImmediateUserInterfacePlot(const std::string& _Hash);
            virtual ~ImmediateUserInterfacePlot();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void measure(ImmediateUserInterfaceContextLayer* _Context) override;

            ImmediateUserInterfacePlotAxis* XAxis {nullptr};
            ImmediateUserInterfacePlotAxis* YAxis {nullptr};
            gs_color                        Color {gs_color_rgb(255, 255, 255)};
        };

        struct ImmediateUserInterfacePlotPie : public ImmediateUserInterfacePlot
        {
            ImmediateUserInterfacePlotPie(const std::string& _Hash) : ImmediateUserInterfacePlot(_Hash){}
            virtual ~ImmediateUserInterfacePlotPie(){}
        };

        struct ImmediateUserInterfacePlotVector : public ImmediateUserInterfacePlot
        {
            ImmediateUserInterfacePlotVector(const std::string& _Hash) : ImmediateUserInterfacePlot(_Hash){}
            virtual ~ImmediateUserInterfacePlotVector(){}
        };

        struct ImmediateUserInterfacePlotLegend : public ImmediateUserInterfaceNode
        {
            ImmediateUserInterfacePlotLegend(const std::string& _Hash);
            virtual ~ImmediateUserInterfacePlotLegend();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context) override;

            gs_color  Color    {gs_color_rgb(255, 255, 255)};
            gs_2d_boxf ButtonBox{gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f)};
            bool      Checked  {true};
        };

        struct ImmediateUserInterfacePlotViewItem : public ImmediateUserInterfaceNode
        {
            ImmediateUserInterfacePlotViewItem(const std::string& _Hash);
            virtual ~ImmediateUserInterfacePlotViewItem();
        };

        struct ImmediateUserInterfacePlotView : public ImmediateUserInterfaceNode
        {
            ImmediateUserInterfacePlotView(const std::string& _Hash);
            virtual ~ImmediateUserInterfacePlotView();

            virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void render(ImmediateUserInterfaceContextLayer* _Context) override;
        };

        struct ImmediateUserInterfacePlotWidget : public ImmediateUserInterfacePanel
        {
            ImmediateUserInterfacePlotWidget(const std::string& _Hash);
            virtual ~ImmediateUserInterfacePlotWidget();

            virtual void attach_child(ImmediateUserInterfaceNode* _Child) override;

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr) override;

            // views
            ImmediateUserInterfaceNode* PlotsView    {nullptr};
            ImmediateUserInterfaceNode* XAxisView    {nullptr};
            ImmediateUserInterfaceNode* YAxisView    {nullptr};

            // axis
            ImmediateUserInterfacePlotAxis* CurrentXAxis {nullptr};
            ImmediateUserInterfacePlotAxis* CurrentYAxis {nullptr};
        };

        // canvas
        struct ImmediateUserInterfaceCanvas : public ImmediateUserInterfaceNode
        {
            ImmediateUserInterfaceCanvas(const std::string& _ID) : ImmediateUserInterfaceNode(_ID){}
            virtual ~ImmediateUserInterfaceCanvas(){}

            mutable int CurrentDepth{0};
            mutable int InitialDepth{0};
        };

        // controllers
        class ImmediateUserInterfaceWindowsController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceWindowsController();
            virtual ~ImmediateUserInterfaceWindowsController();
            virtual void frame_start(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_update(ImmediateUserInterfaceContextLayer*) override;
            virtual void frame_input(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_finish(ImmediateUserInterfaceContextLayer*) override;

            std::vector<ImmediateUserInterfaceNode*> retrieve_docked_windows(
                ImmediateUserInterfaceContextLayer*         _Context,
                ImmediateUserInterfaceNode*                 _Docker,
                const ImmediateUserInterfaceDockingAnchor& _Anchors);

        private:

            void place_on_dockers(ImmediateUserInterfaceContextLayer* _Context);
            bool can_be_docked(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked);
            void attach_to_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked, const ImmediateUserInterfaceDockingAnchor& _Anchor);
            void detach_from_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Detached);

            mutable std::vector<ImmediateUserInterfaceNode*>  m_NodesList {std::vector<ImmediateUserInterfaceNode*>()};
            mutable std::vector<ImmediateUserInterfaceNode*>  m_WindowsList  {std::vector<ImmediateUserInterfaceNode*>()};
            mutable ImmediateUserInterfaceWindow*             m_WorkspaceDockArea   {nullptr};
            ImmediateUserInterfaceWindow*                     m_DockGizmo           {nullptr};
            mutable bool                                      m_DockAreaOpened      {false};
            mutable std::string                               m_DockingWorkspaceName{"##DockingWorkspace##"};
            mutable std::string                               m_DockingGizmoName    {"##DockingWorkspaceGizmo##"};
        };
    
        class ImmediateUserInterfaceInputController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceInputController();
            virtual ~ImmediateUserInterfaceInputController();
            virtual void frame_input(ImmediateUserInterfaceContextLayer* _Context) override;

            bool IsCatchingEvent = false;
        };

        class ImmediateUserInterfaceLayoutController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceLayoutController();
            virtual ~ImmediateUserInterfaceLayoutController();
            virtual void frame_start(ImmediateUserInterfaceContextLayer*) override;
            virtual void frame_input(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_render(ImmediateUserInterfaceContextLayer*) override;

        private:

            static void render_node(ImmediateUserInterfaceContextLayer*, ImmediateUserInterfaceNode*);
            static void node_measure(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node);

            mutable std::vector<ImmediateUserInterfaceNode*> m_NodesRenderingCache;
        };

        class ImmediateUserInterfaceMenusAndPopupsController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceMenusAndPopupsController();
            virtual ~ImmediateUserInterfaceMenusAndPopupsController();

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
    
        class ImmediateUserInterfaceNextNodeController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceNextNodeController();
            virtual ~ImmediateUserInterfaceNextNodeController();

            virtual void frame_start(ImmediateUserInterfaceContextLayer*) override;
            
            // API
            void reset();

            // info
            mutable std::optional<int>                         NextLine;
            mutable std::optional<float>                       NextIndent;
            mutable std::optional<gs_vec2f>                    NextPosition;

            mutable std::optional<float>                       NextMaximumWidth;
            mutable std::optional<float>                       NextMaximumHeight;
            mutable std::optional<float>                       NextMinimumWidth;
            mutable std::optional<float>                       NextMinimumHeight;

            mutable std::optional<gs_vec4f>                    NextContentMargin;
            mutable std::optional<gs_vec4f>                    NextContentPadding;
            mutable std::optional<gs_vec2f>                    NextScrollOffset;
            mutable std::optional<bool>                        NextOrderInFollow;

            mutable std::optional<ImmediateUserInterfaceStyle> NextStyle;

            mutable std::optional<int>                         NextRenderingOrder;
            mutable std::optional<gs_vec2f>                    NextAxisScale;
            mutable std::optional<gs_vec2f>                    NextAxisOffset;
        };

        class ImmediateUserInterfaceScrollBarsController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceScrollBarsController();
            virtual ~ImmediateUserInterfaceScrollBarsController();
            virtual void frame_input(ImmediateUserInterfaceContextLayer* _Context) override;

            bool Locked {false};
        };

        class ImmediateUserInterfacePlotsController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfacePlotsController();
            virtual ~ImmediateUserInterfacePlotsController();
            virtual void frame_input(ImmediateUserInterfaceContextLayer* _Context) override;
        private:
            ImmediateUserInterfaceNode* LastFramePlot {nullptr};
        };

        class ImmediateUserInterfaceDragAndDropController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceDragAndDropController();
            virtual ~ImmediateUserInterfaceDragAndDropController();
            virtual void frame_render(ImmediateUserInterfaceContextLayer* _Context) override;
            virtual void frame_finish(ImmediateUserInterfaceContextLayer* _Context) override;

            // API
            void push_data(const std::any& _Data, const std::function<void(const std::any&, const gs_2d_boxf&, const int& _Depth)>& _Preview);
            std::any pop_data();

        protected:
            std::any                                                                   m_Data;
            std::function<void(const std::any&, const gs_2d_boxf&, const int& _Depth)> m_Preview;
        };

        class ImmediateUserInterfaceCommandsQueueController : public ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceCommandsQueueController();
            virtual ~ImmediateUserInterfaceCommandsQueueController();
            virtual void frame_start(ImmediateUserInterfaceContextLayer* _Context) override;
            void push(const std::function<void()>&);

        protected:
            std::vector<std::function<void()>> m_Commands{std::vector<std::function<void()>>()};
        };

        // internal

        // helpers
        namespace ImmediateUserInterfaceContextLayerHelpers
        {
            class ImmediateUserInterfaceMovedNodeSearcher
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
                    if(_Context == nullptr || _Moved == nullptr || !_Moved->is_enabled(_Context))
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

            class ImmediateUserInterfaceHoveredNodeSearcher
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
                    if(_Context == nullptr || _Next == nullptr || !_Next->State.BoundingBox.contains(_Context->m_Input.get_cusor_position()) || !_Next->is_enabled(_Context))
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
                return _Node != nullptr ?
                        gs_max(_Node->Cache.MaximumChildDepth + _Node->Cache.MaximumChildThickness + _Node->Cache.SelfThickness, _Node->Cache.Depth + _Node->Cache.SelfThickness) + 1 :
                            0;
            }

            int calculate_layer_depth(ImmediateUserInterfaceContextLayer* _Context, int _Layer)
            {
                return (int)(_Layer * _Context->m_Renderer->get_far_plane() / (ImmediateUserInterfaceRenderingLayer_::ImmediateUserInterfaceRenderingLayer_End - ImmediateUserInterfaceRenderingLayer_::ImmediateUserInterfaceRenderingLayer_Begin));
            };

            // layouting
            float get_text_line_height(ImmediateUserInterfaceContextLayer* _Context)
            {
                return _Context != nullptr ?
                            _Context->m_Style.get_font_size() + _Context->m_Style.get_frames_width() + _Context->m_Style.get_frames_radius() * 0.5f :
                                0.f;
            }

            gs_vec2f compute_aligned_position(const gs_2d_boxf& _MarginBox, const gs_2d_boxf& _PaddingBox, const int& _Settings)
            {
                float x = _MarginBox.Min.x;
                float y = _MarginBox.Min.y;

                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft)
                    x = _MarginBox.Min.x;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter)
                    x = _MarginBox.center().x - _PaddingBox.size().x * 0.5f;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight)
                    x = _MarginBox.Max.x - _PaddingBox.size().x;
                
                if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop)
                    y = _MarginBox.Min.y;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter)
                    y = _MarginBox.center().y - _PaddingBox.size().y * 0.5f;
                else if(_Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom)
                    y = _MarginBox.Max.y - _PaddingBox.size().y;

                return gs_vec2f(x, y);
            };

            template<typename Type, typename FrameProcessor>
            void layout_nodes_as_panel(
                const ImmediateUserInterfaceContextLayer* _Context,
                const Type&                               _Begin,
                const Type&                               _End,
                const gs_vec2f&                           _Position,
                const gs_vec2f&                           _Size,
                const gs_vec4f&                           _Padding,
                const gs_vec4f&                           _Margin,
                const int&                                _Settings,
                const FrameProcessor&                     _Filter)
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
                gs_2d_boxf marginBox  = gs_2d_boxf(_Position + gs_vec2f(leftMargin, topMargin), _Position + _Size - gs_vec2f(rightMargin, bottomMargin));
                gs_2d_boxf paddingBox = gs_2d_boxf(marginBox.Min + gs_vec2f(leftPadding, topPadding), marginBox.Max - gs_vec2f(rightPadding, bottomPadding));
                gs_vec2f  position   = paddingBox.Min;
                gs_2d_boxf contentBox = gs_2d_boxf(position, position);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) == nullptr || !_Filter(*it) || !(*it)->is_enabled(_Context))
                        continue;
                    
                    (*it)->State.BoundingBox = gs_2d_boxf(
                        position,
                        position + gs_clamp(paddingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));

                    contentBox = gs_2d_boxf(
                        contentBox.Min,
                        contentBox.Max,
                        (*it)->State.BoundingBox.Min,
                        (*it)->State.BoundingBox.Max);
                }

                // align children within padding box
                position = compute_aligned_position(gs_2d_boxf(_Position, _Position + _Size), contentBox, _Settings);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) == nullptr || !_Filter(*it) || !(*it)->is_enabled(_Context))
                        continue;
                    
                    (*it)->State.BoundingBox = gs_2d_boxf(
                        position,
                        position + gs_clamp(paddingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));
                }
            }

            template<typename Type, typename FrameProcessor>
            void layout_nodes_as_vertical_stack(
                const ImmediateUserInterfaceContextLayer* _Context,
                const Type&                               _Begin,
                const Type&                               _End,
                const gs_vec2f&                           _Position,
                const gs_vec2f&                           _Size,
                const gs_vec4f&                           _Padding,
                const gs_vec4f&                           _Margin,
                const int&                                _Settings,
                const FrameProcessor&                     _Filter)
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
                    if(*it == nullptr || !_Filter(*it) || !(*it)->is_enabled(_Context))
                        continue;

                    totalsize += (*it)->State.BoundingBox.size();
                    childCount++;
                }

                totalsize += gs_vec2f(0.f, (float)(childCount - 1) * (topPadding + bottomPadding) * 0.5f);

                // layout children
                gs_2d_boxf marginBox  = gs_2d_boxf(_Position + gs_vec2f(leftMargin, topMargin), _Position - gs_vec2f(rightMargin, bottomMargin) + _Size);
                gs_2d_boxf paddingBox = gs_2d_boxf(marginBox.Min + gs_vec2f(leftPadding, topPadding), marginBox.Max - gs_vec2f(rightPadding, bottomPadding));
                gs_vec2f   scale      = paddingBox.size() / gs_vec2f(gs_max(totalsize.x, 1.f), gs_max(totalsize.y, 1.f));
                gs_vec2f   position   = paddingBox.Min;
                gs_2d_boxf contentBox = gs_2d_boxf(position, position);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if(*it == nullptr || !_Filter(*it) || !(*it)->is_enabled(_Context))
                        continue;
                    
                    gs_vec2f size = gs_vec2f(
                        gs_clamp(paddingBox.width(), (*it)->State.MinimumSize.x, (*it)->State.MaximumSize.x),
                        gs_clamp(((*it)->State.BoundingBox.size() * scale).y, (*it)->State.MinimumSize.y, (*it)->State.MaximumSize.y));

                    (*it)->State.BoundingBox = gs_2d_boxf(position, position + size);

                    position += gs_vec2f(0.f, size.y + (topPadding + bottomPadding) * 0.5f);

                    contentBox = gs_2d_boxf(
                        contentBox.Min,
                        contentBox.Max,
                        (*it)->State.BoundingBox.Min,
                        (*it)->State.BoundingBox.Max);
                }

                // align children within padding box
                position = compute_aligned_position(gs_2d_boxf(_Position, _Position + _Size), contentBox, _Settings);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if((*it) == nullptr || !_Filter(*it) || !(*it)->is_enabled(_Context))
                        continue;
                    
                    (*it)->State.BoundingBox = gs_2d_boxf(
                        position,
                        position + gs_clamp((*it)->State.BoundingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));

                    position += gs_vec2f(0.f, (*it)->State.BoundingBox.size().y + (topPadding + bottomPadding) * 0.5f);
                }
            }

            template<typename Type, typename FrameProcessor>
            void layout_nodes_as_horizontal_stack(
                const ImmediateUserInterfaceContextLayer* _Context,
                const Type&                               _Begin,
                const Type&                               _End,
                const gs_vec2f&                           _Position,
                const gs_vec2f&                           _Size,
                const gs_vec4f&                           _Padding,
                const gs_vec4f&                           _Margin,
                const int&                                _Settings,
                const FrameProcessor&                     _Filter)
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
                    if(*it == nullptr || !_Filter(*it) || !(*it)->is_enabled(_Context))
                        continue;

                    totalsize += (*it)->State.BoundingBox.size();
                    childCount++;
                }

                totalsize += gs_vec2f((float)(childCount - 1) * (leftPadding + rightPadding) * 0.5f, 0.f);

                // layout children
                gs_2d_boxf marginBox  = gs_2d_boxf(_Position + gs_vec2f(leftMargin, topMargin), _Position - gs_vec2f(rightMargin, bottomMargin) + _Size);
                gs_2d_boxf paddingBox = gs_2d_boxf(marginBox.Min + gs_vec2f(leftPadding, topPadding), marginBox.Max - gs_vec2f(rightPadding, bottomPadding));
                gs_vec2f  scale      = paddingBox.size() / gs_vec2f(gs_max(totalsize.x, 1.f), gs_max(totalsize.y, 1.f));
                gs_vec2f  position   = paddingBox.Min;
                gs_2d_boxf contentBox = gs_2d_boxf(position, position);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if(*it == nullptr || !_Filter(*it) || !(*it)->is_enabled(_Context))
                        continue;
                    
                    gs_vec2f size = gs_vec2f(
                        gs_clamp(((*it)->State.BoundingBox.size() * scale).x, (*it)->State.MinimumSize.x, (*it)->State.MaximumSize.x),
                        gs_clamp(paddingBox.height(), (*it)->State.MinimumSize.y, (*it)->State.MaximumSize.y));

                    (*it)->State.BoundingBox = gs_2d_boxf(position, position + size);

                    position += gs_vec2f(size.x + (leftPadding + rightPadding) * 0.5f, 0.f);

                    contentBox = gs_2d_boxf(
                        contentBox.Min,
                        contentBox.Max,
                        (*it)->State.BoundingBox.Min,
                        (*it)->State.BoundingBox.Max);
                }

                // align children within padding box
                position = compute_aligned_position(gs_2d_boxf(_Position, _Position + _Size), contentBox, _Settings);

                for(auto it = _Begin; it != _End; ++it)
                {
                    if(*it == nullptr || !_Filter(*it) || !(*it)->is_enabled(_Context))
                        continue;
                    
                    (*it)->State.BoundingBox = gs_2d_boxf(
                        position,
                        position + gs_clamp((*it)->State.BoundingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));

                    position += gs_vec2f((*it)->State.BoundingBox.size().x + (leftPadding + rightPadding) * 0.5f, 0.f);
                }
            }

            // gizmos
            gs_2d_ellipsef build_resize_top_left_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 32.f);
                
                float WindowResizeAngleGizmoRadius = 32.f;
                
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Min + _Context->m_Style.get_frames_radius() * 0.25f, WindowResizeAngleGizmoRadius);
            }

            gs_2d_ellipsef build_resize_top_right_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Min + gs_vec2f(_Node->get_visible_rect(_Context).width(), 0.f) + gs_vec2f(-_Context->m_Style.get_frames_radius() * 0.25f, _Context->m_Style.get_frames_radius() * 0.25f), WindowResizeAngleGizmoRadius);
            }

            gs_2d_ellipsef build_resize_bottom_left_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 32.f);
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Max - gs_vec2f(_Node->get_visible_rect(_Context).width(), 0.f) + gs_vec2f(_Context->m_Style.get_frames_radius() * 0.25f, - _Context->m_Style.get_frames_radius() * 0.25f), WindowResizeAngleGizmoRadius);
            };

            gs_2d_ellipsef build_resize_bottom_right_ellipse(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_ellipsef(gs_vec2f(0.f, 0.f), 32.f);
                float WindowResizeAngleGizmoRadius = 32.f;
                return gs_2d_ellipsef(_Node->get_visible_rect(_Context).Max - _Context->m_Style.get_frames_radius() * 0.25f, WindowResizeAngleGizmoRadius);
            };

            gs_2d_boxf build_resize_top_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = gs_max(_Context->m_Style.get_frames_width() * 2.f, 16.f);

                return gs_2d_boxf(
                    _Node->get_visible_rect(_Context).Min - gs_vec2f(0.f, WindowResizeSideGizmoWidth),
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(_Node->get_visible_rect(_Context).width(), WindowResizeSideGizmoWidth));
            };

            gs_2d_boxf build_resize_left_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = gs_max(_Context->m_Style.get_frames_width() * 2.f, 16.f);

                return gs_2d_boxf(
                    _Node->get_visible_rect(_Context).Min - gs_vec2f(WindowResizeSideGizmoWidth, 0.f),
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(WindowResizeSideGizmoWidth, _Node->get_visible_rect(_Context).height()));
            };

            gs_2d_boxf build_resize_right_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = gs_max(_Context->m_Style.get_frames_width() * 2.f, 16.f);

                return gs_2d_boxf(
                    _Node->get_visible_rect(_Context).Min + gs_vec2f(_Node->get_visible_rect(_Context).width() - WindowResizeSideGizmoWidth, 0.f),
                    _Node->get_visible_rect(_Context).Max + gs_vec2f(WindowResizeSideGizmoWidth, 0.f));
            };

            gs_2d_boxf build_resize_bottom_box(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
            {
                if(_Context == nullptr || _Node == nullptr) return gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(32.f, 32.f));
                float WindowResizeSideGizmoWidth = gs_max(_Context->m_Style.get_frames_width() * 2.f, 16.f);

                return gs_2d_boxf(
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
                    gs_2d_ellipsef resizeTopLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_left_ellipse(_Context, _Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeTopLeft.Center,
                        resizeTopLeft.MinorRadius,
                        resizeTopLeft.MajorRadius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
                {
                    gs_2d_ellipsef resizeTopRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_right_ellipse(_Context,_Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeTopRight.Center,
                        resizeTopRight.MinorRadius,
                        resizeTopRight.MajorRadius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
                {
                    gs_2d_ellipsef resizeBottomLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_left_ellipse(_Context,_Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeBottomLeft.Center,
                        resizeBottomLeft.MinorRadius,
                        resizeBottomLeft.MajorRadius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
                {
                    gs_2d_ellipsef resizeBottomRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_right_ellipse(_Context,_Node);

                    _Context->m_Renderer->push_arc_filled(
                        resizeBottomRight.Center,
                        resizeBottomRight.MinorRadius,
                        resizeBottomRight.MajorRadius,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)));
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
                {
                    auto resizeTop = ImmediateUserInterfaceContextLayerHelpers::build_resize_top_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_filled(
                        resizeTop.Min,
                        resizeTop.Max,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)),
                        16.f);
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
                {
                    auto resizeLeft = ImmediateUserInterfaceContextLayerHelpers::build_resize_left_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_filled(
                        resizeLeft.Min,
                        resizeLeft.Max,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)),
                        16.f);
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
                {
                    auto resizeRight = ImmediateUserInterfaceContextLayerHelpers::build_resize_right_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_filled(
                        resizeRight.Min,
                        resizeRight.Max,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)),
                        16.f);
                    return;
                }
            
                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
                {
                    auto resizeBottom = ImmediateUserInterfaceContextLayerHelpers::build_resize_bottom_box(_Context, _Node);

                    _Context->m_Renderer->push_rectangle_filled(
                        resizeBottom.Min,
                        resizeBottom.Max,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                        _Context->m_Renderer->calculate_transform_matrix((float)(depth)),
                        16.f);
                    return;
                }
            }

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
            }

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
            }

            void resize_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const ImmediateUserInterfaceNodeEvents& _ResizeEventType)
            {
                if(_Node == nullptr)
                    return;

                gs_2d_boxf box = _Node->State.BoundingBox;

                if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopLeft)
                    box = gs_2d_boxf(_Node->Cache.BoundingBox.Min + _Context->m_Input.get_cusor_drag_delta(), _Node->Cache.BoundingBox.Max);
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTopRight)
                    box = gs_2d_boxf(_Node->Cache.BoundingBox.Min + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y), _Node->Cache.BoundingBox.Max + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f));
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft)
                    box = gs_2d_boxf(_Node->Cache.BoundingBox.Min + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f), _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y));
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottomRight)
                    box = gs_2d_boxf(_Node->Cache.BoundingBox.Min, _Node->Cache.BoundingBox.Max + ApplicationPlatformBackend::get_window_cursor_dragdelta());
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedTop)
                    box = gs_2d_boxf(_Node->Cache.BoundingBox.Min + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y), _Node->Cache.BoundingBox.Max);
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedLeft)
                    box = gs_2d_boxf(_Node->Cache.BoundingBox.Min + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f), _Node->Cache.BoundingBox.Max);
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedRight)
                    box = gs_2d_boxf(_Node->Cache.BoundingBox.Min, _Node->Cache.BoundingBox.Max + gs_vec2f(ApplicationPlatformBackend::get_window_cursor_dragdelta().x, 0.f));
                else if(_ResizeEventType & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsResizedBottom)
                    box = gs_2d_boxf(_Node->Cache.BoundingBox.Min, _Node->Cache.BoundingBox.Max + gs_vec2f(0.f, ApplicationPlatformBackend::get_window_cursor_dragdelta().y));

                _Node->State.BoundingBox = gs_2d_boxf(box.Min, box.Min + gs_clamp(box.size(), _Node->State.MinimumSize, _Node->State.MaximumSize));
            }

            gs_color close_button_color(ImmediateUserInterfaceContextLayer* _Context, const gs_2d_boxf& closeButtonBox)
            {
                if(_Context->m_Input.is_mouse_button_down() && closeButtonBox.contains(_Context->m_Input.get_cusor_position()))
                    return gs_color_rgba(255, 0, 0, 255);

                return closeButtonBox.contains(_Context->m_Input.get_cusor_position()) ?
                    gs_color_rgba(128, 0, 0, 255) : // TODO: this MUST BE a setting
                    gs_color_rgba(64, 0, 0, 255);
            };

            void render_close_button(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node, const gs_2d_boxf& _Box)
            {
                if(_Node == nullptr) return;

                _Context->m_Renderer->push_arc_filled(
                    _Box.center(),
                    _Box.size().x,
                    _Box.size().y,
                    0.f,
                    360.f,
                    close_button_color(_Context, _Box),
                    _Context->m_Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                gs_vec2f lineVector = _Box.size() * 0.5f;
                    
                _Context->m_Renderer->push_line(
                    _Box.center() + gs_vec2f(-lineVector.x, -lineVector.y),
                    _Box.center() + gs_vec2f(+lineVector.x, +lineVector.y),
                    4.f,
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    _Context->m_Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));

                _Context->m_Renderer->push_line(
                    _Box.center() + gs_vec2f(+lineVector.x, -lineVector.y),
                    _Box.center() + gs_vec2f(-lineVector.x, +lineVector.y),
                    4.f,
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    _Context->m_Renderer->calculate_transform_matrix((float)_Node->place_in_follow()));
            };
        }

        struct ImmediateUserInterfaceDefaultInputTextFilter
        {
            bool operator()(const std::string&) const{return true;}
        };

        struct ImmediateUserInterfaceDefaultInputTextCallback
        {
            void operator()(const std::string&) const{}
        };

        enum ImmediateUserInterfaceInputStringInternalSettings_ : int
        {
            ImmediateUserInterfaceInputStringInternalSettings_None        = 0,
            ImmediateUserInterfaceInputStringInternalSettings_NoMultiline = 1 << 0,
        };

        typedef int ImmediateUserInterfaceInputStringInternalSettings;

        template<typename SymbolFilter = ImmediateUserInterfaceDefaultInputTextFilter, typename InputTextCallback = ImmediateUserInterfaceDefaultInputTextCallback>
        bool input_string_internal(
            ImmediateUserInterfaceContextLayer*                      _Context,
            const std::string&                                       _ID,
            std::string&                                             _Text,
            const ImmediateUserInterfaceInputStringSettings&         _InputSettings,
            const ImmediateUserInterfaceInputStringInternalSettings& _InternalSettings,
            const SymbolFilter&                                      _InputTextFilter   = ImmediateUserInterfaceDefaultInputTextFilter(),
            const InputTextCallback&                                 _InputTextCallback = ImmediateUserInterfaceDefaultInputTextCallback())
        {
            struct ImmediateUserInterfaceInputStringRenderingData
            {
                gs_vec2f                            CursorPosition;
                gs_2d_boxf                           TextBoundingBox;
                std::optional<gs_2d_boxf> HoveredSymbolBoundingBox;
                std::optional<int>       HoveredSymbolUtf8CursorPosition;
            };

            struct ImmediateUserInterfaceInputStringContent : public ImmediateUserInterfaceNode
            {
            public:
                ImmediateUserInterfaceInputStringContent(const std::string& _Name): ImmediateUserInterfaceNode(_Name)
                {
                    State.BoundingBox = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 64.f));
                }

                virtual ~ImmediateUserInterfaceInputStringContent(){}

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

                int                              Utf8LeftCursorPosition  = 0;
                int                              Utf8RightCursorPosition = 0;
                Frenchie::Core::Clock::TimePoint CursorAnimtionTimer;
                Frenchie::Core::Clock::TimePoint CursorMovementTimer;
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
                ImmediateUserInterfaceScrollArea*                     _ScrollArea,
                const ImmediateUserInterfaceInputStringRenderingData& _RenderingData)
            {
                // move scroll bar if the text is behind visible area
                if(!_Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.contains(_RenderingData.CursorPosition) && _ScrollArea != nullptr)
                {
                    // adjust horizontal scrollbar
                    if(_RenderingData.CursorPosition.x > _Context->m_Hierarchy.get_parent(_Contents)->Cache.BoundingBox.Max.x)
                    {
                        _ScrollArea->set_horizontal_scroll_offset(
                            (_RenderingData.CursorPosition.x - _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Max.x) + _Context->m_Style.get_font_size());
                    }
                    if(_RenderingData.CursorPosition.x < _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Min.x)
                    {
                        _ScrollArea->set_horizontal_scroll_offset(
                            (_RenderingData.CursorPosition.x - _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Min.x) - _Context->m_Style.get_font_size());
                    }

                    // adjust vertical scrollbar position
                    if(_RenderingData.CursorPosition.y > _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Max.y)
                    {
                        _ScrollArea->set_vertical_scroll_offset(
                            (_RenderingData.CursorPosition.y - _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Max.y) + _Context->m_Style.get_font_size());
                    }
                    if(_RenderingData.CursorPosition.y < _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Min.y)
                    {
                        _ScrollArea->set_vertical_scroll_offset(
                            (_RenderingData.CursorPosition.y - _Context->m_Hierarchy.get_parent(_Contents)->State.BoundingBox.Min.y) - _Context->m_Style.get_font_size());
                    }
                }
            };

            // main code
            if(_Context == nullptr)
                return false;

            ImmediateUserInterfaceInputStringRenderingData inputStringRenderingData;

            // begin widgets
            ImmediateUserInterfaceScrollArea*         scrollArea = nullptr;
            ImmediateUserInterfaceInputStringContent* widget     = nullptr;
            bool                                      edited     = false;

            if(_Context->begin_node<ImmediateUserInterfaceInputStringContent>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                widget     = _Context->get_rendering_stack_top<ImmediateUserInterfaceInputStringContent>();
                scrollArea = dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Context->m_Hierarchy.get_parent(widget));

                gs_2d_boxf boundingBox = gs_2d_boxf(
                    widget->State.BoundingBox.Min - _Context->m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max + _Context->m_Style.get_frames_width());

                gs_vec2f textPosition =
                    (_InternalSettings & ImmediateUserInterfaceInputStringInternalSettings_::ImmediateUserInterfaceInputStringInternalSettings_NoMultiline) ?
                        gs_vec2f(boundingBox.Min.x + _Context->m_Style.get_frames_width() + _Context->m_Style.get_frames_radius() * 0.5f, boundingBox.center().y - _Context->m_Style.get_font_size() * 0.5f) :
                            boundingBox.Min + _Context->m_Style.get_frames_width() + _Context->m_Style.get_frames_radius() * 0.5f;


                inputStringRenderingData.CursorPosition  = textPosition;
                inputStringRenderingData.TextBoundingBox = gs_2d_boxf(textPosition, textPosition);
                inputStringRenderingData.HoveredSymbolUtf8CursorPosition.reset();

                auto parent = _Context->m_Hierarchy.get_parent(widget);

                while (parent != nullptr)
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
                {
                    _Context->m_Renderer->push_clip_box(
                        scrollArea != nullptr ?
                            scrollArea->get_clipping_box(_Context).clip_with(scrollArea->ContentBox) :
                                widget->get_clipping_box(_Context));

                    int      depth  = widget->Cache.Depth;
                    int      init   = depth;
                    float    scale  = _Context->m_Style.get_current_font().get_scale(_Context->m_Style.get_font_size());
                    float    offset = _Context->m_Style.get_current_font().get_offset(_Context->m_Style.get_font_size());

                    // render background and outline
                    {
                        gs_2d_boxf backgroundBox = scrollArea != nullptr ? scrollArea->ContentBox : boundingBox;

                        // outline
                        _Context->m_Renderer->push_rectangle_filled(
                            backgroundBox.Min,
                            backgroundBox.Max,
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                            _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                            _Context->m_Style.get_frames_radius());

                        _Context->m_Renderer->push_rectangle_filled(
                            backgroundBox.Min + _Context->m_Style.get_frames_width(),
                            backgroundBox.Max - _Context->m_Style.get_frames_width(),
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                            _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                            _Context->m_Style.get_frames_radius());
                    }

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
                        else
                        {
                            _Context->m_Renderer->push_text(
                                textPosition,
                                _Text.begin(),
                                _Text.end(),
                                _Context->m_Style.get_font_size(),
                                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                                _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                                _Context->m_Style.get_current_font(),
                                false,
                                [_Context, widget, &inputStringRenderingData, &depth, &scale, &offset](
                                    const gs_2d_boxf&    _CurrentSymbolBoundingBox,
                                    const gs_vec2f&     _CursorPosition,
                                    const int&          _Utf8IteratorPosition,
                                    const unsigned int& _Symbol)
                                {
                                    // calculate text bounding box
                                    inputStringRenderingData.TextBoundingBox = gs_2d_boxf(
                                        inputStringRenderingData.TextBoundingBox.Min,
                                        _CurrentSymbolBoundingBox.Min,
                                        inputStringRenderingData.TextBoundingBox.Max,
                                        _CurrentSymbolBoundingBox.Max);

                                    if(gs_2d_boxf(
                                        _CursorPosition - gs_vec2f(4.f, offset * 0.5f),
                                        _CursorPosition + gs_vec2f(4.f, offset * 0.5f) + _CurrentSymbolBoundingBox.size()).contains(_Context->m_Renderer->get_cursor_postion()))
                                    {
                                        inputStringRenderingData.HoveredSymbolBoundingBox        = _CurrentSymbolBoundingBox;
                                        inputStringRenderingData.HoveredSymbolUtf8CursorPosition = _Utf8IteratorPosition;
                                    }

                                    // calculate cursor geometrical position
                                    if(widget->Utf8LeftCursorPosition == _Utf8IteratorPosition)
                                        inputStringRenderingData.CursorPosition = _CursorPosition;
                                },
                                inputStringCharacterChanger);
                        }

                        if(widget->Utf8LeftCursorPosition > (int)_Text.size())
                            widget->Utf8LeftCursorPosition = (int)_Text.size();

                        if(widget->Utf8RightCursorPosition > (int)_Text.size())
                            widget->Utf8RightCursorPosition = (int)_Text.size();
                    }

                    // render selection bounding box
                    if(widget->Utf8LeftCursorPosition != widget->Utf8RightCursorPosition && !_Text.empty())
                    {
                        _Context->m_Renderer->push_text(
                            textPosition,
                            _Text.begin(),
                            _Text.end(),
                            _Context->m_Style.get_font_size(),
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                            _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                            _Context->m_Style.get_current_font(),
                            true,
                            [_Context, widget, &inputStringRenderingData, &depth, &scale, &offset](
                                const gs_2d_boxf&    _CurrentSymbolBoundingBox,
                                const gs_vec2f&     _CursorPosition,
                                const int&          _Utf8IteratorPosition,
                                const unsigned int& _Symbol)
                            {
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
                            widget->CursorAnimtionTimer = Frenchie::Core::Clock::tic();
                        }
                        else if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(widget->CursorAnimtionTimer, Frenchie::Core::Clock::tic()) > 300)
                        {
                            if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(widget->CursorAnimtionTimer, Frenchie::Core::Clock::tic()) < 700)
                            {
                                _Context->m_Renderer->push_rectangle_filled(
                                    inputStringRenderingData.CursorPosition,
                                    inputStringRenderingData.CursorPosition + gs_vec2f(4.f, _Context->m_Style.get_font_size()),
                                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                                    _Context->m_Renderer->calculate_transform_matrix((float)depth++));
                            }
                            else
                            {
                                widget->CursorAnimtionTimer = Frenchie::Core::Clock::TimePoint();
                            }
                        }
                    }

                    widget->State.SelfThickness += depth - init;

                    _Context->m_Renderer->pop_clip_box();
                }

                // process events
                {
                    // adjust scrollbar
                    if(widget->State.Selected && _Context->m_Input.is_mouse_button_hold() && !_Context->m_Input.is_mouse_button_pressed())
                    {
                        if(scrollArea != nullptr && (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                            scrollArea->set_horizontal_scroll_offset(gs_vector_normalize(_Context->m_Input.get_cusor_drag_delta()) * 4.f);
                        if(scrollArea != nullptr && (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                            scrollArea->set_vertical_scroll_offset(gs_vector_normalize(_Context->m_Input.get_cusor_drag_delta()) * 4.f);
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
                                    widget->CursorMovementTimer = Frenchie::Core::Clock::tic();
                                }
                                else if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(widget->CursorMovementTimer, Frenchie::Core::Clock::tic()) > cursorMovementInterval)
                                {
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                    widget->CursorMovementTimer     = Frenchie::Core::Clock::TimePoint();
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
                                    widget->CursorMovementTimer = Frenchie::Core::Clock::tic();
                                }
                                else if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(widget->CursorMovementTimer, Frenchie::Core::Clock::tic()) > cursorMovementInterval)
                                {
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_right(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                    widget->CursorMovementTimer     = Frenchie::Core::Clock::TimePoint();
                                }
                            }

                            inputStringScrollBarAdjuster(_Context, widget, scrollArea, inputStringRenderingData);
                        }

                        // stop editing on enter
                        else if( 
                            ((_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter) && _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Enter)) ||
                            ((_InputSettings & ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_StopEditOnEscape) && _Context->m_Input.is_key_pressed(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Escape)))
                        {
                            while (parent != nullptr)
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
                                    widget->CursorMovementTimer = Frenchie::Core::Clock::tic();
                                }
                                else if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(widget->CursorMovementTimer, Frenchie::Core::Clock::tic()) > cursorMovementInterval)
                                {
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_up(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                    widget->CursorMovementTimer     = Frenchie::Core::Clock::TimePoint();
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
                                    widget->CursorMovementTimer = Frenchie::Core::Clock::tic();
                                }
                                else if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(widget->CursorMovementTimer, Frenchie::Core::Clock::tic()) > cursorMovementInterval)
                                {
                                    widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_down(widget->Utf8LeftCursorPosition, _Text);
                                    widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;
                                    widget->CursorMovementTimer     = Frenchie::Core::Clock::TimePoint();
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
                                        widget->CursorMovementTimer = Frenchie::Core::Clock::tic();
                                    }
                                    else if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(widget->CursorMovementTimer, Frenchie::Core::Clock::tic()) > cursorMovementInterval)
                                    {
                                        int previousCursorPosition      = widget->Utf8LeftCursorPosition;
                                        widget->Utf8LeftCursorPosition  = ImmediateUserInterfaceInputStringContent::move_cursor_left(widget->Utf8LeftCursorPosition, _Text);
                                        widget->Utf8RightCursorPosition = widget->Utf8LeftCursorPosition;

                                        if(previousCursorPosition - widget->Utf8LeftCursorPosition > 0)
                                            _Text.erase(widget->Utf8LeftCursorPosition, previousCursorPosition - widget->Utf8LeftCursorPosition);

                                        widget->CursorMovementTimer = Frenchie::Core::Clock::TimePoint();
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
                if(scrollArea != nullptr)
                {
                    widget->State.MinimumSize = gs_vec2f(
                        gs_max(inputStringRenderingData.TextBoundingBox.size().x, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context)),
                        gs_max(inputStringRenderingData.TextBoundingBox.size().y, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context)));

                    widget->State.MaximumSize = widget->State.MinimumSize;

                    widget->State.BoundingBox = gs_2d_boxf(
                        widget->State.BoundingBox.Min,
                        widget->State.BoundingBox.Min + gs_clamp(
                            widget->State.BoundingBox.size(),
                            widget->State.MinimumSize,
                            widget->State.MaximumSize));
                }

                _Context->end_node<ImmediateUserInterfaceInputStringContent>();
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
            struct ImmediateUserInterfaceInputScalarPanel : public ImmediateUserInterfacePanel
            {
            public:
                ImmediateUserInterfaceInputScalarPanel(const std::string& _Name) : ImmediateUserInterfacePanel(_Name)
                {
                    State.BoundingBox = gs_2d_boxf(256.f, 64.f);
                }

                virtual ~ImmediateUserInterfaceInputScalarPanel(){}

                virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
                {
                    IsEdited = is_edited(_Context, this);

                    ImmediateUserInterfacePanel::layout(_Context);
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

            // main code
            bool modified = false;

            if(_Context->begin_node<ImmediateUserInterfaceInputScalarPanel>(
                _ID,
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                ImmediateUserInterfaceInputScalarPanel* panel = _Context->get_rendering_stack_top<ImmediateUserInterfaceInputScalarPanel>();

                modified = input_string_internal(
                    _Context,
                    _Context->next_id("Input"),
                    panel->Buffer,

                    // input settings
                      ((_Settings & ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_StopEditOnEscape) ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_StopEditOnEscape   : 0)
                    | ((_Settings & ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter) ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter : 0)
                    | ((_Settings & ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEdit)  ? ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEdit  : 0),
                    
                    // internal settings
                    ImmediateUserInterfaceInputStringInternalSettings_::ImmediateUserInterfaceInputStringInternalSettings_NoMultiline,
                    [](const std::string& _Input)->bool
                    {
                        return true;
                    }
                );

                if(modified)
                {
                    _Input = gs_clamp(Frenchie::Core::String::from_string<Type>(panel->Buffer), _Min, _Max);
                    writeValueToBuffer(panel, _Input, _Format);
                }

                if(!panel->IsEdited)
                    writeValueToBuffer(panel, gs_clamp(_Input, _Min, _Max), _Format);

                // calculate geometry
                {
                    panel->State.MinimumSize = gs_vec2f(
                        panel->State.MinimumSize.x,
                        ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));
                    
                    panel->State.MaximumSize = gs_vec2f(
                        panel->State.MaximumSize.x,
                        gs_max(panel->State.MinimumSize.y, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context)));

                    panel->State.BoundingBox = gs_2d_boxf(
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
                    State.BoundingBox = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 64.f));
                    State.MinimumSize = gs_vec2f(0.f, 64.f);
                    State.MaximumSize = gs_vec2f(gs_huge<float>(), 64.f);
                }

                virtual ~ImmediateUserInterfaceInputScalarSlider(){}

                virtual void render(ImmediateUserInterfaceContextLayer*) override{}

                virtual bool events(ImmediateUserInterfaceContextLayer*) override
                {                    
                    return false;
                }
                
                virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
                {
                    // layout self
                    State.MinimumSize = gs_vec2f(
                        State.MinimumSize.x,
                        ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));
                    
                    State.MaximumSize = gs_vec2f(
                        State.MaximumSize.x,
                        ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));

                    State.BoundingBox = gs_2d_boxf(
                        State.BoundingBox.Min,
                        State.BoundingBox.Min + gs_clamp(State.BoundingBox.size(), State.MinimumSize, State.MaximumSize));
                }

                float SliderPosition         {0.f};
                float SliderPreviousPosition {0.f};
                bool  Edited                 {false};
            };

            if(_Context->begin_node<ImmediateUserInterfaceInputScalarSlider>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                ImmediateUserInterfaceInputScalarSlider* slider =
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceInputScalarSlider>();

                // render
                gs_2d_boxf boundingBox = gs_2d_boxf(
                    slider->State.BoundingBox.Min - _Context->m_Style.get_frames_width(),
                    slider->State.BoundingBox.Max + _Context->m_Style.get_frames_width());

                // render
                {
                    _Context->m_Renderer->push_clip_box(slider->get_clipping_box(_Context));

                    int depth = slider->Cache.Depth;
                    int init  = depth;

                    // render slider box
                    {
                        _Context->m_Renderer->push_rectangle_filled(
                            boundingBox.Min,
                            boundingBox.Max,
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                            _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                            _Context->m_Style.get_frames_radius());

                        _Context->m_Renderer->push_rectangle_filled(
                            boundingBox.Min + _Context->m_Style.get_frames_width(),
                            boundingBox.Max - _Context->m_Style.get_frames_width(),
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                            _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                            _Context->m_Style.get_frames_radius());
                    }

                    // render slider
                    {
                        gs_2d_boxf paletteSlider = gs_2d_boxf(
                            boundingBox.Min + gs_vec2f(slider->SliderPosition, 0.f) * boundingBox.size() * 0.9f,
                            boundingBox.Min + gs_vec2f(slider->SliderPosition, 0.f) * boundingBox.size() * 0.9f + gs_vec2f(boundingBox.width() * 0.1f, boundingBox.height()));

                        // outline
                        _Context->m_Renderer->push_rectangle_filled(
                            paletteSlider.Min,
                            paletteSlider.Max,
                            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                            _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                            _Context->m_Style.get_frames_radius());

                        // background
                        _Context->m_Renderer->push_rectangle_filled(
                            paletteSlider.Min + gs_vec2f(4.f),
                            paletteSlider.Max - gs_vec2f(4.f),
                            paletteSlider.contains(_Context->m_Input.get_cusor_position()) || slider->Edited ?
                                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                            _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                            _Context->m_Style.get_frames_radius());
                    }

                    slider->State.SelfThickness = depth - init;

                    _Context->m_Renderer->pop_clip_box();
                }

                // event processing
                {
                    // stop catching event
                    if(!_Context->m_Input.is_mouse_button_down())
                    {
                        slider->Edited = false;
                    }
                    // catch vertical color palette event
                    else if((boundingBox.contains(_Context->m_Input.get_cusor_position()) &&_Context->m_Input.is_mouse_button_pressed()) || slider->Edited)
                    {
                        if(_Context->m_Input.is_mouse_button_pressed() &&
                            (slider->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                        {
                            slider->SliderPosition = gs_clamp(
                                ceilf(((_Context->m_Input.get_cusor_position() - boundingBox.Min - boundingBox.size() * 0.1f * 0.5f) / boundingBox.size() / 0.9f).x * 100.f / (float)_Delta),
                                1.f / (float)_Delta,
                                100.f) * (float)_Delta / 100.f;
                            
                            slider->SliderPreviousPosition = slider->SliderPosition;
                            slider->Edited                 = true;
                        }
                        else if(slider->Edited)
                        {
                            slider->SliderPosition = gs_clamp(
                                slider->SliderPreviousPosition + gs_clamp(
                                    ceilf( (_Context->m_Input.get_cusor_drag_delta() / boundingBox.size() / 0.9f).x * 100.f / (float)_Delta),
                                    -100.f,
                                    +100.f) * (float)_Delta / 100.f,
                                0.f,
                                1.f);
                        }
                    }
                }

                if(slider->Edited)
                    _Input = (Type)((float)_Min + (float)(_Max - _Min) * slider->SliderPosition);
                else
                    slider->SliderPosition = gs_clamp<float>((float)(_Input - _Min) / (float)(_Max - _Min), 0.f, 1.f);

                _Context->end_node<ImmediateUserInterfaceInputScalarSlider>();
            }

            return true;
        }
    
        template<typename Type>
        void progress_bar_default_internal(
            ImmediateUserInterfaceContextLayer* _Context,
            const std::string&                  _ID,
            Type&                               _Input,
            const Type&                         _Min,
            const Type&                         _Max)
        {
            // nested types
            struct ImmediateUserInterfaceProgressBar : public ImmediateUserInterfaceNode
            {
            public:
                ImmediateUserInterfaceProgressBar(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
                {
                    State.BoundingBox = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(512.f, 64.f));
                }

                virtual ~ImmediateUserInterfaceProgressBar(){}

                virtual void render(ImmediateUserInterfaceContextLayer*) override{}

                virtual bool events(ImmediateUserInterfaceContextLayer*) override
                {                    
                    return false;
                }
                
                virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
                {
                    State.BoundingBox = gs_2d_boxf(
                        State.BoundingBox.Min,
                        State.BoundingBox.Min + gs_clamp(State.BoundingBox.size(), State.MinimumSize, State.MaximumSize));
                }
            };

            if(_Context->begin_node<ImmediateUserInterfaceProgressBar>(
                _ID,
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                ImmediateUserInterfaceProgressBar* widget =
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceProgressBar>();

                float input    = gs_clamp<float>((float)_Input, (float)_Min, (float)_Max) - (float)_Min;
                float range    = gs_max((float)(_Max - _Min), 1.f);
                float progress = input / range;

                gs_2d_boxf boundingBox = gs_2d_boxf(
                    widget->State.BoundingBox.Min - _Context->m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max + _Context->m_Style.get_frames_width());

                // render
                {
                    _Context->m_Renderer->push_clip_box(widget->get_clipping_box(_Context));
                    int depth = widget->Cache.Depth;
                    int init  = depth;

                    // outline
                    _Context->m_Renderer->push_rectangle_filled(
                        boundingBox.Min,
                        boundingBox.Max,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ProgressbarOutline),
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                        _Context->m_Style.get_frames_radius());

                    // background
                    _Context->m_Renderer->push_rectangle_filled(
                        boundingBox.Min + _Context->m_Style.get_frames_width(),
                        boundingBox.Min + _Context->m_Style.get_frames_width() + gs_vec2f((widget->State.BoundingBox.width() - _Context->m_Style.get_frames_width()) * progress, widget->State.BoundingBox.height()),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ProgressbarBackground),
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                        _Context->m_Style.get_frames_radius());

                    // text
                    std::string text = Frenchie::Core::String::format("%.2f %%", (progress * 100.f));

                    _Context->m_Renderer->push_text(
                        boundingBox.center() - _Context->m_Renderer->calculate_bounding_box(text.begin(), text.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() * 0.5f,
                        text.begin(),
                        text.end(),
                        _Context->m_Style.get_font_size(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                        _Context->m_Style.get_current_font());

                    widget->Cache.SelfThickness = depth - init;

                    _Context->m_Renderer->pop_clip_box();
                }

                _Context->end_node<ImmediateUserInterfaceProgressBar>();
            }
        }

        template<typename Type>
        void progress_bar_circular_internal(
            ImmediateUserInterfaceContextLayer* _Context,
            const std::string&                  _ID,
            Type&                               _Input,
            const Type&                         _Min,
            const Type&                         _Max)
        {
            // nested types
            struct ImmediateUserInterfaceProgressBarCircular : public ImmediateUserInterfaceNode
            {
            public:
                ImmediateUserInterfaceProgressBarCircular(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
                {
                    State.BoundingBox = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 256.f));
                }

                virtual ~ImmediateUserInterfaceProgressBarCircular(){}

                virtual void render(ImmediateUserInterfaceContextLayer*) override{}

                virtual bool events(ImmediateUserInterfaceContextLayer*) override
                {                    
                    return false;
                }
                
                virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
                {
                    State.BoundingBox = gs_2d_boxf(
                        State.BoundingBox.Min,
                        State.BoundingBox.Min + gs_clamp(State.BoundingBox.size(), State.MinimumSize, State.MaximumSize));
                }
            };

            if(_Context->begin_node<ImmediateUserInterfaceProgressBarCircular>(
                _ID,
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                ImmediateUserInterfaceProgressBarCircular* widget =
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceProgressBarCircular>();

                float input    = gs_clamp<float>((float)_Input, (float)_Min, (float)_Max) - (float)_Min;
                float range    = gs_max((float)(_Max - _Min), 1.f);
                float progress = input / range;

                gs_2d_boxf boundingBox = gs_2d_boxf(
                    widget->State.BoundingBox.Min - _Context->m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max + _Context->m_Style.get_frames_width());

                // render
                {
                    _Context->m_Renderer->push_clip_box(widget->get_clipping_box(_Context));
                    int depth = widget->Cache.Depth;
                    int init  = depth;

                    // outline
                    _Context->m_Renderer->push_arc_filled(
                        boundingBox.center(),
                        gs_min(boundingBox.width(), boundingBox.height()) * 0.5f,
                        gs_min(boundingBox.width(), boundingBox.height()) * 0.5f,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ProgressbarOutline),
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));

                    _Context->m_Renderer->push_arc_filled(
                        boundingBox.center(),
                        gs_min(boundingBox.width(), boundingBox.height()) * 0.5f - _Context->m_Style.get_frames_width(),
                        gs_min(boundingBox.width(), boundingBox.height()) * 0.5f - _Context->m_Style.get_frames_width(),
                        0.f,
                        360.f * progress,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ProgressbarBackground),
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));
                    
                    // background
                    _Context->m_Renderer->push_arc_filled(
                        boundingBox.center(),
                        gs_min(boundingBox.width(), boundingBox.height()) * 0.5f * 0.75f,
                        gs_min(boundingBox.width(), boundingBox.height()) * 0.5f * 0.75f,
                        0.f,
                        360.f,
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ProgressbarOutline),
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++));

                    // text
                    std::string text = Frenchie::Core::String::format("%.2f %%", (progress * 100.f));

                    _Context->m_Renderer->push_text(
                        boundingBox.center() - _Context->m_Renderer->calculate_bounding_box(text.begin(), text.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() * 0.5f,
                        text.begin(),
                        text.end(),
                        _Context->m_Style.get_font_size(),
                        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                        _Context->m_Style.get_current_font());

                    widget->State.SelfThickness = depth - init;

                    _Context->m_Renderer->pop_clip_box();
                }

                _Context->end_node<ImmediateUserInterfaceProgressBarCircular>();
            }
        }
    }
}

// ImmediateUserInterfaceStyle
ImmediateUserInterfaceStyle::ImmediateUserInterfaceStyle()
{
    Colors.resize(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End);

    // general ui elements
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground]                  = gs_color_rgba(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground]                 = gs_color_rgba(28, 28, 28, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered]          = gs_color_rgba(72, 82, 72, 255);
    
    // push button
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline]                    = gs_color_rgba(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground]                 = gs_color_rgba(32, 32, 32, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered]          = gs_color_rgba(60, 72, 60, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed]          = gs_color_rgba(120, 128, 120, 255);

    // menus
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground]             = gs_color_rgba(32, 32, 32, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered]      = gs_color_rgba(60, 72, 60, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed]      = gs_color_rgba(120, 128, 120, 255);

    // scrollbar
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground]        = gs_color_rgba(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered] = gs_color_rgba(72, 82, 72, 255);

    // progressbar
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ProgressbarOutline]               = gs_color_rgba(72, 72, 72, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ProgressbarBackground]            = gs_color_rgba(120, 128, 120, 255);

    // gizmos
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos]                           = gs_color_rgba(50, 50, 100, 200);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered]                    = gs_color_rgba(100, 100, 172, 255);

    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsAxis]                      = gs_color_rgba(50, 50, 100, 200);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsAxisTicks]                 = gs_color_rgba(100, 100, 172, 255);
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsBackground]                = gs_color_rgba(50, 50, 100, 200);

    // text
    Colors[ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text]                             = gs_color_rgba(255, 255, 255, 255);
}

ImmediateUserInterfaceStyle::~ImmediateUserInterfaceStyle(){}

float ImmediateUserInterfaceStyle::get_minimum_frames_radius() const
{
    return 0.f;
}

float ImmediateUserInterfaceStyle::get_maximum_frames_radius() const
{
    return 32.f;
}

float& ImmediateUserInterfaceStyle::get_frames_radius() const
{
    FramesRadius = gs_clamp(FramesRadius, get_minimum_frames_radius(), get_maximum_frames_radius());
    return FramesRadius;
}

float ImmediateUserInterfaceStyle::get_minimum_frames_width() const
{
    return 4.f;
}

float ImmediateUserInterfaceStyle::get_maximum_frames_width() const
{
    return 8.f;
}

float& ImmediateUserInterfaceStyle::get_frames_width() const
{
    FramesWidth = gs_clamp(FramesWidth, get_minimum_frames_width(), get_maximum_frames_width());
    return FramesWidth;
}

float ImmediateUserInterfaceStyle::get_minimum_font_size() const
{
    return 24.f;
}

float ImmediateUserInterfaceStyle::get_maximum_font_size() const
{
    return 128.f;
}

float& ImmediateUserInterfaceStyle::get_font_size() const
{
    FontSize = gs_clamp(FontSize, get_minimum_font_size(), get_maximum_font_size());
    return FontSize;
}

float ImmediateUserInterfaceStyle::get_minimum_scrollbar_width() const
{
    return 32.f;
}

float ImmediateUserInterfaceStyle::get_maximum_scrollbar_width() const
{
    return get_maximum_frames_radius() * 2.f;
}

float& ImmediateUserInterfaceStyle::get_scrollbar_width() const
{
    ScrollBarWidth = gs_clamp(ScrollBarWidth, get_minimum_scrollbar_width(), get_maximum_scrollbar_width());
    return ScrollBarWidth;
}

float& ImmediateUserInterfaceStyle::get_popup_menu_pointer_size() const
{
    PopupMenuPointerSize = gs_min(gs_max(PopupMenuPointerSize, 32.f), get_font_size() - 2.f * get_frames_width());

    return PopupMenuPointerSize;
}

ApplicationRenderingBackendFont ImmediateUserInterfaceStyle::get_current_font() const
{
    return Font.is_null() ? ApplicationRenderingBackend::get_default_font() : Font;
}

gs_color& ImmediateUserInterfaceStyle::get_color(const ImmediateUserInterfaceNodeColors_& _Color) const
{
    return Colors[_Color];
}

std::string ImmediateUserInterfaceStyle::style_color_to_string(const ImmediateUserInterfaceNodeColors_& _Color, bool _Camel) const
{
    switch (_Color)
    {
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground:                  return !_Camel ? "Child background"                     : "ChildBackground";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground:                 return !_Camel ? "Parent background"                    : "ParentBackground";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered:          return !_Camel ? "Parent background hovered"            : "ParentBackgroundHovered";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline:                    return !_Camel ? "Button outline"                       : "ButtonOutline";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground:                 return !_Camel ? "Button background"                    : "ButtonBackground";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered:          return !_Camel ? "Button background hovered"            : "ButtonBackgroundHovered";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed:          return !_Camel ? "Button background pressed"            : "ButtonBackgroundPressed";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground:        return !_Camel ? "Scroll bar slider background"         : "ScrollBarSliderBackground";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered: return !_Camel ? "Scroll bar slider background hovered" : "ScrollBarSliderBackgroundHovered";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground:             return !_Camel ? "Menu action background"               : "MenuActionBackground";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered:      return !_Camel ? "Menu action background hovered"       : "MenuActionBackgroundHovered";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed:      return !_Camel ? "Menu action background pressed"       : "MenuActionBackgroundPressed";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos:                           return !_Camel ? "Gizmos"                               : "Gizmos";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered:                    return !_Camel ? "Gizmos hovered"                       : "GizmosHovered";

        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ProgressbarOutline:               return !_Camel ? "Progressbar outline"                  : "ProgressbarOutline";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ProgressbarBackground:            return !_Camel ? "Progressbar background"               : "ProgressbarBackground";

        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsAxis:                      return !_Camel ? "2D plots axis"                        : "2DPlotsAxis";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsAxisTicks:                 return !_Camel ? "2D plots axis ticks"                  : "2DPlotsAxisTicks";
        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsBackground:                return !_Camel ? "2D plots background"                  : "2DPlotsBackground";

        case ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text:                             return !_Camel ? "Text"                                 : "Text";
        default:                                                                                                   return Frenchie::Core::String::format("Unknown color-%d", _Color);
    }

    return Frenchie::Core::String::format("Unknown color-%d", _Color);
}

// ImmediateUserInterfaceInput
ImmediateUserInterfaceInput::ImmediateUserInterfaceInput(ImmediateUserInterfaceContextLayer* _Context) : m_Context(_Context){}

gs_vec2f ImmediateUserInterfaceInput::get_cusor_position() const
{
    return m_Context != nullptr && m_Context->m_Renderer != nullptr ?
            gs_vec2f(m_Context->m_Renderer->get_cursor_postion().x, m_Context->m_Renderer->get_cursor_postion().y) :
                gs_vec2f(0.f, 0.f);
}

gs_vec2f ImmediateUserInterfaceInput::get_cusor_drag_delta() const
{
    return ApplicationPlatformBackend::get_window_cursor_dragdelta();
}

gs_vec2f ImmediateUserInterfaceInput::get_mouse_wheel_scroll_offset() const
{
    return ApplicationPlatformBackend::get_mouse_wheel_scroll_offset();
}

std::string ImmediateUserInterfaceInput::get_input_text() const
{
    return ApplicationPlatformBackend::input_text();
}

std::string ImmediateUserInterfaceInput::get_clipboard_text() const
{
    return ApplicationPlatformBackend::get_clipboard_text();
}

bool ImmediateUserInterfaceInput::has_input_text() const
{
    return ApplicationPlatformBackend::has_input_text();
}

bool ImmediateUserInterfaceInput::has_clipboard_text() const
{
    return ApplicationPlatformBackend::has_clipboard_text();
}

void ImmediateUserInterfaceInput::set_clipboard_text(const std::string& _Value)
{
    ApplicationPlatformBackend::set_clipboard_text(_Value);
}

bool ImmediateUserInterfaceInput::is_mouse_button_down() const
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

bool ImmediateUserInterfaceInput::is_mouse_button_hold() const
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

bool ImmediateUserInterfaceInput::is_mouse_button_pressed() const
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

bool ImmediateUserInterfaceInput::is_mouse_button_released() const
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

bool ImmediateUserInterfaceInput::is_mouse_button_clicked() const
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

bool ImmediateUserInterfaceInput::is_mouse_button_double_clicked() const
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

bool ImmediateUserInterfaceInput::is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_down(_Button);
}

bool ImmediateUserInterfaceInput::is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_hold(_Button);
}

bool ImmediateUserInterfaceInput::is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_pressed(_Button);
}

bool ImmediateUserInterfaceInput::is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_released(_Button);
}

bool ImmediateUserInterfaceInput::is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_clicked(_Button);
}

bool ImmediateUserInterfaceInput::is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const
{
    return ApplicationPlatformBackend::is_mouse_button_double_clicked(_Button);
}

bool ImmediateUserInterfaceInput::is_key_down() const
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

bool ImmediateUserInterfaceInput::is_key_hold() const
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

bool ImmediateUserInterfaceInput::is_key_pressed() const
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

bool ImmediateUserInterfaceInput::is_key_released() const
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

bool ImmediateUserInterfaceInput::is_key_clicked() const
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

bool ImmediateUserInterfaceInput::is_key_down(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_down(_Key);
}

bool ImmediateUserInterfaceInput::is_key_hold(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_hold(_Key);
}

bool ImmediateUserInterfaceInput::is_key_pressed(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_pressed(_Key);
}

bool ImmediateUserInterfaceInput::is_key_released(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_released(_Key);
}

bool ImmediateUserInterfaceInput::is_key_clicked(const ApplicationPlatformBackendKey::Key& _Key) const
{
    return ApplicationPlatformBackend::is_key_clicked(_Key);
}

bool ImmediateUserInterfaceInput::has_modifier(const ApplicationPlatformBackendKeyModifier::Modifier& _Modifier) const
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

template<> gs_color ImmediateUserInterfaceContextConfiguration::get<gs_color>(const std::string& _Section, const std::string& _Name)
{
    return Frenchie::Core::String::from_string<gs_color>(m_Configuration[_Section][_Name]);
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
void ImmediateUserInterfaceContextConfiguration::set<gs_color>(const std::string& _Section, const std::string& _Name, const gs_color& _Value)
{
    m_Configuration[_Section][_Name] = Frenchie::Core::String::to_string<gs_color>(_Value);
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

// ImmediateUserInterfaceNode
ImmediateUserInterfaceNode::ImmediateUserInterfaceNode(const std::string& _Hash) : Hash(_Hash){}
ImmediateUserInterfaceNode::~ImmediateUserInterfaceNode(){}

void ImmediateUserInterfaceNode::render(ImmediateUserInterfaceContextLayer*)
{
}

void ImmediateUserInterfaceNode::layout(ImmediateUserInterfaceContextLayer*){}

void ImmediateUserInterfaceNode::measure(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
    {
        State.ContentSize = gs_2d_boxf(State.BoundingBox.Min, State.BoundingBox.Min).size();
        return;
    }

    gs_2d_boxf box = gs_2d_boxf(State.BoundingBox.Min, State.BoundingBox.Min);

    for (auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        box = gs_2d_boxf(
            box.Min,
            (*it)->State.BoundingBox.Min,
            box.Max,
            (*it)->State.BoundingBox.Max);
    }

    if(MeasuringCount < 3)
    {
        State.ContentSize = box.size();
        MeasuringCount++;
    }
    else
    {
        MeasuringCount = 0;
    }
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
            movable->State.BoundingBox = gs_2d_boxf(
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
void ImmediateUserInterfaceNode::restore(){}

gs_2d_boxf ImmediateUserInterfaceNode::get_clipping_box(ImmediateUserInterfaceContextLayer* _Context) const
{
    // auxiliary lambdas
    auto calculate_clipping_box = [](ImmediateUserInterfaceContextLayer* _Context, const ImmediateUserInterfaceNode* _Node)->gs_2d_boxf
    {
        if(_Node == nullptr)
        {
            return _Context != nullptr ?
                        _Context->m_Renderer->current_viewport() :
                            gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(gs_huge<float>(), gs_huge<float>()));
        }

        const ImmediateUserInterfaceNode* next   = _Node;
        ImmediateUserInterfaceNode*       parent = _Context->m_Hierarchy.get_parent(_Node);

        gs_2d_boxf clippingBox = next->State.BoundingBox;

        while (parent)
        {
            next        = parent;
            parent      = _Context->m_Hierarchy.get_parent(parent);
            clippingBox = clippingBox.clip_with(next->get_visible_rect(_Context));
        }

        return gs_2d_boxf(clippingBox.Min - _Context->m_Style.get_frames_width(), clippingBox.Max + _Context->m_Style.get_frames_width());
    };

    // main code
    if(!State.ClippingBox.has_value())
        State.ClippingBox = calculate_clipping_box(_Context, this);

    return State.ClippingBox.value();
}

gs_2d_boxf ImmediateUserInterfaceNode::get_visible_rect(ImmediateUserInterfaceContextLayer* _Context) const
{
    return State.BoundingBox.clip_with(get_clipping_box(_Context));
}

bool ImmediateUserInterfaceNode::is_partially_visible(ImmediateUserInterfaceContextLayer* _Context) const
{
    gs_2d_boxf clippingBox = get_clipping_box(_Context);

    return gs_2d_boxf(
        State.BoundingBox.Min - gs_max(State.BoundingBox.width(), State.BoundingBox.height()),
        State.BoundingBox.Max + gs_max(State.BoundingBox.width(), State.BoundingBox.height())).overlaps(clippingBox) &&
        gs_min(clippingBox.size().x, clippingBox.size().y) > _Context->m_Style.get_frames_width() * 2.f;
}

bool ImmediateUserInterfaceNode::is_catching_event(ImmediateUserInterfaceContextLayer* _Context) const
{
    return _Context != nullptr && get_visible_rect(_Context).contains(_Context->m_Input.get_cusor_position());
}

bool ImmediateUserInterfaceNode::is_enabled(const ImmediateUserInterfaceContextLayer* _Context) const
{
    if(_Context == nullptr)
        return false;

    bool enabled = Active;
    auto parent  = _Context->m_Hierarchy.get_parent(this);

    while (parent)
    {
        enabled = enabled && parent->Active;
        parent  = _Context->m_Hierarchy.get_parent(parent);
    }

    return enabled;
}

int ImmediateUserInterfaceNode::place_in_follow()
{
    return Cache.Depth + (++State.SelfThickness);
}

int ImmediateUserInterfaceNode::get_rendering_order() const
{
    return RenderingOrder;
}

void ImmediateUserInterfaceNode::set_rendering_order(const int& _RenderingOrder)
{
    if(!NextRenderingOrder.has_value())
        RenderingOrder = _RenderingOrder;
}

void ImmediateUserInterfaceNode::next_rendering_order()
{
    if(NextRenderingOrder.has_value())
        RenderingOrder = NextRenderingOrder.value();
    NextRenderingOrder.reset();
}

void ImmediateUserInterfaceNode::enable()
{
    Active = true;
}

void ImmediateUserInterfaceNode::disable()
{
    Active = false;
}

// ImmediateUserInterfaceHierarchy
ImmediateUserInterfaceHierarchy::ImmediateUserInterfaceHierarchy(const std::function<ImmediateUserInterfaceNode*(const ImmediateUserInterfaceNode*)> _GetParent) : GetParent(_GetParent){}

ImmediateUserInterfaceHierarchy::~ImmediateUserInterfaceHierarchy(){}

std::vector<ImmediateUserInterfaceNode*>::iterator ImmediateUserInterfaceHierarchy::begin(const ImmediateUserInterfaceNode* _Node) const
{
    if( _Node == nullptr                                            ||
        _Node->State.RenderingIndex          >= (int)Indexes.size() ||
        Indexes[_Node->State.RenderingIndex] >= (int)Sorted.size())
    {
        return Sorted.end();
    }

    return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.RenderingIndex];
}

std::vector<ImmediateUserInterfaceNode*>::iterator ImmediateUserInterfaceHierarchy::end(const ImmediateUserInterfaceNode* _Node) const
{
    if(_Node == nullptr                                                 ||
        _Node->State.RenderingIndex + 1          >= (int)Indexes.size() ||
        Indexes[_Node->State.RenderingIndex + 1] >= (int)Sorted.size())
    {
        return Sorted.end();
    }

    return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.RenderingIndex + 1];
}

int ImmediateUserInterfaceHierarchy::size(const ImmediateUserInterfaceNode* _Node) const
{
    return (int)(end(_Node) - begin(_Node));
}

void ImmediateUserInterfaceHierarchy::build(const std::vector<ImmediateUserInterfaceNode*>& _Nodes)
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

// ImmediateUserInterfacePanel
ImmediateUserInterfacePanel::ImmediateUserInterfacePanel(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfacePanel::~ImmediateUserInterfacePanel(){}

void ImmediateUserInterfacePanel::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context,
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min,
        State.BoundingBox.size(),
        ContentPadding,
        ContentMargin,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

void ImmediateUserInterfacePanel::measure(ImmediateUserInterfaceContextLayer* _Context)
{
    ImmediateUserInterfaceNode::measure(_Context);

    if(!(State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren)) return;

    if(_Context->m_Hierarchy.size(this) <= 0)
    {
        if(!LastSize.has_value())
            LastSize = gs_vec2f(gs_max(256.f, State.BoundingBox.width()), gs_max(256.f, State.BoundingBox.height()));

        if(!LastMinimumSize.has_value())
            LastMinimumSize = State.MinimumSize;

        if(!LastMaximumSize.has_value())
            LastMaximumSize = State.MaximumSize;

        State.BoundingBox = gs_2d_boxf(State.BoundingBox.Min, State.BoundingBox.Min);
        State.MinimumSize = gs_vec2f(4.f, 4.f);
        State.MaximumSize = gs_vec2f(4.f, 4.f);
    }
    else
    {
        if(LastSize.has_value())
            State.BoundingBox = gs_2d_boxf(State.BoundingBox.Min, State.BoundingBox.Min + LastSize.value());
        
        if(LastMinimumSize.has_value())
            State.MinimumSize = LastMinimumSize.value();
        
        if(LastMaximumSize.has_value())
            State.MaximumSize = LastMaximumSize.value();

        LastSize.reset();
        LastMinimumSize.reset();
        LastMaximumSize.reset();
    }
}

bool ImmediateUserInterfacePanel::events(ImmediateUserInterfaceContextLayer* _Context)
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

void ImmediateUserInterfacePanel::restore()
{
    ContentMargin  = gs_vec4f(0.f);
    ContentPadding = gs_vec4f(0.f);
}

// ImmediateUserInterfaceNodeVerticalStack
ImmediateUserInterfaceVerticalStack::ImmediateUserInterfaceVerticalStack(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}
ImmediateUserInterfaceVerticalStack::~ImmediateUserInterfaceVerticalStack(){}

void ImmediateUserInterfaceVerticalStack::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
    {
        if(!MinimumSizeBeforeResizeToContents.has_value())
            MinimumSizeBeforeResizeToContents = State.MinimumSize;

        if(!MaximumSizeBeforeResizeToContents.has_value())
            MaximumSizeBeforeResizeToContents = State.MaximumSize;

        State.MinimumSize = gs_vec2f(State.ContentSize.x, State.MinimumSize.y);
        State.MaximumSize = gs_vec2f(State.ContentSize.x, State.MaximumSize.y);
    }
    else
    {
        if(MinimumSizeBeforeResizeToContents.has_value())
            State.MinimumSize = MinimumSizeBeforeResizeToContents.value();
        MinimumSizeBeforeResizeToContents.reset();

        if(MaximumSizeBeforeResizeToContents.has_value())
            State.MaximumSize = MaximumSizeBeforeResizeToContents.value();
        MaximumSizeBeforeResizeToContents.reset();
    }

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_vertical_stack(
        _Context,
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
ImmediateUserInterfaceHorizontalStack::ImmediateUserInterfaceHorizontalStack(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}
ImmediateUserInterfaceHorizontalStack::~ImmediateUserInterfaceHorizontalStack(){}

void ImmediateUserInterfaceHorizontalStack::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
    {
        if(!MinimumSizeBeforeResizeToContents.has_value())
            MinimumSizeBeforeResizeToContents = State.MinimumSize;

        if(!MaximumSizeBeforeResizeToContents.has_value())
            MaximumSizeBeforeResizeToContents = State.MaximumSize;

        State.MinimumSize = gs_vec2f(State.MinimumSize.x, State.ContentSize.y);
        State.MaximumSize = gs_vec2f(State.MaximumSize.x, State.ContentSize.y);
    }
    else
    {
        if(MinimumSizeBeforeResizeToContents.has_value())
            State.MinimumSize = MinimumSizeBeforeResizeToContents.value();
        MinimumSizeBeforeResizeToContents.reset();

        if(MaximumSizeBeforeResizeToContents.has_value())
            State.MaximumSize = MaximumSizeBeforeResizeToContents.value();
        MaximumSizeBeforeResizeToContents.reset();
    }
        
    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_horizontal_stack(
        _Context,
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min,
        State.BoundingBox.size(),
        ContentPadding,
        ContentMargin,
        State.Settings,
        [](const ImmediateUserInterfaceNode*){return true;});
}

// ImmediateUserInterfaceGrid
ImmediateUserInterfaceGrid::ImmediateUserInterfaceGrid(const std::string& _Hash) : ImmediateUserInterfacePanel(_Hash){}
ImmediateUserInterfaceGrid::~ImmediateUserInterfaceGrid(){}

void ImmediateUserInterfaceGrid::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

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

    // calculate number of rows and columns
    int rowsCount = -1;
    int colsCount = -1;

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        ImmediateUserInterfaceGridPlace* gridPlace =
            dynamic_cast<ImmediateUserInterfaceGridPlace*>(*it);

        if(gridPlace == nullptr || !gridPlace->is_enabled(_Context))
            continue;

        rowsCount = gs_max(rowsCount, gridPlace->Row);
        colsCount = gs_max(colsCount, gridPlace->Column);
    }

    ++rowsCount;
    ++colsCount;

    gs_2d_boxf marginBox = gs_2d_boxf(
        State.BoundingBox.Min + gs_vec2f(leftMargin, topMargin),
        State.BoundingBox.Max - gs_vec2f(rightMargin, bottomMargin));

    gs_2d_boxf paddingBox = gs_2d_boxf(
        marginBox.Min + gs_vec2f(leftPadding, topPadding) * gs_vec2f(rowsCount, colsCount),
        marginBox.Max - gs_vec2f(rightPadding, bottomPadding) * gs_vec2f(rowsCount, colsCount));

    // fill cells cache
    Cells.resize(rowsCount * colsCount);

    for (int row = 0; row < rowsCount; row++)
    {
        for (int col = 0; col < colsCount; col++)
        {
            if(row * colsCount + col < CellsCache.size())
            {
                Cells[row * colsCount + col] = CellsCache[row * colsCount + col];
            }
            else
            {
                Cells[row * colsCount + col] = gs_2d_boxf(
                    paddingBox.Min,
                    paddingBox.Min + gs_vec2f(paddingBox.width() / gs_max(colsCount, 1), paddingBox.height() / gs_max(rowsCount, 1)));
            }
        }
    }

    ImmediateUserInterfaceGridPlace* modifiedPlace = nullptr;

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        ImmediateUserInterfaceGridPlace* place =
            dynamic_cast<ImmediateUserInterfaceGridPlace*>(*it);

        if(place == nullptr || !place->is_enabled(_Context))
            continue;

        Cells[place->Row * colsCount + place->Column] = place->State.BoundingBox;

        for (size_t row = 0; row < rowsCount; row++)
        {
            Cells[row * colsCount + place->Column] = gs_2d_boxf(
                Cells[row * colsCount + place->Column].Min,
                Cells[row * colsCount + place->Column].Min + gs_vec2f(
                    Cells[place->Row * colsCount + place->Column].width(),
                    Cells[row * colsCount + place->Column].height()));
        }

        for (size_t col = 0; col < colsCount; col++)
        {
            Cells[place->Row * colsCount + col] = gs_2d_boxf(
                Cells[place->Row * colsCount + col].Min,
                Cells[place->Row * colsCount + col].Min + gs_vec2f(
                    Cells[place->Row * colsCount + col].width(),
                    Cells[place->Row * colsCount + place->Column].height()));
        }

        if(place->State.Events != ImmediateUserInterfaceNodeEvents_None)
            modifiedPlace = place;
    }

    // layout colums horizontally
    for (size_t row = 0; row < rowsCount; row++)
    {
        gs_vec2f total;

        for (int col = 0; col < colsCount; col++)
            total += Cells[row * colsCount + col].size();

        for (int col = 0; col < colsCount; col++)
        {
            Cells[row * colsCount + col] = gs_2d_boxf(
                Cells[row * colsCount + col].Min,
                Cells[row * colsCount + col].Min + gs_vec2f(
                    Cells[row * colsCount + col].width() / total.x * paddingBox.width(),
                    Cells[row * colsCount + col].height()));
        }
    }

    // layout rows vertically
    for (int col = 0; col < colsCount; col++)
    {
        gs_vec2f total;

        for (int row = 0; row < rowsCount; row++)
            total += Cells[row * colsCount + col].size();

        for (int row = 0; row < rowsCount; row++)
        {
            Cells[row * colsCount + col] = gs_2d_boxf(
                Cells[row * colsCount + col].Min,
                Cells[row * colsCount + col].Min + gs_vec2f(
                    Cells[row * colsCount + col].width(),
                    Cells[row * colsCount + col].height() / total.y * paddingBox.height()));
        }
    }

    // adjust row height and column width
    if(modifiedPlace)
    {
        for (size_t row = 0; row < rowsCount; row++)
        {
            Cells[row * colsCount + modifiedPlace->Column] = gs_2d_boxf(
                Cells[row * colsCount + modifiedPlace->Column].Min,
                Cells[row * colsCount + modifiedPlace->Column].Min + gs_vec2f(
                    Cells[modifiedPlace->Row * colsCount + modifiedPlace->Column].width(),
                    Cells[row * colsCount + modifiedPlace->Column].height()));
        }

        for (size_t col = 0; col < colsCount; col++)
        {
            Cells[modifiedPlace->Row * colsCount + col] = gs_2d_boxf(
                Cells[modifiedPlace->Row * colsCount + col].Min,
                Cells[modifiedPlace->Row * colsCount + col].Min + gs_vec2f(
                    Cells[modifiedPlace->Row * colsCount + col].width(),
                    Cells[modifiedPlace->Row * colsCount + modifiedPlace->Column].height()));
        }
    }

    // layout children
    gs_vec2f origin   = ImmediateUserInterfaceContextLayerHelpers::compute_aligned_position(State.BoundingBox, paddingBox, State.Settings);
    gs_vec2f position = origin;

    for (int row = 0; row < rowsCount; row++)
    {
        for (int col = 0; col < colsCount; col++)
        {
            Cells[row * colsCount + col] = gs_2d_boxf(position, position + Cells[row * colsCount + col].size());
            position += gs_vec2f(Cells[row * colsCount + col].width() + (leftPadding + rightPadding) * 0.5f, 0.f);
        }

        position = gs_vec2f(origin.x, position.y + Cells[row * colsCount].height() + (topPadding + bottomPadding) * 0.5f);
    }

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        ImmediateUserInterfaceGridPlace* place =
            dynamic_cast<ImmediateUserInterfaceGridPlace*>(*it);

        if(place == nullptr || !place->is_enabled(_Context))
            continue;

        place->State.BoundingBox = gs_2d_boxf(
            Cells[place->Row * colsCount + place->Column].Min,
            Cells[place->Row * colsCount + place->Column].Min + gs_clamp(
                Cells[place->Row * colsCount + place->Column].size(),
                place->State.MinimumSize,
                place->State.MaximumSize));
    }

    CellsCache = Cells;
}

// ImmediateUserInterfaceGridPlace
ImmediateUserInterfaceGridPlace::ImmediateUserInterfaceGridPlace(const std::string& _Hash) : ImmediateUserInterfacePanel(_Hash){}
ImmediateUserInterfaceGridPlace::~ImmediateUserInterfaceGridPlace(){}

void ImmediateUserInterfaceGridPlace::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    GS_ASSERT(dynamic_cast<ImmediateUserInterfaceGrid*>(
        _Context->m_Hierarchy.get_parent(this)) != nullptr);

    ImmediateUserInterfacePanel::layout(_Context);
}

// ImmediateUserInterfaceScrollArea
ImmediateUserInterfaceScrollArea::ImmediateUserInterfaceScrollArea(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}

ImmediateUserInterfaceScrollArea::~ImmediateUserInterfaceScrollArea(){}

gs_vec2f ImmediateUserInterfaceScrollArea::get_scroll_offset(const bool& _Scaled) const
{
    return _Scaled ?
        gs_vec2f((HorizontalScrollBar.Position * HorizontalScrollBar.PositionScale).x, (VerticalScrollBar.Position * VerticalScrollBar.PositionScale).y) :
            gs_vec2f(HorizontalScrollBar.Position.x, VerticalScrollBar.Position.y);
}

gs_2d_boxf ImmediateUserInterfaceScrollArea::get_visible_rect(ImmediateUserInterfaceContextLayer* _Context) const
{
    return ContentBox;
}

bool ImmediateUserInterfaceScrollArea::is_catching_event(ImmediateUserInterfaceContextLayer* _Context) const
{
    return _Context != nullptr && get_clipping_box(_Context).contains(_Context->m_Input.get_cusor_position());
}

void ImmediateUserInterfaceScrollArea::set_vertical_scroll_offset(const gs_vec2f _Value, const bool& _Relative)
{
    if(_Relative)
    {
        VerticalScrollBar.PreviousPosition = VerticalScrollBar.Position;
        VerticalScrollBar.Position         = gs_clamp(VerticalScrollBar.PreviousPosition + _Value, gs_vec2f(0.f, 0.f), gs_vec2f(0.f, VerticalScrollBarBox.size().y - VerticalScrollBar.UnconstrainedSize.y));
    }
    else
    {
        VerticalScrollBar.PreviousPosition = VerticalScrollBar.Position;
        VerticalScrollBar.Position         = gs_clamp(_Value, gs_vec2f(0.f, 0.f), gs_vec2f(0.f, VerticalScrollBarBox.size().y - VerticalScrollBar.UnconstrainedSize.y));
    }
}

void ImmediateUserInterfaceScrollArea::set_horizontal_scroll_offset(const gs_vec2f _Value, const bool& _Relative)
{
    if(_Relative)
    {
        HorizontalScrollBar.PreviousPosition = HorizontalScrollBar.Position;
        HorizontalScrollBar.Position         = gs_clamp(HorizontalScrollBar.PreviousPosition + _Value, gs_vec2f(0.f, 0.f), gs_vec2f(HorizontalScrollBarBox.size().x - HorizontalScrollBar.UnconstrainedSize.x, 0.f));
    }
    else
    {
        HorizontalScrollBar.PreviousPosition = HorizontalScrollBar.Position;
        HorizontalScrollBar.Position         = gs_clamp(_Value, gs_vec2f(0.f, 0.f), gs_vec2f(HorizontalScrollBarBox.size().x - HorizontalScrollBar.UnconstrainedSize.x, 0.f));
    }
}

void ImmediateUserInterfaceScrollArea::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

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

    // layout self
    {
        if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
           (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
        {
            if(!MinimumSizeBeforeResizeToContents.has_value())
                MinimumSizeBeforeResizeToContents = State.MinimumSize;

            if(!MaximumSizeBeforeResizeToContents.has_value())
                MaximumSizeBeforeResizeToContents = State.MaximumSize;
        }
        else
        {
            if(MinimumSizeBeforeResizeToContents.has_value())
                State.MinimumSize = MinimumSizeBeforeResizeToContents.value();
            MinimumSizeBeforeResizeToContents.reset();

            if(MaximumSizeBeforeResizeToContents.has_value())
                State.MaximumSize = MaximumSizeBeforeResizeToContents.value();
            MaximumSizeBeforeResizeToContents.reset();
        }

        // resize to contents
        State.MinimumSize = gs_vec2f(
            (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally) ?
                State.ContentSize.x + VerticalScrollBarBox.width() + _Context->m_Style.get_frames_width() + gs_vec2f(leftMargin - rightMargin, topMargin - bottomMargin).x :
                    State.MinimumSize.x,
            (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically) ?
                State.ContentSize.y + HorizontalScrollBarBox.height() + _Context->m_Style.get_frames_width() + gs_vec2f(leftMargin - rightMargin, topMargin - bottomMargin).y :
                    State.MinimumSize.y);
        
        State.MaximumSize =
            gs_vec2f(
                (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally) ? State.MinimumSize.x : State.MaximumSize.x,
                (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically) ? State.MinimumSize.y : State.MaximumSize.y);
        
        State.BoundingBox = gs_2d_boxf(
            State.BoundingBox.Min,
            State.BoundingBox.Min + gs_clamp(State.BoundingBox.size(), State.MinimumSize, State.MaximumSize));
    }

    // layout scrollbars
    {
        ImmediateUserInterfaceInputController* controller =
            _Context->get_controller<ImmediateUserInterfaceInputController>();

        // detect if we are being moved, resized e.t.c
        bool isModified =
            controller != nullptr &&
            controller->IsCatchingEvent &&
            State.Events == ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None;

        // calculate vertical scrollbar
        {
            gs_vec2f prevSize = gs_vec2f(gs_max(VerticalScrollBarBox.width(), 1.f), gs_max(VerticalScrollBarBox.height(), 1.f));
            gs_vec2f prevPos  = VerticalScrollBar.Position;

            VerticalScrollBarBox = gs_2d_boxf(gs_vec2f(State.BoundingBox.Max.x - _Context->m_Style.get_scrollbar_width(), State.BoundingBox.Min.y), State.BoundingBox.Max);
            VerticalScrollBarBox = gs_2d_boxf(VerticalScrollBarBox.Min, VerticalScrollBarBox.Max - gs_vec2f(0.f, _Context->m_Style.get_scrollbar_width()));
            VerticalScrollBar.recompute(gs_vec2f(0.f, 0.f), VerticalScrollBarBox.size(), State.ContentSize, _Context->m_Style.get_scrollbar_width());
            
            if(!isModified)
            {
                VerticalScrollBar.Position = gs_clamp(
                    VerticalScrollBarBox.size() * prevPos / prevSize,
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(0.f, VerticalScrollBarBox.size().y - VerticalScrollBar.UnconstrainedSize.y));   
            }

            if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleVerticalScrollBar))
            {
                VerticalScrollBarBox = gs_2d_boxf(VerticalScrollBarBox.Min, VerticalScrollBarBox.Min + gs_vec2f(0.f, VerticalScrollBarBox.height()));
                VerticalScrollBar.recompute(gs_vec2f(0.f, 0.f), VerticalScrollBarBox.size(), State.ContentSize, _Context->m_Style.get_scrollbar_width());
            }
            else if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically) ||
                (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar))
            {
                ResetVerticalScrollBar = true;
            }
            else if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar))
            {
                ResetVerticalScrollBar = (int)VerticalScrollBar.ConstrainedSize.y >= (int)VerticalScrollBarBox.height();
            }
            else if(State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysVerticalScrollBar)
            {
                ResetVerticalScrollBar = false;
            }
            else
            {
                ResetVerticalScrollBar = true;
            }

            if(ResetVerticalScrollBar)
            {
                VerticalScrollBarBox = gs_2d_boxf(State.BoundingBox.Min, State.BoundingBox.Min);
                VerticalScrollBar.reset();
            }
        }

        // calculate horizontal scrollbar
        {
            gs_vec2f prevSize = gs_vec2f(gs_max(HorizontalScrollBarBox.width(), 1.f), gs_max(HorizontalScrollBarBox.height(), 1.f));
            gs_vec2f prevPos  = HorizontalScrollBar.Position;

            HorizontalScrollBarBox = gs_2d_boxf(gs_vec2f(State.BoundingBox.Min.x, State.BoundingBox.Max.y - _Context->m_Style.get_scrollbar_width()), State.BoundingBox.Max);
            HorizontalScrollBar.recompute(gs_vec2f(0.f, 0.f), HorizontalScrollBarBox.size(), State.ContentSize, _Context->m_Style.get_scrollbar_width());

            if(!isModified)
            {
                HorizontalScrollBar.Position = gs_clamp(
                    HorizontalScrollBarBox.size() * prevPos / prevSize,
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(HorizontalScrollBarBox.size().x - HorizontalScrollBar.UnconstrainedSize.x, 0.f));
            }

            if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar))
            {
                HorizontalScrollBarBox = gs_2d_boxf(HorizontalScrollBarBox.Min, HorizontalScrollBarBox.Min + gs_vec2f(HorizontalScrollBarBox.width(), 0.f));
                HorizontalScrollBar.recompute(gs_vec2f(0.f, 0.f), HorizontalScrollBarBox.size(), State.ContentSize, _Context->m_Style.get_scrollbar_width());
            }
            else if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally) ||
                (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverHorizontalScrollBar))
            {
                ResetHorizontalScrollBar = true;
            }
            else if((State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar) && !isModified)
            {
                ResetHorizontalScrollBar = (int)HorizontalScrollBar.ConstrainedSize.x >= (int)HorizontalScrollBarBox.width();
            }
            else if(State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar)
            {
                ResetHorizontalScrollBar = false;
            }
            else if(!isModified)
            {
                ResetHorizontalScrollBar = true;
            }

            if(ResetHorizontalScrollBar)
            {
                HorizontalScrollBarBox = gs_2d_boxf(State.BoundingBox.Min, State.BoundingBox.Min);
                HorizontalScrollBar.reset();
            }
        }

        // calculate content box
        {
            ContentBox = gs_2d_boxf(
                State.BoundingBox.Min,
                State.BoundingBox.Max - gs_vec2f(VerticalScrollBarBox.width(), HorizontalScrollBarBox.height()) - gs_vec2f(_Context->m_Style.get_frames_width(), _Context->m_Style.get_frames_width()));
        }
    }

    // extract horizontal and vertical scrollbars
    gs_vec2f horizontalScrollOffset = HorizontalScrollBar.Position * HorizontalScrollBar.PositionScale;
    gs_vec2f verticalScrollOffset   = VerticalScrollBar.Position * VerticalScrollBar.PositionScale;

    // layout children
    gs_vec2f  origin    = State.BoundingBox.Min - gs_vec2f(horizontalScrollOffset.x, verticalScrollOffset.y) + gs_vec2f(leftMargin - rightMargin, topMargin - bottomMargin);
    gs_vec2f  position  = origin;
    float     maxHeight = 0.f;

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        (*it)->State.BoundingBox = gs_2d_boxf(position, position + gs_clamp((*it)->State.BoundingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));
        maxHeight = gs_max(maxHeight, (*it)->State.BoundingBox.height());

        if((*it)->State.NextLine > 0)
        {
            position = gs_vec2f(origin.x + (*it)->State.Indent, position.y + (maxHeight + _Context->m_Style.get_frames_width() * 2.f) * (*it)->State.NextLine + (topPadding - bottomPadding));
            maxHeight = 0.f;
        }
        else
        {
            position += gs_vec2f((*it)->State.BoundingBox.size().x + (leftPadding - rightPadding) + (*it)->State.Indent + _Context->m_Style.get_frames_width() * 2.f, 0.f);
        }
    }
}

void ImmediateUserInterfaceScrollArea::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    render_background(_Context);

    // vertical scrollbar
    if(gs_min(VerticalScrollBarBox.width(), VerticalScrollBarBox.height()) > 0.f)
    {
        // scrollbar
        _Context->m_Renderer->push_rectangle_filled(
            VerticalScrollBarBox.Min,
            VerticalScrollBarBox.Max,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());

        _Context->m_Renderer->push_rectangle_filled(
            VerticalScrollBarBox.Min + _Context->m_Style.get_frames_width(),
            VerticalScrollBarBox.Max - _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());

        // slider
        gs_vec2f position = gs_clamp(
            VerticalScrollBar.Position,
            gs_vec2f(0.f, 0.f),
            VerticalScrollBarBox.size() - VerticalScrollBar.ConstrainedSize);

        _Context->m_Renderer->push_rectangle_filled(
            VerticalScrollBarBox.Min + _Context->m_Style.get_frames_width() + position,
            VerticalScrollBarBox.Min - _Context->m_Style.get_frames_width() + position + VerticalScrollBar.ConstrainedSize,
            VerticalScrollBarBox.contains(_Context->m_Input.get_cusor_position()) ?
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered) :
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }

    // horizontal scrollbar
    if(gs_min(HorizontalScrollBarBox.width(), HorizontalScrollBarBox.height()) > 0.f)
    {        
        // scrollbar
        _Context->m_Renderer->push_rectangle_filled(
            HorizontalScrollBarBox.Min,
            HorizontalScrollBarBox.Max,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());

        _Context->m_Renderer->push_rectangle_filled(
            HorizontalScrollBarBox.Min + _Context->m_Style.get_frames_width(),
            HorizontalScrollBarBox.Max - _Context->m_Style.get_frames_width(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());

        // slider
        gs_vec2f position = gs_clamp(
            HorizontalScrollBar.Position,
            gs_vec2f(0.f, 0.f),
            HorizontalScrollBarBox.size() - HorizontalScrollBar.ConstrainedSize);

        _Context->m_Renderer->push_rectangle_filled(
            HorizontalScrollBarBox.Min + _Context->m_Style.get_frames_width() + position,
            HorizontalScrollBarBox.Min - _Context->m_Style.get_frames_width() + position + HorizontalScrollBar.ConstrainedSize,
            HorizontalScrollBarBox.contains(_Context->m_Input.get_cusor_position()) ?
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered) :
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }
}

void ImmediateUserInterfaceScrollArea::render_background(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());
}

bool ImmediateUserInterfaceScrollArea::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return false;

    if(!(_Context->m_Input.is_mouse_button_down()))
    {
        VerticalScrollIsMoving   = false;
        HorizontalScrollIsMoving = false;
    }

    // process horizontall scrollbar
    if((VerticalScrollBarBox.contains(_Context->m_Input.get_cusor_position()) && !HorizontalScrollIsMoving) || VerticalScrollIsMoving)
    {        
        if(_Context->m_Input.is_mouse_button_pressed())
        {
            VerticalScrollBar.Position =
                gs_clamp(
                    (_Context->m_Input.get_cusor_position() - State.BoundingBox.Min) - VerticalScrollBar.ConstrainedSize * 0.5f,
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(0.f, VerticalScrollBarBox.size().y - VerticalScrollBar.UnconstrainedSize.y));

            VerticalScrollBar.PreviousPosition = VerticalScrollBar.Position;
            VerticalScrollIsMoving             = true;

            return true;
        }

        if((_Context->m_Input.is_mouse_button_down()) && VerticalScrollIsMoving)
        {
            VerticalScrollBar.Position = gs_clamp(
                VerticalScrollBar.PreviousPosition + _Context->m_Input.get_cusor_drag_delta(),
                gs_vec2f(0.f, 0.f),
                gs_vec2f(0.f, VerticalScrollBarBox.size().y - VerticalScrollBar.UnconstrainedSize.y));

            return true;
        }

        return ImmediateUserInterfacePanel::events(_Context);
    }

    // process vertical scrollbar
    if((HorizontalScrollBarBox.contains(_Context->m_Input.get_cusor_position()) && !VerticalScrollIsMoving) || HorizontalScrollIsMoving)
    {
        if(_Context->m_Input.is_mouse_button_pressed())
        {
            HorizontalScrollBar.Position =
                gs_clamp(
                    (_Context->m_Input.get_cusor_position() - State.BoundingBox.Min) - HorizontalScrollBar.ConstrainedSize * 0.5f,
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(HorizontalScrollBarBox.size().x - HorizontalScrollBar.UnconstrainedSize.x, 0.f));

            HorizontalScrollBar.PreviousPosition = HorizontalScrollBar.Position;
            HorizontalScrollIsMoving             = true;
            return true;
        }

        if((_Context->m_Input.is_mouse_button_down()) && HorizontalScrollIsMoving)
        {
            HorizontalScrollBar.Position = gs_clamp(
                HorizontalScrollBar.PreviousPosition + _Context->m_Input.get_cusor_drag_delta(),
                gs_vec2f(0.f, 0.f),
                gs_vec2f(HorizontalScrollBarBox.size().x - HorizontalScrollBar.UnconstrainedSize.x, 0.f));
            return true;
        }

        return ImmediateUserInterfacePanel::events(_Context);
    }

    return ImmediateUserInterfacePanel::events(_Context);
}

ImmediateUserInterfacePopupScrollArea::ImmediateUserInterfacePopupScrollArea(const std::string& _Name) : ImmediateUserInterfaceScrollArea(_Name)
{
    disable();
}

ImmediateUserInterfacePopupScrollArea::~ImmediateUserInterfacePopupScrollArea(){}

void ImmediateUserInterfacePopupScrollArea::render_background(ImmediateUserInterfaceContextLayer* _Context)
{
    if( _Context             == nullptr ||
        _Context->m_Renderer == nullptr ||
        _Context->m_Hierarchy.get_parent(_Context->m_Hierarchy.get_parent<ImmediateUserInterfaceMenu>(this)) != nullptr)
    {
        return;
    }

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());
}

// ImmediateUserInterfaceMenu
ImmediateUserInterfaceMenu::ImmediateUserInterfaceMenu(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}
ImmediateUserInterfaceMenu::~ImmediateUserInterfaceMenu(){}

void ImmediateUserInterfaceMenu::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(InternalScrollArea != nullptr)
    {
        State.BoundingBox = gs_2d_boxf(
            State.BoundingBox.Min,
            State.BoundingBox.Min + InternalScrollArea->State.BoundingBox.size());
    }

    ImmediateUserInterfacePanel::layout(_Context);
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

void ImmediateUserInterfaceMenuScrollArea::render_background(ImmediateUserInterfaceContextLayer* _Context)
{
    if( _Context             == nullptr ||
        _Context->m_Renderer == nullptr ||
        _Context->m_Hierarchy.get_parent(_Context->m_Hierarchy.get_parent<ImmediateUserInterfaceMenu>(this)) != nullptr)
    {
        return;
    }

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());
}

// ImmediateUserInterfaceMenuAction
ImmediateUserInterfaceMenuAction::ImmediateUserInterfaceMenuAction(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceMenuAction::~ImmediateUserInterfaceMenuAction(){}

void ImmediateUserInterfaceMenuAction::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    gs_vec2f size =
        _Context->m_Renderer->calculate_bounding_box(
            Name.begin(),
            Name.end(),
            _Context->m_Style.get_font_size(),
            _Context->m_Style.get_current_font()).size();

    State.MinimumSize = gs_vec2f(gs_min(size.x, State.MinimumSize.x), ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));
    State.MaximumSize = gs_vec2f(gs_max(size.x, State.MaximumSize.x), ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));
}

void ImmediateUserInterfaceMenuAction::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // background
    if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && _Context->m_Input.is_mouse_button_down())
    {
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max - gs_vec2f(_Context->m_Style.get_frames_width(), 0.f),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }
    else
    {
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max - gs_vec2f(_Context->m_Style.get_frames_width(), 0.f),
            (State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }

    // title
    _Context->m_Renderer->push_text(
        gs_vec2f(
            State.BoundingBox.Min.x + _Context->m_Style.get_frames_width(), 
            (State.BoundingBox.center() - _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() * 0.5f).y),
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix(
        (float)place_in_follow()),
        _Context->m_Style.get_current_font());
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
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max - gs_vec2f(_Context->m_Style.get_frames_width(), 0.f),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }
    else
    {
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max - gs_vec2f(_Context->m_Style.get_frames_width(), 0.f),
            (State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }

    // title
    _Context->m_Renderer->push_text(
        gs_vec2f(
            State.BoundingBox.Min.x + _Context->m_Style.get_frames_width(),
            (State.BoundingBox.center() - _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() * 0.5f).y),
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_current_font());

    // triangle
    if(_Context->m_Hierarchy.get_parent<ImmediateUserInterfaceMenuBar>(this) != nullptr) return;

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

// ImmediateUserInterfaceMenuBar
ImmediateUserInterfaceMenuBar::ImmediateUserInterfaceMenuBar(const std::string& _Name) : ImmediateUserInterfaceScrollArea(_Name){}
ImmediateUserInterfaceMenuBar::~ImmediateUserInterfaceMenuBar(){}

void ImmediateUserInterfaceMenuBar::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child != nullptr)
    {
        _Child->State.Indent   = 0.f;
        _Child->State.NextLine = 0;
    }

    ImmediateUserInterfaceScrollArea::attach_child(_Child);
}

// ImmediateUserInterfaceCombobox
ImmediateUserInterfaceCombobox::ImmediateUserInterfaceCombobox(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
{
    State.BoundingBox = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(256.f, 128.f));
}

ImmediateUserInterfaceCombobox::~ImmediateUserInterfaceCombobox(){}

void ImmediateUserInterfaceCombobox::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // layout self
    State.MinimumSize = gs_vec2f(State.MinimumSize.x, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));
    State.MaximumSize = gs_vec2f(State.MaximumSize.x, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));

    State.BoundingBox = gs_2d_boxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + gs_clamp(
            gs_vec2f(State.BoundingBox.width(), ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context)),
            State.MinimumSize,
            State.MaximumSize));

    if(ScrollArea == nullptr)
        return;

    // resize children
    float MaximumWidth = 0.f;

    for(auto it = _Context->m_Hierarchy.begin(ScrollArea); it != _Context->m_Hierarchy.end(ScrollArea); it++)
    {
        ImmediateUserInterfaceComboboxItem* comboboxItem =
            dynamic_cast<ImmediateUserInterfaceComboboxItem*>(*it);

        if(comboboxItem == nullptr)
            continue;

        MaximumWidth = gs_max(_Context->m_Renderer->calculate_bounding_box(
            comboboxItem->Name.begin(),
            comboboxItem->Name.end(),
            _Context->m_Style.get_font_size(),
            _Context->m_Style.get_current_font()).size().x + _Context->m_Style.get_font_size(), MaximumWidth);
    }

    for(auto it = _Context->m_Hierarchy.begin(ScrollArea); it != _Context->m_Hierarchy.end(ScrollArea); it++)
    {
        ImmediateUserInterfaceComboboxItem* comboboxItem =
            dynamic_cast<ImmediateUserInterfaceComboboxItem*>(*it);

        if(comboboxItem != nullptr)
        {
            comboboxItem->State.MinimumSize = gs_vec2f(MaximumWidth, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));
            comboboxItem->State.MaximumSize = comboboxItem->State.MinimumSize;

            comboboxItem->State.BoundingBox = gs_2d_boxf(
                comboboxItem->State.BoundingBox.Min,
                comboboxItem->State.BoundingBox.Min + gs_vec2f(MaximumWidth, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context)));
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

ImmediateUserInterfaceComboboxScrollArea::ImmediateUserInterfaceComboboxScrollArea(const std::string& _Name) : ImmediateUserInterfaceScrollArea(_Name){}

ImmediateUserInterfaceComboboxScrollArea::~ImmediateUserInterfaceComboboxScrollArea()
{
}

void ImmediateUserInterfaceComboboxScrollArea::render_background(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());
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

    State.MinimumSize = gs_vec2f(gs_min(size.x, State.MinimumSize.x), ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));
    State.MaximumSize = gs_vec2f(gs_max(size.x, State.MaximumSize.x), ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));
}

void ImmediateUserInterfaceComboboxItem::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // background
    if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && _Context->m_Input.is_mouse_button_down())
    {
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }
    else
    {
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            (State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_MenuActionBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
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
        (float)place_in_follow()),
        _Context->m_Style.get_current_font());
}

// ImmediateUserInterfaceTreeNode
ImmediateUserInterfaceTreeNode::ImmediateUserInterfaceTreeNode(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}
ImmediateUserInterfaceTreeNode::~ImmediateUserInterfaceTreeNode(){}

void ImmediateUserInterfaceTreeNode::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;
    
    // background
    if((State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        TitleBox.contains(_Context->m_Input.get_cusor_position()))
    {
        _Context->m_Renderer->push_rectangle_filled(
            TitleBox.Min,
            TitleBox.Max,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
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
                0.f,
                TextureOpened);
        }
        else
        {
            _Context->m_Renderer->push_rectangle_filled(
                IconBox.Min,
                IconBox.Max,
                gs_color_rgba(255, 255, 255, 255),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                0.f,
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
        gs_vec2f(IconBox.Max.x, IconBox.center().y - _Context->m_Renderer->calculate_bounding_box(Name.begin(), Name.end(), _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).height() * 0.5f),
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_current_font());
}

void ImmediateUserInterfaceTreeNode::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

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

    // layout self
    State.BoundingBox = gs_2d_boxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + gs_clamp(State.ContentSize, State.MinimumSize, State.MaximumSize));

    TitleBox = gs_2d_boxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + gs_vec2f(State.BoundingBox.width(), ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context)));

    IconBox = gs_2d_boxf(
        TitleBox.Min,
        TitleBox.Min + ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));

    // layout children
    gs_vec2f  origin    = State.BoundingBox.Min + gs_vec2f(leftMargin - rightMargin, topMargin - bottomMargin) + gs_vec2f(0.f, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context)) + gs_vec2f(IconBox.width(), 0.f);
    gs_vec2f  position  = origin;
    float     maxHeight = 0.f;

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        (*it)->State.BoundingBox = gs_2d_boxf(position, position + gs_clamp((*it)->State.BoundingBox.size(), (*it)->State.MinimumSize, (*it)->State.MaximumSize));
        maxHeight = gs_max(maxHeight, (*it)->State.BoundingBox.height());

        if((*it)->State.NextLine > 0)
        {
            position = gs_vec2f(origin.x + (*it)->State.Indent, position.y + (maxHeight + _Context->m_Style.get_frames_width() * 2.f) * (*it)->State.NextLine + (topPadding - bottomPadding));
            maxHeight = 0.f;
        }
        else
        {
            position += gs_vec2f((*it)->State.BoundingBox.size().x + (leftPadding - rightPadding) + (*it)->State.Indent + _Context->m_Style.get_frames_width() * 2.f, 0.f);
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
    gs_2d_boxf box = gs_2d_boxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + _Context->m_Renderer->calculate_bounding_box(
            Name.begin(),
            Name.end(),
            _Context->m_Style.get_font_size(),
            _Context->m_Style.get_current_font()).size() + ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));

    // this are children
    for (auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        box = gs_2d_boxf(
            box.Min,
            (*it)->State.BoundingBox.Min,
            box.Max,
            (*it)->State.BoundingBox.Max);
    }

    State.ContentSize = box.size();
}

// ImmediateUserInterfaceLayerGridNode
ImmediateUserInterfaceTableGrid::ImmediateUserInterfaceTableGrid(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}
ImmediateUserInterfaceTableGrid::~ImmediateUserInterfaceTableGrid(){}

void ImmediateUserInterfaceTableGrid::attach_child(ImmediateUserInterfaceNode* _Child)
{
    ImmediateUserInterfaceTableGridCell* cell =
        dynamic_cast<ImmediateUserInterfaceTableGridCell*>(_Child);

    GS_ASSERT(cell != nullptr);

    RowsCount = gs_max(RowsCount, cell->Row + 1);
    ColsCount = gs_max(ColsCount, cell->Column + 1);

    cell->State.Parent = this;
}

void ImmediateUserInterfaceTableGrid::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // layout self
    State.BoundingBox = gs_2d_boxf(
        State.BoundingBox.Min,
        State.BoundingBox.Min + State.ContentSize);

    // layout children
    gs_vec2f cellSize = CellSize != nullptr ? *CellSize : gs_vec2f(256.f, 128.f);

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        ImmediateUserInterfaceTableGridCell* cell =
            dynamic_cast<ImmediateUserInterfaceTableGridCell*>(*it);

        if(cell == nullptr)
            continue;

        cell->State.BoundingBox = gs_2d_boxf(
            State.BoundingBox.Min + cellSize * gs_vec2f((float)cell->Column, (float)cell->Row),
            State.BoundingBox.Min + cellSize * gs_vec2f((float)cell->Column + 1.f, (float)cell->Row + 1.f));
    }
}

void ImmediateUserInterfaceTableGrid::measure(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    gs_vec2f cellSize = CellSize != nullptr ? *CellSize : gs_vec2f(256.f, 128.f);

    State.ContentSize = cellSize * gs_vec2f((float)ColsCount, (float)RowsCount);
}

// ImmediateUserInterfaceLayerGridCell
ImmediateUserInterfaceTableGridCell::ImmediateUserInterfaceTableGridCell(const std::string& _Name) : ImmediateUserInterfacePanel(_Name)
{
    State.PlaceInFollow = true;
}
ImmediateUserInterfaceTableGridCell::~ImmediateUserInterfaceTableGridCell(){}

void ImmediateUserInterfaceTableGridCell::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    GS_ASSERT(dynamic_cast<ImmediateUserInterfaceTableGrid*>(
        _Context->m_Hierarchy.get_parent(this)) != nullptr);

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context,
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min + gs_vec2f(_Context->m_Style.get_frames_width()),
        State.BoundingBox.size() - gs_vec2f(_Context->m_Style.get_frames_width()) * 2.f,
        ContentPadding,
        ContentMargin,
        State.Settings,
        [this](const ImmediateUserInterfaceNode* _Node){return true;});
}

bool ImmediateUserInterfaceTableGridCell::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(!ImmediateUserInterfacePanel::events(_Context))
        return false;

    ImmediateUserInterfaceTableGrid * grid =
        _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceTableGrid>(this);

    if(grid != nullptr && grid->CellSize != nullptr)
        *grid->CellSize = State.BoundingBox.size();

    return true;
}

// ImmediateUserInterfaceTable
ImmediateUserInterfaceTable::ImmediateUserInterfaceTable(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}
ImmediateUserInterfaceTable::~ImmediateUserInterfaceTable(){}

bool ImmediateUserInterfaceTable::create_contents(
    ImmediateUserInterfaceContextLayer*       _Context, 
    const std::string&                        _ID,
    const ImmediateUserInterfaceNodeSettings& _Settings,
    bool*                                     _Render)
{
    // layout self
    ImmediateUserInterfaceScrollArea* scrollArea =
        _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceScrollArea>(DataCells);

    gs_vec2f scrollOffset = scrollArea != nullptr ? scrollArea->get_scroll_offset(false) : gs_vec2f(0.f, 0.f);

    VerticalClipper   = ImmediateUserInterfaceVerticalClipper(scrollArea, GridRowsCount, GridCellSize.y);
    HorizontalClipper = ImmediateUserInterfaceHorizontalClipper(scrollArea, GridColsCount, GridCellSize.x);

    if(_Context->begin_vertical_stack(
        _Context->next_id("Table"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        _Context->get_rendering_stack_top()->State.PlaceInFollow = true;
        _Context->next_minimum_size(gs_vec2f(0.f, (RowHeadersCount + ColumnHeadersCount + CornerHeaderCount > 0 ? GridCellSize.y : 0.f)));
        _Context->next_maximum_size(gs_vec2f(gs_huge<float>(), (RowHeadersCount + ColumnHeadersCount + CornerHeaderCount > 0 ? GridCellSize.y : 0.f)));

        // columns titles
        if(_Context->begin_horizontal_stack(
            _Context->next_id("Cols"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
        {
            // corner title box
            _Context->get_rendering_stack_top()->State.PlaceInFollow = true;
            _Context->next_minimum_size(GridCellSize);
            _Context->next_maximum_size(GridCellSize);

            if(_Context->begin_node<ImmediateUserInterfaceTableCornerPanel>(
                _Context->next_id("Corner"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                CorenerHeader = _Context->get_rendering_stack_top<ImmediateUserInterfaceTableCornerPanel>();
                _Context->end_node<ImmediateUserInterfaceTableCornerPanel>();
            }

            // columns titles
            _Context->next_minimum_size(gs_vec2f(GridCellSize.x, GridCellSize.y));
            _Context->next_maximum_size(gs_vec2f(gs_huge<float>(), GridCellSize.y));

            if(_Context->begin_scrollarea(
                _Context->next_id("Columns"),
                  ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar))
            {
                _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollArea>()->set_horizontal_scroll_offset(gs_vec2f(scrollOffset.x, 0.f), false);

                if(_Context->begin_node<ImmediateUserInterfaceTableGrid>(
                    _Context->next_id("Grid"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    ColumnHeaders            = _Context->get_rendering_stack_top<ImmediateUserInterfaceTableGrid>();
                    ColumnHeaders->RowsCount = 1;
                    ColumnHeaders->ColsCount = GridColsCount;
                    ColumnHeaders->CellSize  = &GridCellSize;
                    
                    _Context->end_node<ImmediateUserInterfaceTableGrid>();
                }

                _Context->end_scrollarea();
            }

            _Context->end_horizontal_stack();
        }

        // rows titles and cells
        if(_Context->begin_horizontal_stack(
            _Context->next_id("Rows"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
        {
            _Context->get_rendering_stack_top()->State.PlaceInFollow = true;

            // rows titles
            _Context->next_minimum_size(gs_vec2f((RowHeadersCount + ColumnHeadersCount + CornerHeaderCount > 0 ? GridCellSize.x : 0.f), _Context->current_minimum_size(_Context->get_rendering_stack_top()).y));
            _Context->next_maximum_size(gs_vec2f((RowHeadersCount + ColumnHeadersCount + CornerHeaderCount > 0 ? GridCellSize.x : 0.f), _Context->current_maximum_size(_Context->get_rendering_stack_top()).y));

            if(_Context->begin_vertical_stack(
                _Context->next_id("Rows"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
            {
                if(_Context->begin_scrollarea(
                    _Context->next_id("Rows"),
                  ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_InvisibleVerticalScrollBar
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverHorizontalScrollBar))
                {
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceScrollArea>()->set_vertical_scroll_offset(scrollOffset, false);

                    if(_Context->begin_node<ImmediateUserInterfaceTableGrid>(
                        _Context->next_id("Grid"),
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                    {
                        RowHeaders            = _Context->get_rendering_stack_top<ImmediateUserInterfaceTableGrid>();
                        RowHeaders->RowsCount = GridRowsCount;
                        RowHeaders->ColsCount = 1;
                        RowHeaders->CellSize  = &GridCellSize;
                        
                        _Context->end_node<ImmediateUserInterfaceTableGrid>();
                    }

                    _Context->end_scrollarea();
                }

                _Context->end_vertical_stack();
            }

            // cells
            if(_Context->begin_scrollarea(_Context->next_id("Cells")))
            {
                if(_Context->begin_node<ImmediateUserInterfaceTableGrid>(
                    _Context->next_id("Grid"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    DataCells           = _Context->get_rendering_stack_top<ImmediateUserInterfaceTableGrid>();
                    DataCells->RowsCount   = GridRowsCount;
                    DataCells->ColsCount   = GridColsCount;
                    DataCells->CellSize = &GridCellSize;
                    
                    _Context->end_node<ImmediateUserInterfaceTableGrid>();
                }

                _Context->end_scrollarea();
            }

            _Context->end_horizontal_stack();
        }
    
        _Context->end_vertical_stack();
    }

    return true;
}

void ImmediateUserInterfaceTable::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    // layout self
    ImmediateUserInterfacePanel::layout(_Context);

    // calculate children count within column headers and row headers parent nodes
    RowHeadersCount =
        RowHeaders != nullptr && _Context != nullptr ?
            _Context->m_Hierarchy.count(RowHeaders, [](const ImmediateUserInterfaceNode*)->bool{return true;}) :
                0;

    ColumnHeadersCount =
        ColumnHeaders != nullptr && _Context != nullptr ?
            _Context->m_Hierarchy.count(ColumnHeaders, [](const ImmediateUserInterfaceNode*)->bool{return true;}) :
                0;

    CornerHeaderCount =
        CorenerHeader != nullptr && _Context != nullptr ?
            _Context->m_Hierarchy.count(CorenerHeader, [](const ImmediateUserInterfaceNode*)->bool{return true;}) :
                0;
}

void ImmediateUserInterfaceTable::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(dynamic_cast<ImmediateUserInterfaceTableVerticalHeader*>(_Child) && RowHeaders)
    {
        RowHeaders->attach_child(_Child);
        return;
    }

    if(dynamic_cast<ImmediateUserInterfaceTableHorizontalHeader*>(_Child) && ColumnHeaders)
    {
        ColumnHeaders->attach_child(_Child);
        return;
    }

    if(dynamic_cast<ImmediateUserInterfaceTableCornerHeader*>(_Child) && CorenerHeader)
    {
        CorenerHeader->attach_child(_Child);
        return;
    }

    if(dynamic_cast<ImmediateUserInterfaceTableGridCell*>(_Child) && DataCells)
    {
        DataCells->attach_child(_Child);
        return;
    }

    if(_Child)
        _Child->State.Parent = this;
}

// ImmediateUserInterfaceTableCornerPanel
ImmediateUserInterfaceTableCornerPanel::ImmediateUserInterfaceTableCornerPanel(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}
ImmediateUserInterfaceTableCornerPanel::~ImmediateUserInterfaceTableCornerPanel(){}

// ImmediateUserInterfaceTableVerticalHeader
ImmediateUserInterfaceTableVerticalHeader::ImmediateUserInterfaceTableVerticalHeader(const std::string& _Name) : ImmediateUserInterfaceTableGridCell(_Name){}
ImmediateUserInterfaceTableVerticalHeader::~ImmediateUserInterfaceTableVerticalHeader(){}

// ImmediateUserInterfaceTableHorizontalHeader
ImmediateUserInterfaceTableHorizontalHeader::ImmediateUserInterfaceTableHorizontalHeader(const std::string& _Name) : ImmediateUserInterfaceTableGridCell(_Name){}
ImmediateUserInterfaceTableHorizontalHeader::~ImmediateUserInterfaceTableHorizontalHeader(){}

// ImmediateUserInterfaceTableCornerHeader
ImmediateUserInterfaceTableCornerHeader::ImmediateUserInterfaceTableCornerHeader(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}
ImmediateUserInterfaceTableCornerHeader::~ImmediateUserInterfaceTableCornerHeader(){}

void ImmediateUserInterfaceTableCornerHeader::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    GS_ASSERT(_Context->m_Hierarchy.get_parent<ImmediateUserInterfaceTable>(this));

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context,
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min + gs_vec2f(_Context->m_Style.get_frames_width()),
        State.BoundingBox.size() - gs_vec2f(_Context->m_Style.get_frames_width()) * 2.f,
        ContentPadding,
        ContentMargin,
        State.Settings,
        [this](const ImmediateUserInterfaceNode* _Node){return true;});
}

// ImmediateUserInterfaceWindow
ImmediateUserInterfaceWindow::ImmediateUserInterfaceWindow(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceWindow::~ImmediateUserInterfaceWindow(){}

void ImmediateUserInterfaceWindow::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // content background and outline frame
    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());

    if(DockerView != nullptr)
    {
        for (auto dockedWindow : DockedWindowsCache)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindowDockGizmo*>(dockedWindow) == nullptr) continue;

            _Context->m_Renderer->push_rectangle_filled(
                DockerView->State.BoundingBox.Min,
                DockerView->State.BoundingBox.Max,
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node(this)),
                _Context->m_Style.get_frames_radius());

            break;
        }
    }
}

void ImmediateUserInterfaceWindow::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_panel(
        _Context,
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        State.BoundingBox.Min,
        State.BoundingBox.size(),
        gs_vec4f(0.f),
        gs_vec4f(0.f),
        State.Settings,
        [this](const ImmediateUserInterfaceNode* _Node){return true;});
}

bool ImmediateUserInterfaceWindow::events(ImmediateUserInterfaceContextLayer* _Context)
{
    return ImmediateUserInterfaceNode::events(_Context);
}

void ImmediateUserInterfaceWindow::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(_Child == nullptr)
        return;

    if(dynamic_cast<ImmediateUserInterfaceWindowRoot*>(_Child))
    {
        _Child->State.Parent = this;
        return;
    }

    if( 
        // dynamic_cast<ImmediateUserInterfaceWindowHorizontalSnapper*>(_Child) ||
        // dynamic_cast<ImmediateUserInterfaceWindowVerticalSnapper*>(_Child)   ||
        // dynamic_cast<ImmediateUserInterfaceWindowCentralDocker*>(_Child)     ||
        dynamic_cast<ImmediateUserInterfaceWindowFrame*>(_Child))
    {
        if(RootView)
            RootView->attach_child(_Child);
        return;
    }

    if(ContentView)
        ContentView->attach_child(_Child);
}

bool ImmediateUserInterfaceWindow::create_contents(ImmediateUserInterfaceContextLayer* _Context, 
    const std::string&                        _ID,
    const ImmediateUserInterfaceNodeSettings& _Settings,
    bool*                                     _Render)
{
    // code
    if(_Context == nullptr)
        return false;

    ImmediateUserInterfaceNodeSettings settings = _Settings;
    settings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent;

    ImmediateUserInterfaceWindow* window = this;
    window->Opened                       = _Render;

    if(_Context->begin_node<ImmediateUserInterfaceWindowRoot>(std::string(_ID).append("/").append(_ID), settings))
    {
        window->RootView = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowRoot>();

        gs_2d_boxf frameBox = gs_2d_boxf();

        // frame
        if(Docker == nullptr && State.Parent == nullptr) // here we check hierarchical parent and central docker parent only
        {
            if(_Context->begin_node<ImmediateUserInterfaceWindowFrame>(
                _Context->next_id("Frame"),
                  ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
            {
                ImmediateUserInterfaceWindowFrame* frame =
                    _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowFrame>();

                frameBox = frame->State.BoundingBox;

                float maxWidth = 0.f;

                maxWidth = gs_max(
                    _Context->m_Renderer->calculate_bounding_box(
                        Name.begin(),
                        Name.end(),
                        20,
                        _Context->m_Style.get_font_size(),
                        _Context->m_Style.get_current_font()).size().x + _Context->m_Style.get_font_size() + _Context->m_Style.get_frames_radius(),
                    maxWidth);

                for (int i = 0; i < (int)DockedWindowsCache.size(); i++)
                {
                    maxWidth = gs_max(
                        _Context->m_Renderer->calculate_bounding_box(
                            DockedWindowsCache[i]->Name.begin(),
                            DockedWindowsCache[i]->Name.end(),
                            20,
                            _Context->m_Style.get_font_size(),
                            _Context->m_Style.get_current_font()).size().x + _Context->m_Style.get_font_size() + _Context->m_Style.get_frames_radius(),
                        maxWidth);
                }

                if(DockedWindowsCache.empty())
                    maxWidth = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowFrame>()->State.BoundingBox.width();

                if(_Context->begin_node<ImmediateUserInterfaceWindowFrameButton>(
                    _Context->next_id("Self"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
                {
                    ImmediateUserInterfaceWindowFrameButton* frameButton =
                        _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowFrameButton>();

                    frameButton->Window = this;
                    frameButton->State.BoundingBox = gs_2d_boxf(frameButton->State.BoundingBox.Min, frameButton->State.BoundingBox.Min + gs_vec2f(maxWidth, frameButton->State.BoundingBox.height()));

                    _Context->end_node<ImmediateUserInterfaceWindowFrameButton>();
                }

                if(!DockedWindowsCache.empty())
                {
                    if(_Context->begin_what_is_it(_Context->next_id("SelfDescription"), _Context->get_rendered_stack_top()))
                    {
                        _Context->label(_Context->next_id("SelfDescription"), this->Name);
                        _Context->end_what_is_it();
                    }
                }

                for (int i = 0; i < (int)DockedWindowsCache.size(); i++)
                {
                    if(DockedWindowsCache[i] == nullptr)
                        continue;

                    _Context->same_line();
                    _Context->indent(_Context->m_Style.get_font_size());

                    if(_Context->begin_node<ImmediateUserInterfaceWindowFrameButton>(
                        _Context->next_id(Frenchie::Core::String::format("CenterDockChild-%d", i)),
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
                    {
                        ImmediateUserInterfaceWindowFrameButton* frameButton =
                            _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowFrameButton>();

                        frameButton->Window            = dynamic_cast<ImmediateUserInterfaceWindow*>(DockedWindowsCache[i]);
                        frameButton->State.BoundingBox = gs_2d_boxf(frameButton->State.BoundingBox.Min, frameButton->State.BoundingBox.Min + gs_vec2f(maxWidth, frameButton->State.BoundingBox.height()));

                        _Context->end_node<ImmediateUserInterfaceWindowFrameButton>();
                    }

                    if(_Context->begin_what_is_it(_Context->next_id(Frenchie::Core::String::format("CenterDockChild-%d-Description", i)), _Context->get_rendered_stack_top()))
                    {
                        _Context->label(_Context->next_id(Frenchie::Core::String::format("CenterDockChild-%d-Description", i)), DockedWindowsCache[i]->Name);
                        _Context->end_what_is_it();
                    }
                }
                
                _Context->end_node<ImmediateUserInterfaceWindowFrame>();
            }
        }

        // calculate docked windows bounding box for gizmos
        DockedWindowsBox = gs_2d_boxf(
            window->RootView->State.BoundingBox.Min + gs_vec2f(0.f, frameBox.height()),
            window->RootView->State.BoundingBox.Max);

        // central docker
        if(_Context->begin_panel(
            _Context->next_id("CentralDockerView"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
        {
            window->DockerView = _Context->get_rendering_stack_top();
            _Context->end_panel();
        }

        // vertical snapper        
        _Context->next_content_padding(_Context->m_Style.get_frames_width() * 2.f);

        if(_Context->begin_vertical_stack(
            _Context->next_id("SnapperView"),
            settings
                & ~(ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
                  | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft
                  | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight)
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
        {
            window->SnapperView = _Context->get_rendering_stack_top();

            // top
            if(_Context->begin_horizontal_stack(_Context->next_id("TopSnapperView"), settings | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren))
            {
                window->TopSnapperView = _Context->get_rendering_stack_top();
                _Context->end_horizontal_stack();
            }

            // center
            if(_Context->begin_horizontal_stack(_Context->next_id("CentralSnapperView"), settings | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren))
            {
                if(_Context->begin_horizontal_stack(_Context->next_id("LeftSnapperView"), settings | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren))
                {
                    window->LeftSnapperView = _Context->get_rendering_stack_top();
                    _Context->end_horizontal_stack();
                }

                float padding = _Context->m_Style.get_frames_width() + _Context->m_Style.get_frames_radius() * 0.5f;
                _Context->next_content_padding(gs_vec4f(padding, padding, 0.f, 0.f));

                if(_Context->begin_vertical_stack(
                    _Context->next_id("ContentView"),
                    (settings & ~(ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft | ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight))
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
                {
                    window->ContentView = _Context->get_rendering_stack_top();
                    _Context->end_vertical_stack();
                }

                if(_Context->begin_horizontal_stack(_Context->next_id("RightSnapperView"), settings | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren))
                {
                    window->RightSnapperView = _Context->get_rendering_stack_top();
                    _Context->end_horizontal_stack();
                }

                _Context->end_horizontal_stack();
            }

            // bottom
            if(_Context->begin_horizontal_stack(_Context->next_id("BottomSnapperView"), settings | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren))
            {
                window->BottomSnapperView = _Context->get_rendering_stack_top();
                _Context->end_horizontal_stack();
            }

            _Context->end_vertical_stack();
        }

        _Context->end_node<ImmediateUserInterfaceWindowRoot>();
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

    State.BoundingBox = gs_2d_boxf(position, position + gs_clamp(size, State.MinimumSize, State.MaximumSize));

    // layout
    if(_Context->m_IniFileState.contains(Hash, "TopSnapperViewSize") && TopSnapperView != nullptr)
    {
        TopSnapperView->State.BoundingBox =
            gs_2d_boxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "TopSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "LeftSnapperViewSize") && LeftSnapperView != nullptr)
    {
        LeftSnapperView->State.BoundingBox =
            gs_2d_boxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "LeftSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "RightSnapperViewSize") && RightSnapperView != nullptr)
    {
        RightSnapperView->State.BoundingBox =
            gs_2d_boxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "RightSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "BottomSnapperViewSize") && BottomSnapperView != nullptr)
    {
        BottomSnapperView->State.BoundingBox =
            gs_2d_boxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "BottomSnapperViewSize"));
    }

    if(_Context->m_IniFileState.contains(Hash, "ContentViewSize") && ContentView != nullptr)
    {
        ContentView->State.BoundingBox =
            gs_2d_boxf(gs_vec2f(0.f, 0.f), _Context->m_IniFileState.get<gs_vec2f>(Hash, "ContentViewSize"));
    }
}

void ImmediateUserInterfaceWindow::save_state(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    // self position and size
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "Size", State.BoundingBox.size());
    _Context->m_IniFileState.set<gs_vec2f>(Hash, "Position", State.BoundingBox.Min);

    // layout items positions and sizes
    if(TopSnapperView != nullptr)
        _Context->m_IniFileState.set<gs_vec2f>(Hash, "TopSnapperViewSize", TopSnapperView->State.BoundingBox.size());
    
    if(LeftSnapperView != nullptr)
        _Context->m_IniFileState.set<gs_vec2f>(Hash, "LeftSnapperViewSize", LeftSnapperView->State.BoundingBox.size());
    
    if(RightSnapperView != nullptr)
        _Context->m_IniFileState.set<gs_vec2f>(Hash, "RightSnapperViewSize", RightSnapperView->State.BoundingBox.size());
    
    if(BottomSnapperView != nullptr)
        _Context->m_IniFileState.set<gs_vec2f>(Hash, "BottomSnapperViewSize", BottomSnapperView->State.BoundingBox.size());
    
    if(ContentView != nullptr)
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

ImmediateUserInterfaceWindow* ImmediateUserInterfaceWindow::retrieve_docker_by_view(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _DockerView)
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

// ImmediateUserInterfaceWindowDockArea
ImmediateUserInterfaceWindowDockArea::ImmediateUserInterfaceWindowDockArea(const std::string& _Name) : ImmediateUserInterfaceWindow(_Name){}
ImmediateUserInterfaceWindowDockArea::~ImmediateUserInterfaceWindowDockArea(){}

void ImmediateUserInterfaceWindowDockArea::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    State.BoundingBox = _Context->m_Renderer->current_viewport();
    ImmediateUserInterfaceWindow::layout(_Context);
}

ImmediateUserInterfaceWindowDockGizmo::ImmediateUserInterfaceWindowDockGizmo(const std::string& _Name) : ImmediateUserInterfaceWindow(_Name){}
ImmediateUserInterfaceWindowDockGizmo::~ImmediateUserInterfaceWindowDockGizmo(){}

void ImmediateUserInterfaceWindowDockGizmo::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // content background and outline frame
    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());
}

void ImmediateUserInterfaceWindowDockGizmo::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    State.MinimumSize = _Context->m_Renderer->current_viewport().size() / 8.f;
    State.MaximumSize = gs_vec2f(gs_huge<float>(), gs_huge<float>());
}

bool ImmediateUserInterfaceWindowDockGizmo::events(ImmediateUserInterfaceContextLayer* _Context)
{
    (void)_Context;
    return false;
}

void ImmediateUserInterfaceWindowDockGizmo::attach_child(ImmediateUserInterfaceNode* _Child)
{
    (void)_Child;
}

bool ImmediateUserInterfaceWindowDockGizmo::create_contents(
    ImmediateUserInterfaceContextLayer*       _Context, 
    const std::string&                        _ID,
    const ImmediateUserInterfaceNodeSettings& _Settings,
    bool*                                     _Render)
{
    (void)_Context;
    (void)_ID;
    (void)_Settings;
    (void)_Render;
    return true;
}

void ImmediateUserInterfaceWindowDockGizmo::load_state(ImmediateUserInterfaceContextLayer*){}
void ImmediateUserInterfaceWindowDockGizmo::save_state(ImmediateUserInterfaceContextLayer*){}

// ImmediateUserInterfaceWindowRoot
ImmediateUserInterfaceWindowRoot::ImmediateUserInterfaceWindowRoot(const std::string& _Name) : ImmediateUserInterfaceVerticalStack(_Name){}
ImmediateUserInterfaceWindowRoot::~ImmediateUserInterfaceWindowRoot(){}

// ImmediateUserInterfaceWindowFrame
ImmediateUserInterfaceWindowFrame::ImmediateUserInterfaceWindowFrame(const std::string& _Name) : ImmediateUserInterfaceScrollArea(_Name){}
ImmediateUserInterfaceWindowFrame::~ImmediateUserInterfaceWindowFrame(){}

void ImmediateUserInterfaceWindowFrame::render_background(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    ImmediateUserInterfaceWindow* window = _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceWindow>(this);

    
    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
        State.BoundingBox.Max - _Context->m_Style.get_frames_width(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());
}

// ImmediateUserInterfaceWindowFrameButton
ImmediateUserInterfaceWindowFrameButton::ImmediateUserInterfaceWindowFrameButton(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceWindowFrameButton::~ImmediateUserInterfaceWindowFrameButton(){}

void ImmediateUserInterfaceWindowFrameButton::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    auto parent = _Context->m_Hierarchy.get_parent(this);

    float maxWidth = parent != nullptr ? parent->State.BoundingBox.width() : 256.f;

    // layout self
    State.MinimumSize = gs_vec2f(0.f, gs_max(_Context->m_Style.get_font_size() * 2.f, 64.f));
    State.MaximumSize = gs_vec2f(gs_huge<float>(), gs_max(_Context->m_Style.get_font_size() * 2.f, 64.f));

    // layout close button
    float buttonSize = gs_max(_Context->m_Style.get_font_size() * 0.5f, 16.f);

    CloseButtonBox  = gs_2d_boxf(
        gs_vec2f(State.BoundingBox.Max.x - buttonSize - _Context->m_Style.get_frames_radius() - _Context->m_Style.get_frames_width() * 2.f - _Context->m_Style.get_minimum_frames_width() * 2.f, State.BoundingBox.center().y - buttonSize * 0.5f),
        gs_vec2f(State.BoundingBox.Max.x - buttonSize - _Context->m_Style.get_frames_radius() - _Context->m_Style.get_frames_width() * 2.f - _Context->m_Style.get_minimum_frames_width() * 2.f, State.BoundingBox.center().y - buttonSize * 0.5f) + buttonSize);
}

void ImmediateUserInterfaceWindowFrameButton::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr || Window == nullptr) return;

    if(dynamic_cast<ImmediateUserInterfaceWindowDockGizmo*>(Window))
    {
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
        return;
    }

    if(Window->IsActive && (Window->Docker != nullptr || !Window->DockedWindowsCache.empty()))
    {
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
            State.BoundingBox.Max - gs_vec2f(_Context->m_Style.get_frames_width(), 0.f),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }
    else
    {
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
            State.BoundingBox.Max - gs_vec2f(_Context->m_Style.get_frames_width(), 0.f),
            State.MouseHover & ImmediateUserInterfaceNodeMouseHover_MouseHovered && (Window->Docker != nullptr || !Window->DockedWindowsCache.empty()) ?
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackgroundHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }

    _Context->m_Renderer->push_text_wrapped(
        gs_vec2f(
            State.BoundingBox.Min.x + _Context->m_Style.get_font_size() * 0.5f,
            State.BoundingBox.center().y - _Context->m_Style.get_font_size() * 0.5f),
        Window->Name.begin(),
        Window->Name.end(),
        gs_2d_boxf(
            State.BoundingBox.Min + gs_vec2f((State.BoundingBox.Max - CloseButtonBox.Min).x, 0.f),
            State.BoundingBox.Max - gs_vec2f((State.BoundingBox.Max - CloseButtonBox.Min).x, 0.f)),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_current_font());

    if(Window->Opened)
        ImmediateUserInterfaceContextLayerHelpers::render_close_button(_Context, this, CloseButtonBox);
}

bool ImmediateUserInterfaceWindowFrameButton::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return false;

    // close
    if(_Context->m_Input.is_mouse_button_clicked() && CloseButtonBox.contains(_Context->m_Input.get_cusor_position()))
    {
        if(Window && Window->Opened)
            *Window->Opened = false;

        return true;
    }

    // activate window
    if(_Context->m_Input.is_mouse_button_clicked())
    {
        if(Window)
            Window->Activate = true;
    }

    // move window
    if(_Context->m_Input.is_mouse_button_pressed())
        Pressed = true;
    else if(!_Context->m_Input.is_mouse_button_down())
        Pressed = false;

    if(Pressed && gs_vector_length(_Context->m_Input.get_cusor_drag_delta()) > 8.f)
    {
        if(Window)
        {
            Window->State.Events    |= ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved;
            Window->ReattachChildren = true;
        }

        return true;
    }

    return ImmediateUserInterfaceNode::events(_Context);
}

// ImmediateUserInterfaceDialog
ImmediateUserInterfaceDialog::ImmediateUserInterfaceDialog(const std::string& _Name) : ImmediateUserInterfacePanel(_Name){}
ImmediateUserInterfaceDialog::~ImmediateUserInterfaceDialog(){}

void ImmediateUserInterfaceDialog::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    // layout self
    State.BoundingBox = gs_2d_boxf(
        _Context->m_Renderer->current_viewport().Min - _Context->m_Style.get_frames_width(),
        _Context->m_Renderer->current_viewport().Max + _Context->m_Style.get_frames_width());
}

void ImmediateUserInterfaceDialog::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr || !(State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ShowDialogBlur)) return;

    // outline
    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        gs_color_rgba(128, 128, 128, 128),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());
}

void ImmediateUserInterfaceDialog::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(dynamic_cast<ImmediateUserInterfaceDialogContent*>(_Child))
    {
        _Child->State.Parent = this;
        return;
    }

    if(Contents)
        Contents->attach_child(_Child);
}

bool ImmediateUserInterfaceDialog::create_contents(
    ImmediateUserInterfaceContextLayer*       _Context, 
    const std::string&                        _ID,
    const ImmediateUserInterfaceNodeSettings& _Settings,
    bool*                                     _Render)
{
    // disable self moving
    State.Settings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;

    int settings = _Settings;
    settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop; // this we need for menu bars
    settings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent;

    if(_Context->begin_node<ImmediateUserInterfaceDialogContent>(_Context->next_id("Panel"), settings))
    {
        Contents = _Context->get_rendering_stack_top<ImmediateUserInterfaceDialogContent>();

        _Context->end_node<ImmediateUserInterfaceDialogContent>();
    }

    return true;
}

// ImmediateUserInterfaceDialogContent
ImmediateUserInterfaceDialogContent::ImmediateUserInterfaceDialogContent(const std::string& _Name) : ImmediateUserInterfaceNode(_Name){}
ImmediateUserInterfaceDialogContent::~ImmediateUserInterfaceDialogContent(){}

void ImmediateUserInterfaceDialogContent::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // compute self geometry
    FrameBox = gs_2d_boxf(
        State.BoundingBox.Min + _Context->m_Style.get_frames_width(),
        gs_vec2f(
            State.BoundingBox.Max.x,
            State.BoundingBox.Min.y + gs_max(_Context->m_Style.get_font_size() * 2.f, 64.f)) - _Context->m_Style.get_frames_width());

    ContentBox = gs_2d_boxf(gs_vec2f(FrameBox.Min.x, FrameBox.Max.y), State.BoundingBox.Max);

    ContentBox = gs_2d_boxf(
        ContentBox.Min + _Context->m_Style.get_frames_width(),
        ContentBox.Max - _Context->m_Style.get_frames_width());

    ImmediateUserInterfaceContextLayerHelpers::layout_nodes_as_vertical_stack(
        _Context,
        _Context->m_Hierarchy.begin(this),
        _Context->m_Hierarchy.end(this),
        ContentBox.Min,
        ContentBox.size(),
        gs_vec4f(0.f),
        gs_vec4f(0.f),
        State.Settings,
        [this](const ImmediateUserInterfaceNode* _Node){return true;});
}

bool ImmediateUserInterfaceDialogContent::events(ImmediateUserInterfaceContextLayer* _Context)
{                
    return ImmediateUserInterfaceNode::events(_Context);
}

void ImmediateUserInterfaceDialogContent::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // outline
    {
        _Context->m_Renderer->push_rectangle_filled(
            State.BoundingBox.Min,
            State.BoundingBox.Max,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }

    // frame
    {
        // framebox
        _Context->m_Renderer->push_rectangle_filled(
            FrameBox.Min,
            FrameBox.Max,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());

        // close button
        float buttonSize = gs_max(_Context->m_Style.get_font_size() * 0.5f, 16.f);

        gs_2d_boxf closeButtonBox  = gs_2d_boxf(
            gs_vec2f(FrameBox.Max.x - buttonSize - _Context->m_Style.get_frames_radius() - _Context->m_Style.get_frames_width() * 2.f - _Context->m_Style.get_minimum_frames_width() * 2.f, FrameBox.center().y - buttonSize * 0.5f),
            gs_vec2f(FrameBox.Max.x - buttonSize - _Context->m_Style.get_frames_radius() - _Context->m_Style.get_frames_width() * 2.f - _Context->m_Style.get_minimum_frames_width() * 2.f, FrameBox.center().y - buttonSize * 0.5f) + buttonSize);

        ImmediateUserInterfaceContextLayerHelpers::render_close_button(_Context, this, closeButtonBox);

        ImmediateUserInterfaceDialog* dialog =
            _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceDialog>(this);
        
        if(dialog != nullptr && dialog->Opened != nullptr && State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered)
            *dialog->Opened = !(closeButtonBox.contains(_Context->m_Input.get_cusor_position()) && _Context->m_Input.is_mouse_button_clicked());

        // title
        if(dialog != nullptr)
        {
            _Context->m_Renderer->push_text(
                gs_vec2f(FrameBox.Min.x + _Context->m_Style.get_font_size() * 0.5f, FrameBox.center().y - _Context->m_Style.get_font_size() * 0.5f),
                dialog->Name.begin(),
                dialog->Name.end(),
                _Context->m_Style.get_font_size(),
                _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                _Context->m_Style.get_current_font());
        }
    }

    // content
    {
        _Context->m_Renderer->push_rectangle_filled(
            ContentBox.Min,
            ContentBox.Max,
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }
}

// ImmediateUserInterfaceAxis
ImmediateUserInterfacePlotAxis::ImmediateUserInterfacePlotAxis(const std::string& _Hash) : ImmediateUserInterfaceNode(_Hash){}
ImmediateUserInterfacePlotAxis::~ImmediateUserInterfacePlotAxis(){}

gs_vec2f ImmediateUserInterfacePlotAxis::get_scroll_offset(const bool& _Scaled) const
{
    return gs_vec2f(-CurrentOffset.x, -CurrentOffset.y);
}

bool ImmediateUserInterfacePlotAxis::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return false;
    
    // scroll
    if(Settings & ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Scrollable)
    {
        if(_Context->m_Input.is_mouse_button_pressed())
        {
            Edited         = true;
            PreviousOffset = CurrentOffset;
        }

        if(_Context->m_Input.is_mouse_button_down() && Edited)
            CurrentOffset = PreviousOffset + _Context->m_Input.get_cusor_drag_delta();

        if(_Context->m_Input.is_mouse_button_released())
            Edited = false;
    }

    // zoom
    if(ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Zoomable)
    {
        if(_Context->m_Input.has_modifier(ApplicationPlatformBackendKeyModifier::Modifier::ApplicationPlatformBackendKeyModifier_Ctrl) &&
            gs_vector_length(_Context->m_Input.get_mouse_wheel_scroll_offset()) > 0.f)
        {
            _Context->get_controller<ImmediateUserInterfaceScrollBarsController>()->Locked = true;

            ZoomScale = gs_clamp(
                _Context->m_Input.get_mouse_wheel_scroll_offset().y > 0.f ?
                    ZoomScale * 0.5f :
                        ZoomScale * 1.5f, MinZoomScale, MaxZoomScale);
        }
    }

    return true;
}

void ImmediateUserInterfacePlotAxis::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    GS_ASSERT(dynamic_cast<ImmediateUserInterfacePlotWidget*>(State.Scope));

    MinScaled = MinReference * ZoomScale;
    MaxScaled = MaxReference * ZoomScale;
}

// ImmediateUserInterfaceVerticalAxis
ImmediateUserInterfaceVerticalPlotAxis::ImmediateUserInterfaceVerticalPlotAxis(const std::string& _Hash) : ImmediateUserInterfacePlotAxis(_Hash){}
ImmediateUserInterfaceVerticalPlotAxis::~ImmediateUserInterfaceVerticalPlotAxis(){}

void ImmediateUserInterfaceVerticalPlotAxis::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;
    
    // call base implementation
    ImmediateUserInterfacePlotAxis::layout(_Context);

    // custom logic
    ImmediateUserInterfaceNode* parent = _Context->m_Hierarchy.get_parent(this);

    LabelSize = _Context->m_Renderer->calculate_bounding_box(
        LabelFormat.begin(),
        LabelFormat.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_current_font()).size();

    State.MinimumSize = gs_vec2f(
        LabelSize.x * 2.f,
            parent != nullptr ? parent->State.BoundingBox.height() : State.MinimumSize.y);
    
    State.MaximumSize = gs_vec2f(
        LabelSize.x * 2.f,
            parent != nullptr ? parent->State.BoundingBox.height() : State.MaximumSize.y);
}

void ImmediateUserInterfaceVerticalPlotAxis::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // background
    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsAxis),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());

    // labels
    float offset = CurrentOffset.y;
    while(gs_abs(offset) > State.BoundingBox.height())
        offset = (gs_abs(offset) - State.BoundingBox.height()) * gs_sign(offset);

    gs_vec2f interval = State.BoundingBox.size() / gs_vec2f(1.f, TicksCount);
    gs_vec2f position = State.BoundingBox.Min + gs_vec2f(0.f, offset);

    while(position.y < State.BoundingBox.Min.y)
        position += gs_vec2f(0.f, interval.y);

    while(position.y > State.BoundingBox.Min.y)
        position -= gs_vec2f(0.f, interval.y);

    float oneTick     = (MaxScaled.y - MinScaled.y) / TicksCount;
    float currentTick = (MinScaled.y - (ceilf(CurrentOffset.y / interval.y)) * oneTick);

    for (int i = 0; i < TicksCount; i++, currentTick += oneTick)
    {
        _Context->m_Renderer->push_rectangle_filled(
            position,
            position + gs_vec2f(State.BoundingBox.width() * 0.1f, _Context->m_Style.get_frames_width()),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsAxisTicks),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

        std::string text = Frenchie::Core::String::format("%.2f", -currentTick);

        _Context->m_Renderer->push_text(
            position + gs_vec2f(State.BoundingBox.width() * 0.15f, 0.f),
            text.begin(),
            text.end(),
            _Context->m_Style.get_font_size(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_current_font());

        position += gs_vec2f(0.f, interval.y);
    }

    // axis name
    std::string labelFormat = "0.00"; // TODO: this MUST BE a setting
    float labelWidth = _Context->m_Renderer->calculate_bounding_box(
        labelFormat.begin(),
        labelFormat.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_current_font()).width();

    float axisNameWidth = _Context->m_Renderer->calculate_bounding_box(
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_current_font()).width();

    _Context->m_Renderer->push_text(
        gs_vec2f(0.f, 0.f),
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix(
            (float)place_in_follow(),
            State.BoundingBox.center() + gs_vec2f(labelWidth, -axisNameWidth * 0.5f),
            90.f),
        _Context->m_Style.get_current_font());
}

bool ImmediateUserInterfaceVerticalPlotAxis::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(!ImmediateUserInterfacePlotAxis::events(_Context))
        return false;

    CurrentOffset = gs_vec2f(0.f, CurrentOffset.y);

    return true;
}

// ImmediateUserInterfaceHorizontalAxis
ImmediateUserInterfaceHorizontalPlotAxis::ImmediateUserInterfaceHorizontalPlotAxis(const std::string& _Hash) : ImmediateUserInterfacePlotAxis(_Hash){}
ImmediateUserInterfaceHorizontalPlotAxis::~ImmediateUserInterfaceHorizontalPlotAxis(){}

void ImmediateUserInterfaceHorizontalPlotAxis::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    // call base implementation
    ImmediateUserInterfacePlotAxis::layout(_Context);

    // custom logic
    ImmediateUserInterfaceNode* parent = _Context->m_Hierarchy.get_parent(this);

    LabelSize = _Context->m_Renderer->calculate_bounding_box(
        LabelFormat.begin(),
        LabelFormat.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_current_font()).size();

    State.MinimumSize = gs_vec2f(
        parent != nullptr ? parent->State.BoundingBox.width() : State.MinimumSize.x,
            ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context) * 2.f);
    
    State.MaximumSize = gs_vec2f(
        parent != nullptr ? parent->State.BoundingBox.width() : State.MaximumSize.x,
            ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context) * 2.f);
}

void ImmediateUserInterfaceHorizontalPlotAxis::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    // background
    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsAxis),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());

    // labels
    float offset = CurrentOffset.x;
    while(gs_abs(offset) > State.BoundingBox.width())
        offset = (gs_abs(offset) - State.BoundingBox.width()) * gs_sign(offset);

    gs_vec2f interval = State.BoundingBox.size() / gs_vec2f(TicksCount, 1.f);
    gs_vec2f position = State.BoundingBox.Min + gs_vec2f(offset, 0.f);

    while(position.x < State.BoundingBox.Min.x)
        position += gs_vec2f(interval.x, 0.f);

    while(position.x > State.BoundingBox.Min.x)
        position -= gs_vec2f(interval.x, 0.f);

    float oneTick     = (MaxScaled.x - MinScaled.x) / TicksCount;
    float currentTick = MinScaled.x - (ceilf(CurrentOffset.x / interval.x)) * oneTick;

    for (int i = 0; i < TicksCount; i++, currentTick += oneTick)
    {
        _Context->m_Renderer->push_rectangle_filled(
            position + gs_vec2f(LabelSize.x * 0.5f, 0.f),
            position + gs_vec2f(LabelSize.x * 0.5f, 0.f) + gs_vec2f(_Context->m_Style.get_frames_width(), State.BoundingBox.height() * 0.1f),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsAxisTicks),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

        std::string text = Frenchie::Core::String::format("%.2f", currentTick);

        _Context->m_Renderer->push_text(
            position + gs_vec2f(0.f, State.BoundingBox.height() * 0.15f),
            text.begin(),
            text.end(),
            _Context->m_Style.get_font_size(),
            _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_current_font());

        position += gs_vec2f(interval.x, 0.f);
    }

    // axis name
    float axisNameWidth = _Context->m_Renderer->calculate_bounding_box(
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_current_font()).width();

    _Context->m_Renderer->push_text(
        gs_vec2f(
            State.BoundingBox.center().x - axisNameWidth * 0.5f,
            State.BoundingBox.Min.y + ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context)),
        Name.begin(),
        Name.end(),
        _Context->m_Style.get_font_size(),
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_current_font());
}

bool ImmediateUserInterfaceHorizontalPlotAxis::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(!ImmediateUserInterfacePlotAxis::events(_Context))
        return false;

    CurrentOffset = gs_vec2f(CurrentOffset.x, 0.f);

    return true;
}

// ImmediateUserInterfacePlot
ImmediateUserInterfacePlot::ImmediateUserInterfacePlot(const std::string& _Hash) : ImmediateUserInterfaceNode(_Hash){}
ImmediateUserInterfacePlot::~ImmediateUserInterfacePlot(){}

void ImmediateUserInterfacePlot::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    GS_ASSERT(_Context && _Context->m_Hierarchy.get_parent<ImmediateUserInterfacePlotWidget>(this));
}
void ImmediateUserInterfacePlot::measure(ImmediateUserInterfaceContextLayer* _Context){}

// ImmediateUserInterfacePlotLegend
ImmediateUserInterfacePlotLegend::ImmediateUserInterfacePlotLegend(const std::string& _Hash) : ImmediateUserInterfaceNode(_Hash){}
ImmediateUserInterfacePlotLegend::~ImmediateUserInterfacePlotLegend(){}

void ImmediateUserInterfacePlotLegend::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    // layout self
    State.MinimumSize = gs_vec2f(
        _Context->m_Renderer->calculate_bounding_box(
            Name.begin(),
            Name.end(),
            _Context->m_Style.get_font_size(),
            _Context->m_Style.get_current_font()).width() * 3.f,

        ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context));
    
    State.MaximumSize = State.MinimumSize;

    // caluclate button box
    ButtonBox = gs_2d_boxf(
        State.BoundingBox.Min,
        gs_vec2f(State.BoundingBox.center().x, State.BoundingBox.Max.y));
}

void ImmediateUserInterfacePlotLegend::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr)
        return;

    gs_color buttonColor = Color;

    if(ButtonBox.contains(_Context->m_Input.get_cusor_position()))
    {
        buttonColor = _Context->m_Input.is_mouse_button_down() ?
            gs_color_rgb(gs_color_rgba_get_r(buttonColor) * 0.8, gs_color_rgba_get_g(buttonColor) * 0.8, gs_color_rgba_get_b(buttonColor) * 0.8) :
                gs_color_rgb(gs_color_rgba_get_r(buttonColor) * 0.5, gs_color_rgba_get_g(buttonColor) * 0.5, gs_color_rgba_get_b(buttonColor) * 0.5);
    }

    // render button
    if(Checked)
    {
        _Context->m_Renderer->push_rectangle_filled(
            ButtonBox.Min,
            ButtonBox.Max,
            buttonColor,
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }
    else
    {
        _Context->m_Renderer->push_rectangle(
            ButtonBox.Min,
            ButtonBox.Max,
            gs_color_rgb(128, 128, 128),
            _Context->m_Style.get_frames_width(),
            _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
            _Context->m_Style.get_frames_radius());
    }

    // render name
    _Context->m_Renderer->push_text(

    gs_vec2f(

        State.BoundingBox.center().x,
        State.BoundingBox.center().y - _Context->m_Renderer->calculate_bounding_box(
            Name.begin(),
            Name.end(),
            _Context->m_Style.get_font_size(),
            _Context->m_Style.get_current_font()).height() * 0.5f),

    Name.begin(),
    Name.end(),
    _Context->m_Style.get_font_size(),
    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
    _Context->m_Style.get_current_font());
}

bool ImmediateUserInterfacePlotLegend::events(ImmediateUserInterfaceContextLayer* _Context)
{
    if(
        _Context != nullptr &&
        ButtonBox.contains(_Context->m_Input.get_cusor_position()) &&
        _Context->m_Input.is_mouse_button_clicked(Frenchie::Application::ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonLeft))
    {
        Checked = !Checked;
        return true;
    }

    return false;
}

// ImmediateUserInterfacePlotSurface
ImmediateUserInterfacePlotViewItem::ImmediateUserInterfacePlotViewItem(const std::string& _Hash) : ImmediateUserInterfaceNode(_Hash){}
ImmediateUserInterfacePlotViewItem::~ImmediateUserInterfacePlotViewItem(){}

// ImmediateUserInterfacePlotArea
ImmediateUserInterfacePlotView::ImmediateUserInterfacePlotView(const std::string& _Hash) : ImmediateUserInterfaceNode(_Hash){}
ImmediateUserInterfacePlotView::~ImmediateUserInterfacePlotView(){}

void ImmediateUserInterfacePlotView::layout(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    for(auto it = _Context->m_Hierarchy.begin(this); it != _Context->m_Hierarchy.end(this); it++)
    {
        // layout plots
        ImmediateUserInterfacePlot* plot =
            dynamic_cast<ImmediateUserInterfacePlot*>(*it);

        if(plot != nullptr)
        {
            gs_vec2f offset = gs_vec2f(
                plot->XAxis != nullptr ? plot->XAxis->CurrentOffset.x : 0.f,
                plot->YAxis != nullptr ? plot->YAxis->CurrentOffset.y : 0.f);

            plot->State.BoundingBox = gs_2d_boxf(
                State.BoundingBox.Min + offset,
                State.BoundingBox.Min + offset + plot->State.BoundingBox.size());
        }

        // layout plots surfaces
        ImmediateUserInterfacePlotViewItem* surface =
            dynamic_cast<ImmediateUserInterfacePlotViewItem*>(*it);

        if(surface != nullptr)
        {
            surface->State.BoundingBox = gs_2d_boxf(
                State.BoundingBox.Min,
                State.BoundingBox.Min + surface->State.BoundingBox.size());
        }
    }
}

void ImmediateUserInterfacePlotView::render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr || _Context->m_Renderer == nullptr) return;

    _Context->m_Renderer->push_rectangle_filled(
        State.BoundingBox.Min,
        State.BoundingBox.Max,
        _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_2DPlotsBackground),
        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
        _Context->m_Style.get_frames_radius());
}

// ImmediateUserInterfacePlotWidget
ImmediateUserInterfacePlotWidget::ImmediateUserInterfacePlotWidget(const std::string& _Hash) : ImmediateUserInterfacePanel(_Hash){}
ImmediateUserInterfacePlotWidget::~ImmediateUserInterfacePlotWidget(){}

void ImmediateUserInterfacePlotWidget::attach_child(ImmediateUserInterfaceNode* _Child)
{
    if(dynamic_cast<ImmediateUserInterfacePlot*>(_Child) != nullptr && PlotsView != nullptr)
    {
        dynamic_cast<ImmediateUserInterfacePlot*>(_Child)->XAxis = CurrentXAxis;
        dynamic_cast<ImmediateUserInterfacePlot*>(_Child)->YAxis = CurrentYAxis;
        PlotsView->attach_child(_Child);
        return;
    }

    if(dynamic_cast<ImmediateUserInterfacePlotViewItem*>(_Child) != nullptr && PlotsView != nullptr)
    {
        PlotsView->attach_child(_Child);
        return;
    }

    if(dynamic_cast<ImmediateUserInterfaceHorizontalPlotAxis*>(_Child) != nullptr)
    {
        CurrentXAxis = dynamic_cast<ImmediateUserInterfaceHorizontalPlotAxis*>(_Child);
        
        if(XAxisView != nullptr)
            XAxisView->attach_child(_Child);
        
        return;
    }

    if(dynamic_cast<ImmediateUserInterfaceVerticalPlotAxis*>(_Child) != nullptr)
    {
        CurrentYAxis = dynamic_cast<ImmediateUserInterfaceVerticalPlotAxis*>(_Child);
        CurrentYAxis->State.NextLine = 0;
        
        if(YAxisView != nullptr)
            YAxisView->attach_child(_Child);
        
        return;
    }

    _Child->State.Parent = this;
}

bool ImmediateUserInterfacePlotWidget::create_contents(
    ImmediateUserInterfaceContextLayer*       _Context, 
    const std::string&                        _ID,
    const ImmediateUserInterfaceNodeSettings& _Settings,
    bool*                                     _Render)
{
    ImmediateUserInterfaceNodeSettings settings = _Settings;
    settings &= ~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent;

    if(_Context->begin_vertical_stack(_Context->next_id("Root"), settings))
    {
        float plotWidth = 0.f;

        // here we do not resize Y axis vertically to contents as plot MUST stay resizable
        if(_Context->begin_horizontal_stack(
            _Context->next_id("Plots"),
              ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft))
        {
            // plots
            _Context->next_order_in_follow();

            if(_Context->begin_node<ImmediateUserInterfacePlotView>(
                _Context->next_id("Plots"),
                ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable))
            {
                PlotsView = _Context->get_rendering_stack_top();
                plotWidth = _Context->current_bounding_box(_Context->get_rendering_stack_top()).width();

                _Context->end_node<ImmediateUserInterfacePlotView>();
            }

            // y-axis
            // if any y axis has been added to plot widget then current x axis is not nullptr
            if(CurrentYAxis != nullptr)
            {
                _Context->next_content_margin(gs_vec4f(0.f, 12.f, 0.f, 0.f));

                if(_Context->begin_scrollarea(
                    _Context->next_id("YAxis"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren

                    // y axis horizontal fit
                    | (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_PlotFitYAxis ?
                            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally :
                                0)

                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar))
                {
                    YAxisView = _Context->get_rendering_stack_top();
                    _Context->end_scrollarea();
                }
            }

            _Context->end_horizontal_stack();
        }

        // x-axis
        if(_Context->begin_horizontal_stack(
            _Context->next_id("Axis"),
              ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable

            // x axis vertical fit
            | (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_PlotFitXAxis ?
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically :
                        0)

            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft))
        {
            // x-axis
            // if any x axis has been added to plot widget then current x axis is not nullptr
            if(CurrentXAxis != nullptr)
            {
                _Context->next_width(plotWidth);
                _Context->next_content_margin(gs_vec4f(12.f, 0.f, 0.f, 0.f));

                if(_Context->begin_scrollarea(_Context->next_id("XAxis"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren

                    // x axis vertical fit
                    | (State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_PlotFitXAxis ?
                            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically :
                                0)

                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar))
                {
                    XAxisView = _Context->get_rendering_stack_top();
                    _Context->end_scrollarea();
                }
            }

            // legend
            if(State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_PlotDrawLegend)
            {
                _Context->next_content_margin(_Context->get_content_default_margin());

                if(_Context->begin_scrollarea(_Context->next_id("Legend"),
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar))
                {
                    int counter = 0;
                    
                    for(auto it = _Context->m_Hierarchy.begin(PlotsView); it != _Context->m_Hierarchy.end(PlotsView); it++)
                    {
                        ImmediateUserInterfacePlot* plot =
                            dynamic_cast<ImmediateUserInterfacePlot*>(*it);

                        if(plot == nullptr)
                            continue;

                        if(_Context->begin_node<ImmediateUserInterfacePlotLegend>(
                            _Context->next_id((*it)->Name, Frenchie::Core::String::format("legend-%d", counter++)),
                            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                        {
                            ImmediateUserInterfacePlotLegend* legend =
                                _Context->get_rendering_stack_top<ImmediateUserInterfacePlotLegend>();

                            legend->Color = plot->Color;

                            if(!legend->Checked)
                                plot->disable();
                            else
                                plot->enable();

                            _Context->end_node<ImmediateUserInterfacePlotLegend>();
                        }
                    }

                    _Context->end_scrollarea();
                }
            }

            _Context->end_horizontal_stack();
        }

        _Context->end_vertical_stack();

        return true;
    }

    return false;
}

// ImmediateUserInterfaceWindowsController
ImmediateUserInterfaceWindowsController::ImmediateUserInterfaceWindowsController(){}
ImmediateUserInterfaceWindowsController::~ImmediateUserInterfaceWindowsController(){}

void ImmediateUserInterfaceWindowsController::frame_start(ImmediateUserInterfaceContextLayer* _Context){}

void ImmediateUserInterfaceWindowsController::frame_update(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr) return;

    // create worksapce dockarea
    m_DockAreaOpened = (_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking);

    if(!m_DockAreaOpened) return;

    _Context->next_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Background);

    if(_Context->begin_node<ImmediateUserInterfaceWindowDockArea>(
        _Context->next_id(ApplicationPlatformBackend::get_window_name(), m_DockingWorkspaceName),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        nullptr))
    {
        // retrieve window
        m_WorkspaceDockArea = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindow>();
        _Context->end_node<ImmediateUserInterfaceWindowDockArea>();
    }
}

void ImmediateUserInterfaceWindowsController::frame_input(ImmediateUserInterfaceContextLayer* _Context)
{
    place_on_dockers(_Context);

    // rebuild hierarchy
    m_WindowsList.clear();
    m_NodesList.clear();
    
    for(auto node : _Context->m_NodesRenderingList)
    {
        if(dynamic_cast<ImmediateUserInterfaceWindow*>(node))
            m_WindowsList.push_back(node);
        else
            m_NodesList.push_back(node);
    }

    std::stable_sort(
        m_WindowsList.begin(),
        m_WindowsList.end(),
        [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B) 
        {
            return dynamic_cast<const ImmediateUserInterfaceWindow*>(_A)->DockingIndex <
                dynamic_cast<const ImmediateUserInterfaceWindow*>(_B)->DockingIndex;
        });

    _Context->m_NodesRenderingList.clear();

    for(auto node : m_WindowsList)
        _Context->m_NodesRenderingList.push_back(node);

    for(auto node : m_NodesList)
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

                std::vector<ImmediateUserInterfaceNode*> dockedWindows =
                    retrieve_docked_windows(_Context, docker, ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Center);

                for(auto node : dockedWindows)
                {
                    if(dynamic_cast<ImmediateUserInterfaceWindow*>(node) != nullptr)
                        dynamic_cast<ImmediateUserInterfaceWindow*>(node)->IsActive = false;
                }

                // activate self
                window->IsActive = true;
            }

            // activate singletone window
            std::vector<ImmediateUserInterfaceNode*> dockedWindows =
                retrieve_docked_windows(_Context, window, ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_All);

            if((window->Docker        == nullptr &&
                window->TopSnapper    == nullptr &&
                window->LeftSnapper   == nullptr &&
                window->RightSnapper  == nullptr &&
                window->BottomSnapper == nullptr &&
                dockedWindows.empty()))
            {
                window->IsActive = true;
            }

            // setup visible areas
            if(window->IsActive)
            {
                if(window->SnapperView)
                    window->SnapperView->enable();
                
                if(window->DockerView)
                    window->DockerView->disable();
            }
            else
            {
                if(window->SnapperView)
                    window->SnapperView->disable();
                
                if(window->DockerView)
                    window->DockerView->enable();
            }

            // reset all
            window->Activate         = false;
            window->ReattachChildren = false;
        }
    }
}

void ImmediateUserInterfaceWindowsController::frame_finish(ImmediateUserInterfaceContextLayer* _Context)
{
    // extract opened windows
    std::set<ImmediateUserInterfaceNode*> openedWindows;
    
    for(auto node : _Context->m_NodesRenderingList)
    {
        if(dynamic_cast<ImmediateUserInterfaceWindow*>(node))
            openedWindows.insert(node);
    }

    for(auto node : _Context->m_NodesRenderingList)
    {
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(node);

        if(window == nullptr)
            continue;
        
        // detach all windows from closed window
        ImmediateUserInterfaceWindow* docker = nullptr;

        if(window->Docker != nullptr)
            docker = ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, window->Docker);
        else if(window->TopSnapper != nullptr)
            docker = ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, window->TopSnapper);
        else if(window->LeftSnapper != nullptr)
            docker = ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, window->LeftSnapper);
        else if(window->RightSnapper != nullptr)
            docker = ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, window->RightSnapper);
        else if(window->BottomSnapper != nullptr)
            docker = ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, window->BottomSnapper);

        if(
            (docker != nullptr && openedWindows.find(docker) == openedWindows.end()) ||
            (docker != nullptr && docker->Opened != nullptr && !(*docker->Opened)))
        {
            std::vector<ImmediateUserInterfaceNode*> dockedWindows = _Context->get_controller<ImmediateUserInterfaceWindowsController>()->retrieve_docked_windows(
                _Context,
                docker,
                ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_All);

            for(auto dockedWindow : dockedWindows)
            {
                ImmediateUserInterfaceWindow* window =
                    dynamic_cast<ImmediateUserInterfaceWindow*>(dockedWindow);

                if(window == nullptr)
                    continue;

                window->Docker         = nullptr;
                window->TopSnapper     = nullptr;
                window->LeftSnapper    = nullptr;
                window->RightSnapper   = nullptr;
                window->BottomSnapper  = nullptr;
                window->DockingIndex   = -1;
                
                window->set_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Main);
            }
        }

        // setup active window in docker if we are closed
        if(
            (openedWindows.find(window) == openedWindows.end()) ||
            (window->Opened != nullptr && window->Opened != nullptr && !(*window->Opened)))
        {
            detach_from_docker(_Context, window);
        }

        // save docked windows cache
        window->DockedWindowsCache =
            retrieve_docked_windows(_Context, window, ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Center);

        std::stable_sort(
            window->DockedWindowsCache.begin(),
            window->DockedWindowsCache.end(),
            [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)->bool
        {
            return dynamic_cast<const ImmediateUserInterfaceWindow*>(_A)->DockingIndex <
                    dynamic_cast<const ImmediateUserInterfaceWindow*>(_B)->DockingIndex;
        });
    }
}

void ImmediateUserInterfaceWindowsController::place_on_dockers(ImmediateUserInterfaceContextLayer* _Context)
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

    // find moved window and detach it from a docker
    auto movedNode = ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceMovedNodeSearcher().search(_Context, [](const ImmediateUserInterfaceNode*)->bool{return true;});

    ImmediateUserInterfaceWindow* moved =
        dynamic_cast<ImmediateUserInterfaceWindow*>(movedNode);

    if(moved == nullptr)
        moved = _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceWindow>(movedNode);

    if(moved != nullptr &&
        (moved->State.Events & ImmediateUserInterfaceWindowEvents_::ImmediateUserInterfaceWindowEvents_DragStarted))
    {
        moved->State.Events &= ~ImmediateUserInterfaceWindowEvents_::ImmediateUserInterfaceWindowEvents_DragStarted;

        float deltaY = _Context->m_Input.get_cusor_position().y - (moved->Cache.BoundingBox.Min.y + gs_max(_Context->m_Style.get_font_size() * 2.f, 64.f));

        moved->Cache.BoundingBox = gs_2d_boxf(
            moved->Cache.BoundingBox.Min + gs_vec2f(0.f, deltaY),
            moved->Cache.BoundingBox.Min + gs_vec2f(0.f, deltaY) + moved->State.BoundingBox.size());
    }

    detach_from_docker(_Context, moved);

    if(!(_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking))
        return;

    // find top most hovered node not equal to the moved one
    ImmediateUserInterfaceNode* hoveredNode = 
        ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceHoveredNodeSearcher().search(
            _Context,
            [moved, _Context](const ImmediateUserInterfaceNode* _Node)->bool
            {
                return _Node != moved && _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceWindow>(_Node) != moved;
            }
        );

    // detect hovered window
    ImmediateUserInterfaceWindow* hovered = _Context->m_Hierarchy.get_parent<ImmediateUserInterfaceWindow>(hoveredNode);

    if(hovered == nullptr || moved == nullptr)
        return;

    // dock the moved window to the hovered one if all mouse buttons are released

    // compute gizmos
    gs_2d_boxf dockingGizmo = gs_2d_boxf(
        hovered->State.BoundingBox.center() - gs_min(hovered->State.BoundingBox.size().x, hovered->State.BoundingBox.size().y) * 0.25f,
        hovered->State.BoundingBox.center() + gs_min(hovered->State.BoundingBox.size().x, hovered->State.BoundingBox.size().y) * 0.25f);

    dockingGizmo = gs_2d_boxf(
        dockingGizmo.Min,
        dockingGizmo.Min + gs_min(dockingGizmo.size().x, 256.f));

    dockingGizmo = gs_2d_boxf(
        hovered->State.BoundingBox.center() - dockingGizmo.size() * 0.5f,
        hovered->State.BoundingBox.center() + dockingGizmo.size() * 0.5f);


    gs_vec2f gizmoSize = dockingGizmo.size() * 0.333f;

    gs_2d_boxf topDockingGizmo = gs_2d_boxf(
        dockingGizmo.Min + gs_vec2f(gizmoSize.x, 0.f),
        dockingGizmo.Min + gs_vec2f(gizmoSize.x * 2.f, gizmoSize.y));

    gs_2d_boxf leftDockingGizmo = gs_2d_boxf(
        dockingGizmo.Min + gs_vec2f(0.f, gizmoSize.y),
        dockingGizmo.Min + gs_vec2f(gizmoSize.x, gizmoSize.y * 2.f));

    gs_2d_boxf rightDockingGizmo = gs_2d_boxf(
        dockingGizmo.Min + gs_vec2f(gizmoSize.x * 2.f, gizmoSize.y),
        dockingGizmo.Min + gs_vec2f(gizmoSize.x * 3.f, gizmoSize.y * 2.f));

    gs_2d_boxf bottomDockingGizmo = gs_2d_boxf(
        dockingGizmo.Min + gs_vec2f(gizmoSize.x, gizmoSize.y * 2.f),
        dockingGizmo.Min + gs_vec2f(gizmoSize.x * 2.f, gizmoSize.y * 3.f));

    gs_2d_boxf centralDockingGizmo = gs_2d_boxf(
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
                ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Center);
        }
        if(topDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Top);
        }
        else if(leftDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Left);
        }
        else if(rightDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Right);
        }
        else if(bottomDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            attach_to_docker(
                _Context,
                hovered,
                moved,
                ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Bottom);
        }
    }
    else if(can_be_docked(_Context, moved, hovered))
    {
        // render potential docking window gizmo
        int depth = ImmediateUserInterfaceContextLayerHelpers::calculate_layer_depth(
            _Context,
            ImmediateUserInterfaceRenderingLayer_::ImmediateUserInterfaceRenderingLayer_Gizmos);

        if(dockingGizmo.contains(_Context->m_Input.get_cusor_position()))
        {
            _Context->m_Renderer->push_rectangle_filled(
                dockingGizmo.Min,
                dockingGizmo.Max,
                gs_color_rgba(
                    gs_color_rgba_get_r(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    gs_color_rgba_get_g(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    gs_color_rgba_get_b(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    128),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                _Context->m_Style.get_frames_radius());

            _Context->m_Renderer->push_rectangle_filled(
                topDockingGizmo.Min,
                topDockingGizmo.Max,
                topDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                _Context->m_Style.get_frames_radius());

            _Context->m_Renderer->push_rectangle_filled(
                leftDockingGizmo.Min,
                leftDockingGizmo.Max,
                leftDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                _Context->m_Style.get_frames_radius());

            _Context->m_Renderer->push_rectangle_filled(
                rightDockingGizmo.Min,
                rightDockingGizmo.Max,
                rightDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                _Context->m_Style.get_frames_radius());

            _Context->m_Renderer->push_rectangle_filled(
                bottomDockingGizmo.Min,
                bottomDockingGizmo.Max,
                bottomDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                _Context->m_Style.get_frames_radius());

            _Context->m_Renderer->push_rectangle_filled(
                centralDockingGizmo.Min,
                centralDockingGizmo.Max,
                centralDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ?
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_GizmosHovered) :
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                _Context->m_Renderer->calculate_transform_matrix((float)depth++),
                _Context->m_Style.get_frames_radius());

            // topDockingGizmo
            if(centralDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
            {
                _Context->m_Renderer->push_rectangle_filled(
                    hovered->DockedWindowsBox.Min,
                    hovered->DockedWindowsBox.Max,
                    _Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos),
                    _Context->m_Renderer->calculate_transform_matrix(ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node(hovered)),
                    _Context->m_Style.get_frames_radius());
            }
            else if(
               topDockingGizmo.contains(_Context->m_Input.get_cusor_position())   ||
               leftDockingGizmo.contains(_Context->m_Input.get_cusor_position())  ||
               rightDockingGizmo.contains(_Context->m_Input.get_cusor_position()) ||
               bottomDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
            {
                detach_from_docker(_Context, m_DockGizmo);

                if(m_DockGizmo != nullptr)
                {
                    if(topDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
                        attach_to_docker(_Context, hovered, m_DockGizmo, ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Top);
                    else if(leftDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
                        attach_to_docker(_Context, hovered, m_DockGizmo, ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Left);
                    else if(rightDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
                        attach_to_docker(_Context, hovered, m_DockGizmo, ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Right);
                    else if(bottomDockingGizmo.contains(_Context->m_Input.get_cusor_position()))
                        attach_to_docker(_Context, hovered, m_DockGizmo, ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Bottom);
                }

                if(_Context->begin_node<ImmediateUserInterfaceWindowDockGizmo>(
                    m_DockingGizmoName,
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    m_DockGizmo = _Context->get_rendering_stack_top<ImmediateUserInterfaceWindowDockGizmo>();
                    _Context->end_node<ImmediateUserInterfaceWindowDockGizmo>();
                }
            }
        }
    }
}

bool ImmediateUserInterfaceWindowsController::can_be_docked(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked)
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

void ImmediateUserInterfaceWindowsController::attach_to_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Docker, ImmediateUserInterfaceWindow* _Docked, const ImmediateUserInterfaceDockingAnchor& _Anchors)
{
    // auxiliary lambdas
    auto move_to_cache = [this](
        ImmediateUserInterfaceContextLayer* _Context,
        ImmediateUserInterfaceWindow*       _Docker)
    {
        if(_Context == nullptr || _Docker == nullptr)
            return;
        m_NodesList.push_back(_Docker);
    };

    auto move_child_docked_windows_to_cache = [this](
        ImmediateUserInterfaceContextLayer*          _Context,
        ImmediateUserInterfaceNode*                  _Docker,
        const ImmediateUserInterfaceDockingAnchor& _Orientation)
    {
        if(_Context == nullptr || _Docker == nullptr)
            return;

        std::vector<ImmediateUserInterfaceNode*> dockedWindows = retrieve_docked_windows(_Context, _Docker, _Orientation);
        for(auto dockedWindow : dockedWindows)
            m_NodesList.push_back(dockedWindow);
    };

    if(!can_be_docked(_Context, _Docker, _Docked))
        return;

    // get ready
    m_NodesList.clear();
    m_WindowsList.clear();

    // attach to a central part as a tab
    if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Center)
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

        for(auto it = m_NodesList.begin(); it != m_NodesList.end(); it++)
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
        m_NodesList.clear();
        m_WindowsList.clear();
        return;
    }

    // attach to top/left/right/bottom parts
    ImmediateUserInterfaceWindow* docker = _Docker;

    // move child docked windows and self to windows docking cache
    move_child_docked_windows_to_cache(_Context, docker, _Anchors);
    move_to_cache(_Context, _Docked);

    // reindex docked nodes and setup their docker
    int dockindex = 0;

    for(auto it = m_NodesList.begin(); it != m_NodesList.end(); it++)
    {
        ImmediateUserInterfaceWindow* window =
            dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

        if(window == nullptr)
            continue;

        if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Top)
            window->TopSnapper = docker->TopSnapperView;
        else if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Left)
            window->LeftSnapper = docker->LeftSnapperView;
        else if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Right)
            window->RightSnapper = docker->RightSnapperView;
        else if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Bottom)
            window->BottomSnapper = docker->BottomSnapperView;

        window->DockingIndex = dockindex++;
    }

    // clear
    m_NodesList.clear();
    m_WindowsList.clear();
}

void ImmediateUserInterfaceWindowsController::detach_from_docker(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceWindow* _Detached)
{
    if(_Detached == nullptr ||
        (_Detached->Docker        == nullptr &&
         _Detached->TopSnapper    == nullptr &&
         _Detached->LeftSnapper   == nullptr &&
         _Detached->RightSnapper  == nullptr &&
         _Detached->BottomSnapper == nullptr))
    {
        return;
    }

    // setup custom event
    if((_Detached->State.Events & ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_IsMoved))
        _Detached->State.Events |= ImmediateUserInterfaceWindowEvents_::ImmediateUserInterfaceWindowEvents_DragStarted;

    // reattach docked windows of detached window
    if(_Detached->ReattachChildren)
    {
        std::vector<ImmediateUserInterfaceNode*> dockedWindows = retrieve_docked_windows(
            _Context,
            _Detached,
            ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Center);

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

        return;
    }

    // setup active window within self
    std::vector<ImmediateUserInterfaceNode*> dockedWindows = retrieve_docked_windows(
        _Context,
        _Detached->Docker,
        ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Center);

    if (dockedWindows.size() > 1)
    {
        for(auto it = dockedWindows.begin(); it != dockedWindows.end(); it++)
        {
            ImmediateUserInterfaceWindow* window =
                dynamic_cast<ImmediateUserInterfaceWindow*>(*it);

            if(window != _Detached)
            {
                window->Activate = true;
                break;
            }
        }
    }
    else
    {
        ImmediateUserInterfaceWindow* docker =
            ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, _Detached->Docker);

        if(docker != nullptr)
            docker->Activate = true;
    }

    // detach from docker
    _Detached->TopSnapper    = nullptr;
    _Detached->LeftSnapper   = nullptr;
    _Detached->RightSnapper  = nullptr;
    _Detached->BottomSnapper = nullptr;
    _Detached->Docker        = nullptr;
    _Detached->DockingIndex  = -1;
}

std::vector<ImmediateUserInterfaceNode*> ImmediateUserInterfaceWindowsController::retrieve_docked_windows(
    ImmediateUserInterfaceContextLayer*         _Context,
    ImmediateUserInterfaceNode*                 _Docker,
    const ImmediateUserInterfaceDockingAnchor& _Anchors)
{
    ImmediateUserInterfaceWindow* docker =
        dynamic_cast<ImmediateUserInterfaceWindow*>(
            ImmediateUserInterfaceWindow::retrieve_docker_by_view(_Context, _Docker));

    if(docker == nullptr)
        return std::vector<ImmediateUserInterfaceNode*>();

    std::vector<ImmediateUserInterfaceNode*> windows;

    if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Top)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->TopSnapperView); it != _Context->m_Hierarchy.end(docker->TopSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                windows.push_back(*it);
        }
    }

    if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Left)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->LeftSnapperView); it != _Context->m_Hierarchy.end(docker->LeftSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                windows.push_back(*it);
        }
    }

    if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Right)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->RightSnapperView); it != _Context->m_Hierarchy.end(docker->RightSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                windows.push_back(*it);
        }
    }

    if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Bottom)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->BottomSnapperView); it != _Context->m_Hierarchy.end(docker->BottomSnapperView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                windows.push_back(*it);
        }
    }

    if(_Anchors & ImmediateUserInterfaceDockingAnchor_::ImmediateUserInterfaceDockingAnchor_Center)
    {
        for(auto it = _Context->m_Hierarchy.begin(docker->DockerView); it != _Context->m_Hierarchy.end(docker->DockerView); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceWindow*>(*it) != nullptr)
                windows.push_back(*it);
        }
    }

    return windows;
}

// ImmediateUserInterfaceInputController
ImmediateUserInterfaceInputController::ImmediateUserInterfaceInputController(){}
ImmediateUserInterfaceInputController::~ImmediateUserInterfaceInputController(){}

void ImmediateUserInterfaceInputController::frame_input(ImmediateUserInterfaceContextLayer* _Context)
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

            // setup default rendering order
            if(_Context->m_Hierarchy.get_parent(node) == nullptr)
                node->set_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Main);
        }

        // unhover invisible node
        if(!node->is_partially_visible(_Context) || !node->is_enabled(_Context))
        {
            node->State.MouseHover = ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None;
            node->Cache.MouseHover = ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None;
            continue;
        }

        // find node catching events
        if(eventNode == nullptr && node->State.Events != ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
            eventNode = node;

        // hover end logic
        if(!node->is_catching_event(_Context))
        {
            if(!(node->Cache.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseLeft))
            {
                node->State.MouseLeaveTimer = Frenchie::Core::Clock::tic();
                node->State.MouseHover |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseLeft;
            }
            else if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(node->State.MouseLeaveTimer, Frenchie::Core::Clock::tic()) > 200.f) // TODO: this MUST BE A SETTING !!!!
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
        if((_Context->m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlightHoveredNodes))
        {
            int depth = ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node(hoveredNode);

            _Context->m_Renderer->push_rectangle(
                hoveredNode->get_visible_rect(_Context).Min,
                hoveredNode->get_visible_rect(_Context).Max,
                gs_color_rgba(
                    gs_color_rgba_get_r(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    gs_color_rgba_get_g(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    gs_color_rgba_get_b(_Context->m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Gizmos)),
                    128),
                _Context->m_Style.get_frames_width(),
                _Context->m_Renderer->calculate_transform_matrix((float)depth),
                _Context->m_Style.get_frames_radius());
        }

        // start hover node
        if(!(hoveredNode->Cache.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseEntered))
        {
            hoveredNode->State.MouseEnterTimer = Frenchie::Core::Clock::tic();
            hoveredNode->State.MouseHover     |= ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseEntered;
        }
        else if(Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Milliseconds>(hoveredNode->State.MouseEnterTimer, Frenchie::Core::Clock::tic()) > 10.0) // TODO: this MUST BE A SETTING !!!
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

            if(gs_2d_boxf(
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

        // pass focus on event
        if(eventCatcher->State.Events != ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None)
        {
            // setup default rendering order for all singletone nodes
            for(auto singletone : _Context->m_Hierarchy.Singletons)
                singletone->set_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Main);

            // pass focus to event catcher node
            eventCatcher->set_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Focus);
        }
        // pass focus on mouse press
        else if(_Context->m_Input.is_mouse_button_pressed())
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

            focused->set_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Focus);
        }

        IsCatchingEvent =
            eventCatcher != nullptr &&
            (_Context->m_Input.is_mouse_button_pressed() ||
             _Context->m_Input.is_mouse_button_down()    ||
             _Context->m_Input.is_key_pressed()          ||
             _Context->m_Input.is_key_down());
    }
}

// ImmediateUserInterfaceLayoutController
ImmediateUserInterfaceLayoutController::ImmediateUserInterfaceLayoutController(){}
ImmediateUserInterfaceLayoutController::~ImmediateUserInterfaceLayoutController(){}

void ImmediateUserInterfaceLayoutController::frame_start(ImmediateUserInterfaceContextLayer* _Context)
{
    for (auto node : _Context->m_NodesRenderingList)
    {
        if(node != nullptr)
            node->next_rendering_order();
    }
}

void ImmediateUserInterfaceLayoutController::frame_input(ImmediateUserInterfaceContextLayer* _Context)
{
    for (auto& singleton : _Context->m_Hierarchy.Singletons)
        node_measure(_Context, singleton);
}

void ImmediateUserInterfaceLayoutController::frame_render(ImmediateUserInterfaceContextLayer* _Context)
{
    // get ready
    m_NodesRenderingCache.clear();

    std::stable_sort(
        _Context->m_Hierarchy.Singletons.begin(),
        _Context->m_Hierarchy.Singletons.end(),
        [](const ImmediateUserInterfaceNode* _A, const ImmediateUserInterfaceNode* _B)
        {
            return _A->get_rendering_order() < _B->get_rendering_order();
        }
    );

    // render nodes
    for (auto& singleton : _Context->m_Hierarchy.Singletons)
    {
        for (auto& renderedNode : m_NodesRenderingCache)
        {
            singleton->State.Depth = gs_max(
                singleton->State.Depth,
                renderedNode->State.MaximumChildDepth + renderedNode->State.SelfThickness + 1,
                renderedNode->Cache.MaximumChildDepth + renderedNode->Cache.MaximumChildThickness + renderedNode->Cache.SelfThickness + 1);
        }

        ImmediateUserInterfaceLayoutController::render_node(_Context, singleton);
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

void ImmediateUserInterfaceLayoutController::node_measure(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
{
    if(_Context == nullptr || _Node == nullptr || !_Node->is_enabled(_Context))
        return;

    _Node->measure(_Context);

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
        node_measure(_Context, (*it));   
}

void ImmediateUserInterfaceLayoutController::render_node(ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode* _Node)
{
    if(_Node == nullptr || !_Node->is_partially_visible(_Context) || !_Node->is_enabled(_Context)) return;

    // push style
    std::optional<ImmediateUserInterfaceStyle> backup =
        _Node->NextStyle.has_value() ?
            _Context->m_Style :
                std::optional<ImmediateUserInterfaceStyle>();

    if(_Node->NextStyle.has_value())
        _Context->m_Style = _Node->NextStyle.value();

    // layout self
    _Node->layout(_Context);

    // calculate self depth attributes
    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); ++it)
    {
        (*it)->State.Depth =
            !_Node->State.PlaceInFollow ?
                _Node->State.Depth + _Node->State.SelfThickness + 1 :
                    gs_max(_Node->State.MaximumChildDepth + _Node->State.MaximumChildThickness + _Node->State.SelfThickness, _Node->State.Depth + _Node->State.SelfThickness) + 1;

        render_node(_Context, (*it));

        _Node->State.MaximumChildDepth     = gs_max(_Node->State.MaximumChildDepth, (*it)->State.Depth);
        _Node->State.MaximumChildThickness = gs_max(_Node->State.MaximumChildThickness, (*it)->State.SelfThickness);
    }

    // update parent maximum child depth and maximum child thickness
    ImmediateUserInterfaceNode* parent = _Context->m_Hierarchy.get_parent(_Node);

    while (parent)
    {
        parent->State.MaximumChildDepth     = gs_max(parent->State.MaximumChildDepth, _Node->State.MaximumChildDepth);
        parent->State.MaximumChildThickness = gs_max(parent->State.MaximumChildThickness, _Node->State.MaximumChildThickness);
        parent                              = _Context->m_Hierarchy.get_parent(parent);
    }

    // pop style
    if(backup.has_value())
        _Context->m_Style = backup.value();
}

ImmediateUserInterfaceMenusAndPopupsController::ImmediateUserInterfaceMenusAndPopupsController(){}
ImmediateUserInterfaceMenusAndPopupsController::~ImmediateUserInterfaceMenusAndPopupsController(){}

void ImmediateUserInterfaceMenusAndPopupsController::frame_finish(ImmediateUserInterfaceContextLayer* _Context)
{
    ActiveMenus.clear();

    for(auto node : _Context->m_NodesRenderingList)
    {
        // layout popups
        ImmediateUserInterfacePopupScrollArea* popup =
            dynamic_cast<ImmediateUserInterfacePopupScrollArea*>(node);

        if(popup != nullptr)
        {            
            float internal = 0.f;
            detect_maximum_width(_Context, popup, internal);
            setup_maximum_with(_Context, popup, internal);
        }

        // layout menus
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

void ImmediateUserInterfaceMenusAndPopupsController::detect_maximum_width(
    ImmediateUserInterfaceContextLayer* _Context,
    ImmediateUserInterfaceNode*         _Node,
    float&                              _MaximumWidth)
{
    if(_Context == nullptr) return;

    ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Node);

    if(scrollArea != nullptr)
    {
        for(auto it = _Context->m_Hierarchy.begin(scrollArea); it != _Context->m_Hierarchy.end(scrollArea); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceMenuAction*>(*it) != nullptr)
            {
                _MaximumWidth = gs_max(
                    _MaximumWidth,
                    (_Context->m_Renderer->calculate_bounding_box(
                        (*it)->Name.begin(),
                        (*it)->Name.end(),
                        _Context->m_Style.get_font_size(), _Context->m_Style.get_current_font()).size() + gs_vec2f(_Context->m_Style.get_font_size(), 0.f)).x);
            }
        }
    }

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
        detect_maximum_width(_Context, *it, _MaximumWidth);
}

void ImmediateUserInterfaceMenusAndPopupsController::setup_maximum_with(
    ImmediateUserInterfaceContextLayer* _Context,
    ImmediateUserInterfaceNode*         _Node,
    float&                              _MaximumWidth)
{
    if(_Context == nullptr) return;

    ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Node);

    if(scrollArea != nullptr)
    {
        for(auto it = _Context->m_Hierarchy.begin(scrollArea); it != _Context->m_Hierarchy.end(scrollArea); it++)
        {
            if(dynamic_cast<ImmediateUserInterfaceMenuAction*>(*it) != nullptr)
            {
                (*it)->State.BoundingBox =
                    gs_2d_boxf(
                        (*it)->State.BoundingBox.Min,
                        (*it)->State.BoundingBox.Min + gs_vec2f(_MaximumWidth, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(_Context)));
            }
        }
    }

    for(auto it = _Context->m_Hierarchy.begin(_Node); it != _Context->m_Hierarchy.end(_Node); it++)
        setup_maximum_with(_Context, *it, _MaximumWidth);
}

// ImmediateUserInterfaceNextNodeController
ImmediateUserInterfaceNextNodeController::ImmediateUserInterfaceNextNodeController(){}
ImmediateUserInterfaceNextNodeController::~ImmediateUserInterfaceNextNodeController(){}

void ImmediateUserInterfaceNextNodeController::frame_start(ImmediateUserInterfaceContextLayer*)
{
    // reset all
    reset();
}

void ImmediateUserInterfaceNextNodeController::reset()
{
    // reset all
    NextLine.reset();
    NextIndent.reset();
    NextPosition.reset();

    NextMinimumWidth.reset();
    NextMinimumHeight.reset();

    NextMaximumWidth.reset();
    NextMaximumHeight.reset();

    NextContentMargin.reset();
    NextContentPadding.reset();
    NextScrollOffset.reset();
    NextOrderInFollow.reset();

    NextRenderingOrder.reset();

    NextAxisScale.reset();
    NextAxisOffset.reset();

    NextStyle.reset();
}

// ImmediateUserInterfaceScrollBarsController
ImmediateUserInterfaceScrollBarsController::ImmediateUserInterfaceScrollBarsController(){}
ImmediateUserInterfaceScrollBarsController::~ImmediateUserInterfaceScrollBarsController(){}
void ImmediateUserInterfaceScrollBarsController::frame_input(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    if(Locked)
    {
        Locked = false;
        return;
    }

    ImmediateUserInterfaceNode* hoveredNode =
        ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceHoveredNodeSearcher().search(
            _Context,
            [](const ImmediateUserInterfaceNode* _Node)->bool{return dynamic_cast<const ImmediateUserInterfaceScrollArea*>(_Node);});

    ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(hoveredNode);
    
    if(scrollArea == nullptr ||
        !scrollArea->State.BoundingBox.contains(_Context->m_Input.get_cusor_position())) return;

    // adjust vertical scroll bar by mouse wheel
    if((scrollArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment))
    {
        if(gs_vector_length(_Context->m_Input.get_mouse_wheel_scroll_offset()) > 0.f)
        {
            scrollArea->set_vertical_scroll_offset(
                _Context->m_Input.get_mouse_wheel_scroll_offset() * (-1.f) * gs_min(scrollArea->State.ContentSize.y, scrollArea->State.BoundingBox.size().y) * 0.05f);
        }
    }

    // adjust vertical scrollbar by arrow keys
    if((scrollArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment))
    {
        if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow) ||
            _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow))
        {
            gs_vec2f offset =
                !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow) ?
                    (-1.f) * gs_min(scrollArea->State.ContentSize.y, scrollArea->State.BoundingBox.size().y) * 0.05f :
                        (-4.f);

            scrollArea->set_vertical_scroll_offset(offset);
        }
        else if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow) ||
                _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow))
        {
            gs_vec2f offset =
                !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow) ?
                    (+1.f) * gs_min(scrollArea->State.ContentSize.y, scrollArea->State.BoundingBox.size().y) * 0.05f :
                        (+4.f);

            scrollArea->set_vertical_scroll_offset(offset);
        }
    }

    // adjust horizontal scrollbar by arrow keys
    if((scrollArea->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment))
    {
        if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow) ||
            _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow))
        {
            gs_vec2f offset =
                !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow) ?
                    (-1.f) * gs_min(scrollArea->State.ContentSize.x, scrollArea->State.BoundingBox.size().x ) * 0.05f :
                        (-4.f);

            scrollArea->set_horizontal_scroll_offset(offset);
        }
        else if(_Context->m_Input.is_key_clicked(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ||
                _Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow))
        {
            gs_vec2f offset =
                !_Context->m_Input.is_key_hold(ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow) ?
                    (+1.f) * gs_min(scrollArea->State.ContentSize.x, scrollArea->State.BoundingBox.size().x) * 0.05f :
                        (+4.f);

            scrollArea->set_horizontal_scroll_offset(offset);
        }
    }
}

// ImmediateUserInterfacePlotsController
ImmediateUserInterfacePlotsController::ImmediateUserInterfacePlotsController(){}
ImmediateUserInterfacePlotsController::~ImmediateUserInterfacePlotsController(){}
void ImmediateUserInterfacePlotsController::frame_input(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context == nullptr)
        return;

    // release all axis
    if(_Context->m_Input.is_mouse_button_released())
    {
        for(auto renderedNode : _Context->m_NodesRenderingList)
        {
            ImmediateUserInterfacePlotAxis* axis =
                dynamic_cast<ImmediateUserInterfacePlotAxis*>(renderedNode);

            if(axis != nullptr)
                axis->events(_Context);
        }

        return;
    }

    // process plots
    ImmediateUserInterfaceNode* plots =
        ImmediateUserInterfaceContextLayerHelpers::ImmediateUserInterfaceHoveredNodeSearcher().search(
            _Context,
            [](const ImmediateUserInterfaceNode* _Node)->bool
        {
            return dynamic_cast<const ImmediateUserInterfacePlot*>(_Node) ||
                    dynamic_cast<const ImmediateUserInterfacePlotView*>(_Node);
        });

    if(plots == nullptr && LastFramePlot != nullptr)
    {
        for(auto renderedNode : _Context->m_NodesRenderingList)
        {
            ImmediateUserInterfacePlotAxis* axis =
                dynamic_cast<ImmediateUserInterfacePlotAxis*>(renderedNode);

            if(axis != nullptr)
                axis->Edited = false;
        }

        LastFramePlot = plots;

        return;
    }

    LastFramePlot = plots;

    ImmediateUserInterfacePlotWidget* widget =
        _Context->m_Hierarchy.get_parent<ImmediateUserInterfacePlotWidget>(plots);

    if(widget == nullptr)
        return;

    // catch events
    if(widget->XAxisView != nullptr)
    {
        for(auto it = _Context->m_Hierarchy.begin(widget->XAxisView); it != _Context->m_Hierarchy.end(widget->XAxisView); it++)
        {
            ImmediateUserInterfacePlotAxis* axis =
                dynamic_cast<ImmediateUserInterfacePlotAxis*>(*it);

            if(axis == nullptr)
                continue;

            // scroll
            if(_Context->m_Input.is_mouse_button_down(ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonMiddle))
                axis->events(_Context);

            // zoom
            else if(
                gs_vector_length(_Context->m_Input.get_mouse_wheel_scroll_offset()) > 0.f &&
                (axis->Settings & ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Zoomable))
            {
                axis->ZoomScale = gs_clamp(
                    _Context->m_Input.get_mouse_wheel_scroll_offset().y > 0.f ? axis->ZoomScale * 0.5f : axis->ZoomScale * 1.5f,
                        axis->MinZoomScale,
                            axis->MaxZoomScale);
            }
        }
    }

    if(widget->YAxisView != nullptr)
    {
        for(auto it = _Context->m_Hierarchy.begin(widget->YAxisView); it != _Context->m_Hierarchy.end(widget->YAxisView); it++)
        {
            ImmediateUserInterfacePlotAxis* axis =
                dynamic_cast<ImmediateUserInterfacePlotAxis*>(*it);

            if(axis == nullptr)
                continue;

            // scroll
            if(_Context->m_Input.is_mouse_button_down(ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonMiddle))
                axis->events(_Context);

            // zoom
            else if(
                gs_vector_length(_Context->m_Input.get_mouse_wheel_scroll_offset()) > 0.f &&
                (axis->Settings & ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Zoomable))
            {
                float offset = _Context->m_Input.get_mouse_wheel_scroll_offset().y;

                axis->ZoomScale = gs_clamp(
                    offset > 0.f ? axis->ZoomScale * 0.5f : axis->ZoomScale * 1.5f,
                        gs_vec2f(0.01f, 0.01f),
                            gs_vec2f(10.f, 10.f));
            }
        }
    }
}

// ImmediateUserInterfaceDragAndDropController
ImmediateUserInterfaceDragAndDropController::ImmediateUserInterfaceDragAndDropController(){}
ImmediateUserInterfaceDragAndDropController::~ImmediateUserInterfaceDragAndDropController(){}

void ImmediateUserInterfaceDragAndDropController::frame_render(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context != nullptr && m_Preview != nullptr)
    {
        m_Preview(
            m_Data,
            gs_2d_boxf(
                _Context->m_Input.get_cusor_position(),
                _Context->m_Input.get_cusor_position() + gs_vec2f(64.f, 64.f) // TODO: THIS MUST BE A SETTING
            ),
            ImmediateUserInterfaceContextLayerHelpers::calculate_layer_depth(_Context, ImmediateUserInterfaceRenderingLayer_::ImmediateUserInterfaceRenderingLayer_Gizmos)
        );
    }
}

void ImmediateUserInterfaceDragAndDropController::frame_finish(ImmediateUserInterfaceContextLayer* _Context)
{
    if(_Context != nullptr && _Context->m_Input.is_mouse_button_released())
    {
        m_Data    = std::any();
        m_Preview = nullptr;
    }
}

void ImmediateUserInterfaceDragAndDropController::push_data(const std::any& _Data, const std::function<void(const std::any&, const gs_2d_boxf&, const int&)>& _Preview)
{
    m_Data    = _Data;
    m_Preview = _Preview;
}

std::any ImmediateUserInterfaceDragAndDropController::pop_data()
{
    return m_Data;
}

ImmediateUserInterfaceCommandsQueueController::ImmediateUserInterfaceCommandsQueueController(){}
ImmediateUserInterfaceCommandsQueueController::~ImmediateUserInterfaceCommandsQueueController(){}

void ImmediateUserInterfaceCommandsQueueController::frame_start(ImmediateUserInterfaceContextLayer*)
{
    for(auto& command : m_Commands)
    {
        if(command != nullptr)
            command();
    }

    m_Commands.clear();
}

void ImmediateUserInterfaceCommandsQueueController::push(const std::function<void()>& _Command)
{
    m_Commands.push_back(_Command);
}

// ImmediateUserInterfaceVerticalClipper
ImmediateUserInterfaceVerticalClipper::ImmediateUserInterfaceVerticalClipper(const ImmediateUserInterfaceNode* _ScorllArea, const int& _ElementsCount, const float& _CellSize, const float& _Offset)
{
    if(_ScorllArea == nullptr)
        return;

    const IImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<const IImmediateUserInterfaceScrollArea*>(_ScorllArea);

    if(scrollArea == nullptr)
    {
        SourceElement = 0;
        TargetElement = _ElementsCount;
        return;
    }

    gs_vec2f scrollOffset = scrollArea->get_scroll_offset();
    gs_vec2f visibleSize  = _ScorllArea->State.BoundingBox.size();

    SourceElement = gs_min(gs_max((int)roundf((scrollOffset.y + _Offset) / _CellSize) - 1, 0), _ElementsCount - 1);
    TargetElement = gs_min(gs_min((int)roundf(((scrollOffset + visibleSize).y  + _Offset) / _CellSize) + 1, _ElementsCount), _ElementsCount);
}

// ImmediateUserInterfaceHorizontalClipper
ImmediateUserInterfaceHorizontalClipper::ImmediateUserInterfaceHorizontalClipper(const ImmediateUserInterfaceNode* _ScorllArea, const int& _ElementsCount, const float& _CellSize, const float& _Offset)
{
    if(_ScorllArea == nullptr)
        return;

    const IImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<const IImmediateUserInterfaceScrollArea*>(_ScorllArea);

    if(scrollArea == nullptr)
    {
        SourceElement = 0;
        TargetElement = _ElementsCount;
        return;
    }

    gs_vec2f scrollOffset = scrollArea->get_scroll_offset();
    gs_vec2f visibleSize  = _ScorllArea->State.BoundingBox.size();

    SourceElement = gs_min(gs_max((int)roundf((scrollOffset.x + _Offset) / _CellSize) - 1, 0), _ElementsCount - 1);
    TargetElement = gs_min(gs_min((int)roundf(((scrollOffset + visibleSize).x + _Offset) / _CellSize) + 1, _ElementsCount), _ElementsCount);
}

// ImmediateUserInterfaceContextLayer2
ImmediateUserInterfaceContextLayer::ImmediateUserInterfaceContextLayer() : Layer(STRINGIFY(ImmediateUserInterfaceContextLayer)){}
ImmediateUserInterfaceContextLayer::~ImmediateUserInterfaceContextLayer(){}

bool ImmediateUserInterfaceContextLayer::awake()
{
    // launch renderer
    if(m_Renderer == nullptr)
        m_Renderer = Application::push_layer<RenderingQueue2D>();

    // create hierarchy
    m_Hierarchy = ImmediateUserInterfaceHierarchy(
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
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceCommandsQueueController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceWindowsController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceInputController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceMenusAndPopupsController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceScrollBarsController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfacePlotsController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceNextNodeController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceLayoutController>());
    m_Controllers.push_back(std::make_unique<ImmediateUserInterfaceDragAndDropController>());

    // awake controllers
    for(auto& controller : m_Controllers)
        GS_ASSERT(controller->awake(this));

    // load .ini file
    m_IniFileState.read(m_IniFilePath);

    // create input handler
    m_Input = ImmediateUserInterfaceInput(this);

    return m_Renderer != nullptr;
}

void ImmediateUserInterfaceContextLayer::frame_start()
{
    // execute controllers
    for(auto& controller : m_Controllers)
        controller->frame_start(this);

    // push clear color
    m_Renderer->push_clear_color(
        m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking ?
            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ChildBackground) :
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ParentBackground));

    // check rendering stack
    GS_ASSERT(m_NodesRenderingStack.empty());
    GS_ASSERT(m_StyleBackups.empty());

    // clean-up rendering data
    m_NodesRenderingList.clear();
    m_NodesRenderingStack.clear();
    m_NodesRenderedStack.clear();
}

void ImmediateUserInterfaceContextLayer::frame_update()
{
    // execute controllers
    for(auto& controller : m_Controllers)
        controller->frame_update(this);
}

void ImmediateUserInterfaceContextLayer::frame_input()
{
    // build hierarchy
    m_Hierarchy.build(m_NodesRenderingList);

    // execute controllers
    for(auto& controller : m_Controllers)
        controller->frame_input(this);
}

void ImmediateUserInterfaceContextLayer::frame_render()
{
    for(auto& controller : m_Controllers)
        controller->frame_render(this);
}

void ImmediateUserInterfaceContextLayer::frame_finish()
{
    // save state
    if(Application::is_closed())
        save_state_ini_file();
    else
        load_state_ini_file();

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

        node->NextStyle.reset();

        node->State.Depth                 = 0;
        node->State.SelfThickness         = 0;
        node->State.RenderingIndex        = 0;
        node->State.MaximumChildDepth     = 0;
        node->State.MaximumChildThickness = 0;
        node->State.Settings              = 0;
        node->Count                       = 0;

        node->restore();
    }

    // clear ini file state
    m_IniFileState.clear();
}

void ImmediateUserInterfaceContextLayer::finish()
{
    // close renderer
    if(m_Renderer != nullptr)
        m_Renderer->close();
}

bool ImmediateUserInterfaceContextLayer::allows_multiple_instances() const
{
    return false;
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
    return begin_node<ImmediateUserInterfacePanel>(_ID, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_panel()
{
    end_node<ImmediateUserInterfacePanel>();
}

bool ImmediateUserInterfaceContextLayer::begin_vertical_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceVerticalStack>(_Name, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_vertical_stack()
{
    end_node<ImmediateUserInterfaceVerticalStack>();
}

bool ImmediateUserInterfaceContextLayer::begin_horizontal_stack(const std::string& _Name, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceHorizontalStack>(_Name, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_horizontal_stack()
{
    end_node<ImmediateUserInterfaceHorizontalStack>();
}

bool ImmediateUserInterfaceContextLayer::begin_grid(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceGrid>(_ID, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_grid()
{
    end_node<ImmediateUserInterfaceGrid>();
}

bool ImmediateUserInterfaceContextLayer::begin_grid_place(const int& _Row, const int& _Column, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    if(begin_node<ImmediateUserInterfaceGridPlace>(Frenchie::Core::String::format("Place-%d-%d", _Row, _Column), _Settings))
    {
        ImmediateUserInterfaceGridPlace* gridPlace =
            get_rendering_stack_top<ImmediateUserInterfaceGridPlace>();

        gridPlace->Row    = _Row;
        gridPlace->Column = _Column;

        return true;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::end_grid_place()
{
    end_node<ImmediateUserInterfaceGridPlace>();
}

void ImmediateUserInterfaceContextLayer::empty_node(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, const gs_color& _Color)
{
    if(begin_node<ImmediateUserInterfaceNode>(_ID, _Settings))
    {
        ImmediateUserInterfaceNode* widget = get_rendering_stack_top<ImmediateUserInterfaceNode>();

        // render
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            int depth = widget->Cache.Depth;
            int init  = depth;

            m_Renderer->push_rectangle_filled(
                widget->State.BoundingBox.Min - m_Style.get_frames_width(),
                widget->State.BoundingBox.Max + m_Style.get_frames_width(),
                _Color,
                m_Renderer->calculate_transform_matrix((float)depth++),
                m_Style.get_frames_radius());


            widget->State.SelfThickness = depth - init;

            m_Renderer->pop_clip_box();
        }
        
        end_node<ImmediateUserInterfaceNode>();
    }
}

bool ImmediateUserInterfaceContextLayer::push_button(const std::string& _ID)
{
    struct ImmediateUserInterfacePushButton : public ImmediateUserInterfaceNode
    {
    public:
        ImmediateUserInterfacePushButton(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
        {
            State.BoundingBox = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(128.f, 64.f));
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
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            int depth = widget->Cache.Depth;
            int init  = depth;

            // background
            m_Renderer->push_rectangle_filled(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Max,
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                m_Renderer->calculate_transform_matrix((float)depth++),
                m_Style.get_frames_radius());

            if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down())
            {
                m_Renderer->push_rectangle_filled(
                    widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_frames_radius());
            }
            else
            {
                m_Renderer->push_rectangle_filled(
                    widget->State.BoundingBox.Min + m_Style.get_frames_width(),
                    widget->State.BoundingBox.Max - m_Style.get_frames_width(),
                    (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_frames_radius());
            }

            // title
            m_Renderer->push_text(
                widget->State.BoundingBox.center() - textSize * 0.5f, // text is aligned on center of the push button
                widget->Name.begin(),
                widget->Name.end(),
                m_Style.get_font_size(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                m_Renderer->calculate_transform_matrix((float)depth++),
                m_Style.get_current_font());
        
            widget->State.SelfThickness = depth - init;

            m_Renderer->pop_clip_box();
        }

        // calculate geometry
        {
            widget->State.MinimumSize = gs_vec2f(
                textSize.x + ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this),
                gs_max(textSize.y, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this)));

            widget->State.MaximumSize = gs_vec2f(
                gs_max(widget->State.MinimumSize.x, widget->State.MaximumSize.x),
                gs_max(widget->State.MinimumSize.y, widget->State.MaximumSize.y));

            widget->State.BoundingBox = gs_2d_boxf(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Min + gs_clamp(widget->State.BoundingBox.size(), widget->State.MinimumSize, widget->State.MaximumSize));
        }

        bool clicked = is_current_node_mouse_clicked();

        end_node<ImmediateUserInterfacePushButton>();

        return clicked;
    }

    return false;
}

bool ImmediateUserInterfaceContextLayer::image_button(
    const std::string&                        _ID,
    const gs_color&                           _Color,
    const ApplicationRenderingBackendTexture& _Texture)
{
    if(begin_node<ImmediateUserInterfaceNode>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceNode* widget = get_rendering_stack_top();

        int depth = widget->Cache.Depth;
        int init  = depth;

        // render
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            m_Renderer->push_rectangle_filled(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Max,
                (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down() ?
                    gs_color_rgb(gs_color_rgba_get_r(_Color) / 2, gs_color_rgba_get_g(_Color) / 2, gs_color_rgba_get_b(_Color) / 2) :
                    _Color,
                m_Renderer->calculate_transform_matrix((float)depth++),
                0.f,
                _Texture);

            widget->State.SelfThickness = depth - init;

            m_Renderer->pop_clip_box();
        }

        bool clicked = is_current_node_mouse_clicked();

        end_node<ImmediateUserInterfaceNode>();

        return clicked;
    }

    return false;
}

bool ImmediateUserInterfaceContextLayer::begin_canvas(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    const ImmediateUserInterfaceCheckButtonSettings settings =
        _Settings & (~ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable);

    if(begin_node<ImmediateUserInterfaceCanvas>(_ID, settings))
    {
        ImmediateUserInterfaceCanvas* widget = get_rendering_stack_top<ImmediateUserInterfaceCanvas>();
        m_Renderer->push_clip_box(widget->get_clipping_box(this));
        widget->InitialDepth = widget->Cache.Depth;
        widget->CurrentDepth = widget->Cache.Depth;

        return true;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::end_canvas()
{
    m_Renderer->pop_clip_box();

    ImmediateUserInterfaceCanvas* widget =
        get_rendering_stack_top<ImmediateUserInterfaceCanvas>();

    if(widget != nullptr)
        widget->State.SelfThickness = widget->CurrentDepth - widget->InitialDepth;

    end_node<ImmediateUserInterfaceCanvas>();
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
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            gs_2d_boxf boundingBox = gs_2d_boxf(widget->State.BoundingBox.Min - m_Style.get_frames_width(), widget->State.BoundingBox.Max + m_Style.get_frames_width());
            int       depth       = widget->Cache.Depth;
            int       init        = depth;

            // render checkbox
            if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox)
            {
                // background
                m_Renderer->push_rectangle_filled(
                    boundingBox.Min,
                    boundingBox.Max,
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_frames_radius());

                if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down())
                {
                    m_Renderer->push_rectangle_filled(
                        boundingBox.Min + m_Style.get_frames_width(),
                        boundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_frames_radius());
                }
                else
                {
                    m_Renderer->push_rectangle_filled(
                        boundingBox.Min + m_Style.get_frames_width(),
                        boundingBox.Max - m_Style.get_frames_width(),
                        (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_frames_radius());
                }

                // tick
                if(_Checked)
                {
                    gs_vec2f start = gs_vec2f(
                        boundingBox.center().x,
                        boundingBox.center().y + boundingBox.height() * 0.5f * 0.5f);

                    m_Renderer->push_line(
                        start,
                        gs_vec2f(
                            boundingBox.center().x - boundingBox.width() * 0.5f * 0.7f,
                            boundingBox.center().y - boundingBox.height() * 0.5f * 0.25f),
                        m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));

                    m_Renderer->push_line(
                        start,
                        gs_vec2f(
                            boundingBox.center().x + boundingBox.width() * 0.5f * 0.7f,
                            boundingBox.center().y - boundingBox.height() * 0.5f * 0.9f),
                        m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }
            }
            
            // render radio button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton)
            {
                // background
                m_Renderer->push_rectangle_filled(
                    boundingBox.Min,
                    boundingBox.Max,
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_frames_radius());

                if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) && m_Input.is_mouse_button_down())
                {
                    m_Renderer->push_rectangle_filled(
                        boundingBox.Min + m_Style.get_frames_width(),
                        boundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_frames_radius());
                }
                else
                {
                    m_Renderer->push_rectangle_filled(
                        boundingBox.Min + m_Style.get_frames_width(),
                        boundingBox.Max - m_Style.get_frames_width(),
                        (widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ?
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_frames_radius());
                }

                if(_Checked)
                {
                    m_Renderer->push_rectangle_filled(
                        boundingBox.Min + m_Style.get_frames_width(),
                        boundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_frames_radius());
                }
            }
            
            // render slider button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton)
            {
                // background
                m_Renderer->push_rectangle_filled(
                    boundingBox.Min,
                    boundingBox.Max,
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_frames_radius());

                if(_Checked)
                {
                    m_Renderer->push_rectangle_filled(
                        boundingBox.Min,
                        boundingBox.Max,
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_frames_radius());

                    m_Renderer->push_rectangle_filled(
                        boundingBox.Min + gs_vec2f(boundingBox.width() * 0.5f, m_Style.get_frames_width()),
                        boundingBox.Max - m_Style.get_frames_width(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_frames_radius());
                }
                else
                {
                    m_Renderer->push_rectangle_filled(
                        boundingBox.Min + m_Style.get_frames_width(),
                        boundingBox.Min + gs_vec2f(boundingBox.width() * 0.5f, boundingBox.height() - m_Style.get_frames_width()),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_frames_radius());
                }
            }

            widget->State.SelfThickness = depth - init;
        
            m_Renderer->pop_clip_box();
        }

        // layout geometry
        {
            // layout checkbox
            if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox)
            {
                widget->State.MinimumSize = ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this);
                widget->State.MaximumSize = widget->State.MinimumSize;

                widget->State.BoundingBox = gs_2d_boxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + widget->State.MaximumSize);
            }
            // layout radio button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton)
            {
                widget->State.MinimumSize = ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this);
                widget->State.MaximumSize = widget->State.MinimumSize;

                widget->State.BoundingBox = gs_2d_boxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + widget->State.MaximumSize);
            }
            // layout slider button
            else if(_Settings & ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton)
            {
                widget->State.MinimumSize = gs_vec2f(
                    ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this) * 2.f,
                    ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this));
                widget->State.MaximumSize = widget->State.MinimumSize;

                widget->State.BoundingBox = gs_2d_boxf(
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

        bool clicked = is_current_node_mouse_clicked();

        end_node<ImmediateUserInterfaceMenuAction>();

        return clicked;
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

        bool clicked = is_current_node_mouse_clicked();

        end_node<ImmediateUserInterfaceComboboxItem>();

        return clicked;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::label(
    const std::string&                         _ID,
    const std::string&                         _Text,
    const ImmediateUserInterfaceLabelSettings& _Settings,
    const int&                                 _MaxSymbolsCount)
{
    // auxiliary types
    struct ImmediateUserInterfaceLabel : public ImmediateUserInterfaceNode
    {
    public:
        ImmediateUserInterfaceLabel(const std::string& _Name) : ImmediateUserInterfaceNode(_Name)
        {
            State.BoundingBox = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
        }

        virtual ~ImmediateUserInterfaceLabel(){}
    };

    if(begin_node<ImmediateUserInterfaceLabel>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        // setup
        ImmediateUserInterfaceLabel* widget   = get_rendering_stack_top<ImmediateUserInterfaceLabel>();
        gs_vec2f                     textSize = m_Renderer->calculate_bounding_box(_Text.begin(), _Text.end(), m_Style.get_font_size(), m_Style.get_current_font()).size();

        // render
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));

            int depth = widget->Cache.Depth;
            int init  = depth;

            float x = widget->State.BoundingBox.Min.x;

            if(_Settings & ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignLeft)
                x = widget->State.BoundingBox.Min.x;
            else if(_Settings & ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignRight)
                x = widget->State.BoundingBox.Max.x - textSize.x;
            else if(_Settings & ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter)
                x = widget->State.BoundingBox.center().x - textSize.x * 0.5f;

            if((int)_Text.size() < _MaxSymbolsCount)
            {
                m_Renderer->push_text(
                    gs_vec2f(
                        x,
                        widget->State.BoundingBox.center().y - textSize.y * 0.5f),
                    _Text.begin(),
                    _Text.end(),
                    m_Style.get_font_size(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_current_font());
            }
            else
            {
                m_Renderer->push_text_wrapped(
                    gs_vec2f(
                        x,
                        widget->State.BoundingBox.center().y - textSize.y * 0.5f),
                    _Text.begin(),
                    _Text.end(),
                    _MaxSymbolsCount,
                    m_Style.get_font_size(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_current_font());  
            }

            widget->State.SelfThickness = depth - init;

            m_Renderer->pop_clip_box();
        }

        // calculate geometry
        {
            widget->State.MinimumSize = gs_vec2f(gs_max(textSize.x, widget->State.MinimumSize.x), ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this));
            widget->State.MaximumSize = gs_vec2f(
                gs_max(widget->State.MaximumSize.x, widget->State.MinimumSize.x),
                gs_max(textSize.y, ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this)));

            widget->State.BoundingBox = gs_2d_boxf(
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
        
        // internal settings
        ImmediateUserInterfaceInputStringInternalSettings_::ImmediateUserInterfaceInputStringInternalSettings_NoMultiline,
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

template<> void ImmediateUserInterfaceContextLayer::progressbar_default<float>(const std::string& _ID, float& _Input, const float& _Min, const float& _Max)
{
    progress_bar_default_internal<float>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_default<double>(const std::string& _ID, double& _Input, const double& _Min, const double& _Max)
{
    progress_bar_default_internal<double>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_default<long double>(const std::string& _ID, long double& _Input, const long double& _Min, const long double& _Max)
{
    progress_bar_default_internal<long double>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_default<int>(const std::string& _ID, int& _Input, const int& _Min, const int& _Max)
{
    progress_bar_default_internal<int>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_default<short>(const std::string& _ID, short& _Input, const short& _Min, const short& _Max)
{
    progress_bar_default_internal<short>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_default<unsigned short>(const std::string& _ID, unsigned short& _Input, const unsigned short& _Min, const unsigned short& _Max)
{
    progress_bar_default_internal<unsigned short>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_default<unsigned int>(const std::string& _ID, unsigned int& _Input, const unsigned int& _Min, const unsigned int& _Max)
{
    progress_bar_default_internal<unsigned int>(this, _ID, _Input, _Min, _Max);
}

// progress_bar_circular
template<> void ImmediateUserInterfaceContextLayer::progressbar_circular<float>(const std::string& _ID, float& _Input, const float& _Min, const float& _Max)
{
    progress_bar_circular_internal<float>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_circular<double>(const std::string& _ID, double& _Input, const double& _Min, const double& _Max)
{
    progress_bar_circular_internal<double>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_circular<long double>(const std::string& _ID, long double& _Input, const long double& _Min, const long double& _Max)
{
    progress_bar_circular_internal<long double>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_circular<int>(const std::string& _ID, int& _Input, const int& _Min, const int& _Max)
{
    progress_bar_circular_internal<int>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_circular<short>(const std::string& _ID, short& _Input, const short& _Min, const short& _Max)
{
    progress_bar_circular_internal<short>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_circular<unsigned short>(const std::string& _ID, unsigned short& _Input, const unsigned short& _Min, const unsigned short& _Max)
{
    progress_bar_circular_internal<unsigned short>(this, _ID, _Input, _Min, _Max);
}

template<> void ImmediateUserInterfaceContextLayer::progressbar_circular<unsigned int>(const std::string& _ID, unsigned int& _Input, const unsigned int& _Min, const unsigned int& _Max)
{
    progress_bar_circular_internal<unsigned int>(this, _ID, _Input, _Min, _Max);
}

bool ImmediateUserInterfaceContextLayer::input_color(const std::string& _ID, gs_color& _Color, const ImmediateUserInterfaceColorPickerSettings& _Settings)
{
    struct ImmediateUserInterfaceInputColor : public ImmediateUserInterfaceHorizontalStack
    {
    public:
        ImmediateUserInterfaceInputColor(const std::string& _Hash) : ImmediateUserInterfaceHorizontalStack(_Hash){}
        virtual ~ImmediateUserInterfaceInputColor(){}

        gs_vec3ui RGB   = {255, 255, 255};
        gs_vec3ui HSV   = {255, 255, 255};
        gs_vec3ui HSL   = {255, 255, 255};
        gs_color  Alpha = 255;
    };

    ImmediateUserInterfaceInputScalarSettings settings =
        ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEdit |
        ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter;

    bool colorButtonClicked = false;

    if(begin_node<ImmediateUserInterfaceInputColor>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceInputColor* picker =
            get_rendering_stack_top<ImmediateUserInterfaceInputColor>();

        int elementsCount =
            (int)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB) +
            (int)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV) +
            (int)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL) +
            (int)(bool)(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha);

        float lineHeight = ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this);
        float height     = ((float)elementsCount + (float)(bool)(elementsCount > 1)) * lineHeight;

        picker->State.MinimumSize = gs_vec2f((float)picker->State.MinimumSize.x, height);
        picker->State.MaximumSize = gs_vec2f((float)picker->State.MaximumSize.x, height);

        gs_vec2f parentSize = get_rendering_stack_top()->State.BoundingBox.size();
        float    weight     =
            (_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton) ||
            (_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane) ? 0.8f : 1.f;

        // editors
        next_size(gs_vec2f(parentSize.x * weight, parentSize.y));
        next_content_padding(gs_vec4f(m_Style.get_font_size(), 0.f, 0.f, 0.f));

        if(begin_vertical_stack(
            next_id("Editors"),
              ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
        {
            float labelWidth = m_Style.get_font_size();

            // RGB
            if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB)
            {
                next_minimum_size(gs_vec2f(labelWidth, lineHeight));
                next_maximum_size(gs_vec2f(gs_huge<float>(), lineHeight));
                next_content_padding(gs_vec4f(0.f, m_Style.get_font_size(), 0.f, 0.f));

                if(begin_horizontal_stack(next_id("RGB"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    next_size(gs_vec2f(labelWidth, lineHeight));
                    label(next_id("R"), "R");

                    if(input_scalar<gs_color>(next_id("RedValue"), picker->RGB.x, 0, 255, settings))
                        _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                    else
                        picker->RGB.x = gs_color_rgba_get_r(_Color);
                    
                    next_size(gs_vec2f(labelWidth, lineHeight));
                    label(next_id("G"), "G");

                    if(input_scalar<gs_color>(next_id("GreenValue"), picker->RGB.y, 0, 255, settings))
                        _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                    else
                        picker->RGB.y = gs_color_rgba_get_g(_Color);
                    
                    next_size(gs_vec2f(labelWidth, lineHeight));
                    label(next_id("B"), "B");

                    if(input_scalar<gs_color>(next_id("BlueValue"), picker->RGB.z, 0, 255, settings))
                        _Color = gs_color_rgba(picker->RGB.x, picker->RGB.y, picker->RGB.z, picker->Alpha);
                    else
                        picker->RGB.z = gs_color_rgba_get_b(_Color);

                    end_horizontal_stack();
                }
            }

            // HSV
            if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV)
            {
                next_minimum_size(gs_vec2f(labelWidth, lineHeight));
                next_maximum_size(gs_vec2f(gs_huge<float>(), lineHeight));
                next_content_padding(gs_vec4f(0.f, m_Style.get_font_size(), 0.f, 0.f));

                if(begin_horizontal_stack(next_id("HSV"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    bool hsvChanged = false;

                    next_size(gs_vec2f(labelWidth, lineHeight));   
                    label(next_id("H"), "H");

                    if(input_scalar<gs_color>(next_id("HueValue"), picker->HSV.x, 0, 360, settings))
                        hsvChanged = true;
                    else
                        picker->HSV.x = (gs_color)((float)gs_color_hsv_get_h(gs_color_rgb_to_hsv(_Color)) / 255.f * 360.f);
                    
                    next_size(gs_vec2f(labelWidth, lineHeight));   
                    label(next_id("S"), "S");

                    if(input_scalar<gs_color>(next_id("SaturationValue"), picker->HSV.y, 0, 100, settings))
                        hsvChanged = true;
                    else
                        picker->HSV.y = (gs_color)((float)gs_color_hsv_get_s(gs_color_rgb_to_hsv(_Color)) / 255.f * 100.f);
                    
                    next_size(gs_vec2f(labelWidth, lineHeight));   
                    label(next_id("V"), "V");

                    if(input_scalar<gs_color>(next_id("BrightnessValue"), picker->HSV.z, 0, 100, settings))
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
                next_minimum_size(gs_vec2f(labelWidth, lineHeight));
                next_maximum_size(gs_vec2f(gs_huge<float>(), lineHeight));
                next_content_padding(gs_vec4f(0.f, m_Style.get_font_size(), 0.f, 0.f));

                if(begin_horizontal_stack(next_id("HSL"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    bool hsvChanged = false;

                    next_size(gs_vec2f(labelWidth, lineHeight));   
                    label(next_id("H"), "H");

                    if(input_scalar<gs_color>(next_id("HueValue"), picker->HSL.x, 0, 360, settings))
                        hsvChanged = true;
                    else
                        picker->HSL.x = (gs_color)((float)gs_color_hsl_get_h(gs_color_rgb_to_hsl(_Color)) / 255.f * 360.f);
                    
                    next_size(gs_vec2f(labelWidth, lineHeight));   
                    label(next_id("S"), "S");

                    if(input_scalar<gs_color>(next_id("SaturationValue"), picker->HSL.y, 0, 100, settings))
                        hsvChanged = true;
                    else
                        picker->HSL.y = (gs_color)((float)gs_color_hsl_get_s(gs_color_rgb_to_hsl(_Color)) / 255.f * 100.f);
                    
                    next_size(gs_vec2f(labelWidth, lineHeight));   
                    label(next_id("L"), "L");

                    if(input_scalar<gs_color>(next_id("BrightnessValue"), picker->HSL.z, 0, 100, settings))
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
                next_minimum_size(gs_vec2f(labelWidth, lineHeight));
                next_maximum_size(gs_vec2f(gs_huge<float>(), lineHeight));
                next_content_padding(gs_vec4f(0.f, m_Style.get_font_size(), 0.f, 0.f));

                if(begin_horizontal_stack(next_id("Alpha"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
                {
                    next_size(gs_vec2f(labelWidth, lineHeight));
                    label(next_id("A"), "A");

                    if(input_scalar<gs_color>(next_id("AlphaValue"), picker->Alpha, 0, 255, settings))
                        _Color = gs_color_rgba(gs_color_rgba_get_r(_Color), gs_color_rgba_get_g(_Color), gs_color_rgba_get_b(_Color), picker->Alpha);
                    else
                        picker->Alpha = gs_color_rgba_get_a(_Color);
                    
                    end_horizontal_stack();
                }
            }

            end_vertical_stack();
        }

        // preview color
        next_size(gs_vec2f(parentSize.x * (1.f - weight), parentSize.y));

        if((_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton) ||
            (_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane))
        {
            // color button
            if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton)
            {
                colorButtonClicked = image_button(next_id("Preview"), _Color);
            }

            // drag and drop
            else if(_Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane)
            {
                image(next_id("Preview"), _Color);

                drag(
                    _Color,
                    [this](const std::any& _Data, const gs_2d_boxf& _Box, const int& _Depth)
                    {
                        m_Renderer->push_rectangle_filled(
                            _Box.Min,
                            _Box.Max,
                            gs_color_rgb(0, 0, 0),
                            m_Renderer->calculate_transform_matrix((float)_Depth));

                        m_Renderer->push_rectangle_filled(
                            _Box.Min + 8.f,
                            _Box.Max - 8.f,
                            std::any_cast<gs_color>(_Data),
                            m_Renderer->calculate_transform_matrix((float)(_Depth + 1)));
                    });
            }
        }

        end_node<ImmediateUserInterfaceInputColor>();
    }

    return colorButtonClicked;
}

void ImmediateUserInterfaceContextLayer::color_picker_rgba(const std::string& _ID, gs_color& _Color, const ImmediateUserInterfaceColorPickerSettings& _Settings)
{
    struct ImmediateUserInterfaceColorPickerRGBA : public ImmediateUserInterfacePanel
    {
    public:
        ImmediateUserInterfaceColorPickerRGBA(const std::string& _Hash) : ImmediateUserInterfacePanel(_Hash){}
        virtual ~ImmediateUserInterfaceColorPickerRGBA(){}

        virtual void layout(ImmediateUserInterfaceContextLayer* _Context) override
        {
            ImmediateUserInterfacePanel::layout(_Context);

            gs_vec2f gradientBoxSize = gs_vec2f(256.f, 256.f);
            gs_vec2f paletteBoxSize  = gs_vec2f(32.f, 256.f);
            gs_vec2f alphaBoxSize    = gs_vec2f(((Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)    ? 32.f : 0.f), 256.f);

            gs_vec2f padding   = gs_vec2f(8.f);
            gs_vec2f position  = State.BoundingBox.Min;
            gs_vec2f totalSize = gradientBoxSize + paletteBoxSize + alphaBoxSize + padding;

            // calculate gradient box
            {
                gs_vec2f size = gs_vec2f((gradientBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height());
                GradientBox   = gs_2d_boxf(position, position + size);
                position     += gs_vec2f(size.x + padding.x, 0.f);
            }

            // calculate palette box
            {
                gs_vec2f size = gs_vec2f((paletteBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height());
                PaletteBox    = gs_2d_boxf(position, position + size);
                position     += gs_vec2f(size.x + padding.x, 0.f);
            }

            // calculate alpha box
            {
                gs_vec2f size = gs_vec2f((alphaBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height());
                AlphaBox    = gs_2d_boxf(position, position + size);
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

                    gs_color colors[4] =
                    {
                        sourceColor,
                        sourceColor,
                        targetColor,
                        targetColor
                    };

                    gs_vec2f points[4] =
                    {
                        position,
                        position + gs_vec2f(size.x, 0.f),
                        position + gs_vec2f(size.x, size.y),
                        position + gs_vec2f(0.f, size.y),
                    };

                    _Context->m_Renderer->push_poly_filled(
                        points,
                        colors,
                        4,
                        _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                    position += gs_vec2f(0.f, size.y);
                }
                
                // palette slider
                gs_2d_boxf paletteSlider = gs_2d_boxf(
                    PaletteBox.Min + gs_vec2f(0.f, PaletteBoxSliderPosition) * PaletteBox.size() * 0.9f,
                    PaletteBox.Min + gs_vec2f(0.f, PaletteBoxSliderPosition) * PaletteBox.size() * 0.9f + gs_vec2f(PaletteBox.width(), PaletteBox.height() * 0.1f));

                _Context->m_Renderer->push_rectangle_filled(
                    paletteSlider.Min,
                    paletteSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());

                _Context->m_Renderer->push_rectangle_filled(
                    paletteSlider.Min + gs_vec2f(4.f),
                    paletteSlider.Max - gs_vec2f(4.f),
                    paletteSlider.contains(_Context->m_Input.get_cusor_position()) || PaletteBoxSliderIsMoving ?
                        gs_color_rgba(128, 128, 128, 255) :
                            gs_color_rgba(255, 255, 255, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());
            }

            // render color gradient box
            {
                float h = PaletteMaximumHue * PaletteBoxSliderPosition;

                gs_color c1 = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)(h * 255.f), 0, 255));
                gs_color c2 = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)(h * 255.f), 255, 255));
                gs_color c3 = gs_color_hsv_to_rgb(gs_color_hsv((gs_color)(h * 255.f), 255, 0));

                // gradient box
                gs_color colors[4] = {c1, c2, c3, c3};

                gs_vec2f points[4] =
                {
                    gs_vec2f(GradientBox.Min.x, GradientBox.Min.y),
                    gs_vec2f(GradientBox.Max.x, GradientBox.Min.y),
                    gs_vec2f(GradientBox.Max.x, GradientBox.Max.y),
                    gs_vec2f(GradientBox.Min.x, GradientBox.Max.y),
                };

                _Context->m_Renderer->push_poly_filled(
                    points,
                    colors,
                    4,
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // gradient box slider
                gs_2d_boxf gradientBoxSlider = gs_2d_boxf(
                    GradientBox.Min + GradientBoxSliderPosition * GradientBox.size() * 0.9f,
                    GradientBox.Min + GradientBoxSliderPosition * GradientBox.size() * 0.9f + GradientBox.size() * 0.1f);

                _Context->m_Renderer->push_rectangle_filled(
                    gradientBoxSlider.Min,
                    gradientBoxSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());

                _Context->m_Renderer->push_rectangle_filled(
                    gradientBoxSlider.Min + gs_vec2f(4.f),
                    gradientBoxSlider.Max - gs_vec2f(4.f),
                    gs_color_rgb(gs_color_rgba_get_r(Color), gs_color_rgba_get_g(Color), gs_color_rgba_get_b(Color)),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());
            }

            // render alpha editor
            if(Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)
            {
                // alpha box
                gs_color colors[4] =
                {
                    gs_color_rgba(255, 255, 255, 255),
                    gs_color_rgba(255, 255, 255, 255),
                    gs_color_rgba(255, 255, 255, 0),
                    gs_color_rgba(255, 255, 255, 0),
                };

                gs_vec2f points[4] =
                {
                    gs_vec2f(AlphaBox.Min.x, AlphaBox.Min.y),
                    gs_vec2f(AlphaBox.Max.x, AlphaBox.Min.y),
                    gs_vec2f(AlphaBox.Max.x, AlphaBox.Max.y),
                    gs_vec2f(AlphaBox.Min.x, AlphaBox.Max.y),
                };

                _Context->m_Renderer->push_poly_filled(
                    points,
                    colors,
                    4,
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // alpha box slider
                gs_2d_boxf aphaSlider = gs_2d_boxf(
                    AlphaBox.Min + gs_vec2f(0.f, AlphaBoxSliderPosition) * AlphaBox.size() * 0.9f,
                    AlphaBox.Min + gs_vec2f(0.f, AlphaBoxSliderPosition) * AlphaBox.size() * 0.9f + gs_vec2f(AlphaBox.width(), AlphaBox.height() * 0.1f));

                _Context->m_Renderer->push_rectangle_filled(
                    aphaSlider.Min,
                    aphaSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());

                _Context->m_Renderer->push_rectangle_filled(
                    aphaSlider.Min + gs_vec2f(4.f),
                    aphaSlider.Max - gs_vec2f(4.f),
                    aphaSlider.contains(_Context->m_Input.get_cusor_position()) || AlphaBoxSliderIsMoving ?
                        gs_color_rgba(128, 128, 128, 255) :
                            gs_color_rgba(255, 255, 255, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());
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
            if(gs_abs(s - (float)(gs_color_hsv_get_s(gs_color_rgb_to_hsv(Color)) / 255.f)) > gs_tiny<float>() * 2.f ||
               gs_abs(v - (float)(gs_color_hsv_get_v(gs_color_rgb_to_hsv(Color)) / 255.f)) > gs_tiny<float>() * 2.f)
            {
                PaletteBoxSliderPosition         = gs_clamp(h / PaletteMaximumHue, 0.f, 1.f);
                PaletteBoxSliderPreviousPosition = PaletteBoxSliderPosition;
            }

            // setup grdient slider position
            GradientBoxSliderPosition         = gs_clamp(gs_vec2f(s, 1.f - v), gs_vec2f(0.f, 0.f), gs_vec2f(1.f, 1.f));
            GradientBoxSliderPreviousPosition = GradientBoxSliderPosition;

            // setup alpha slider position
            AlphaBoxSliderPosition            = gs_clamp(1.f - a, 0.f, 1.f);
            AlphaBoxSliderPreviousPosition    = AlphaBoxSliderPosition;
        }
        
        // slider attributes
        gs_color                                  Color    = gs_color_rgb(255, 255, 255);
        gs_vec3ui                                 RGB      = {0, 0, 0};
        gs_vec3ui                                 HSV      = {0, 0, 0};
        gs_vec3ui                                 HSL      = {0, 0, 0};
        gs_color                                  Alpha    = 255;
        bool                                      Edited   = false;
        ImmediateUserInterfaceColorPickerSettings Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults;

    private:

        // gradient box
        gs_2d_boxf GradientBox                       = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
        gs_vec2f  GradientBoxSliderPosition         = gs_vec2f(0.f, 0.f);
        gs_vec2f  GradientBoxSliderPreviousPosition = gs_vec2f(0.f, 0.f);
        bool      GradientBoxSliderIsMoving         = false;

        // palette box
        float     PaletteMaximumHue                 = 1.00f;
        float     PaletteHueStep                    = 0.05f;
        
        gs_2d_boxf PaletteBox                        = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
        float     PaletteBoxSliderPosition          = 0.f;
        float     PaletteBoxSliderPreviousPosition  = 0.f;
        bool      PaletteBoxSliderIsMoving          = false;
        
        // alpha box
        gs_2d_boxf AlphaBox                          = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
        float     AlphaBoxSliderPosition            = 0.f;
        float     AlphaBoxSliderPreviousPosition    = 0.f;
        bool      AlphaBoxSliderIsMoving            = false;
    };

    next_content_margin(gs_vec4f(m_Style.get_frames_width() * 2.f));
    next_content_padding(gs_vec4f(m_Style.get_frames_width() * 2.f));

    if(begin_vertical_stack(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        if(begin_node<ImmediateUserInterfaceColorPickerRGBA>(next_id("ColorPicker"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
        {
            ImmediateUserInterfaceColorPickerRGBA* picker = get_rendering_stack_top<ImmediateUserInterfaceColorPickerRGBA>();

            picker->Settings = _Settings;

            if(picker != nullptr && !picker->Edited)
                picker->force_rgba_color(_Color);
            else
                _Color = picker->Color;

            end_node<ImmediateUserInterfaceColorPickerRGBA>();
        }

        input_color(next_id("ColorEditor"), _Color, _Settings);

        end_vertical_stack();
    }
}

void ImmediateUserInterfaceContextLayer::color_picker_hsva(const std::string& _ID, gs_color& _Color, const ImmediateUserInterfaceColorPickerSettings& _Settings)
{
    struct ImmediateUserInterfaceColorPickerHSVA : public ImmediateUserInterfacePanel
    {
    public:
        ImmediateUserInterfaceColorPickerHSVA(const std::string& _Hash) : ImmediateUserInterfacePanel(_Hash){}
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
                gs_2d_boxf ellipseBox = gs_2d_boxf(position, position + gs_vec2f((ellpseBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height()));

                Ellipse = gs_2d_ellipsef(
                    ellipseBox.center(),
                    gs_min(ellipseBox.width(), ellipseBox.height()) * 0.4f);

                EllipseSlider = gs_2d_ellipsef(
                    Ellipse.Center + EllipseSliderPosition * gs_vec2f(Ellipse.MinorRadius, Ellipse.MajorRadius),
                    Ellipse.MinorRadius * 0.1f,
                    Ellipse.MajorRadius * 0.1f);

                position += gs_vec2f(ellipseBox.width() + padding.x, 0.f);
            }

            // brightness box
            {
                BrightnessBox = gs_2d_boxf(position, position + gs_vec2f((brightnessBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height()));
                position     += gs_vec2f(BrightnessBox.width() + padding.x, 0.f);
            }

            // transparency box
            {
                TransparencyBox = gs_2d_boxf(position, position + gs_vec2f((transparencyBoxSize / totalSize * State.BoundingBox.size()).x, State.BoundingBox.height()));
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
                    gs_vec2f points[3] =
                    {
                        Ellipse.Center,
                        gs_vec2f(Ellipse.Center.x + Ellipse.MinorRadius * cos(gs_to_radians(angle)), Ellipse.Center.y + Ellipse.MajorRadius * sin(gs_to_radians(angle))),
                        gs_vec2f(Ellipse.Center.x + Ellipse.MinorRadius * cos(gs_to_radians(angle + delta)), Ellipse.Center.y + Ellipse.MajorRadius * sin(gs_to_radians(angle + delta)))
                    };

                    gs_color colors[3] =
                    {
                        gs_color_rgba(255, 255, 255, 255),
                        gs_color_hsv_to_rgb(gs_color_hsv((gs_color)(angle / 360.f * 255.f), 255, brightness)),
                        gs_color_hsv_to_rgb(gs_color_hsv((gs_color)((angle + delta) / 360.f * 255.f), 255, brightness))
                    };

                    _Context->m_Renderer->build_poly_mesh_filled(points, colors, nullptr, 3);
                }

                _Context->m_Renderer->push_rendering_command(
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // slider
                _Context->m_Renderer->push_arc_filled(
                    EllipseSlider.Center,
                    EllipseSlider.MinorRadius,
                    EllipseSlider.MajorRadius,
                    0.f,
                    360.f,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                _Context->m_Renderer->push_arc_filled(
                    EllipseSlider.Center,
                    EllipseSlider.MinorRadius * 0.8f,
                    EllipseSlider.MajorRadius * 0.8f,
                    0.f,
                    360.f,
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, brightness)),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));
            }

            // render brightness box
            {
                // box
                gs_color colors[4] =
                {
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, 255)),
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, 255)),
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, 0)),
                    gs_color_hsv_to_rgb(gs_color_hsv(hue, saturation, 0)),
                };

                gs_vec2f points[4] =
                {
                    gs_vec2f(BrightnessBox.Min.x, BrightnessBox.Min.y),
                    gs_vec2f(BrightnessBox.Max.x, BrightnessBox.Min.y),
                    gs_vec2f(BrightnessBox.Max.x, BrightnessBox.Max.y),
                    gs_vec2f(BrightnessBox.Min.x, BrightnessBox.Max.y),
                };

                _Context->m_Renderer->push_poly_filled(
                    points,
                    colors,
                    4,
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // slider
                gs_2d_boxf brightnessBoxSlider = gs_2d_boxf(
                    BrightnessBox.Min + gs_vec2f(0.f, BrightnessSliderPosition * BrightnessBox.height() * 0.9f),
                    BrightnessBox.Min + gs_vec2f(0.f, BrightnessSliderPosition * BrightnessBox.height() * 0.9f) + gs_vec2f(BrightnessBox.width(), BrightnessBox.height() * 0.1f));

                _Context->m_Renderer->push_rectangle_filled(
                    brightnessBoxSlider.Min,
                    brightnessBoxSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());

                _Context->m_Renderer->push_rectangle_filled(
                    brightnessBoxSlider.Min + gs_vec2f(4.f),
                    brightnessBoxSlider.Max - gs_vec2f(4.f),
                    brightnessBoxSlider.contains(_Context->m_Input.get_cusor_position()) || BrightnessSliderIsMoving ?
                        gs_color_rgba(128, 128, 128, 255) :
                            gs_color_rgba(255, 255, 255, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());
            }

            // render transparency box
            if(Settings & ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha)
            {
                gs_color colors[4] =
                {
                    gs_color_rgba(255, 255, 255, 255),
                    gs_color_rgba(255, 255, 255, 255),
                    gs_color_rgba(255, 255, 255, 0),
                    gs_color_rgba(255, 255, 255, 0)
                };

                gs_vec2f points[4] =
                {
                    gs_vec2f(TransparencyBox.Min.x, TransparencyBox.Min.y),
                    gs_vec2f(TransparencyBox.Max.x, TransparencyBox.Min.y),
                    gs_vec2f(TransparencyBox.Max.x, TransparencyBox.Max.y),
                    gs_vec2f(TransparencyBox.Min.x, TransparencyBox.Max.y),
                };

                _Context->m_Renderer->push_poly_filled(
                    points,
                    colors,
                    4,
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()));

                // slider
                gs_2d_boxf transparencyBoxSlider = gs_2d_boxf(
                    TransparencyBox.Min + gs_vec2f(0.f, TransparencySliderPosition * TransparencyBox.height() * 0.9f),
                    TransparencyBox.Min + gs_vec2f(0.f, TransparencySliderPosition * TransparencyBox.height() * 0.9f) + gs_vec2f(TransparencyBox.width(), TransparencyBox.height() * 0.1f));

                _Context->m_Renderer->push_rectangle_filled(
                    transparencyBoxSlider.Min,
                    transparencyBoxSlider.Max,
                    gs_color_rgba(0, 0, 0, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());

                _Context->m_Renderer->push_rectangle_filled(
                    transparencyBoxSlider.Min + gs_vec2f(4.f),
                    transparencyBoxSlider.Max - gs_vec2f(4.f),
                    transparencyBoxSlider.contains(_Context->m_Input.get_cusor_position()) || TransparencySliderIsMoving ?
                        gs_color_rgba(128, 128, 128, 255) :
                            gs_color_rgba(255, 255, 255, 255),
                    _Context->m_Renderer->calculate_transform_matrix((float)place_in_follow()),
                    _Context->m_Style.get_frames_radius());

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
                    EllipseSliderPosition         = (_Context->m_Input.get_cusor_position() - Ellipse.Center) / gs_vec2f(Ellipse.MinorRadius, Ellipse.MajorRadius);
                    EllipseSliderPreviousPosition = EllipseSliderPosition;
                }

                gs_vec2f radiusVector = (EllipseSliderPreviousPosition + _Context->m_Input.get_cusor_drag_delta() / gs_vec2f(Ellipse.MinorRadius, Ellipse.MajorRadius));
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
        gs_2d_boxf      BrightnessBox                      = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.0, 0.f));
        float          BrightnessSliderPosition           = 0.f;
        float          BrightnessSliderPreviousPosition   = 0.f;
        bool           BrightnessSliderIsMoving           = false;

        // transparency
        gs_2d_boxf      TransparencyBox                    = gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.0, 0.f));
        float          TransparencySliderPosition         = 0.f;
        float          TransparencySliderPreviousPosition = 0.f;
        bool           TransparencySliderIsMoving         = false;
    };

    next_content_margin(gs_vec4f(m_Style.get_frames_width() * 2.f));
    next_content_padding(gs_vec4f(m_Style.get_frames_width() * 2.f));

    if(begin_vertical_stack(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        if(begin_node<ImmediateUserInterfaceColorPickerHSVA>(next_id("ColorPicker"), ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
        {
            ImmediateUserInterfaceColorPickerHSVA* picker = get_rendering_stack_top<ImmediateUserInterfaceColorPickerHSVA>();

            picker->Settings = _Settings;

            if(picker != nullptr && !picker->Edited)
                picker->force_rgba_color(_Color);
            else
                _Color = picker->Color;

            end_node<ImmediateUserInterfaceColorPickerHSVA>();
        }

        input_color(next_id("ColorEditor"), _Color, _Settings);

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
                0.f,
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

void ImmediateUserInterfaceContextLayer::plot_axis_x(const std::string& _ID, const float& _Min, const float& _Max, const int& _TicksCount, const ImmediateUserInterfacePlotLineAxisSettings& _Settings)
{
    if(begin_node<ImmediateUserInterfaceHorizontalPlotAxis>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceHorizontalPlotAxis* axis = 
            get_rendering_stack_top<ImmediateUserInterfaceHorizontalPlotAxis>();

        axis->MinReference = gs_vec2f(_Min, 0.f);
        axis->MaxReference = gs_vec2f(_Max, 0.f);
        axis->TicksCount   = _TicksCount;
        axis->Settings     = _Settings;

        end_node<ImmediateUserInterfaceHorizontalPlotAxis>();
    }
}

void ImmediateUserInterfaceContextLayer::plot_axis_y(const std::string& _ID, const float& _Min, const float& _Max, const int& _TicksCount, const ImmediateUserInterfacePlotLineAxisSettings& _Settings)
{
    if(begin_node<ImmediateUserInterfaceVerticalPlotAxis>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceVerticalPlotAxis* axis = 
            get_rendering_stack_top<ImmediateUserInterfaceVerticalPlotAxis>();

        axis->MinReference = gs_vec2f(0.f, _Min);
        axis->MaxReference = gs_vec2f(0.f, _Max);
        axis->TicksCount   = _TicksCount;
        axis->Settings     = _Settings;

        end_node<ImmediateUserInterfaceVerticalPlotAxis>();
    }
}

std::optional<gs_vec4f> ImmediateUserInterfaceContextLayer::plot_line(
    const std::string&                            _ID,
    const float                                   _X[],
    const float                                   _Y[],
    const int&                                    _N,
    const gs_color&                               _Color,
    const float&                                  _Width,
    const ImmediateUserInterfacePlotLineSettings& _Settings,
    const std::optional<gs_vec4f>&     _Range)
{
    if(_X == nullptr || _Y == nullptr)
        return _Range;

    if(begin_node<ImmediateUserInterfacePlot>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfacePlot* widget =
            get_rendering_stack_top<ImmediateUserInterfacePlot>();

        auto parent = m_Hierarchy.get_parent(widget);

        // bounding box
        gs_2d_boxf referenceBox =
            parent != nullptr ?
                gs_2d_boxf(widget->State.BoundingBox.Min, widget->State.BoundingBox.Min + parent->get_visible_rect(this).size()) :
                    widget->State.BoundingBox;

        gs_2d_boxf visibleBox = 
            parent != nullptr ?
                parent->get_visible_rect(this) :
                    widget->get_visible_rect(this);

        // axis
        ImmediateUserInterfacePlotWidget* plotWidget =
            m_Hierarchy.get_parent<ImmediateUserInterfacePlotWidget>(widget);

        GS_ASSERT(plotWidget != nullptr);
        GS_ASSERT(plotWidget->CurrentXAxis != nullptr);
        GS_ASSERT(plotWidget->CurrentYAxis != nullptr);

        // axis
        float scaleX  = referenceBox.width()  / (plotWidget->CurrentXAxis->MaxScaled.x - plotWidget->CurrentXAxis->MinScaled.x);
        float scaleY  = referenceBox.height() / (plotWidget->CurrentYAxis->MinScaled.y - plotWidget->CurrentYAxis->MaxScaled.y);
        float offsetX = referenceBox.Min.x - plotWidget->CurrentXAxis->MinScaled.x * scaleX;
        float offsetY = referenceBox.Min.y - plotWidget->CurrentYAxis->MaxScaled.y * scaleY;
            
        // render
        {
            m_Renderer->push_clip_box(visibleBox);

            int depth = widget->Cache.Depth;
            int init  = depth;

            // construct clipper
            ImmediateUserInterfaceHorizontalClipper clipper = ImmediateUserInterfaceHorizontalClipper(
                plotWidget->CurrentXAxis,
                _N,
                gs_max(widget->State.ContentSize.x, 4.f) / _N);

            widget->Color = _Color;

            // adjust source and target indexes of clipper
            {
                int start = clipper.SourceElement;

                // decrement source element untill we reach an invisible point
                // This is going to be out starting point
                for (int i = start; i >= 0; --i, --clipper.SourceElement)
                {
                    gs_vec2f point = gs_vec2f(_X[i] * scaleX + offsetX, visibleBox.center().y);
                    if(!visibleBox.contains(gs_vec2f(point))) break;
                }

                // look for the first visible point between initial start and end points
                for (int i = start; i < clipper.TargetElement; ++i, ++start)
                {
                    gs_vec2f point = gs_vec2f(_X[i] * scaleX + offsetX, visibleBox.center().y);
                    if(visibleBox.contains(gs_vec2f(point))) break;
                }

                // increment end point untill we get first invisible point
                // This is going to be an end
                for (int i = start; i < _N; ++i, ++clipper.TargetElement)
                {
                    gs_vec2f point = gs_vec2f(_X[i] * scaleX + offsetX, visibleBox.center().y);
                    if(!visibleBox.contains(gs_vec2f(point))) break;
                }

                // normalize bounds
                clipper.SourceElement = gs_max(clipper.SourceElement, 0);
                clipper.TargetElement = gs_min(clipper.TargetElement, _N);
            }

            // plot clipped data range
            std::optional<gs_2d_linef> previousSegment;

            for (int i = clipper.SourceElement; i < clipper.TargetElement; i++)
            {
                // restore depth
                depth = init;

                // render
                gs_vec2f source = gs_vec2f(_X[i] * scaleX + offsetX, _Y[i] * scaleY + offsetY);
                gs_vec2f target =
                    i < _N - 1 ?
                        gs_vec2f(_X[i + 1] * scaleX + offsetX, _Y[i + 1] * scaleY + offsetY) :
                            gs_vec2f(_X[i - 1] * scaleX + offsetX, _Y[i - 1] * scaleY + offsetY);

                if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsLines)
                {
                    // highlight
                    if(
                        (widget->XAxis->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ||
                        (widget->YAxis->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                    {
                        m_Renderer->push_line(
                            source,
                            target,
                            _Width * 2.f,
                            _Color,
                            m_Renderer->calculate_transform_matrix((float)(depth++)),
                            previousSegment);
                    }

                    // line
                    m_Renderer->push_line(
                        source,
                        target,
                        _Width,
                        _Color,
                        m_Renderer->calculate_transform_matrix((float)(depth++)),
                        previousSegment);
                }
                else if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsStems)
                {
                    // highlight
                    if(
                        (widget->XAxis->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ||
                        (widget->YAxis->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                    {
                        m_Renderer->push_line(
                            gs_vec2f(source.x, offsetY),
                            source,
                            _Width * 1.2f,
                            _Color,
                            m_Renderer->calculate_transform_matrix((float)(depth++)));

                        m_Renderer->push_arc_filled(
                            source,
                            _Width * 1.2f,
                            _Width * 1.2f,
                            0.f,
                            360.f,
                            _Color,
                            m_Renderer->calculate_transform_matrix((float)(depth++)));
                    }

                    m_Renderer->push_line(
                        gs_vec2f(source.x, offsetY),
                        source,
                        _Width,
                        _Color,
                        m_Renderer->calculate_transform_matrix((float)(depth++)));

                    m_Renderer->push_arc_filled(
                        source,
                        _Width,
                        _Width,
                        0.f,
                        360.f,
                        _Color,
                        m_Renderer->calculate_transform_matrix((float)(depth++)));
                }
                else if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsPoints)
                {
                    // highlight
                    if(
                        (widget->XAxis->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ||
                        (widget->YAxis->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                    {
                        m_Renderer->push_arc_filled(
                            source,
                            _Width * 1.2f,
                            _Width * 1.2f,
                            0.f,
                            360.f,
                            _Color,
                            m_Renderer->calculate_transform_matrix((float)(depth++)));                        
                    }

                    m_Renderer->push_arc_filled(
                        source,
                        _Width,
                        _Width,
                        0.f,
                        360.f,
                        _Color,
                        m_Renderer->calculate_transform_matrix((float)(depth++)));
                }
                else if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsRectangles)
                {
                    // highlight
                    if(
                        (widget->XAxis->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) ||
                        (widget->YAxis->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
                    {
                        m_Renderer->push_rectangle_filled(
                            source - gs_vec2f(1.f, 1.f),
                            gs_vec2f(target.x - 1.f, offsetY) - gs_vec2f(1.f, 1.f),
                            _Color,
                            m_Renderer->calculate_transform_matrix((float)(depth++)));                 
                    }

                    m_Renderer->push_rectangle_filled(
                        source,
                        gs_vec2f(target.x - 1.f, offsetY),
                        _Color,
                        m_Renderer->calculate_transform_matrix((float)(depth++)));
                }
                else if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsConvexAreas)
                {
                    // line
                    m_Renderer->push_line(
                        source,
                        target,
                        _Width,
                        _Color,
                        m_Renderer->calculate_transform_matrix((float)(depth++)),
                        previousSegment);

                    // convex area
                    gs_color convexAreaFillColor = gs_color_rgba(
                        gs_color_rgba_get_r(_Color),
                        gs_color_rgba_get_g(_Color),
                        gs_color_rgba_get_b(_Color),
                        128);

                    gs_vec2f points[4] = { gs_vec2f(source.x, offsetY), gs_vec2f(source.x, source.y), gs_vec2f(target.x, target.y), gs_vec2f(target.x, offsetY) };
                    gs_color colors[4] = { convexAreaFillColor, convexAreaFillColor, convexAreaFillColor, convexAreaFillColor };

                    m_Renderer->push_poly_filled(points, colors, 4, m_Renderer->calculate_transform_matrix((float)(depth++)));
                }

                // markers
                gs_color markerColor = gs_color_rgb(
                    gs_color_rgba_get_r(_Color) * 0.8,
                    gs_color_rgba_get_g(_Color) * 0.8,
                    gs_color_rgba_get_b(_Color) * 0.8);

                if(
                    !(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsStems) &&
                    !(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsPoints))
                {
                    if(!(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersOpened))
                    {
                        if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersPoints)
                        {
                            m_Renderer->push_arc_filled(
                                source,
                                _Width,
                                _Width,
                                0.f,
                                360.f,
                                markerColor,
                                m_Renderer->calculate_transform_matrix((float)(depth++)));
                        }
                        else if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersTriangles)
                        {
                            m_Renderer->push_triangle_filled(
                                source + gs_vec2f(0.f, -_Width * 1.2f),
                                source + gs_vec2f(0.f, +_Width * 1.2f),
                                source + gs_vec2f(_Width * 1.2f, 0.f),
                                markerColor,
                                m_Renderer->calculate_transform_matrix((float)(depth++)));
                        }
                        else if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersRectangles)
                        {
                            m_Renderer->push_rectangle_filled(
                                source + gs_vec2f(-_Width, -_Width),
                                source + gs_vec2f(+_Width, +_Width),
                                markerColor,
                                m_Renderer->calculate_transform_matrix((float)(depth++)));
                        }
                    }
                    else
                    {
                        if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersPoints)
                        {
                            m_Renderer->push_arc(
                                source,
                                _Width,
                                _Width,
                                0.f,
                                360.f,
                                markerColor,
                                4.f,
                                m_Renderer->calculate_transform_matrix((float)(depth++)));
                        }
                        else if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersTriangles)
                        {
                            m_Renderer->push_triangle(
                                source + gs_vec2f(0.f, -_Width * 1.2f),
                                source + gs_vec2f(0.f, +_Width * 1.2f),
                                source + gs_vec2f(_Width * 1.2f, 0.f),
                                markerColor,
                                4.f,
                                m_Renderer->calculate_transform_matrix((float)(depth++)));
                        }
                        else if(_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersRectangles)
                        {
                            m_Renderer->push_rectangle(
                                source + gs_vec2f(-_Width, -_Width),
                                source + gs_vec2f(+_Width, +_Width),
                                markerColor,
                                4.f,
                                m_Renderer->calculate_transform_matrix((float)(depth++)));
                        }
                    }
                }

                if(
                    (_Settings & ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderLabelsOnHover)   &&
                    gs_2d_ellipsef(source, _Width).contains(m_Input.get_cusor_position()))
                {
                    m_Renderer->push_arc_filled(
                        source,
                        _Width,
                        _Width,
                        0.f,
                        360.f,
                        gs_color_32bit_invert(_Color),
                        m_Renderer->calculate_transform_matrix((float)(ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node((parent != nullptr ? parent : widget)))));

                    std::string label = Frenchie::Core::String::format("X: %.2f Y: %.2f", _X[i], _Y[i]);

                    m_Renderer->push_text(
                        source + _Width,
                        label.begin(),
                        label.end(),
                        m_Style.get_font_size(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)(ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node((parent != nullptr ? parent : widget)) + 1.f)),
                        m_Style.get_current_font());
                }
            
                previousSegment = gs_2d_linef(source, target);
            }

            widget->State.SelfThickness = depth - init;

            m_Renderer->pop_clip_box();
        }

        // geometry
        gs_vec2f min = gs_vec2f(gs_huge<float>(), gs_huge<float>());
        gs_vec2f max = gs_vec2f(gs_tiny<float>(), gs_tiny<float>());

        {
            if(_Range.has_value())
            {
                min = gs_vec2f(_Range.value().x, _Range.value().y);
                max = gs_vec2f(_Range.value().z, _Range.value().w);
            }
            else
            {
                for (int i = 0; i < _N; i++)
                {
                    min = gs_vec2f(gs_min(_X[i], min.x), gs_min(_Y[i], min.y));
                    max = gs_vec2f(gs_max(_X[i], max.x), gs_max(_Y[i], max.y));
                }
            }

            widget->State.ContentSize = gs_2d_boxf(
                gs_vec2f(min.x * scaleX + offsetX, min.y * scaleY + offsetY),
                gs_vec2f(max.x * scaleX + offsetX, max.y * scaleY + offsetY)).size();
        }

        end_node<ImmediateUserInterfacePlot>();

        return gs_vec4f(min.x, min.y, max.x, max.y);
    }

    return _Range;
}

void ImmediateUserInterfaceContextLayer::plot_pie(const std::string _Names [], const float _Values[], const gs_color _Colors[], const int& _Count)
{
    if(_Names == nullptr || _Values == nullptr || _Colors == nullptr)
        return;

    // assert
    ImmediateUserInterfacePlotWidget* plotWidget =
        get_rendering_stack_top<ImmediateUserInterfacePlotWidget>();

    GS_ASSERT(plotWidget);
    GS_ASSERT(plotWidget->PlotsView);

    // compute total
    float total = 0.f;
    for (int i = 0; i < _Count; i++)
        total += _Values[i];

    // compute minimum text label height
    float textLabelHeight = gs_huge<float>();

    {
        float sum    = 0.f;
        float radius = gs_min(plotWidget->PlotsView->State.BoundingBox.width(), plotWidget->PlotsView->State.BoundingBox.height()) * 0.5f;

        for (int i = 0; i < _Count; i++)
        {
            float    sourceAngle = sum / total * 360.f;
            float    targetAngle = (sum + _Values[i]) / total * 360.f;
            gs_vec2f sourcePoint = plotWidget->PlotsView->State.BoundingBox.center() + gs_vec2f(cos(gs_to_radians(sourceAngle)), sin(gs_to_radians(sourceAngle))) * radius * 0.5f;
            gs_vec2f targetPoint = plotWidget->PlotsView->State.BoundingBox.center() + gs_vec2f(cos(gs_to_radians(targetAngle)), sin(gs_to_radians(targetAngle))) * radius * 0.5f;

            textLabelHeight = gs_min(textLabelHeight, (float)gs_vector_length(targetPoint - sourcePoint) * 0.75f);
        }
    }

    // plot pie
    float sum = 0.f;
    for (int i = 0; i < _Count; i++)
    {
        if(begin_node<ImmediateUserInterfacePlotPie>(
            next_id(_Names[i], Frenchie::Core::String::format("pie-%d", i)),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
        {
            ImmediateUserInterfacePlotPie* widget = get_rendering_stack_top<ImmediateUserInterfacePlotPie>();

            // render
            {
                m_Renderer->push_clip_box(widget->get_clipping_box(this));
                int depth = widget->Cache.Depth;
                int init  = depth;

                // pie
                float radius      = gs_min(widget->State.BoundingBox.width(), widget->State.BoundingBox.height()) * 0.5f;
                float sourceAngle = sum / total * 360.f;
                float targetAngle = (sum + _Values[i]) / total * 360.f;

                m_Renderer->push_arc_filled(
                    widget->State.BoundingBox.center(),
                    radius,
                    radius,
                    sourceAngle,
                    targetAngle,
                    _Colors[i],
                    m_Renderer->calculate_transform_matrix((float)depth++));

                // highlight
                gs_vec2f cursorVector = m_Input.get_cusor_position() - widget->State.BoundingBox.center();

                double cursorAngleNorm = gs_normalize_angle(gs_vector_argument(cursorVector));
                double sourceAngleNorm = gs_normalize_angle(gs_to_radians(sourceAngle));
                double targetAngleNorm = gs_normalize_angle(gs_to_radians(targetAngle));
                bool   sectorIsHovered = gs_vector_length(cursorVector) < radius &&
                    (sourceAngleNorm <= targetAngleNorm ?
                        (cursorAngleNorm >= sourceAngleNorm && cursorAngleNorm <= targetAngleNorm) :
                            (cursorAngleNorm >= sourceAngleNorm || cursorAngleNorm <= targetAngleNorm));

                if(sectorIsHovered)
                {
                    m_Renderer->push_arc_filled(
                        widget->State.BoundingBox.center(),
                        radius + 16.f,
                        radius + 16.f,
                        sourceAngle,
                        targetAngle,
                        _Colors[i],
                        m_Renderer->calculate_transform_matrix((float)depth++));
                }

                // label
                std::string percantage = Frenchie::Core::String::format("%.2f %%", _Values[i] / total * 100.f);

                if(textLabelHeight < 24.f)
                {
                    if(sectorIsHovered)
                    {
                        m_Renderer->push_text(
                            m_Input.get_cusor_position() + gs_vec2f(12.f, 12.f),
                            percantage.begin(),
                            percantage.end(),
                            m_Style.get_font_size(),
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                            m_Renderer->calculate_transform_matrix(ImmediateUserInterfaceContextLayerHelpers::calculate_depth_over_node(plotWidget)),
                            m_Style.get_current_font());
                    }
                }
                else
                {
                    gs_vec2f labelSize  = m_Renderer->calculate_bounding_box(percantage.begin(), percantage.end(), textLabelHeight, m_Style.get_current_font()).size();
                    float    textAngle  = (targetAngle + sourceAngle) * 0.5f;

                    m_Renderer->push_text(
                        widget->State.BoundingBox.center() + gs_vec2f(radius, radius) * gs_vec2f(cos(gs_to_radians(textAngle)), sin(gs_to_radians(textAngle))) * 0.7f - labelSize * 0.5f,
                        percantage.begin(),
                        percantage.end(),
                        textLabelHeight,
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix((float)depth++),
                        m_Style.get_current_font());
                }

                widget->Color = _Colors[i];
                widget->State.SelfThickness = depth - init;
                m_Renderer->pop_clip_box();
            }

            // geometry
            {
                ImmediateUserInterfaceNode* parent = m_Hierarchy.get_parent(widget);

                widget->State.BoundingBox = gs_2d_boxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + (parent != nullptr ? parent->State.BoundingBox.size() : widget->State.BoundingBox.size()));
            }

            end_node<ImmediateUserInterfacePlotPie>();
        }

        sum += _Values[i];
    }
}

void ImmediateUserInterfaceContextLayer::plot_vector(const std::string _Names [], const gs_vec4f _Values[], const gs_color _Colors[], const int& _Count)
{
    // nested types
    struct ImmediateUserInterfaceVectorPlotSurface : public ImmediateUserInterfacePlotViewItem
    {
        ImmediateUserInterfaceVectorPlotSurface(const std::string& _Hash) : ImmediateUserInterfacePlotViewItem(_Hash){}
        virtual ~ImmediateUserInterfaceVectorPlotSurface(){}

        std::optional<gs_vec2f> SourcePoint;
        std::optional<gs_vec2f> TargetPoint;
    };
    

    // assert
    ImmediateUserInterfacePlotWidget* plotWidget =
        get_rendering_stack_top<ImmediateUserInterfacePlotWidget>();

    GS_ASSERT(plotWidget);
    GS_ASSERT(plotWidget->PlotsView);
    
    std::optional<gs_vec2f> vectorDiagramOrigin;
    std::optional<float>    vectorDiagramradius;

    // find farthest from origin point vector length
    float max = 0.f;
    
    for (int i = 0; i < _Count; i++)
    {
        max = gs_max<float>(
            gs_vector_length(gs_vec2f(_Values[i].x, _Values[i].y)),
            gs_vector_length(gs_vec2f(_Values[i].z, _Values[i].w)),
            max);
    }

    // render surface
    if(begin_node<ImmediateUserInterfaceVectorPlotSurface>(
        next_id("Surface"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceVectorPlotSurface* widget = get_rendering_stack_top<ImmediateUserInterfaceVectorPlotSurface>();
        ImmediateUserInterfacePlotView*          parent = m_Hierarchy.get_parent<ImmediateUserInterfacePlotView>(widget);

        GS_ASSERT(parent);

        vectorDiagramOrigin = parent->State.BoundingBox.center();
        vectorDiagramradius = gs_min(parent->State.BoundingBox.width(), parent->State.BoundingBox.height()) * 0.5f * 0.9f;

        // render
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));
            int depth = widget->Cache.Depth;
            int init  = depth;

            // background
            m_Renderer->push_arc_filled(
                widget->State.BoundingBox.center(),
                vectorDiagramradius.value(),
                vectorDiagramradius.value(),
                0.f,
                360.f,
                gs_color_rgb(128, 128, 128),
                m_Renderer->calculate_transform_matrix((float)depth++));

            // horizontal base line
            m_Renderer->push_line(
                widget->State.BoundingBox.center() + gs_vec2f(vectorDiagramradius.value(), 0.f),
                widget->State.BoundingBox.center() - gs_vec2f(vectorDiagramradius.value(), 0.f),
                4.f,
                gs_color_rgb(32, 32, 32),
                m_Renderer->calculate_transform_matrix((float)depth++));

            // vertical base line
            m_Renderer->push_line(
                widget->State.BoundingBox.center() + gs_vec2f(0.f, vectorDiagramradius.value()),
                widget->State.BoundingBox.center() - gs_vec2f(0.f, vectorDiagramradius.value()),
                4.f,
                gs_color_rgb(32, 32, 32),
                m_Renderer->calculate_transform_matrix((float)depth++));

            // ellipses
            for (int i = 1; i < 5; i++)            
            {
                float radius = vectorDiagramradius.value() * (i * 0.25f);

                m_Renderer->push_arc(
                    widget->State.BoundingBox.center(),
                    radius,
                    radius,
                    0.f,
                    360.f,
                    gs_color_rgb(32, 32, 32),
                    4.f,
                    m_Renderer->calculate_transform_matrix((float)depth++));

                // horizontal label
                std::string plus  = Frenchie::Core::String::format("%.0f", +max * radius / vectorDiagramradius.value());
                std::string minus = Frenchie::Core::String::format("%.0f", -max * radius / vectorDiagramradius.value());

                // horizontal labels
                m_Renderer->push_text(
                    widget->State.BoundingBox.center() + gs_vec2f(+radius, + 0.5f * m_Style.get_font_size()),
                    plus.begin(),
                    plus.end(),
                    m_Style.get_font_size(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_current_font());

                m_Renderer->push_text(
                    widget->State.BoundingBox.center() + gs_vec2f(-radius, +0.5f * m_Style.get_font_size()),
                    minus.begin(),
                    minus.end(),
                    m_Style.get_font_size(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_current_font());

                // vertical labels
                m_Renderer->push_text(
                    widget->State.BoundingBox.center() + gs_vec2f(+0.5f * m_Style.get_font_size(), +radius),
                    plus.begin(),
                    plus.end(),
                    m_Style.get_font_size(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_current_font());

                m_Renderer->push_text(
                    widget->State.BoundingBox.center() + gs_vec2f(+0.5f * m_Style.get_font_size(), -radius),
                    minus.begin(),
                    minus.end(),
                    m_Style.get_font_size(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_current_font());
            }

            // angle measurement arc
            if(widget->SourcePoint.has_value() && widget->TargetPoint.has_value())
            {
                auto normalizeAngle = [](double angle)
                {
                    while (angle < 0   ) angle += PI2;
                    while (angle >= PI2) angle -= PI2;
                    return angle;
                };

                // arc
                float angleMeasurementArcRadius       = gs_vector_length(widget->SourcePoint.value() - vectorDiagramOrigin.value());
                float angleMeasurementArcSourceAngle  = normalizeAngle(gs_vector_argument(widget->SourcePoint.value() - vectorDiagramOrigin.value()));
                float targetMeasurementArcSourceAngle = normalizeAngle(gs_vector_argument(widget->TargetPoint.value() - vectorDiagramOrigin.value()));

                m_Renderer->push_arc(
                    widget->State.BoundingBox.center(),
                    angleMeasurementArcRadius,
                    angleMeasurementArcRadius,
                    gs_to_degrees(angleMeasurementArcSourceAngle),
                    gs_to_degrees(targetMeasurementArcSourceAngle),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    12.f,
                    m_Renderer->calculate_transform_matrix(
                        ImmediateUserInterfaceContextLayerHelpers::calculate_layer_depth(this, ImmediateUserInterfaceRenderingLayer_::ImmediateUserInterfaceRenderingLayer_Gizmos)));

                // text label
                std::string label = Frenchie::Core::String::format("%.2f", gs_to_degrees(targetMeasurementArcSourceAngle - angleMeasurementArcSourceAngle));

                m_Renderer->push_text(
                    widget->TargetPoint.value() + ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this),
                    label.begin(),
                    label.end(),
                    m_Style.get_font_size(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                    m_Renderer->calculate_transform_matrix(
                        ImmediateUserInterfaceContextLayerHelpers::calculate_layer_depth(this, ImmediateUserInterfaceRenderingLayer_::ImmediateUserInterfaceRenderingLayer_Gizmos)),
                    m_Style.get_current_font());
            }
                
            widget->State.SelfThickness = depth - init;
            m_Renderer->pop_clip_box();
        }

        // events
        if(widget->State.BoundingBox.contains(m_Input.get_cusor_position()))
        {
            if(m_Input.is_mouse_button_pressed())
                widget->SourcePoint = m_Input.get_cusor_position();
            
            if(m_Input.is_mouse_button_down())
                widget->TargetPoint = m_Input.get_cusor_position();
            
            if(m_Input.is_mouse_button_released())
            {
                widget->SourcePoint.reset();
                widget->TargetPoint.reset();
            }
        }

        // geometry
        {
            widget->State.BoundingBox = gs_2d_boxf(
                widget->State.BoundingBox.Min,
                widget->State.BoundingBox.Min + parent->State.BoundingBox.size());
        }

        end_node<ImmediateUserInterfaceVectorPlotSurface>();
    }

    if(!vectorDiagramOrigin.has_value() || !vectorDiagramradius.has_value())
        return;

    // render vectors
    bool anyHovered = false;

    // render scaled
    for (int i = 0; i < _Count; i++)
    {
        if(begin_node<ImmediateUserInterfacePlotVector>(
            next_id(_Names[i], Frenchie::Core::String::format("vector-%d", i)),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
        {
            ImmediateUserInterfacePlotVector* widget =
                get_rendering_stack_top<ImmediateUserInterfacePlotVector>();

            widget->Color = _Colors[i];

            // render
            {
                m_Renderer->push_clip_box(widget->get_clipping_box(this));
                int depth = widget->Cache.Depth;
                int init  = depth;

                // line
                float    vectorLineWidth     = 16.f;
                float    vectorArrowWidth    = vectorLineWidth * 2.5f;
                gs_vec2f sourceVectorPoint   = gs_vec2f(_Values[i].x, _Values[i].y);
                gs_vec2f targetVectorPoint   = gs_vec2f(_Values[i].z, _Values[i].w);
                gs_vec2f vectorDirection     = gs_vector_normalize(targetVectorPoint - sourceVectorPoint);
                gs_vec2f vectorPerpendicular = gs_vector_normalize(gs_vector_cross(gs_vec3f(vectorDirection), gs_vec3f(0.f, 0.f, 1.f))) * vectorArrowWidth * 0.5f;

                m_Renderer->push_line(
                    vectorDiagramOrigin.value() + sourceVectorPoint / max * vectorDiagramradius.value(),
                    vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value() + (-1.f * gs_vec2f(vectorDirection)) * vectorArrowWidth,
                    vectorLineWidth,
                    _Colors[i],
                    m_Renderer->calculate_transform_matrix((float)depth++));

                // arrow
                m_Renderer->push_triangle_filled(
                    vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value(),
                    vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value() - vectorPerpendicular + (-1.f * vectorDirection) * vectorArrowWidth,
                    vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value() + vectorPerpendicular + (-1.f * vectorDirection) * vectorArrowWidth,
                    _Colors[i],
                    m_Renderer->calculate_transform_matrix((float)depth++));

                // highlight
                gs_vec2f linePoints[4] =
                {
                    vectorDiagramOrigin.value() + sourceVectorPoint / max * vectorDiagramradius.value() - vectorPerpendicular,
                    vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value() - vectorPerpendicular,
                    vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value() + vectorPerpendicular,
                    vectorDiagramOrigin.value() + sourceVectorPoint / max * vectorDiagramradius.value() + vectorPerpendicular,
                };


                if(gs_2D_point_in_polygon(linePoints, 4, m_Input.get_cusor_position()) && !m_Input.is_mouse_button_down() && !anyHovered)
                {
                    // line
                    m_Renderer->push_line(
                        vectorDiagramOrigin.value() + sourceVectorPoint / max * vectorDiagramradius.value(),
                        vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value() + (-1.f * gs_vec2f(vectorDirection)) * vectorArrowWidth,
                        vectorLineWidth * 1.2f,
                        _Colors[i],
                        m_Renderer->calculate_transform_matrix((float)depth++));

                    // arrow
                    m_Renderer->push_triangle_filled(
                        vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value() + vectorDirection * vectorArrowWidth * 0.25f,
                        vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value() - vectorPerpendicular * 1.2f + (-1.f * vectorDirection) * vectorArrowWidth,
                        vectorDiagramOrigin.value() + targetVectorPoint / max * vectorDiagramradius.value() + vectorPerpendicular * 1.2f + (-1.f * vectorDirection) * vectorArrowWidth,
                        _Colors[i],
                        m_Renderer->calculate_transform_matrix((float)depth++));

                    // text label
                    std::string label = Frenchie::Core::String::format(
                        "%.2f exp(j %.2f)",
                        gs_vector_length(targetVectorPoint - sourceVectorPoint),
                        gs_to_degrees(gs_vector_argument(targetVectorPoint - sourceVectorPoint)));

                    m_Renderer->push_text(
                        m_Input.get_cusor_position() + ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this),
                        label.begin(),
                        label.end(),
                        m_Style.get_font_size(),
                        m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Text),
                        m_Renderer->calculate_transform_matrix(
                            ImmediateUserInterfaceContextLayerHelpers::calculate_layer_depth(this, ImmediateUserInterfaceRenderingLayer_::ImmediateUserInterfaceRenderingLayer_Gizmos)),
                        m_Style.get_current_font());

                    anyHovered = true;
                }

                widget->State.SelfThickness = depth - init;
                m_Renderer->pop_clip_box();
            }

            // geometry
            {
                ImmediateUserInterfaceNode* parent = m_Hierarchy.get_parent(widget);

                widget->State.BoundingBox = gs_2d_boxf(
                    widget->State.BoundingBox.Min,
                    widget->State.BoundingBox.Min + (parent != nullptr ? parent->State.BoundingBox.size() : widget->State.BoundingBox.size()));
            }

            end_node<ImmediateUserInterfacePlotVector>();
        }
    }
}

bool ImmediateUserInterfaceContextLayer::begin_combobox(const std::string& _ID, const std::string& _Preview)
{
    if(begin_node<ImmediateUserInterfaceCombobox>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceCombobox* widget = get_rendering_stack_top<ImmediateUserInterfaceCombobox>();

        gs_2d_boxf boundingBox = gs_2d_boxf(widget->State.BoundingBox.Min - m_Style.get_frames_width(), widget->State.BoundingBox.Max + m_Style.get_frames_width());

        // render
        {
            m_Renderer->push_clip_box(widget->get_clipping_box(this));
            int depth = widget->Cache.Depth;
            int init  = depth;

            // outline
            m_Renderer->push_rectangle_filled(
                boundingBox.Min,
                boundingBox.Max,
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                m_Renderer->calculate_transform_matrix((float)depth++),
                m_Style.get_frames_radius());

            // background
            m_Renderer->push_rectangle_filled(
                boundingBox.Min + m_Style.get_frames_width(),
                boundingBox.Max - m_Style.get_frames_width(),
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                m_Renderer->calculate_transform_matrix((float)depth++),
                m_Style.get_frames_radius());

            // open button
            gs_2d_boxf openButtonBox = gs_2d_boxf(
                boundingBox.Min,
                boundingBox.Min + boundingBox.height());

            m_Renderer->push_rectangle_filled(
                openButtonBox.Min,
                openButtonBox.Max,
                m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonOutline),
                m_Renderer->calculate_transform_matrix((float)depth++),
                m_Style.get_frames_radius());

            if(openButtonBox.contains(m_Input.get_cusor_position()) && m_Input.is_mouse_button_down())
            {
                m_Renderer->push_rectangle_filled(
                    openButtonBox.Min + m_Style.get_frames_width(),
                    openButtonBox.Max - m_Style.get_frames_width(),
                    m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_frames_radius());
            }
            else
            {
                m_Renderer->push_rectangle_filled(
                    openButtonBox.Min + m_Style.get_frames_width(),
                    openButtonBox.Max - m_Style.get_frames_width(),
                        openButtonBox.contains(m_Input.get_cusor_position()) ?
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered) :
                            m_Style.get_color(ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_ButtonBackground),
                    m_Renderer->calculate_transform_matrix((float)depth++),
                    m_Style.get_frames_radius());
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

            widget->State.SelfThickness = depth - init;
            m_Renderer->pop_clip_box();
        }

        // activate
        if((widget->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered))
        {
            widget->HoverTime = Frenchie::Core::Clock::tic();
        }
        else if(
             widget->ScrollArea != nullptr                                                &&
            !widget->ScrollArea->State.BoundingBox.contains(m_Input.get_cusor_position()) &&
            Frenchie::Core::Clock::elapsed<Frenchie::Core::Clock::Microseconds>(widget->HoverTime, Frenchie::Core::Clock::tic()) > 100)
        {
            widget->Active  = false;
            widget->Hovered = false;
        }

        if(!widget->Active)
        {
            end_combobox();
            return false;
        }

        float margin = m_Style.get_frames_width() + m_Style.get_frames_radius() * 0.5f;
        next_content_margin(gs_vec4f(margin, margin, 0.f, 0.f));
        next_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Popup);

        if(begin_node<ImmediateUserInterfaceComboboxScrollArea>(next_id("ScrollArea"),
              ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent
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

            // calculate rect
            gs_2d_boxf box = widget->get_visible_rect(this);
            
            widget->ScrollArea->State.BoundingBox = gs_2d_boxf(
                gs_vec2f(box.Min.x, box.Max.y) - gs_vec2f(m_Style.get_frames_width(), 0.f),
                gs_vec2f(box.Min.x, box.Max.y) - gs_vec2f(m_Style.get_frames_width(), 0.f) + widget->ScrollArea->State.BoundingBox.size());

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

            // create an empty filler node
            next_size(gs_vec2f(widget->State.BoundingBox.width(), 0.f));
            empty_node(next_id("Filler"));

            end_node<ImmediateUserInterfaceComboboxScrollArea>();
        }
    }

    return true;
}

void ImmediateUserInterfaceContextLayer::end_combobox()
{
    end_node<ImmediateUserInterfaceCombobox>();
}

bool ImmediateUserInterfaceContextLayer::begin_what_is_it(const std::string& _ID, const ImmediateUserInterfaceNode* _Node)
{
    return _Node != nullptr && is_current_node_mouse_hovered(_Node) && begin_popup(_ID, true);
}

void ImmediateUserInterfaceContextLayer::end_what_is_it()
{
    end_popup();
}

bool ImmediateUserInterfaceContextLayer::begin_popup(const std::string& _ID, const bool _Popup)
{
    float margin = m_Style.get_frames_width() + m_Style.get_frames_radius() * 0.5f;
    
    if(_Popup)
    {
        next_position(m_Input.get_cusor_position() + gs_vec2f(16.f, 16.f));
        create_node<ImmediateUserInterfacePopupScrollArea>(_ID, false)->enable();
    }

    next_content_margin(gs_vec4f(margin, margin, 0.f, 0.f));
    next_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Popup);

    if(begin_node<ImmediateUserInterfacePopupScrollArea>(
        _ID,
          ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent
        | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically
        | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally))
    {
        if(!_Popup && m_Input.is_mouse_button_clicked())
        {
            get_rendering_stack_top<ImmediateUserInterfacePopupScrollArea>()->WantsToBeDisabled = true;
        }
        else if(get_rendering_stack_top<ImmediateUserInterfacePopupScrollArea>()->WantsToBeDisabled)
        {
            get_rendering_stack_top<ImmediateUserInterfacePopupScrollArea>()->WantsToBeDisabled = false;
            get_rendering_stack_top()->disable();
            end_node<ImmediateUserInterfacePopupScrollArea>();
            return false;
        }

        return true;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::end_popup()
{
    end_node<ImmediateUserInterfacePopupScrollArea>();
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

bool ImmediateUserInterfaceContextLayer::begin_table(const std::string& _ID, const int& _RowsCount, const int& _ColumnsCount, const gs_vec2f& _CellSize)
{
    if(begin_node<ImmediateUserInterfaceTable>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        ImmediateUserInterfaceTable* grid =
            get_rendering_stack_top<ImmediateUserInterfaceTable>();

        grid->GridRowsCount = _RowsCount;
        grid->GridColsCount = _ColumnsCount;
        grid->GridCellSize  = _CellSize;

        return true;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::end_table()
{
    end_node<ImmediateUserInterfaceTable>();
}

bool ImmediateUserInterfaceContextLayer::begin_table_column_header(const int& _Index, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    if(begin_node<ImmediateUserInterfaceTableHorizontalHeader>(next_id(Frenchie::Core::String::format("HorizontalTitle-%d", _Index)), _Settings))
    {
        ImmediateUserInterfaceTableHorizontalHeader* cell =
            get_rendering_stack_top<ImmediateUserInterfaceTableHorizontalHeader>();

        cell->Row    = 0;
        cell->Column = _Index;

        return true;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::end_table_column_header()
{
    end_node<ImmediateUserInterfaceTableHorizontalHeader>();
}

bool ImmediateUserInterfaceContextLayer::begin_table_row_header(const int& _Index, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    if(begin_node<ImmediateUserInterfaceTableVerticalHeader>(next_id(Frenchie::Core::String::format("VerticalTitle-%d", _Index)), _Settings))
    {
        ImmediateUserInterfaceTableVerticalHeader* cell =
            get_rendering_stack_top<ImmediateUserInterfaceTableVerticalHeader>();

        cell->Row    = _Index;
        cell->Column = 0;

        return true;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::end_table_row_header()
{
    end_node<ImmediateUserInterfaceTableVerticalHeader>();
}

bool ImmediateUserInterfaceContextLayer::begin_table_corner_header(const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfaceTableCornerHeader>(next_id("Corner"), _Settings);
}

void ImmediateUserInterfaceContextLayer::end_table_corner_header()
{
    end_node<ImmediateUserInterfaceTableCornerHeader>();
}

bool ImmediateUserInterfaceContextLayer::begin_table_data_cell(const int& _Row, const int& _Column, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    if(begin_node<ImmediateUserInterfaceTableGridCell>(next_id(Frenchie::Core::String::format("Cell-%d-%d", _Row, _Column)), _Settings))
    {
        ImmediateUserInterfaceTableGridCell* cell =
            get_rendering_stack_top<ImmediateUserInterfaceTableGridCell>();

        cell->Row    = _Row;
        cell->Column = _Column;

        return true;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::end_table_data_cell()
{
    end_node<ImmediateUserInterfaceTableGridCell>();
}

bool ImmediateUserInterfaceContextLayer::begin_plot(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    return begin_node<ImmediateUserInterfacePlotWidget>(_ID, _Settings);
}

void ImmediateUserInterfaceContextLayer::end_plot()
{
    end_node<ImmediateUserInterfacePlotWidget>();
}

bool ImmediateUserInterfaceContextLayer::begin_menu(const std::string& _ID)
{
    ImmediateUserInterfaceMenu*       menu      = nullptr;
    ImmediateUserInterfaceMenuAction* menuItem  = nullptr;
    bool                              hasParent = false;
    bool                              isHovered = false;
    float                             margin    = m_Style.get_frames_width() + m_Style.get_frames_radius() * 0.5f;

    // retrieve controller
    ImmediateUserInterfaceMenusAndPopupsController* menusController =
        get_controller<ImmediateUserInterfaceMenusAndPopupsController>();

    if(begin_node<ImmediateUserInterfaceMenu>(_ID, ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None))
    {
        menu      = get_rendering_stack_top<ImmediateUserInterfaceMenu>();
        hasParent = m_Hierarchy.get_parent(menu) != nullptr;

        if(begin_node<ImmediateUserInterfaceMenuScrollArea>(
              next_id("InternalScrollArea"),
              ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults
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
                    next_id(_ID, "Entry"),
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
            next_content_margin(gs_vec4f(margin, margin, 0.f, 0.f));
            next_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Popup);

            if(begin_node<ImmediateUserInterfaceMenuScrollArea>(
                  next_id("ExternalScrollArea"),
                  ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent
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

                // calculate rect
                gs_2d_boxf box = menuItem->get_visible_rect(this);
                
                if(m_Hierarchy.get_parent<ImmediateUserInterfaceMenuBar>(menu))
                {
                    menu->ExternalScrollArea->State.BoundingBox = gs_2d_boxf(
                        gs_vec2f(box.Min.x, box.Max.y),
                        gs_vec2f(box.Min.x, box.Max.y) + menu->ExternalScrollArea->State.BoundingBox.size());
                }
                else
                {
                    menu->ExternalScrollArea->State.BoundingBox = gs_2d_boxf(
                        gs_vec2f(box.Max.x, box.Min.y),
                        gs_vec2f(box.Max.x, box.Min.y) + menu->ExternalScrollArea->State.BoundingBox.size());
                }

                end_node<ImmediateUserInterfaceMenuScrollArea>();
            }

            return true;
        }
        else menu->ExternalScrollArea = nullptr;

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

bool ImmediateUserInterfaceContextLayer::begin_menubar(const std::string& _ID)
{
    return begin_node<ImmediateUserInterfaceMenuBar>(
        _ID,
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
        | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically);
}

void ImmediateUserInterfaceContextLayer::end_menubar()
{
    end_node<ImmediateUserInterfaceMenuBar>();
}

bool ImmediateUserInterfaceContextLayer::begin_window(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, bool* _Opened)
{
    return begin_node<ImmediateUserInterfaceWindow>(_ID, _Settings, _Opened);
}

void ImmediateUserInterfaceContextLayer::end_window()
{
    end_node<ImmediateUserInterfaceWindow>();
}

bool ImmediateUserInterfaceContextLayer::begin_dialog(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings, bool* _Opened)
{
    int settings = _Settings;
    settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent;

    next_rendering_order(ImmediateUserInterfaceRenderingOrder_::ImmediateUserInterfaceRenderingOrder_Modal);

    if(begin_node<ImmediateUserInterfaceDialog>(_ID, settings, _Opened))
    {
        get_rendering_stack_top<ImmediateUserInterfaceDialog>()->Opened = _Opened;
        return true;
    }

    return false;
}

void ImmediateUserInterfaceContextLayer::end_dialog()
{
    end_node<ImmediateUserInterfaceDialog>();
}

float ImmediateUserInterfaceContextLayer::get_text_line_height()
{
    return ImmediateUserInterfaceContextLayerHelpers::get_text_line_height(this);
}

gs_vec4f ImmediateUserInterfaceContextLayer::get_content_default_margin()
{
    return gs_vec4f(
        m_Style.get_frames_width() + m_Style.get_frames_radius() * 0.5f,
        m_Style.get_frames_width() + m_Style.get_frames_radius() * 0.5f,
        0.f,
        0.f);
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

void ImmediateUserInterfaceContextLayer::next_style(const ImmediateUserInterfaceStyle& _Style)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextStyle = _Style;
}

void ImmediateUserInterfaceContextLayer::next_rendering_order(const ImmediateUserInterfaceRenderingOrder& _Order)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextRenderingOrder = _Order;
}

void ImmediateUserInterfaceContextLayer::next_axis_scale(const gs_vec2f& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextAxisScale = _Value;
}

void ImmediateUserInterfaceContextLayer::next_axis_offset(const gs_vec2f& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextAxisOffset = _Value;
}

void ImmediateUserInterfaceContextLayer::next_line()
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextLine = controller->NextLine.has_value() ? controller->NextLine.value() + 1 : 1;
}

void ImmediateUserInterfaceContextLayer::same_line()
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextLine = 0;
}

void ImmediateUserInterfaceContextLayer::indent(const float& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextIndent = controller->NextIndent.has_value() ? controller->NextIndent.value() + _Value : _Value;
}

void ImmediateUserInterfaceContextLayer::next_width(const float& _Value)
{
    next_minimum_width(_Value);
    next_maximum_width(_Value);
}

void ImmediateUserInterfaceContextLayer::next_height(const float& _Value)
{
    next_minimum_height(_Value);
    next_maximum_height(_Value);
}

void ImmediateUserInterfaceContextLayer::next_size(const gs_vec2f& _Value)
{
    next_minimum_size(_Value);
    next_maximum_size(_Value);
}

void ImmediateUserInterfaceContextLayer::next_position(const gs_vec2f& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextPosition = _Value;
}

void ImmediateUserInterfaceContextLayer::next_minimum_width(const float& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextMinimumWidth = _Value;
}

void ImmediateUserInterfaceContextLayer::next_minimum_height(const float& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextMinimumHeight = _Value;
}

void ImmediateUserInterfaceContextLayer::next_minimum_size(const gs_vec2f& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller == nullptr)
        return;
        
    controller->NextMinimumWidth  = _Value.x;
    controller->NextMinimumHeight = _Value.y;
}

void ImmediateUserInterfaceContextLayer::next_maximum_width(const float& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextMaximumWidth = _Value;
}

void ImmediateUserInterfaceContextLayer::next_maximum_height(const float& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextMaximumHeight = _Value;
}

void ImmediateUserInterfaceContextLayer::next_maximum_size(const gs_vec2f& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller == nullptr)
        return;
        
    controller->NextMaximumWidth  = _Value.x;
    controller->NextMaximumHeight = _Value.y;
}

void ImmediateUserInterfaceContextLayer::next_content_margin(const gs_vec4f& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextContentMargin = _Value;
}

void ImmediateUserInterfaceContextLayer::next_content_padding(const gs_vec4f& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextContentPadding = _Value;
}

void ImmediateUserInterfaceContextLayer::next_scroll_offset(const gs_vec2f& _Value)
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextScrollOffset = _Value;
}

void ImmediateUserInterfaceContextLayer::next_order_in_follow()
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextOrderInFollow = true;
}

void ImmediateUserInterfaceContextLayer::next_order_in_parallel()
{
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->NextOrderInFollow = false;
}

gs_vec2f ImmediateUserInterfaceContextLayer::current_local_position(const gs_vec2f& _Position, const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return node != nullptr ? _Position - node->State.BoundingBox.Min : _Position;
}

gs_vec2f ImmediateUserInterfaceContextLayer::current_world_position(const gs_vec2f& _Position, const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return node != nullptr ? _Position + node->State.BoundingBox.Min : _Position;
}

gs_2d_boxf ImmediateUserInterfaceContextLayer::current_bounding_box(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return node != nullptr ? node->State.BoundingBox : gs_2d_boxf(gs_vec2f(0.f, 0.f), gs_vec2f(0.f, 0.f));
}

gs_vec2f ImmediateUserInterfaceContextLayer::current_maximum_size(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return node != nullptr ? node->State.MaximumSize : gs_vec2f(0.f, 0.f);
}

gs_vec2f ImmediateUserInterfaceContextLayer::current_minimum_size(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return node != nullptr ? node->State.MinimumSize : gs_vec2f(0.f, 0.f);
}

int ImmediateUserInterfaceContextLayer::current_canvas_depth(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceCanvas* node =
        _Node != nullptr ? dynamic_cast<const ImmediateUserInterfaceCanvas*>(_Node) : get_rendering_stack_top<ImmediateUserInterfaceCanvas>();

    return node != nullptr ? node->CurrentDepth : 0;
}

int ImmediateUserInterfaceContextLayer::current_canvas_place_in_follow(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceCanvas* node =
        _Node != nullptr ? dynamic_cast<const ImmediateUserInterfaceCanvas*>(_Node) : get_rendering_stack_top<ImmediateUserInterfaceCanvas>();
    
    return node != nullptr ? ++node->CurrentDepth : 0;
}

gs_vec2f ImmediateUserInterfaceContextLayer::current_scroll_offset(const ImmediateUserInterfaceNode* _Node, const bool& _Scaled) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    const ImmediateUserInterfaceScrollArea* scrollArea =
        dynamic_cast<const ImmediateUserInterfaceScrollArea*>(node);

    return scrollArea != nullptr ? scrollArea->get_scroll_offset(_Scaled) : gs_vec2f(0.f, 0.f);
}

ImmediateUserInterfaceVerticalClipper ImmediateUserInterfaceContextLayer::current_vertical_clipper(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    const ImmediateUserInterfaceTable* table =
        dynamic_cast<const ImmediateUserInterfaceTable*>(node);

    return table != nullptr ? table->VerticalClipper : ImmediateUserInterfaceVerticalClipper();
}

ImmediateUserInterfaceHorizontalClipper ImmediateUserInterfaceContextLayer::current_horizontal_clipper(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    const ImmediateUserInterfaceTable* table =
        dynamic_cast<const ImmediateUserInterfaceTable*>(node);

    return table != nullptr ? table->HorizontalClipper : ImmediateUserInterfaceHorizontalClipper();
}

bool ImmediateUserInterfaceContextLayer::does_current_node_has_modifier(const ApplicationPlatformBackendKeyModifier::Modifier& _Modifier, const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return
        node != nullptr &&
        (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        m_Input.has_modifier(_Modifier);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_hovered(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return node != nullptr && (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_down(const ApplicationPlatformBackendMouseButton::Button& _Button, const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return
         node != nullptr &&
        (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        m_Input.is_mouse_button_down(_Button);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_down(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    for (
        size_t i = ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonBegin;
               i < ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
               i++)
    {
        if(is_current_node_mouse_down((ApplicationPlatformBackendMouseButton::Button)i, node))
            return true;
    }
    
    return false;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button, const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return
         node != nullptr &&
        (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        m_Input.is_mouse_button_pressed(_Button);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_pressed(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    for (
        size_t i = ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonBegin;
               i < ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
               i++)
    {
        if(is_current_node_mouse_pressed((ApplicationPlatformBackendMouseButton::Button)i, node))
            return true;
    }
    
    return false;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_released(const ApplicationPlatformBackendMouseButton::Button& _Button, const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return
         node != nullptr &&
        (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        m_Input.is_mouse_button_released(_Button);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_released(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    for (
        size_t i = ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonBegin;
               i < ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
               i++)
    {
        if(is_current_node_mouse_released((ApplicationPlatformBackendMouseButton::Button)i, node))
            return true;
    }
    
    return false;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button, const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return
         node != nullptr &&
        (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        m_Input.is_mouse_button_clicked(_Button);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_clicked(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    for (
        size_t i = ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonBegin;
               i < ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
               i++)
    {
        if(is_current_node_mouse_clicked((ApplicationPlatformBackendMouseButton::Button)i, node))
            return true;
    }
    
    return false;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button, const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return
         node != nullptr &&
        (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        m_Input.is_mouse_button_double_clicked(_Button);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_mouse_double_clicked(const ImmediateUserInterfaceNode* _Node) const
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    for (
        size_t i = ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonBegin;
               i < ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
               i++)
    {
        if(is_current_node_mouse_double_clicked((ApplicationPlatformBackendMouseButton::Button)i, node))
            return true;
    }
    
    return false;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_key_pressed(const ApplicationPlatformBackendKey::Key& _Key, const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return
         node != nullptr &&
        (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        m_Input.is_key_pressed(_Key);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_key_pressed(const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    for (
        size_t i = ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_BEGIN;
               i < ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_END;
               i++)
    {
        if(is_current_node_key_pressed((ApplicationPlatformBackendKey::Key)i, node))
            return true;
    }
    
    return false;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_key_clicked(const ApplicationPlatformBackendKey::Key& _Key, const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return
         node != nullptr &&
        (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        m_Input.is_key_clicked(_Key);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_key_clicked(const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    for (
        size_t i = ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_BEGIN;
               i < ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_END;
               i++)
    {
        if(is_current_node_key_clicked((ApplicationPlatformBackendKey::Key)i, node))
            return true;
    }
    
    return false;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_key_down(const ApplicationPlatformBackendKey::Key& _Key, const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return
         node != nullptr &&
        (node->State.MouseHover & ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_MouseHovered) &&
        m_Input.is_key_down(_Key);
}

bool ImmediateUserInterfaceContextLayer::is_current_node_key_down(const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    for (
        size_t i = ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_BEGIN;
               i < ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_END;
               i++)
    {
        if(is_current_node_key_down((ApplicationPlatformBackendKey::Key)i, node))
            return true;
    }
    
    return false;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_panel(const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return dynamic_cast<const ImmediateUserInterfacePanel*>(node) != nullptr;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_scrollarea(const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return dynamic_cast<const ImmediateUserInterfaceScrollArea*>(node) != nullptr;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_vertical_stack(const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return dynamic_cast<const ImmediateUserInterfaceVerticalStack*>(node) != nullptr;
}

bool ImmediateUserInterfaceContextLayer::is_current_node_horizontal_stack(const ImmediateUserInterfaceNode* _Node)
{
    const ImmediateUserInterfaceNode* node =
        _Node != nullptr ? _Node : get_rendering_stack_top();

    return dynamic_cast<const ImmediateUserInterfaceHorizontalStack*>(node) != nullptr;
}

bool ImmediateUserInterfaceContextLayer::does_node_exist(const std::string& _Name, const std::string& _Hash)
{
    push_id(next_id(_Name, _Hash));

    ImmediateUserInterfaceNode* node =
        m_Cache.find(m_CurrentHash) != m_Cache.end() ?
            m_Cache[m_CurrentHash].get() :
                nullptr;

    return node != nullptr && node->Count >= 1;
}

void ImmediateUserInterfaceContextLayer::drag(const std::any& _Data, const std::function<void(const std::any&, const gs_2d_boxf&, const int&)>& _Preview)
{
    ImmediateUserInterfaceDragAndDropController* controller =
        get_controller<ImmediateUserInterfaceDragAndDropController>();

    if(controller != nullptr && is_current_node_mouse_pressed((get_rendered_stack_top() ? get_rendered_stack_top() : get_rendering_stack_top())))
        controller->push_data(_Data, _Preview);
}

bool ImmediateUserInterfaceContextLayer::dragging() const
{
    ImmediateUserInterfaceDragAndDropController* controller =
        get_controller<ImmediateUserInterfaceDragAndDropController>();

    return controller != nullptr && controller->pop_data().has_value();
}

void ImmediateUserInterfaceContextLayer::clear_cache()
{
    ImmediateUserInterfaceCommandsQueueController* controller =
        get_controller<ImmediateUserInterfaceCommandsQueueController>();

    if(controller == nullptr)
        return;

    controller->push([this](){save_state_ini_file();});
    controller->push([this](){m_Cache.clear();});
}

void ImmediateUserInterfaceContextLayer::save_state_ini_file()
{
    // save widgets state to .ini file
    for (auto node : m_NodesRenderingList)
        node->save_state(this);

    // save style settings to .ini file
    if(m_Settings & ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_SaveStyleSettingsToIniFile)
    {
        // save color scheme
        for (int color = ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Begin;
                    color < ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End;
                    color++)
        {
            m_IniFileState.set(
                "Style",
                m_Style.style_color_to_string((ImmediateUserInterfaceNodeColors_)color, true),
                m_Style.get_color((ImmediateUserInterfaceNodeColors_)color));
        }
        
        // save geometry settings
        m_IniFileState.set("Style", "FontSize", m_Style.get_font_size());
        m_IniFileState.set("Style", "FramesWidth", m_Style.get_frames_width());
        m_IniFileState.set("Style", "FramesRadius", m_Style.get_frames_radius());
        m_IniFileState.set("Style", "ScrollbarWidth", m_Style.get_scrollbar_width());
    }

    // save .ini file
    m_IniFileState.write(m_IniFilePath);
}

void ImmediateUserInterfaceContextLayer::load_state_ini_file()
{
    if(m_IniFileState.empty()) return;

    // load color scheme
    for (int color = ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_Begin;
                color < ImmediateUserInterfaceNodeColors_::ImmediateUserInterfaceNodeColors_End;
                color++)
    {
        if(m_IniFileState.contains("Style", m_Style.style_color_to_string((ImmediateUserInterfaceNodeColors_)color, true)))
        {
            m_Style.get_color((ImmediateUserInterfaceNodeColors_)color) =
                m_IniFileState.get<gs_color>("Style", m_Style.style_color_to_string((ImmediateUserInterfaceNodeColors_)color, true));
        }
    }

    // load geometry settings
    if(m_IniFileState.contains("Style", "FontSize"))
        m_Style.get_font_size() = m_IniFileState.get<float>("Style", "FontSize");

    if(m_IniFileState.contains("Style", "FramesWidth"))
        m_Style.get_frames_width() = m_IniFileState.get<float>("Style", "FramesWidth");

    if(m_IniFileState.contains("Style", "FramesRadius"))
        m_Style.get_frames_radius() = m_IniFileState.get<float>("Style", "FramesRadius");

    if(m_IniFileState.contains("Style", "ScrollbarWidth"))
        m_Style.get_scrollbar_width() = m_IniFileState.get<float>("Style", "ScrollbarWidth");
}

std::any ImmediateUserInterfaceContextLayer::drop() const
{
    ImmediateUserInterfaceDragAndDropController* controller =
        get_controller<ImmediateUserInterfaceDragAndDropController>();

    return controller != nullptr && m_Input.is_mouse_button_released() ? controller->pop_data() : std::any();
}

void ImmediateUserInterfaceContextLayer::setup_created_node(ImmediateUserInterfaceNode* _Node, const ImmediateUserInterfaceNodeSettings& _Settings)
{
    if(_Node == nullptr)
        return;

    // setup node parameters
    _Node->State.Settings       = _Settings;
    _Node->State.RenderingIndex = (int)m_NodesRenderingList.size();

    // build nodes hierarchy
    if(!m_NodesRenderingStack.empty())
    {
        if(!(_Node->State.Settings & ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NullParent))
            m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]->attach_child(_Node);
        _Node->State.Scope = m_NodesRenderingStack[m_NodesRenderingStack.size() - 1];
    }

    // setup next rendered node parameters
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller == nullptr)
        return;

    // next line
    if(!m_NodesRenderedStack.empty() && controller->NextLine.has_value())
        m_NodesRenderedStack[m_NodesRenderedStack.size() - 1]->State.NextLine = controller->NextLine.value();

    // next indent
    if(!m_NodesRenderedStack.empty() && controller->NextIndent.has_value())
        m_NodesRenderedStack[m_NodesRenderedStack.size() - 1]->State.Indent = controller->NextIndent.value();

    // next minimum width
    if(controller->NextMinimumWidth.has_value())
        _Node->State.MinimumSize = gs_vec2f(controller->NextMinimumWidth.value(), _Node->State.MinimumSize.y);

    // next minimum height
    if(controller->NextMinimumHeight.has_value())
        _Node->State.MinimumSize = gs_vec2f(_Node->State.MinimumSize.x, controller->NextMinimumHeight.value());

    // next maximum width
    if(controller->NextMaximumWidth.has_value())
        _Node->State.MaximumSize = gs_vec2f(controller->NextMaximumWidth.value(), _Node->State.MaximumSize.y);

    // next maximum height
    if(controller->NextMaximumHeight.has_value())
        _Node->State.MaximumSize = gs_vec2f(_Node->State.MaximumSize.x, controller->NextMaximumHeight.value());

    // next position
    if(controller->NextPosition.has_value())
    {
        _Node->State.BoundingBox = gs_2d_boxf(
            controller->NextPosition.value(),
            controller->NextPosition.value() + gs_clamp(_Node->State.BoundingBox.size(), _Node->State.MinimumSize, _Node->State.MaximumSize));
    }

    // next rendering order
    if(controller->NextOrderInFollow.has_value())
        _Node->State.PlaceInFollow = controller->NextOrderInFollow.value();

    // next content margin
    if(dynamic_cast<ImmediateUserInterfacePanel*>(_Node) && controller->NextContentMargin.has_value())
        dynamic_cast<ImmediateUserInterfacePanel*>(_Node)->ContentMargin = controller->NextContentMargin.value();

    // next content padding
    if(dynamic_cast<ImmediateUserInterfacePanel*>(_Node) && controller->NextContentPadding.has_value())
        dynamic_cast<ImmediateUserInterfacePanel*>(_Node)->ContentPadding = controller->NextContentPadding.value();

    // next scroll offset
    if(dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Node) && controller->NextScrollOffset.has_value())
    {
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Node)->set_horizontal_scroll_offset(gs_vec2f(controller->NextScrollOffset.value().x, 0.f), false);
        dynamic_cast<ImmediateUserInterfaceScrollArea*>(_Node)->set_vertical_scroll_offset(gs_vec2f(0.f, controller->NextScrollOffset.value().y), false);
    }

    // reset next plot axis scale
    if(dynamic_cast<ImmediateUserInterfacePlotAxis*>(_Node) && controller->NextAxisScale.has_value())
        dynamic_cast<ImmediateUserInterfacePlotAxis*>(_Node)->ZoomScale = controller->NextAxisScale.value();

    // reset next plot axis offset
    if(dynamic_cast<ImmediateUserInterfacePlotAxis*>(_Node) && controller->NextAxisOffset.has_value())
        dynamic_cast<ImmediateUserInterfacePlotAxis*>(_Node)->CurrentOffset = controller->NextAxisOffset.value();

    // next rendering order
    if(controller->NextRenderingOrder.has_value())
        _Node->NextRenderingOrder = controller->NextRenderingOrder.value();

    if(controller->NextStyle.has_value())
        _Node->NextStyle = controller->NextStyle.value();

    // save style backup
    if(_Node->NextStyle.has_value())
    {
        m_StyleBackups.push_back(m_Style);
        m_Style = _Node->NextStyle.value();
    }
    else
    {
        m_StyleBackups.push_back(std::optional<ImmediateUserInterfaceStyle>());
    }

    // reset next item controller
    controller->reset();
}

void ImmediateUserInterfaceContextLayer::restore_created_node()
{
    // reset next node controller
    ImmediateUserInterfaceNextNodeController* controller =
        get_controller<ImmediateUserInterfaceNextNodeController>();

    if(controller != nullptr)
        controller->reset();

    // restore style
    if(!m_StyleBackups.empty())
    {
        if(m_StyleBackups[m_StyleBackups.size() - 1].has_value())
            m_Style = m_StyleBackups[m_StyleBackups.size() - 1].value();
        m_StyleBackups.pop_back();
    }
}