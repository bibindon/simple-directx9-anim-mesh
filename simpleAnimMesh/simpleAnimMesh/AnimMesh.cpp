#include "AnimMesh.h"

#include "AnimMeshAlloc.h"
#include "Common.h"

using std::vector;
using std::string;

 AnimController::AnimController(LPD3DXANIMATIONCONTROLLER controller)
        : m_defaultAnim { "" },
        m_animTime { },
        m_currentAnim { "" }
{
    m_D3DAnimController = controller;
    DWORD animation_count { m_D3DAnimController->GetNumAnimationSets() };

    std::vector<LPD3DXANIMATIONSET> animation_sets;

    for (DWORD i { 0 }; i < animation_count; ++i)
    {
        LPD3DXANIMATIONSET temp_animation_set { nullptr };
        m_D3DAnimController->GetAnimationSet(i, &temp_animation_set);
        m_animSets.push_back(temp_animation_set);
    }
}

AnimController::~AnimController()
{
    for (int i = 0; i < m_animSets.size(); ++i)
    {
        m_animSets.at(i)->Release();
    }
    SAFE_RELEASE(m_D3DAnimController);
}

void AnimController::SetAnim(const std::string& animation_set)
{
    std::vector<LPD3DXANIMATIONSET>::const_iterator kit;

    kit = std::find_if(
        m_animSets.cbegin(), m_animSets.cend(),
        [&](const LPD3DXANIMATIONSET& a)
        {
            return animation_set == a->GetName();
        });

    if (m_animSets.cend() == kit)
    {
        // TODO return error
    //    THROW_WITH_TRACE("An illegal animation set was sent.: " + animation_set);
    }

    m_D3DAnimController->SetTrackAnimationSet(0, *kit);
    m_D3DAnimController->SetTrackPosition(0, -1.001f / 60);
    m_animTime = 0.f;

    if (m_animConfigMap.find(animation_set) == m_animConfigMap.end())
    {
        return;
    }
    if (animation_set != m_defaultAnim &&
        !m_animConfigMap.at(animation_set).loop)
    {
        m_isPlaying = true;
        m_currentAnim = animation_set;
    }
};

void AnimController::Update()
{
    m_animTime += 1.f/60;
    m_D3DAnimController->SetTrackPosition(0, 0.f);
    m_D3DAnimController->AdvanceTime(m_animTime, nullptr);
    if (m_isPlaying)
    {
        float duration { m_animConfigMap.at(m_currentAnim).duration };
        if (m_animTime + 0.001f >= duration)
        {
            SetAnim(m_defaultAnim);
            m_isPlaying = false;
            m_animTime = 0;
        }
    }
};

void AnimController::SetDefaultAnim(const std::string& animation_name)
{
    m_defaultAnim = animation_name;
    SetAnim(m_defaultAnim);
}

void AnimController::SetAnimConfig(
    const std::string& animation_name, const bool& loop, const float& duration)
{
    m_animConfigMap.emplace(animation_name, AnimConfig { loop, duration });
}

bool AnimController::is_playing()
{
    return m_isPlaying;
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

AnimMesh::AnimMesh(
    const LPDIRECT3DDEVICE9 D3DDevice,
    const string& xFilename,
    const D3DXVECTOR3& position,
    const D3DXVECTOR3& rotation,
    const float& scale)
    : m_allocator { new AnimMeshAllocator { xFilename } }
    , m_frameRoot { nullptr }
    , m_position { position }
    , m_rotation { rotation }
    , m_centerPos { 0.0f, 0.0f, 0.0f }
{
    HRESULT result { 0 };
    D3DXCreateEffectFromFile(
        D3DDevice,
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

    LPD3DXFRAME temp_root_frame { nullptr };
    LPD3DXANIMATIONCONTROLLER temp_animation_controller { nullptr };

    result = D3DXLoadMeshHierarchyFromX(
        xFilename.c_str(),
        D3DXMESH_MANAGED,
        D3DDevice,
        m_allocator,
        nullptr,
        &temp_root_frame,
        &temp_animation_controller);

    if (FAILED(result))
    {
        throw std::exception("Failed to load a x-file.");
    }
    // lazy initialization 
    m_frameRoot = temp_root_frame;
    m_animController = new AnimController(temp_animation_controller);

    m_scale = scale;
}

AnimMesh::~AnimMesh()
{
    ReleaseMeshAllocator(m_frameRoot);
    delete m_allocator;
    delete m_animController;
    m_D3DEffect->Release();
}

void AnimMesh::Render(const D3DXMATRIX& view, const D3DXMATRIX& proj)
{
    m_viewMatrix = view;
    m_projMatrix = proj;

    m_animController->Update();

    D3DXMATRIX worldMatrix { };
    D3DXMatrixIdentity(&worldMatrix);
    {
        D3DXMATRIX mat { };
        D3DXMatrixTranslation(&mat, -m_centerPos.x, -m_centerPos.y, -m_centerPos.z);
        worldMatrix *= mat;

        D3DXMatrixScaling(&mat, m_scale, m_scale, m_scale);
        worldMatrix *= mat;

        D3DXMatrixRotationYawPitchRoll(&mat, m_rotation.x, m_rotation.y, m_rotation.z);
        worldMatrix *= mat;

        D3DXMatrixTranslation(&mat, m_position.x, m_position.y, m_position.z);
        worldMatrix *= mat;
    }
    UpdateFrameMatrix(m_frameRoot, &worldMatrix);
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
    AnimMeshFrame* frame { static_cast<AnimMeshFrame*>(frameBase) };
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
    {
        LPD3DXMESHCONTAINER mesh_container { frame->pMeshContainer };
        while (mesh_container != nullptr)
        {
            RenderMeshContainer(mesh_container, frame);
            mesh_container = mesh_container->pNextMeshContainer;
        }
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

void AnimMesh::RenderMeshContainer(
    const LPD3DXMESHCONTAINER meshContainerBase, const LPD3DXFRAME frameBase)
{
    AnimMeshFrame* frame { static_cast<AnimMeshFrame*>(frameBase) };

    D3DXMATRIX worldViewProjMatrix { frame->m_combinedMatrix };

    worldViewProjMatrix *= m_viewMatrix;
    worldViewProjMatrix *= m_projMatrix;

    m_D3DEffect->SetMatrix("g_world_view_proj", &worldViewProjMatrix);

    m_D3DEffect->Begin(nullptr, 0);

    if (m_D3DEffect->BeginPass(0) == S_OK)
    {
        AnimMeshContainer* meshContainer { static_cast<AnimMeshContainer*>(meshContainerBase) };

        for (DWORD i = 0; i < meshContainer->NumMaterials; ++i)
        {
            D3DXVECTOR4 color {
                meshContainer->pMaterials[i].MatD3D.Diffuse.r,
                meshContainer->pMaterials[i].MatD3D.Diffuse.g,
                meshContainer->pMaterials[i].MatD3D.Diffuse.b,
                meshContainer->pMaterials[i].MatD3D.Diffuse.a };
            m_D3DEffect->SetVector("g_diffuse", &color);
            m_D3DEffect->SetTexture("g_mesh_texture", meshContainer->m_vecTexture.at(i));

            m_D3DEffect->CommitChanges();
            meshContainer->MeshData.pMesh->DrawSubset(i);
        }
    }
    m_D3DEffect->EndPass();
    m_D3DEffect->End();
}

