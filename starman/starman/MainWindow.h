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
#include "Sprite.h"
#include "AnimMesh.h"
#include "SeqTitle.h"
#include "SeqBattle.h"

class MainWindow
{
public:
    explicit MainWindow(const HINSTANCE&);
    ~MainWindow();
    int MainLoop();

private:
    const std::string TITLE = "ƒzƒVƒ}ƒ“";
    MSG m_msg;
    HWND m_hWnd;
    LPDIRECT3D9 m_D3D;
    D3DLIGHT9 light;
    ID3DXBuffer* pMaterials;
    DWORD NumMaterials;
    ID3DXMesh* pMesh;
    ID3DXBuffer* pAX_Materials;
    DWORD AX_NumMaterials;
    ID3DXMesh* pAX_Mesh;

    Mesh* m_Mesh1 { nullptr };
    LPDIRECTINPUT8 m_directInput { nullptr };
    Sprite* m_sprite { nullptr };

    eSequence m_sequence { eSequence::TITLE };
    SeqTitle* m_seqTitle { nullptr };
    SeqBattle* m_seqBattle { nullptr };
};

