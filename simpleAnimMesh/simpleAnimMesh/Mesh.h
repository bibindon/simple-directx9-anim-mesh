#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include <memory>

class Mesh
{
public:
    Mesh(
        const std::string&,
        const D3DXVECTOR3&,
        const D3DXVECTOR3&,
        const float&);
    ~Mesh();

    void Render(const D3DXMATRIX& view, const D3DXMATRIX& proj);

private:
    const std::string SHADER_FILENAME { "mesh_shader.fx" };
    LPD3DXMESH m_D3DMesh { nullptr };

    LPD3DXEFFECT m_D3DEffect { nullptr };

    D3DXVECTOR3 m_pos { };
    D3DXVECTOR3 m_rotate { };

    DWORD m_materialCount { 0 };
    std::vector<D3DCOLORVALUE> m_vecColor { };
    std::vector<LPDIRECT3DTEXTURE9> m_vecTexture { };
    D3DXVECTOR3 m_centerPos { 0.0f, 0.0f, 0.0f };
    float m_radius { 0.0f };
    float m_scale { 0.0f };
    std::string m_meshName { "" };
};


