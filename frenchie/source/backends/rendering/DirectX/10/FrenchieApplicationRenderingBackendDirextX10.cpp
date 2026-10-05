// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// D3D9
#include <d3d10.h>
#include <d3dcompiler.h>
#include <d3d10effect.h> // Содержит описание ID3D10Effect

#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include <iostream>


using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationRenderingBackendDirectX10 : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendDirectX10(){}
            virtual ~ApplicationRenderingBackendDirectX10(){}

            HWND                   hWnd;

            // device and swap chain
            ID3D10Device*          m_Device       {nullptr};
            IDXGISwapChain*        m_SwapChain    {nullptr};

            ID3D10VertexShader*    m_VertexShader {nullptr};
            ID3D10PixelShader*     m_PixelShader  {nullptr};
            ID3D10InputLayout*     m_VertexLayout {nullptr};
            ID3D10Buffer*          m_ProjectionBuffer {nullptr};

            ID3D10Buffer*          m_VertexBuffer    {nullptr};
            int                    m_VertexBufferSize{0};

            ID3D10Buffer*          m_IndexBuffer     {nullptr};
            int                    m_IndexBufferSize {0};

            // MSAA render target
            ID3D10Texture2D*        m_MSAARenderTarget        {nullptr};
            ID3D10RenderTargetView* m_MSAARenderTargetView    {nullptr};

            // depth stencil render target
            ID3D10Texture2D*         m_DepthStencilTarget     {nullptr};
            ID3D10DepthStencilView*  m_DepthStencilTargetView {nullptr};

            ID3D10BlendState*        m_AlphaBlendState        {nullptr};
            ID3D10DepthStencilState* m_DepthStencilState      {nullptr};
            ID3D10SamplerState*      m_DefaultSamplerState    {nullptr};


            gs_color                  m_ClearColor;
            std::optional<gs_2d_boxf> m_Viewport;
        };

        struct ApplicationRenderingBackendDirectXCBuffer
        {
            float Projection[16]{};
        };

        bool d3d10_create_device_and_swap_chain(ApplicationRenderingBackendDirectX10* DirectX9, const float& width, const float& height)
        {
            HRESULT HResult;

            // device swap chain description
            DXGI_SWAP_CHAIN_DESC sd;
            ZeroMemory(&sd, sizeof(sd));
            sd.BufferCount                        = 2;                               // Number of back buffers
            sd.BufferDesc.Width                   = width;                           // Resolution width
            sd.BufferDesc.Height                  = height;                          // Resolution height
            sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;      // Pixel format
            sd.BufferDesc.RefreshRate.Numerator   = 60;                              // Refresh rate
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Usage of the buffer
            sd.OutputWindow                       = DirectX9->hWnd;                  // Target window handle
            sd.SampleDesc.Count                   = 1;                               // Multi-sampling (1 = no MSAA)
            sd.SampleDesc.Quality                 = 0;                               // quality
            sd.Windowed                           = TRUE;                            // Windowed or fullscreen
            sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

            // create swap chain
            UINT createDeviceFlags = 0;
    #if defined(DEBUG) || defined(_DEBUG)  
        createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
    #endif

            // 3. Create the device and swap chain
            if (FAILED(HResult = D3D10CreateDeviceAndSwapChain(
                NULL,                          // Default adapter / video card
                D3D10_DRIVER_TYPE_HARDWARE,    // Driver type (Hardware acceleration)
                NULL,                          // Software rasterizer handle (NULL if not using software)
                createDeviceFlags,             // Creation flags
                D3D10_SDK_VERSION,             // SDK version
                &sd,                           // Swap chain description pointer
                &DirectX9->m_SwapChain,        // Output: Swap chain pointer
                &DirectX9->m_Device            // Output: D3D10 device pointer
            )))
            {
                std::cout << "could not create D3D10 device and swap chain\n";
                return false;
            }

            return true;
        }

        bool d3d10_create_and_compile_shaders(ApplicationRenderingBackendDirectX10* DirectX9)
        {
            HRESULT HResult;

            const char* shaderProgram =
R"(
Texture2D Texture;

cbuffer ApplicationRenderingBackendDirectXCBuffer : register(b0)
{
    column_major float4x4 Projection;
};

SamplerState linearSampler
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 UV       : TEXCOORD;
    float4 Color    : COLOR;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD;
    float4 Color    : COLOR;
};

