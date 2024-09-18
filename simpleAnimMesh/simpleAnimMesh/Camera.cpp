#include "Camera.h"
#include <cmath>

const D3DXVECTOR3 Camera::UPWARD { 0.0f, 1.0f, 0.0f };
D3DXVECTOR3 Camera::m_eyePos { 0.0f, 3.0f, -2.0f };
D3DXVECTOR3 Camera::m_lookAtPos { 0.0f, 0.0f, 0.0f };
float Camera::m_viewAngle { D3DX_PI / 4 };
float Camera::m_radian { D3DX_PI*3 / 4 };

D3DXMATRIX Camera::GetViewMatrix()
{
    D3DXMATRIX viewMatrix { };
    D3DXMatrixLookAtLH(&viewMatrix,
        &m_eyePos,
        &m_lookAtPos,
        &UPWARD);
    return viewMatrix;
}

D3DXMATRIX Camera::GetProjMatrix()
{
    D3DXMATRIX projection_matrix { };
    D3DXMatrixPerspectiveFovLH(
        &projection_matrix,
        m_viewAngle,
        static_cast<float>(1920) / 1080, /* TODO */
        0.1f,
        500.0f);
    return projection_matrix;
}

void Camera::Update()
{
    m_radian += 1/100.f;
    m_eyePos.x = m_lookAtPos.x + std::sin(m_radian)*10;
    m_eyePos.z = m_lookAtPos.z + std::cos(m_radian)*10;
}
