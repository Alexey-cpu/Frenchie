#include <FrenchieApplicationLayerRenderingQueue2D.hpp>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        struct RenderingQueue2DPathSegment
        {
        public:
            RenderingQueue2DPathSegment(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width = 4.f);
            void setup(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width);
            void setup(const float& _Width);

            gs_vec2f P1;
            gs_vec2f P1min;
            gs_vec2f P1max;

            gs_vec2f P2;
            gs_vec2f P2min;
            gs_vec2f P2max;

            int      Index{0};
        };

        struct RenderingQueue2DPathBuilder
        {
        public:

            RenderingQueue2DPathBuilder(const float& _PolygonLinesWidth);
            ~RenderingQueue2DPathBuilder();

            void begin(const gs_vec2f& _Point);
            void line_to(const gs_vec2f& _Target);
            void arc_to(const gs_vec2f& _Target, const float& _Radius);

            void build_mesh(
                const gs_color&                                          _Color,
                const float&                                             _Width,
                std::vector<ApplicationRenderingBackendMeshVertex>&      _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
                const ApplicationRenderingBackendTexture&                _Texture);

            void build_mesh_filled(
                const gs_color&                                          _Color,
                std::vector<ApplicationRenderingBackendMeshVertex>&      _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
                const ApplicationRenderingBackendTexture&                _Texture);

        protected:

            std::vector<RenderingQueue2DPathSegment> m_PolygonLines           {std::vector<RenderingQueue2DPathSegment>()};
            float                                    m_PolygonLinesWidth      {4.f};
            std::vector<int>                         m_PolygonLinesIndexes    {std::vector<int>()};
            gs_vec2f                                 m_PolygonLinesSourcePoint{gs_vec2f(0.f, 0.f)};

            // service methods
            void end();

            void build_triangle_filled_mesh(
                const gs_vec2f&                                          _P1,
                const gs_vec2f&                                          _P2,
                const gs_vec2f&                                          _P3,
                const gs_color&                                          _Color,
                const ApplicationRenderingBackendTexture&                _Texture,
                std::vector<ApplicationRenderingBackendMeshVertex>&      _Vertexes,
                std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes);
        };
    }
}

RenderingQueue2DPathSegment::RenderingQueue2DPathSegment(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width)
{
    setup(_P1, _P2, _Width);
}

void RenderingQueue2DPathSegment::setup(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width)
{
    P1 = _P1;
    P2 = _P2;

    gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
    gs_vec2f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * _Width * 0.5f;
    
    P1min = _P1 - perpendicular;
    P1max = _P1 + perpendicular;
    P2min = _P2 - perpendicular;
    P2max = _P2 + perpendicular;
}

void RenderingQueue2DPathSegment::setup(const float& _Width)
{
    setup(P1, P2, _Width);
}

// RenderingQueuePathBuilder
RenderingQueue2DPathBuilder::RenderingQueue2DPathBuilder(const float& _PolygonLinesWidth) : m_PolygonLinesWidth(_PolygonLinesWidth){}
RenderingQueue2DPathBuilder::~RenderingQueue2DPathBuilder(){}

void RenderingQueue2DPathBuilder::begin(const gs_vec2f& _Point)
{
    m_PolygonLinesSourcePoint = _Point;
    m_PolygonLines.clear();
    m_PolygonLinesIndexes.clear();
}

void RenderingQueue2DPathBuilder::line_to(const gs_vec2f& _Target)
{
    m_PolygonLines.push_back(RenderingQueue2DPathSegment(m_PolygonLinesSourcePoint, _Target, m_PolygonLinesWidth));
    m_PolygonLinesIndexes.push_back((int)m_PolygonLinesIndexes.size());
    m_PolygonLinesSourcePoint = _Target;
}

