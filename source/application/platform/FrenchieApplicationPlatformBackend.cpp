#include <FrenchieApplicationPlatformBackend.hpp>

using namespace Frenchie::Application;

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// STB
#include <stb_image.h>
#include <stb_image_write.h>

// STL
#include <iostream>

namespace Frenchie
{
    namespace Application
    {
        // Callbacks
        void OpenGLPlatformBackendOnWindowResize(GLFWwindow* _Window, int _Width, int _Height)
        {
            (void)_Window;
            glViewport(0, 0, _Width, _Height);
        }

        void OpenGLPlatformBackendOnWindowMaximizedCallback(GLFWwindow* _Window, int _Maximized)
        {
            int width  = 0;
            int height = 0;
            glfwGetWindowSize(_Window, &width, &height);
            glViewport(0, 0, width, height);
        }

        void OpenGLPlatformBackendPushStatusMessage(char*  _Message, char** _ImageLoadStatusBuffer)
        {
            if(_ImageLoadStatusBuffer == nullptr)
                return;

            if(strlen(*_ImageLoadStatusBuffer) < strlen(_Message))
            {
                *_ImageLoadStatusBuffer = 
                    (char*)realloc(*_ImageLoadStatusBuffer, 2 * strlen(_Message) * sizeof(char));
            }

            strcpy(*_ImageLoadStatusBuffer, _Message);
        }

        // template<typename Head>
        // inline Head __max__(const Head& _A, const Head& _B)
        // {
        //     return _A > _B ? _A : _B;
        // }

        // template<typename Head, typename ... Args>
        // Head __max__(const Head& _A, const Head& _B, Args... _Args)
        // {
        //     return __max__(__max__(_A, _B), _Args...);
        // }

        // template<typename Head>
        // inline  Head __min__(const Head& _A, const Head&)
        // {
        //     return _A < _B ? _A : _B;
        // }

        // template<typename Head, typename ... Args>
        // Head __min__(const Head& _A, const Head& _B, Args... _Args)
        // {
        //     return __min__(__min__( _A, _B ), _Args...);
        // }

        template<typename Type>
        class PlatformBackendAllocator final
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

                    std::cout << "~PlatformBackendAllocatorMemoryChunk() \n";
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
            PlatformBackendAllocator(uintptr_t _ChunkSize = 16) : 
                m_ChunkSize(std::max<uintptr_t>(_ChunkSize, 16)), 
                m_Head(new PlatformBackendAllocatorMemoryChunk(sizeof(Type), m_ChunkSize)), 
                m_Tail(m_Head){}

            ~PlatformBackendAllocator()
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
    }
}

PlatformBackendInstance::PlatformBackendInstance() :
    m_Textures(new PlatformBackendAllocator<PlatformBackendRendererTexture>(32)),
    m_Shaders(new PlatformBackendAllocator<PlatformBackendRendererShader>(32))
{
}

PlatformBackendInstance::~PlatformBackendInstance()
{
    delete m_Textures;
    delete m_Shaders;
}

void* PlatformBackendInstance::get_context() const
{
    return m_Context;
}

const char* PlatformBackendInstance::get_context_window_name() const
{
    return glfwGetWindowTitle(reinterpret_cast<GLFWwindow*>(m_Context));
}

void PlatformBackendInstance::set_context_window_name(const char* _Value)
{
    glfwSetWindowTitle(reinterpret_cast<GLFWwindow*>(m_Context), _Value);
}

void PlatformBackendInstance::set_context_buffer_swap_interval(const int& _Milliseconds)
{
    glfwSwapInterval(_Milliseconds);
}