PS_INPUT vertex_shader(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = mul(Projection, input.Position);
    output.Color    = input.Color;
    output.UV       = input.UV;
    return output;  
}

float4 pixel_shader(PS_INPUT input) : SV_Target
{
    return input.Color; 
    //return Texture.Sample(linearSampler, input.UV) * input.Color; 
}
)";

            // try compile shaders
            ID3DBlob* pErrors           = nullptr;
            ID3DBlob* pVertexShaderBlob = nullptr;
            ID3DBlob* pPixelShaderBlob  = nullptr;


            UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
        #if defined(DEBUG) || defined(_DEBUG)
            compileFlags |= D3DCOMPILE_DEBUG;
        #endif

            // try compile vertex shader
            if (FAILED(HResult = D3DCompile(
                shaderProgram,                     // Pointer to the string data
                std::strlen(shaderProgram),        // Size of the string data
                nullptr,                           // Optional source name for debug/errors
                nullptr,                           // Optional defines
                D3D_COMPILE_STANDARD_FILE_INCLUDE, // Optional include handler
                "vertex_shader",                   // Entrypoint (NULL for full effects)
                "vs_4_0",                          // Target profile for D3D11 effects
                D3DCOMPILE_ENABLE_STRICTNESS,      // Compile flags
                0,                                 // Effect flags
                &pVertexShaderBlob,                // Output compiled binary blob
                &pErrors                           // Output compiler error messages
                )))
            {
                if (pErrors)
                {
                    char* compileErrors = static_cast<char*>(pErrors->GetBufferPointer());
                    std::cerr << "Shader Compilation Error:\n" << compileErrors << std::endl;
                    pErrors->Release();
                }
                else
                {
                    std::cerr << "Shader compilation failed, but no error log was generated ...\n";
                }

                if(pVertexShaderBlob)
                    pVertexShaderBlob->Release();

                return false;
            }

            // try compile pixel shader
            if (FAILED(HResult = D3DCompile(
                shaderProgram,                     // Pointer to the string data
                std::strlen(shaderProgram),        // Size of the string data
                nullptr,                           // Optional source name for debug/errors
                nullptr,                           // Optional defines
                D3D_COMPILE_STANDARD_FILE_INCLUDE, // Optional include handler
                "pixel_shader",                    // Entrypoint (NULL for full effects)
                "ps_4_0",                          // Target profile for D3D11 effects
                D3DCOMPILE_ENABLE_STRICTNESS,      // Compile flags
                0,                                 // Effect flags
                &pPixelShaderBlob,                // Output compiled binary blob
                &pErrors                           // Output compiler error messages
                )))
            {
                if (pErrors)
                {
                    char* compileErrors = static_cast<char*>(pErrors->GetBufferPointer());
                    std::cerr << "Shader Compilation Error:\n" << compileErrors << std::endl;
                    pErrors->Release();
                }
                else
                {
                    std::cerr << "Shader compilation failed, but no error log was generated ...\n";
                }

                if(pPixelShaderBlob)
                    pPixelShaderBlob->Release();

                return false;
            }

            // create mesh vertex layout
            D3D10_INPUT_ELEMENT_DESC inputLayout[] =
            {
                {
                    "POSITION",
                    0,
                    DXGI_FORMAT_R32G32B32_FLOAT,
                    0,
                    static_cast<UINT>(offsetof(ApplicationRenderingBackendMeshVertex, Position)),
                    D3D10_INPUT_PER_VERTEX_DATA,
                    0
                },
                {
                    "TEXCOORD",
                    0,
                    DXGI_FORMAT_R32G32_FLOAT,
                    0,
                    static_cast<UINT>(offsetof(ApplicationRenderingBackendMeshVertex, UV)),
                    D3D10_INPUT_PER_VERTEX_DATA,
                    0
                },
                {
                    "COLOR",
                    0,
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    0,
                    static_cast<UINT>(offsetof(ApplicationRenderingBackendMeshVertex, Color)),
                    D3D10_INPUT_PER_VERTEX_DATA,
                    0
                }
            };

            if(FAILED(HResult = DirectX9->m_Device->CreateInputLayout(
                inputLayout,
                ARRAYSIZE(inputLayout),
                pVertexShaderBlob->GetBufferPointer(), // Указатель на скомпилированный шейдер
                pVertexShaderBlob->GetBufferSize(),    // Размер скомпилированного шейдера
                &DirectX9->m_VertexLayout
            )))
            {
                std::cout << "could not create vertex layout " << HResult << "\n";

                if(pVertexShaderBlob)
                    pVertexShaderBlob->Release();

                if(pPixelShaderBlob)
                    pPixelShaderBlob->Release();

                return false;
            }

            // create vertex shader object
            if(FAILED(HResult = DirectX9->m_Device->CreateVertexShader(
                pVertexShaderBlob->GetBufferPointer(),
                pVertexShaderBlob->GetBufferSize(),
                &DirectX9->m_VertexShader)))
            {
                std::cout << "could not create vertex shader \n";

                if(pVertexShaderBlob)
                    pVertexShaderBlob->Release();

                if(pPixelShaderBlob)
                    pPixelShaderBlob->Release();

                return false;
            }
            
            // create pixel shader object
            if(FAILED(HResult = DirectX9->m_Device->CreatePixelShader(
                pPixelShaderBlob->GetBufferPointer(),
                pPixelShaderBlob->GetBufferSize(),
                &DirectX9->m_PixelShader)))
            {
                std::cout << "could not create pixel shader \n";

                if(pVertexShaderBlob)
                    pVertexShaderBlob->Release();

                if(pPixelShaderBlob)
                    pPixelShaderBlob->Release();

                return false;
            }

            D3D10_BUFFER_DESC projectionBufferDescription = {};
            projectionBufferDescription.Usage          = D3D10_USAGE_DYNAMIC;
            projectionBufferDescription.ByteWidth      = sizeof(ApplicationRenderingBackendDirectXCBuffer);
            projectionBufferDescription.BindFlags      = D3D10_BIND_CONSTANT_BUFFER;
            projectionBufferDescription.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

            if(FAILED(HResult = DirectX9->m_Device->CreateBuffer(
                &projectionBufferDescription,
                nullptr,
                &DirectX9->m_ProjectionBuffer)))
            {
                std::cout << "could not create projection constant buffer " << HResult << "\n";

                if(pVertexShaderBlob)
                    pVertexShaderBlob->Release();

                if(pPixelShaderBlob)
                    pPixelShaderBlob->Release();

                return false;
            }

            if(pVertexShaderBlob)
                pVertexShaderBlob->Release();

            if(pPixelShaderBlob)
                pPixelShaderBlob->Release();

            return true;
        }

        bool d3d10_create_render_target_and_depth_view(ApplicationRenderingBackendDirectX10* _DirectX, const float& _Width, const float& _Height)
        {
            if(_DirectX == nullptr) return false;

            HRESULT HResult;

            // 1. Проверяем поддержку 4x MSAA
            UINT sampleCount   = 4;
            UINT qualityLevels = 0;
            _DirectX->m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &qualityLevels);
            
            if (qualityLevels == 0)
            {
                // MSAA не поддерживается для данного формата
            }

            // create MSAA texure
            D3D10_TEXTURE2D_DESC msaaTextureDescription;
            msaaTextureDescription.Width              = _Width;
            msaaTextureDescription.Height             = _Height;
            msaaTextureDescription.MipLevels          = 1;
            msaaTextureDescription.ArraySize          = 1;
            msaaTextureDescription.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
            msaaTextureDescription.SampleDesc.Count   = sampleCount;
            msaaTextureDescription.SampleDesc.Quality = qualityLevels - 1;
            msaaTextureDescription.Usage              = D3D10_USAGE_DEFAULT;
            msaaTextureDescription.BindFlags          = D3D10_BIND_RENDER_TARGET;
            msaaTextureDescription.MiscFlags          = 0;
            msaaTextureDescription.CPUAccessFlags     = 0;

            if(FAILED(HResult = _DirectX->m_Device->CreateTexture2D(
                &msaaTextureDescription,
                nullptr,
                &_DirectX->m_MSAARenderTarget)))
            {
                std::cout << "could not create MSAA texture \n";
                return false;
            }

            // create render target view with MSAA texture
            if (FAILED(HResult = _DirectX->m_Device->CreateRenderTargetView(
                _DirectX->m_MSAARenderTarget,
                NULL,
                &_DirectX->m_MSAARenderTargetView)))
            {
                std::cout << "could not create rendering target view with MSAA texture \n";
                return false;
            }

            // RTV depth/stencil texture
            D3D10_TEXTURE2D_DESC depthTextureDescription;
            depthTextureDescription.Width              = _Width;
            depthTextureDescription.Height             = _Height;
            depthTextureDescription.MipLevels          = 1;
            depthTextureDescription.ArraySize          = 1;
            depthTextureDescription.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthTextureDescription.SampleDesc.Count   = 4;
            depthTextureDescription.SampleDesc.Quality = 0;
            depthTextureDescription.Usage              = D3D10_USAGE_DEFAULT;
            depthTextureDescription.BindFlags          = D3D10_BIND_DEPTH_STENCIL;
            depthTextureDescription.CPUAccessFlags     = 0;
            depthTextureDescription.MiscFlags          = 0;

            if(FAILED(HResult = _DirectX->m_Device->CreateTexture2D(
                &depthTextureDescription,
                nullptr,
                &_DirectX->m_DepthStencilTarget)))
            {
                std::cout << "could not create depth buffer texture \n";
                return false;
            }

            // create depth/stencil view
            if(FAILED(HResult = _DirectX->m_Device->CreateDepthStencilView(
                _DirectX->m_DepthStencilTarget,
                nullptr,
                &_DirectX->m_DepthStencilTargetView)))
            {
                std::cout << "could not create depth/stencil view with corresponding buffer texture \n";
                return false;
            }

            return true;
        }
        
        bool d3d10_create_viewport(ApplicationRenderingBackendDirectX10* _DirectX, const float& _Width, const float& _Height)
        {
            if(_DirectX == nullptr) return false;

            D3D10_VIEWPORT viewport;
            viewport.TopLeftX = 0;          // x coordinate
            viewport.TopLeftY = 0;          // y coordinate
            viewport.Width    = _Width;     // width
            viewport.Height   = _Height;    // height
            viewport.MinDepth = 0.0f;       // min depth
            viewport.MaxDepth = 1.0f;       // max depth

            // attach viewport to swap chain
            _DirectX->m_Device->RSSetViewports(1, &viewport);
            return true;

        }

        bool d3d10_enable_blending(ApplicationRenderingBackendDirectX10* DirectX9)
        {
            if(DirectX9 == nullptr) return false;

            // describe the blend state for standard transparency
            D3D10_BLEND_DESC blendDesc;
            ZeroMemory(&blendDesc, sizeof(D3D10_BLEND_DESC));

            blendDesc.BlendEnable[0]           = TRUE;                        // Enable blending for render target 0
            blendDesc.SrcBlend                 = D3D10_BLEND_SRC_ALPHA;       // Source factor: incoming alpha
            blendDesc.DestBlend                = D3D10_BLEND_INV_SRC_ALPHA;   // Destination factor: 1.0 - incoming alpha
            blendDesc.BlendOp                  = D3D10_BLEND_OP_ADD;          // Combine equation: add them up
            blendDesc.SrcBlendAlpha            = D3D10_BLEND_ONE;             // Alpha source factor
            blendDesc.DestBlendAlpha           = D3D10_BLEND_ZERO;            // Alpha destination factor
            blendDesc.BlendOpAlpha             = D3D10_BLEND_OP_ADD;          // Alpha combine equation
            blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

            // create the blend state object
            HRESULT HResult;

            if(FAILED(HResult = DirectX9->m_Device->CreateBlendState(&blendDesc, &DirectX9->m_AlphaBlendState)))
            {
                std::cout << "could not create alpha blend state \n";
                return false;
            }

            return true;
        }

        bool d3d10_depth_testing(ApplicationRenderingBackendDirectX10* DirectX9)
        {
            if(DirectX9 == nullptr) return false;

            D3D10_DEPTH_STENCIL_DESC depthstencildesc = {};
            depthstencildesc.DepthEnable    = TRUE;
            depthstencildesc.StencilEnable  = TRUE; 
            depthstencildesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
            depthstencildesc.DepthFunc      = D3D10_COMPARISON_LESS;

            HRESULT HResult;

            if(FAILED(HResult = DirectX9->m_Device->CreateDepthStencilState(&depthstencildesc, &DirectX9->m_DepthStencilState)))
            {
                std::cout << "could not create depth/stencil state \n";
                return false;
            }

            return true;
        }

        bool d3d10_default_sampler(ApplicationRenderingBackendDirectX10* DirectX9)
        {
            if(DirectX9 == nullptr) return false;

            // fill sampler description
            D3D10_SAMPLER_DESC sampDesc;
            ZeroMemory(&sampDesc, sizeof(sampDesc));
            sampDesc.Filter         = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
            sampDesc.AddressU       = D3D10_TEXTURE_ADDRESS_WRAP;
            sampDesc.AddressV       = D3D10_TEXTURE_ADDRESS_WRAP;
            sampDesc.AddressW       = D3D10_TEXTURE_ADDRESS_WRAP;
            sampDesc.ComparisonFunc = D3D10_COMPARISON_NEVER;
            sampDesc.MinLOD         = 0;
            sampDesc.MaxLOD         = D3D10_FLOAT32_MAX;

            // create sampler
            HRESULT HResult;

            if(FAILED(HResult = DirectX9->m_Device->CreateSamplerState(&sampDesc, &DirectX9->m_DefaultSamplerState)))
            {
                std::cout << "could not create default sampler \n";
                return false;   
            }

            return true;
        }

        // D3DMATRIX gs_convert_transform_from_opengl_to_directx(const gs_mat4f& _Matrix)
        // {
        //     D3DMATRIX result;

        //     for (int i = 0; i < _Matrix.columns(); i++)
        //     {
        //         for (int j = 0; j < _Matrix.rows(); j++)
        //         {
        //             result.m[i][j] = _Matrix[i][j];
        //         }
        //     }

        //     return result;
        // }

        typedef Frenchie::Application::ApplicationRenderingBackendMeshVertex      CUSTOMVERTEX;
        typedef Frenchie::Application::ApplicationRenderingBackendMeshVertexIndex CUSTOMINDEX;
    }
}