void RenderingQueue2DPathBuilder::arc_to(const gs_vec2f& _Target, const float& _Radius)
{
    // calculate ellipse data
    gs_complex<float> perpendicular  = gs_cnormf<float>(gs_complex<float>((_Target - m_PolygonLinesSourcePoint).x, (_Target - m_PolygonLinesSourcePoint).y)) * gs_complex<float>(0.f, 1.f);
    gs_vec2f          center         = (_Target + m_PolygonLinesSourcePoint) * 0.5f + gs_vec2f(gs_realf(perpendicular), gs_imagf(perpendicular)) * _Radius;
    float             sourceAngle    = gs_to_degrees(atan2((m_PolygonLinesSourcePoint - center).y, (m_PolygonLinesSourcePoint - center).x));
    float             targetAngle    = gs_to_degrees(atan2((_Target - center).y, (_Target - center).x));
    float             radius         = (float)gs_vector_length(center - _Target);
    int               segmentsCount  = 36; // TODO: how to compute approximate ellipse segments count ???
    
    if(gs_abs(targetAngle - sourceAngle) < 1e-3) return;
    
    float angleIncrement = gs_abs(360.f / segmentsCount);

    for (float angle = sourceAngle; angle <= targetAngle; angle += angleIncrement)
        line_to(gs_vec2f(center.x + radius * cos(gs_to_radians(angle)), center.y + radius * sin(gs_to_radians(angle))));
    line_to(_Target);
}

void RenderingQueue2DPathBuilder::build_mesh(
    const gs_color&                                          _Color,
    const float&                                             _Width,
    std::vector<ApplicationRenderingBackendMeshVertex>&      _Vertexes,
    std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
    const ApplicationRenderingBackendTexture&                _Texture)
{
    if(m_PolygonLines.empty())
    {
        end();
        return;
    }

    float width = gs_max(_Width, m_PolygonLinesWidth);

    if (m_PolygonLines.size() == 1)
    {
        m_PolygonLines[0].setup(width);

        build_triangle_filled_mesh(
            m_PolygonLines[0].P1min,
            m_PolygonLines[0].P2min,
            m_PolygonLines[0].P1max,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);

        build_triangle_filled_mesh(
            m_PolygonLines[0].P2min,
            m_PolygonLines[0].P2max,
            m_PolygonLines[0].P1max,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);
        
        end();
        return;
    }
    
    auto get_element = [](const int& _Index, const int& _Size)->int
    {
        int index = _Index;
        while (index < 0)      index += _Size;
        while (index >= _Size) index -= _Size;
        return index;
    };

    const bool pathIsClosed = gs_vector_length(m_PolygonLines[0].P1 - m_PolygonLines[m_PolygonLines.size() - 1].P2) < gs_epsilon<float>();

    for (int i = 0; i < (int)m_PolygonLines.size(); i++)
    {
        m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].setup(width);

        build_triangle_filled_mesh(
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P1min,
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P2min,
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P1max,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);

        build_triangle_filled_mesh(
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P2min,
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P2max,
            m_PolygonLines[get_element(i, (int)m_PolygonLines.size())].P1max,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);

        // TODO: need another algorithm of lines smoothing
        if(i-1 >= 0 || pathIsClosed)
        {
            m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].setup(width);

            build_triangle_filled_mesh(
                m_PolygonLines[i].P1max,
                m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].P2max,
                m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].P2,
                _Color,
                _Texture,
                _Vertexes,
                _Indexes);

            build_triangle_filled_mesh(
                m_PolygonLines[i].P1min,
                m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].P2min,
                m_PolygonLines[get_element(i-1, (int)m_PolygonLines.size())].P2,
                _Color,
                _Texture,
                _Vertexes,
                _Indexes);
        }
    }

    end();
}

