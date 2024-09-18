#pragma once

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include  <string>
#include "Mesh.h"
#include "AnimMesh.h"

class MainWindow
{
public:
    explicit MainWindow(const HINSTANCE&);
    ~MainWindow();
    int MainLoop();

private:
    const std::string TITLE = "simple anim mesh";
    HWND m_hWnd;
    LPDIRECT3D9 m_D3D;
    Mesh* m_Mesh1 { nullptr };
    AnimMesh* m_AnimMesh2 = { nullptr };
};

