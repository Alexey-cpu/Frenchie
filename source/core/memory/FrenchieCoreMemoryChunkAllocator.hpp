#pragma once

#include <cstdint>
#include <stdlib.h>
#include <algorithm>

namespace Frenchie
{
    namespace Core
    {
        namespace Memory
        {
            struct AllocationInfo final
            {
                uintptr_t Chunk  = 0;
                uintptr_t Amount = 0;
            };

            template<typename Type, bool DeleteLater = false>
            class MemoryChunkAllocator final
            {
            public:

                // nested types
                struct MemoryChunk final
                {
                    // info
                    mutable uintptr_t    ElementSize   = 0;
                    mutable uintptr_t    ElementsCount = 0;
                    mutable uintptr_t    Free          = 0;
                    mutable uintptr_t    Head          = 0;
                    mutable uintptr_t    Size          = 0;
                    mutable char*        Memory        = nullptr;
                    mutable MemoryChunk* Next          = nullptr;
                    mutable MemoryChunk* Prev          = nullptr;

                    MemoryChunk(uintptr_t _ChunkElementSize, uintptr_t _ChunkElementsCount)
                    {
                        ElementSize   = std::max<uintptr_t>(_ChunkElementSize, 1);
                        ElementsCount = std::max<uintptr_t>(_ChunkElementsCount, 1);
                        Free          = (sizeof(AllocationInfo) + ElementSize) * ElementsCount;
                        Head          = 0;
                        Size          = Free;
                        Memory        = reinterpret_cast<char*>(malloc(Size * sizeof(char)));
                    }

                    ~MemoryChunk()
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

                    void* request(uintptr_t _Size)
                    {
                        uintptr_t amount = sizeof(AllocationInfo) + std::max<uintptr_t>(_Size, 1) * ElementSize;

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

                    static AllocationInfo* release(void* _Pointer)
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

                    bool is_free() const
                    {
                        return Free >= Size;
                    }
                };

                MemoryChunkAllocator(uintptr_t _ChunkSize) : 
                    m_ChunkSize(std::max<uintptr_t>(_ChunkSize, 16)), 
                    m_Head(new MemoryChunk(sizeof(Type), m_ChunkSize)), 
                    m_Tail(m_Head){}

                ~MemoryChunkAllocator()
                {
                    release();
                }

                Type* allocate(uintptr_t _Size) const
                {
                    // create chunks list
                    if(m_Head == nullptr && m_Tail == nullptr)
                    {
                        m_Head = new MemoryChunk(sizeof(Type), m_ChunkSize);
                        m_Tail = m_Head;
                    }

                    // try to allocate memory
                    void* buffer = m_Head->request(_Size);
                    if(buffer != nullptr)
                        return reinterpret_cast<Type*>(buffer);

                    // adjust chunk size
                    m_ChunkSize = std::max<uintptr_t>(m_ChunkSize, _Size);

                    // allocate new chunk
                    MemoryChunk* chunk  = new MemoryChunk(sizeof(Type), m_ChunkSize);
                    chunk->Next = nullptr;
                    chunk->Prev = m_Head;

                    m_Head->Next = chunk;
                    m_Head = chunk;

                    return reinterpret_cast<Type*>(m_Head->request(_Size));
                }

                void deallocate(Type* _Pointer) const
                {
                    if(m_Head == nullptr && m_Tail == nullptr) 
                        return;

                    // clear pointer and retrieve allocation info
                    auto info  = MemoryChunk::release(_Pointer);
                    auto chunk = info != nullptr ? reinterpret_cast<MemoryChunk*>(info->Chunk) : nullptr;

                    // check that this is the first chunk
                    if(chunk == nullptr || (chunk->Prev == nullptr && chunk->Next == nullptr) || !chunk->is_free() || DeleteLater)
                        return;

                    remove_chunk(chunk);
                }

                template<typename ... Args>
                Type* construct(Args ... _Args) const
                {
                    Type* memory = allocate(1);
                    return new(memory) Type(_Args...);
                }

                void destroy(Type* _Object) const
                {
                    if(_Object == nullptr)
                        return;

                    _Object->~Type();
                    deallocate(_Object);
                }

                void release()
                {
                    // remove all chunks
                    auto next = m_Tail;

                    while (next)
                    {
                        auto current = next;
                        next = next->Next;
                        delete current;
                    }

                    // clean up tail
                    m_Tail = nullptr;
                    m_Head = nullptr;
                }

                void release_unused_chunks()
                {
                    if(m_Head == nullptr && m_Tail == nullptr) 
                        return;

                    if(m_Head == m_Tail)
                        return;

                    // remove unused chunks
                    MemoryChunk* next = m_Tail;

                    while (next)
                    {
                        MemoryChunk* chunk = next;
                        next = next->Next;
                        
                        if(chunk->is_free())
                            remove_chunk(chunk);
                    }
                }

                uintptr_t get_total_memory_size() const
                {
                    uintptr_t freeMemory = 0;

                    auto next = m_Tail;

                    while (next)
                    {
                        freeMemory  += next->Size;
                        next = next->Next;
                    }

                    return freeMemory;
                }

                uintptr_t get_free_memory_amount() const
                {
                    uintptr_t freeMemory = 0;

                    auto next = m_Tail;

                    while (next)
                    {
                        freeMemory  += next->Free;
                        next = next->Next;
                    }

                    return freeMemory;
                }

                uintptr_t get_busy_memory_amount() const
                {
                    uintptr_t freeMemory = 0;

                    auto next = m_Tail;

                    while (next)
                    {
                        freeMemory += next->Head;
                        next = next->Next;
                    }

                    return freeMemory;
                }

                uintptr_t get_total_chunks_number() const
                {
                    uintptr_t totalChunksNumber = 0;

                    auto next = m_Tail;

                    while (next)
                    {
                        ++totalChunksNumber;
                        next = next->Next;
                    }

                    return totalChunksNumber;
                }

                uintptr_t get_free_chunks_number() const
                {
                    uintptr_t freeChunksNumber = 0;

                    auto next = m_Tail;

                    while (next)
                    {
                        if(next->is_free())
                            ++freeChunksNumber;
                        next = next->Next;
                    }

                    return freeChunksNumber;
                }

            private:
                
                void isnert_chunk(MemoryChunk* _What, MemoryChunk* _Where) const
                {
                }

                void remove_chunk(MemoryChunk* _Chunk) const
                {
                    // check if tail needs update
                    if(_Chunk->Prev != nullptr)
                        _Chunk->Prev->Next = _Chunk->Next;
                    else
                        m_Tail = _Chunk->Next;
                    
                    // check if head needs update
                    if(_Chunk->Next != nullptr)
                        _Chunk->Next->Prev = _Chunk->Prev;
                    else
                        m_Head = _Chunk->Prev;

                    // remove chunk
                    delete _Chunk;
                }

                mutable  uintptr_t    m_ChunkSize = 1024;
                mutable  MemoryChunk* m_Head      = nullptr;
                mutable  MemoryChunk* m_Tail      = nullptr;
            };
        }
    }
}