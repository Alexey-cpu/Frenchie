#pragma once

#include <cstdint>
#include <stdlib.h>
#include <algorithm>

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
            size_t       ElementSize   = 0;
            size_t       ElementsCount = 0;
            size_t       Free          = 0;
            size_t       Head          = 0;
            size_t       Size          = 0;
            char*        Memory        = nullptr;
            MemoryChunk* Next          = nullptr;
            MemoryChunk* Prev          = nullptr;

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
                
                char* buffer         = Memory + Head + amount;
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

        template<typename Type, size_t ChunkSize>
        class MemoryChunkAllocator final
        {
        public:
            MemoryChunkAllocator() : 
                m_Head(new MemoryChunk(sizeof(Type), ChunkSize)), m_Tail(m_Head){}

            ~MemoryChunkAllocator()
            {
                release();
            }

            Type* allocate(size_t _Size)
            {
                auto buffer = m_Head->request(_Size);

                if(buffer != nullptr) 
                    return reinterpret_cast<Type*>(buffer);

                MemoryChunk* chunk  = new MemoryChunk(sizeof(Type), ChunkSize);
                chunk->Next = nullptr;
                chunk->Prev = m_Head;

                m_Head->Next = chunk;
                m_Head = chunk;

                return reinterpret_cast<Type*>(m_Head->request(_Size));
            }

            void deallocate(Type* _Pointer)
            {
                // clear pointer and retrieve allocation info
                auto info  = MemoryChunk::release(_Pointer);
                auto chunk = info != nullptr ? reinterpret_cast<MemoryChunk*>(info->Chunk) : nullptr;

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
                // remove all chunks
                auto next = m_Tail;

                while (next)
                {
                    auto current = next;
                    next = next->Next;
                    delete current;
                }
            }

        private:
            MemoryChunk* m_Head = nullptr;
            MemoryChunk* m_Tail = nullptr;
        };
    }
}