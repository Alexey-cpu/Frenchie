#include <FrenchieCoreMeshSurface2DTriangulations.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Mesh;

// AbstractDelaunator2D
AbstractDelaunator2D::AbstractDelaunator2D()
{
    for (int i = 0; i < (int)m_MeshVertexesPerturbations.size(); i++)
        m_MeshVertexesPerturbations.push(gs_pseudo_random<float>(-gs_tiny<float>(), +gs_tiny<float>()) * 2.f);
}

AbstractDelaunator2D::~AbstractDelaunator2D(){}

void AbstractDelaunator2D::discard_bounding_triangle()
{
    m_DiscardSuperTriangle = true;
}

void AbstractDelaunator2D::dont_discard_bounding_triangle()
{
    m_DiscardSuperTriangle = false;
}

gs_2d_trianglef AbstractDelaunator2D::get_face_triangle(
    const Surface2D&             _Mesh,
    const Surface2D::FaceHandle& _Face)
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

// BowyerWatsonDelaunator2D
BowyerWatsonDelaunator2D::BowyerWatsonDelaunator2D(){}
BowyerWatsonDelaunator2D::~BowyerWatsonDelaunator2D(){}

gs_2d_boxf BowyerWatsonDelaunator2D::build_triangulated_mesh(const gs_vec2f _Points[], const int& _Count)
{
    if(_Points == nullptr || _Count <= 0)
        return gs_2d_boxf();

    // get ready
    m_Mesh.flush();
    m_NonDelaunayFaces.clear();
    m_NonDelaunayFacesCavityNodes.clear();

    // driver code
    gs_2d_trianglef boundingTriangle  = gs_2d_trianglef(_Points, _Count);
    gs_2d_boxf      boundingRectangle = gs_2d_boxf(_Points[0], _Points[0]);
    m_Mesh.add_face({m_Mesh.add_node(boundingTriangle.P1), m_Mesh.add_node(boundingTriangle.P2), m_Mesh.add_node(boundingTriangle.P3)});

    for (int i = 0; i < _Count; i++)
    {
        // add a little random perturbation to a point coordinate to move it to a general position.
        // This guarantees degeneracies absence in general case.
        Surface2D::NodeHandle newNode = m_Mesh.add_node(_Points[i] + m_MeshVertexesPerturbations.at(i));
        
        // recompute bounding rectangle considering perturbations
        boundingRectangle = gs_2d_boxf(boundingRectangle.Min, boundingRectangle.Max, newNode.self().get_data());

        // find not-Delaunay faces
        m_NonDelaunayFaces.clear();

        for (auto& face : m_Mesh.get_faces())
        {
            if(face.is_not_null() && get_face_triangle(m_Mesh, face).circum_circle().contains(newNode.self().get_data()))
                m_NonDelaunayFaces.push_back(face.self());
        }

        // dig not-Delaunay faces cavity nodes
        m_NonDelaunayFacesCavityNodes.clear();

        for (auto& nonDelaunayFace : m_NonDelaunayFaces)
        {
            Surface2D::EdgeHandle next = nonDelaunayFace.self().get_edge();

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
            [newNode](const Surface2D::NodeHandle& _A, const Surface2D::NodeHandle& _B)
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
            m_Mesh.add_face(
                {
                    newNode.self(),
                    m_NonDelaunayFacesCavityNodes[gs_array_index_clamp(j + 0, m_NonDelaunayFacesCavityNodes.size())].self(),
                    m_NonDelaunayFacesCavityNodes[gs_array_index_clamp(j + 1, m_NonDelaunayFacesCavityNodes.size())].self()
                }
            );
        }
    }

    return boundingRectangle;
}

// SloanDelaunator2D
SloanDelaunator2D::SloanDelaunator2D(){}
SloanDelaunator2D::~SloanDelaunator2D(){}

