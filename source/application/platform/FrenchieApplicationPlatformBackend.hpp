#pragma once

#include <functional>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

// STL
#include <filesystem>
#include <utility>
#include <memory>
#include <vector>
#include <string>

namespace Frenchie
{
    namespace Application
    {
        template<typename Type>
        class PlatformBackendRendererAllocator final
        {
        public:

            // nested types
            struct PlatformBackendAllocatorAllocationInfo final
            {
                uintptr_t Chunk {0};
                uintptr_t Amount{0};
            };

            struct PlatformBackendAllocatorMemoryChunk final
            {
                mutable uintptr_t                            ElementSize   = 0;
                mutable uintptr_t                            ElementsCount = 0;
                mutable uintptr_t                            Free          = 0;
                mutable uintptr_t                            Head          = 0;
                mutable uintptr_t                            Size          = 0;
                mutable char*                                Memory        = nullptr;
                mutable PlatformBackendAllocatorMemoryChunk* Next          = nullptr;
                mutable PlatformBackendAllocatorMemoryChunk* Prev          = nullptr;

                PlatformBackendAllocatorMemoryChunk(uintptr_t _ChunkElementSize, uintptr_t _ChunkSize)
                {
                    ElementSize   = std::max<uintptr_t>(_ChunkElementSize, 1);
                    ElementsCount = std::max<uintptr_t>(_ChunkSize, 1);
                    Free          = (sizeof(PlatformBackendAllocatorAllocationInfo) + ElementSize) * ElementsCount;
                    Head          = 0;
                    Size          = Free;
                    Memory        = reinterpret_cast<char*>(malloc(Size * sizeof(char)));
                }

                ~PlatformBackendAllocatorMemoryChunk()
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

                void* request(uintptr_t _Size) const
                {
                    uintptr_t amount = sizeof(PlatformBackendAllocatorAllocationInfo) + std::max<uintptr_t>(_Size, 1) * ElementSize;

                    if(Head + amount > Size) 
                        return nullptr; // out-of memory
                    
                    char* buffer         = Memory + Head + sizeof(PlatformBackendAllocatorAllocationInfo);
                    PlatformBackendAllocatorAllocationInfo* info = reinterpret_cast<PlatformBackendAllocatorAllocationInfo*>(buffer - sizeof(PlatformBackendAllocatorAllocationInfo));
                    info->Chunk          = reinterpret_cast<uintptr_t>(this);
                    info->Amount         = amount;
                    Head                += amount;
                    Free                -= amount;

                    return buffer;
                }

                PlatformBackendAllocatorAllocationInfo* release(void* _Pointer) const
                {
                    if(_Pointer == nullptr)
                        return nullptr;

                    PlatformBackendAllocatorAllocationInfo* info  = reinterpret_cast<PlatformBackendAllocatorAllocationInfo*>(reinterpret_cast<char*>(_Pointer) - sizeof(PlatformBackendAllocatorAllocationInfo));
                    PlatformBackendAllocatorMemoryChunk*    chunk = reinterpret_cast<PlatformBackendAllocatorMemoryChunk*>(info->Chunk);
                    chunk->Free += info->Amount;

                    if(chunk->Free >= chunk->Size)
                    {
                        chunk->Head = 0;
                        chunk->Free = chunk->Size;
                    }

                    return info;
                }

                bool PlatformBackendAllocatorMemoryChunk::is_free() const
                {
                    return Free >= Size;
                }
            };

            // constructors
            PlatformBackendRendererAllocator(uintptr_t _ChunkSize = 16) : 
                m_ChunkSize(std::max<uintptr_t>(_ChunkSize, 16)), 
                m_Head(new PlatformBackendAllocatorMemoryChunk(sizeof(Type), m_ChunkSize)), 
                m_Tail(m_Head){}

            ~PlatformBackendRendererAllocator()
            {
                release();
            }

            Type* allocate(uintptr_t _Size) const
            {
                // create chunks list
                if(m_Head == nullptr && m_Tail == nullptr)
                {
                    m_Head = new PlatformBackendAllocatorMemoryChunk(sizeof(Type), m_ChunkSize);
                    m_Tail= m_Head;
                }

                // allocate buffer
                auto buffer = m_Head->request(_Size);

                if(buffer != nullptr) 
                    return reinterpret_cast<Type*>(buffer);

                m_ChunkSize = std::max<uintptr_t>(m_ChunkSize, _Size);

                PlatformBackendAllocatorMemoryChunk* chunk  = new PlatformBackendAllocatorMemoryChunk(sizeof(Type), m_ChunkSize);
                chunk->Next = nullptr;
                chunk->Prev = m_Head;

                m_Head->Next = chunk;
                m_Head = chunk;

                return reinterpret_cast<Type*>(m_Head->request(_Size));
            }

