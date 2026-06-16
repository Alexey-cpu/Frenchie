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

            MeshFaceHandle add_face1(std::vector<MeshNodeHandle> _Nodes, const std::string& _Name)
            {
                // create face
                MeshFaceHandle face = create_face();
                face.self().Name = _Name;

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