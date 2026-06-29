#pragma once

#include <FrenchieCoreMesh.hpp>
#include <FrenchieCoreRingBuffer.hpp>

/*! \defgroup <Core> (Core)
 *  @brief The module contains core utility functions and classes.
    @{
*/

/*! @} */

/*! \defgroup <Mesh> (Mesh)
 *  @ingroup Core
 *  @brief The module contains core utility functions and classes for 2D/3D mesh processing.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Core
    {
        namespace Mesh
        {
            /*! \defgroup <Triangulations2D> (2D Triangulations)
            *  @ingroup Mesh
            *  @brief The module contains core utility functions and classes to triangulate 2D meshes.
            * @{
            */

            class AbstractDelaunator2D
            {
            public:
                
                typedef Frenchie::Core::Mesh::Mesh<gs_vec2f> Surface;

                AbstractDelaunator2D()
                {
                    for (int i = 0; i < m_MeshVertexesPerturbations.size(); i++)
                        m_MeshVertexesPerturbations.push(gs_pseudo_random<float>(-gs_tiny<float>(), +gs_tiny<float>()) * 2.f);
                }

                virtual ~AbstractDelaunator2D(){}

                /**
                 * @brief This function implements classic Bowyer-Watson Delaunay triangulation algorithm of the input points cloud
                 * @tparam Commit 
                 * @param _Points input discrete points cloud
                 * @param _Count the number of points within discrete points cloud
                 * @param _Commit the callback called for every gs_2d_trianglef object generated for input points cloud
                 */
                template<typename Commit>
                void triangulate(const gs_vec2f _Points[], const int& _Count, const Commit& _Commit)
                {
                    // build triangulated mesh
                    gs_2d_boxf boundingRectangle = build_triangulated_mesh(_Points, _Count);

                    // commit not null faces
                    for(auto& face : m_Mesh.get_faces())
                    {
                        if(face.is_null()) continue;

                        auto triangle = get_face_triangle(m_Mesh, face);

                        // commit all faces including bounding triangle
                        if(!m_DiscardSuperTriangle) 
                        {
                            _Commit(triangle);
                            continue;
                        }

                        // commit faces that don't share vertexes with bounding triangle
                        if( boundingRectangle.contains(triangle.P1) &&
                            boundingRectangle.contains(triangle.P2) &&
                            boundingRectangle.contains(triangle.P3))
                        {
                            _Commit(triangle);
                        }
                    }
                }

                // API
                void discard_bounding_triangle()
                {
                    m_DiscardSuperTriangle = true;
                }

                void dont_discard_bounding_triangle()
                {
                    m_DiscardSuperTriangle = false;
                }

            protected:

                // info
                Surface                                  m_Mesh                      {Surface()};
                bool                                     m_DiscardSuperTriangle      {true};
                Frenchie::Core::RingBuffer<gs_vec2f, 16> m_MeshVertexesPerturbations {Frenchie::Core::RingBuffer<gs_vec2f, 16>()};

                // protected virtual methods
                virtual gs_2d_boxf build_triangulated_mesh(const gs_vec2f _Points[], const int& _Count) = 0;

                // service methods
                gs_2d_trianglef get_face_triangle(const Surface& _Mesh, const Surface::FaceHandle& _Face)
                {
                    if(_Face.is_null())
                        return gs_2d_trianglef();

                    auto start  = _Face.self().get_edge();
                    auto next   = start;
                    int  vertex = 0;

                    gs_vec2f cavity[3]{};

                    do
                    {
                        if(vertex < 3 && next.is_not_null())
                            cavity[vertex] = next.self().get_node().self().get_data();
                        next = next.get_next();
                        vertex++;
                    }
                    while (next.is_not_null() && next != start);
                    
                    return gs_2d_trianglef(cavity[0], cavity[1], cavity[2]);
                };
            };

            /**
             * @brief This class implements classic Bowyer-Watson Delaunay triangulation algorithm of the input points cloud
             * @class BowyerWatsonDelanuator2D
             */
            class BowyerWatsonDelaunator2D final : public AbstractDelaunator2D
            {
            public:
                BowyerWatsonDelaunator2D();
                virtual ~BowyerWatsonDelaunator2D();

            protected:

                // info
                std::vector<Surface::FaceHandle> m_NonDelaunayFaces            {std::vector<Surface::FaceHandle>()};
                std::vector<Surface::NodeHandle> m_NonDelaunayFacesCavityNodes {std::vector<Surface::NodeHandle>()};

                // virtual methods override
                virtual gs_2d_boxf build_triangulated_mesh(const gs_vec2f _Points[], const int& _Count) override;
            };

            /**
             * @brief This class implements Sloan Delaunay triangulation algorithm of the input points cloud
             * @class BowyerWatsonDelanuator2D
             */
            class SloanDelaunator2D final : public AbstractDelaunator2D
            {
            public:
                SloanDelaunator2D();
                virtual ~SloanDelaunator2D();
                
            protected:

                // virtual methods override
                virtual gs_2d_boxf build_triangulated_mesh(const gs_vec2f _Points[], const int& _Count) override;

                // service methods
                AbstractDelaunator2D::Surface::FaceHandle
                find_face_containing_node(
                    const AbstractDelaunator2D::Surface&             _Mesh,
                    const AbstractDelaunator2D::Surface::NodeHandle& _Node);

                std::vector<AbstractDelaunator2D::Surface::FaceHandle>
                split_face_by_node(
                    AbstractDelaunator2D::Surface&                   _Mesh,
                    const AbstractDelaunator2D::Surface::FaceHandle& _Face,
                    const AbstractDelaunator2D::Surface::NodeHandle& _Node);
            
                std::vector<AbstractDelaunator2D::Surface::FaceHandle>
                find_neighbours_opposite_to_node(
                    const AbstractDelaunator2D::Surface&             _Mesh,
                    const AbstractDelaunator2D::Surface::FaceHandle& _Face,
                    const AbstractDelaunator2D::Surface::NodeHandle& _Node);
            
                Surface::FaceHandle find_neighbour_that_has_node(
                    const AbstractDelaunator2D::Surface&             _Mesh,
                    const AbstractDelaunator2D::Surface::FaceHandle& _Face,
                    const AbstractDelaunator2D::Surface::NodeHandle& _Node);
            
                bool is_convex_quadrilateral(
                    const AbstractDelaunator2D::Surface&             _Mesh,
                    const AbstractDelaunator2D::Surface::FaceHandle& _Left,
                    const AbstractDelaunator2D::Surface::FaceHandle& _Right);
            
            };
            
            /*! @} */
        }
    }
}