void RenderingQueue2DPathBuilder::build_mesh_filled(
    const gs_color&                                          _Color,
    std::vector<ApplicationRenderingBackendMeshVertex>&          _Vertexes,
    std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes,
    const ApplicationRenderingBackendTexture&                _Texture)
{
    // auxiliary lambdas
    auto get_element = [](const int& _Index, const int& _Size)->int
    {
        int index = _Index;
        while (index < 0)      index += _Size;
        while (index >= _Size) index -= _Size;
        return index;
    };

    // checks

    // no triangles
    if((int)m_PolygonLines.size() < 3)
    {
        end();
        return;
    }

    // triangle
    if((int)m_PolygonLines.size() == 3)
    {
        build_triangle_filled_mesh(
            m_PolygonLines[0].P1,
            m_PolygonLines[1].P1,
            m_PolygonLines[2].P1,
            _Color,
            _Texture,
            _Vertexes,
            _Indexes);

        end();
        return;
    }

    // setup lines indexes
    for (int i = 0; i < (int)m_PolygonLines.size(); i++)
        m_PolygonLines[i].Index = i;

    // triangulate polygon
    while (m_PolygonLinesIndexes.size() > 2)
    {
        for (int i = 0; i < (int)m_PolygonLinesIndexes.size(); i++)
        {
            // construct triangle
            int      ia = m_PolygonLinesIndexes[get_element(i, (int)m_PolygonLinesIndexes.size())    ];
            int      ib = m_PolygonLinesIndexes[get_element(i - 1, (int)m_PolygonLinesIndexes.size())];
            int      ic = m_PolygonLinesIndexes[get_element(i + 1, (int)m_PolygonLinesIndexes.size())];
            gs_vec2f pa = m_PolygonLines[ia].P1;
            gs_vec2f pb = m_PolygonLines[ib].P1;
            gs_vec2f pc = m_PolygonLines[ic].P1;

            // detect if this triangle is an ear, i.e there are no other points besides
            // this triangle points that are inside this triangle
            bool isEar = true;

            for (int j = 0; j < (int)m_PolygonLines.size(); j++)
            {
                if( m_PolygonLines[j].Index == m_PolygonLines[ia].Index ||
                    m_PolygonLines[j].Index == m_PolygonLines[ib].Index ||
                    m_PolygonLines[j].Index == m_PolygonLines[ic].Index) continue;

                gs_vec2f poly[3] = {pa, pb, pc};

                if(gs_point_in_2D_polygon(poly, 3, gs_vec2f(m_PolygonLines[j].P1)))
                {
                    isEar = false;
                    break;
                }
            }

            if(isEar || m_PolygonLinesIndexes.size() <= 3)
            {
                build_triangle_filled_mesh(pb, pa, pc, _Color, _Texture, _Vertexes, _Indexes);
                m_PolygonLinesIndexes.erase(m_PolygonLinesIndexes.begin() + i);
                break;
            }
        }
    }

    end();
}

void RenderingQueue2DPathBuilder::end()
{
    m_PolygonLines.clear();
    m_PolygonLinesIndexes.clear();
}

void RenderingQueue2DPathBuilder::build_triangle_filled_mesh(
    const gs_vec2f&                                          _P1,
    const gs_vec2f&                                          _P2,
    const gs_vec2f&                                          _P3,
    const gs_color&                                          _Color,
    const ApplicationRenderingBackendTexture&                _Texture,
    std::vector<ApplicationRenderingBackendMeshVertex>&          _Vertexes,
    std::vector<ApplicationRenderingBackendMeshVertexIndex>& _Indexes)
{
    const int size = (int)_Vertexes.size();

    _Vertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P1.x, _P1.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P1.x / _Texture.Width, _P1.y / _Texture.Height),
            _Color));
    
    _Vertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P2.x, _P2.y, 0.f),
            gs_vec3f(0.f), gs_vec2f(_P2.x / _Texture.Width, _P2.y / _Texture.Height),
            _Color));
    
    _Vertexes.push_back(
        ApplicationRenderingBackendMeshVertex(
            gs_vec3f(_P3.x, _P3.y, 0.f),
            gs_vec3f(0.f),
            gs_vec2f(_P3.x / _Texture.Width, _P3.y / _Texture.Height),
            _Color));
    
    for (int i = size; i < (int)_Vertexes.size(); ++i)
        _Indexes.push_back(i);
}

// RenderingQueue2D
RenderingQueue2D::RenderingQueue2D() : RenderingQueue(STRINGIFY(RenderingQueue2D)){}
RenderingQueue2D::~RenderingQueue2D(){}

gs_mat4f RenderingQueue2D::calculate_transform_matrix(const float& _Depth, const gs_vec2f& _Position, const float& _Rotation, const gs_vec2f& _Scale)
{
    return Frenchie::Application::ApplicationRenderingBackend::calculate_2d_transform_matrix(_Depth, _Position, _Rotation, _Scale);
}

