#pragma once

// Application
#include <FrenchieApplicationRenderingBackend.hpp>

// Core
#include <FrenchieCoreMath.hpp>

// STL
#include <vector>
#include <optional>

namespace Frenchie
{
    namespace Application
    {
        class MeshGenerator2D final
        {
        public:
            MeshGenerator2D()  = default;
            ~MeshGenerator2D() = default;

            struct Vertex
            {
                gs_vec2f Position {gs_vec2f(0.f, 0.f)};
                gs_color Color    {gs_color_rgb(255, 255, 255)};
                float    Radius   {0.f};
            };

            void begin()
            {
                m_Input.clear();
                m_Points.clear();
                m_Colors.clear();
            }

            void next(const gs_vec2f& _Point, gs_color _Color, float _Radius = 0.f)
            {
                m_Input.push_back({_Point, _Color, _Radius});
            }

            void end()
            {
                // auxiliary lambdas
                auto get_tessellated_segments_count = [](const float& _Radius, const float& _TesselationTolerance)->int
                {
                    float radius = gs_clamp(gs_abs(_Radius), 8.f, 2048.f);
                    return gs_min<int>(90, PI2 * radius * 0.5f / sqrtf(2.f * radius * _TesselationTolerance - _TesselationTolerance * _TesselationTolerance));
                };

                // generate mesh
                for (int i = 0; i < m_Input.size(); ++i)
                {
                    if(gs_abs(m_Input[i].Radius) <= 4.f)
                    {
                        m_Points.push_back(m_Input[i].Position);
                        m_Colors.push_back(m_Input[i].Color);
                        continue;
                    }

                    gs_vec2f pointA = m_Input[gs_array_index_clamp(i + 0, m_Input.size())].Position;
                    gs_vec2f pointB = m_Input[gs_array_index_clamp(i - 1, m_Input.size())].Position;
                    gs_vec2f pointC = m_Input[gs_array_index_clamp(i + 1, m_Input.size())].Position;

                    gs_vec2f normalizedVectorAB = gs_vector_normalize(pointB - pointA);
                    gs_vec2f normalizedVectorAC = gs_vector_normalize(pointC - pointA);
                    float    maxSmoothingRadius = gs_min(gs_min(gs_vector_length(pointB - pointA), gs_vector_length(pointC - pointA)) * 0.5f, m_Input[i].Radius);

                    gs_vec2f center   = pointA + gs_vector_normalize(normalizedVectorAB + normalizedVectorAC) * maxSmoothingRadius;
                    float    radius   = sqrtf((1.f - gs_vectors_dot(normalizedVectorAB, normalizedVectorAC)) * 0.5f)  * maxSmoothingRadius;
                    float    tangent  = sqrtf((1.f + gs_vectors_dot(normalizedVectorAB, normalizedVectorAC)) * 0.5f)  * maxSmoothingRadius;

                    float sourceAngle = gs_to_degrees(gs_vector_argument(pointA + normalizedVectorAC * tangent - center));
                    float targetAngle = gs_to_degrees(gs_vector_argument(pointA + normalizedVectorAB * tangent - center));
                    bool  isConcave   = gs_vector_cross(normalizedVectorAB, normalizedVectorAC) < 0.f;

                    if(isConcave)
                        gs_swap(sourceAngle, targetAngle);

                    while(targetAngle < sourceAngle)
                        targetAngle += 360.f;

                    float deltaAngle    = 360.f / get_tessellated_segments_count(radius, 0.1f);
                    int   segmentsCount = (targetAngle - sourceAngle) / deltaAngle;

                    for (int j = 0; j < segmentsCount; ++j)
                    {
                        float angle = sourceAngle + (isConcave ? j * deltaAngle : (segmentsCount - j - 1) * deltaAngle);
                        float a = angle;
                        float b = gs_clamp(angle + deltaAngle, sourceAngle, targetAngle);
                        gs_vec2f p1 = center + gs_vec2f(cos(gs_to_radians(a)), sin(gs_to_radians(a))) * radius;
                        gs_vec2f p2 = center + gs_vec2f(cos(gs_to_radians(b)), sin(gs_to_radians(b))) * radius;

                        m_Points.push_back(p1);
                        m_Points.push_back(p2);

                        m_Colors.push_back(m_Input[i].Color);
                        m_Colors.push_back(m_Input[i].Color);
                    }
                }

                // compute normals
                // ...
            }