bool ApplicationRenderingBackend::awake(const std::any& _Stuff)
{
    HWND hWnd;

    try
    {
        hWnd = std::any_cast<HWND>(_Stuff);
    }
    catch(...)
    {
        return false;
    }
    
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 =
        std::dynamic_pointer_cast<ApplicationRenderingBackendDirectX10>(m_Api = std::make_shared<ApplicationRenderingBackendDirectX10>());

    DirectX9->hWnd = hWnd;

    RECT rc;
    GetClientRect(DirectX9->hWnd, &rc);
    UINT width  = rc.right  - rc.left;
    UINT height = rc.bottom - rc.top;

    if(!d3d10_create_device_and_swap_chain(DirectX9.get(), width, height)) 
        return false;

    if(!d3d10_create_render_target_and_depth_view(DirectX9.get(), width, height))
        return false;

    if(!d3d10_create_viewport(DirectX9.get(), width, height))
        return false;

    if(!d3d10_create_and_compile_shaders(DirectX9.get()))
        return false;

    if(!d3d10_enable_blending(DirectX9.get()))
        return false;

    if(!d3d10_default_sampler(DirectX9.get()))
        return false;

    return true;
}

void ApplicationRenderingBackend::begin_render(ApplicationRenderingBackendRenderingTarget* _Target)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();
    
    if(DirectX9 == nullptr)
        return;

    DirectX9->m_Device->OMSetRenderTargets(1, &DirectX9->m_MSAARenderTargetView, DirectX9->m_DepthStencilTargetView);

    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    DirectX9->m_Device->OMSetBlendState(DirectX9->m_AlphaBlendState, blendFactor, 0xffffffff);
    DirectX9->m_Device->OMSetDepthStencilState(DirectX9->m_DepthStencilState, 1);

    FLOAT clearcolor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
    DirectX9->m_Device->ClearRenderTargetView(DirectX9->m_MSAARenderTargetView, clearcolor);
    DirectX9->m_Device->ClearDepthStencilView(DirectX9->m_DepthStencilTargetView, D3D10_CLEAR_DEPTH, 1.0f, 0);
    DirectX9->m_Device->IASetInputLayout(DirectX9->m_VertexLayout);

    DirectX9->m_Device->VSSetShader(DirectX9->m_VertexShader);
    DirectX9->m_Device->PSSetShader(DirectX9->m_PixelShader);
    DirectX9->m_Device->PSSetSamplers(0, 1, &DirectX9->m_DefaultSamplerState);
}

