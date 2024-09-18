#pragma once

#include <d3d9.h>
#include "Common.h"
#include "Sprite.h"
#include "Mesh.h"
#include "AnimMesh.h"
#include "Player.h"

class SeqBattle
{
public:
    SeqBattle();
    ~SeqBattle();
    void Update(eSequence* sequence);
    void Render();
private:
    Player* m_player { nullptr };
    Mesh* m_mesh1 { nullptr };
//    AnimMesh* m_AnimMesh1 { nullptr };
};

