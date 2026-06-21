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

                virtual T& self() const = 0;

                REFERENCE get_ref() const
                {
                    return SelfRef;
                }

                // setters
                void set_ref(const REFERENCE& _Ref)
                {
                    SelfRef = _Ref;
                }

                // operators
                bool operator == (const Handle& _Other)
                {
                    return _Other.SelfRef == SelfRef && _Other.Surface == Surface;
                }

                bool operator != (const Handle& _Other)
                {
                    return _Other.SelfRef != SelfRef || _Other.Surface != Surface;
                }

                bool is_null() const
                {
                    return SelfRef == NULLREF || Surface == nullptr;
                }

                bool is_not_null() const
                {
                    return SelfRef != NULLREF && Surface != nullptr;
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
                EdgeHandle& get_edge() const
                {
                    return
                        this->EdgeRef != NULLREF && this->Surface != nullptr && this->EdgeRef < this->Surface->Edges.size() ?
                            this->Surface->Edges[this->EdgeRef] :
                                Mesh::FallbackEdge;
                }

                Node get_data() const
                {
                    return NodeData;
                }

                // setters
                void set_edge(const EdgeHandle& _Edge)
                {
                    this->EdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;
                }

                void set_data(const Node& _Data)
                {
                    this->NodeData = _Data;
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
                EdgeHandle& get_edge() const
                {
                    return
                        this->EdgeRef != NULLREF && this->Surface != nullptr && this->EdgeRef < this->Surface->Edges.size() ?
                            this->Surface->Edges[this->EdgeRef] :
                                Mesh::FallbackEdge;
                }

                Face get_data() const
                {
                    return FaceData;
                }

                // setters
                void set_edge(const EdgeHandle& _Edge)
                {
                    this->EdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;
                }

                void set_data(const Face& _Data)
                {
                    this->FaceData = _Data;
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
                NodeHandle& get_node() const
                {
                    return
                        this->NodeRef != NULLREF && this->Surface != nullptr  && this->NodeRef < this->Surface->Nodes.size() ?
                            this->Surface->Nodes[this->NodeRef] :
                                Mesh::FallbackNode;
                }

                FaceHandle& get_face() const
                {
                    return
                        this->FaceRef != NULLREF && this->Surface != nullptr  && this->FaceRef < this->Surface->Faces.size() ?
                            this->Surface->Faces[this->FaceRef] :
                                Mesh::FallbackFace;
                }

                EdgeHandle& get_next() const
                {
                    return
                        this->NextEdgeRef != NULLREF && this->Surface != nullptr  && this->NextEdgeRef < this->Surface->Edges.size() ?
                            this->Surface->Edges[NextEdgeRef] :
                                Mesh::FallbackEdge;
                }

                EdgeHandle& get_prev() const
                {
                    return
                        this->PrevEdgeRef != NULLREF && this->Surface != nullptr  && this->PrevEdgeRef < this->Surface->Edges.size() ?
                            this->Surface->Edges[PrevEdgeRef] :
                                Mesh::FallbackEdge;
                }

                EdgeHandle& get_twin() const
                {
                    return
                        this->TwinEdgeRef != NULLREF && this->Surface != nullptr && (this->TwinEdgeRef) < this->Surface->Edges.size() ?
                            this->Surface->Edges[this->TwinEdgeRef] :
                                Mesh::FallbackEdge;
                }

                // setters
                void set_node(const NodeHandle& _Node)
                {
                    self().NodeRef = _Node.is_not_null() ? _Node.self().get_ref() : NULLREF;
                }

                void set_face(const FaceHandle& _Face)
                {
                    self().FaceRef = _Face.is_not_null() ? _Face.self().get_ref() : NULLREF;
                }

                void set_next(const EdgeHandle& _Edge)
                {
                    self().NextEdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;

                    if(_Edge.is_not_null())
                        _Edge.self().PrevEdgeRef = self().get_ref();
                }

                void set_prev(const EdgeHandle& _Edge)
                {
                    self().PrevEdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;

                    if(_Edge.is_not_null())
                        _Edge.self().NextEdgeRef = self().get_ref();
                }

                void set_twin(const EdgeHandle& _Edge)
                {
                    self().TwinEdgeRef = _Edge.is_not_null() ? _Edge.self().get_ref() : NULLREF;
                    
                    if(_Edge.is_not_null())
                        _Edge.self().TwinEdgeRef = self().get_ref();
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
                        if(
                            edge.self().get_node() == _Source.self() &&
                            edge.self().get_next().is_not_null()   &&
                            edge.self().get_next().get_node() == _Target.self()) return true;
                    }

                    return false;
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
                PathFinder pathFinder(this);

                struct MeshFacePathElement
                {
                    MeshFacePathElement(NodeHandle _Source, NodeHandle _Target, bool _Swapped, bool _Existing) : Source(_Source), Target(_Target), Swapped(_Swapped), Existing(_Existing){}

                    NodeHandle Source   {false};
                    NodeHandle Target   {false};
                    bool           Swapped  {false};
                    bool           Existing {false};
                };

                // generate path
                std::vector<MeshFacePathElement> path;

                // create path elements
                for (size_t i = 0; i < _Nodes.size(); i++)
                {
                    int s = gs_array_index_clamp(i + 0, _Nodes.size());
                    int t = gs_array_index_clamp(i + 1, _Nodes.size());

                    if(pathFinder.edge_exists(_Nodes[s], _Nodes[t]))
                        path.push_back(MeshFacePathElement(_Nodes[t], _Nodes[s], true, true));
                    else if(pathFinder.edge_exists(_Nodes[t], _Nodes[s]))
                        path.push_back(MeshFacePathElement(_Nodes[s], _Nodes[t], true, true));
                    else
                        path.push_back(MeshFacePathElement(_Nodes[s], _Nodes[t], false, false));
                }
                
                // swap source and target nodes of path elements that are starting or ending at the same node
                while ([](std::vector<MeshFacePathElement>& _Path)->bool
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

                // check if resulting face is manifold
                // TODO: the following check is not correct !!!!
                // for (size_t i = 0; i < path.size(); i++)
                // {
                //     if(
                //         pathFinder.edge_exists(path[i].Source, path[i].Target) && 
                //         pathFinder.edge_exists(path[i].Target, path[i].Source))
                //     {
                //         return FaceHandle();
                //     }
                // }

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

                    for (size_t j = 0; j < edges.size(); j++)
                    {
                        if(path[i].Source.self() == edges[j].self().get_node())
                        {
                            e1 = edges[j].self();
                            break;
                        }
                    }

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

            FaceHandle split_face(const FaceHandle& _Face, const NodeHandle& _Source, const NodeHandle& _Target)
            {
                PathFinder pathFiner(this);
                    
                if(_Face.is_null() || _Source.is_null() || _Target.is_null() || pathFiner.edge_exists(_Source, _Target) || pathFiner.edge_exists(_Target, _Source))
                    return FaceHandle();

                // find half edges outgoing out-of splitting edge node
                auto sourceEnd = pathFiner.node_outgoing_edge(_Face, _Source);
                auto targetEnd = pathFiner.node_outgoing_edge(_Face, _Target);                

                if(sourceEnd.is_null() || targetEnd.is_null())
                    return FaceHandle();

                // create interconnecting edges
                auto e1 = create_edge(_Source);
                auto e2 = create_edge(_Target);
                sourceEnd.get_prev().set_next(e1);
                targetEnd.get_prev().set_next(e2);
                e1.self().set_next(targetEnd);
                e2.self().set_next(sourceEnd);
                e2.self().set_twin(e1);

                // create new face
                auto face = create_face();

                // reset old face
                {
                    auto next = e1;

                    do
                    {
                        next.self().set_face(_Face.self());
                        next = next.get_next();
                    } while (next.is_not_null() && next.self() != e1);

                    _Face.self().set_edge(e1);
                }

                // setup new face
                {
                    auto next = e2;

                    do
                    {
                        next.self().set_face(face.self());
                        next = next.get_next();
                    } while (next.is_not_null() && next.self() != e2);

                    face.self().set_edge(e2);
                }

                return face;
            }

            bool merge_faces(const FaceHandle& _First, const FaceHandle& _Second)
            {
                if (_First.is_null() || _Second.is_null())
                    return false;

                while (([this](const FaceHandle& _First, const FaceHandle& _Second)->bool
                {
                    // count the number of edges dividing first and second face
                    EdgeHandle border = EdgeHandle();
                    int        count  = 0;

                    {
                        auto next = _First.self().get_edge();

                        do
                        {
                            if(next.self().get_twin().get_face() == _Second.self())
                            {
                                border = next;
                                count++;
                            }
                            
                            next = next.get_next();

                        } while (next.is_not_null() && next.self() != _First.self().get_edge());
                    }
                    
                    // collapse bordering edges untill we have the only such edge
                    if(count > 1)
                    {
                        auto next = _First.self().get_edge();

                        do
                        {
                            if(next.self().get_twin().get_face() == _Second.self())
                            {
                                collapse_edge(next.self());
                                return true;
                            }

                            next = next.get_next();

                        } while (next.is_not_null() && next.self() != _First.self().get_edge());
                    }

                    // destroy the last bordering edge between first and second face
                    {                        
                        // reconnect pointers
                        border.self().get_prev().set_next(border.self().get_twin().self().get_next());
                        border.self().get_next().set_prev(border.self().get_twin().self().get_prev());

                        // destroy twin
                        border.self().get_twin().self().set_next(EdgeHandle());
                        border.self().get_twin().self().set_next(EdgeHandle());
                        border.self().get_twin().self().set_twin(EdgeHandle());
                        
                        // destroy self
                        border.self().set_next(EdgeHandle());
                        border.self().set_next(EdgeHandle());
                        border.self().set_twin(EdgeHandle());

                        // setup face
                        auto next = _First.self().get_edge();
                        do
                        {
                            next.self().set_face(_First);
                            next = next.get_next();
                        } while (next.is_not_null() && next.self() != _First.self().get_edge());
                    }

                    return false;

                })(_First, _Second));

                return true;
            }

            // edge API
            bool collapse_edge(const EdgeHandle& _Edge)
            {
                if(_Edge.is_null())
                    return false;

                if(_Edge.self().get_prev().is_not_null())
                    _Edge.self().get_prev().set_next(_Edge.self().get_next());
                
                if(_Edge.self().get_twin().is_not_null())
                {
                    if(_Edge.self().get_twin().get_prev().is_not_null())
                        _Edge.self().get_twin().get_prev().set_next(_Edge.self().get_twin().get_next());

                    // destroy twin
                    _Edge.self().get_twin().set_next(EdgeHandle());
                    _Edge.self().get_twin().set_prev(EdgeHandle());
                    _Edge.self().get_twin().set_twin(EdgeHandle());
                }
                
                // destroy self
                _Edge.self().set_next(EdgeHandle());
                _Edge.self().set_prev(EdgeHandle());
                _Edge.self().set_twin(EdgeHandle());

                return true;
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

        private:

            // append-only containers
            mutable std::vector<NodeHandle> Nodes {std::vector<NodeHandle>()};
            mutable std::vector<EdgeHandle> Edges {std::vector<EdgeHandle>()};
            mutable std::vector<FaceHandle> Faces {std::vector<FaceHandle>()};

            // fallback
            inline static NodeHandle FallbackNode = NodeHandle();
            inline static FaceHandle FallbackFace = FaceHandle();
            inline static EdgeHandle FallbackEdge = EdgeHandle();

            // service methods
            NodeHandle create_node()
            {
                Nodes.push_back(NodeHandle(this, (REFERENCE)Nodes.size()));
                return Nodes[Nodes.size() - 1];
            }

            FaceHandle create_face()
            {
                Faces.push_back(FaceHandle(this, (REFERENCE)Faces.size()));
                return Faces[Faces.size() - 1];
            }

            EdgeHandle create_edge(const NodeHandle& _Node)
            {
                // create half edge
                Edges.push_back(EdgeHandle(this, (REFERENCE)Edges.size()));
                Edges[Edges.size() - 1].set_node(_Node.self());
                
                if(_Node.self().get_edge().is_null())
                    _Node.self().set_edge(Edges[Edges.size() - 1]);

                return Edges[Edges.size() - 1];
            }
        };
    }
}