            void  deallocate(Type* _Pointer) const
            {
                if(m_Head == nullptr && m_Tail == nullptr) 
                    return;

                // clear pointer and retrieve allocation info
                auto info  = PlatformBackendAllocatorMemoryChunk::release(_Pointer);
                auto chunk = info != nullptr ? reinterpret_cast<PlatformBackendAllocatorMemoryChunk*>(info->Chunk) : nullptr;

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

            void release() const
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
                free(_Object);
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

        private:
            
            mutable  uintptr_t                            m_ChunkSize = 1024;
            mutable  PlatformBackendAllocatorMemoryChunk* m_Head      = nullptr;
            mutable  PlatformBackendAllocatorMemoryChunk* m_Tail      = nullptr;
        };

        enum PlatformBackendRendererContextHints_ : int
        {
            // tweaks
            PlatformBackendRendererContextHints_None,
            PlatformBackendRendererContextHints_Visible,
            PlatformBackendRendererContextHints_Decorated,
            PlatformBackendRendererContextHints_Resizable,
            PlatformBackendRendererContextHints_Iconified,
            PlatformBackendRendererContextHints_Focused,

            // defaults
            PlatformBackendContextWindowHints_Default =
                PlatformBackendRendererContextHints_Visible   |
                PlatformBackendRendererContextHints_Decorated |
                PlatformBackendRendererContextHints_Resizable |
                PlatformBackendRendererContextHints_Focused
        };

        enum PlatformBackendRendererHints_ : int
        {
            // tweaks
            PlatformBackendRendererHints_None,
            PlatformBackendRendererHints_ClearColorBuffer,
            PlatformBackendRendererHints_ClearDepthBuffer,
            PlatformBackendRendererHints_ClearStencilBuffer,
            PlatformBackendRendererHints_PollEvents,

            // defaults
            PlatformBackendRendererHints_Default =
                PlatformBackendRendererHints_ClearColorBuffer   |
                PlatformBackendRendererHints_ClearDepthBuffer   |
                PlatformBackendRendererHints_ClearStencilBuffer |
                PlatformBackendRendererHints_PollEvents
        };

        enum PlatformBackendRendererTextureFormat_ : int
        {
            PlatformBackendRendererTextureFormat_ALPHA,
            PlatformBackendRendererTextureFormat_RGB,
            PlatformBackendRendererTextureFormat_RGBA,
        };

        enum PlatformBackendRendererTextureWrap_ : int
        {
            PlatformBackendRendererTextureWrap_Repeat,
            PlatformBackendRendererTextureWrap_Mirrored,
            PlatformBackendRendererTextureWrap_ClampToEdge,
            PlatformBackendRendererTextureWrap_ClampToBorder
        };

        enum PlatformBackendRendererTextureMinFilter_ : int
        {
            PlatformBackendRendererTextureMinFilter_Linear,
            PlatformBackendRendererTextureMinFilter_Nearest,
            PlatformBackendRendererTextureMinFilter_NearestMipMapLinear,
            PlatformBackendRendererTextureMinFilter_NearestMipMapNearest,
            PlatformBackendRendererTextureMinFilter_LinearMipMapLinear,
            PlatformBackendRendererTextureMinFilter_LinearMipMapNearest,
        };

        enum PlatformBackendRendererTextureMaxFilter_ : int
        {
            PlatformBackendRendererTextureMaxFilter_Linear,
            PlatformBackendRendererTextureMaxFilter_Nearest
        };

        enum PlatformBackendRendererShaderType_ : int
        {
            PlatformBackendRendererShaderType_Vertex,
            PlatformBackendRendererShaderType_Fragment
        };

        typedef int PlatformBackendContextWindowHints;
        typedef int PlatformBackendRendererHints;
        typedef int PlatformBackendRendererTextureFormat;
        typedef int PlatformBackendRendererTextureWrap;
        typedef int PlatformBackendRendererTextureMinFilter;
        typedef int PlatformBackendRendererTextureMaxFilter;
        typedef int PlatformBackendRendererShaderType;

