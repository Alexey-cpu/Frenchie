// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// D3D9
#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include <iostream>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationRenderingBackendDirectX9 : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendDirectX9(){}
            virtual ~ApplicationRenderingBackendDirectX9(){}

            LPDIRECT3D9                  g_pD3D                  = NULL; // D3D interface
            LPDIRECT3DDEVICE9            g_D3DDevice             = NULL; // rendering device
            LPDIRECT3DVERTEXBUFFER9      g_D3DVertexBuffer       = NULL; // vertex buffer
            UINT                         g_D3DVertexBufferSize   = 0;
            LPDIRECT3DINDEXBUFFER9       g_D3DIndexBuffer        = NULL; // index buffer
            UINT                         g_D3DIndexBufferSize    = 0;
            IDirect3DVertexDeclaration9* g_D3DVertexDeclaration  = NULL; // vertex layout
            bool                         g_D3DDeviceLost         = false;
            D3DPRESENT_PARAMETERS        g_D3DPresentParameters;
            HWND                         g_D3DContextWindow;

            void reset()
            {
                if(g_D3DVertexBuffer != NULL)
                    g_D3DVertexBuffer->Release();
                g_D3DVertexBuffer = NULL;

                if(g_D3DIndexBuffer != NULL)
                    g_D3DIndexBuffer->Release();
                g_D3DIndexBuffer = NULL;
            }
        };

        D3DMATRIX gs_convert_transform_from_opengl_to_directx(const gs_mat4f& _Matrix)
        {
            D3DMATRIX result;

            for (int i = 0; i < _Matrix.columns(); i++)
            {
                for (int j = 0; j < _Matrix.rows(); j++)
                {
                    result.m[i][j] = _Matrix[i][j];
                }
            }

            return result;
        }

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
    
    m_Api = std::make_shared<ApplicationRenderingBackendDirectX9>();
    
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    // Create the D3D object.
    if(NULL == (g_DirectX9->g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return false;

    // Set up the structure used to create the D3DDevice
    ZeroMemory(&g_DirectX9->g_D3DPresentParameters, sizeof(g_DirectX9->g_D3DPresentParameters));
    g_DirectX9->g_D3DPresentParameters.Windowed               = TRUE;
    g_DirectX9->g_D3DPresentParameters.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    g_DirectX9->g_D3DPresentParameters.BackBufferFormat       = D3DFMT_A8R8G8B8;
    g_DirectX9->g_D3DPresentParameters.EnableAutoDepthStencil = TRUE;
    g_DirectX9->g_D3DPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8; 

    // Create the D3DDevice
    if(FAILED(g_DirectX9->g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_DirectX9->g_D3DPresentParameters, &g_DirectX9->g_D3DDevice)))
        return false;

    // Declare mesh vertex
    D3DVERTEXELEMENT9 VertexColElements[] =
    {
        {0, sizeof(float) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, sizeof(float) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
        {0, sizeof(float) * 6, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, sizeof(float) * 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0},
        D3DDECL_END(),
    };
    
    if(FAILED(g_DirectX9->g_D3DDevice->CreateVertexDeclaration(VertexColElements, &g_DirectX9->g_D3DVertexDeclaration)))
        return false;

    return true;
}

void ApplicationRenderingBackend::quit()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(g_DirectX9 == nullptr)
        return;

    if(g_DirectX9->g_D3DVertexBuffer != NULL)
        g_DirectX9->g_D3DVertexBuffer->Release();

    if(g_DirectX9->g_D3DIndexBuffer != NULL)
        g_DirectX9->g_D3DIndexBuffer->Release();

    if(g_DirectX9->g_D3DDevice != NULL)
        g_DirectX9->g_D3DDevice->Release();

    if(g_DirectX9->g_pD3D != NULL)
        g_DirectX9->g_pD3D->Release();
}

