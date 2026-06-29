#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreOptional.hpp>

// STL
#include <vector>
#include <string>
#include <algorithm>

#define NULLREF -1
#define REFERENCE int

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
            /*! \defgroup <MeshModel> (Mesh)
            *  @ingroup Mesh
            *  @brief The module contains core utility functions to work 2D/3D mesh.
            * @{
            */

            // default face and node type handles
            struct DefaultNodeType{};
            struct DefaultFaceType{};
            struct DefaultEdgeType{};

            /**
             * @brief This class is a simple and straight-forward implementation of half edge data structure
             * @class  Mesh
             * @tparam NodeInfo mesh node info handle
             * @tparam FaceInfo mesh face info handle
             * @tparam EdgeInfo mesh edge info handle
             */
            template<
            typename NodeInfo = DefaultNodeType,
            typename FaceInfo = DefaultFaceType,
            typename EdgeInfo = DefaultEdgeType>
            struct Mesh final
            {
            private:
                typedef Mesh<NodeInfo, FaceInfo, EdgeInfo> MeshSurface;

            public:

                struct NodeHandle;
                struct FaceHandle;
                struct EdgeHandle;
                struct PathFinder;

                typedef NodeInfo NodeInfoType;
                typedef FaceInfo FaceInfoType;
                typedef EdgeInfo EdgeInfoType;

                // nested types
                template<typename HandleType, typename HandleInfoType>
                struct Handle
                {
                    explicit Handle(const MeshSurface* _Surface = nullptr, const REFERENCE&   _Reference = NULLREF) : Surface(_Surface), SelfRef(_Reference){}
                    virtual ~Handle(){}

                    // self
                    virtual HandleType& self() const = 0;

                    // getters
                    REFERENCE get_ref() const
                    {
                        return SelfRef;
                    }

                    const MeshSurface* get_surface() const
                    {
                        return Surface;
                    }

                    HandleInfoType get_data() const
                    {
                        return Info;
                    }

                    // setters
                    void set_ref(const REFERENCE& _Ref)
                    {
                        SelfRef = _Ref;
                    }

                    void set_surface(const MeshSurface* _Surface)
                    {
                        Surface = _Surface;
                    }

                    void set_data(const HandleInfoType& _Info) const
                    {
                        Info = _Info;
                    }

                    // operators
                    bool operator == (const Handle& _Other) const
                    {
                        return _Other.SelfRef == SelfRef && _Other.Surface == Surface;
                    }

                    bool operator != (const Handle& _Other) const
                    {
                        return _Other.SelfRef != SelfRef || _Other.Surface != Surface;
                    }

                    bool operator < (const Handle& _Other) const
                    {
                        return SelfRef < _Other.SelfRef;
                    }

                    bool operator > (const Handle& _Other) const
                    {
                        return SelfRef > _Other.SelfRef;
                    }

                    bool is_null() const
                    {
                        return SelfRef == NULLREF || Surface == nullptr;
                    }

                    bool is_not_null() const
                    {
                        return !is_null();
                    }

                protected:
                    const   MeshSurface*   Surface {nullptr};
                    mutable REFERENCE      SelfRef {NULLREF};
                    mutable HandleInfoType Info    {HandleInfoType()};
                };

                struct NodeHandle final : public Handle<NodeHandle, NodeInfo>
                {
                    explicit NodeHandle(const MeshSurface* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) :
                        Handle<NodeHandle, NodeInfo>(_Surface, _Reference){}
                    virtual ~NodeHandle(){}

                    // self
                    NodeHandle& self() const
                    {
                        return this->Surface->Nodes[this->SelfRef];
                    }

                    // getters
                    EdgeHandle get_edge() const
                    {
                        return
                            this->EdgeRef != NULLREF && this->Surface != nullptr && this->EdgeRef < this->Surface->Edges.size() ?
                                this->Surface->Edges[this->EdgeRef] :
                                    EdgeHandle();
                    }

                    // setters
                    void set_edge(const EdgeHandle& _Edge)
                    {
                        EdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;
                    }

                protected:
                    REFERENCE EdgeRef {NULLREF};
                };

                struct FaceHandle final : public Handle<FaceHandle, FaceInfo>
                {
                    explicit FaceHandle(const MeshSurface* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) :
                        Handle<FaceHandle, FaceInfo>(_Surface, _Reference){}
                    virtual ~FaceHandle(){}

                    // self
                    FaceHandle& self() const
                    {
                        return this->Surface->Faces[this->SelfRef];
                    }

                    // getters
                    EdgeHandle get_edge() const
                    {
                        return
                            this->EdgeRef != NULLREF && this->Surface != nullptr && this->EdgeRef < this->Surface->Edges.size() ?
                                this->Surface->Edges[this->EdgeRef] :
                                    EdgeHandle();
                    }

                    // setters
                    void set_edge(const EdgeHandle& _Edge)
                    {
                        EdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;
                    }

                protected:
                    REFERENCE EdgeRef {NULLREF};
                };

                struct EdgeHandle final : public Handle<EdgeHandle, EdgeInfo>
                {
                    // MeshEdgeHandle
                    explicit EdgeHandle(const MeshSurface* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) :
                        Handle<EdgeHandle, EdgeInfo>(_Surface, _Reference){}
                    virtual ~EdgeHandle(){}

                    // self
                    EdgeHandle& self() const
                    {
                        return this->Surface->Edges[this->SelfRef];
                    }

                    // getters
                    NodeHandle get_node() const
                    {
                        return
                            this->NodeRef != NULLREF && this->Surface != nullptr  && this->NodeRef < this->Surface->Nodes.size() ?
                                this->Surface->Nodes[this->NodeRef] :
                                    NodeHandle();
                    }

                    FaceHandle get_face() const
                    {
                        return
                            this->FaceRef != NULLREF && this->Surface != nullptr  && this->FaceRef < this->Surface->Faces.size() ?
                                this->Surface->Faces[this->FaceRef] :
                                    FaceHandle();
                    }

                    EdgeHandle get_next() const
                    {
                        return
                            this->NextEdgeRef != NULLREF && this->Surface != nullptr  && this->NextEdgeRef < this->Surface->Edges.size() ?
                                this->Surface->Edges[NextEdgeRef] :
                                    EdgeHandle();
                    }

                    EdgeHandle get_prev() const
                    {
                        return
                            this->PrevEdgeRef != NULLREF && this->Surface != nullptr  && this->PrevEdgeRef < this->Surface->Edges.size() ?
                                this->Surface->Edges[PrevEdgeRef] :
                                    EdgeHandle();
                    }

                    EdgeHandle get_twin() const
                    {
                        return
                            this->TwinEdgeRef != NULLREF && this->Surface != nullptr && (this->TwinEdgeRef) < this->Surface->Edges.size() ?
                                this->Surface->Edges[this->TwinEdgeRef] :
                                    EdgeHandle();
                    }

                    // setters
                    void set_node(const NodeHandle& _Node)
                    {
                        NodeRef = _Node.is_not_null() ? _Node.self().get_ref() : NULLREF;
                    }

                    void set_face(const FaceHandle& _Face)
                    {
                        FaceRef = _Face.is_not_null() ? _Face.self().get_ref() : NULLREF;
                    }

                    void set_next(const EdgeHandle& _Edge)
                    {
                        NextEdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;

                        if(_Edge.is_not_null())
                            _Edge.self().PrevEdgeRef = get_ref();
                    }

                    void set_prev(const EdgeHandle& _Edge)
                    {
                        PrevEdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;

                        if(_Edge.is_not_null())
                            _Edge.self().NextEdgeRef = get_ref();
                    }

                    void set_twin(const EdgeHandle& _Edge)
                    {
                        TwinEdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;
                        
                        if(_Edge.is_not_null())
                            _Edge.self().TwinEdgeRef = get_ref();
                    }

                protected:
                    REFERENCE NodeRef     {NULLREF};
                    REFERENCE FaceRef     {NULLREF};
                    REFERENCE NextEdgeRef {NULLREF};
                    REFERENCE PrevEdgeRef {NULLREF};
                    REFERENCE TwinEdgeRef {NULLREF};
                };

                struct PathFinder final
                {
                    explicit PathFinder(const MeshSurface* _Surface) : Surface(_Surface){}

                    // nested types
                    enum EdgeDirection
                    {
                        None,
                        Forward,
                        Backward,
                    };

                    // API
                    EdgeHandle edge_source_end(const NodeHandle& _Source, const NodeHandle& _Target) const
                    {
                        for(auto& edge : Surface->get_edges())
                        {
                            if(edge.is_null()) continue;

                            if(
                                edge.self().get_node() == _Source.self() &&
                                edge.self().get_next().is_not_null()     &&
                                edge.self().get_next().get_node() == _Target.self()) return edge;
                        }

                        return EdgeHandle(); 
                    }

                    EdgeHandle edge_target_end(const NodeHandle& _Source, const NodeHandle& _Target) const
                    {
                        for(auto& edge : Surface->get_edges())
                        {
                            if(edge.is_null()) continue;

                            if(
                                edge.self().get_node() == _Source.self() &&
                                edge.self().get_next().is_not_null()     &&
                                edge.self().get_next().get_node() == _Target.self()) return edge.self().get_next();
                        }

                        return EdgeHandle(); 
                    }

                    EdgeDirection next_edge_direction(const NodeHandle& _Source, const NodeHandle& _Target) const
                    {
                        EdgeDirection direction = EdgeDirection::None;

                        for(auto& edge : Surface->get_edges())
                        {
                            if(edge.is_null()) continue;

                            if(
                                edge.self().get_node() == _Source.self() &&
                                edge.self().get_next().is_not_null()     &&
                                edge.self().get_next().get_node() == _Target.self())
                            {
                                direction = EdgeDirection::Backward;
                            }

                            if(
                                edge.self().get_node() == _Target.self() &&
                                edge.self().get_next().is_not_null()     &&
                                edge.self().get_next().get_node() == _Source.self())
                            {
                                direction = EdgeDirection::Forward;
                            }
                        }

                        return direction;
                    }

                private:

                    // info
                    const MeshSurface* Surface;
                };

                // node API
                /**
                 * @brief This function inserts new node into a mesh
                 * @param _NodeData node info data which type is defined by 'Node' template paramter of mesh
                 * @return returns created node 
                 */
                NodeHandle add_node(const NodeInfo& _NodeData = NodeInfo() )
                {
                    NodeHandle node = create_node();
                    node.self().set_data(_NodeData);
                    return node;
                }

                // face API

                /**
                 * @brief This function creates new face within mesh
                 * @param _Nodes input nodes that all have to belong to this face and they MUST BE not null
                 * @param _Count input nodes count
                 * @param _FaceData face data which type is defined by 'Face' template paramter of mesh
                 * @return returns newly created face or null face if something went wrong: one or more nodes are null or 
                 * one or more nodes belong to a different face 
                 */
                FaceHandle add_face(const NodeHandle _Nodes[], const int& _Count, const FaceInfo& _FaceData = FaceInfo())
                {
                    // nested types
                    struct PathElement final
                    {
                        PathElement(
                            const NodeHandle& _Source,
                            const NodeHandle& _Target,
                            const bool&       _Swapped,
                            const bool&       _Existing) :
                                Source(_Source),
                                Target(_Target),
                                Swapped(_Swapped),
                                Existing(_Existing){}

                        NodeHandle Source   {false};
                        NodeHandle Target   {false};
                        bool       Swapped  {false};
                        bool       Existing {false};
                    };

                    // driver code
                    if(_Nodes == nullptr || _Count < 3)
                        return FaceHandle();

                    for (int i = 0; i < _Count; i++)
                    {
                        if(_Nodes[i].is_null() || _Nodes[i].get_surface() != this)
                            return FaceHandle();
                    }

                    // this is mesh navigation helper class
                    PathFinder pathFinder(this);

                    // generate path
                    std::vector<PathElement> path;

                    for (int i = 0; i < _Count; i++)
                    {
                        int s = gs_array_index_clamp(i + 0, _Count);
                        int t = gs_array_index_clamp(i + 1, _Count);

                        // Every existing edge MUST BE enclosed by backward looking twin,
                        // so if there is yet another one existing edge we create backward
                        // or forward looking twin untill the sequence is equilibrated, i.e
                        // the number of forward looking existing edges is equal to the number
                        // of backward looking edges
                        switch (pathFinder.next_edge_direction(_Nodes[s], _Nodes[t]))
                        {
                        case PathFinder::EdgeDirection::Backward:
                            path.push_back(PathElement(_Nodes[t], _Nodes[s], true, true));
                            break;
                        
                        case PathFinder::EdgeDirection::Forward:
                            path.push_back(PathElement(_Nodes[s], _Nodes[t], true, true));
                            break;

                        default:
                            path.push_back(PathElement(_Nodes[s], _Nodes[t], false, false));
                            break;
                        }
                    }
                    
                    // Every face MUST BE clock wise or counter-clock-wise ordered, so to suport this we
                    // swap source and target nodes of path elements that are starting
                    // or ending at the same node untill there are no such elements
                    while ([](std::vector<PathElement>& _Path)->bool
                    {
                        for (int s = 0; s < (int)_Path.size(); s++)
                        {
                            for (int t = 0; t < (int)_Path.size(); t++)
                            {
                                if(s == t) continue;

                                if (_Path[s].Source.self() != _Path[t].Source.self() &&
                                    _Path[s].Target.self() != _Path[t].Target.self()) continue;

                                if(!_Path[s].Swapped && !_Path[s].Existing)
                                {
                                    gs_swap(_Path[s].Source, _Path[s].Target);
                                    _Path[s].Swapped = true;
                                    return true;
                                }

                                if(!_Path[t].Swapped && !_Path[t].Existing)
                                {
                                    gs_swap(_Path[t].Source, _Path[t].Target);
                                    _Path[t].Swapped = true;
                                    return true;
                                }
                            }
                        }

                        return false;
                    }(path));

                    // If we could not swap path elements the way they do not contain any edges
                    // starting or ending at the same node we return an empty null face
                    for (int s = 0; s < (int)path.size(); s++)
                    {
                        for (int t = 0; t < (int)path.size(); t++)
                        {
                            if(s == t) continue;

                            if (path[s].Source.self() != path[t].Source.self() &&
                                path[s].Target.self() != path[t].Target.self()) continue;

                            return FaceHandle();
                        }
                    }

                    // create face
                    FaceHandle face = create_face();
                    face.self().set_data(_FaceData);

                    // create half edges for corresponding not exsting path elements
                    std::vector<EdgeHandle> edges;

                    for (size_t i = 0; i < path.size(); i++)
                    {
                        if(path[i].Existing)
                            continue;

                        EdgeHandle edge = create_edge(path[i].Source);
                        face.self().set_edge(edge);
                        edges.push_back(edge);
                    }

                    // create twins for corresponding exsting path elements
                    for (size_t i = 0; i < path.size(); i++)
                    {
                        if(!path[i].Existing)
                            continue;

                        EdgeHandle twin = create_edge(pathFinder.edge_target_end(path[i].Target, path[i].Source).get_node());
                        twin.self().set_twin(pathFinder.edge_source_end(path[i].Target, path[i].Source));
                        face.self().set_edge(twin);
                        edges.push_back(twin);
                    }

                    for (size_t i = 0; i < path.size(); i++)
                    {
                        EdgeHandle e1;
                        EdgeHandle e2;

                        // find the new edge starting at source node
                        for (size_t j = 0; j < edges.size(); j++)
                        {
                            if(path[i].Source.self() == edges[j].self().get_node())
                            {
                                e1 = edges[j].self();
                                break;
                            }
                        }

                        // find the new edge starting at target node
                        for (size_t j = 0; j < edges.size(); j++)
                        {
                            if(path[i].Target.self() == edges[j].self().get_node())
                            {
                                e2 = edges[j].self();
                                break;
                            }
                        }

                        if(e1.is_not_null())
                        {
                            e1.self().set_next(e2);
                            e1.self().set_face(face);
                        }

                        if(e2.is_not_null())
                        {
                            e2.self().set_prev(e1);
                            e2.self().set_face(face);
                        }
                    }

                    return face;
                }

                /**
                 * @brief This function creates new face within mesh
                 * @param _Nodes input nodes
                 * @param _FaceData face data which type is defined by 'Face' template paramter of mesh
                 * @return returns newly created face or null face if something went wrong: one or more nodes are null or 
                 * one or more nodes belong to a different face 
                 */
                FaceHandle add_face(std::vector<NodeHandle> _Nodes, const FaceInfo& _FaceData = FaceInfo())
                {
                    return !_Nodes.empty() ? add_face(&_Nodes[0], (int)_Nodes.size(), _FaceData) : FaceHandle();
                }

                /**
                 * @brief This function removes the face out-of mesh
                 * @param _Face face to be removed
                 */
                void remove_face(const FaceHandle& _Face)
                {
                    if(_Face.is_null()) return;

                    // destroy face edges
                    auto start = _Face.self().get_edge();
                    auto curr  = _Face.self().get_edge();
                    auto next  = _Face.self().get_edge();

                    do
                    {
                        curr = next;
                        next = next.get_next();
                        destroy_edge(curr);
                    } while (next.is_not_null() && next != start);
                    
                    // destroy face itself
                    destroy_face(_Face);
                }

                // edge API
                bool flip_edge(const EdgeHandle& _Edge)
                {
                    // check that edge is not null
                    if(_Edge.is_null() || _Edge.self().get_twin().is_null() || !is_edge_manifold(_Edge))
                        return false;

                    // make copies of self and twin neighbouring nodes
                    auto selfNext = _Edge.self().get_next();
                    auto selfPrev = _Edge.self().get_prev();
                    auto twinNext = _Edge.self().get_twin().self().get_next();
                    auto twinPrev = _Edge.self().get_twin().self().get_prev();

                    // reconnect self neightbours
                    selfPrev.self().set_next(twinNext);
                    twinPrev.self().set_next(selfNext);

                    // flip self
                    _Edge.self().set_node(twinNext.get_next().get_node());
                    _Edge.self().set_next(selfNext.get_next());
                    twinNext.self().set_next(_Edge);

                    // flip twin
                    _Edge.self().get_twin().self().set_node(selfNext.get_next().get_node());
                    _Edge.self().get_twin().self().set_next(twinNext.get_next());
                    selfNext.self().set_next(_Edge.self().get_twin());

                    // setup actual faces
                    {
                        auto start = _Edge.self();
                        auto next  = start;

                        do
                        {
                            next.self().set_face(start.self().get_face());
                            next = next.get_next();
                        }
                        while (next.is_not_null() && next != start);
                        
                        start.self().get_face().self().set_edge(_Edge.self());
                    }

                    {
                        auto start = _Edge.self().get_twin().self();
                        auto next  = start;

                        do
                        {
                            next.self().set_face(start.get_face());
                            next = next.get_next();
                        }
                        while (next.is_not_null() && next != start);

                        _Edge.self().get_twin().self().get_face().self().set_edge(_Edge.self().get_twin().self());
                    }

                    return true;
                }

                /**
                 * @brief Detects if an edge is topologically manifold
                 * @param _Edge input edge
                 * @return returns 'true' if edge is not null and if it is shared by exactly two faces, and if it's twin
                 * does not form nested polygon
                 */
                bool is_edge_manifold(const EdgeHandle& _Edge) const
                {
                    if(_Edge.is_null())
                        return false;

                    // check that edge is shared by exactly two faces
                    auto next       = _Edge;
                    auto prev       = _Edge;
                    int  neighbours = 0;

                    do
                    {
                        if(prev == next.get_twin())
                            break;

                        // go further
                        prev = next;
                        next = next.get_twin();

                        ++neighbours;

                        if(neighbours > 1)
                            return false;

                    } while (next.is_not_null() && next.get_twin().is_not_null());

                    // check that edge is not shared by a nested polygon (i.e if mesh has nested polygons it's not manifold)
                    {
                        // get edge polygon
                        std::vector<NodeHandle> polygon1;

                        auto next = _Edge;

                        do
                        {
                            polygon1.push_back(next.get_node());
                            next = next.get_next();
                        } while (next.is_not_null() && next != _Edge);
                        
                        std::sort(polygon1.begin(), polygon1.end());

                        // get twin edge polygon
                        std::vector<NodeHandle> polygon2;

                        next = _Edge.self().get_twin();

                        do
                        {
                            polygon2.push_back(next.get_node());
                            next = next.get_next();
                        } while (next.is_not_null() && next != _Edge.self().get_twin());

                        std::sort(polygon2.begin(), polygon2.end());

                        if(std::equal(polygon1.begin(), polygon1.end(), polygon2.begin(), polygon2.end()))
                            return false;
                    }

                    return true;
                }

                // mesh API

                /**
                 * @brief Checks that mesh is topologically manifold
                 * @return returns true if the mesh is topologically manifold, i.e. that all it's edges are manifold and that
                 * it's faces graph has the only one connected component 
                 */
                bool is_manifold() const
                {
                    // check that all edges are manifold, i.e that they are shared by exactly two faces                    
                    for (auto& edge : get_edges())
                    {
                        if(edge.is_not_null() && !is_edge_manifold(edge))
                            return false;
                    }

                    // check that mesh faces graph has the only one connected component
                    std::vector<bool> facesMarks;
                    facesMarks.resize(get_faces().size());
                    for (size_t i = 0; i < facesMarks.size(); i++)
                        facesMarks[i] = false;

                    std::vector<FaceHandle> facesStack;

                    int facesGraphConnectedComponentsCount = 0;

                    for(auto& face : get_faces())
                    {
                        if(face.is_null() || facesMarks[face.get_ref()])
                            continue;

                        facesStack.push_back(face);

                        while (!facesStack.empty())
                        {
                            auto current = facesStack[facesStack.size() - 1];
                            facesStack.pop_back();

                            facesMarks[current.get_ref()] = true;

                            auto startEdge = current.self().get_edge();
                            auto nextEdge  = current.self().get_edge();

                            do
                            {                                
                                if(
                                    nextEdge.self().get_twin().is_not_null() &&
                                    nextEdge.self().get_twin().self().get_face().is_not_null() &&
                                    !facesMarks[nextEdge.self().get_twin().self().get_face().get_ref()])
                                {
                                    facesStack.push_back(nextEdge.self().get_twin().self().get_face());
                                }

                                nextEdge = nextEdge.get_next();
                            } while (nextEdge.is_not_null() && nextEdge != startEdge);
                            
                        }
                        
                        ++facesGraphConnectedComponentsCount;

                        if(facesGraphConnectedComponentsCount > 1)
                            return false;
                    }
                        
                    return true;
                }

                /**
                 * @brief This function removes all nodes, faces and half edges out-of mesh
                 */
                void flush()
                {
                    // clear mesh handles
                    Nodes.clear();
                    Faces.clear();
                    Edges.clear();

                    // clear mesh vacancies
                    VacantNodes.clear();
                    VacantFaces.clear();
                    VacantEdges.clear();
                }

                /**
                 * @brief This function returns all existing mesh nodes
                 */
                const std::vector<NodeHandle>& get_nodes() const
                {
                    return Nodes;
                }

                /**
                 * @brief This function returns all existing mesh edges
                 * @details Some of the edges within returned array can be null.
                 * This means that these edges have been removed, so they are just not valid
                 * and you just have to skip them while iterating
                 */
                const std::vector<EdgeHandle>& get_edges() const
                {
                    return Edges;
                }

                /**
                 * @brief This function returns all existing mesh faces
                 * @details Some of the faces within returned array can be null.
                 * This means that these faces have been removed, so they are just not valid
                 * and you just have to skip them while iterating
                 */
                const std::vector<FaceHandle>& get_faces() const
                {
                    return Faces;
                }

                Mesh(){}

                Mesh(const Mesh<NodeInfo, FaceInfo>& _Other)
                {
                    // copy nodes, edges and faces
                    Nodes = _Other.Nodes;
                    Edges = _Other.Edges;
                    Faces = _Other.Faces;

                    // setup self
                    for (auto& node : Nodes)
                        node.set_surface(this);
                    
                    for (auto& node : Edges)
                        node.set_surface(this);

                    for (auto& node : Faces)
                        node.set_surface(this);
                }

            private:

                // append-only containers
                mutable std::vector<NodeHandle> Nodes {std::vector<NodeHandle>()};
                mutable std::vector<EdgeHandle> Edges {std::vector<EdgeHandle>()};
                mutable std::vector<FaceHandle> Faces {std::vector<FaceHandle>()};

                // append only vacant indexes arrays
                mutable std::vector<int>        VacantNodes {std::vector<int>()};
                mutable std::vector<int>        VacantEdges {std::vector<int>()};
                mutable std::vector<int>        VacantFaces {std::vector<int>()};

                // service methods

                // node factory
                NodeHandle create_node()
                {
                    if(!VacantNodes.empty())
                    {
                        int vacantIndex = VacantNodes[VacantNodes.size() - 1];
                        VacantNodes.pop_back();

                        Nodes[vacantIndex].set_ref(vacantIndex);
                        Nodes[vacantIndex].set_surface(this);

                        return Nodes[vacantIndex];
                    }

                    Nodes.push_back(NodeHandle(this, (REFERENCE)Nodes.size()));
                    return Nodes[Nodes.size() - 1];
                }

                void destroy_node(const NodeHandle& _Node)
                {
                    if(_Node.is_null()) return;

                    // make vacant
                    VacantNodes.push_back(_Node.get_ref());
                    
                    // nullify
                    _Node.self().set_edge(EdgeHandle());
                    _Node.self().set_data(NodeInfo());
                    _Node.self().set_ref(NULLREF);
                }

                // face factory
                FaceHandle create_face()
                {
                    if(!VacantFaces.empty())
                    {
                        int vacantIndex = VacantFaces[VacantFaces.size() - 1];
                        VacantFaces.pop_back();

                        Faces[vacantIndex].set_ref(vacantIndex);
                        Faces[vacantIndex].set_surface(this);

                        return Faces[vacantIndex];
                    }

                    Faces.push_back(FaceHandle(this, (REFERENCE)Faces.size()));
                    return Faces[Faces.size() - 1];
                }

                void destroy_face(const FaceHandle& _Face)
                {
                    if(_Face.is_null())
                        return;
                    
                    // make vacant
                    VacantFaces.push_back(_Face.get_ref());

                    // nullify self within edge
                    if(_Face.self().get_edge().is_not_null())
                        _Face.self().get_edge().self().set_face(FaceHandle());

                    // nullify
                    _Face.self().set_edge(EdgeHandle());
                    _Face.self().set_ref(NULLREF);
                }

                // edge factory
                EdgeHandle create_edge(const NodeHandle& _Node)
                {
                    if(!VacantEdges.empty())
                    {
                        int vacantIndex = VacantEdges[VacantEdges.size() - 1];
                        VacantEdges.pop_back();

                        Edges[vacantIndex].set_ref(vacantIndex);
                        Edges[vacantIndex].set_surface(this);

                        Edges[vacantIndex].set_node(_Node.self());

                        if(_Node.self().get_edge().is_null())
                            _Node.self().set_edge(Edges[vacantIndex]);

                        return Edges[vacantIndex];
                    }

                    Edges.push_back(EdgeHandle(this, (REFERENCE)Edges.size()));
                    Edges[Edges.size() - 1].set_node(_Node.self());
                    
                    if(_Node.self().get_edge().is_null())
                        _Node.self().set_edge(Edges[Edges.size() - 1]);

                    return Edges[Edges.size() - 1];
                }

                void destroy_edge(const EdgeHandle& _Edge)
                {
                    if(_Edge.is_null())
                        return;

                    // make self vacant
                    VacantEdges.push_back(_Edge.get_ref());

                    // nullify self within twin
                    if(_Edge.self().get_twin().is_not_null())
                        _Edge.self().get_twin().self().set_twin(EdgeHandle());

                    // nullify self within node
                    if(_Edge.self().get_node().is_not_null() && _Edge.self().get_node().self().get_edge() == _Edge.self())
                        _Edge.self().get_node().self().set_edge(EdgeHandle());

                    // nullify self within face
                    if(_Edge.self().get_face().is_not_null() && _Edge.self().get_face().self().get_edge() == _Edge.self())
                        _Edge.self().get_face().self().set_edge(EdgeHandle());

                    // nullify self
                    _Edge.self().set_next(EdgeHandle());
                    _Edge.self().set_prev(EdgeHandle());
                    _Edge.self().set_twin(EdgeHandle());
                    _Edge.self().set_node(NodeHandle());
                    _Edge.self().set_face(FaceHandle());
                    _Edge.self().set_ref(NULLREF);
                }
            };
        
            /*! @} */
        }
    }
}

#undef NULLREF
#undef REFERENCE