        template<typename Type>
        class PlatformBackendRendererAllocator;

        // enities
        struct PlatformBackendRendererTexture final
        {
            PlatformBackendRendererTexture(
                const unsigned int&                            _Ptr,
                const int&                                     _Width,
                const int&                                     _Height,
                const PlatformBackendRendererTextureFormat&    _Format,
                const PlatformBackendRendererTextureWrap&      _Wrap,
                const PlatformBackendRendererTextureMinFilter& _MinFilter,
                const PlatformBackendRendererTextureMaxFilter& _MaxFilter) : 
                Width(_Width),
                Height(_Height),
                Ptr(_Ptr),
                Format(_Format),
                Wrap(_Wrap),
                MinFilter(_MinFilter),
                MaxFilter(_MaxFilter){}

            // info
            const unsigned int                    Ptr       {+0};
            const int                             Width     {-1};
            const int                             Height    {-1};
            const PlatformBackendRendererTextureFormat    Format    {PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA};
            const PlatformBackendRendererTextureWrap      Wrap      {PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat};
            const PlatformBackendRendererTextureMinFilter MinFilter {PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear};
            const PlatformBackendRendererTextureMaxFilter MaxFilter {PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear};
        };

        struct PlatformBackendRendererShader final
        {
            PlatformBackendRendererShader(const unsigned int& _Ptr) : Ptr(_Ptr){}

            // info
            const unsigned int Ptr {0};
        };

        class PlatformBackendRenderer
        {
        public:
            PlatformBackendRenderer();
            virtual ~PlatformBackendRenderer();

            // getters
            void* get_context() const;

            // virtual API
            virtual bool awake(
                const char*                       _Name        = "DefaultPlatformWindow",
                void*                             _Share       = nullptr,
                PlatformBackendContextWindowHints _WindowHints = PlatformBackendRendererContextHints_::PlatformBackendContextWindowHints_Default);

            virtual void frame_start(const PlatformBackendRendererHints& _RendererHints = PlatformBackendRendererHints_Default);

            virtual void frame_update();
            virtual void frame_render();
            virtual void frame_finish();
            virtual void finish();
            virtual void quit();
            virtual bool is_closed() const;
            virtual void close();

            // images virtual API
            virtual PlatformBackendRendererTexture* construct_image(
                const unsigned char*                           _RawBuffer,
                const int&                                     _Width,
                const int&                                     _Height,
                const PlatformBackendRendererTextureFormat&    _Format    = PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA,
                const PlatformBackendRendererTextureWrap&      _Wrap      = PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat,
                const PlatformBackendRendererTextureMinFilter& _MinFilter = PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear, 
                const PlatformBackendRendererTextureMaxFilter& _MaxFilter = PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear
            );

            virtual PlatformBackendRendererTexture* construct_image(
                const char*                                    _FilePath,
                const PlatformBackendRendererTextureFormat&    _Format    = PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA,
                const PlatformBackendRendererTextureWrap&      _Wrap      = PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat,
                const PlatformBackendRendererTextureMinFilter& _MinFilter = PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear, 
                const PlatformBackendRendererTextureMaxFilter& _MaxFilter = PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear
            );

            virtual void destroy_image(PlatformBackendRendererTexture* _Image);

            // shaders virtual API
            virtual PlatformBackendRendererShader* construct_shader(
                const std::vector<std::pair<std::string, PlatformBackendRendererShaderType>>& ShaderInfos);
            
            virtual PlatformBackendRendererShader* construct_shader(
                const std::vector<std::filesystem::path>& _ShaderFilesPaths);

            virtual void destroy_shader(PlatformBackendRendererShader* _Shader);

            virtual void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const bool& _Value);
            virtual void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const int& _Value);
            virtual void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const float& _Value);
            virtual void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::vec2& _Value);
            virtual void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::vec3& _Value);
            virtual void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::vec4& _Value);
            virtual void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::mat2& _Value);
            virtual void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::mat3& _Value);
            virtual void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::mat4& _Value);
            
        protected:

            PlatformBackendRendererAllocator<PlatformBackendRendererTexture>* m_Textures{nullptr};
            PlatformBackendRendererAllocator<PlatformBackendRendererShader>*  m_Shaders {nullptr};
            void*                                                             m_Context {nullptr};
        };
    }
}