void ApplicationRenderingBackend::end_render()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();

    if(DirectX9 == nullptr)
        return;

    ID3D10Texture2D* swapChainFrameBuffer = nullptr;
    DirectX9->m_SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&swapChainFrameBuffer);
    DirectX9->m_Device->ResolveSubresource(swapChainFrameBuffer, 0, DirectX9->m_MSAARenderTarget, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
    DirectX9->m_SwapChain->Present(0, 0);

    swapChainFrameBuffer->Release();
}

void ApplicationRenderingBackend::quit()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();

    if(DirectX9 == nullptr)
        return;
}

ApplicationRenderingBackendTexture ApplicationRenderingBackend::construct_texture(
    const unsigned char*                               _RawBuffer,
    const int&                                         _Width,
    const int&                                         _Height,
    const ApplicationRenderingBackendTextureFormat&    _Format,
    const ApplicationRenderingBackendTextureWrapMode&  _Wrap,
    const ApplicationRenderingBackendTextureMinFilter& _MinFilter,
    const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter,
    const int&                                         _Attributes)
{
    (void)_RawBuffer;
    (void)_Width;
    (void)_Height;
    (void)_Format;
    (void)_Wrap;
    (void)_MinFilter;
    (void)_MaxFilter;
    (void)_Attributes;

    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();

    if(DirectX9 == nullptr)
        return ApplicationRenderingBackendTexture();

    std::shared_ptr<unsigned char> image = std::shared_ptr<unsigned char>(new unsigned char[_Width * _Height * 4]);

    const int height   = _Height;
    const int width    = _Width;
    const int channels = _Format == ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA ? 4 : _Format == ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGB ? 3 : 1;

    const int     red      = 0;
    const int     green    = 1;
    const int     blue     = 2;
    const int     alpha    = 3;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image.get()[channels * (y * width + x) + blue ] = _RawBuffer[channels * (y * width + x) + red  ];
            image.get()[channels * (y * width + x) + green] = _RawBuffer[channels * (y * width + x) + green];
            image.get()[channels * (y * width + x) + red  ] = _RawBuffer[channels * (y * width + x) + blue ];
            image.get()[channels * (y * width + x) + alpha] = _RawBuffer[channels * (y * width + x) + alpha];
        }
    }

    return ApplicationRenderingBackendTexture();
}

