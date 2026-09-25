#include <FrenchieImmediateUserInterfaceWidgetsTablesTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceTablesTest::FrenchieImmediateUserInterfaceTablesTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceTablesTest)){}
FrenchieImmediateUserInterfaceTablesTest::~FrenchieImmediateUserInterfaceTablesTest(){}

bool FrenchieImmediateUserInterfaceTablesTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();
    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceTablesTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Tables test window", "TablesTestWindow"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        // settings
        if(m_UI->begin_scrollarea(
            m_UI->next_id("Settings"),
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically))
        {
            m_UI->check_box(m_UI->next_id("RenderCornerHeaderCheckbox"), m_RenderCornerHeader);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("RenderCornerHeaderLabel"), "Render corner header");
            
            m_UI->check_box(m_UI->next_id("RenderColumnHeadersCheckbox"), m_RenderColumnHeaders);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("RenderColumnHeadersLabel"), "Render column headers");

            m_UI->check_box(m_UI->next_id("RenderRowHeadersCheckbox"), m_RenderRowHeaders);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("RenderRowHeadersLabel"), "Render row headers");

            m_UI->input_scalar(m_UI->next_id("RowsCountInput"), m_RowsCount, 1, 10000);
            m_UI->same_line();
            m_UI->input_scalar_slider(m_UI->next_id("RowsCountSlider"), m_RowsCount, 1, 10000);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("RowsCountLabel"), "Rows count");

            m_UI->input_scalar(m_UI->next_id("ColsCountInput"), m_ColumnsCount, 1, 10000);
            m_UI->same_line();
            m_UI->input_scalar_slider(m_UI->next_id("ColsCountSlider"), m_ColumnsCount, 1, 10000);
            m_UI->same_line();
            m_UI->label(m_UI->next_id("ColsCountLabel"), "Cols count");

            m_UI->end_scrollarea();
        }

        // table
        if(m_UI->begin_table(m_UI->next_id("Table"), m_RowsCount, m_ColumnsCount, gs_vec2f(256.f, m_UI->get_text_line_height())))
        {
            // retrieve clipper
            auto rowClipper = m_UI->current_vertical_clipper(m_UI->get_rendering_stack_top());
            auto colClipper = m_UI->current_horizontal_clipper(m_UI->get_rendering_stack_top());

            // corner title
            if(m_RenderCornerHeader)
            {
                if(m_UI->begin_table_corner_header(
                    ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
                    | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
                {
                    m_UI->image(
                        m_UI->next_id("Background"),
                        gs_color_rgb(128.f, 64.f, 64.f));

                    m_UI->label(
                        m_UI->next_id("Label"),
                        Frenchie::Core::String::format("%d x %d", m_RowsCount, m_ColumnsCount),
                        ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter);
                    m_UI->end_table_corner_header();
                }
            }

            // column titles
            if(m_RenderColumnHeaders)
            {
                for (int j = colClipper.SourceElement; j < colClipper.TargetElement; ++j)
                {
                    if(m_UI->begin_table_column_header(j,
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
                        | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
                    {
                        m_UI->image(
                            m_UI->next_id("Background"),
                            j % 2 == 0 ? gs_color_rgb(64.f, 64.f, 64.f) : gs_color_rgb(128.f, 128.f, 128.f));

                        m_UI->label(
                            m_UI->next_id("Label"),
                            Frenchie::Core::String::to_string(j),
                            ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter);

                        m_UI->end_table_column_header();
                    }
                }
            }

            // row titles
            if(m_RenderRowHeaders)
            {
                for (int i = rowClipper.SourceElement; i < rowClipper.TargetElement; ++i)
                {
                    if(m_UI->begin_table_row_header(i,
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
                        | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
                    {
                        m_UI->image(
                            m_UI->next_id("Background"),
                            i % 2 == 0 ? gs_color_rgb(64.f, 64.f, 64.f) : gs_color_rgb(128.f, 128.f, 128.f));
                        
                        m_UI->label(
                            m_UI->next_id("Label"),
                            Frenchie::Core::String::to_string(i),
                            ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter);

                        m_UI->end_table_row_header();
                    }
                }
            }

            // data cells
            for (int i = rowClipper.SourceElement; i < rowClipper.TargetElement; ++i)
            {
                for (int j = colClipper.SourceElement; j < colClipper.TargetElement; ++j)
                {
                    if(m_UI->begin_table_data_cell(
                        i,
                        j,
                        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
                        | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
                    {
                        m_UI->input_string_singleline(m_UI->next_id("Data"), m_TableDataCellString);
                        
                        m_UI->end_table_data_cell();
                    }
                }
            }
            

            m_UI->end_table();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterfaceTablesTest::allows_multiple_instances() const
{
    return false;
}