void RenderingQueue2D::build_convex_poly_mesh(const gs_vec2f* _Points, const gs_color* _Colors, const int& _Count)
{
    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    GS_ASSERT(_Count >= 3);

    gs_vec2f min   = gs_vec2f(gs_huge<float>(), gs_huge<float>());
    gs_vec2f max   = gs_vec2f(gs_tiny<float>(), gs_tiny<float>());
    gs_color red   = 0;
    gs_color green = 0;
    gs_color blue  = 0;
    gs_color alpha = 0;

    for (int i = 0; i < _Count; i++)
    {
        min   =  gs_vec2f(gs_min(_Points[i].x, min.x), gs_min(_Points[i].y, min.y));
        max   =  gs_vec2f(gs_max(_Points[i].x, max.x), gs_max(_Points[i].y, max.y));
        red   += gs_color_rgba_get_r(_Colors[i]);
        green += gs_color_rgba_get_g(_Colors[i]);
        blue  += gs_color_rgba_get_b(_Colors[i]);
        alpha += gs_color_rgba_get_a(_Colors[i]);
    }

    gs_2dboxf box    = gs_2dboxf(min, max);
    float     width  = box.width();
    float     height = box.height();
    gs_vec2f  center = box.center();
    gs_color  color  = gs_color_rgba(red / _Count, green / _Count, blue / _Count, alpha / _Count);

    if(_Count < 4)
    {
        for (int i = 0; i < _Count; i++)
        {
            m_MeshVertexes.push_back(
                ApplicationRenderingBackendMeshVertex(
                    gs_vec3f(_Points[i].x, _Points[i].y, 0.f),
                    gs_vec3f(0.f),
                    gs_vec2f((_Points[i].x - box.Min.x) / width, (_Points[i].y - box.Min.y) / height),
                    _Colors[i]));
        }
    }
    else
    {
        for (int i = 0; i < _Count; i++)
        {
            m_MeshVertexes.push_back(
                ApplicationRenderingBackendMeshVertex(
                    gs_vec3f(_Points[i].x, _Points[i].y, 0.f),
                    gs_vec3f(0.f),
                    gs_vec2f((_Points[i].x - box.Min.x) / width, (_Points[i].y - box.Min.y) / height),
                    _Colors[i]));
            
            m_MeshVertexes.push_back(
                ApplicationRenderingBackendMeshVertex(
                    gs_vec3f(_Points[(i + 1) % _Count].x, _Points[(i + 1) % _Count].y, 0.f),
                    gs_vec3f(0.f),
                    gs_vec2f((_Points[(i + 1) % _Count].x - box.Min.x) / width, (_Points[(i + 1) % _Count].y - box.Min.y) / height),
                    _Colors[(i + 1) % _Count]));
            
            m_MeshVertexes.push_back(
                ApplicationRenderingBackendMeshVertex(
                    gs_vec3f(center.x, center.y, 0.f),
                    gs_vec3f(0.f),
                    gs_vec2f((center.x - box.Min.x) / width, (center.y - box.Min.y) / height),
                    color));
        }
    }

    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue2D::build_convex_poly_mesh(const gs_vec2f* _Points, const gs_color* _Colors, gs_vec2f* _UVs, const int& _Count)
{
    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    GS_ASSERT(_Count >= 3);

    gs_vec2f min   = gs_vec2f(gs_huge<float>(), gs_huge<float>());
    gs_vec2f max   = gs_vec2f(gs_tiny<float>(), gs_tiny<float>());
    gs_vec2f uvmin = gs_vec2f(gs_huge<float>(), gs_huge<float>());
    gs_vec2f uvmax = gs_vec2f(gs_tiny<float>(), gs_tiny<float>());
    gs_color red   = 0;
    gs_color green = 0;
    gs_color blue  = 0;
    gs_color alpha = 0;

    for (int i = 0; i < _Count; i++)
    {
        min   =  gs_vec2f(gs_min(_Points[i].x, min.x), gs_min(_Points[i].y, min.y));
        max   =  gs_vec2f(gs_max(_Points[i].x, max.x), gs_max(_Points[i].y, max.y));
        uvmin =  gs_vec2f(gs_min(_UVs[i].x, uvmin.x), gs_min(_UVs[i].y, uvmin.y));
        uvmax =  gs_vec2f(gs_max(_UVs[i].x, uvmax.x), gs_max(_UVs[i].y, uvmax.y));
        red   += gs_color_rgba_get_r(_Colors[i]);
        green += gs_color_rgba_get_g(_Colors[i]);
        blue  += gs_color_rgba_get_b(_Colors[i]);
        alpha += gs_color_rgba_get_a(_Colors[i]);
    }

    gs_2dboxf box      = gs_2dboxf(min, max);
    float     width    = box.width();
    float     height   = box.height();
    gs_vec2f  center   = box.center();
    gs_vec2f  uvcenter = gs_2dboxf(uvmin, uvmax).center();
    gs_color  color  = gs_color_rgba(red / _Count, green / _Count, blue / _Count, alpha / _Count);

    if(_Count < 4)
    {
        for (int i = 0; i < _Count; i++)
        {
            m_MeshVertexes.push_back(
                ApplicationRenderingBackendMeshVertex(
                    gs_vec3f(_Points[i].x, _Points[i].y, 0.f),
                    gs_vec3f(0.f),
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
                    gs_vec3f(_Points[i].x, _Points[i].y, 0.f),
                    gs_vec3f(0.f),
                    gs_vec2f(_UVs[i].x, _UVs[i].y),
                    _Colors[i]));
            
            m_MeshVertexes.push_back(
                ApplicationRenderingBackendMeshVertex(
                    gs_vec3f(_Points[(i + 1) % _Count].x, _Points[(i + 1) % _Count].y, 0.f),
                    gs_vec3f(0.f),
                    gs_vec2f(_UVs[(i + 1) % _Count].x, _UVs[(i + 1) % _Count].y),
                    _Colors[(i + 1) % _Count]));
            
            m_MeshVertexes.push_back(
                ApplicationRenderingBackendMeshVertex(
                    gs_vec3f(center.x, center.y, 0.f),
                    gs_vec3f(0.f),
                    uvcenter,
                    color));
        }
    }

    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue2D::build_line_mesh(const gs_vec2f&  _P1, const gs_vec2f&  _P2, const float& _Width, const gs_color& _Color)
{
    // build line mesh
    gs_vec3f direction     = gs_vector_normalize(_P2 - _P1);
    gs_vec2f perpendicular = gs_vector_normalize(gs_vector_cross(direction, gs_vec3f(0.f, 0.f, 1.f))) * gs_max(_Width, m_MinimumLineWidth) * 0.5f;
    gs_vec2f points[4]     = { _P1 - perpendicular, _P2 - perpendicular, _P2 + perpendicular, _P1 + perpendicular };
    gs_color colors[4]     = { _Color, _Color, _Color, _Color };
    build_convex_poly_mesh(points, colors, 4);

    // build smoothing ellipses
    if(_Width > m_MinimumLineWidth * 2.f)
    {
        build_arc_filled_mesh(
            _P1,
            _Width * 0.5f,
            _Width * 0.5f,
            gs_to_degrees(gs_vector_argument(perpendicular * (-1.f))),
            gs_to_degrees(gs_vector_argument(perpendicular * (-1.f))) + 180.f,
            _Color,
            5);
        
        build_arc_filled_mesh(
            _P2,
            _Width * 0.5f,
            _Width * 0.5f,
            gs_to_degrees(gs_vector_argument(perpendicular)),
            gs_to_degrees(gs_vector_argument(perpendicular)) + 180.f,
            _Color,
            5);
    }
}

void RenderingQueue2D::build_triangle_filled_mesh(const gs_vec2f& _P1, const gs_vec2f& _P2, const gs_vec2f& _P3, const gs_color& _Color)
{
    gs_vec2f points[3] = {_P1, _P2, _P3};
    gs_color colors[3] = {_Color, _Color, _Color};
    build_convex_poly_mesh(points, colors, 3);
}

void RenderingQueue2D::build_triangle_mesh(const gs_vec2f& _P1, const gs_vec2f& _P2, const gs_vec2f& _P3, const float& _Width, const gs_color& _Color)
{
    build_line_mesh(_P1, _P2, _Width, _Color);
    build_line_mesh(_P2, _P3, _Width, _Color);
    build_line_mesh(_P3, _P1, _Width, _Color);
}

void RenderingQueue2D::build_rectangle_filled_mesh(const gs_vec2f& _Min, const gs_vec2f& _Max, const gs_color& _Color, const float& _Radius)
{
    if(gs_abs(_Radius) <= m_MinimumLineWidth)
    {
        gs_vec2f points[4] = {gs_vec2f(_Min.x, _Min.y), gs_vec2f(_Max.x, _Min.y), gs_vec2f(_Max.x, _Max.y), gs_vec2f(_Min.x, _Max.y)};
        gs_color colors[4] = {_Color, _Color, _Color, _Color};
        build_convex_poly_mesh(points, colors, 4);
        return;
    }

    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    gs_2dboxf box(_Min, _Max);

    const float sourceAngle   = 0.f;
    const float targetAngle   = 360.f;
    const float segmentsCount = 36.f;
    const float deltaAngle    = 360.f / segmentsCount;
    const float cornerRadius  = gs_min(gs_abs(_Radius), box.width() * 0.5f, box.height() * 0.5f);
    const float innerWidth    = box.width() - 2 * cornerRadius;
    const float innerHeight   = box.height() - 2 * cornerRadius;

    for (float angle = sourceAngle; angle < targetAngle; angle += deltaAngle)
    {
        float a = angle;
        float b = angle + deltaAngle;

        gs_vec2f p1 = gs_vec2f(
            box.center().x + innerWidth * 0.5f * gs_sign(cos(gs_to_radians(a))),
            box.center().y + innerHeight * 0.5f * gs_sign(sin(gs_to_radians(a)))) + gs_vec2f(cos(gs_to_radians(a)), sin(gs_to_radians(a))) * cornerRadius;

        gs_vec2f p2 = gs_vec2f(
            box.center().x + innerWidth * 0.5f * gs_sign(cos(gs_to_radians(b))),
            box.center().y + innerHeight * 0.5f * gs_sign(sin(gs_to_radians(b)))) + gs_vec2f(cos(gs_to_radians(b)), sin(gs_to_radians(b))) * cornerRadius;
        
        gs_vec2f p3 = box.center();

        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                gs_vec3f(p1.x, p1.y, 0.f),
                gs_vec3f(0.f),
                gs_vec2f((p1.x - box.Min.x) / box.width(), (p1.y - box.Min.y) / box.height()),
                _Color));
        
        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                gs_vec3f(p2.x, p2.y, 0.f),
                gs_vec3f(0.f),
                gs_vec2f((p2.x - box.Min.x) / box.width(), (p2.y - box.Min.y) / box.height()),
                _Color));
        
        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                gs_vec3f(p3.x, p3.y, 0.f),
                gs_vec3f(0.f),
                gs_vec2f((p3.x - box.Min.x) / box.width(), (p3.y - box.Min.y) / box.height()),
                _Color));
    }

    for (ApplicationRenderingBackendMeshVertexIndex i = size; i < (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size(); ++i)
        m_MeshVertexesIndexes.push_back(i);
}

