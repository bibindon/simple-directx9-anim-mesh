#include "AnimMesh.h"

#include "AnimMeshAlloc.h"
#include "Common.h"

AnimMesh::AnimMesh(const LPDIRECT3DDEVICE9 D3DDevice,
                   const std::string& xFilename,
                   const D3DXVECTOR3& position,
                   const D3DXVECTOR3& rotation,
                   const float& scale)
    : m_allocator(new AnimMeshAllocator(xFilename))
    , m_position(position)
    , m_rotation(rotation)
    , m_scale(scale)
{
    HRESULT result = D3DXCreateEffectFromFile(D3DDevice,
                                              SHADER_FILENAME.c_str(),
                                              nullptr,
                                              nullptr,
                                              0,
                                              nullptr,
                                              &m_D3DEffect,
                                              nullptr);
    if (FAILED(result))
    {
        throw std::exception("Failed to create an effect file.");
    }

    result = D3DXLoadMeshHierarchyFromX(xFilename.c_str(),
                                        D3DXMESH_MANAGED,
                                        D3DDevice,
                                        m_allocator,
                                        nullptr,
                                        &m_frameRoot,
                                        &m_D3DAnimController);

    if (FAILED(result))
    {
        throw std::exception("Failed to load a x-file.");
    }
    DWORD animationCount = m_D3DAnimController->GetNumAnimationSets();
    std::vector<LPD3DXANIMATIONSET> animationSets;

    for (DWORD i = 0; i < animationCount; ++i)
    {
        LPD3DXANIMATIONSET tempAnimationSet = nullptr;
        m_D3DAnimController->GetAnimationSet(i, &tempAnimationSet);
        m_animSets.push_back(tempAnimationSet);
    }
}

AnimMesh::~AnimMesh()
{
    for (std::size_t i = 0; i < m_animSets.size(); ++i)
    {
        SAFE_RELEASE(m_animSets.at(i));
    }
    SAFE_RELEASE(m_D3DAnimController);

    ReleaseMeshAllocator(m_frameRoot);
    SAFE_DELETE(m_allocator);
    SAFE_RELEASE(m_D3DEffect);
}

void AnimMesh::Render(const D3DXMATRIX& view, const D3DXMATRIX& proj)
{
    m_viewMatrix = view;
    m_projMatrix = proj;

    D3DXMATRIX matWorld { };
    D3DXMatrixIdentity(&matWorld);

    D3DXMATRIX mat { };

    D3DXMatrixTranslation(&mat, -m_centerPos.x, -m_centerPos.y, -m_centerPos.z);
    matWorld *= mat;

    D3DXMatrixScaling(&mat, m_scale, m_scale, m_scale);
    matWorld *= mat;

    D3DXMatrixRotationYawPitchRoll(&mat, m_rotation.x, m_rotation.y, m_rotation.z);
    matWorld *= mat;

    D3DXMatrixTranslation(&mat, m_position.x, m_position.y, m_position.z);
    matWorld *= mat;

    UpdateFrameMatrix(m_frameRoot, &matWorld);
    RenderFrame(m_frameRoot);
}

void AnimMesh::SetPos(const D3DXVECTOR3& pos)
{
    m_position = pos;
}

void AnimMesh::SetRotate(const D3DXVECTOR3& rotate)
{
    m_rotation = rotate;
}

void AnimMesh::UpdateFrameMatrix(const LPD3DXFRAME frameBase, const LPD3DXMATRIX parentMatrix)
{
    AnimMeshFrame* frame = static_cast<AnimMeshFrame*>(frameBase);
    if (parentMatrix != nullptr)
    {
        frame->m_combinedMatrix = frame->TransformationMatrix * (*parentMatrix);
    }
    else
    {
        frame->m_combinedMatrix = frame->TransformationMatrix;
    }

    if (frame->pFrameSibling != nullptr)
    {
        UpdateFrameMatrix(frame->pFrameSibling, parentMatrix);
    }

    if (frame->pFrameFirstChild != nullptr)
    {
        UpdateFrameMatrix(frame->pFrameFirstChild, &frame->m_combinedMatrix);
    }
}

