#include <FrenchieCoreMesh.hpp>

using namespace Frenchie::Core;

// MeshNodeHandle
MeshNodeHandle::MeshNodeHandle(const MeshSurfaceHandle* _Surface, const REFERENCE& _Reference) : MeshHandle<MeshNodeHandle>(_Surface, _Reference){}
MeshNodeHandle::~MeshNodeHandle(){}

MeshHandle<MeshNodeHandle>::Handle& MeshNodeHandle::self() const
{
    return this->Surface->Nodes[this->SelfRef];
}

MeshHalfEdgeHandle& MeshNodeHandle::edge() const
{
    return
        this->HalfEdgeRef != NULLREF && this->Surface != nullptr && this->HalfEdgeRef < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[this->HalfEdgeRef] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

// MeshEdgeHandle
MeshHalfEdgeHandle::MeshHalfEdgeHandle(const MeshSurfaceHandle* _Surface, const REFERENCE& _Reference) : MeshHandle<MeshHalfEdgeHandle>(_Surface, _Reference){}
MeshHalfEdgeHandle::~MeshHalfEdgeHandle(){}

MeshHandle<MeshHalfEdgeHandle>::Handle& MeshHalfEdgeHandle::self() const
{
    return this->Surface->HalfEdges[this->SelfRef];
}

MeshNodeHandle& MeshHalfEdgeHandle::node() const
{
    return
        this->NodeRef != NULLREF && this->Surface != nullptr  && this->NodeRef < this->Surface->Nodes.size() ?
            this->Surface->Nodes[this->NodeRef] :
                MeshSurfaceHandle::FallbackNode;
}

MeshFaceHandle& MeshHalfEdgeHandle::face() const
{
    return
        this->FaceRef != NULLREF && this->Surface != nullptr  && this->FaceRef < this->Surface->Faces.size() ?
            this->Surface->Faces[this->FaceRef] :
                MeshSurfaceHandle::FallbackFace;
}

MeshHalfEdgeHandle& MeshHalfEdgeHandle::next() const
{
    return
        this->NextHalfEdgeRef != NULLREF && this->Surface != nullptr  && this->NextHalfEdgeRef < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[NextHalfEdgeRef] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

MeshHalfEdgeHandle& MeshHalfEdgeHandle::prev() const
{
    return
        this->PrevHalfEdgeRef != NULLREF && this->Surface != nullptr  && this->PrevHalfEdgeRef < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[PrevHalfEdgeRef] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

MeshHalfEdgeHandle& MeshHalfEdgeHandle::twin() const
{
    return
        this->SelfRef != NULLREF && this->Surface != nullptr && (this->SelfRef ^ 1) < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[this->SelfRef ^ 1] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

bool MeshHalfEdgeHandle::shared() const
{
    return self().twin().is_not_null() && self().twin().node().is_not_null();
}

// MeshFaceHandle
MeshFaceHandle::MeshFaceHandle(const MeshSurfaceHandle* _Surface, const REFERENCE& _Reference) : MeshHandle<MeshFaceHandle>(_Surface, _Reference){}
MeshFaceHandle::~MeshFaceHandle(){}

MeshHandle<MeshFaceHandle>::Handle& MeshFaceHandle::self() const
{
    return this->Surface->Faces[this->SelfRef];
}

MeshHalfEdgeHandle& MeshFaceHandle::edge() const
{
    return
        this->HalfEdgeRef != NULLREF && this->Surface != nullptr  && this->HalfEdgeRef < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[HalfEdgeRef] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

// MeshSurfaceHandle
MeshNodeHandle     MeshSurfaceHandle::FallbackNode     = MeshNodeHandle();
MeshFaceHandle     MeshSurfaceHandle::FallbackFace     = MeshFaceHandle();
MeshHalfEdgeHandle MeshSurfaceHandle::FallbackHalfEdge = MeshHalfEdgeHandle();