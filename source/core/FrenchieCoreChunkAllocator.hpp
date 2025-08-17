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

            MemoryChunk(size_t _ChunkElementSize, size_t _ChunkSize)
            {
                ElementSize   = std::max<size_t>(_ChunkElementSize, 1);
                ElementsCount = std::max<size_t>(_ChunkSize, 1);
                Free          = (sizeof(AllocationInfo) + ElementSize) * ElementsCount;
                Head          = 0;
                Size          = Free;
                Memory        = reinterpret_cast<char*>(malloc(Size));
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

            void* request(size_t _Size)
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
                if(m_Tail->Next == nullptr) 
                {
                    // clean up tail
                    m_Tail->Prev = nullptr;
                    m_Tail->Next = nullptr;
                    m_Head       = m_Tail;
                    m_Tail->Head = 0;
                    m_Tail->Free = m_Tail->Size;

                    return;
                }

                // remove all chunks besides the first one !!!
                auto next = m_Tail->Next;

                while (next)
                {
                    auto current = next;
                    next = next->Next;
                    delete current;
                }

                // clean up tail
                m_Tail->Prev = nullptr;
                m_Tail->Next = nullptr;
                m_Head       = m_Tail;
                m_Tail->Head = 0;
                m_Tail->Free = m_Tail->Size;
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

        //private:
            
            mutable  size_t       m_ChunkSize = 1024;
            mutable  MemoryChunk* m_Head      = nullptr;
            mutable  MemoryChunk* m_Tail      = nullptr;
        };
    }
}