void ApplicationRenderingBackend::destroy_texture(const ApplicationRenderingBackendTexture& _Texture)
{
    if(_Texture.is_null())
        return;
}

bool ApplicationRenderingBackend::load_mesh(
    const ApplicationRenderingBackendMeshVertex*      _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex& _VertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex* _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex& _IndexesCount)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();

    if(DirectX9 == nullptr)
    {
        return false;
    }

    // resize vertex buffer
    if(DirectX9->m_VertexBuffer == nullptr || DirectX9->m_VertexBufferSize < _VertexesCount)
    {
        if(DirectX9->m_VertexBuffer != nullptr)
            DirectX9->m_VertexBuffer->Release();

        D3D10_BUFFER_DESC bufferDescription;
        bufferDescription.Usage          = D3D10_USAGE_DYNAMIC;
        bufferDescription.ByteWidth      = sizeof(ApplicationRenderingBackendMeshVertex) * _VertexesCount;
        bufferDescription.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
        bufferDescription.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
        bufferDescription.MiscFlags      = 0;

        HRESULT HResult;
        if(FAILED(HResult = DirectX9->m_Device->CreateBuffer(&bufferDescription, nullptr, &DirectX9->m_VertexBuffer)))
        {
            std::cout << "could not crteate vertex buffer \n";
            return false;
        }
        DirectX9->m_VertexBufferSize = _VertexesCount;
    }

    // resize index buffer
    if(DirectX9->m_IndexBuffer == nullptr || DirectX9->m_IndexBufferSize < _IndexesCount)
    {
        if(DirectX9->m_IndexBuffer != nullptr)
            DirectX9->m_IndexBuffer->Release();

        D3D10_BUFFER_DESC bufferDescription;
        bufferDescription.Usage          = D3D10_USAGE_DYNAMIC;
        bufferDescription.ByteWidth      = sizeof(ApplicationRenderingBackendMeshVertexIndex) * _IndexesCount;
        bufferDescription.BindFlags      = D3D10_BIND_INDEX_BUFFER;
        bufferDescription.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
        bufferDescription.MiscFlags      = 0;

        HRESULT HResult;
        if(FAILED(HResult = DirectX9->m_Device->CreateBuffer(&bufferDescription, nullptr, &DirectX9->m_IndexBuffer)))
        {
            std::cout << "could not crteate index buffer \n";
            return false;
        }
        DirectX9->m_IndexBufferSize = _IndexesCount;
    }

    // write data into vertex buffer
    void* pMappedVertexResource = nullptr;

    if (SUCCEEDED(DirectX9->m_VertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pMappedVertexResource)))
    {
        memcpy(pMappedVertexResource, _Vertexes, sizeof(ApplicationRenderingBackendMeshVertex) * _VertexesCount);
        DirectX9->m_VertexBuffer->Unmap();
    }

    // write data into index buffer
    void* pMappedIndexResource = nullptr;

    if (SUCCEEDED(DirectX9->m_IndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pMappedIndexResource)))
    {
        memcpy(pMappedIndexResource, _Indexes, sizeof(ApplicationRenderingBackendMeshVertexIndex) * _IndexesCount);
        DirectX9->m_IndexBuffer->Unmap();
    }

    // attach vertex buffer
    UINT stride = sizeof(ApplicationRenderingBackendMeshVertex);
    UINT offset = 0;
    DirectX9->m_Device->IASetVertexBuffers(0, 1, &DirectX9->m_VertexBuffer, &stride, &offset);

    // attach index buffer
    DirectX9->m_Device->IASetIndexBuffer(DirectX9->m_IndexBuffer, sizeof(ApplicationRenderingBackendMeshVertexIndex) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

    return true;
}

