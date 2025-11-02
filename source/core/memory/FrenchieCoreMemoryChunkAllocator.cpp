#include <FrenchieCoreMemoryChunkAllocator.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Memory;

MemoryChunk::MemoryChunk(size_t _ChunkElementSize, size_t _ChunkSize)
{
    ElementSize   = std::max<size_t>(_ChunkElementSize, 1);
    ElementsCount = std::max<size_t>(_ChunkSize, 1);
    Free          = (sizeof(AllocationInfo) + ElementSize) * ElementsCount;
    Head          = 0;
    Size          = Free;
    Memory        = reinterpret_cast<char*>(malloc(Size));
}

MemoryChunk::~MemoryChunk()
{
    ElementSize   = 0;
    ElementsCount = 0;
    Free          = 0;
    Head          = 0;
    Size          = 0;
    free(Memory);
    Memory = nullptr;
    Next   = nullptr;
    Prev   = nullptr;
}

void* MemoryChunk::request(size_t _Size)
{
    size_t amount = sizeof(AllocationInfo) + std::max<size_t>(_Size, 1) * ElementSize;

    if(Head + amount > Size) 
        return nullptr; // out-of memory
    
    char* buffer         = Memory + Head + sizeof(AllocationInfo);
    AllocationInfo* info = reinterpret_cast<AllocationInfo*>(buffer - sizeof(AllocationInfo));
    info->Chunk          = reinterpret_cast<uintptr_t>(this);
    info->Amount         = amount;
    Head                += amount;
    Free                -= amount;

    return buffer;
}

AllocationInfo* MemoryChunk::release(void* _Pointer)
{
    if(_Pointer == nullptr)
        return nullptr;

    AllocationInfo* info  = reinterpret_cast<AllocationInfo*>(reinterpret_cast<char*>(_Pointer) - sizeof(AllocationInfo));
    MemoryChunk*    chunk = reinterpret_cast<MemoryChunk*>(info->Chunk);
    chunk->Free += info->Amount;

    if(chunk->Free >= chunk->Size)
    {
        chunk->Head = 0;
        chunk->Free = chunk->Size;
    }

    return info;
}

bool MemoryChunk::is_free() const
{
    return Free >= Size;
}