void RenderingQueue2D::build_rectangle_mesh(const gs_vec2f& _Min, const gs_vec2f& _Max, const gs_color& _Color, const float& _Width, const float& _Radius)
{
    if(gs_abs(_Radius) <= m_MinimumLineWidth)
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
    const float deltaAngle    = 360.f / segmentsCount;
    const float cornerRadius  = gs_min(gs_abs(_Radius), box.width() * 0.5f, box.height() * 0.5f);
    const float innerWidth    = box.width() - 2 * cornerRadius;
    const float innerHeight   = box.height() - 2 * cornerRadius;

    for (float angle = sourceAngle; angle < targetAngle; angle += deltaAngle)
    {
        float a = angle;
        float b = angle + deltaAngle;

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
    const gs_vec2f&                           _Center,
    const float&                              _MinorRadius,
    const float&                              _MajorRadius,
    const float&                              _SourceAngle,
    const float&                              _TargetAngle,
    const gs_color&                           _Color,
    const int&                                _SegmentsCount)
{
    const ApplicationRenderingBackendMeshVertexIndex size = (ApplicationRenderingBackendMeshVertexIndex)m_MeshVertexes.size();

    const gs_2dboxf box        = gs_2dboxf(_Center - gs_vec2f(_MinorRadius, _MajorRadius), _Center + gs_vec2f(_MinorRadius, _MajorRadius));
    const float     deltaAngle = 360.f / _SegmentsCount;

    for (float angle = gs_min(_SourceAngle, _TargetAngle); angle < gs_max(_SourceAngle, _TargetAngle); angle += deltaAngle)
    {
        float a = angle;
        float b = gs_clamp(angle + deltaAngle, gs_min(_SourceAngle, _TargetAngle), gs_max(_SourceAngle, _TargetAngle));

        gs_vec2f p1 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(a)), _Center.y + _MajorRadius * sin(gs_to_radians(a)));
        gs_vec2f p2 = gs_vec2f(_Center.x + _MinorRadius * cos(gs_to_radians(b)), _Center.y + _MajorRadius * sin(gs_to_radians(b)));
        gs_vec2f p3 = _Center;

        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                gs_vec3f(p1.x, p1.y, 0.f),
                gs_vec3f(0.f),
                gs_vec2f((p1.x - box.Min.x) / box.width(), (p1.y - box.Min.y) / box.height()),
                _Color));
        
        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                gs_vec3f(p2.x, p2.y, 0.f),
                gs_vec3f(0.f),
                gs_vec2f((p2.x - box.Min.x) / box.width(), (p2.y - box.Min.y) / box.height()),
                _Color));
        
        m_MeshVertexes.push_back(
            ApplicationRenderingBackendMeshVertex(
                gs_vec3f(p3.x, p3.y, 0.f),
                gs_vec3f(0.f),
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
    const gs_color& _Color,
    const int&      _SegmentsCount)
{
    const float lineWidth  = gs_max(_Width, m_MinimumLineWidth);
    const float deltaAngle = 360.f / _SegmentsCount;

    for (float angle = gs_min(_SourceAngle, _TargetAngle); angle < gs_max(_SourceAngle, _TargetAngle); angle += deltaAngle)
    {
        float a = angle;
        float b = gs_clamp(angle + deltaAngle, gs_min(_SourceAngle, _TargetAngle), gs_max(_SourceAngle, _TargetAngle));
        gs_vec2f p1 = _Center + gs_vec2f(cos(gs_to_radians(a)), sin(gs_to_radians(a))) * _MinorRadius;
        gs_vec2f p2 = _Center + gs_vec2f(cos(gs_to_radians(b)), sin(gs_to_radians(b))) * _MinorRadius;
        build_line_mesh(p1, p2, _Width, _Color);
    }
}

// commands
void RenderingQueue2D::push_convex_poly(
    const gs_vec2f*                           _Points,
    const gs_color*                           _Colors,
    const int&                                _Count,
    const gs_mat4f&                           _Transform,
    const ApplicationRenderingBackendTexture& _Texture)
{
    build_convex_poly_mesh(_Points, _Colors, _Count);
    
    push_rendering_command(
        _Texture.is_null() ? ApplicationRenderingBackend::get_default_texture() : _Texture,
        gs_color_rgb(255, 255, 255),
        _Transform);
}

void RenderingQueue2D::push_line(const gs_vec2f& _P1, const gs_vec2f& _P2, const float& _Width, const gs_color& _Color, const gs_mat4f& _Transform)
{
    if(!current_clipping_box().intersects( _Transform * gs_vec4f(_P1, 0.f, 1.f), _Transform * gs_vec4f(_P2, 0.f, 1.f)))
        return;

    RenderingQueue2D::build_line_mesh(_P1, _P2, _Width, _Color);
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

void RenderingQueue2D::push_triangle(
    const gs_vec2f& _P1,
    const gs_vec2f& _P2,
    const gs_vec2f& _P3,
    const float&    _Width,
    const gs_color& _Color,
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
    push_rendering_command(_Transform, _Color);
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