ApplicationRenderingBackendTexture ApplicationRenderingBackend::construct_texture(
    const unsigned char*                               _RawBuffer,
    const int&                                         _Width,
    const int&                                         _Height,
    const ApplicationRenderingBackendTextureFormat&    _Format,
    const ApplicationRenderingBackendTextureWrapMode&  _Wrap,
    const ApplicationRenderingBackendTextureMinFilter& _MinFilter,
    const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter)
{
    (void)_RawBuffer;
    (void)_Width;
    (void)_Height;
    (void)_Format;
    (void)_Wrap;
    (void)_MinFilter;
    (void)_MaxFilter;

    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(g_DirectX9 == nullptr || g_DirectX9->g_D3DDevice == nullptr)
        return ApplicationRenderingBackendTexture();

    // create texture
    LPDIRECT3DTEXTURE9 pTexture = nullptr;
    g_DirectX9->g_D3DDevice->CreateTexture(_Width, _Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, nullptr);

    // lock texture rect
    D3DLOCKED_RECT lockedRect;
    pTexture->LockRect(0, &lockedRect, nullptr, 0);

    const int     height   = _Height;
    const int     width    = _Width;
    const int     channels = 4;
    const int     red      = 0;
    const int     green    = 1;
    const int     blue     = 2;
    const int     alpha    = 3;

    unsigned char* pDest = (unsigned char*)lockedRect.pBits;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char r = _RawBuffer[channels * (y * width + x) + red  ];
            unsigned char g = _RawBuffer[channels * (y * width + x) + green];
            unsigned char b = _RawBuffer[channels * (y * width + x) + blue ];
            unsigned char a = _RawBuffer[channels * (y * width + x) + alpha];

            pDest[channels * (y * width + x) + red  ] = b;
            pDest[channels * (y * width + x) + green] = g;
            pDest[channels * (y * width + x) + blue ] = r;
            pDest[channels * (y * width + x) + alpha] = a;
        }
    }

    // 4. Unlock
    pTexture->UnlockRect(0);

    return ApplicationRenderingBackendTexture(reinterpret_cast<uintptr_t>(pTexture), _Width, _Height, 1, _Format, _Wrap, _MinFilter, _MaxFilter);
}

void ApplicationRenderingBackend::destroy_texture(const ApplicationRenderingBackendTexture& _Texture)
{
    if(_Texture.is_null())
        return;

    reinterpret_cast<LPDIRECT3DTEXTURE9>(_Texture.Ptr)->Release();
    _Texture.Ptr = 0;
}

