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
        struct MeshNodeHandle;
        struct MeshFaceHandle;
        struct MeshHalfEdgeHandle;

        struct MeshSurfaceHandle;
        
        // handles
        template<typename T>
        struct MeshHandle
        {
            explicit MeshHandle(const MeshSurfaceHandle* _Surface = nullptr, const REFERENCE& _Reference = NULLREF) : Surface(_Surface), SelfRef(_Reference){}
            ~MeshHandle(){}

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
            const MeshSurfaceHandle* Surface {nullptr};
            mutable REFERENCE        SelfRef {NULLREF};
        };

        struct MeshNodeHandle final : public MeshHandle<MeshNodeHandle>
        {
            explicit MeshNodeHandle(const MeshSurfaceHandle* _Surface = nullptr, const REFERENCE& _Reference = NULLREF);
            virtual ~MeshNodeHandle();

            // getters
            MeshHandle<MeshNodeHandle>::Handle& self() const override;
            MeshHalfEdgeHandle& get_edge() const;

            // setters
            void set_edge(const MeshHalfEdgeHandle& _Edge);

            std::string Name;

        protected:
            REFERENCE HalfEdgeRef {NULLREF};
        };

        struct MeshFaceHandle final : public MeshHandle<MeshFaceHandle>
        {
            explicit MeshFaceHandle(const MeshSurfaceHandle* _Surface = nullptr, const REFERENCE& _Reference = NULLREF);
            virtual ~MeshFaceHandle();

            // getters
            MeshHandle<MeshFaceHandle>::Handle& self() const override;
            MeshHalfEdgeHandle& get_edge() const;

            // setters
            void set_edge(const MeshHalfEdgeHandle& _Edge);

            std::string Name;

        protected:
            REFERENCE HalfEdgeRef {NULLREF};
        };

        struct MeshHalfEdgeHandle final : public MeshHandle<MeshHalfEdgeHandle>
        {
            explicit MeshHalfEdgeHandle(const MeshSurfaceHandle* _Surface = nullptr, const REFERENCE& _Reference = NULLREF);
            virtual ~MeshHalfEdgeHandle();

            MeshHandle<MeshHalfEdgeHandle>::Handle& self() const override;

            // getters
            MeshNodeHandle&     get_node() const;
            MeshFaceHandle&     get_face() const;
            MeshHalfEdgeHandle& get_next() const;
            MeshHalfEdgeHandle& get_prev() const;
            MeshHalfEdgeHandle& get_twin() const;

            // setters
            void set_node(const MeshNodeHandle&);
            void set_face(const MeshFaceHandle&);
            void set_next(const MeshHalfEdgeHandle&);
            void set_prev(const MeshHalfEdgeHandle&);
            void set_twin(const MeshHalfEdgeHandle&);

        protected:
            REFERENCE NodeRef         {NULLREF};
            REFERENCE FaceRef         {NULLREF};
            REFERENCE NextHalfEdgeRef {NULLREF};
            REFERENCE PrevHalfEdgeRef {NULLREF};
            REFERENCE TwinHalfEdgeRef {NULLREF};
        };

        struct MeshSurfaceHandle
        {
            // containers
            mutable std::vector<MeshNodeHandle>     Nodes     {std::vector<MeshNodeHandle>()};
            mutable std::vector<MeshFaceHandle>     Faces     {std::vector<MeshFaceHandle>()};
            mutable std::vector<MeshHalfEdgeHandle> HalfEdges {std::vector<MeshHalfEdgeHandle>()};

            // fallback
            static MeshNodeHandle     FallbackNode;
            static MeshFaceHandle     FallbackFace;
            static MeshHalfEdgeHandle FallbackHalfEdge;

            // retrieves all outgoing half edges
            std::vector<MeshHalfEdgeHandle> outgoing_half_edges(const MeshNodeHandle& _Node)
            {
                std::vector<MeshHalfEdgeHandle> outgoingHalfEdges;

                for (auto& halfEdge : HalfEdges)
                {
                    if(halfEdge.self().get_node() == _Node.self())
                        outgoingHalfEdges.push_back(halfEdge);
                }
                
                return outgoingHalfEdges;
            }

            bool edge_exists(const MeshNodeHandle& _Source, const MeshNodeHandle& _Target)
            {
                std::vector<MeshHalfEdgeHandle> halfEdgesStartingAtSource = outgoing_half_edges(_Source);

                for (auto& halfEdgeStartingAtSource : halfEdgesStartingAtSource)
                {
                    if(
                        halfEdgeStartingAtSource.self().get_next().is_not_null() &&
                        halfEdgeStartingAtSource.self().get_next().get_node() == _Target.self())
                    {
                        return true;
                    }
                }

                return false;
            }

            MeshHalfEdgeHandle request_existing_edge_source_end(const MeshNodeHandle& _Source, const MeshNodeHandle& _Target)
            {
                std::vector<MeshHalfEdgeHandle> halfEdgesStartingAtSource = outgoing_half_edges(_Source);

                for (auto& halfEdgeStartingAtSource : halfEdgesStartingAtSource)
                {
                    if(
                        halfEdgeStartingAtSource.self().get_next().is_not_null() &&
                        halfEdgeStartingAtSource.self().get_next().get_node() == _Target.self())
                    {
                        return halfEdgeStartingAtSource;
                    }
                }

                return MeshHalfEdgeHandle(); 
            }

            MeshHalfEdgeHandle request_existing_edge_target_end(const MeshNodeHandle& _Source, const MeshNodeHandle& _Target)
            {
                std::vector<MeshHalfEdgeHandle> halfEdgesStartingAtSource = outgoing_half_edges(_Source);

                for (auto& halfEdgeStartingAtSource : halfEdgesStartingAtSource)
                {
                    if(
                        halfEdgeStartingAtSource.self().get_next().is_not_null() &&
                        halfEdgeStartingAtSource.self().get_next().get_node() == _Target.self())
                    {
                        return halfEdgeStartingAtSource.self().get_next();
                    }
                }

                return MeshHalfEdgeHandle(); 
            }

            // node
            MeshNodeHandle add_node()
            {
                return create_node();
            }

            MeshFaceHandle add_face(std::vector<MeshNodeHandle> _Nodes, const std::string& _Name)
            {
                struct MeshFacePathElement
                {
                    MeshFacePathElement(
                        MeshNodeHandle _Source,
                        MeshNodeHandle _Target,
                        bool           _Swapped,
                        bool           _Existing) :
                            Source(_Source), Target(_Target), Swapped(_Swapped), Existing(_Existing){}

                    MeshNodeHandle Source   {false};
                    MeshNodeHandle Target   {false};
                    bool           Swapped  {false};
                    bool           Existing {false};
                };

                // generate path
                std::vector<MeshFacePathElement> path;

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

                // cehck if resulting face can be manifold
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
                face.self().Name = _Name;

                // create half edges for corresponding not exsting path elements
                std::vector<MeshHalfEdgeHandle> edges;

                for (size_t i = 0; i < path.size(); i++)
                {
                    if(path[i].Existing)
                        continue;

                    MeshHalfEdgeHandle edge = create_half_edge(path[i].Source);
                    face.self().set_edge(edge);
                    edges.push_back(edge);
                }

                // create twins for corresponding exsting path elements
                for (size_t i = 0; i < path.size(); i++)
                {
                    if(!path[i].Existing)
                        continue;

                    MeshHalfEdgeHandle twin = create_half_edge(request_existing_edge_target_end(path[i].Target, path[i].Source).get_node());
                    twin.self().set_twin(request_existing_edge_source_end(path[i].Target, path[i].Source));
                    face.self().set_edge(twin);
                    edges.push_back(twin);
                }

                for (size_t i = 0; i < path.size(); i++)
                {
                    MeshHalfEdgeHandle e1;
                    MeshHalfEdgeHandle e2;

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

        private:

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

            MeshHalfEdgeHandle create_half_edge(const MeshNodeHandle& _Node)
            {
                // create half edge
                HalfEdges.push_back(MeshHalfEdgeHandle(this, (REFERENCE)HalfEdges.size()));
                HalfEdges[HalfEdges.size() - 1].set_node(_Node.self());
                
                if(_Node.self().get_edge().is_null())
                    _Node.self().set_edge(HalfEdges[HalfEdges.size() - 1]);

                return HalfEdges[HalfEdges.size() - 1];
            }
        };
    }
}