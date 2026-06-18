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

            struct  MeshNodeHandle;
            struct  MeshFaceHandle;
            struct  MeshEdgeHandle;

            typedef MeshNodeHandle MeshNode;
            typedef MeshFaceHandle MeshFace;
            typedef MeshEdgeHandle MeshEdge;

            typedef Node NodeType;
            typedef Face FaceType;

            // nested types
            template<typename T>
            struct MeshHandle
            {
                explicit MeshHandle(const MeshSurface* _Surface = nullptr, const REFERENCE&   _Reference = NULLREF) : Surface(_Surface), SelfRef(_Reference){}
                virtual ~MeshHandle(){}

                typedef T Handle;

                virtual Handle& self() const = 0;

                REFERENCE& ref() const
                {
                    return self().SelfRef;
                }

                bool operator == (const MeshHandle& _Other)
                {
                    return _Other.SelfRef == SelfRef && _Other.Surface == Surface;
                }

                bool operator != (const MeshHandle& _Other)
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

            struct MeshNodeHandle final : public MeshHandle<MeshNodeHandle>
            {
                explicit MeshNodeHandle(const MeshSurface* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) : MeshHandle<MeshNodeHandle>(_Surface, _Reference){}
                virtual ~MeshNodeHandle(){}

                // self
                MeshNodeHandle& self() const
                {
                    return this->Surface->Nodes[this->SelfRef];
                }

                // getters
                MeshEdgeHandle& get_edge() const
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
                void set_edge(const MeshEdgeHandle& _Edge)
                {
                    this->EdgeRef = _Edge.is_not_null() ? _Edge.self().ref() : NULLREF;
                }

                void set_data(const Node& _Data)
                {
                    this->NodeData = _Data;
                }

            protected:
                REFERENCE EdgeRef  {NULLREF};
                Node      NodeData {Node()};
            };

            struct MeshFaceHandle final : public MeshHandle<MeshFaceHandle>
            {
                explicit MeshFaceHandle(const MeshSurface* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) : MeshHandle<MeshFaceHandle>(_Surface, _Reference){}
                virtual ~MeshFaceHandle(){}

                // self
                MeshFaceHandle& self() const
                {
                    return this->Surface->Faces[this->SelfRef];
                }

                // getters
                MeshEdgeHandle& get_edge() const
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
                void set_edge(const MeshEdgeHandle& _Edge)
                {
                    this->EdgeRef = _Edge.is_not_null() ? _Edge.self().ref() : NULLREF;
                }

                void set_data(const Face& _Data)
                {
                    this->FaceData = _Data;
                }

            protected:
                REFERENCE EdgeRef {NULLREF};
                Face      FaceData{Face()};
            };

            struct MeshEdgeHandle final : public MeshHandle<MeshEdgeHandle>
            {
                // MeshEdgeHandle
                explicit MeshEdgeHandle(const MeshSurface* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) : MeshHandle<MeshEdgeHandle>(_Surface, _Reference){}
                virtual ~MeshEdgeHandle(){}

                // self
                MeshEdgeHandle& self() const
                {
                    return this->Surface->Edges[this->SelfRef];
                }

                // getters
                MeshNodeHandle& get_node() const
                {
                    return
                        this->NodeRef != NULLREF && this->Surface != nullptr  && this->NodeRef < this->Surface->Nodes.size() ?
                            this->Surface->Nodes[this->NodeRef] :
                                Mesh::FallbackNode;
                }

                MeshFaceHandle& get_face() const
                {
                    return
                        this->FaceRef != NULLREF && this->Surface != nullptr  && this->FaceRef < this->Surface->Faces.size() ?
                            this->Surface->Faces[this->FaceRef] :
                                Mesh::FallbackFace;
                }

                MeshEdgeHandle& get_next() const
                {
                    return
                        this->NextEdgeRef != NULLREF && this->Surface != nullptr  && this->NextEdgeRef < this->Surface->Edges.size() ?
                            this->Surface->Edges[NextEdgeRef] :
                                Mesh::FallbackEdge;
                }

                MeshEdgeHandle& get_prev() const
                {
                    return
                        this->PrevEdgeRef != NULLREF && this->Surface != nullptr  && this->PrevEdgeRef < this->Surface->Edges.size() ?
                            this->Surface->Edges[PrevEdgeRef] :
                                Mesh::FallbackEdge;
                }

                MeshEdgeHandle& get_twin() const
                {
                    return
                        this->SelfRef != NULLREF && this->Surface != nullptr && (this->TwinEdgeRef) < this->Surface->Edges.size() ?
                            this->Surface->Edges[this->TwinEdgeRef] :
                                Mesh::FallbackEdge;
                }

                // setters
                void set_node(const MeshNodeHandle& _Node)
                {
                    self().NodeRef = _Node.is_not_null() ? _Node.self().ref() : NULLREF;
                }

                void set_face(const MeshFaceHandle& _Face)
                {
                    self().FaceRef = _Face.is_not_null() ? _Face.self().ref() : NULLREF;
                }

                void set_next(const MeshEdgeHandle& _Edge)
                {
                    self().NextEdgeRef = _Edge.is_not_null() ? _Edge.self().ref() : NULLREF;
                }

                void set_prev(const MeshEdgeHandle& _Edge)
                {
                    self().PrevEdgeRef = _Edge.is_not_null() ? _Edge.self().ref() : NULLREF;
                }

                void set_twin(const MeshEdgeHandle& _Edge)
                {
                    if(_Edge.is_not_null())
                    {
                        self().TwinEdgeRef = _Edge.self().ref();
                        _Edge.self().TwinEdgeRef = self().ref();
                        return;
                    }

                    self().TwinEdgeRef = NULLREF;
                }

            protected:
                REFERENCE NodeRef     {NULLREF};
                REFERENCE FaceRef     {NULLREF};
                REFERENCE NextEdgeRef {NULLREF};
                REFERENCE PrevEdgeRef {NULLREF};
                REFERENCE TwinEdgeRef {NULLREF};
            };

            // API
            MeshNodeHandle add_node(const Node& _NodeData)
            {
                MeshNodeHandle node = create_node();
                node.self().set_data(_NodeData);
                return node;
            }

            MeshFaceHandle add_face(std::vector<MeshNodeHandle> _Nodes, const Face& _FaceData)
            {
                struct MeshFacePathElement
                {
                    MeshFacePathElement(MeshNodeHandle _Source, MeshNodeHandle _Target, bool _Swapped, bool _Existing) : Source(_Source), Target(_Target), Swapped(_Swapped), Existing(_Existing){}

                    MeshNodeHandle Source   {false};
                    MeshNodeHandle Target   {false};
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

                    if(edge_exists(_Nodes[s], _Nodes[t]))
                        path.push_back(MeshFacePathElement(_Nodes[t], _Nodes[s], true, true));
                    else if(edge_exists(_Nodes[t], _Nodes[s]))
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

                // cehck if resulting face is not manifold
                for (size_t i = 0; i < path.size(); i++)
                {
                    if(
                        edge_exists(path[i].Source, path[i].Target) && 
                        edge_exists(path[i].Target, path[i].Source))
                    {
                        return MeshFaceHandle();
                    }
                }

                // create face
                MeshFaceHandle face = create_face();
                face.self().set_data(_FaceData);

                // create half edges for corresponding not exsting path elements
                std::vector<MeshEdgeHandle> edges;

                for (size_t i = 0; i < path.size(); i++)
                {
                    if(path[i].Existing)
                        continue;

                    MeshEdgeHandle edge = create_edge(path[i].Source);
                    face.self().set_edge(edge);
                    edges.push_back(edge);
                }

                // create twins for corresponding exsting path elements
                for (size_t i = 0; i < path.size(); i++)
                {
                    if(!path[i].Existing)
                        continue;

                    MeshEdgeHandle twin = create_edge(edge_target_end(path[i].Target, path[i].Source).get_node());
                    twin.self().set_twin(edge_source_end(path[i].Target, path[i].Source));
                    face.self().set_edge(twin);
                    edges.push_back(twin);
                }

                for (size_t i = 0; i < path.size(); i++)
                {
                    MeshEdgeHandle e1;
                    MeshEdgeHandle e2;

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

            void split_face(const MeshFaceHandle& _Face, const MeshEdgeHandle& _Source, const MeshEdgeHandle& _Target)
            {
                (void)_Face;
                (void)_Source;
                (void)_Target;
            }

            void merge_faces(const MeshFaceHandle& _First, const MeshFaceHandle& _Second)
            {
                (void)_First;
                (void)_Second;
            }

            const std::vector<MeshNodeHandle>& get_nodes() const
            {
                return Nodes;
            }

            const std::vector<MeshEdgeHandle>& get_edges() const
            {
                return Edges;
            }

            const std::vector<MeshFaceHandle>& get_faces() const
            {
                return Faces;
            }

            // retrieves all outgoing half edges
            std::vector<MeshEdgeHandle> node_half_edges(const MeshNodeHandle& _Node) const
            {
                std::vector<MeshEdgeHandle> outgoingEdges;

                for (auto& halfEdge : Edges)
                {
                    if(halfEdge.self().get_node() == _Node.self())
                        outgoingEdges.push_back(halfEdge);
                }
                
                return outgoingEdges;
            }

            MeshEdgeHandle edge_source_end(const MeshNodeHandle& _Source, const MeshNodeHandle& _Target) const
            {
                std::vector<MeshEdgeHandle> EdgesStartingAtSource = node_half_edges(_Source);

                for (auto& EdgestartingAtSource : EdgesStartingAtSource)
                {
                    if(
                        EdgestartingAtSource.self().get_next().is_not_null() &&
                        EdgestartingAtSource.self().get_next().get_node() == _Target.self())
                    {
                        return EdgestartingAtSource;
                    }
                }

                return MeshEdgeHandle(); 
            }

            MeshEdgeHandle edge_target_end(const MeshNodeHandle& _Source, const MeshNodeHandle& _Target) const
            {
                std::vector<MeshEdgeHandle> EdgesStartingAtSource = node_half_edges(_Source);

                for (auto& EdgestartingAtSource : EdgesStartingAtSource)
                {
                    if(
                        EdgestartingAtSource.self().get_next().is_not_null() &&
                        EdgestartingAtSource.self().get_next().get_node() == _Target.self())
                    {
                        return EdgestartingAtSource.self().get_next();
                    }
                }

                return MeshEdgeHandle(); 
            }

            bool edge_exists(const MeshNodeHandle& _Source, const MeshNodeHandle& _Target) const
            {
                std::vector<MeshEdgeHandle> EdgesStartingAtSource = node_half_edges(_Source);

                for (auto& EdgestartingAtSource : EdgesStartingAtSource)
                {
                    if(
                        EdgestartingAtSource.self().get_next().is_not_null() &&
                        EdgestartingAtSource.self().get_next().get_node() == _Target.self())
                    {
                        return true;
                    }
                }

                return false;
            }

        private:

            // append-only containers
            mutable std::vector<MeshNodeHandle> Nodes {std::vector<MeshNodeHandle>()};
            mutable std::vector<MeshEdgeHandle> Edges {std::vector<MeshEdgeHandle>()};
            mutable std::vector<MeshFaceHandle> Faces {std::vector<MeshFaceHandle>()};

            // fallback
            inline static MeshNodeHandle FallbackNode = MeshNodeHandle();
            inline static MeshFaceHandle FallbackFace = MeshFaceHandle();
            inline static MeshEdgeHandle FallbackEdge = MeshEdgeHandle();

            // service methods
            MeshNodeHandle create_node()
            {
                Nodes.push_back(MeshNodeHandle(this, (REFERENCE)Nodes.size()));
                return Nodes[Nodes.size() - 1];
            }

            MeshFaceHandle create_face()
            {
                Faces.push_back(MeshFaceHandle(this, (REFERENCE)Faces.size()));
                return Faces[Faces.size() - 1];
            }

            MeshEdgeHandle create_edge(const MeshNodeHandle& _Node)
            {
                // create half edge
                Edges.push_back(MeshEdgeHandle(this, (REFERENCE)Edges.size()));
                Edges[Edges.size() - 1].set_node(_Node.self());
                
                if(_Node.self().get_edge().is_null())
                    _Node.self().set_edge(Edges[Edges.size() - 1]);

                return Edges[Edges.size() - 1];
            }
        };
    }
}