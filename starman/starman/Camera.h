#pragma once
#include <d3dx9.h>
class Camera
{
public:
    static D3DXMATRIX GetViewMatrix();
    static D3DXMATRIX GetProjMatrix();
    static void SetPos(const D3DXVECTOR3& lookAtPos);
    static float GetRadian();
    static void Update();

private:

    const static D3DXVECTOR3 UPWARD;
    static D3DXVECTOR3 m_eyePos;
    static D3DXVECTOR3 m_lookAtPos;
    static float m_viewAngle;
    static float m_radian;
};

