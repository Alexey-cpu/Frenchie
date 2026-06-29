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
            class BowyerWatsonDelaunator2D
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
                        if(face.is_null()) continue;

                        if(!m_DiscardSuperTriangle) 
                        {
                            _Commit(face.self().get_data());
                            continue;
                        }

                        if( boundingRectangle.contains(face.self().get_data().P1) &&
                            boundingRectangle.contains(face.self().get_data().P2) &&
                            boundingRectangle.contains(face.self().get_data().P3))
                        {
                            _Commit(face.self().get_data());
                        }
                    }
                }

                bool m_DiscardSuperTriangle = true;

            protected:

                // info
                Surface                          m_Mesh                        {Surface()};
                std::vector<Surface::FaceHandle> m_NonDelaunayFaces            {std::vector<Surface::FaceHandle>()};
                std::vector<Surface::NodeHandle> m_NonDelaunayFacesCavityNodes {std::vector<Surface::NodeHandle>()};
                std::vector<gs_vec2f>            m_MeshVertexesPerturbations   {std::vector<gs_vec2f>()};
            };

            void Sloan_Delaunay_triangulation_2d(
                const gs_vec2f                                           _Points[],
                const int&                                               _Count,
                std::shared_ptr<Frenchie::Application::RenderingQueue2D> _Renderer)
            {    
                typedef Frenchie::Core::Mesh::Mesh<gs_vec2f> Surface;

                // auxiliary lambdas
                auto get_face_triangle = [](const Surface& _Mesh, const Surface::FaceHandle& _Face)->gs_2d_trianglef
                {
                    if(_Face.is_null())
                        return gs_2d_trianglef();

                    auto start = _Face.self().get_edge();
                    auto next  = start;
                    int  cnt = 0;

                    gs_vec2f cavity[3]{};

                    do
                    {
                        if(cnt < 3 && next.is_not_null())
                            cavity[cnt] = next.self().get_node().self().get_data();
                        next = next.get_next();
                        cnt++;
                    }
                    while (next.is_not_null() && next != start);
                    
                    return gs_2d_trianglef(cavity[0], cavity[1], cavity[2]);
                };

                auto find_face_containing_node = [&get_face_triangle](const Surface& _Mesh, const Surface::NodeHandle& _Node)->Surface::FaceHandle
                {
                    if(_Node.is_null())
                        return Surface::FaceHandle();

                    for(auto& face : _Mesh.get_faces())
                    {
                        if(face.is_not_null() && get_face_triangle(_Mesh, face.self()).contains(_Node.self().get_data()))
                            return face;
                    }

                    return Surface::FaceHandle();
                };

                auto split_face_by_node = [](Surface& _Mesh, const Surface::FaceHandle& _Face, const Surface::NodeHandle& _Node)->std::vector<Surface::FaceHandle>
                {
                    if(_Face.is_null() || _Node.is_null())
                        return std::vector<Surface::FaceHandle>();
                    
                    // retrieve face nodes
                    std::vector<Surface::NodeHandle> nodes;
                    
                    auto start = _Face.self().get_edge();
                    auto next  = start;

                    do
                    {
                        nodes.push_back(next.self().get_node().self());
                        next = next.self().get_next();
                    }
                    while (next.is_not_null() && next.self() != start.self());

                    // remove the face
                    _Mesh.remove_face(_Face.self());

                    // generate new faces
                    std::vector<Surface::FaceHandle> faces;

                    for (int i = 0; i < (int)nodes.size(); i++)
                    {
                        int a = gs_array_index_clamp(i + 0, (int)nodes.size());
                        int b = gs_array_index_clamp(i + 1, (int)nodes.size());
                        faces.push_back(_Mesh.add_face({_Node.self(), nodes[a].self(), nodes[b].self()}));
                    }

                    return faces;
                };

                auto find_neighbours_opposite_to_node = [](const Surface& _Mesh, const Surface::FaceHandle& _Face, const Surface::NodeHandle& _Node)->std::vector<Surface::FaceHandle>
                {
                    if(_Face.is_null() || _Node.is_null())
                        return std::vector<Surface::FaceHandle>();

                    std::vector<Surface::FaceHandle> neighbours;

                    auto start = _Face.self().get_edge();
                    auto next  = start;

                    do
                    {
                        if(
                            next.self().get_twin().is_not_null() &&
                            next.self().get_node().self() != _Node      &&
                            next.self().get_twin().self().get_node() != _Node //&& next.self().get_twin().self().get_face() != _Face
                        )
                        {
                            neighbours.push_back(next.self().get_twin().self().get_face().self());
                        }

                        next = next.get_next();
                    }
                    while (next.is_not_null() && next != start);
                    
                    return neighbours;
                };

                auto find_neighbour_that_has_node = [](const Surface& _Mesh, const Surface::FaceHandle& _Face, const Surface::NodeHandle& _Node)->Surface::FaceHandle
                {
                    if(_Face.is_null() || _Node.is_null())
                        return Surface::FaceHandle();

                    auto start = _Face.self().get_edge();
                    auto next  = start;

                    do
                    {
                        if(next.self().get_twin().is_not_null())
                        {
                            auto startTwin = next.self().get_twin().self();
                            auto nextTwin  = startTwin;

                            do
                            {
                                if(nextTwin.self().get_node().self() == _Node.self())
                                    return nextTwin.self().get_face();

                                nextTwin = nextTwin.self().get_next();
                            }
                            while (nextTwin.is_not_null() && nextTwin.self() != startTwin.self());
                        }

                        next = next.self().get_next();
                    }
                    while (next.is_not_null() && next.self() != start.self());
                    
                    return Surface::FaceHandle();
                };

                auto is_convex_quadrilateral = [&get_face_triangle](
                    const Surface& _Mesh,
                    const Surface::FaceHandle& _Left,
                    const Surface::FaceHandle& _Right,
                    std::shared_ptr<Frenchie::Application::RenderingQueue2D> _Renderer)->bool
                {
                    if(_Left.is_null() || _Right.is_null())
                        return false;

                    std::vector<Surface::NodeHandle> nodes;
                    Surface::EdgeHandle diagonal;

                    {
                        auto start = _Left.self().get_edge().self();
                        auto next  = start;
                        
                        do
                        {
                            if(std::find(nodes.begin(), nodes.end(), next.self().get_node()) == nodes.end())
                                nodes.push_back(next.self().get_node().self());

                            if(
                                next.self().get_twin().is_not_null() &&
                                next.self().get_twin().self().get_face().self() == _Right.self()) diagonal = next;

                            next = next.get_next();
                        }
                        while (next.is_not_null() && next != start);
                    }
                
                    {
                        auto start = _Right.self().get_edge();
                        auto next  = start;
                        
                        do
                        {
                            if(std::find(nodes.begin(), nodes.end(), next.self().get_node().self()) == nodes.end())
                                nodes.push_back(next.self().get_node().self());

                            if(
                                next.self().get_twin().is_not_null() &&
                                next.self().get_twin().self().get_face().self() == _Left.self()) diagonal = next;

                            next = next.get_next();
                        }
                        while (next.is_not_null() && next != start);
                    }

                    gs_vec2f center = (diagonal.self().get_node().get_data() + diagonal.self().get_next().self().get_node().get_data()) * 0.5f;

                    std::sort(
                        nodes.begin(),
                        nodes.end(),
                        [center](const Surface::NodeHandle& _A, const Surface::NodeHandle& _B)
                        {
                            if(gs_abs(
                                gs_vector_argument(_A.self().get_data() - center) -
                                gs_vector_argument(_B.self().get_data() - center)) < 1e-4)
                            {
                                return gs_vector_length(_A.self().get_data() - center) <
                                        gs_vector_length(_B.self().get_data() - center);
                            }

                            return gs_vector_argument(_A.self().get_data() - center) <
                                    gs_vector_argument(_B.self().get_data() - center);
                        });

                        // for (int j = 0; j < nodes.size() - 2; j++)
                        // {
                        //     int a = gs_array_index_clamp(j + 0, (int)nodes.size());
                        //     int b = gs_array_index_clamp(j + 1, (int)nodes.size());

                        //     _Renderer->push_line(
                        //         nodes[a].get_data(),
                        //         nodes[b].get_data(),
                        //         12.f,
                        //         gs_color_rgb(0, 0, 0),
                        //         _Renderer->calculate_transform_matrix(+500.f)
                        //     );
                        // }

                        Frenchie::Core::Optional<int> sign;

                        for (int j = 0; j < nodes.size(); j++)
                        {
                            int a = gs_array_index_clamp(j + 0, (int)nodes.size());
                            int b = gs_array_index_clamp(j - 1, (int)nodes.size());
                            int c = gs_array_index_clamp(j + 1, (int)nodes.size());

                            int d = (int)gs_sign(gs_vector_cross(nodes[a].get_data() - nodes[b].get_data(), nodes[a].get_data() - nodes[c].get_data()));

                            if(sign.has_value() && sign.value() != d)
                                return false;

                            sign = d;
                        }

                    return true;
                };

                // driver code
                gs_2d_trianglef boundingTriangle  = gs_2d_trianglef(_Points, _Count);
                gs_2d_boxf      boundingRectangle = gs_2d_boxf(_Points[0], _Points[0]);

                Surface mesh;
                mesh.add_face({mesh.add_node(boundingTriangle.P1), mesh.add_node(boundingTriangle.P2), mesh.add_node(boundingTriangle.P3)});

                for (int i = 0; i < _Count; i++)
                {
                    // construct new node
                    auto newNode = mesh.add_node(_Points[i]);

                    boundingRectangle = gs_2d_boxf(boundingRectangle.Min, boundingRectangle.Max, newNode.self().get_data(), newNode.self().get_data());

                    // find and split face containing new node
                    auto faceToSplit = find_face_containing_node(mesh, newNode.self());

                    if(faceToSplit.is_null()) continue;

                    auto newFaces = split_face_by_node(mesh, faceToSplit.self(), newNode.self());

                    // find neighbouring faces sharing the edge opposite to the new node into the stack
                    std::vector<Surface::FaceHandle> stack;

                    for(auto& newFace : newFaces)
                    {
                        auto neighbours = find_neighbours_opposite_to_node(mesh, newFace, newNode);
                        for(auto& neighbour : neighbours)
                            stack.push_back(neighbour);
                    }

                    // swap diagonals
                    while (!stack.empty())
                    {
                        auto top = stack[stack.size() - 1];
                        stack.pop_back();

                        if(!get_face_triangle(mesh, top.self()).circum_circle().contains(newNode.self().get_data()))
                            continue;

                        auto neighbour = find_neighbour_that_has_node(mesh, top.self(), newNode.self());

                        if(neighbour.is_null()) continue;

                        auto start = neighbour.self().get_edge();
                        auto next  = start;

                        if(!is_convex_quadrilateral(mesh, top, neighbour, _Renderer)) continue;

                        do
                        {
                            if(
                                next.self().get_twin().is_not_null() &&
                                next.self().get_twin().self().get_face() == top.self() &&
                                mesh.flip_edge(next.self()))
                            {
                                {
                                    auto neighbours = find_neighbours_opposite_to_node(mesh, neighbour.self(), newNode.self());
                                    for(auto& neighbour : neighbours)
                                        stack.push_back(neighbour);
                                }

                                {
                                    auto neighbours = find_neighbours_opposite_to_node(mesh, top.self(), newNode.self());
                                    for(auto& neighbour : neighbours)
                                        stack.push_back(neighbour);
                                }

                                break;
                            }

                            next = next.self().get_next();
                        }
                        while (next.is_not_null() && next != start);
                    }
                }

                // render faces
                for(auto& face : mesh.get_faces())
                {
                    if(face.is_null()) continue;

                    auto _Triangle = get_face_triangle(mesh, face.self());

                    if( boundingRectangle.contains(_Triangle.P1) &&
                        boundingRectangle.contains(_Triangle.P2) &&
                        boundingRectangle.contains(_Triangle.P3))
                    {
                        // if(_Triangle.contains(_Renderer->get_cursor_postion()))
                        // {
                        //     _Renderer->push_triangle_filled(
                        //         _Triangle.P1,
                        //         _Triangle.P2,
                        //         _Triangle.P3,
                        //         gs_color_rgb(255, 0, 0),
                        //         _Renderer->calculate_transform_matrix((float)0)
                        //     );
                            
                        //     auto ellipse = _Triangle.circum_circle();

                        //     _Renderer->push_arc(
                        //         ellipse.Center,
                        //         ellipse.MinorRadius,
                        //         ellipse.MajorRadius,
                        //         0.f,
                        //         360.f,
                        //         gs_color_rgb(255, 0, 0),
                        //         12.f
                        //     );
                        // }

                        _Renderer->push_triangle(
                            _Triangle.P1,
                            _Triangle.P2,
                            _Triangle.P3,
                            gs_color_rgb(255, 0, 0),
                            4.f,
                            _Renderer->calculate_transform_matrix((float)0)
                        );

                        _Renderer->push_arc_filled(
                            _Triangle.P1,
                            16.f,
                            16.f,
                            0.f,
                            360.f,
                            gs_color_rgb(255, 0, 0),
                            _Renderer->calculate_transform_matrix((float)0));

                        _Renderer->push_arc_filled(
                            _Triangle.P2,
                            16.f,
                            16.f,
                            0.f,
                            360.f,
                            gs_color_rgb(255, 0, 0),
                            _Renderer->calculate_transform_matrix((float)0));
                    }
                }
            }

            /*! @} */
        }
    }
}