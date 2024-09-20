#pragma comment(lib, "d3d9.lib")
#ifdef _DEBUG
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#endif

#include "Common.h"
#include "Mesh.h"
#include "AnimMesh.h"

#include <windows.h>
#include <string>
#include <d3dx9.h>

const std::string   TITLE = "simple anim mesh";
LPDIRECT3DDEVICE9   g_D3DDevice;
LPDIRECT3D9         g_D3D;
Mesh*               g_Mesh { nullptr };
AnimMesh*           g_AnimMesh = { nullptr };

const D3DXVECTOR3   UPWARD { 0.0f, 1.0f, 0.0f };
D3DXVECTOR3         g_eyePos { 0.0f, 3.0f, 0.0f };
D3DXVECTOR3         g_lookAtPos { 0.0f, 1.0f, 0.0f };
float               g_viewAngle { D3DX_PI / 4 };
float               g_radian { D3DX_PI * 3 / 4 };

D3DXMATRIX GetViewMatrix()
{
    D3DXMATRIX viewMatrix { };
    D3DXMatrixLookAtLH(&viewMatrix, &g_eyePos, &g_lookAtPos, &UPWARD);
    return viewMatrix;
}

D3DXMATRIX GetProjMatrix()
{
    D3DXMATRIX projection_matrix { };
    D3DXMatrixPerspectiveFovLH(&projection_matrix,
                               g_viewAngle,
                               static_cast<float>(1600) / 900,
                               0.1f,
                               500.0f);
    return projection_matrix;
}

void Update()
{
    g_radian += 1/100.f;
    g_eyePos.x = g_lookAtPos.x + std::sin(g_radian)*4;
    g_eyePos.z = g_lookAtPos.z + std::cos(g_radian)*4;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam)
{
    if (mes == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, mes, wParam, lParam);
}

void Init(const HINSTANCE& hInstance)
{
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX),
                        CS_HREDRAW | CS_VREDRAW,
                        WndProc,
                        0,
                        0,
                        hInstance,
                        NULL,
                        NULL,
                        (HBRUSH)(COLOR_WINDOW + 1),
                        NULL,
                        TITLE.c_str(), NULL
    };

    if (!RegisterClassEx(&wcex))
    {
        throw std::exception("");
    }

    HWND hWnd = CreateWindow(TITLE.c_str(),
                             TITLE.c_str(),
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT,
                             0,
                             CW_USEDEFAULT,
                             0,
                             NULL,
                             NULL,
                             hInstance,
                             NULL);

    if (hWnd == nullptr)
    {
        throw std::exception("");
    }

    if (!(g_D3D = Direct3DCreate9(D3D_SDK_VERSION)))
    {
        throw std::exception("");
    }

    D3DPRESENT_PARAMETERS d3dpp = { 0,
                                    0,
                                    D3DFMT_UNKNOWN,
                                    0,
                                    D3DMULTISAMPLE_NONE,
                                    0,
                                    D3DSWAPEFFECT_DISCARD,
                                    NULL,
                                    TRUE,
                                    TRUE,
                                    D3DFMT_D24S8,
                                    0,
                                    D3DPRESENT_RATE_DEFAULT,
                                    D3DPRESENT_INTERVAL_DEFAULT };

    LPDIRECT3DDEVICE9 D3DDevice = nullptr;
    if (FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT,
                                   D3DDEVTYPE_HAL,
                                   hWnd,
                                   D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                   &d3dpp,
                                   &D3DDevice)))
    {
        if (FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT,
                                       D3DDEVTYPE_HAL,
                                       hWnd,
                                       D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                       &d3dpp,
                                       &D3DDevice)))
        {
            g_D3D->Release();
            throw std::exception("");
        }
    }
    g_D3DDevice = D3DDevice;

    g_Mesh = new Mesh(g_D3DDevice,
                      "tiger.x",
                      D3DXVECTOR3(3, 1, 0),
                      D3DXVECTOR3(0, 0, 0),
                      1.0f);
    g_AnimMesh = new AnimMesh(g_D3DDevice,
                              "hoshiman.x",
                              D3DXVECTOR3(0, 0, 0),
                              D3DXVECTOR3(0, 0, 0),
                              0.5f);

    ShowWindow(hWnd, SW_SHOW);
}

void Finalize()
{
    SAFE_DELETE(g_Mesh);
    SAFE_DELETE(g_AnimMesh);
    SAFE_RELEASE(g_D3DDevice);
    SAFE_RELEASE(g_D3D);
}

int MainLoop()
{
    MSG msg;

    do
    {
        Sleep(1);
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            DispatchMessage(&msg);
        }

        Update();
        g_AnimMesh->Update();

        g_D3DDevice->Clear(0,
                           NULL,
                           D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                           D3DCOLOR_XRGB(40, 40, 80),
                           1.0f,
                           0);
        g_D3DDevice->BeginScene();

        g_Mesh->Render(GetViewMatrix(), GetProjMatrix());
        g_AnimMesh->Render(GetViewMatrix(), GetProjMatrix());

        g_D3DDevice->EndScene();
        g_D3DDevice->Present(NULL, NULL, NULL, NULL);

    } while (msg.message != WM_QUIT);

    return 0;
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE, _In_ LPSTR, _In_ int)
{
    Init(hInstance);
    MainLoop();
    Finalize();
    return 0;
}

