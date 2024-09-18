#include "SeqTitle.h"
#include "KeyBoard.h"
#include "Mouse.h"
#include "Common.h"
#include "SharedObj.h"

SeqTitle::SeqTitle()
{
    m_sprite1 = new Sprite("res\\image\\title01.png");
    m_sprite2 = new Sprite("res\\image\\title02.png");
    m_sprite3 = new Sprite("res\\image\\title03.png");
    m_sprite3->SetFill(true);
    m_spriteCursor = new Sprite("res\\image\\cursor.png");
}

SeqTitle::~SeqTitle()
{
    SAFE_DELETE(m_sprite1);
    SAFE_DELETE(m_sprite2);
    SAFE_DELETE(m_sprite3);
    SAFE_DELETE(m_spriteCursor);
}

void SeqTitle::Update(eSequence* sequence)
{
    if (KeyBoard::IsDown(DIK_UP))
    {

    }
    else if (KeyBoard::IsDown(DIK_DOWN))
    {

    }
    else if (KeyBoard::IsDown(DIK_RETURN))
    {
        *sequence = eSequence::BATTLE;
    }
}

void SeqTitle::Render()
{
    D3DXVECTOR3 pos4 { 0.0f, 0.0f, 0.0f };
    m_sprite3->Render(pos4);

    D3DXVECTOR3 pos1 { 0.0f, 0.0f, 0.0f };
    m_sprite1->Render(pos1);

    D3DXVECTOR3 pos2 { 800.0f, 200.0f, 0.0f };
    m_sprite2->Render(pos2);

    D3DXVECTOR3 pos3 { 900.0f, 220.0f, 0.0f };
    m_spriteCursor->Render(pos3);
}
