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
                int       Amount = 0;
            };

            // nested types
            struct MemoryChunk final : public Frenchie::Core::Containers::ObjectTreeLeaf<MemoryChunk>
            {
                // info
                mutable int   ElementSize   = 0;
                mutable int   ElementsCount = 0;
                mutable int   Free          = 0;
                mutable int   Busy          = 0;
                mutable int   Size          = 0;
                mutable char* Memory        = nullptr;

                MemoryChunk(const int& _ChunkElementSize, const int& _ChunkElementsCount)
                {
                    ElementSize   = std::max<int>(_ChunkElementSize, 1);
                    ElementsCount = std::max<int>(_ChunkElementsCount, 1);
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

                void* request(const int& _Size)
                {
                    int amount = sizeof(AllocationInfo) + std::max<int>(_Size, 1) * ElementSize;

                    if(Busy + amount > Size) 
                        return nullptr; // out-of memory
                    
                    char*           buffer = Memory + Busy + sizeof(AllocationInfo);
                    AllocationInfo* info   = reinterpret_cast<AllocationInfo*>(buffer - sizeof(AllocationInfo));
                    info->Chunk            = reinterpret_cast<uintptr_t>(this);
                    info->Amount           = amount;
                    Busy                  += amount;
                    Free                  -= amount;

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
            class MemoryChunkAllocator final : public Frenchie::Core::Containers::ObjectTreeRoot<MemoryChunk>
            {
            public:

                MemoryChunkAllocator(const int& _ChunkSize = 32) : m_ChunkSize(std::max<int>(_ChunkSize, 16)){}

                ~MemoryChunkAllocator(){}

                Type* allocate(const int& _Size) const
                {
                    // create chunks list
                    if(empty())
                    {
                        m_CurrentChunk = raw_memory_insert_after(raw_memory_last(), (int)sizeof(Type), m_ChunkSize);
                        ++m_TotalChunks;
                    }

                    // try to allocate memory using current chunk
                    for(; m_CurrentChunk != raw_memory_end(); m_CurrentChunk = raw_memory_next(m_CurrentChunk))
                    {
                        void* buffer = m_CurrentChunk->request(_Size);
                        if(buffer != nullptr)
                            return reinterpret_cast<Type*>(buffer);
                    }

                    // adjust chunk size
                    m_ChunkSize    = std::max<int>(m_ChunkSize * 2, _Size);
                    m_CurrentChunk = raw_memory_insert_after(raw_memory_last(), sizeof(Type), m_ChunkSize);
                    ++m_TotalChunks;

                    // request memory from just allocated chunk
                    return reinterpret_cast<Type*>(m_CurrentChunk->request(_Size));
                }

                void deallocate(Type* _Pointer) const
                {
                    // clear pointer and retrieve allocation info
                    auto info  = MemoryChunk::release(_Pointer);
                    auto chunk = info != nullptr ? reinterpret_cast<MemoryChunk*>(info->Chunk) : nullptr;

                    // check that this is the first chunk
                    if(chunk == nullptr || empty() || !chunk->is_free())
                        return;

                    --m_TotalChunks;

                    if(DeleteLater)
                    {
                        if(m_TotalChunks <= 0)
                        {
                            m_TotalChunks  = 0;
                            m_CurrentChunk = raw_memory_first();
                        }
                    }
                    else
                    {
                        raw_memory_remove(chunk);
                    }
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

                int get_total_memory_size() const
                {
                    if (empty())
                        return 0;

                    int freeMemory = 0;

                    for(auto&& child : *this)
                        freeMemory += child->Size;

                    return freeMemory;
                }

                int get_free_memory_amount() const
                {
                    if (empty())
                        return 0;

                    int freeMemory = 0;

                    for(auto&& child : *this)
                        freeMemory += child->Free;

                    return freeMemory;
                }

                int get_busy_memory_amount() const
                {
                    if (empty())
                        return 0;

                    int freeMemory = 0;

                    for(auto&& child : *this)
                        freeMemory += child->Busy;

                    return freeMemory;
                }

                int get_total_chunks_number() const
                {
                    int totalChunksNumber = 0;

                    for(auto&& child : *this)
                        ++totalChunksNumber;

                    return totalChunksNumber;
                }

                int get_free_chunks_number() const
                {
                    int freeChunksNumber = 0;

                    for(auto&& child : *this)
                    {
                        if(child->is_free())
                            ++freeChunksNumber;
                    }

                    return freeChunksNumber;
                }

            private:

                mutable int          m_ChunkSize    {1024};
                mutable MemoryChunk* m_CurrentChunk {nullptr};
                mutable int          m_TotalChunks  {0};
            };
        }
    }
}