            template<typename Submit>
            void submit(const Submit& _Submit)
            {
                // assert
                if(m_Points.empty() || m_Colors.empty()) return;

                // determine bounding box and orientation
                gs_2d_boxf                polygonBoundingBox        = gs_2d_boxf(m_Points[0], m_Points[0]);
                gs_color                  polygonCentralColor       = 0;
                bool                      isPolygonCounterClockWise = gs_2D_polygon_signed_area(m_Points.data(), (int)m_Points.size()) < 0.f;

                gs_color red   = 0;
                gs_color green = 0;
                gs_color blue  = 0;
                gs_color alpha = 0;

                for (int i = 0; i < (int)m_Points.size(); i++)
                {
                    polygonBoundingBox = gs_2d_boxf(polygonBoundingBox.Min, polygonBoundingBox.Max, m_Points[i]);
                    
                    red   += gs_color_rgba_get_r(m_Colors[i]);
                    green += gs_color_rgba_get_g(m_Colors[i]);
                    blue  += gs_color_rgba_get_b(m_Colors[i]);
                    alpha += gs_color_rgba_get_a(m_Colors[i]);
                }

                polygonCentralColor = gs_color_rgba(red / (int)m_Points.size(), green / (int)m_Points.size(), blue / (int)m_Points.size(), alpha / (int)m_Points.size());
                
                // build convex filled mesh mesh
                if(gs_2D_point_in_polygon(m_Points.data(), (int)m_Points.size(), polygonBoundingBox.center()))
                {

                    if((int)m_Points.size() < 4)
                    {
                        for (int i = 0; i < (int)m_Points.size(); ++i)
                        {
                            _Submit(
                                ApplicationRenderingBackendMeshVertex(
                                        m_Points[i],
                                        gs_vec2f((m_Points[i].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (m_Points[i].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()),
                                        m_Colors[i]));
                        }
                    }
                    else
                    {
                        for (int i = 0; i < (int)m_Points.size(); ++i)
                        {
                            _Submit(
                                ApplicationRenderingBackendMeshVertex(
                                    m_Points[i],
                                    gs_vec2f((m_Points[i].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (m_Points[i].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()),
                                    m_Colors[i]));
                            
                            _Submit(
                                ApplicationRenderingBackendMeshVertex(
                                    m_Points[(i + 1) % m_Points.size()],
                                    gs_vec2f((m_Points[(i + 1) % m_Points.size()].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (m_Points[(i + 1) % m_Points.size()].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()),
                                    m_Colors[(i + 1) % m_Points.size()]));

                            _Submit(
                                ApplicationRenderingBackendMeshVertex(
                                    polygonBoundingBox.center(),
                                    gs_vec2f((polygonBoundingBox.center().x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (polygonBoundingBox.center().y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()),
                                    polygonCentralColor));
                        }
                    }

                    return;
                }

                // build concave filled mesh using ear clipping algorithm
                m_TriangulationIndexes.clear();
                for (int i = 0; i < (int)m_Points.size(); ++i)
                    m_TriangulationIndexes.push_back(i);

                for (int i = 0; (int)m_TriangulationIndexes.size() > 2 && i < (int)m_Points.size(); ++i)    
                {
                    for (int j = 0; j < m_TriangulationIndexes.size(); ++j)
                    {                    
                        int  point1   = m_TriangulationIndexes[gs_array_index_clamp(j + 0, (int)m_TriangulationIndexes.size())];
                        int  point2   = m_TriangulationIndexes[gs_array_index_clamp(j - 1, (int)m_TriangulationIndexes.size())];
                        int  point3   = m_TriangulationIndexes[gs_array_index_clamp(j + 1, (int)m_TriangulationIndexes.size())];
                        bool isEar    = true;
                        bool isConvex = isPolygonCounterClockWise ?
                                gs_vector_cross(m_Points[point1] - m_Points[point2], m_Points[point1] - m_Points[point3]) > 0.f :
                                    gs_vector_cross(m_Points[point1] - m_Points[point3], m_Points[point1] - m_Points[point2]) > 0.f;

                        // only convex points can create ears
                        if(!isConvex) continue;

                        // check that triangle does not contain other poly points
                        gs_vec2f poly[3] = {m_Points[point1], m_Points[point2], m_Points[point3]};
                        
                        for (int k = 0; k < (int)m_TriangulationIndexes.size(); ++k)
                        {
                            if(m_TriangulationIndexes[k] == point1 || m_TriangulationIndexes[k] == point2 || m_TriangulationIndexes[k] == point3)
                                continue;

                            if(gs_2D_point_in_polygon(poly, 3, m_Points[m_TriangulationIndexes[k]]))
                            {
                                isEar = false;
                                break;
                            }
                        }

                        if(!isEar) continue;

                        // build mesh
                        if(!isPolygonCounterClockWise) gs_swap(point2, point3);

                        _Submit(
                            ApplicationRenderingBackendMeshVertex(
                                m_Points[point1],
                                gs_vec2f((m_Points[point1].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (m_Points[point1].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()),
                                m_Colors[point1]));

                        _Submit(
                            ApplicationRenderingBackendMeshVertex(
                                m_Points[point2],
                                gs_vec2f((m_Points[point2].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (m_Points[point2].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()),
                                m_Colors[point2]));


                        _Submit(
                            ApplicationRenderingBackendMeshVertex(
                                m_Points[point3],
                                gs_vec2f((m_Points[point3].x - polygonBoundingBox.Min.x) / polygonBoundingBox.width(), (m_Points[point3].y - polygonBoundingBox.Min.y) / polygonBoundingBox.height()),
                                m_Colors[point3]));

                        // erase point triangulated point
                        m_TriangulationIndexes.erase(m_TriangulationIndexes.begin() + j);

                        break;
                    }
                }
            }

        private:

            std::vector<Vertex>   m_Input;
            std::vector<gs_vec2f> m_Points;
            std::vector<gs_color> m_Colors;

            std::vector<int>      m_TriangulationIndexes      {std::vector<int>()};
        };
    }
}