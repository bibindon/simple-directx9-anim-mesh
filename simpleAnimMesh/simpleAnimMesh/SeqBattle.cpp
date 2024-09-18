#include "SeqBattle.h"
#include "Common.h"
#include "Camera.h"

SeqBattle::SeqBattle()
{
    D3DXVECTOR3 b = D3DXVECTOR3(0, -10, 0);
    D3DXVECTOR3 c = D3DXVECTOR3(0, 0, 0);
    m_mesh1 = new Mesh("cube2.x", b, c, 10.0f);
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

    m_player->SetPos(pos);
    Camera::SetPos(pos);
}

void SeqBattle::Render()
{
    m_mesh1->Render();
    m_player->Render();
}
