#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreOptional.hpp>

// STL
#include <vector>
#include <string>

#ifdef DEBUGMESH
#include <iostream>
#endif

#include <algorithm>

#define NULLREF -1
#define REFERENCE int

namespace Frenchie
{
    namespace Core
    {
        template<typename Node, typename Face>
        struct Mesh
        {
        private:
            typedef Mesh<Node, Face> MeshSurface;

        public:

            struct NodeHandle;
            struct FaceHandle;
            struct EdgeHandle;
            struct PathFinder;

            typedef Node NodeType;
            typedef Face FaceType;

            // nested types
            template<typename T>
            struct Handle
            {
                explicit Handle(const MeshSurface* _Surface = nullptr, const REFERENCE&   _Reference = NULLREF) : Surface(_Surface), SelfRef(_Reference){}
                virtual ~Handle(){}

                // self
                virtual T& self() const = 0;

                // getters
                REFERENCE get_ref() const
                {
                    return SelfRef;
                }

                const MeshSurface* get_surface() const
                {
                    return Surface;
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

                // operators
                bool operator == (const Handle& _Other) const
                {
                    return _Other.SelfRef == SelfRef && _Other.Surface == Surface;
                }

                bool operator != (const Handle& _Other) const
                {
                    return _Other.SelfRef != SelfRef || _Other.Surface != Surface;
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
                const MeshSurface* Surface {nullptr};
                mutable REFERENCE  SelfRef {NULLREF};
            };

            struct NodeHandle final : public Handle<NodeHandle>
            {
                explicit NodeHandle(const MeshSurface* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) : Handle<NodeHandle>(_Surface, _Reference){}
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

                Node get_data() const
                {
                    return NodeData;
                }

                // setters
                void set_edge(const EdgeHandle& _Edge)
                {
                    EdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;
                }

                void set_data(const Node& _Data)
                {
                    NodeData = _Data;
                }

            protected:
                REFERENCE EdgeRef  {NULLREF};
                Node      NodeData {Node()};
            };

            struct FaceHandle final : public Handle<FaceHandle>
            {
                explicit FaceHandle(const MeshSurface* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) : Handle<FaceHandle>(_Surface, _Reference){}
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

                Face get_data() const
                {
                    return FaceData;
                }

                // setters
                void set_edge(const EdgeHandle& _Edge)
                {
                    EdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;
                }

                void set_data(const Face& _Data)
                {
                    FaceData = _Data;
                }

            protected:
                REFERENCE EdgeRef {NULLREF};
                Face      FaceData{Face()};
            };

            struct EdgeHandle final : public Handle<EdgeHandle>
            {
                // MeshEdgeHandle
                explicit EdgeHandle(const MeshSurface* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) : Handle<EdgeHandle>(_Surface, _Reference){}
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

                // API
                EdgeHandle node_outgoing_edge(const FaceHandle& _Face, const NodeHandle& _Source)
                {
                    auto next = _Face.self().get_edge();

                    do
                    {
                        if(next.is_not_null() && next.get_node() == _Source.self())
                            return next;

                        next = next.get_next();
                    } while (next.is_not_null() && next != _Face.self().get_edge());

                    return EdgeHandle();
                };

                EdgeHandle edge_source_end(const NodeHandle& _Source, const NodeHandle& _Target) const
                {
                    for(auto& edge : Surface->get_edges())
                    {
                        if(edge.is_null()) continue;

                        if(
                            edge.self().get_node() == _Source.self() &&
                            edge.self().get_next().is_not_null()   &&
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
                            edge.self().get_next().is_not_null()   &&
                            edge.self().get_next().get_node() == _Target.self()) return edge.self().get_next();
                    }

                    return EdgeHandle(); 
                }

                bool edge_exists(const NodeHandle& _Source, const NodeHandle& _Target) const
                {
                    for(auto& edge : Surface->get_edges())
                    {
                        if(edge.is_null()) continue;

                        if(
                            edge.self().get_node() == _Source.self() &&
                            edge.self().get_next().is_not_null()     &&
                            edge.self().get_next().get_node() == _Target.self()) return true;
                    }

                    return false;
                }

                int existing_edges_count(const NodeHandle& _Source, const NodeHandle& _Target) const
                {
                    int count = 0;

                    for(auto& edge : Surface->get_edges())
                    {
                        if(edge.is_null()) continue;

                        if(
                            edge.self().get_node() == _Source.self() &&
                            edge.self().get_next().is_not_null()     &&
                            edge.self().get_next().get_node() == _Target.self()) count++;
                    }

                    return count;
                }

            private:

                // info
                const MeshSurface* Surface;
            };

            // node API
            NodeHandle add_node(const Node& _NodeData)
            {
                NodeHandle node = create_node();
                node.self().set_data(_NodeData);
                return node;
            }

            // face API
            FaceHandle add_face(std::vector<NodeHandle> _Nodes, const Face& _FaceData)
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
                PathFinder pathFinder(this);

                // generate path
                std::vector<PathElement> path;

                // create path elements
                for (int i = 0; i < (int)_Nodes.size(); i++)
                {
                    int s = gs_array_index_clamp(i + 0, _Nodes.size());
                    int t = gs_array_index_clamp(i + 1, _Nodes.size());
                    int a = pathFinder.existing_edges_count(_Nodes[s], _Nodes[t]);
                    int b = pathFinder.existing_edges_count(_Nodes[t], _Nodes[s]);

                    if(a + b > 0)
                    {
                        if(a < b)
                            path.push_back(PathElement(_Nodes[s], _Nodes[t], true, true));
                        else
                            path.push_back(PathElement(_Nodes[t], _Nodes[s], true, true));
                    }
                    else
                    {
                        path.push_back(PathElement(_Nodes[s], _Nodes[t], false, false));
                    }
                }
                
                // swap source and target nodes of path elements that are starting or ending at the same node untill there are no such elements
                while ([](std::vector<PathElement>& _Path)->bool
                {
                    for (int i = 0; i < _Path.size(); i++)
                    {
                        int s = gs_array_index_clamp(i + 0, _Path.size());
                        int t = gs_array_index_clamp(i + 1, _Path.size());

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

                    return false;
                }(path));

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

            void remove_face(const FaceHandle& _Face)
            {
                if(_Face.is_null()) return;

                auto curr = _Face.self().get_edge();
                auto next = _Face.self().get_edge();

                do
                {
                    curr = next;
                    next = next.get_next();
                    destroy_edge(curr);
                } while (next.is_not_null() && next != _Face.self().get_edge());
                
                destroy_face(_Face);
            }

            // look-up
            const std::vector<NodeHandle>& get_nodes() const
            {
                return Nodes;
            }

            const std::vector<EdgeHandle>& get_edges() const
            {
                return Edges;
            }

            const std::vector<FaceHandle>& get_faces() const
            {
                return Faces;
            }

            Mesh(){}

            Mesh(const Mesh& _Other)
            {
                Nodes = _Other.Nodes;
                Edges = _Other.Edges;
                Faces = _Other.Faces;

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
                _Node.self().set_data(Node());
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

                // create half edge
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

                // make vacant
                VacantEdges.push_back(_Edge.get_ref());

                // nullify self within twin
                if(_Edge.self().get_twin().is_not_null())
                    _Edge.self().get_twin().self().set_twin(EdgeHandle());

                // nullify self within node
                if(_Edge.self().get_node().self().get_edge() == _Edge.self())
                    _Edge.self().get_node().self().set_edge(EdgeHandle());

                _Edge.self().set_next(EdgeHandle());
                _Edge.self().set_prev(EdgeHandle());
                _Edge.self().set_twin(EdgeHandle());
                _Edge.self().set_ref(NULLREF);
            }
        };
    }
}