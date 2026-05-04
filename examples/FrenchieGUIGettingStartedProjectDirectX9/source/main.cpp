#include <FrenchieApplicationLayerRenderingQueue.hpp>

//-----------------------------------------------------------------------------
// File: Vertices.cpp
//
// Desc: In this tutorial, we are rendering some vertices. This introduces the
//       concept of the vertex buffer, a Direct3D object used to store
//       vertices. Vertices can be defined any way we want by defining a
//       custom structure and a custom FVF (flexible vertex format). In this
//       tutorial, we are using vertices that are transformed (meaning they
//       are already in 2D window coordinates) and lit (meaning we are not
//       using Direct3D lighting, but are supplying our own colors).
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include <tchar.h>
#include <iostream>

//-----------------------------------------------------------------------------
// D3D9 API State
//-----------------------------------------------------------------------------
struct ApplicationRenderingBackendDirectX9
{
    ApplicationRenderingBackendDirectX9(){}
    virtual ~ApplicationRenderingBackendDirectX9(){}

    LPDIRECT3D9                  g_pD3D                  = NULL; // D3D interface
    LPDIRECT3DDEVICE9            g_D3DDevice             = NULL; // rendering device
    LPDIRECT3DVERTEXBUFFER9      g_D3DVertexBuffer       = NULL; // vertex buffer
    LPDIRECT3DINDEXBUFFER9       g_D3DIndexBuffer        = NULL; // index buffer
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

std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = std::make_shared<ApplicationRenderingBackendDirectX9>();

//-----------------------------------------------------------------------------
// Rendering Queue State
//-----------------------------------------------------------------------------
typedef Frenchie::Application::ApplicationRenderingBackendMeshVertex      CUSTOMVERTEX;
typedef Frenchie::Application::ApplicationRenderingBackendMeshVertexIndex CUSTOMINDEX;

std::vector<CUSTOMVERTEX> g_Vertices;
std::vector<CUSTOMINDEX>  g_Indexes;
std::vector<CUSTOMINDEX>  g_Offsets;
std::vector<CUSTOMINDEX>  g_Counts;
std::vector<gs_mat4f>     g_Transforms;

//-----------------------------------------------------------------------------
// Auxiliary functions
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if(NULL == (g_DirectX9->g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return E_FAIL;

    // Set up the structure used to create the D3DDevice
    ZeroMemory(&g_DirectX9->g_D3DPresentParameters, sizeof(g_DirectX9->g_D3DPresentParameters));
    g_DirectX9->g_D3DPresentParameters.Windowed               = TRUE;
    g_DirectX9->g_D3DPresentParameters.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    g_DirectX9->g_D3DPresentParameters.BackBufferFormat       = D3DFMT_A8R8G8B8;
    g_DirectX9->g_D3DPresentParameters.EnableAutoDepthStencil = TRUE;
    g_DirectX9->g_D3DPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8; 

    // Create the D3DDevice
    if(FAILED(g_DirectX9->g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_DirectX9->g_D3DPresentParameters, &g_DirectX9->g_D3DDevice)))
        return E_FAIL;

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
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitVB()
// Desc: Creates a vertex buffer and fills it with our vertices. The vertex
//       buffer is basically just a chuck of memory that holds vertices. After
//       creating it, we must Lock()/Unlock() it to fill it. For indices, D3D
//       also uses index buffers. The special thing about vertex and index
//       buffers is that they can be created in device memory, allowing some
//       cards to process them in hardware, resulting in a dramatic
//       performance gain.
//-----------------------------------------------------------------------------
HRESULT InitVB()
{
    D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 0, 0);

    // Initialize three vertices for rendering a triangle
    g_Vertices =
    {
        // triangle 1
        { {150.0f,  50.0f, 0.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 255, 0, 0)},
        { {250.0f, 250.0f, 0.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 255, 0, 0)},
        { {50.0f, 250.0f, 0.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 255, 0, 0)},

        // triangle 2
        { {250.0f,  150.0f, 0.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 0, 255, 0)},
        { {250.0f, 350.0f, 0.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 0, 255, 0)},
        { {150.0f, 350.0f, 0.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 0, 255, 0)},
    };

    g_Indexes = 
    {
        // triangle 1
        0, 1, 2,
        
        // triangle 2
        3, 4, 5
    };

    g_Offsets =
    {
        0,
        3
    };

    g_Counts =
    {
        3,
        3
    };
    
    g_Transforms =
    {
        gs_matrix_translate(gs_mat4f(1.f), gs_vec3f(0.f, 0.f, 0.1f)),
        gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(5.f), gs_vec3f(0.f, 0.f, 1.f)) * gs_matrix_translate(gs_mat4f(1.f), gs_vec3f(0.f, 0.f, 0.5f)),
    };

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if(g_DirectX9->g_D3DVertexBuffer != NULL)
        g_DirectX9->g_D3DVertexBuffer->Release();