bool ApplicationRenderingBackend::begin_render(
    ApplicationRenderingBackendMeshVertex*            _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex& _VertexesCount,
    ApplicationRenderingBackendMeshVertexIndex*       _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex& _IndexesCount)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();
    
    if(g_DirectX9 == nullptr || g_DirectX9->g_D3DDevice == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0)
        return false;

    // adjust colors of mesh points
    for (int i = 0; i < _VertexesCount; i++)
    {
        gs_color color = _Vertexes[i].Color;
        _Vertexes[i].Color = D3DCOLOR_ARGB(gs_color_rgba_get_a(color), gs_color_rgba_get_r(color), gs_color_rgba_get_g(color), gs_color_rgba_get_b(color));
    }

    // handle lost D3D9 device
    if (g_DirectX9->g_D3DDeviceLost)
    {
        HRESULT hr = g_DirectX9->g_D3DDevice->TestCooperativeLevel();

        if (hr == D3DERR_DEVICELOST)
        {
            ::Sleep(10);
            return false;
        }

        if (hr == D3DERR_DEVICENOTRESET)
            g_DirectX9->reset();
        g_DirectX9->g_D3DDevice = false;
    }

    // manage buffers
    {
        if(g_DirectX9->g_D3DVertexBuffer == nullptr || g_DirectX9->g_D3DIndexBuffer == nullptr || g_DirectX9->g_D3DVertexBufferSize < _VertexesCount || g_DirectX9->g_D3DIndexBufferSize < _IndexesCount)
        {
            if(g_DirectX9->g_D3DVertexBuffer != NULL)
            {
                g_DirectX9->g_D3DVertexBuffer->Release();
                g_DirectX9->g_D3DVertexBuffer = NULL;
            }

            if(g_DirectX9->g_D3DIndexBuffer != NULL)
            {
                g_DirectX9->g_D3DIndexBuffer->Release();
                g_DirectX9->g_D3DIndexBuffer = NULL;
            }

            if(FAILED(g_DirectX9->g_D3DDevice->CreateVertexBuffer(sizeof(CUSTOMVERTEX) * _VertexesCount, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &g_DirectX9->g_D3DVertexBuffer, NULL)))
                return false;
            else
                g_DirectX9->g_D3DVertexBufferSize = _VertexesCount;

            if(FAILED(g_DirectX9->g_D3DDevice->CreateIndexBuffer(sizeof(CUSTOMINDEX) * _IndexesCount, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(CUSTOMINDEX) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_DirectX9->g_D3DIndexBuffer, nullptr)))
                return false;
            else
                g_DirectX9->g_D3DIndexBufferSize  = _IndexesCount;
        }
    }

    // write buffers
    {
        // write
        VOID* pVertices;
        if(FAILED(g_DirectX9->g_D3DVertexBuffer->Lock(0, sizeof(CUSTOMVERTEX) * _VertexesCount, (void**)&pVertices, 0)))
            return false;
        memcpy(pVertices, _Vertexes, sizeof(CUSTOMVERTEX) * _VertexesCount);
        g_DirectX9->g_D3DVertexBuffer->Unlock();

        VOID* pIndexes;
        if(FAILED(g_DirectX9->g_D3DIndexBuffer->Lock(0, sizeof(CUSTOMINDEX) * _IndexesCount, (void**)&pIndexes, 0)))
            return false;
        memcpy(pIndexes, _Indexes, sizeof(CUSTOMINDEX) * _IndexesCount);
        g_DirectX9->g_D3DIndexBuffer->Unlock();
    }

    if(g_DirectX9->g_D3DVertexBuffer == nullptr || g_DirectX9->g_D3DIndexBuffer == nullptr)
        return false;

    g_DirectX9->g_D3DDevice->SetPixelShader(nullptr);
    g_DirectX9->g_D3DDevice->SetVertexShader(nullptr);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

    // g_D3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    //g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // I DON'T KNOW HOW TO ENABLE CULLING HERE CORRECTLY ...

    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
    
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    // g_DirectX9->g_D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    // g_DirectX9->g_D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    // g_DirectX9->g_D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    // g_DirectX9->g_D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    // g_DirectX9->g_D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    // g_DirectX9->g_D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    // g_DirectX9->g_D3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    // g_DirectX9->g_D3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    // g_DirectX9->g_D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    // g_DirectX9->g_D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    // g_DirectX9->g_D3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    // g_DirectX9->g_D3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    // clear back buffer
    g_DirectX9->g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);

    if(SUCCEEDED(g_DirectX9->g_D3DDevice->BeginScene()))
    {
        g_DirectX9->g_D3DDevice->SetStreamSource(0, g_DirectX9->g_D3DVertexBuffer, 0, sizeof(CUSTOMVERTEX));
        g_DirectX9->g_D3DDevice->SetIndices(g_DirectX9->g_D3DIndexBuffer);
        g_DirectX9->g_D3DDevice->SetVertexDeclaration(g_DirectX9->g_D3DVertexDeclaration);
        return true;
    }

    return false;
}

void ApplicationRenderingBackend::render_mesh(
    const ApplicationRenderingBackendMeshVertex*                _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex&           _VertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshVertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshVertexesOffset,
    const ApplicationRenderingBackendMeshVertexIndex*           _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex&           _IndexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshIndexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshIndexesOffset,
    const ApplicationRenderingBackendTexture&                   _Texture,
    const gs_mat4f&                                             _MeshProjectionMatrix,
    const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderHints)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();
    
    if(g_DirectX9 == nullptr || g_DirectX9->g_D3DDevice == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0 || _MeshVertexesCount <= 0 || _MeshIndexesCount <= 0)
        return;

    D3DMATRIX mat_world      = {{{1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f}}};
    D3DMATRIX mat_camera     = {{{1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f}}};
    D3DMATRIX mat_projection = gs_convert_transform_from_opengl_to_directx(_MeshProjectionMatrix);

    if(!_Texture.is_null())
    {
        g_DirectX9->g_D3DDevice->SetTexture(0, reinterpret_cast<LPDIRECT3DTEXTURE9>(_Texture.Ptr));
    }

    g_DirectX9->g_D3DDevice->SetTransform(D3DTS_WORLD, &mat_world);
    g_DirectX9->g_D3DDevice->SetTransform(D3DTS_VIEW, &mat_camera);
    g_DirectX9->g_D3DDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
    g_DirectX9->g_D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _IndexesCount, _MeshIndexesOffset, (_MeshIndexesCount - _MeshIndexesOffset) / 3);

    g_DirectX9->g_D3DDevice->SetTexture(0, NULL);    // Unbind to prevent leaks
}

