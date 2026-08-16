#pragma once

#include <FrenchieCoreRingBuffer.hpp>
#include <FrenchieCoreMeshSurface.hpp>

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

            // 2D surface
            typedef Frenchie::Core::Mesh::Surface<gs_vec2f> Surface2D;
            
            /**
             * @brief This is an abstract triangulator
             * @class AbstractTriangulator2D
             */
            class AbstractTriangulator2D
            {
            public:

                AbstractTriangulator2D();
                virtual ~AbstractTriangulator2D();

                /**
                 * @brief This function triangulates a set of input points
                 * @tparam Commit 
                 * @param _Points input discrete points cloud
                 * @param _Count the number of points within discrete points cloud
                 * @param _Commit the callback called for every gs_2d_trianglef object generated for input points cloud
                 */
                template<typename Commit>
                void triangulate(const gs_vec2f _Points[], const int& _Count, const Commit& _Commit)
                {
                    if(_Points == nullptr || _Count < 3)
                        return;

                    // triangulate mesh
                    build_triangulated_mesh(_Points, _Count);

                    // commit not null faces
                    for(auto& face : m_Mesh.get_faces())
                    {
                        if(face.is_not_null())
                            _Commit(get_face_triangle(m_Mesh, face));
                    }
                }

            protected:

                // info
                Surface2D                                m_Mesh                      {Surface2D()};
                Frenchie::Core::RingBuffer<gs_vec2f, 16> m_MeshVertexesPerturbations {Frenchie::Core::RingBuffer<gs_vec2f, 16>()};

                // service virtual API
                virtual void build_triangulated_mesh(const gs_vec2f _Points[], const int& _Count) = 0;

                // service API
                gs_2d_trianglef get_face_triangle(const Surface2D& _Mesh, const Surface2D::FaceHandle& _Face);
            };

            /**
             * @brief This class implements classic Bowyer-Watson Delaunay triangulation algorithm of the input points cloud
             * @class BowyerWatsonDelanuator2D
             */
            class BowyerWatsonDelaunator2D final : public AbstractTriangulator2D
            {
            public:
                BowyerWatsonDelaunator2D();
                virtual ~BowyerWatsonDelaunator2D();

            protected:

                // info
                std::vector<Surface2D::FaceHandle> m_NonDelaunayFaces            {std::vector<Surface2D::FaceHandle>()};
                std::vector<Surface2D::NodeHandle> m_NonDelaunayFacesCavityNodes {std::vector<Surface2D::NodeHandle>()};

                // virtual methods override
                virtual void build_triangulated_mesh(const gs_vec2f _Points[], const int& _Count) override;
            };

            /**
             * @brief This class implements Sloan Delaunay triangulation algorithm of the input points cloud
             * @class BowyerWatsonDelanuator2D
             */
            class SloanDelaunator2D final : public AbstractTriangulator2D
            {
            public:
                SloanDelaunator2D();
                virtual ~SloanDelaunator2D();
                
            protected:

                // virtual methods override
                virtual void build_triangulated_mesh(const gs_vec2f _Points[], const int& _Count) override;

                // service methods
                Surface2D::FaceHandle
                find_face_containing_node(
                    const Surface2D&             _Mesh,
                    const Surface2D::NodeHandle& _Node);

                std::vector<Surface2D::FaceHandle>
                split_face_by_node(
                    Surface2D&                   _Mesh,
                    const Surface2D::FaceHandle& _Face,
                    const Surface2D::NodeHandle& _Node);
            
                std::vector<Surface2D::FaceHandle>
                find_neighbours_opposite_to_node(
                    const Surface2D&             _Mesh,
                    const Surface2D::FaceHandle& _Face,
                    const Surface2D::NodeHandle& _Node);
            
                Surface2D::FaceHandle find_neighbour_that_has_node(
                    const Surface2D&             _Mesh,
                    const Surface2D::FaceHandle& _Face,
                    const Surface2D::NodeHandle& _Node);
            
                bool is_convex_quadrilateral(
                    const Surface2D&             _Mesh,
                    const Surface2D::FaceHandle& _Left,
                    const Surface2D::FaceHandle& _Right);
            
            };
            
            /*! @} */
        }
    }
}