void ApplicationRenderingBackend::render_mesh(
    const ApplicationRenderingBackendMeshVertexIndex& _SourceMeshVertex,
    const ApplicationRenderingBackendMeshVertexIndex& _TargetMeshVertex,
    const ApplicationRenderingBackendTexture&         _Texture,
    const gs_mat4f&                                   _MeshProjectionMatrix)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();
    
    if(DirectX9 == nullptr || _SourceMeshVertex < 0 || _TargetMeshVertex < 0 || (_TargetMeshVertex - _SourceMeshVertex) <= 0)
        return;

    // 1. Блокируем буфер для записи
    ApplicationRenderingBackendDirectXCBuffer* projectionMatrixDataPtr{nullptr};    
    HRESULT HResult;

    if(SUCCEEDED(HResult = DirectX9->m_ProjectionBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&projectionMatrixDataPtr)))
    {
        memcpy(projectionMatrixDataPtr, &_MeshProjectionMatrix[0][0], _MeshProjectionMatrix.rows() * _MeshProjectionMatrix.columns() * sizeof(float));
        DirectX9->m_ProjectionBuffer->Unmap();
    }

    DirectX9->m_Device->VSSetConstantBuffers(0, 1, &DirectX9->m_ProjectionBuffer);
    DirectX9->m_Device->DrawIndexed(_TargetMeshVertex - _SourceMeshVertex, _SourceMeshVertex, 0);
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();

    if(DirectX9 != nullptr)
        DirectX9->m_Viewport = gs_2d_boxf(_Position, _Position + _Size);
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();

    if(DirectX9 != nullptr)
        DirectX9->m_ClearColor = _Color;
}

