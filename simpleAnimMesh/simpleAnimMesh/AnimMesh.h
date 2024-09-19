// TODO Remove smart pointer
#pragma once

#include "AnimMeshAlloc.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// TODO Remove this class
class AnimController
{
public:
    AnimController(LPD3DXANIMATIONCONTROLLER controller);
    void SetAnim(const std::string& animation_set);
    void Update();
    void SetDefaultAnim(const std::string& animation_name);
    void SetAnimConfig(
        const std::string& animation_name,
        const bool& loop,
        const float& duration);
    bool is_playing();

private:
    bool m_isPlaying { false };
    std::vector<LPD3DXANIMATIONSET> m_animSets;
    LPD3DXANIMATIONCONTROLLER m_D3DAnimController { nullptr };
    std::string m_defaultAnim;
    float m_animTime;
    std::string m_currentAnim;

    struct AnimConfig
    {
        bool loop { true };
        float duration { 1.0f };
    };
    std::unordered_map<std::string, AnimConfig> m_animConfigMap;
};

class AnimMesh
{
public:

    AnimMesh(
        const LPDIRECT3DDEVICE9 D3DDevice,
        const std::string&,
        const D3DXVECTOR3&,
        const D3DXVECTOR3&,
        const float&);
    ~AnimMesh();

    void Render(const D3DXMATRIX& view, const D3DXMATRIX& proj);
    void SetPos(const D3DXVECTOR3& pos);
    void SetRotate(const D3DXVECTOR3& rotate);

private:

    void UpdateFrameMatrix(const LPD3DXFRAME, const LPD3DXMATRIX);
    void RenderFrame(const LPD3DXFRAME);
    void RenderMeshContainer(const LPD3DXMESHCONTAINER, const LPD3DXFRAME);
    void ReleaseMeshAllocator(const LPD3DXFRAME);

    const std::string SHADER_FILENAME { "animation_mesh_shader.fx" };
    AnimMeshAllocator* m_allocator;
    D3DXFRAME* m_frameRoot;
    D3DXMATRIX m_viewMatrix;
    D3DXMATRIX m_projMatrix;

    D3DXVECTOR3 m_centerPos;

    LPD3DXEFFECT m_D3DEffect { nullptr };

    D3DXVECTOR3 m_position;
    D3DXVECTOR3 m_rotation;
    float m_scale { 1.0f };
    std::string m_meshName;

    AnimController* m_animController;
};

