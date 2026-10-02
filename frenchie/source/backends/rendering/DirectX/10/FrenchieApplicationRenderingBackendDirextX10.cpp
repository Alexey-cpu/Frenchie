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

            ID3D10Device*          m_Device       {nullptr};
            IDXGISwapChain*        m_SwapChain    {nullptr};

            ID3D10VertexShader*    m_VertexShader {nullptr};
            ID3D10PixelShader*     m_PixelShader  {nullptr};

            ID3DBlob*              m_VertexShaderBlob {nullptr};
            ID3DBlob*              m_PixelShaderBlob  {nullptr};

            ID3D10InputLayout*     m_VertexLayout {nullptr};

            ID3D10RenderTargetView* m_RTV{nullptr};
            ID3D10RenderTargetView* m_RTVMSAA{nullptr};
            ID3D10DepthStencilView* m_DSV {nullptr};

            ID3D10RasterizerState*   m_RasterizerState{nullptr};
            ID3D10DepthStencilState* m_DepthStencilState{nullptr};

            ID3D10Texture2D* framebuffer;
            ID3D10Texture2D* msaabuffer;
            ID3D10Texture2D* pDepthStencilBuffer = nullptr;
        };

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
    HWND    hWnd;
    HRESULT HResult;

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

	//get window dimensions
	RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width  = rc.right  - rc.left;
    UINT height = rc.bottom - rc.top;

    // create swap chain and device
    {
        // 1. Fill out the DXGI_SWAP_CHAIN_DESC structure
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        
        sd.BufferCount                        = 2;                               // Number of back buffers
        sd.BufferDesc.Width                   = width;                           // Resolution width
        sd.BufferDesc.Height                  = height;                          // Resolution height
        sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;      // Pixel format
        sd.BufferDesc.RefreshRate.Numerator   = 60;                              // Refresh rate
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Usage of the buffer
        sd.OutputWindow                       = hWnd;                            // Target window handle
        sd.SampleDesc.Count                   = 2;                               // Multi-sampling (1 = no MSAA)
        sd.SampleDesc.Quality                 = 0;
        sd.Windowed                           = TRUE;                            // Windowed or fullscreen
        //sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

        // 2. Set creation parameters
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
            std::cout << "could not create D3D10 device\n";
            return false;
        }

        // 4. Enable multisampling if supported and setup quality levels
        UINT numQualityLevels = 0;

        if (SUCCEEDED(DirectX9->m_Device->CheckMultisampleQualityLevels(
            DXGI_FORMAT_R8G8B8A8_UNORM, // Проверяемый формат текстуры
            2,                          // Количество сэмплов (Count)
            &numQualityLevels           // Возвращаемое число уровней качества
            )) && numQualityLevels > 0)
        {
            sd.SampleDesc.Quality = numQualityLevels;
        }
        else
        {
        }
    }

    // create and compile HLSL shader
    {
        const char* shaderProgram =
R"(
Texture2D Texture;
matrix    Projection;

SamplerState linearSampler
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD;
    float4 Color    : COLOR;
};

struct VS_INPUT
{
	float4 Position : POSITION;
    float2 UV       : TEXCOORD;
    float4 Color    : COLOR;
};

PS_INPUT vertex_shader(VS_INPUT input)
{
	PS_INPUT output;
    output.Position = mul(input.Position, Projection);
	output.Color    = input.Color;
	output.UV       = input.UV;
    return output;  
}

