#include "AnimMesh.h"

#include "AnimMeshAlloc.h"
#include "MainWindow.h"
#include "Common.h"
#include "AnimationStrategy.h"
#include "Light.h"
#include "Camera.h"
#include "SharedObj.h"

using std::vector;
using std::string;

void AnimMesh::frame_root_deleter_object::operator()(const LPD3DXFRAME frameRoot)
{
    release_mesh_allocator(frameRoot);
}

void AnimMesh::frame_root_deleter_object::release_mesh_allocator(const LPD3DXFRAME frame)
{
    if (frame->pMeshContainer != nullptr)
    {
        allocator_->DestroyMeshContainer(frame->pMeshContainer);
    }
    if (frame->pFrameSibling != nullptr)
    {
        release_mesh_allocator(frame->pFrameSibling);
    }
    if (frame->pFrameFirstChild != nullptr)
    {
        release_mesh_allocator(frame->pFrameFirstChild);
    }
    allocator_->DestroyFrame(frame);
}

AnimMesh::AnimMesh(
    const string& xFilename,
    const D3DXVECTOR3& position,
    const D3DXVECTOR3& rotation,
    const float& scale)
    : m_allocator { new AnimMeshAllocator { xFilename } }
    , m_frameRoot { nullptr, frame_root_deleter_object { m_allocator } }
    , m_rotationMatrix { D3DMATRIX { } }
    , m_position { position }
    , m_rotation { rotation }
    , m_centerPos { 0.0f, 0.0f, 0.0f }
{
    HRESULT result { 0 };
    D3DXCreateEffectFromFile(
        SharedObj::GetD3DDevice(),
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

    m_worldHandle = m_D3DEffect->GetParameterByName(nullptr, "g_world");
    m_worldViewProjHandle = m_D3DEffect->GetParameterByName(nullptr, "g_world_view_projection");
    m_lightNormalHandle = m_D3DEffect->GetParameterByName(nullptr, "g_light_normal");
    m_brightnessHandle = m_D3DEffect->GetParameterByName(nullptr, "g_light_brightness");
    m_meshTextureHandle = m_D3DEffect->GetParameterByName(nullptr, "g_mesh_texture");
    m_diffuseHandle = m_D3DEffect->GetParameterByName(nullptr, "g_diffuse");

    LPD3DXFRAME temp_root_frame { nullptr };
    LPD3DXANIMATIONCONTROLLER temp_animation_controller { nullptr };

    result = D3DXLoadMeshHierarchyFromX(
        xFilename.c_str(),
        D3DXMESH_MANAGED,
        SharedObj::GetD3DDevice(),
        m_allocator.get(),
        nullptr,
        &temp_root_frame,
        &temp_animation_controller);

    if (FAILED(result))
    {
        throw std::exception("Failed to load a x-file.");
    }
    // lazy initialization 
    m_frameRoot.reset(temp_root_frame);
    m_animationStrategy.reset(new normal_animation { temp_animation_controller });

    m_scale = scale;
}

AnimMesh::~AnimMesh()
{
}

void AnimMesh::Render()
{
    D3DXVECTOR4 normal = Light::GetLightNormal();
    m_D3DEffect->SetVector(m_lightNormalHandle, &normal);
    m_D3DEffect->SetFloat(m_brightnessHandle, Light::GetBrightness());

    m_viewMatrix = Camera::GetViewMatrix();
    m_projMatrix = Camera::GetProjMatrix();

    m_animationStrategy->update();

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
    UpdateFrameMatrix(m_frameRoot.get(), &worldMatrix);
    RenderFrame(m_frameRoot.get());
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

    m_D3DEffect->SetMatrix(m_worldHandle, &worldViewProjMatrix);

    worldViewProjMatrix *= m_viewMatrix;
    worldViewProjMatrix *= m_projMatrix;

    m_D3DEffect->SetMatrix(m_worldViewProjHandle, &worldViewProjMatrix);

    m_D3DEffect->Begin(nullptr, 0);

    if (FAILED(m_D3DEffect->BeginPass(0)))
    {
        m_D3DEffect->End();
        //THROW_WITH_TRACE("Failed 'BeginPass' function.");
        // TODO return false;
    }

    AnimMeshContainer* meshContainer { static_cast<AnimMeshContainer*>(meshContainerBase) };

    for (DWORD i = 0; i < meshContainer->NumMaterials; ++i)
    {
        D3DXVECTOR4 color {
            meshContainer->pMaterials[i].MatD3D.Diffuse.r,
            meshContainer->pMaterials[i].MatD3D.Diffuse.g,
            meshContainer->pMaterials[i].MatD3D.Diffuse.b,
            meshContainer->pMaterials[i].MatD3D.Diffuse.a };
        m_D3DEffect->SetVector(m_diffuseHandle, &color);
        m_D3DEffect->SetTexture(m_meshTextureHandle, meshContainer->m_vecTexture.at(i));

        m_D3DEffect->CommitChanges();
        meshContainer->MeshData.pMesh->DrawSubset(i);
    }
    m_D3DEffect->EndPass();
    m_D3DEffect->End();
}

