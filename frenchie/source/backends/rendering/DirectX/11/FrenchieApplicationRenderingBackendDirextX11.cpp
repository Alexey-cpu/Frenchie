// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// D3D9
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include <iostream>


using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationRenderingBackendDirectX11 : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendDirectX11(){}
            virtual ~ApplicationRenderingBackendDirectX11(){}

            HWND                   hWnd;

            // device and swap chain
            ID3D11Device*            m_Device                 {nullptr};
            ID3D11DeviceContext*     m_DeviceContext          {nullptr};
            IDXGISwapChain*          m_SwapChain              {nullptr};

            ID3D11VertexShader*      m_VertexShader           {nullptr};
            ID3D11PixelShader*       m_PixelShader            {nullptr};
            ID3D11InputLayout*       m_VertexLayout           {nullptr};
            ID3D11Buffer*            m_ProjectionBuffer       {nullptr};

            ID3D11Buffer*            m_VertexBuffer           {nullptr};
            int                      m_VertexBufferSize       {0};

            ID3D11Buffer*            m_IndexBuffer            {nullptr};
            int                      m_IndexBufferSize        {0};

            // MSAA render target
            ID3D11Texture2D*         m_MSAARenderTarget       {nullptr};
            ID3D11RenderTargetView*  m_MSAARenderTargetView   {nullptr};

            // depth stencil render target
            ID3D11Texture2D*         m_DepthStencilTarget     {nullptr};
            ID3D11DepthStencilView*  m_DepthStencilTargetView {nullptr};

            ID3D11BlendState*        m_AlphaBlendState        {nullptr};
            ID3D11DepthStencilState* m_DepthStencilState      {nullptr};
            ID3D11RasterizerState*   m_RasterizerState        {nullptr};
            ID3D11SamplerState*      m_DefaultSamplerState    {nullptr};

            gs_color                  m_ClearColor;
            std::optional<gs_2d_boxf> m_Viewport;
        };

        struct ApplicationRenderingBackendDirectXCBuffer
        {
            float Projection[16]{};
        };

        struct ApplicationRenderingBackendDirectX11Texture
        {
            ID3D11Texture2D*          Texture            {nullptr};
            ID3D11ShaderResourceView* TextureShaderView  {nullptr};
            ID3D11SamplerState*       TextureSamplerState{nullptr};
        };

        template <typename T>
        inline void d3d11_release(T*& _Resource)
        {
            if(_Resource == nullptr)
                return;

            _Resource->Release();
            _Resource = nullptr;
        }

        bool d3d11_create_device_and_swap_chain(ApplicationRenderingBackendDirectX11* _DirectX, const float& _Width, const float& height)
        {
            HRESULT HResult;

            // device swap chain description
            DXGI_SWAP_CHAIN_DESC swapChainDescription;
            ZeroMemory(&swapChainDescription, sizeof(swapChainDescription));
            swapChainDescription.BufferCount                        = 2;                               // Number of back buffers
            swapChainDescription.BufferDesc.Width                   = _Width;                           // Resolution width
            swapChainDescription.BufferDesc.Height                  = height;                          // Resolution height
            swapChainDescription.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;      // Pixel format
            swapChainDescription.BufferDesc.RefreshRate.Numerator   = 60;                              // Refresh rate
            swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
            swapChainDescription.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Usage of the buffer
            swapChainDescription.OutputWindow                       = _DirectX->hWnd;                  // Target window handle
            swapChainDescription.SampleDesc.Count                   = 1;                               // Multi-sampling (1 = no MSAA)
            swapChainDescription.SampleDesc.Quality                 = 0;                               // quality
            swapChainDescription.Windowed                           = TRUE;                            // Windowed or fullscreen
            swapChainDescription.SwapEffect                         = DXGI_SWAP_EFFECT_FLIP_DISCARD;

            // create swap chain
            UINT createDeviceFlags = 0;
    #if defined(DEBUG) || defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

            // 3. Create the device, immediate context and swap chain
            if (FAILED(HResult = D3D11CreateDeviceAndSwapChain(
                nullptr,                       // Default adapter / video card
                D3D_DRIVER_TYPE_HARDWARE,      // Driver type (Hardware acceleration)
                nullptr,                       // Software rasterizer handle (NULL if not using software)
                createDeviceFlags,             // Creation flags
                nullptr,                       // Feature levels
                0,                             // Number of feature levels
                D3D11_SDK_VERSION,             // SDK version
                &swapChainDescription,         // Swap chain description pointer
                &_DirectX->m_SwapChain,        // Output: Swap chain pointer
                &_DirectX->m_Device,           // Output: D3D11 device pointer
                nullptr,                       // Output: feature level
                &_DirectX->m_DeviceContext     // Output: D3D11 device context pointer
            )))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create D3D11 device and swap chain\n";
                #endif

                d3d11_release(_DirectX->m_SwapChain);
                d3d11_release(_DirectX->m_Device);
                d3d11_release(_DirectX->m_DeviceContext);
                return false;
            }

            return true;
        }

        bool d3d11_create_and_compile_shaders(ApplicationRenderingBackendDirectX11* DirectX)
        {
            HRESULT HResult;

            const char* shaderProgram =
R"(
cbuffer ApplicationRenderingBackendDirectXCBuffer : register(b0)
{
    column_major float4x4 Projection;
};

Texture2D Texture;

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
    //return input.Color; 
    return Texture.Sample(linearSampler, input.UV) * input.Color; 
}
)";

            // try compile shaders
            ID3DBlob* pVertexShaderBlob = nullptr;
            ID3DBlob* pPixelShaderBlob  = nullptr;
            ID3DBlob* pErrors           = nullptr;


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
                    #if defined(DEBUG) || defined(_DEBUG)
                    char* compileErrors = static_cast<char*>(pErrors->GetBufferPointer());
                    std::cerr << "Shader Compilation Error:\n" << compileErrors << std::endl;
                    #endif
                    pErrors->Release();
                }
                else
                {
                    #if defined(DEBUG) || defined(_DEBUG)
                    std::cerr << "Shader compilation failed, but no error log was generated ...\n";
                    #endif
                }

                d3d11_release(pVertexShaderBlob);
                d3d11_release(pPixelShaderBlob);
                d3d11_release(pErrors);

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
                    #if defined(DEBUG) || defined(_DEBUG)
                    char* compileErrors = static_cast<char*>(pErrors->GetBufferPointer());
                    std::cerr << "Shader Compilation Error:\n" << compileErrors << std::endl;
                    #endif
                    pErrors->Release();
                }
                else
                {
                    std::cerr << "Shader compilation failed, but no error log was generated ...\n";
                }

                d3d11_release(pVertexShaderBlob);
                d3d11_release(pPixelShaderBlob);
                d3d11_release(pErrors);

                return false;
            }

            // create mesh vertex layout
            D3D11_INPUT_ELEMENT_DESC inputLayout[] =
            {
                {
                    "POSITION",
                    0,
                    DXGI_FORMAT_R32G32B32_FLOAT,
                    0,
                    static_cast<UINT>(offsetof(ApplicationRenderingBackendMeshVertex, Position)),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0
                },
                {
                    "TEXCOORD",
                    0,
                    DXGI_FORMAT_R32G32_FLOAT,
                    0,
                    static_cast<UINT>(offsetof(ApplicationRenderingBackendMeshVertex, UV)),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0
                },
                {
                    "COLOR",
                    0,
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    0,
                    static_cast<UINT>(offsetof(ApplicationRenderingBackendMeshVertex, Color)),
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0
                }
            };

            if(FAILED(HResult = DirectX->m_Device->CreateInputLayout(
                inputLayout,
                ARRAYSIZE(inputLayout),
                pVertexShaderBlob->GetBufferPointer(), // Указатель на скомпилированный шейдер
                pVertexShaderBlob->GetBufferSize(),    // Размер скомпилированного шейдера
                &DirectX->m_VertexLayout
            )))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create vertex layout " << HResult << "\n";
                #endif

                d3d11_release(pVertexShaderBlob);
                d3d11_release(pPixelShaderBlob);
                d3d11_release(pErrors);
                return false;
            }

            // create vertex shader object
            if(FAILED(HResult = DirectX->m_Device->CreateVertexShader(
                pVertexShaderBlob->GetBufferPointer(),
                pVertexShaderBlob->GetBufferSize(),
                nullptr,
                &DirectX->m_VertexShader)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create vertex shader \n";
                #endif

                d3d11_release(pVertexShaderBlob);
                d3d11_release(pPixelShaderBlob);
                d3d11_release(pErrors);

                return false;
            }
            
            // create pixel shader object
            if(FAILED(HResult = DirectX->m_Device->CreatePixelShader(
                pPixelShaderBlob->GetBufferPointer(),
                pPixelShaderBlob->GetBufferSize(),
                nullptr,
                &DirectX->m_PixelShader)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create pixel shader \n";
                #endif

                d3d11_release(pVertexShaderBlob);
                d3d11_release(pPixelShaderBlob);
                d3d11_release(pErrors);

                return false;
            }

            D3D11_BUFFER_DESC projectionBufferDescription = {};
            projectionBufferDescription.Usage          = D3D11_USAGE_DYNAMIC;
            projectionBufferDescription.ByteWidth      = sizeof(ApplicationRenderingBackendDirectXCBuffer);
            projectionBufferDescription.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
            projectionBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            if(FAILED(HResult = DirectX->m_Device->CreateBuffer(
                &projectionBufferDescription,
                nullptr,
                &DirectX->m_ProjectionBuffer)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create projection constant buffer " << HResult << "\n";
                #endif

                d3d11_release(pVertexShaderBlob);
                d3d11_release(pPixelShaderBlob);
                d3d11_release(pErrors);

                return false;
            }

            d3d11_release(pVertexShaderBlob);
            d3d11_release(pPixelShaderBlob);
            d3d11_release(pErrors);

            return true;
        }

        bool d3d11_create_render_target_and_depth_view(ApplicationRenderingBackendDirectX11* _DirectX, const float& _Width, const float& _Height)
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
            D3D11_TEXTURE2D_DESC msaaTextureDescription;
            msaaTextureDescription.Width              = _Width;
            msaaTextureDescription.Height             = _Height;
            msaaTextureDescription.MipLevels          = 1;
            msaaTextureDescription.ArraySize          = 1;
            msaaTextureDescription.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
            msaaTextureDescription.SampleDesc.Count   = sampleCount;
            msaaTextureDescription.SampleDesc.Quality = qualityLevels - 1;
            msaaTextureDescription.Usage              = D3D11_USAGE_DEFAULT;
            msaaTextureDescription.BindFlags          = D3D11_BIND_RENDER_TARGET;
            msaaTextureDescription.MiscFlags          = 0;
            msaaTextureDescription.CPUAccessFlags     = 0;

            if(FAILED(HResult = _DirectX->m_Device->CreateTexture2D(
                &msaaTextureDescription,
                nullptr,
                &_DirectX->m_MSAARenderTarget)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create MSAA texture \n";
                #endif

                return false;
            }

            // create render target view with MSAA texture
            if (FAILED(HResult = _DirectX->m_Device->CreateRenderTargetView(
                _DirectX->m_MSAARenderTarget,
                NULL,
                &_DirectX->m_MSAARenderTargetView)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create rendering target view with MSAA texture \n";
                #endif

                return false;
            }

            // RTV depth/stencil texture
            D3D11_TEXTURE2D_DESC depthTextureDescription;
            depthTextureDescription.Width              = _Width;
            depthTextureDescription.Height             = _Height;
            depthTextureDescription.MipLevels          = 1;
            depthTextureDescription.ArraySize          = 1;
            depthTextureDescription.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthTextureDescription.SampleDesc.Count   = 4;
            depthTextureDescription.SampleDesc.Quality = 0;
            depthTextureDescription.Usage              = D3D11_USAGE_DEFAULT;
            depthTextureDescription.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
            depthTextureDescription.CPUAccessFlags     = 0;
            depthTextureDescription.MiscFlags          = 0;

            if(FAILED(HResult = _DirectX->m_Device->CreateTexture2D(
                &depthTextureDescription,
                nullptr,
                &_DirectX->m_DepthStencilTarget)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create depth buffer texture \n";
                #endif

                return false;
            }

            // create depth/stencil view
            if(FAILED(HResult = _DirectX->m_Device->CreateDepthStencilView(
                _DirectX->m_DepthStencilTarget,
                nullptr,
                &_DirectX->m_DepthStencilTargetView)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create depth/stencil view with corresponding buffer texture \n";
                #endif

                return false;
            }

            return true;
        }
        
        bool d3d11_create_viewport(ApplicationRenderingBackendDirectX11* _DirectX, const float& _Width, const float& _Height)
        {
            if(_DirectX == nullptr) return false;

            D3D11_VIEWPORT viewport;
            viewport.TopLeftX = 0;          // x coordinate
            viewport.TopLeftY = 0;          // y coordinate
            viewport.Width    = _Width;     // width
            viewport.Height   = _Height;    // height
            viewport.MinDepth = 0.0f;       // min depth
            viewport.MaxDepth = 1.0f;       // max depth

            // attach viewport to swap chain
            _DirectX->m_DeviceContext->RSSetViewports(1, &viewport);
            return true;

        }

        bool d3d11_alpha_blending(ApplicationRenderingBackendDirectX11* DirectX)
        {
            if(DirectX == nullptr) return false;

            // describe the blend state for standard transparency
            D3D11_BLEND_DESC blendDesc;
            ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

            blendDesc.AlphaToCoverageEnable  = FALSE;
            blendDesc.IndependentBlendEnable = FALSE;
            blendDesc.RenderTarget[0].BlendEnable           = TRUE;
            blendDesc.RenderTarget[0].SrcBlend               = D3D11_BLEND_SRC_ALPHA;
            blendDesc.RenderTarget[0].DestBlend              = D3D11_BLEND_INV_SRC_ALPHA;
            blendDesc.RenderTarget[0].BlendOp                = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].SrcBlendAlpha          = D3D11_BLEND_ONE;
            blendDesc.RenderTarget[0].DestBlendAlpha         = D3D11_BLEND_ZERO;
            blendDesc.RenderTarget[0].BlendOpAlpha           = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].RenderTargetWriteMask  = D3D11_COLOR_WRITE_ENABLE_ALL;

            // create the blend state object
            HRESULT HResult;

            if(FAILED(HResult = DirectX->m_Device->CreateBlendState(&blendDesc, &DirectX->m_AlphaBlendState)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create alpha blend state \n";
                #endif

                return false;
            }

            return true;
        }

        bool d3d11_depth_testing(ApplicationRenderingBackendDirectX11* DirectX)
        {
            if(DirectX == nullptr) return false;

            HRESULT HResult;
            D3D11_DEPTH_STENCIL_DESC dsDesc;
            ZeroMemory(&dsDesc, sizeof(dsDesc));

            dsDesc.DepthEnable                  = TRUE;
            dsDesc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
            dsDesc.DepthFunc                    = D3D11_COMPARISON_LESS;
            dsDesc.StencilEnable                = TRUE;
            dsDesc.StencilReadMask              = 0xFF;
            dsDesc.StencilWriteMask             = 0xFF;
            dsDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
            dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
            dsDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
            dsDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
            dsDesc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
            dsDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
            dsDesc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
            dsDesc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;

            if (FAILED(HResult = DirectX->m_Device->CreateDepthStencilState(&dsDesc, &DirectX->m_DepthStencilState))) 
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create depth/stencil state \n";
                #endif

                return false;
            }
            return true;
        }

        bool d3d11_scissor_testing(ApplicationRenderingBackendDirectX11* DirectX)
        {
            // m_RasterizerState

            D3D11_RASTERIZER_DESC rasterizerDesc;
            ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

            rasterizerDesc.FillMode              = D3D11_FILL_SOLID;
            rasterizerDesc.CullMode              = D3D11_CULL_NONE;
            rasterizerDesc.FrontCounterClockwise = TRUE;
            rasterizerDesc.DepthBias             = 0;
            rasterizerDesc.DepthBiasClamp        = 0.0f;
            rasterizerDesc.SlopeScaledDepthBias  = 0.0f;
            rasterizerDesc.DepthClipEnable       = TRUE;
            rasterizerDesc.MultisampleEnable     = TRUE;
            rasterizerDesc.AntialiasedLineEnable = TRUE;
            rasterizerDesc.ScissorEnable         = TRUE; 

            HRESULT HResult;

            if(FAILED(HResult = DirectX->m_Device->CreateRasterizerState(&rasterizerDesc, &DirectX->m_RasterizerState)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create rasterizer state \n";
                #endif

                return false;
            }

            return true;
        }

        bool d3d11_default_sampler(ApplicationRenderingBackendDirectX11* DirectX)
        {
            if(DirectX == nullptr) return false;

            // fill sampler description
            D3D11_SAMPLER_DESC sampDesc;
            ZeroMemory(&sampDesc, sizeof(sampDesc));
            sampDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
            sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
            sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
            sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            sampDesc.MinLOD         = 0;
            sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;

            // create sampler
            HRESULT HResult;

            if(FAILED(HResult = DirectX->m_Device->CreateSamplerState(&sampDesc, &DirectX->m_DefaultSamplerState)))
            {
                #if defined(DEBUG) || defined(_DEBUG)
                std::cout << "could not create default sampler \n";
                #endif

                return false;   
            }

            return true;
        }

        bool d3d11_resize_viewport(ApplicationRenderingBackendDirectX11* _DirectX, const float& _Width, const float& _Height)
        {
            if(_DirectX == nullptr) return false;

            if(_DirectX->m_MSAARenderTarget)
                _DirectX->m_MSAARenderTarget->Release();
            _DirectX->m_MSAARenderTarget = nullptr;
            
            if(_DirectX->m_MSAARenderTargetView)
                _DirectX->m_MSAARenderTargetView->Release();
            _DirectX->m_MSAARenderTargetView = nullptr;

            if(_DirectX->m_DepthStencilTarget)
                _DirectX->m_DepthStencilTarget->Release();
            _DirectX->m_DepthStencilTarget = nullptr;

            if(_DirectX->m_DepthStencilTargetView)
                _DirectX->m_DepthStencilTargetView->Release();
            _DirectX->m_DepthStencilTargetView = nullptr;

            _DirectX->m_SwapChain->ResizeBuffers(0, _Width, _Height, DXGI_FORMAT_UNKNOWN, 0);

            return d3d11_create_render_target_and_depth_view(_DirectX, _Width, _Height) && d3d11_create_viewport(_DirectX, _Width, _Height);
        }
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
    
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX =
        std::dynamic_pointer_cast<ApplicationRenderingBackendDirectX11>(m_Api = std::make_shared<ApplicationRenderingBackendDirectX11>());

    DirectX->hWnd = hWnd;

    RECT rc;
    GetClientRect(DirectX->hWnd, &rc);
    UINT width  = rc.right  - rc.left;
    UINT height = rc.bottom - rc.top;

    if(!d3d11_create_device_and_swap_chain(DirectX.get(), width, height)) 
        return false;

    if(!d3d11_create_render_target_and_depth_view(DirectX.get(), width, height))
        return false;

    if(!d3d11_create_viewport(DirectX.get(), width, height))
        return false;

    if(!d3d11_create_and_compile_shaders(DirectX.get()))
        return false;

    if(!d3d11_alpha_blending(DirectX.get()))
        return false;

    if(!d3d11_depth_testing(DirectX.get()))
        return false;

    if(!d3d11_scissor_testing(DirectX.get()))
        return false;

    if(!d3d11_default_sampler(DirectX.get()))
        return false;

    return true;
}

void ApplicationRenderingBackend::begin_render(ApplicationRenderingBackendRenderingTarget* _Target)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();
    
    if(DirectX == nullptr)
        return;

    if(DirectX->m_Viewport.has_value())
    {
        d3d11_resize_viewport(DirectX.get(), DirectX->m_Viewport.value().width(), DirectX->m_Viewport.value().height());
        DirectX->m_Viewport.reset();
    }

    // setup new render targets
    if((DirectX->m_RenderingTarget = _Target) != nullptr)
    {
        if(DirectX->m_Viewport.has_value() && DirectX->m_RenderingTarget->FrameBufferTexture.has_value())
        {
            destroy_texture(DirectX->m_RenderingTarget->FrameBufferTexture.value());
            DirectX->m_RenderingTarget->FrameBufferTexture.reset();
        }

        if(!DirectX->m_RenderingTarget->FrameBufferTexture.has_value())
        {
            D3D11_TEXTURE2D_DESC msaaRenderingTargetDescription;
            DirectX->m_MSAARenderTarget->GetDesc(&msaaRenderingTargetDescription);

            DirectX->m_RenderingTarget->FrameBufferTexture = construct_texture(
                nullptr,
                msaaRenderingTargetDescription.Width,
                msaaRenderingTargetDescription.Height,
                ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                ApplicationRenderingBackendTextureFilter_::ApplicationRenderingBackendTextureFilter_Linear, 
                0);
        }
    }

    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    FLOAT clearcolor[4] =
    {
        gs_color_rgba_get_r(DirectX->m_ClearColor) / 255.f,
        gs_color_rgba_get_g(DirectX->m_ClearColor) / 255.f,
        gs_color_rgba_get_b(DirectX->m_ClearColor) / 255.f,
        gs_color_rgba_get_a(DirectX->m_ClearColor) / 255.f,
    };

    DirectX->m_DeviceContext->OMSetRenderTargets(1, &DirectX->m_MSAARenderTargetView, DirectX->m_DepthStencilTargetView);
    DirectX->m_DeviceContext->OMSetBlendState(DirectX->m_AlphaBlendState, blendFactor, 0xffffffff);
    DirectX->m_DeviceContext->OMSetDepthStencilState(DirectX->m_DepthStencilState, 1);
    DirectX->m_DeviceContext->ClearRenderTargetView(DirectX->m_MSAARenderTargetView, clearcolor);
    DirectX->m_DeviceContext->ClearDepthStencilView(DirectX->m_DepthStencilTargetView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    DirectX->m_DeviceContext->IASetInputLayout(DirectX->m_VertexLayout);
    DirectX->m_DeviceContext->VSSetShader(DirectX->m_VertexShader, nullptr, 0);
    DirectX->m_DeviceContext->PSSetShader(DirectX->m_PixelShader, nullptr, 0);
    DirectX->m_DeviceContext->RSSetState(DirectX->m_RasterizerState);
    DirectX->m_DeviceContext->PSSetSamplers(0, 1, &DirectX->m_DefaultSamplerState);
}

void ApplicationRenderingBackend::end_render()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();

    if(DirectX == nullptr)
        return;

    if(DirectX->m_RenderingTarget && DirectX->m_RenderingTarget->FrameBufferTexture.has_value())
    {
        ApplicationRenderingBackendDirectX11Texture* texture =
            reinterpret_cast<ApplicationRenderingBackendDirectX11Texture*>(DirectX->m_RenderingTarget->FrameBufferTexture.value().Ptr);

        DirectX->m_DeviceContext->ResolveSubresource(texture->Texture, 0, DirectX->m_MSAARenderTarget, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
        DirectX->m_SwapChain->Present(0, 0);
    }
    else
    {
        ID3D11Texture2D* swapChainFrameBuffer = nullptr;
        DirectX->m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&swapChainFrameBuffer);
        DirectX->m_DeviceContext->ResolveSubresource(swapChainFrameBuffer, 0, DirectX->m_MSAARenderTarget, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
        DirectX->m_SwapChain->Present(0, 0);
        swapChainFrameBuffer->Release();
    }
}

void ApplicationRenderingBackend::quit()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();

    if(DirectX == nullptr)
        return;

    d3d11_release(DirectX->m_PixelShader);
    d3d11_release(DirectX->m_VertexShader);
    d3d11_release(DirectX->m_VertexLayout);

    d3d11_release(DirectX->m_AlphaBlendState);
    d3d11_release(DirectX->m_DepthStencilState);
    d3d11_release(DirectX->m_RasterizerState);
    d3d11_release(DirectX->m_DefaultSamplerState);

    d3d11_release(DirectX->m_ProjectionBuffer);
    d3d11_release(DirectX->m_VertexBuffer);
    d3d11_release(DirectX->m_IndexBuffer);

    d3d11_release(DirectX->m_MSAARenderTargetView);
    d3d11_release(DirectX->m_MSAARenderTarget);
    d3d11_release(DirectX->m_DepthStencilTargetView);
    d3d11_release(DirectX->m_DepthStencilTarget);

    d3d11_release(DirectX->m_DeviceContext);
    d3d11_release(DirectX->m_SwapChain);
    d3d11_release(DirectX->m_Device);

}

ApplicationRenderingBackendTexture ApplicationRenderingBackend::construct_texture(
    const unsigned char*                              _RawBuffer,
    const int&                                        _Width,
    const int&                                        _Height,
    const ApplicationRenderingBackendTextureFormat&   _Format,
    const ApplicationRenderingBackendTextureWrapMode& _Wrap,
    const ApplicationRenderingBackendTextureFilter&   _Filter,
    const int&                                        _Attributes)
{
    (void)_RawBuffer;
    (void)_Width;
    (void)_Height;
    (void)_Format;
    (void)_Wrap;
    (void)_Filter;
    (void)_Attributes;

    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();

    if(DirectX == nullptr)
        return ApplicationRenderingBackendTexture();

    HRESULT                   HResult;
    ID3D11Texture2D*          pTexture                   = nullptr;
    ID3D11ShaderResourceView* pTextureShaderResourceView = nullptr;
    ID3D11SamplerState*       pSamplerState              = nullptr;

    // create texture
    D3D11_TEXTURE2D_DESC textureDescription;
    textureDescription.Width              = _Width;                     // width
    textureDescription.Height             = _Height;                    // height
    textureDescription.MipLevels          = 1;                          // mip levels count
    textureDescription.ArraySize          = 1;                          // textures count within pixel buffer
    textureDescription.Format             = DXGI_FORMAT_R8G8B8A8_UNORM; // pixel format
    textureDescription.SampleDesc.Count   = 1;                          // MSAA samples count
    textureDescription.SampleDesc.Quality = 0;                          // MSAA quality
    textureDescription.Usage              = D3D11_USAGE_DEFAULT;        //
    textureDescription.BindFlags          = D3D11_BIND_SHADER_RESOURCE; //
    textureDescription.CPUAccessFlags     = 0;                          //
    textureDescription.MiscFlags          = 0;

    // create texture
    if (FAILED(HResult = DirectX->m_Device->CreateTexture2D(&textureDescription, nullptr, &pTexture)))
    {
        return ApplicationRenderingBackendTexture();
    }

    // create texture resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC textureShaderResourceDescription;
    textureShaderResourceDescription.Format                    = textureDescription.Format;
    textureShaderResourceDescription.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    textureShaderResourceDescription.Texture2D.MostDetailedMip = 0;
    textureShaderResourceDescription.Texture2D.MipLevels       = textureDescription.MipLevels;

    // create texture shader resource view
    if (FAILED(HResult = DirectX->m_Device->CreateShaderResourceView(pTexture, &textureShaderResourceDescription, &pTextureShaderResourceView)))
    {
        pTexture->Release();
        return ApplicationRenderingBackendTexture();
    }

    // create texture sampler
    D3D11_SAMPLER_DESC textureSamplerDescription;

    // setup texture wrap mode
    switch (_Wrap)
    {
        case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat:
            textureSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            textureSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            textureSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            break;
        
        case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Mirrored:
            textureSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
            textureSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
            textureSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
            break;

        case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToEdge:
            textureSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            textureSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            textureSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            break;

        case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToBorder:
            textureSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            textureSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            textureSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
            break;
        default:
            break;
    }

    // set minifying filter
    switch (_Filter)
    {
    case ApplicationRenderingBackendTextureFilter_::ApplicationRenderingBackendTextureFilter_Linear:
        textureSamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        break;
    
    case ApplicationRenderingBackendTextureFilter_::ApplicationRenderingBackendTextureFilter_Nearest:
        textureSamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        break;
    default:
        textureSamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    }

    textureSamplerDescription.MipLODBias     = 0.0f;
    textureSamplerDescription.MaxAnisotropy  = 1;
    textureSamplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
    textureSamplerDescription.BorderColor[0] = 0.0f;
    textureSamplerDescription.BorderColor[1] = 0.0f;
    textureSamplerDescription.BorderColor[2] = 0.0f;
    textureSamplerDescription.BorderColor[3] = 0.0f;

    if (FAILED(HResult = DirectX->m_Device->CreateSamplerState(&textureSamplerDescription, &pSamplerState)))
    {
        pTexture->Release();
        pTextureShaderResourceView->Release();
        return ApplicationRenderingBackendTexture();
    }

    if(_RawBuffer != nullptr)
    {
        DirectX->m_DeviceContext->UpdateSubresource(
            pTexture,                // Destination texture
            0,                       // Subresource index
            NULL,                    // DestBox (NULL means write to the whole texture)
            _RawBuffer,              // Source raw buffer pointer
            _Width * _Format,        // Source row pitch
            0                        // Depth pitch (0 for 2D textures)
        );
    }

    return ApplicationRenderingBackendTexture(
        reinterpret_cast<uintptr_t>(new ApplicationRenderingBackendDirectX11Texture(
            {
                pTexture,
                pTextureShaderResourceView,
                pSamplerState
            })),
        _Width,
        _Height,
        gs_color_rgba(255, 255, 255, 255),
        _Format,
        _Wrap,
        _Filter,
        _Attributes);
}

void ApplicationRenderingBackend::destroy_texture(const ApplicationRenderingBackendTexture& _Texture)
{
    if(_Texture.is_null())
        return;

    ApplicationRenderingBackendDirectX11Texture* texture =
        reinterpret_cast<ApplicationRenderingBackendDirectX11Texture*>(_Texture.Ptr);

    if(texture == nullptr)
        return;

    d3d11_release(texture->Texture);
    d3d11_release(texture->TextureShaderView);
    d3d11_release(texture->TextureSamplerState);

    delete texture;
    const_cast<ApplicationRenderingBackendTexture&>(_Texture).Ptr = 0;
}

bool ApplicationRenderingBackend::load_mesh(
    const ApplicationRenderingBackendMeshVertex*      _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex& _VertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex* _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex& _IndexesCount)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();

    if(DirectX == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0)
        return false;

    // resize vertex buffer
    if(DirectX->m_VertexBuffer == nullptr || DirectX->m_VertexBufferSize < _VertexesCount)
    {
        if(DirectX->m_VertexBuffer != nullptr)
            DirectX->m_VertexBuffer->Release();

        D3D11_BUFFER_DESC bufferDescription;
        bufferDescription.Usage          = D3D11_USAGE_DYNAMIC;
        bufferDescription.ByteWidth      = sizeof(ApplicationRenderingBackendMeshVertex) * _VertexesCount;
        bufferDescription.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDescription.MiscFlags      = 0;

        HRESULT HResult;
        if(FAILED(HResult = DirectX->m_Device->CreateBuffer(&bufferDescription, nullptr, &DirectX->m_VertexBuffer)))
        {
            std::cout << "could not crteate vertex buffer \n";
            return false;
        }
        DirectX->m_VertexBufferSize = _VertexesCount;
    }

    // resize index buffer
    if(DirectX->m_IndexBuffer == nullptr || DirectX->m_IndexBufferSize < _IndexesCount)
    {
        if(DirectX->m_IndexBuffer != nullptr)
            DirectX->m_IndexBuffer->Release();

        D3D11_BUFFER_DESC bufferDescription;
        bufferDescription.Usage          = D3D11_USAGE_DYNAMIC;
        bufferDescription.ByteWidth      = sizeof(ApplicationRenderingBackendMeshVertexIndex) * _IndexesCount;
        bufferDescription.BindFlags      = D3D11_BIND_INDEX_BUFFER;
        bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDescription.MiscFlags      = 0;

        HRESULT HResult;
        if(FAILED(HResult = DirectX->m_Device->CreateBuffer(&bufferDescription, nullptr, &DirectX->m_IndexBuffer)))
        {
            std::cout << "could not crteate index buffer \n";
            return false;
        }
        DirectX->m_IndexBufferSize = _IndexesCount;
    }

    // write data into vertex buffer
    D3D11_MAPPED_SUBRESOURCE mappedVertexResource{};
    if (SUCCEEDED(DirectX->m_DeviceContext->Map(DirectX->m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource)))
    {
        memcpy(mappedVertexResource.pData, _Vertexes, sizeof(ApplicationRenderingBackendMeshVertex) * _VertexesCount);
        DirectX->m_DeviceContext->Unmap(DirectX->m_VertexBuffer, 0);
    }

    // write data into index buffer
    D3D11_MAPPED_SUBRESOURCE mappedIndexResource{};
    if (SUCCEEDED(DirectX->m_DeviceContext->Map(DirectX->m_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndexResource)))
    {
        memcpy(mappedIndexResource.pData, _Indexes, sizeof(ApplicationRenderingBackendMeshVertexIndex) * _IndexesCount);
        DirectX->m_DeviceContext->Unmap(DirectX->m_IndexBuffer, 0);
    }

    // attach vertex buffer
    UINT stride = sizeof(ApplicationRenderingBackendMeshVertex);
    UINT offset = 0;
    DirectX->m_DeviceContext->IASetVertexBuffers(0, 1, &DirectX->m_VertexBuffer, &stride, &offset);

    // attach index buffer
    DirectX->m_DeviceContext->IASetIndexBuffer(DirectX->m_IndexBuffer, sizeof(ApplicationRenderingBackendMeshVertexIndex) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

    return true;
}

void ApplicationRenderingBackend::render_mesh(
    const ApplicationRenderingBackendMeshVertexIndex& _SourceMeshVertex,
    const ApplicationRenderingBackendMeshVertexIndex& _TargetMeshVertex,
    const ApplicationRenderingBackendTexture&         _Texture,
    const gs_mat4f&                                   _MeshProjectionMatrix)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();
    
    if(DirectX == nullptr || _SourceMeshVertex < 0 || _TargetMeshVertex < 0 || (_TargetMeshVertex - _SourceMeshVertex) <= 0)
        return;

    // setup shader projection matrix
    D3D11_MAPPED_SUBRESOURCE mappedProjectionResource{};
    HRESULT HResult;

    if(SUCCEEDED(HResult = DirectX->m_DeviceContext->Map(DirectX->m_ProjectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedProjectionResource)))
    {
        memcpy(mappedProjectionResource.pData, &_MeshProjectionMatrix[0][0], _MeshProjectionMatrix.rows() * _MeshProjectionMatrix.columns() * sizeof(float));
        DirectX->m_DeviceContext->Unmap(DirectX->m_ProjectionBuffer, 0);
    }

    // bind texture
    if(!_Texture.is_null())
    {
        ApplicationRenderingBackendDirectX11Texture* texture =
            reinterpret_cast<ApplicationRenderingBackendDirectX11Texture*>(_Texture.Ptr);

        DirectX->m_DeviceContext->PSSetShaderResources(0, 1, &texture->TextureShaderView);
        DirectX->m_DeviceContext->PSSetSamplers(0, 1, &texture->TextureSamplerState);
    }

    DirectX->m_DeviceContext->VSSetConstantBuffers(0, 1, &DirectX->m_ProjectionBuffer);
    DirectX->m_DeviceContext->DrawIndexed(_TargetMeshVertex - _SourceMeshVertex, _SourceMeshVertex, 0);
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();

    if(DirectX != nullptr)
        DirectX->m_Viewport = gs_2d_boxf(_Position, _Position + _Size);
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();

    if(DirectX != nullptr)
        DirectX->m_ClearColor = _Color;
}

void ApplicationRenderingBackend::scissor_box(const gs_2d_boxf& _ClippingRect)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();

    if(DirectX == nullptr)
        return;

    gs_vec2f   displayScale = ApplicationPlatformBackend::get_window_framebuffer_size() / ApplicationPlatformBackend::get_window_size();
    gs_2d_boxf clippingBox  = gs_2d_boxf(_ClippingRect.Min * displayScale, _ClippingRect.Max * displayScale);

    D3D11_RECT scissorRect;
    scissorRect.left   = clippingBox.Min.x; // Левая граница в пикселях
    scissorRect.top    = clippingBox.Min.y; // Верхняя граница
    scissorRect.right  = clippingBox.Min.x + clippingBox.width(); // Правая граница
    scissorRect.bottom = clippingBox.Min.y + clippingBox.height(); // Нижняя граница
    DirectX->m_DeviceContext->RSSetScissorRects(1, &scissorRect);
}

void ApplicationRenderingBackend::mesh_rendering_hints(const ApplicationRenderingBackendMeshRenderingHints& _Hints)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX11> DirectX = graphics_api<ApplicationRenderingBackendDirectX11>();

    if(DirectX == nullptr)
        return;

    if(_Hints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Lines)
        DirectX->m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
    else if(_Hints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Triangles)
        DirectX->m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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