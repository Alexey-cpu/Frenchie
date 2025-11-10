#pragma once

#include <cstdint>
#include <stdlib.h>
#include <algorithm>

#include <FrenchieCoreContainersObjectList.hpp>

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

            // nested types
            struct MemoryChunk final : public Frenchie::Core::Containers::ObjectTreeLeaf<MemoryChunk, Frenchie::Core::Memory::DefaultAllocator<MemoryChunk>>
            {
                // info
                mutable uintptr_t ElementSize   = 0;
                mutable uintptr_t ElementsCount = 0;
                mutable uintptr_t Free          = 0;
                mutable uintptr_t Busy          = 0;
                mutable uintptr_t Size          = 0;
                mutable char*     Memory        = nullptr;

                MemoryChunk(uintptr_t _ChunkElementSize, uintptr_t _ChunkElementsCount)
                {
                    ElementSize   = std::max<uintptr_t>(_ChunkElementSize, 1);
                    ElementsCount = std::max<uintptr_t>(_ChunkElementsCount, 1);
                    Free          = (sizeof(AllocationInfo) + ElementSize) * ElementsCount;
                    Busy          = 0;
                    Size          = Free;
                    Memory        = reinterpret_cast<char*>(malloc(Size * sizeof(char)));
                }

                ~MemoryChunk()
                {
                    ElementSize   = 0;
                    ElementsCount = 0;
                    Free          = 0;
                    Busy          = 0;
                    Size          = 0;
                    free(Memory);
                    Memory = nullptr;
                }

                void* request(uintptr_t _Size)
                {
                    uintptr_t amount = sizeof(AllocationInfo) + std::max<uintptr_t>(_Size, 1) * ElementSize;

                    if(Busy + amount > Size) 
                        return nullptr; // out-of memory
                    
                    char* buffer         = Memory + Busy + sizeof(AllocationInfo);
                    AllocationInfo* info = reinterpret_cast<AllocationInfo*>(buffer - sizeof(AllocationInfo));
                    info->Chunk          = reinterpret_cast<uintptr_t>(this);
                    info->Amount         = amount;
                    Busy                += amount;
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
                        chunk->Busy = 0;
                        chunk->Free = chunk->Size;
                    }

                    return info;
                }

                bool is_free() const
                {
                    return Free >= Size;
                }
            };

            template<typename Type, bool DeleteLater = false>
            class MemoryChunkAllocator final  :
                public Frenchie::Core::Containers::ObjectTreeRoot<MemoryChunk, Frenchie::Core::Memory::DefaultAllocator<MemoryChunk>>
            {
            public:

                MemoryChunkAllocator(uintptr_t _ChunkSize) : m_ChunkSize(std::max<uintptr_t>(_ChunkSize, 16)){}

                ~MemoryChunkAllocator(){}

                Type* allocate(uintptr_t _Size) const
                {
                    // create chunks list
                    if(empty())
                        raw_memory_insert_after(raw_memory_last(), sizeof(Type), m_ChunkSize);

                    // try to allocate memory
                    void* buffer = raw_memory_last()->request(_Size);
                    if(buffer != nullptr)
                        return reinterpret_cast<Type*>(buffer);

                    // adjust chunk size
                    m_ChunkSize = std::max<uintptr_t>(m_ChunkSize, _Size);

                    // allocate new chunk and request memory from it
                    return reinterpret_cast<Type*>(
                        raw_memory_insert_after(raw_memory_last(), sizeof(Type), m_ChunkSize)->request(_Size));
                }

                void deallocate(Type* _Pointer) const
                {
                    // clear pointer and retrieve allocation info
                    auto info  = MemoryChunk::release(_Pointer);
                    auto chunk = info != nullptr ? reinterpret_cast<MemoryChunk*>(info->Chunk) : nullptr;

                    // check that this is the first chunk
                    if(chunk == nullptr || empty() || !chunk->is_free() || DeleteLater)
                        return;

                    raw_memory_remove(chunk);
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
                    clear();
                }

                void release_unused_chunks()
                {
                    // if (empty())
                    //     return;

                    // // remove unused chunks
                    // auto next = m_Tail;

                    // while (next)
                    // {
                    //     auto chunk = next;
                    //     next = next->Next;
                        
                    //     if(chunk->is_free())
                    //         raw_memory_remove(chunk);
                    // }
                }

                uintptr_t get_total_memory_size() const
                {
                    if (empty())
                        return 0;

                    uintptr_t freeMemory = 0;

                    for(auto&& child : *this)
                        freeMemory += child->Size;

                    return freeMemory;
                }

                uintptr_t get_free_memory_amount() const
                {
                    if (empty())
                        return 0;

                    uintptr_t freeMemory = 0;

                    for(auto&& child : *this)
                        freeMemory += child->Free;

                    return freeMemory;
                }

                uintptr_t get_busy_memory_amount() const
                {
                    if (empty())
                        return 0;

                    uintptr_t freeMemory = 0;

                    for(auto&& child : *this)
                        freeMemory += child->Busy;

                    return freeMemory;
                }

                uintptr_t get_total_chunks_number() const
                {
                    uintptr_t totalChunksNumber = 0;

                    for(auto&& child : *this)
                        ++totalChunksNumber;

                    return totalChunksNumber;
                }

                uintptr_t get_free_chunks_number() const
                {
                    uintptr_t freeChunksNumber = 0;

                    for(auto&& child : *this)
                    {
                        if(child->is_free())
                            ++freeChunksNumber;
                    }

                    return freeChunksNumber;
                }

            private:

                mutable uintptr_t m_ChunkSize = 1024;
            };
        }
    }
}