bool PlatformBackendInstance::awake(
    const char*                       _PlatformBackendContextWindowName,
    const int&                        _PlatformBackendContextWindowWidth,
    const int&                        _PlatformBackendContextWindowHeight,
    void*                             _PlatformBackendContextWindowShare,
    PlatformBackendContextWindowHints _PlatformBackendContextWindowHints)
{
    // initialization
    if(glfwInit() == GLFW_FALSE)
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if(_PlatformBackendContextWindowHints & PlatformBackendContextWindowHints_::PlatformBackendContextWindowHint_Visible)
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    if(_PlatformBackendContextWindowHints & PlatformBackendContextWindowHints_::PlatformBackendContextWindowHint_Decorated)
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    if(_PlatformBackendContextWindowHints & PlatformBackendContextWindowHints_::PlatformBackendContextWindowHint_Resizable)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if(_PlatformBackendContextWindowHints & PlatformBackendContextWindowHints_::PlatformBackendContextWindowHint_Iconified)
        glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE);

    if(_PlatformBackendContextWindowHints & PlatformBackendContextWindowHints_::PlatformBackendContextWindowHint_Focused)
        glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    // create context
    m_Context = glfwCreateWindow(
        _PlatformBackendContextWindowWidth,
        _PlatformBackendContextWindowHeight,
        _PlatformBackendContextWindowName,
        nullptr,
        reinterpret_cast<GLFWwindow*>(_PlatformBackendContextWindowShare));

    if(m_Context == nullptr)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_Context));

    // setup callbacks
    glfwSetWindowSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), &OpenGLPlatformBackendOnWindowResize);
    glfwSetFramebufferSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), &OpenGLPlatformBackendOnWindowResize);
    glfwSetWindowMaximizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), &OpenGLPlatformBackendOnWindowMaximizedCallback);

    // load OpenGL interface using GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    // call window maximize callback if the Window has been maximized
    OpenGLPlatformBackendOnWindowMaximizedCallback(
        reinterpret_cast<GLFWwindow*>(m_Context),
        glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_Context), GLFW_MAXIMIZED));

    return true;
}

void PlatformBackendInstance::frame_start(const PlatformBackendRendererHints& _PlatformBackendRendererHints)
{
    if((_PlatformBackendRendererHints & PlatformBackendRendererHints_::PlatformBackendRendererHints_ClearColorBuffer))
        glClear(GL_COLOR_BUFFER_BIT);
    
    if((_PlatformBackendRendererHints & PlatformBackendRendererHints_::PlatformBackendRendererHints_ClearDepthBuffer))
        glClear(GL_DEPTH_BUFFER_BIT);
    
    if((_PlatformBackendRendererHints & PlatformBackendRendererHints_::PlatformBackendRendererHints_ClearStencilBuffer))
        glClear(GL_STENCIL_BUFFER_BIT);

    if((_PlatformBackendRendererHints & PlatformBackendRendererHints_::PlatformBackendRendererHints_PollEvents))
        glfwPollEvents();
}

void PlatformBackendInstance::frame_update()
{
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Context), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
}

void PlatformBackendInstance::frame_render()
{
    // TODO: execute rendering commands here
}

void PlatformBackendInstance::frame_finish()
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_Context));
}

void PlatformBackendInstance::finish()
{
}

void PlatformBackendInstance::quit()
{
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwTerminate();
    m_Context = nullptr;
}

bool PlatformBackendInstance::is_closed() const
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context));
}

void PlatformBackendInstance::close()
{
    glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context), GL_TRUE);
}

void PlatformBackendInstance::set_context_window_maximized()
{
    glfwMaximizeWindow(reinterpret_cast<GLFWwindow*>(m_Context));
}

