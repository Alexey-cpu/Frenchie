#pragma once

#include <FrenchieCoreMesh.hpp>

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

            /**
             * @brief This class implements classic Bowyer-Watson Delaunay triangulation algorithm of the input points cloud
             * @class BowyerWatsonDelanuator2D
             */
            class BowyerWatsonDelanuator2D
            {
            public:
                typedef Frenchie::Core::Mesh::Mesh<gs_vec2f, gs_2d_trianglef> Surface;    

                /**
                 * @brief This function implements classic Bowyer-Watson Delaunay triangulation algorithm of the input points cloud
                 * @tparam Commit 
                 * @param _Points input discrete points cloud
                 * @param _Count the number of points within discrete points cloud
                 * @param _Commit the callback called for every gs_2d_trianglef object generated for input points cloud
                 */
                template<typename Commit>
                void triangulate(const gs_vec2f _Points[], const int& _Count, const Commit&  _Commit)
                {
                    if(_Points == nullptr || _Count <= 0)
                        return;

                    // get ready
                    m_Mesh.flush();
                    m_NonDelaunayFaces.clear();
                    m_NonDelaunayFacesCavityNodes.clear();

                    // driver code
                    gs_2d_trianglef boundingTriangle  = gs_2d_trianglef(_Points, _Count);
                    gs_2d_boxf      boundingRectangle = gs_2d_boxf(_Points[0], _Points[0]);

                    Surface::NodeHandle boundingTriangleVolume[3] = 
                    {
                        m_Mesh.add_node(boundingTriangle.P1),
                        m_Mesh.add_node(boundingTriangle.P2),
                        m_Mesh.add_node(boundingTriangle.P3)
                    };

                    m_Mesh.add_face(boundingTriangleVolume, 3, boundingTriangle);

                    for (int i = 0; i < _Count; i++)
                    {
                        // add a little random perturbation to a point coordinate to move it to a general position.
                        // This guarantees degeneracies absence in general case.
                        if(m_MeshVertexesPerturbations.size() <= i)
                            m_MeshVertexesPerturbations.push_back(gs_pseudo_random<float>(-gs_tiny<float>(), +gs_tiny<float>()) * 2.f);

                        Surface::NodeHandle newNode = m_Mesh.add_node(_Points[i] + m_MeshVertexesPerturbations[i]);
                        
                        // recompute bounding rectangle considering perturbations
                        boundingRectangle = gs_2d_boxf(boundingRectangle.Min, boundingRectangle.Max, newNode.self().get_data());

                        // find not-Delaunay faces
                        m_NonDelaunayFaces.clear();

                        for (auto& face : m_Mesh.get_faces())
                        {
                            if(face.is_not_null() && face.self().get_data().circum_circle().contains(newNode.self().get_data()))
                                m_NonDelaunayFaces.push_back(face.self());
                        }

                        // dig not-Delaunay faces cavity nodes
                        m_NonDelaunayFacesCavityNodes.clear();

                        for (auto& nonDelaunayFace : m_NonDelaunayFaces)
                        {
                            Surface::EdgeHandle next = nonDelaunayFace.self().get_edge();

                            do
                            {
                                if(
                                    // look for edges that don't belong to non-Delaunay faces
                                    std::find(m_NonDelaunayFaces.begin(), m_NonDelaunayFaces.end(), next.self().get_twin().get_face()) == m_NonDelaunayFaces.end() &&
                                    
                                    // exclude duplicated vertexes from cavity boundary
                                    std::find(m_NonDelaunayFacesCavityNodes.begin(), m_NonDelaunayFacesCavityNodes.end(), next.self().get_node()) == m_NonDelaunayFacesCavityNodes.end())
                                {
                                    m_NonDelaunayFacesCavityNodes.push_back(next.self().get_node().self());
                                }

                                next = next.get_next();
                            } while (next.is_not_null() && next.self() != nonDelaunayFace.self().get_edge());
                        }

                        // Order not-Delaunay faces cavity nodes around newly inserted point
                        // in clock-wise or counter-clock-wise order to build new cavity faces correctly
                        std::sort(
                            m_NonDelaunayFacesCavityNodes.begin(),
                            m_NonDelaunayFacesCavityNodes.end(),
                            [newNode](const Surface::NodeHandle& _A, const Surface::NodeHandle& _B)
                            {
                                if(gs_abs(
                                    gs_vector_argument(_A.self().get_data() - newNode.self().get_data()) -
                                    gs_vector_argument(_B.self().get_data() - newNode.self().get_data())) < 1e-4)
                                {
                                    return gs_vector_length(_A.self().get_data() - newNode.self().get_data()) <
                                            gs_vector_length(_B.self().get_data() - newNode.self().get_data());
                                }

                                return gs_vector_argument(_A.self().get_data() - newNode.self().get_data()) <
                                        gs_vector_argument(_B.self().get_data() - newNode.self().get_data());
                            });

                        // remove non-Delaunay faces 
                        for (auto& nonDelaunayFace : m_NonDelaunayFaces)
                            m_Mesh.remove_face(nonDelaunayFace.self());

                        // build cavity faces
                        for (int j = 0; j < (int)m_NonDelaunayFacesCavityNodes.size(); j++)
                        {
                            int a = gs_array_index_clamp(j + 0, m_NonDelaunayFacesCavityNodes.size());
                            int b = gs_array_index_clamp(j + 1, m_NonDelaunayFacesCavityNodes.size());             

                            Surface::NodeHandle cavity[3] =
                            {
                                newNode.self(),
                                m_NonDelaunayFacesCavityNodes[a].self(),
                                m_NonDelaunayFacesCavityNodes[b].self()
                            };

                            m_Mesh.add_face(
                                cavity,
                                3,
                                gs_2d_trianglef(
                                    cavity[0].self().get_data(),
                                    cavity[1].self().get_data(),
                                    cavity[2].self().get_data()));
                        }
                    }

                    // commit not null faces that don't share vertexes with bounding triangle
                    for(auto& face : m_Mesh.get_faces())
                    {
                        if( face.is_not_null()                                    &&
                            boundingRectangle.contains(face.self().get_data().P1) &&
                            boundingRectangle.contains(face.self().get_data().P2) &&
                            boundingRectangle.contains(face.self().get_data().P3))
                        {
                            _Commit(face.self().get_data());
                        }
                    }
                }

            protected:

                // info
                Surface                          m_Mesh                        {Surface()};
                std::vector<Surface::FaceHandle> m_NonDelaunayFaces            {std::vector<Surface::FaceHandle>()};
                std::vector<Surface::NodeHandle> m_NonDelaunayFacesCavityNodes {std::vector<Surface::NodeHandle>()};
                std::vector<gs_vec2f>            m_MeshVertexesPerturbations   {std::vector<gs_vec2f>()};
            };

            /*! @} */
        }
    }
}