void AnimMesh::RenderFrame(const LPD3DXFRAME frame)
{
    LPD3DXMESHCONTAINER meshContainer = frame->pMeshContainer;
    while (meshContainer != nullptr)
    {
        RenderMeshContainer(meshContainer, frame);
        meshContainer = meshContainer->pNextMeshContainer;
    }

    if (frame->pFrameSibling != nullptr)
    {
        RenderFrame(frame->pFrameSibling);
    }

    if (frame->pFrameFirstChild != nullptr)
    {
        RenderFrame(frame->pFrameFirstChild);
    }
}

void AnimMesh::RenderMeshContainer(const LPD3DXMESHCONTAINER meshContainerBase,
                                   const LPD3DXFRAME frameBase)
{
    AnimMeshFrame* frame = static_cast<AnimMeshFrame*>(frameBase);

    D3DXMATRIX matWorldViewProj = frame->m_combinedMatrix;

    matWorldViewProj *= m_viewMatrix;
    matWorldViewProj *= m_projMatrix;

    m_D3DEffect->SetMatrix("g_worldViewProj", &matWorldViewProj);

    m_D3DEffect->Begin(nullptr, 0);

    if (m_D3DEffect->BeginPass(0) == S_OK)
    {
        AnimMeshContainer* meshContainer = static_cast<AnimMeshContainer*>(meshContainerBase);

        for (DWORD i = 0; i < meshContainer->NumMaterials; ++i)
        {
            D3DXVECTOR4 color { meshContainer->pMaterials[i].MatD3D.Diffuse.r,
                                meshContainer->pMaterials[i].MatD3D.Diffuse.g,
                                meshContainer->pMaterials[i].MatD3D.Diffuse.b,
                                meshContainer->pMaterials[i].MatD3D.Diffuse.a };
            m_D3DEffect->SetVector("g_diffuse", &color);
            m_D3DEffect->SetTexture("g_tex", meshContainer->m_vecTexture.at(i));
            m_D3DEffect->CommitChanges();

            meshContainer->MeshData.pMesh->DrawSubset(i);
        }
        m_D3DEffect->EndPass();
    }
    m_D3DEffect->End();
}

void AnimMesh::ReleaseMeshAllocator(const LPD3DXFRAME frame)
{
    if (frame->pMeshContainer != nullptr)
    {
        m_allocator->DestroyMeshContainer(frame->pMeshContainer);
    }

    if (frame->pFrameSibling != nullptr)
    {
        ReleaseMeshAllocator(frame->pFrameSibling);
    }

    if (frame->pFrameFirstChild != nullptr)
    {
        ReleaseMeshAllocator(frame->pFrameFirstChild);
    }

    m_allocator->DestroyFrame(frame);
}

void AnimMesh::SetAnim(const std::string& animSet)
{
    std::size_t _index = -1;
    for (std::size_t i = 0; i < m_animSets.size(); ++i)
    {
        if (m_animSets.at(i)->GetName() == animSet)
        {
            _index = i;
            break;
        }
    }

    if (_index == -1)
    {
        return;
    }

    m_D3DAnimController->SetTrackAnimationSet(0, m_animSets.at(_index));
    m_D3DAnimController->SetTrackPosition(0, -1.001f / 60);
    m_animTime = 0.f;

    if (m_animConfigMap.find(animSet) == m_animConfigMap.end())
    {
        return;
    }

    if (animSet != m_defaultAnim && !m_animConfigMap.at(animSet).loop)
    {
        m_isPlaying = true;
        m_currentAnim = animSet;
    }
};

void AnimMesh::Update()
{
    m_animTime += 1.f/60;
    m_D3DAnimController->SetTrackPosition(0, 0.f);
    m_D3DAnimController->AdvanceTime(m_animTime, nullptr);
    if (m_isPlaying)
    {
        float duration = m_animConfigMap.at(m_currentAnim).duration;
        if (m_animTime + 0.001f >= duration)
        {
            SetAnim(m_defaultAnim);
            m_isPlaying = false;
            m_animTime = 0;
        }
    }
};

void AnimMesh::SetDefaultAnim(const std::string& animation_name)
{
    m_defaultAnim = animation_name;
    SetAnim(m_defaultAnim);
}

void AnimMesh::SetAnimConfig(const std::string& animName,
                             const bool loop,
                             const float duration)
{
    m_animConfigMap[animName] = AnimConfig { loop, duration };
}

bool AnimMesh::isPlaying()
{
    return m_isPlaying;
}