void ApplicationRenderingBackend::scissor_box(const gs_2d_boxf& _ClippingRect)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();

    if(DirectX9 == nullptr)
        return;

//     gs_vec2f   displayScale = ApplicationPlatformBackend::get_window_framebuffer_size() / ApplicationPlatformBackend::get_window_size();
//     gs_2d_boxf clippingBox  = gs_2d_boxf(_ClippingRect.Min * displayScale, _ClippingRect.Max * displayScale);

//     RECT scissorRect;
//     SetRect(
//         &scissorRect,
//         (int)clippingBox.Min.x,
//         (int)clippingBox.Min.y,
//         (int)(clippingBox.Min.x + clippingBox.width()),
//         (int)(clippingBox.Min.y + clippingBox.height()));

//     DirectX9->m_Device->SetScissorRect(&scissorRect);
}

void ApplicationRenderingBackend::mesh_rendering_hints(const ApplicationRenderingBackendMeshRenderingHints& _Hints)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();

    if(DirectX9 == nullptr)
        return;

    if(_Hints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Lines)
        DirectX9->m_Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
    else if(_Hints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Triangles)
        DirectX9->m_Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// camera and view projection API
ApplicationRenderingBackend::Projections ApplicationRenderingBackend::calculate_2d_camera_view_and_projection(
    const gs_vec2f& _CameraWorldPosition,
    const gs_vec2f& _CameraResolution,
    const float&    _CameraRotationAngle,
    const float&    _CameraNearPlanePosition,
    const float&    _CameraFarPlanePosition)
{
    // compute projection matrix
    float left   = -_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
    float right  = +_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
    float bottom = +_CameraResolution.y * 0.5f + _CameraWorldPosition.y;
    float top    = -_CameraResolution.y * 0.5f + _CameraWorldPosition.y;

    // camera orientation
    gs_vec3f cameraWorldUpAxisDirection    = gs_vec3f(0.f, 1.f, 0.f);
    gs_vec3f cameraWorldFrontAxisDirection = gs_vec3f(0.f, 0.f, +1.f);
    gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(cameraWorldFrontAxisDirection);
    gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, cameraWorldUpAxisDirection));
    gs_vec3f cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

    gs_mat4f cameraview =
        gs_matrix_look_at(
            gs_vec3f(0.f, 0.f, 1),
            gs_vec3f(0.f, 0.f, 1) + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection, false);
    
    gs_mat4f projection =
        gs_matrix_ortho(
            left,
            right,
            bottom,
            top,
            _CameraNearPlanePosition,
            _CameraFarPlanePosition,
            false,
            false) * gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(_CameraRotationAngle), gs_vec3f(0.f, 0.f, 1.f));

    return {cameraview, projection};
}

float ApplicationRenderingBackend::calculate_object_depth(const float& _Depth)
{
    return -_Depth;
}

bool ApplicationRenderingBackend::compare_objects_depths(const float& _A, const float& _B)
{
    return _A > _B;
}

gs_vec2f ApplicationRenderingBackend::convert_to_NDC(const gs_vec2f& _Position, const gs_vec2f& _Screen)
{
    return gs_vec2f((2.0f * _Position.x) / _Screen.x - 1.0f, 1.0f - (2.0f * _Position.y) / _Screen.y);
}