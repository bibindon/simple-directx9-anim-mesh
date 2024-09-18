#pragma once

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>
#include  <string>
#include "Mesh.h"
#include "AnimMesh.h"
#include "SeqBattle.h"

class MainWindow
{
public:
    explicit MainWindow(const HINSTANCE&);
    ~MainWindow();
    int MainLoop();

private:
    const std::string TITLE = "simple anim mesh";
    MSG m_msg;
    HWND m_hWnd;
    LPDIRECT3D9 m_D3D;

    Mesh* m_Mesh1 { nullptr };
    LPDIRECTINPUT8 m_directInput { nullptr };

    SeqBattle* m_seqBattle { nullptr };
};

