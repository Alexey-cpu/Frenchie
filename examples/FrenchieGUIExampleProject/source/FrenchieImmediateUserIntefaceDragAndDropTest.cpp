#include <FrenchieImmediateUserIntefaceDragAndDropTest.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;

FrenchieImmediateUserIntefaceDragAndDropTest::FrenchieImmediateUserIntefaceDragAndDropTest() :
    Frenchie::Application::Layer(STRINGIFY(FrenchieImmediateUserIntefaceDragAndDropTest)){}

FrenchieImmediateUserIntefaceDragAndDropTest::~FrenchieImmediateUserIntefaceDragAndDropTest(){}

bool FrenchieImmediateUserIntefaceDragAndDropTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return m_UI != nullptr;
}

void FrenchieImmediateUserIntefaceDragAndDropTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("DragAndDropTesting", "Drag and Drop testing"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        m_UI->next_content_padding(gs_vec4f(32.f, 32.f, 0.f, 0.f));

        if(m_UI->begin_horizontal_stack(
            m_UI->next_id("HorizontalStack"),
              ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
        {
            m_UI->next_content_padding(gs_vec4f(32.f, 32.f, 0.f, 0.f));

            if(m_UI->begin_grid(
                m_UI->next_id("Colors"),
                  ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter))
            {
                if(m_UI->begin_grid_place(0, 0))
                {
                    m_UI->image(m_UI->next_id("Color-1"), gs_color_rgb(255, 0, 0));

                    m_UI->drag(
                        gs_color_rgb(255, 0, 0),
                        [this](const std::any& _Color, const gs_2d_boxf& _Box, const int& _Depth)
                        {
                            m_UI->renderer()->push_rectangle_filled(
                                _Box.Min,
                                _Box.Max,
                                gs_color_rgb(0, 0, 0),
                                m_UI->renderer()->calculate_transform_matrix((float)_Depth));

                            m_UI->renderer()->push_rectangle_filled(
                                _Box.Min + 4.f,
                                _Box.Max - 4.f,
                                std::any_cast<gs_color>(_Color),
                                m_UI->renderer()->calculate_transform_matrix((float)(_Depth + 1)));
                        }
                    );

                    m_UI->end_grid_place();
                }

                if(m_UI->begin_grid_place(0, 1))
                {
                    m_UI->image(m_UI->next_id("Color-2"), gs_color_rgb(0, 255, 0));

                    m_UI->drag(
                        gs_color_rgb(0, 255, 0),
                        [this](const std::any& _Color, const gs_2d_boxf& _Box, const int& _Depth)
                        {
                            m_UI->renderer()->push_rectangle_filled(
                                _Box.Min,
                                _Box.Max,
                                gs_color_rgb(0, 0, 0),
                                m_UI->renderer()->calculate_transform_matrix((float)_Depth));

                            m_UI->renderer()->push_rectangle_filled(
                                _Box.Min + 4.f,
                                _Box.Max - 4.f,
                                std::any_cast<gs_color>(_Color),
                                m_UI->renderer()->calculate_transform_matrix((float)(_Depth + 1)));
                        }
                    );
                
                    m_UI->end_grid_place();
                }

                if(m_UI->begin_grid_place(1, 0))
                {
                    m_UI->image(m_UI->next_id("Color-3"), gs_color_rgb(0, 0, 255));

                    m_UI->drag(
                        gs_color_rgb(0, 0, 255),
                        [this](const std::any& _Color, const gs_2d_boxf& _Box, const int& _Depth)
                        {
                            m_UI->renderer()->push_rectangle_filled(
                                _Box.Min,
                                _Box.Max,
                                gs_color_rgb(0, 0, 0),
                                m_UI->renderer()->calculate_transform_matrix((float)_Depth));

                            m_UI->renderer()->push_rectangle_filled(
                                _Box.Min + 4.f,
                                _Box.Max - 4.f,
                                std::any_cast<gs_color>(_Color),
                                m_UI->renderer()->calculate_transform_matrix((float)(_Depth + 1)));
                        }
                    );

                    m_UI->end_grid_place();
                }

                if(m_UI->begin_grid_place(1, 1))
                {
                    m_UI->image(m_UI->next_id("Color-4"), gs_color_rgb(255, 0, 255));

                    m_UI->drag(
                        gs_color_rgb(255, 0, 255),
                        [this](const std::any& _Color, const gs_2d_boxf& _Box, const int& _Depth)
                        {
                            m_UI->renderer()->push_rectangle_filled(
                                _Box.Min,
                                _Box.Max,
                                gs_color_rgb(0, 0, 0),
                                m_UI->renderer()->calculate_transform_matrix((float)_Depth));

                            m_UI->renderer()->push_rectangle_filled(
                                _Box.Min + 4.f,
                                _Box.Max - 4.f,
                                std::any_cast<gs_color>(_Color),
                                m_UI->renderer()->calculate_transform_matrix((float)(_Depth + 1)));
                        }
                    );

                    m_UI->end_grid_place();
                }

                m_UI->end_grid();
            }

            m_UI->custom_widget(
                m_UI->next_id("Canvas"),
                [](ImmediateUserInterfaceContextLayer*, ImmediateUserInterfaceNode*){},
                [](ImmediateUserInterfaceContextLayer*, ImmediateUserInterfaceNode*){},
                [this](ImmediateUserInterfaceContextLayer* _Context, ImmediateUserInterfaceNode*)
                {
                    _Context->renderer()->push_rectangle_filled(
                        _Context->current_bounding_box().Min,
                        _Context->current_bounding_box().Max,
                        std::any_cast<gs_color>(m_CanvasColor),
                        _Context->renderer()->calculate_transform_matrix((float)_Context->current_depth()),
                        _Context->style().get_frames_radius());
                
                    std::any dropData = _Context->drop();

                    if(dropData.has_value())
                    {
                        try
                        {
                            m_CanvasColor = std::any_cast<gs_color>(dropData);
                        }
                        catch(...)
                        {
                        }
                    }
                });

            m_UI->end_horizontal_stack();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserIntefaceDragAndDropTest::allows_multiple_instances() const
{
    return false;
}