float4 pixel_shader(PS_INPUT input) : SV_Target
{
    return Texture.Sample(linearSampler, input.UV) * input.Color; 
}
)";

        // try compile shaders
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
            &DirectX9->m_VertexShaderBlob,     // Output compiled binary blob
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
            &DirectX9->m_PixelShaderBlob,     // Output compiled binary blob
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

            return false;
        }

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
            DirectX9->m_VertexShaderBlob->GetBufferPointer(), // Указатель на скомпилированный шейдер
            DirectX9->m_VertexShaderBlob->GetBufferSize(),    // Размер скомпилированного шейдера
            &DirectX9->m_VertexLayout
        )))
        {
            std::cout << "could not create vertex layout " << HResult << "\n";
            return false;
        }

        // create vertex/pixel shader objects
        if(FAILED(HResult = DirectX9->m_Device->CreateVertexShader(
            DirectX9->m_VertexShaderBlob->GetBufferPointer(),
            DirectX9->m_VertexShaderBlob->GetBufferSize(),
            &DirectX9->m_VertexShader)))
        {
            std::cout << "could not create vertex shader \n";
            return false;
        }
        
        if(FAILED(HResult = DirectX9->m_Device->CreatePixelShader(
            DirectX9->m_PixelShaderBlob->GetBufferPointer(),
            DirectX9->m_PixelShaderBlob->GetBufferSize(),
            &DirectX9->m_PixelShader)))
        {
            std::cout << "could not create pixel shader \n";
            return false;
        }
    }

    // create render target view
    {        
        if (FAILED(HResult = DirectX9->m_SwapChain->GetBuffer(
            0,
            __uuidof(ID3D10Texture2D),
            (LPVOID*)&DirectX9->framebuffer)))
        {
            std::cout << "could not extract swap chain back buffer \n";
            return false;
        }

        // create RTV
        if (FAILED(HResult = DirectX9->m_Device->CreateRenderTargetView(
            DirectX9->framebuffer,
            NULL,
            &DirectX9->m_RTV)))
        {
            std::cout << "could not create RTV \n";
            return false;
        }

        // create RTV MSAAA texure
        D3D10_TEXTURE2D_DESC msaaDesc;
        msaaDesc.Width              = 2048;
        msaaDesc.Height             = 1024;
        msaaDesc.MipLevels          = 1;
        msaaDesc.ArraySize          = 1;
        msaaDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
        msaaDesc.SampleDesc.Count   = 4; // Количество семплов (например, 4x MSAA)
        msaaDesc.SampleDesc.Quality = 0; // Уровень качества
        msaaDesc.Usage              = D3D10_USAGE_DEFAULT;
        msaaDesc.BindFlags          = D3D10_BIND_RENDER_TARGET;
        msaaDesc.CPUAccessFlags     = 0;
        msaaDesc.MiscFlags          = 0;

        DirectX9->m_Device->CreateTexture2D(&msaaDesc, nullptr, &DirectX9->msaabuffer);

        if (FAILED(HResult = DirectX9->m_Device->CreateRenderTargetView(
            DirectX9->msaabuffer,
            NULL,
            &DirectX9->m_RTVMSAA)))
        {
            std::cout << "could not create RTV MSAA \n";
            return false;
        }

        // RTV depth/stencil texture
        D3D10_TEXTURE2D_DESC descDepth;
        descDepth.Width              = width;      // Ширина экрана/окна
        descDepth.Height             = height;    // Высота экрана/окна
        descDepth.MipLevels          = 1;
        descDepth.ArraySize          = 1;
        descDepth.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 бита на глубину, 8 бит на трафарет
        descDepth.SampleDesc.Count   = 4;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage              = D3D10_USAGE_DEFAULT;
        descDepth.BindFlags          = D3D10_BIND_DEPTH_STENCIL; // Флаг привязки как буфера глубины-трафарета
        descDepth.CPUAccessFlags     = 0;
        descDepth.MiscFlags          = 0;

        DirectX9->m_Device->CreateTexture2D(&descDepth, nullptr, &DirectX9->pDepthStencilBuffer);
        if(FAILED(HResult = DirectX9->m_Device->CreateDepthStencilView(DirectX9->pDepthStencilBuffer, nullptr, &DirectX9->m_DSV)))
        {
            std::cout << "could not create RTV depth buffer \n";
            return false;
        }
    }

    // create rasterizer state
    {
        D3D10_RASTERIZER_DESC rasterizerState;
        rasterizerState.CullMode              = D3D10_CULL_NONE;
        rasterizerState.FillMode              = D3D10_FILL_SOLID;
        rasterizerState.FrontCounterClockwise = true;
        rasterizerState.DepthBias             = false;
        rasterizerState.DepthBiasClamp        = 0;
        rasterizerState.SlopeScaledDepthBias  = 0;
        rasterizerState.DepthClipEnable       = true;
        rasterizerState.ScissorEnable         = true;
        rasterizerState.MultisampleEnable     = true;
        rasterizerState.AntialiasedLineEnable = true;

        DirectX9->m_Device->CreateRasterizerState(&rasterizerState , &DirectX9->m_RasterizerState);
        DirectX9->m_Device->RSSetState(DirectX9->m_RasterizerState);
    }

    // create depth stencil state
    {
        D3D10_DEPTH_STENCIL_DESC depthstencildesc = {};
        depthstencildesc.DepthEnable    = TRUE;
        depthstencildesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
        depthstencildesc.DepthFunc      = D3D10_COMPARISON_LESS;

        DirectX9->m_Device->CreateDepthStencilState(&depthstencildesc, &DirectX9->m_DepthStencilState);
    }

    return true;
}

void ApplicationRenderingBackend::begin_render(ApplicationRenderingBackendRenderingTarget* _Target)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();
    
    if(DirectX9 == nullptr)
        return;

    std::cout  << "ApplicationRenderingBackend::begin_render \n";

    //DirectX9->m_Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    FLOAT clearcolor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    DirectX9->m_Device->ClearRenderTargetView(DirectX9->m_RTV, clearcolor); // clear MSAA texture
    DirectX9->m_Device->ClearDepthStencilView(DirectX9->m_DSV, D3D10_CLEAR_DEPTH, 1.0f, 0);
    DirectX9->m_Device->IASetInputLayout(DirectX9->m_VertexLayout);

    // DirectX9->m_Device->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);
    // DirectX9->m_Device->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);

    DirectX9->m_Device->VSSetShader(DirectX9->m_VertexShader);

    // create viewport
    D3D10_VIEWPORT vp;
    {
        vp.Width    = 1024;
        vp.Height   = 1024;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        DirectX9->m_Device->RSSetViewports(1 , &vp);
    }

    DirectX9->m_Device->RSSetViewports(1, &vp);
    DirectX9->m_Device->RSSetState(DirectX9->m_RasterizerState);
    DirectX9->m_Device->PSSetShader(DirectX9->m_PixelShader);
    DirectX9->m_Device->OMSetRenderTargets(1, &DirectX9->m_RTVMSAA, DirectX9->m_DSV); // render to MSAA texture
    DirectX9->m_Device->OMSetDepthStencilState(DirectX9->m_DepthStencilState, 0);
    DirectX9->m_Device->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. no blending)
}

void ApplicationRenderingBackend::end_render()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX10> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX10>();

    if(DirectX9 == nullptr)
        return;

    DirectX9->m_Device->ResolveSubresource(DirectX9->framebuffer, 0, DirectX9->msaabuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB); // resolve the MSAA texture into framebuffer

    DirectX9->m_SwapChain->Present(1, 0);
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
        return false;

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
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    // std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    // if(DirectX9 != nullptr)
    //     DirectX9->m_Viewport = gs_2d_boxf(_Position, _Position + _Size);
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
    // std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    // if(DirectX9 != nullptr)
    //     DirectX9->m_ClearColor = _Color;
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

    // if(_Hints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Lines)
    //     DirectX9->m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    // else if(_Hints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Triangles)
    //     DirectX9->m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
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