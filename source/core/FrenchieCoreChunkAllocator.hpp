#pragma once

#include <cstdint>
#include <stdlib.h>
#include <algorithm>
#include <iostream>

namespace Frenchie
{
    namespace Core
    {
        struct AllocationInfo final
        {
            uintptr_t Chunk  = 0;
            uintptr_t Amount = 0;
        };

        struct MemoryChunk final
        {
            // info
            mutable size_t       ElementSize   = 0;
            mutable size_t       ElementsCount = 0;
            mutable size_t       Free          = 0;
            mutable size_t       Head          = 0;
            mutable size_t       Size          = 0;
            mutable char*        Memory        = nullptr;
            mutable MemoryChunk* Next          = nullptr;
            mutable MemoryChunk* Prev          = nullptr;

            MemoryChunk(size_t _ChunkElementSize, size_t _ChunkSize);
            ~MemoryChunk();

            void* request(size_t _Size);
            static AllocationInfo* release(void* _Pointer);

            bool is_free() const;
        };

        template<typename Type>
        class MemoryChunkAllocator final
        {
        public:
            MemoryChunkAllocator(size_t _ChunkSize) : 
                m_ChunkSize(std::max<size_t>(_ChunkSize, 16)), 
                m_Head(new MemoryChunk(sizeof(Type), m_ChunkSize)), 
                m_Tail(m_Head){}

            ~MemoryChunkAllocator()
            {
                release();
            }

            Type* allocate(size_t _Size) const
            {
                // create chunks list
                if(m_Head == nullptr && m_Tail == nullptr)
                {
                    m_Head = new MemoryChunk(sizeof(Type), m_ChunkSize);
                    m_Tail= m_Head;
                }

                // allocate buffer
                auto buffer = m_Head->request(_Size);

                if(buffer != nullptr) 
                    return reinterpret_cast<Type*>(buffer);

                m_ChunkSize = std::max<size_t>(m_ChunkSize, _Size);

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
                if(chunk == nullptr || 
                    (chunk->Prev == nullptr && chunk->Next == nullptr) || !chunk->is_free()) 
                {
                    return;
                }

                // update chunk links
                if(chunk->Prev != nullptr)
                {
                    chunk->Prev->Next = chunk->Next;
                }
                else
                {
                    // update tail
                    m_Tail = chunk->Next;
                }
                
                if(chunk->Next != nullptr)
                {
                    chunk->Next->Prev = chunk->Prev;
                }
                else 
                {
                    // update head
                    m_Head = chunk->Prev;
                }

                // remove chunk
                delete chunk;
            }

            void release()
            {
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

            size_t get_total_memory_size() const
            {
                size_t freeMemory = 0;

                auto next = m_Tail;

                while (next)
                {
                    freeMemory  += next->Size;
                    next = next->Next;
                }

                return freeMemory;
            }

            size_t get_free_memory_amount() const
            {
                size_t freeMemory = 0;

                auto next = m_Tail;

                while (next)
                {
                    freeMemory  += next->Free;
                    next = next->Next;
                }

                return freeMemory;
            }

            size_t get_busy_memory_amount() const
            {
                size_t freeMemory = 0;

                auto next = m_Tail;

                while (next)
                {
                    freeMemory += next->Head;
                    next = next->Next;
                }

                return freeMemory;
            }

        private:
            
            mutable  size_t       m_ChunkSize = 1024;
            mutable  MemoryChunk* m_Head      = nullptr;
            mutable  MemoryChunk* m_Tail      = nullptr;
        };
    }
}