    if(g_DirectX9->g_D3DIndexBuffer != NULL)
        g_DirectX9->g_D3DIndexBuffer->Release();

    if(g_DirectX9->g_D3DDevice != NULL)
        g_DirectX9->g_D3DDevice->Release();

    if(g_DirectX9->g_pD3D != NULL)
        g_DirectX9->g_pD3D->Release();
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------

static bool Started = false;

VOID Render()
{
    if(g_DirectX9->g_D3DDevice == nullptr)
        return;

    // handle lost D3D9 device
    if (g_DirectX9->g_D3DDeviceLost)
    {
        HRESULT hr = g_DirectX9->g_D3DDevice->TestCooperativeLevel();

        if (hr == D3DERR_DEVICELOST)
        {
            ::Sleep(10);
            return;
        }

        if (hr == D3DERR_DEVICENOTRESET)
            g_DirectX9->reset();
        g_DirectX9->g_D3DDevice = false;
    }

    // handle window resize (setup viewport)
    RECT clientRect;

    if (GetClientRect(g_DirectX9->g_D3DContextWindow, &clientRect))
    {
        // adjust backbuffer
        g_DirectX9->g_D3DPresentParameters.BackBufferWidth  = clientRect.right - clientRect.left;
        g_DirectX9->g_D3DPresentParameters.BackBufferHeight = clientRect.bottom - clientRect.top;
        g_DirectX9->g_D3DDevice->Reset(&g_DirectX9->g_D3DPresentParameters);

        // adjust viewport
        D3DVIEWPORT9 vp;
        vp.X      = 0;
        vp.Y      = 0;
        vp.Width  = clientRect.right - clientRect.left;
        vp.Height = clientRect.bottom - clientRect.top;
        vp.MinZ   = 0.0f;
        vp.MaxZ   = 1.0f;
        g_DirectX9->g_D3DDevice->SetViewport(&vp);

        g_DirectX9->reset();
    }

    // manage vertex buffer
    {
        // reallocate if needed
        D3DVERTEXBUFFER_DESC vertexBufferDesc;
        UINT                 vertexBufferSize = 0;
        if (g_DirectX9->g_D3DVertexBuffer != nullptr && SUCCEEDED(g_DirectX9->g_D3DVertexBuffer->GetDesc(&vertexBufferDesc)))
            vertexBufferSize = vertexBufferDesc.Size / sizeof(CUSTOMVERTEX);

        if(vertexBufferSize < g_Vertices.size())
        {
            if(FAILED(g_DirectX9->g_D3DDevice->CreateVertexBuffer(sizeof(CUSTOMVERTEX) * g_Vertices.size(), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &g_DirectX9->g_D3DVertexBuffer, NULL)))
                return;
        }

        // write
        VOID* pVertices;
        if(FAILED(g_DirectX9->g_D3DVertexBuffer->Lock(0, sizeof(CUSTOMVERTEX) * g_Vertices.size(), (void**)&pVertices, 0)))
            return;

        memcpy(pVertices, &g_Vertices[0], sizeof(CUSTOMVERTEX) * g_Vertices.size());
        g_DirectX9->g_D3DVertexBuffer->Unlock();
    }

    // manage index buffer
    {
        // reallocate if needed
        D3DINDEXBUFFER_DESC indexBufferDesc;
        UINT                indexBufferSize = 0;
        if (g_DirectX9->g_D3DIndexBuffer != nullptr && SUCCEEDED(g_DirectX9->g_D3DIndexBuffer->GetDesc(&indexBufferDesc)))
            indexBufferSize = indexBufferDesc.Size / sizeof(CUSTOMVERTEX);

        if(indexBufferSize < g_Indexes.size())
        {
            if(FAILED(g_DirectX9->g_D3DDevice->CreateIndexBuffer(sizeof(CUSTOMVERTEX) * g_Indexes.size(), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(CUSTOMVERTEX) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_DirectX9->g_D3DIndexBuffer, nullptr)))
                return;
        }

        // write
        VOID* pIndexes;
        if(FAILED(g_DirectX9->g_D3DIndexBuffer->Lock( 0, sizeof(CUSTOMVERTEX) * g_Indexes.size(), (void**)&pIndexes, 0)))
            return;
        memcpy(pIndexes, &g_Indexes[0], sizeof(CUSTOMVERTEX) * g_Indexes.size());
        g_DirectX9->g_D3DIndexBuffer->Unlock();
    }

    if(g_DirectX9->g_D3DVertexBuffer == nullptr || g_DirectX9->g_D3DIndexBuffer == nullptr)
        return;

    g_DirectX9->g_D3DDevice->SetPixelShader(nullptr);
    g_DirectX9->g_D3DDevice->SetVertexShader(nullptr);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

    // g_D3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

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
    g_DirectX9->g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);

