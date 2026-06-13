#pragma once

// Core
#include <FrenchieCoreMath.hpp>

// STL
#include <map>
#include <vector>
#include <iostream>

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

            // API
            MeshHandle<MeshNodeHandle>::Handle& self() const override;
            MeshHalfEdgeHandle& edge() const;

            std::string Name;

        protected:
            friend struct MeshSurfaceHandle;
            REFERENCE HalfEdgeRef {NULLREF};
        };

        // struct MeshEdgeHandle final : public MeshHandle<MeshEdgeHandle>
        // {
        //     explicit MeshEdgeHandle(const MeshSurfaceHandle* _Surface = nullptr, const REFERENCE& _Reference = NULLREF);
        //     virtual ~MeshEdgeHandle();

        //     MeshHandle<MeshEdgeHandle>::Handle& self() const override;
        //     MeshHalfEdgeHandle& source_half_edge() const;
        //     MeshHalfEdgeHandle& target_half_edge() const;

        // protected:
        //     friend struct MeshSurfaceHandle;
        //     REFERENCE SourceHalfEdge{NULLREF};
        //     REFERENCE TargetHalfEdge{NULLREF};
        // };

        struct MeshHalfEdgeHandle final : public MeshHandle<MeshHalfEdgeHandle>
        {
            explicit MeshHalfEdgeHandle(const MeshSurfaceHandle* _Surface = nullptr, const REFERENCE& _Reference = NULLREF);
            virtual ~MeshHalfEdgeHandle();

            MeshHandle<MeshHalfEdgeHandle>::Handle& self() const override;
            MeshNodeHandle&     node() const;
            MeshFaceHandle&     face() const;
            MeshHalfEdgeHandle& next() const;
            MeshHalfEdgeHandle& prev() const;
            MeshHalfEdgeHandle& twin() const;

            bool shared() const;

        protected:
            friend struct MeshSurfaceHandle;
            REFERENCE NodeRef         {NULLREF};
            REFERENCE FaceRef         {NULLREF};
            REFERENCE NextHalfEdgeRef {NULLREF};
            REFERENCE PrevHalfEdgeRef {NULLREF};
        };

        struct MeshFaceHandle final : public MeshHandle<MeshFaceHandle>
        {
            explicit MeshFaceHandle(const MeshSurfaceHandle* _Surface = nullptr, const REFERENCE& _Reference = NULLREF);
            virtual ~MeshFaceHandle();

            MeshHandle<MeshFaceHandle>::Handle& self() const override;
            MeshHalfEdgeHandle& edge() const;

        protected:
            friend struct MeshSurfaceHandle;
            REFERENCE HalfEdgeRef {NULLREF};
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
                MeshNodeHandle     node     = create_node();
                MeshHalfEdgeHandle halfEdge = create_half_edge(node);
                return node;
            }

            MeshFaceHandle add_face(std::vector<MeshNodeHandle> _Nodes)
            {
                std::vector<MeshHalfEdgeHandle> faceHalfEdges;

                // auxiliary lambdas
                auto path_exists = [this](const MeshNodeHandle& _From, const MeshNodeHandle& _To)->bool
                {
                    MeshHalfEdgeHandle next = _From.self().edge();

                    do
                    {
                        if(next.node() == _To)
                            return true;
                        next = next.next();
                    } while (next.is_not_null() && next != _From.self().edge());
                    
                    return false;
                };

                std::cout << "\ngenerating FACE\n";

                // setup twins
                for (int i = 0; i < _Nodes.size(); i++)
                {
                    int s = gs_array_index_clamp(i + 0,_Nodes.size());
                    int t = gs_array_index_clamp(i + 1,_Nodes.size());

                    if(!path_exists(_Nodes[s], _Nodes[t]))
                    {
                        if(_Nodes[i].self().edge().next().is_null())
                            std::cout << "vacant " << _Nodes[s].self().Name << " --> " << _Nodes[t].self().Name << "\n";
                        else
                            std::cout << "created " << _Nodes[s].self().Name << " --> " << _Nodes[t].self().Name << "\n";

                        faceHalfEdges.push_back(
                            _Nodes[i].self().edge().next().is_null() ?
                                _Nodes[s].self().edge() :
                                    create_half_edge(_Nodes[s]));
                        continue;
                    }
                    
                    std::cout << "shared "  << _Nodes[s].self().Name << " --> " << _Nodes[t].self().Name << "\n";
                    
                    _Nodes[t].self().edge().twin().NodeRef = _Nodes[s].self().ref();
                    faceHalfEdges.push_back(_Nodes[t].self().edge().twin());
                }

                // connect face
                MeshFaceHandle face = create_face();

                for (int i = 0; i < faceHalfEdges.size(); i++)
                {
                    int s = gs_array_index_clamp(i + 0,_Nodes.size());
                    int t = gs_array_index_clamp(i + 1,_Nodes.size());

                    faceHalfEdges[s].FaceRef = face.ref();
                    faceHalfEdges[t].FaceRef = face.ref();
                    
                    faceHalfEdges[s].self().NextHalfEdgeRef = faceHalfEdges[t].self().ref();
                    faceHalfEdges[t].self().PrevHalfEdgeRef = faceHalfEdges[s].self().ref();
                }

                std::cout << "\n";

                return MeshFaceHandle();
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
                HalfEdges[HalfEdges.size() - 1].NodeRef = _Node.self().ref();
                
                if(_Node.self().edge().is_null())
                    _Node.self().HalfEdgeRef = HalfEdges[HalfEdges.size() - 1].SelfRef;

                // create half edge twin
                MeshHalfEdgeHandle halfEdge = HalfEdges[HalfEdges.size() - 1];
                HalfEdges.push_back(MeshHalfEdgeHandle(this, (REFERENCE)HalfEdges.size()));

                return halfEdge;
            }
        };
    }
}