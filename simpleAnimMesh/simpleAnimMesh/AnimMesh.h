#pragma once

#include "AnimMeshAlloc.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class AnimMesh
{

public:

    AnimMesh(const LPDIRECT3DDEVICE9 D3DDevice,
             const std::string&,
             const D3DXVECTOR3&,
             const D3DXVECTOR3&,
             const float&);

    ~AnimMesh();
    void Render(const D3DXMATRIX& view, const D3DXMATRIX& proj);
    void SetPos(const D3DXVECTOR3& pos);
    void SetRotate(const D3DXVECTOR3& rotate);

    void Update();
    void SetAnim(const std::string& animationSet);
    void SetDefaultAnim(const std::string& animationName);
    void SetAnimConfig(const std::string& animationName,
                       const bool loop,
                       const float duration);
    bool isPlaying();
private:

    void UpdateFrameMatrix(const LPD3DXFRAME, const LPD3DXMATRIX);
    void RenderFrame(const LPD3DXFRAME);
    void RenderMeshContainer(const LPD3DXMESHCONTAINER, const LPD3DXFRAME);
    void ReleaseMeshAllocator(const LPD3DXFRAME);

    const std::string SHADER_FILENAME = "animMeshShader.fx";
    std::string m_meshName = "";

    LPD3DXEFFECT m_D3DEffect = nullptr;

    AnimMeshAllocator* m_allocator = nullptr;
    D3DXFRAME* m_frameRoot = nullptr;

    D3DXMATRIX m_viewMatrix { };
    D3DXMATRIX m_projMatrix { };

    D3DXVECTOR3 m_centerPos { 0.f, 0.f, 0.f };
    D3DXVECTOR3 m_position { 0.f, 0.f, 0.f };
    D3DXVECTOR3 m_rotation { 0.f, 0.f, 0.f };
    float m_scale = 1.0f;

    //-------------------------------------------------------
    // LPD3DXANIMATIONCONTROLLERŠÖ˜A
    //-------------------------------------------------------
    bool m_isPlaying = false;
    std::vector<LPD3DXANIMATIONSET> m_animSets;
    LPD3DXANIMATIONCONTROLLER m_D3DAnimController = nullptr;
    std::string m_defaultAnim = "";
    float m_animTime = 0.f;
    std::string m_currentAnim = "";

    struct AnimConfig
    {
        bool loop = true;
        float duration = 1.0f;
    };
    std::unordered_map<std::string, AnimConfig> m_animConfigMap;
};