    // Render scene into a back buffer
    if(SUCCEEDED(g_DirectX9->g_D3DDevice->BeginScene()))
    {
        g_DirectX9->g_D3DDevice->SetStreamSource(0, g_DirectX9->g_D3DVertexBuffer, 0, sizeof( CUSTOMVERTEX ) );
        g_DirectX9->g_D3DDevice->SetIndices(g_DirectX9->g_D3DIndexBuffer);
        g_DirectX9->g_D3DDevice->SetVertexDeclaration(g_DirectX9->g_D3DVertexDeclaration);

        for (int i = 0; i < (int)g_Offsets.size(); i++)        
        {
            // adjust projectrion matrix
            float L = 0;
            float R = (clientRect.right - clientRect.left);
            float T = 0;
            float B = (clientRect.bottom - clientRect.top);
            float W = clientRect.right - clientRect.left;
            float H = clientRect.bottom - clientRect.top;

            auto camera = Frenchie::Application::ApplicationRenderingBackend::calculate_2d_camera_view_and_projection(
                gs_vec2f((R - L) * 0.5f, (B - T) * 0.5f),
                gs_vec2f((R - L), (B - T)),
                0.f,
                -10000.f,
                +10000.f);

            D3DMATRIX mat_world      = gs_convert_transform_from_opengl_to_directx(g_Transforms[i]);
            D3DMATRIX mat_camera     = gs_convert_transform_from_opengl_to_directx(camera.CameraView);//{{{ 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f}}};
            D3DMATRIX mat_projection = gs_convert_transform_from_opengl_to_directx(camera.Projection);//gs_convert_transform_from_opengl_to_directx(gs_matrix_ortho(L, R, B, T, -1.f, +1.f, false, false));

            g_DirectX9->g_D3DDevice->SetTransform(D3DTS_WORLD, &mat_world);
            g_DirectX9->g_D3DDevice->SetTransform(D3DTS_VIEW, &mat_camera);
            g_DirectX9->g_D3DDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);

            g_DirectX9->g_D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (UINT)g_Vertices.size(), g_Offsets[i], (UINT)(g_Counts[i] / 3));
        }

        g_DirectX9->g_D3DDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    HRESULT result = g_DirectX9->g_D3DDevice->Present(NULL, NULL, NULL, NULL);

    if (result == D3DERR_DEVICELOST)
        g_DirectX9->g_D3DDeviceLost = true;

    Started =  true;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT main(int argc, char *argv[])
{
    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
        TEXT("D3D Tutorial"), NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    g_DirectX9->g_D3DContextWindow = CreateWindow(
        TEXT("D3D Tutorial"),
        TEXT("D3D Tutorial 02: Vertices"),
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        300,
        300,
        NULL,
        NULL,
        wc.hInstance,
        NULL);

    // Initialize Direct3D
    if(SUCCEEDED(InitD3D(g_DirectX9->g_D3DContextWindow)))
    {
        // Create the vertex buffer
        if(SUCCEEDED(InitVB()))
        {
            // Show the window
            ShowWindow(g_DirectX9->g_D3DContextWindow, SW_SHOWDEFAULT);
            UpdateWindow(g_DirectX9->g_D3DContextWindow);

            // Enter the message loop
            MSG msg;
            ZeroMemory(&msg, sizeof(msg));
            while( msg.message != WM_QUIT )
            {
                if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass(TEXT("D3D Tutorial"), wc.hInstance);
    return 0;
}