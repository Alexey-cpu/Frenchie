#include <FrenchieApplicationLayerRenderingQueue2D.hpp>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        class FrenchieApplicationLayerRenderingQueue2DHelpers
        {
        public:
            static int get_tessellated_segments_count(const float& _Radius, const float& _TesselationTolerance)
            {
                float radius     = gs_clamp(gs_abs(_Radius), 8.f, 2048.f);
                float tollerance = gs_clamp(gs_abs(_TesselationTolerance), 0.001f, gs_huge<float>());

                return PI2 * radius / 2.f / sqrtf( 2.f * radius * _TesselationTolerance - _TesselationTolerance * _TesselationTolerance);
            }
        };
    }
}

// RenderingQueue2D
RenderingQueue2D::RenderingQueue2D() : RenderingQueue(STRINGIFY(RenderingQueue2D)){}
RenderingQueue2D::~RenderingQueue2D(){}

void RenderingQueue2D::frame_finish()
{
    // clean-up self
    m_TriangulationIndexes.clear();

    // call base implementation
    RenderingQueue::frame_finish();
}

gs_mat4f RenderingQueue2D::calculate_transform_matrix(const float& _Depth, const gs_vec2f& _Position, const float& _Rotation, const gs_vec2f& _Scale)
{
    return Frenchie::Application::ApplicationRenderingBackend::calculate_2d_transform_matrix(_Depth, _Position, _Rotation, _Scale);
}

