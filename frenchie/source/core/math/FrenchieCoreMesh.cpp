#include <FrenchieCoreMesh.hpp>

using namespace Frenchie::Core;

// MeshNodeHandle
MeshNodeHandle::MeshNodeHandle(const MeshSurfaceHandle* _Surface, const REFERENCE& _Reference) : MeshHandle<MeshNodeHandle>(_Surface, _Reference){}
MeshNodeHandle::~MeshNodeHandle(){}

MeshHandle<MeshNodeHandle>::Handle& MeshNodeHandle::self() const
{
    return this->Surface->Nodes[this->SelfRef];
}

MeshHalfEdgeHandle& MeshNodeHandle::get_edge() const
{
    return
        this->HalfEdgeRef != NULLREF && this->Surface != nullptr && this->HalfEdgeRef < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[this->HalfEdgeRef] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

void MeshNodeHandle::set_edge(const MeshHalfEdgeHandle& _Edge)
{
    this->HalfEdgeRef = _Edge.is_not_null() ? _Edge.self().ref() : NULLREF;
}

// MeshFaceHandle
MeshFaceHandle::MeshFaceHandle(const MeshSurfaceHandle* _Surface, const REFERENCE& _Reference) : MeshHandle<MeshFaceHandle>(_Surface, _Reference){}
MeshFaceHandle::~MeshFaceHandle(){}

MeshHandle<MeshFaceHandle>::Handle& MeshFaceHandle::self() const
{
    return this->Surface->Faces[this->SelfRef];
}

MeshHalfEdgeHandle& MeshFaceHandle::get_edge() const
{
    return
        this->HalfEdgeRef != NULLREF && this->Surface != nullptr && this->HalfEdgeRef < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[this->HalfEdgeRef] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

void MeshFaceHandle::set_edge(const MeshHalfEdgeHandle& _Edge)
{
    this->HalfEdgeRef = _Edge.is_not_null() ? _Edge.self().ref() : NULLREF;
}

// MeshEdgeHandle
MeshHalfEdgeHandle::MeshHalfEdgeHandle(const MeshSurfaceHandle* _Surface, const REFERENCE& _Reference) : MeshHandle<MeshHalfEdgeHandle>(_Surface, _Reference){}
MeshHalfEdgeHandle::~MeshHalfEdgeHandle(){}

MeshHandle<MeshHalfEdgeHandle>::Handle& MeshHalfEdgeHandle::self() const
{
    return this->Surface->HalfEdges[this->SelfRef];
}

MeshNodeHandle& MeshHalfEdgeHandle::get_node() const
{
    return
        this->NodeRef != NULLREF && this->Surface != nullptr  && this->NodeRef < this->Surface->Nodes.size() ?
            this->Surface->Nodes[this->NodeRef] :
                MeshSurfaceHandle::FallbackNode;
}

MeshFaceHandle& MeshHalfEdgeHandle::get_face() const
{
    return
        this->FaceRef != NULLREF && this->Surface != nullptr  && this->FaceRef < this->Surface->Faces.size() ?
            this->Surface->Faces[this->FaceRef] :
                MeshSurfaceHandle::FallbackFace;
}

MeshHalfEdgeHandle& MeshHalfEdgeHandle::get_next() const
{
    return
        this->NextHalfEdgeRef != NULLREF && this->Surface != nullptr  && this->NextHalfEdgeRef < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[NextHalfEdgeRef] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

MeshHalfEdgeHandle& MeshHalfEdgeHandle::get_prev() const
{
    return
        this->PrevHalfEdgeRef != NULLREF && this->Surface != nullptr  && this->PrevHalfEdgeRef < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[PrevHalfEdgeRef] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

MeshHalfEdgeHandle& MeshHalfEdgeHandle::get_twin() const
{
    return
        this->SelfRef != NULLREF && this->Surface != nullptr && (this->TwinHalfEdgeRef) < this->Surface->HalfEdges.size() ?
            this->Surface->HalfEdges[this->TwinHalfEdgeRef] :
                MeshSurfaceHandle::FallbackHalfEdge;
}

void MeshHalfEdgeHandle::set_node(const MeshNodeHandle& _Node)
{
    self().NodeRef = _Node.is_not_null() ? _Node.self().ref() : NULLREF;
}

void MeshHalfEdgeHandle::set_face(const MeshFaceHandle& _Face)
{
    self().FaceRef = _Face.is_not_null() ? _Face.self().ref() : NULLREF;
}

void MeshHalfEdgeHandle::set_next(const MeshHalfEdgeHandle& _Edge)
{
    self().NextHalfEdgeRef = _Edge.is_not_null() ? _Edge.self().ref() : NULLREF;
}

void MeshHalfEdgeHandle::set_prev(const MeshHalfEdgeHandle& _Edge)
{
    self().PrevHalfEdgeRef = _Edge.is_not_null() ? _Edge.self().ref() : NULLREF;
}

void MeshHalfEdgeHandle::set_twin(const MeshHalfEdgeHandle& _Edge)
{
    if(_Edge.is_not_null())
    {
        self().TwinHalfEdgeRef = _Edge.self().ref();
        _Edge.self().TwinHalfEdgeRef = self().ref();
        return;
    }

    self().TwinHalfEdgeRef = NULLREF;
}

// MeshSurfaceHandle
MeshNodeHandle     MeshSurfaceHandle::FallbackNode     = MeshNodeHandle();
MeshFaceHandle     MeshSurfaceHandle::FallbackFace     = MeshFaceHandle();
MeshHalfEdgeHandle MeshSurfaceHandle::FallbackHalfEdge = MeshHalfEdgeHandle();