#include "SeqBattle.h"
#include "Common.h"
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>
#include "KeyBoard.h"
#include "Camera.h"

SeqBattle::SeqBattle()
{
    D3DXVECTOR3 b = D3DXVECTOR3(0, -10, 0);
    D3DXVECTOR3 c = D3DXVECTOR3(0, 0, 0);
    m_mesh1 = new Mesh("res\\model\\cube2\\cube2.x", b, c, 10.0f);
    b.y = 0.f;
    m_player = new Player();
}

SeqBattle::~SeqBattle()
{
}

void SeqBattle::Update()
{
    D3DXVECTOR3 pos = m_player->GetPos();
    float radian = Camera::GetRadian();
    if (KeyBoard::IsHold(DIK_W))
    {
        pos.x += std::sin(radian+D3DX_PI)/10;
        pos.z += std::sin(radian+D3DX_PI*3/2)/10;
    }
    if (KeyBoard::IsHold(DIK_A))
    {
        pos.x += std::sin(radian+D3DX_PI/2)/10;
        pos.z += std::sin(radian+D3DX_PI)/10;
    }
    if (KeyBoard::IsHold(DIK_S))
    {
        pos.x += std::sin(radian)/10;
        pos.z += std::sin(radian+D3DX_PI/2)/10;
    }
    if (KeyBoard::IsHold(DIK_D))
    {
        pos.x += std::sin(radian+D3DX_PI*3/2)/10;
        pos.z += std::sin(radian)/10;
    }

    m_player->SetPos(pos);
    Camera::SetPos(pos);
}

void SeqBattle::Render()
{
    m_mesh1->Render();
    m_player->Render();
}