void RenderingQueue2D::build_poly_mesh_filled(const gs_vec2f _Points[], const gs_color _Colors[], gs_vec2f _UVs[], const int& _Count)
{
    // assert
    GS_ASSERT(_Count >= 3);

    // auxilaty lambdas
    auto clampIndex = [](const int& _Index, const int& _Size)->int
    {
        int index = _Index;
        while (index < 0     ) index += _Size;
        while (index >= _Size) index -= _Size;
        return index;
    };

    // determine bounding box and orientation
    gs_2dboxf                           polygonBoundingBox        = gs_2dboxf(_Points[0], _Points[0]);
    gs_color                            polygonCentralColor       = 0;
    Frenchie::Core::Optional<gs_2dboxf> polygonTextureBox         = _UVs == nullptr ? Frenchie::Core::Optional<gs_2dboxf>() : gs_2dboxf(_UVs[0], _UVs[0]);
    bool                                isPolygonConvex           = true;
    bool                                isPolygonCounterClockWise = gs_2D_polygon_signed_area(_Points, _Count) < 0.f;

    gs_color red   = 0;
    gs_color green = 0;
    gs_color blue  = 0;
    gs_color alpha = 0;

    for (int i = 0; i < _Count; i++)
    {
        polygonBoundingBox = gs_2dboxf(polygonBoundingBox.Min, polygonBoundingBox.Max, _Points[i]);
        
        red   += gs_color_rgba_get_r(_Colors[i]);
        green += gs_color_rgba_get_g(_Colors[i]);
        blue  += gs_color_rgba_get_b(_Colors[i]);
        alpha += gs_color_rgba_get_a(_Colors[i]);

        if(polygonTextureBox.has_value())
            polygonTextureBox = gs_2dboxf(polygonTextureBox.value().Min, polygonTextureBox.value().Max, _UVs[i]);
    }

    polygonCentralColor = gs_color_rgba(red / _Count, green / _Count, blue / _Count, alpha / _Count);

    for (int j = 0; j < _Count; j++)
    {
        int point1 = clampIndex(j + 0, _Count);
        int point2 = clampIndex(j - 1, _Count);
        int point3 = clampIndex(j + 1, _Count);

        if(!(isPolygonCounterClockWise ?
                    gs_vector_cross(_Points[point1] - _Points[point2], _Points[point1] - _Points[point3]) > 0.f :
                        gs_vector_cross(_Points[point1] - _Points[point3], _Points[point1] - _Points[point2]) > 0.f))
        {
            isPolygonConvex = false;
            break;
        }
    }
    
    // build convex mesh
    if(isPolygonConvex)
    {
        const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

        if(_Count < 4)
        {
            for (int i = 0; i < _Count; i++)
            {
                m_MeshVertexes.push_back(
                    ApplicationRenderingBackendMeshVertex(
                        _Points[i],
                        _UVs == nullptr ?
                            gs_vec2f((_Points[i].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (_Points[i].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()) :
                                _UVs[i],
                        _Colors[i]));
            }
        }
        else
        {
            for (int i = 0; i < _Count; i++)
            {
                m_MeshVertexes.push_back(
                    ApplicationRenderingBackendMeshVertex(
                        _Points[i],
                        _UVs == nullptr ?
                            gs_vec2f((_Points[i].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (_Points[i].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()) :
                                _UVs[i],
                        _Colors[i]));
                
                m_MeshVertexes.push_back(
                    ApplicationRenderingBackendMeshVertex(
                        _Points[(i + 1) % _Count],
                        _UVs == nullptr ?
                            gs_vec2f((_Points[(i + 1) % _Count].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (_Points[(i + 1) % _Count].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()) :
                                _UVs[(i + 1) % _Count],
                        _Colors[(i + 1) % _Count]));

                m_MeshVertexes.push_back(
                    ApplicationRenderingBackendMeshVertex(
                        polygonBoundingBox.center(),
                        !polygonTextureBox.has_value() ?
                            gs_vec2f((polygonBoundingBox.center().x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (polygonBoundingBox.center().y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()) :
                                polygonTextureBox.value().center(),
                        polygonCentralColor));
            }
        }

        for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
            m_MeshVertexesIndexes.push_back(i);

        return;
    }

    // build concave filled mesh using ear clipping algorithm
    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    m_TriangulationIndexes.clear();
    for (int i = 0; i < _Count; i++)
        m_TriangulationIndexes.push_back(i);

    for (int i = 0; (int)m_TriangulationIndexes.size() > 2 && i < _Count; i++)    
    {
        for (int j = 0; j < m_TriangulationIndexes.size(); j++)
        {                    
            int  point1   = m_TriangulationIndexes[clampIndex(j + 0, (int)m_TriangulationIndexes.size())];
            int  point2   = m_TriangulationIndexes[clampIndex(j - 1, (int)m_TriangulationIndexes.size())];
            int  point3   = m_TriangulationIndexes[clampIndex(j + 1, (int)m_TriangulationIndexes.size())];
            bool isEar    = true;
            bool isConvex = isPolygonCounterClockWise ?
                    gs_vector_cross(_Points[point1] - _Points[point2], _Points[point1] - _Points[point3]) > 0.f :
                        gs_vector_cross(_Points[point1] - _Points[point3], _Points[point1] - _Points[point2]) > 0.f;

            // check that triangle does not contain other poly points
            gs_vec2f poly[3] = {_Points[point1], _Points[point2], _Points[point3]};
            
            for (int k = 0; k < (int)m_TriangulationIndexes.size(); k++)
            {
                if(m_TriangulationIndexes[k] == point1 || m_TriangulationIndexes[k] == point2 || m_TriangulationIndexes[k] == point3)
                    continue;

                if(gs_point_in_2D_polygon(poly, 3, _Points[m_TriangulationIndexes[k]]))
                {
                    isEar = false;
                    break;
                }
            }

            if((isEar && isConvex) || m_TriangulationIndexes.size() <= 3)
            {
                // build mesh
                m_MeshVertexes.push_back(
                    ApplicationRenderingBackendMeshVertex(
                        _Points[point1],
                        _UVs == nullptr ?
                            gs_vec2f((_Points[point1].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (_Points[point1].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()) :
                                _UVs[point1],
                        _Colors[point1]));

                m_MeshVertexes.push_back(
                    ApplicationRenderingBackendMeshVertex(
                        _Points[point2],
                        _UVs == nullptr ? 
                            gs_vec2f((_Points[point2].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (_Points[point2].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height())
                                : _UVs[point2],
                        _Colors[point2]));

                m_MeshVertexes.push_back(
                    ApplicationRenderingBackendMeshVertex(
                        _Points[point3],
                        _UVs == nullptr ?
                            gs_vec2f((_Points[point3].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (_Points[point3].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()) :
                                _UVs[point3],
                        _Colors[point3]));

                // erase point
                m_TriangulationIndexes.erase(m_TriangulationIndexes.begin() + j);
                break;
            }
        }
    }

    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

#include <iostream>

void RenderingQueue2D::build_line_mesh(const gs_vec2f&  _P1, const gs_vec2f&  _P2, const float& _Width, const gs_color& _Color)
{
    float    width         = gs_max(_Width, get_minimum_line_width()) * 0.5f;
    gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
    gs_vec2f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * width;
    
    gs_vec2f points[] =
    {
        _P1 - perpendicular,
        _P2 - perpendicular,
        _P2 + perpendicular,
        _P1 + perpendicular
    };
    
    gs_color colors[] =
    {
        _Color,
        _Color,
        _Color,
        _Color
    };
    
    build_poly_mesh_filled(points, colors, nullptr, 4);
}

void RenderingQueue2D::build_triangle_filled_mesh(const gs_vec2f& _P1, const gs_vec2f& _P2, const gs_vec2f& _P3, const gs_color& _Color)
{
    gs_vec2f points[3] = {_P1, _P2, _P3};
    gs_color colors[3] = {_Color, _Color, _Color};
    build_poly_mesh_filled(points, colors, nullptr, 3);
}

void RenderingQueue2D::build_triangle_mesh(const gs_vec2f& _P1, const gs_vec2f& _P2, const gs_vec2f& _P3, const float& _Width, const gs_color& _Color)
{
    build_line_mesh(_P1, _P2, _Width, _Color);
    build_line_mesh(_P2, _P3, _Width, _Color);
    build_line_mesh(_P3, _P1, _Width, _Color);
}

void RenderingQueue2D::build_rectangle_filled_mesh(const gs_vec2f& _Min, const gs_vec2f& _Max, const gs_color& _Color, const float& _Radius)
{
    if(gs_abs(_Radius) <= get_minimum_line_width())
    {
        gs_vec2f points[4] = {gs_vec2f(_Min.x, _Min.y), gs_vec2f(_Max.x, _Min.y), gs_vec2f(_Max.x, _Max.y), gs_vec2f(_Min.x, _Max.y)};
        gs_color colors[4] = {_Color, _Color, _Color, _Color};
        build_poly_mesh_filled(points, colors, nullptr, 4);
        return;
    }

    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    gs_2dboxf box(_Min, _Max);

    const float sourceAngle   = 0.f;
    const float targetAngle   = 360.f;
    const float segmentsCount = 36.f;
    const float cornerRadius  = gs_min(gs_abs(_Radius), box.width() * 0.5f, box.height() * 0.5f);
    const float deltaAngle    = 360.f / FrenchieApplicationLayerRenderingQueue2DHelpers::get_tessellated_segments_count(cornerRadius, m_TesselationTolerance);
    const float innerWidth    = box.width() - 2 * cornerRadius;
    const float innerHeight   = box.height() - 2 * cornerRadius;

    for (float angle = sourceAngle; angle < targetAngle; angle += deltaAngle)
    {
        float a = angle;
        float b = gs_clamp(angle + deltaAngle, sourceAngle, targetAngle);

        gs_vec2f p1 = gs_vec2f(
            box.center().x + innerWidth * 0.5f * gs_sign(cos(gs_to_radians(a))),
            box.center().y + innerHeight * 0.5f * gs_sign(sin(gs_to_radians(a)))) + gs_vec2f(cos(gs_to_radians(a)), sin(gs_to_radians(a))) * cornerRadius;

        gs_vec2f p2 = gs_vec2f(
            box.center().x + innerWidth * 0.5f * gs_sign(cos(gs_to_radians(b))),
            box.center().y + innerHeight * 0.5f * gs_sign(sin(gs_to_radians(b)))) + gs_vec2f(cos(gs_to_radians(b)), sin(gs_to_radians(b))) * cornerRadius;
        
        gs_vec2f p3 = box.center();

        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                p1,
                gs_vec2f((p1.x - box.Min.x) / box.width(), (p1.y - box.Min.y) / box.height()),
                _Color));
        
        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                p2,
                gs_vec2f((p2.x - box.Min.x) / box.width(), (p2.y - box.Min.y) / box.height()),
                _Color));
        
        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                p3,
                gs_vec2f((p3.x - box.Min.x) / box.width(), (p3.y - box.Min.y) / box.height()),
                _Color));
    }

    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue2D::build_rectangle_mesh(const gs_vec2f& _Min, const gs_vec2f& _Max, const gs_color& _Color, const float& _Width, const float& _Radius)
{
    if(gs_abs(_Radius) <= get_minimum_line_width())
    {
        gs_vec2f P1 = gs_vec2f(_Min.x, _Min.y);
        gs_vec2f P2 = gs_vec2f(_Max.x, _Min.y);
        gs_vec2f P3 = gs_vec2f(_Max.x, _Max.y);
        gs_vec2f P4 = gs_vec2f(_Min.x, _Max.y);

        build_line_mesh(P1, P2, _Width, _Color);
        build_line_mesh(P2, P3, _Width, _Color);
        build_line_mesh(P3, P4, _Width, _Color);
        build_line_mesh(P4, P1, _Width, _Color);

        return;
    }

    gs_2dboxf box(_Min, _Max);

    const float sourceAngle   = 0.f;
    const float targetAngle   = 360.f;
    const float segmentsCount = 36.f;
    const float cornerRadius  = gs_min(gs_abs(_Radius), box.width() * 0.5f, box.height() * 0.5f);
    const float deltaAngle    = 360.f / FrenchieApplicationLayerRenderingQueue2DHelpers::get_tessellated_segments_count(cornerRadius, m_TesselationTolerance);
    const float innerWidth    = box.width() - 2 * cornerRadius;
    const float innerHeight   = box.height() - 2 * cornerRadius;

    for (float angle = sourceAngle; angle < targetAngle; angle += deltaAngle)
    {
        float a = angle;
        float b = gs_clamp(angle + deltaAngle, sourceAngle, targetAngle);

        gs_vec2f p1 = gs_vec2f(
            box.center().x + innerWidth * 0.5f * gs_sign(cos(gs_to_radians(a))),
            box.center().y + innerHeight * 0.5f * gs_sign(sin(gs_to_radians(a)))) + gs_vec2f(cos(gs_to_radians(a)), sin(gs_to_radians(a))) * cornerRadius;

        gs_vec2f p2 = gs_vec2f(
            box.center().x + innerWidth * 0.5f * gs_sign(cos(gs_to_radians(b))),
            box.center().y + innerHeight * 0.5f * gs_sign(sin(gs_to_radians(b)))) + gs_vec2f(cos(gs_to_radians(b)), sin(gs_to_radians(b))) * cornerRadius;

        build_line_mesh(p1, p2, _Width, _Color);
    }
}

void RenderingQueue2D::build_arc_filled_mesh(
    const gs_vec2f& _Center,
    const float&    _MinorRadius,
    const float&    _MajorRadius,
    const float&    _SourceAngle,
    const float&    _TargetAngle,
    const gs_color& _Color)
{
    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    const gs_2dboxf box        = gs_2dboxf(_Center - gs_vec2f(_MinorRadius, _MajorRadius), _Center + gs_vec2f(_MinorRadius, _MajorRadius));
    const float     deltaAngle = 360.f / FrenchieApplicationLayerRenderingQueue2DHelpers::get_tessellated_segments_count(gs_max(_MinorRadius, _MajorRadius), m_TesselationTolerance);

    for (float angle = gs_min(_SourceAngle, _TargetAngle); angle < gs_max(_SourceAngle, _TargetAngle); angle += deltaAngle)
    {
        float a = angle;
        float b = gs_clamp(angle + deltaAngle, gs_min(_SourceAngle, _TargetAngle), gs_max(_SourceAngle, _TargetAngle));

        gs_vec2f p1 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(a)), _Center.y + _MajorRadius * sin(gs_to_radians(a)));
        gs_vec2f p2 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(b)), _Center.y + _MajorRadius * sin(gs_to_radians(b)));
        gs_vec2f p3 = _Center;

        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                p1,
                gs_vec2f((p1.x - box.Min.x) / box.width(), (p1.y - box.Min.y) / box.height()),
                _Color));
        
        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                p2,
                gs_vec2f((p2.x - box.Min.x) / box.width(), (p2.y - box.Min.y) / box.height()),
                _Color));
        
        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                p3,
                gs_vec2f((p3.x - box.Min.x) / box.width(), (p3.y - box.Min.y) / box.height()),
                _Color));
    }

    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue2D::build_arc_mesh(
    const gs_vec2f& _Center,
    const float&    _MinorRadius,
    const float&    _MajorRadius,
    const float&    _SourceAngle,
    const float&    _TargetAngle,
    const float&    _Width,
    const gs_color& _Color)
{
    const float lineWidth  = gs_max(_Width, get_minimum_line_width());
    const float deltaAngle = 360.f / FrenchieApplicationLayerRenderingQueue2DHelpers::get_tessellated_segments_count(gs_max(_MinorRadius, _MajorRadius), m_TesselationTolerance);

    for (float angle = gs_min(_SourceAngle, _TargetAngle); angle < gs_max(_SourceAngle, _TargetAngle); angle += deltaAngle)
    {
        float a = angle;
        float b = gs_clamp(angle + deltaAngle, gs_min(_SourceAngle, _TargetAngle), gs_max(_SourceAngle, _TargetAngle));
        gs_vec2f p1 = _Center + gs_vec2f(cos(gs_to_radians(a)), sin(gs_to_radians(a))) * _MinorRadius;
        gs_vec2f p2 = _Center + gs_vec2f(cos(gs_to_radians(b)), sin(gs_to_radians(b))) * _MinorRadius;
        build_line_mesh(p1, p2, _Width, _Color);
    }
}

void RenderingQueue2D::build_poly_mesh(
    const gs_vec2f  _Points[],
    const gs_color  _Color,
    const int&      _Count,
    const float&    _Width)
{
    for (int i = 1; i < _Count; i++)
        build_line_mesh(_Points[i-1], _Points[i], _Width, _Color);
}

void RenderingQueue2D::push_line(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width, const gs_color& _Color, const gs_mat4f& _Transform)
{
    if(!current_clipping_box().intersects(_Transform * gs_vec4f(_P1, 0.f, 1.f), _Transform * gs_vec4f(_P2, 0.f, 1.f)))
        return;

    build_line_mesh(_P1, _P2, _Width, _Color);
    push_rendering_command(ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue2D::push_triangle_filled(
    const gs_vec2f&                           _P1,
    const gs_vec2f&                           _P2,
    const gs_vec2f&                           _P3,
    const gs_color&                           _Color,
    const gs_mat4f&                           _Transform,
    const ApplicationRenderingBackendTexture& _Texture)
{
    if( !current_clipping_box().contains(_Transform * gs_vec4f(_P1, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P2, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P3, 0.f, 1.f)))
    {
        return;
    }

    build_triangle_filled_mesh(_P1, _P2, _P3, _Color);
    push_rendering_command(!_Texture.is_null() ? _Texture : ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue2D::push_rectangle_filled(
    const gs_vec2f&                           _Min,
    const gs_vec2f&                           _Max,
    const gs_color&                           _Color,
    const gs_mat4f&                           _Transform,
    const float&                              _Radius,
    const ApplicationRenderingBackendTexture& _Texture)
{
    if(!current_clipping_box().overlaps(gs_2dboxf(_Transform * gs_vec4f(_Min, 0.f, 1.f), _Transform * gs_vec4f(_Max, 0.f, 1.f))))
        return;

    build_rectangle_filled_mesh(_Min, _Max, _Color, _Radius);
    push_rendering_command(!_Texture.is_null() ? _Texture : ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue2D::push_arc_filled(
    const gs_vec2f&                           _Center,
    const float&                              _MinorRadius,
    const float&                              _MajorRadius,
    const float&                              _SourceAngle,
    const float&                              _TargetAngle,
    const gs_color&                           _Color,
    const gs_mat4f&                           _Transform,
    const ApplicationRenderingBackendTexture& _Texture)
{
    // check that we are within viewport
    if(!current_clipping_box().overlaps(
            gs_2dboxf(
                _Transform * gs_vec4f((_Center - gs_vec2f(_MinorRadius, _MajorRadius)), 0.f, 1.f),
                _Transform * gs_vec4f((_Center + gs_vec2f(_MinorRadius, _MajorRadius)), 0.f, 1.f))))
    {
        return;
    }

    build_arc_filled_mesh(_Center, _MinorRadius, _MajorRadius, _SourceAngle, _TargetAngle, _Color);
    push_rendering_command(!_Texture.is_null() ? _Texture : ApplicationRenderingBackend::get_default_texture(), _Color, _Transform);
}

void RenderingQueue2D::push_poly_filled(
    const gs_vec2f                            _Points[],
    const gs_color                            _Colors[],
    const int&                                _Count,
    const gs_mat4f&                           _Transform,
    const ApplicationRenderingBackendTexture& _Texture)
{
    gs_2dboxf box = gs_2dboxf(_Points[0], _Points[0]);
    for (int i = 0; i < _Count; i++)
        box = gs_2dboxf(box.Min, box.Max, _Points[i], _Points[i]);

    if(!current_clipping_box().overlaps(gs_2dboxf(_Transform * gs_vec4f(box.Min, 0.f, 1.f), _Transform * gs_vec4f(box.Max, 0.f, 1.f))))
        return;

    build_poly_mesh_filled(_Points, _Colors, nullptr, _Count);

    push_rendering_command(
        _Texture.is_null() ? ApplicationRenderingBackend::get_default_texture() : _Texture,
        gs_color_rgb(255, 255, 255),
        _Transform);
}

void RenderingQueue2D::push_triangle(
    const gs_vec2f& _P1,
    const gs_vec2f& _P2,
    const gs_vec2f& _P3,
    const gs_color& _Color,
    const float&    _Width,
    const gs_mat4f& _Transform)
{
    // check if we are within viewport
    if( !current_clipping_box().contains(_Transform * gs_vec4f(_P1, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P2, 0.f, 1.f)) &&
        !current_clipping_box().contains(_Transform * gs_vec4f(_P3, 0.f, 1.f)))
    {
        return;
    }

    build_triangle_mesh(_P1, _P2, _P3, _Width, _Color);
    push_rendering_command(_Transform);
}

void RenderingQueue2D::push_rectangle(const gs_vec2f& _Min, const gs_vec2f& _Max, const gs_color& _Color, const float& _Width, const gs_mat4f& _Transform, const float& _Radius)
{
    if(!current_clipping_box().overlaps(
        gs_2dboxf(
            _Transform * gs_vec4f(_Min, 0.f, 1.f, 1.f),
            _Transform * gs_vec4f(_Max, 0.f, 1.f, 1.f))))
    {
        return;
    }

    build_rectangle_mesh(_Min, _Max, _Color, _Width, _Radius);
    push_rendering_command(_Transform);
}

void RenderingQueue2D::push_arc(
    const gs_vec2f& _Center,
    const float&    _MinorRadius,
    const float&    _MajorRadius,
    const float&    _SourceAngle,
    const float&    _TargetAngle,
    const gs_color& _Color,
    const float&    _Width,
    const gs_mat4f& _Transform)
{
    // check that we are within viewport
    if(!current_clipping_box().overlaps(
            gs_2dboxf(
                _Transform * gs_vec4f((_Center - gs_vec2f(_MinorRadius, _MajorRadius)), 0.f, 1.f),
                _Transform * gs_vec4f((_Center + gs_vec2f(_MinorRadius, _MajorRadius)), 0.f, 1.f))))
    {
        return;
    }

    build_arc_mesh(_Center, _MinorRadius, _MajorRadius, _SourceAngle, _TargetAngle, _Width, _Color);
    push_rendering_command(_Transform);
}

void RenderingQueue2D::push_poly(
    const gs_vec2f  _Points[],
    const gs_color  _Color,
    const int&      _Count,
    const float&    _Width,
    const gs_mat4f& _Transform)
{
    gs_2dboxf box = gs_2dboxf(_Points[0], _Points[0]);
    for (int i = 0; i < _Count; i++)
        box = gs_2dboxf(box.Min, box.Max, _Points[i], _Points[i]);

    if(!current_clipping_box().overlaps(gs_2dboxf(_Transform * gs_vec4f(box.Min, 0.f, 1.f), _Transform * gs_vec4f(box.Max, 0.f, 1.f))))
        return;

    build_poly_mesh(_Points, _Color, _Count, _Width);

    push_rendering_command(
        ApplicationRenderingBackend::get_default_texture(),
        gs_color_rgb(255, 255, 255),
        _Transform);
}