PlatformBackendRendererTexture* PlatformBackendInstance::construct_image(
    const unsigned char*                   _RawBuffer,
    const int&                             _Width,
    const int&                             _Height,
    const PlatformBackendRendererTextureFormat&    _Format,
    const PlatformBackendRendererTextureWrap&      _Wrap,
    const PlatformBackendRendererTextureMinFilter& _MinFilter, 
    const PlatformBackendRendererTextureMaxFilter& _MaxFilter
)
{
    if(_RawBuffer == nullptr)
    {
        return nullptr;
    }

    // register image within platform specific low level grphics API
    unsigned int sampler;
    glGenTextures(1, &sampler);
    glBindTexture(GL_TEXTURE_2D, sampler); 
    
    // set the texture wrapping parameters
    if((_Wrap & PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else if((_Wrap & PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Mirrored))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    }
    else if((_Wrap & PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_ClampToEdge))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else if((_Wrap & PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_ClampToBorder))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }
    
    // set minifying filter
    if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Nearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Nearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_LinearMipMapLinear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_LinearMipMapNearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_NearestMipMapLinear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_NearestMipMapNearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
    // set magnifying filter
    if((_MaxFilter & PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    else if((_MaxFilter & PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Nearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // set format
    if((_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_ALPHA))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _Width, _Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _RawBuffer);
    else if((_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGB))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _Width, _Height, 0, GL_RGB, GL_UNSIGNED_BYTE, _RawBuffer);
    else if((_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _RawBuffer);

    // gemerate MimMaps
    if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_LinearMipMapLinear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_LinearMipMapNearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_NearestMipMapLinear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_NearestMipMapNearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sampler);

    return m_Textures->construct(sampler, _Width, _Height, _Format, _Wrap, _MinFilter, _MaxFilter);
}

PlatformBackendRendererTexture* PlatformBackendInstance::construct_image(
    const char*                            _FilePath,
    const PlatformBackendRendererTextureFormat&    _Format,
    const PlatformBackendRendererTextureWrap&      _Wrap,
    const PlatformBackendRendererTextureMinFilter& _MinFilter, 
    const PlatformBackendRendererTextureMaxFilter& _MaxFilter,
    char**                                 _ImageLoadStatusBuffer
)
{
    // auxiliary lambdas
    auto formatToRequestdChannels = [](PlatformBackendRendererTextureFormat _Format)->int
    {
        if(_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA)  return 4;
        if(_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGB)   return 3;
        if(_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_ALPHA) return 1;

        return 0; // extract everything by default
    };

    int width   {0};
    int height  {0};
    int channels{0};

    stbi_uc* buffer =
        stbi_load(_FilePath, &width, &height, &channels, formatToRequestdChannels(_Format));

    if(buffer == nullptr)
    {
        OpenGLPlatformBackendPushStatusMessage(
            "Could not load message using stbi_load(har const *filename, int *x, int *y, int *comp, int req_comp)",
            _ImageLoadStatusBuffer);
        return nullptr;
    }

    // construct image
    PlatformBackendRendererTexture* image =
        construct_image(buffer, width, height, _Format, _Wrap, _MinFilter, _MaxFilter);

    // clear raw image buffer
    stbi_image_free(buffer);

    return image;
}

void PlatformBackendInstance::destroy_image(PlatformBackendRendererTexture* _Image)
{
    if(_Image != nullptr)
        m_Textures->destroy(_Image);
}

// PlatformBackendShader PlatformBackend::load_shader(
//     const char*                                     _ShaderPath,
//     const PlatformBackendShaderType& _ShaderType,
//     char**                                          _CompileStatusBuffer,
//     const int&                                      _CompileStatusBufferSize)
// {
//     FILE *fptr = fopen(_ShaderPath, "rb");
    
//     if (fptr == NULL) 
//     {
//         return PlatformBackendShader();
//     }

//     fseek(fptr, 0, SEEK_END);
//     long file_size = ftell(fptr);
//     fseek(fptr, 0, SEEK_SET);

//     char *buffer = (char *)malloc(file_size + 1); // +1 for null terminator
//     if (buffer == NULL) 
//     {
//         // Handle error: memory allocation failed
//         fclose(fptr);
//         return PlatformBackendShader();
//     }

//     unsigned int bytes_read = fread(buffer, 1, file_size, fptr);
//     if (bytes_read != file_size) {
//         // Handle error: not all bytes were read
//         fprintf(stderr, "Error reading file, expected %ld bytes, read %zu\n", file_size, bytes_read);
//         free(buffer);
//         fclose(fptr);
//         return PlatformBackendShader();
//     }
//     buffer[file_size] = '\0'; // Null-terminate the buffer

//     fclose(fptr);
//     free(buffer);

//     return PlatformBackendShader();
// }