gs_2d_boxf SloanDelaunator2D::build_triangulated_mesh(const gs_vec2f _Points[], const int& _Count)
{
    // get ready
    m_Mesh.flush();

    // driver code
    gs_2d_trianglef boundingTriangle  = gs_2d_trianglef(_Points, _Count);
    gs_2d_boxf      boundingRectangle = gs_2d_boxf(_Points[0], _Points[0]);
    
    m_Mesh.add_face(
        {
            m_Mesh.add_node(boundingTriangle.P1),
            m_Mesh.add_node(boundingTriangle.P2),
            m_Mesh.add_node(boundingTriangle.P3)});

    for (int i = 0; i < _Count; i++)
    {
        // construct new node
        Surface2D::NodeHandle newNode = m_Mesh.add_node(_Points[i] + m_MeshVertexesPerturbations.at(i));
        boundingRectangle = gs_2d_boxf(boundingRectangle.Min, boundingRectangle.Max, newNode.self().get_data(), newNode.self().get_data());

        // find and split face containing new node
        Surface2D::FaceHandle faceToSplit = find_face_containing_node(m_Mesh, newNode.self());

        if(faceToSplit.is_null())
            continue;

        std::vector<Surface2D::FaceHandle> newFaces = split_face_by_node(m_Mesh, faceToSplit.self(), newNode.self());

        // find neighbouring faces sharing the edge opposite to the new node into the stack
        std::vector<Surface2D::FaceHandle> stack;

        for(auto& newFace : newFaces)
        {
            std::vector<Surface2D::FaceHandle> neighbours =
                find_neighbours_opposite_to_node(m_Mesh, newFace, newNode);

            for(auto& neighbour : neighbours)
                stack.push_back(neighbour);
        }

        // swap diagonals
        while (!stack.empty())
        {
            auto top = stack[stack.size() - 1];
            stack.pop_back();

            if(!get_face_triangle(m_Mesh, top.self()).circum_circle().contains(newNode.self().get_data()))
                continue;

            Surface2D::FaceHandle neighbour = find_neighbour_that_has_node(m_Mesh, top.self(), newNode.self());

            if(neighbour.is_null()) continue;

            Surface2D::EdgeHandle start = neighbour.self().get_edge();
            Surface2D::EdgeHandle next  = start;

            if(!is_convex_quadrilateral(m_Mesh, top, neighbour)) continue;

            do
            {
                if(
                    next.self().get_twin().is_not_null() &&
                    next.self().get_twin().self().get_face() == top.self() &&
                    m_Mesh.flip_edge(next.self()))
                {
                    {
                        std::vector<Surface2D::FaceHandle> neighbours =
                            find_neighbours_opposite_to_node(m_Mesh, neighbour.self(), newNode.self());
                        for(auto& neighbour : neighbours)
                            stack.push_back(neighbour);
                    }

                    {
                        std::vector<Surface2D::FaceHandle> neighbours =
                            find_neighbours_opposite_to_node(m_Mesh, top.self(), newNode.self());
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

    return boundingRectangle;
}

Surface2D::FaceHandle
SloanDelaunator2D::find_face_containing_node(
    const Surface2D&             _Mesh,
    const Surface2D::NodeHandle& _Node)
{
    if(_Node.is_null())
        return Surface2D::FaceHandle();

    for(auto& face : _Mesh.get_faces())
    {
        if(face.is_not_null() && get_face_triangle(_Mesh, face.self()).contains(_Node.self().get_data()))
            return face;
    }

    return Surface2D::FaceHandle();
};

std::vector<Surface2D::FaceHandle>
SloanDelaunator2D::split_face_by_node(
    Surface2D&                   _Mesh,
    const Surface2D::FaceHandle& _Face,
    const Surface2D::NodeHandle& _Node)
{
    if(_Face.is_null() || _Node.is_null())
        return std::vector<Surface2D::FaceHandle>();
    
    // retrieve face nodes
    std::vector<Surface2D::NodeHandle> nodes;
    
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
    std::vector<Surface2D::FaceHandle> faces;

    for (int i = 0; i < (int)nodes.size(); i++)
    {
        faces.push_back(
            _Mesh.add_face(
                {
                    _Node.self(),
                    nodes[gs_array_index_clamp(i + 0, (int)nodes.size())].self(),
                    nodes[gs_array_index_clamp(i + 1, (int)nodes.size())].self()
                }
            )
        );
    }

    return faces;
};

std::vector<Surface2D::FaceHandle>
SloanDelaunator2D::find_neighbours_opposite_to_node(
    const Surface2D&             _Mesh,
    const Surface2D::FaceHandle& _Face,
    const Surface2D::NodeHandle& _Node)
{
    if(_Face.is_null() || _Node.is_null())
        return std::vector<Surface2D::FaceHandle>();

    std::vector<Surface2D::FaceHandle> neighbours;

    auto start = _Face.self().get_edge();
    auto next  = start;

    do
    {
        if(
            next.self().get_twin().is_not_null()   &&
            next.self().get_node().self() != _Node &&
            next.self().get_twin().self().get_node() != _Node
        )
        {
            neighbours.push_back(next.self().get_twin().self().get_face().self());
        }

        next = next.get_next();
    }
    while (next.is_not_null() && next != start);
    
    return neighbours;
};

Surface2D::FaceHandle
SloanDelaunator2D::find_neighbour_that_has_node(
    const Surface2D&             _Mesh,
    const Surface2D::FaceHandle& _Face,
    const Surface2D::NodeHandle& _Node)
{
    if(_Face.is_null() || _Node.is_null())
        return Surface2D::FaceHandle();

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
    
    return Surface2D::FaceHandle();
};

bool SloanDelaunator2D::is_convex_quadrilateral(
    const Surface2D&             _Mesh,
    const Surface2D::FaceHandle& _Left,
    const Surface2D::FaceHandle& _Right)
{
    if(_Left.is_null() || _Right.is_null())
        return false;

    std::vector<Surface2D::NodeHandle> nodes;
    Surface2D::EdgeHandle              diagonal;

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
        [center](const Surface2D::NodeHandle& _A, const Surface2D::NodeHandle& _B)
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
}