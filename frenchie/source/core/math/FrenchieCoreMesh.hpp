#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreOptional.hpp>

// STL
#include <map>
#include <vector>
#include <iostream>
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
            friend struct MeshSurfaceHandle;
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
            mutable std::vector<MeshNodeHandle>     Nodes     {std::vector<MeshNodeHandle>()};
            mutable std::vector<MeshFaceHandle>     Faces     {std::vector<MeshFaceHandle>()};
            mutable std::vector<MeshHalfEdgeHandle> HalfEdges {std::vector<MeshHalfEdgeHandle>()};

            // null check
            static MeshNodeHandle     FallbackNode;
            static MeshFaceHandle     FallbackFace;
            static MeshHalfEdgeHandle FallbackHalfEdge;

            // node
            MeshNodeHandle add_node()
            {
                return create_node();
            }

            MeshFaceHandle add_face1(std::vector<MeshNodeHandle> _Nodes)
            {
                // nested types
                struct MeshEdgeHandle
                {
                    MeshEdgeHandle(
                        const MeshNodeHandle& _Source,
                        const MeshNodeHandle& _Target,
                        const bool&           _Swapped,
                        const bool&           _Existing) :
                        Source(_Source),
                        Target(_Target),
                        Swapped(_Swapped),
                        Existing(_Existing){}

                    MeshNodeHandle Source   {MeshNodeHandle()};
                    MeshNodeHandle Target   {MeshNodeHandle()};
                    bool           Swapped  {false};
                    bool           Existing {false};
                };

                // collect edges inverting directions of already existing edges 
                std::vector<MeshEdgeHandle> edges;

                std::cout << "\n\nbuild face \n";

                for (int i = 0; i < _Nodes.size(); i++)
                {
                    int s = gs_array_index_clamp(i + 0, _Nodes.size());
                    int t = gs_array_index_clamp(i + 1, _Nodes.size());

                    if(!does_edge_exist(_Nodes[s].self(), _Nodes[t].self()))
                    {
                        std::cout << "edge does not exist " << _Nodes[s].self().Name << " --> " << _Nodes[t].self().Name << "\n";
                    }
                    else
                    {
                        std::cout << "edge exists " << _Nodes[s].self().Name << " --> " << _Nodes[t].self().Name << "\n";
                    }

                    edges.push_back(

                        does_edge_exist(_Nodes[s].self(), _Nodes[t].self()) ?

                            MeshEdgeHandle(
                                get_edge_target_node(_Nodes[s].self(), _Nodes[t].self()),
                                get_edge_source_node(_Nodes[s].self(), _Nodes[t].self()),
                                false,
                                true) :

                                MeshEdgeHandle(
                                    get_edge_source_node(_Nodes[s].self(), _Nodes[t].self()),
                                    get_edge_target_node(_Nodes[s].self(), _Nodes[t].self()),
                                    false,
                                    false));
                }

                // swap source and target nodes of edges that are starting or ending at the same node
                while ([](std::vector<MeshEdgeHandle>& _Edges)->bool
                {
                    for (int i = 0; i < _Edges.size(); i++)
                    {
                        int s = gs_array_index_clamp(i + 0, _Edges.size());
                        int t = gs_array_index_clamp(i + 1, _Edges.size());

                        if (_Edges[s].Source.self() == _Edges[t].Source.self() ||
                            _Edges[s].Target.self() == _Edges[t].Target.self())
                        {
                            if(!_Edges[s].Swapped && !_Edges[s].Existing)
                            {
                                gs_swap(_Edges[s].Source, _Edges[s].Target);
                                _Edges[s].Swapped = true;
                                return true;
                            }

                            if(!_Edges[t].Swapped && !_Edges[s].Existing)
                            {
                                gs_swap(_Edges[t].Source, _Edges[t].Target);
                                _Edges[t].Swapped = true;
                                return true;
                            }
                        }
                    }

                    return false;
                }(edges));

                // create face
                MeshFaceHandle face = create_face();

                // create half edeges
                std::vector<MeshHalfEdgeHandle> halfEdges;

                for (int i = 0; i < edges.size(); i++)
                {
                    if (!edges[i].Existing)
                    {
                        std::cout << "new edge " << edges[i].Source.self().Name << " --> " << edges[i].Target.self().Name << "\n";

                        auto edge = create_half_edge(edges[i].Source.self());
                        
                        face.self().set_edge(edge);
                        edge.self().set_face(face);

                        halfEdges.push_back(edge);
                    }
                }

                // create half edges twins
                for (int i = 0; i < edges.size(); i++)
                {
                    if (edges[i].Existing)
                    {
                        std::cout << "twin edge " << edges[i].Source.self().Name << " --> " << edges[i].Target.self().Name << "\n";

                        MeshHalfEdgeHandle twin = create_half_edge(edges[i].Source.self());
                        twin.set_twin(edges[i].Target.self().get_edge());

                        twin.self().set_face(face);

                        edges[i].Target.self().get_edge().set_twin(twin);
                        halfEdges.push_back(edges[i].Target.self().get_edge().get_twin());
                    }
                }

                // connect half edges
                for (int i = 0; i < edges.size(); i++)
                {
                    MeshHalfEdgeHandle e1;
                    MeshHalfEdgeHandle e2;

                    // find source half edge
                    for (int j = 0; j < halfEdges.size(); j++)
                    {
                        if(halfEdges[j].self().get_node() == edges[i].Source.self())
                        {
                            e1 = halfEdges[j];
                            break;
                        }
                    }

                    // find target half edge
                    for (int j = 0; j < halfEdges.size(); j++)
                    {
                        if(halfEdges[j].self().get_node() == edges[i].Target.self())
                        {
                            e2 = halfEdges[j];
                            break;
                        }
                    }

                    if(e1.is_not_null())
                        e1.set_next(e2);
                    
                    if(e2.is_not_null())
                        e2.set_prev(e1);

                    std::cout << "connecting " << e1.get_node().Name << " --> " << e2.get_node().Name << "\n";
                }

                return face;
            }

            // auxiliary lambdas
            bool does_edge_exist(const MeshNodeHandle& _Source, const MeshNodeHandle& _Target)
            {
                return
                    (_Source.self().get_edge().is_not_null() && _Source.self().get_edge().get_next().is_not_null() && _Source.self().get_edge().get_next().get_node() == _Target.self()) ||
                    (_Target.self().get_edge().is_not_null() && _Target.self().get_edge().get_next().is_not_null() && _Target.self().get_edge().get_next().get_node() == _Source.self());
            };

            MeshNodeHandle get_edge_source_node(const MeshNodeHandle& _Source, const MeshNodeHandle& _Target)
            {
                if(_Source.is_null() || _Target.is_null())
                    return MeshNodeHandle();

                if(!does_edge_exist(_Source, _Target))
                    return _Source;

                if(_Source.self().get_edge().is_not_null() && _Source.self().get_edge().get_next().is_not_null() && _Source.self().get_edge().get_next().get_node() == _Target.self())
                    return _Source;

                if(_Target.self().get_edge().is_not_null() && _Target.self().get_edge().get_next().is_not_null() && _Target.self().get_edge().get_next().get_node() == _Source.self())
                    return _Target;

                return _Source;
            };

            MeshNodeHandle get_edge_target_node(const MeshNodeHandle& _Source, const MeshNodeHandle& _Target)
            {
                if(_Source.is_null() || _Target.is_null())
                    return MeshNodeHandle();

                if(!does_edge_exist(_Source, _Target))
                    return _Source;

                if(_Source.self().get_edge().is_not_null() && _Source.self().get_edge().get_next().is_not_null() && _Source.self().get_edge().get_next().get_node() == _Target.self())
                    return _Target;

                if(_Target.self().get_edge().is_not_null() && _Target.self().get_edge().get_next().is_not_null() && _Target.self().get_edge().get_next().get_node() == _Source.self())
                    return _Source;

                return _Target;
            };

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