void ApplicationRenderingBackend::end_render()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(g_DirectX9 == nullptr)
        return;

    g_DirectX9->g_D3DDevice->EndScene();

    // Present the backbuffer contents to the display
    HRESULT result = g_DirectX9->g_D3DDevice->Present(NULL, NULL, NULL, NULL);

    if (result == D3DERR_DEVICELOST)
        g_DirectX9->g_D3DDeviceLost = true;
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(g_DirectX9 == nullptr)
        return;

    // adjust backbuffer
    g_DirectX9->g_D3DPresentParameters.BackBufferWidth  = _Size.x;
    g_DirectX9->g_D3DPresentParameters.BackBufferHeight = _Size.y;
    g_DirectX9->g_D3DDevice->Reset(&g_DirectX9->g_D3DPresentParameters);

    // adjust viewport
    D3DVIEWPORT9 vp;
    vp.X      = _Position.x;
    vp.Y      = _Position.y;
    vp.Width  = _Size.x;
    vp.Height = _Size.y;
    vp.MinZ   = 0.0f;
    vp.MaxZ   = 1.0f;
    g_DirectX9->g_D3DDevice->SetViewport(&vp);

    // reset rendering device
    g_DirectX9->reset();
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
}

void ApplicationRenderingBackend::scissor_box(const gs_2dboxf& _ClippingRect)
{
}

// camera and view projection API
ApplicationRenderingBackend::Projections ApplicationRenderingBackend::calculate_2d_camera_view_and_projection(
    const gs_vec2f& _CameraWorldPosition,
    const gs_vec2f& _CameraResolution,
    const float&    _CameraRotationAngle,
    const float&    _CameraNearPlanePosition,
    const float&    _CameraFarPlanePosition)
{
    const gs_vec3f& _CameraWorldUpAxisDirection    = gs_vec3f(0.f, 1.f, 0.f);
    const gs_vec3f& _CameraWorldFrontAxisDirection = gs_vec3f(0.f, 0.f, +1.f);

    // compute projection matrix
    float left   = -_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
    float right  = +_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
    float bottom = +_CameraResolution.y * 0.5f + _CameraWorldPosition.y;
    float top    = -_CameraResolution.y * 0.5f + _CameraWorldPosition.y;

    // camera orientation
    gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(_CameraWorldFrontAxisDirection);
    gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, _CameraWorldUpAxisDirection));
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

gs_mat4f ApplicationRenderingBackend::calculate_2d_transform_matrix(const float& _Depth, const gs_vec2f& _Position, const float& _Rotation, const gs_vec2f& _Scale)
{
    gs_mat4f matrix(1.f);

    return gs_matrix_translate(matrix, gs_vec3f(_Position, -_Depth)) *
            gs_matrix_rotate(matrix, gs_to_radians(_Rotation), gs_vec3f(0.f, 0.f, 1.f)) * 
            gs_matrix_scale(matrix, gs_vec3f(_Scale, 1.f));
}

gs_vec2f ApplicationRenderingBackend::convert_to_NDC(const gs_vec2f& _Position, const gs_vec2f& _Screen)
{
    return gs_vec2f((2.0f * _Position.x) / _Screen.x - 1.0f, 1.0f - (2.0f * _Position.y) / _Screen.y);
}