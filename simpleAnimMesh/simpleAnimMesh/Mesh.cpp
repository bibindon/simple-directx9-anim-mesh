#include "Mesh.h"
#include "Common.h"

Mesh::Mesh(const LPDIRECT3DDEVICE9 D3DDevice,
           const std::string& xFilename,
           const D3DXVECTOR3& position,
           const D3DXVECTOR3& rotation,
           const float& scale)
    : m_meshName(xFilename)
    , m_pos(position)
    , m_rotate(rotation)
    , m_scale(scale)
{
    HRESULT result = 0;
    D3DXCreateEffectFromFile(D3DDevice,
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

    LPD3DXBUFFER adjacencyBuffer = nullptr;
    LPD3DXBUFFER materialBuffer = nullptr;

    result = D3DXLoadMeshFromX(xFilename.c_str(),
                               D3DXMESH_SYSTEMMEM,
                               D3DDevice,
                               &adjacencyBuffer,
                               &materialBuffer,
                               nullptr,
                               &m_materialCount,
                               &m_D3DMesh);

    if (FAILED(result))
    {
        throw std::exception("Failed to load a x-file.");
    }

    D3DVERTEXELEMENT9 decl[] = {
        {
            0,
            0,
            D3DDECLTYPE_FLOAT3,
            D3DDECLMETHOD_DEFAULT,
            D3DDECLUSAGE_POSITION,
            0},
        {
            0,
            12,
            D3DDECLTYPE_FLOAT3,
            D3DDECLMETHOD_DEFAULT,
            D3DDECLUSAGE_NORMAL,
            0
        },
        {
            0,
            24,
            D3DDECLTYPE_FLOAT2,
            D3DDECLMETHOD_DEFAULT,
            D3DDECLUSAGE_TEXCOORD,
            0
        },
        D3DDECL_END(),
    };

    LPD3DXMESH tempMesh = nullptr;
    result = m_D3DMesh->CloneMesh(D3DXMESH_MANAGED, decl, D3DDevice, &tempMesh);

    if (FAILED(result))
    {
        throw std::exception("Failed 'CloneMesh' function.");
    }
    SAFE_RELEASE(m_D3DMesh);
    m_D3DMesh = tempMesh;
    DWORD* wordBuffer = static_cast<DWORD*>(adjacencyBuffer->GetBufferPointer());

    result = D3DXComputeNormals(m_D3DMesh, wordBuffer);

    if (FAILED(result))
    {
        throw std::exception("Failed 'D3DXComputeNormals' function.");
    }

    result = m_D3DMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT |
                                        D3DXMESHOPT_VERTEXCACHE,
                                        wordBuffer,
                                        nullptr,
                                        nullptr,
                                        nullptr);

    SAFE_RELEASE(adjacencyBuffer);

    if (FAILED(result))
    {
        throw std::exception("Failed 'OptimizeInplace' function.");
    }

    m_vecColor.insert(begin(m_vecColor), m_materialCount, D3DCOLORVALUE { });
    std::vector<LPDIRECT3DTEXTURE9> tempVecTexture { m_materialCount };
    m_vecTexture.swap(tempVecTexture);

    D3DXMATERIAL* materials = static_cast<D3DXMATERIAL*>(materialBuffer->GetBufferPointer());

    std::string xFileDir = m_meshName;
    std::size_t lastPos = xFileDir.find_last_of("\\");
    xFileDir = xFileDir.substr(0, lastPos + 1);

    for (DWORD i = 0; i < m_materialCount; ++i)
    {
        m_vecColor.at(i) = materials[i].MatD3D.Diffuse;
        if (materials[i].pTextureFilename != nullptr)
        {
            std::string texPath = xFileDir;
            texPath += materials[i].pTextureFilename;
            LPDIRECT3DTEXTURE9 tempTexture = nullptr;
            if (FAILED(D3DXCreateTextureFromFile(D3DDevice,
                                                 texPath.c_str(),
                                                 &tempTexture)))
            {
                throw std::exception("texture file is not found.");
            }
            else
            {
                SAFE_RELEASE(m_vecTexture.at(i));
                m_vecTexture.at(i) = tempTexture;
            }
        }
    }
    SAFE_RELEASE(materialBuffer);

    m_scale = scale;
}

Mesh::~Mesh()
{
    for (std::size_t i = 0; i < m_vecTexture.size(); ++i)
    {
        SAFE_RELEASE(m_vecTexture.at(i));
    }
    SAFE_RELEASE(m_D3DMesh);
    SAFE_RELEASE(m_D3DEffect);
}

void Mesh::Render(const D3DXMATRIX& view, const D3DXMATRIX& proj)
{
    D3DXMATRIX matWorldViewProj { };
    D3DXMatrixIdentity(&matWorldViewProj);

    D3DXMATRIX mat { };

    D3DXMatrixTranslation(&mat, -m_centerPos.x, -m_centerPos.y, -m_centerPos.z);
    matWorldViewProj *= mat;

    D3DXMatrixScaling(&mat, m_scale, m_scale, m_scale);
    matWorldViewProj *= mat;

    D3DXMatrixRotationYawPitchRoll(&mat, m_rotate.x, m_rotate.y, m_rotate.z);
    matWorldViewProj *= mat;

    D3DXMatrixTranslation(&mat, m_pos.x, m_pos.y, m_pos.z);
    matWorldViewProj *= mat;

    matWorldViewProj *= view;
    matWorldViewProj *= proj;

    m_D3DEffect->SetMatrix("g_worldViewProj", &matWorldViewProj);

    m_D3DEffect->Begin(nullptr, 0);

    if (SUCCEEDED(m_D3DEffect->BeginPass(0)))
    {
        for (DWORD i = 0; i < m_materialCount; ++i)
        {
            D3DXVECTOR4 vec4Color { m_vecColor.at(i).r,
                                    m_vecColor.at(i).g,
                                    m_vecColor.at(i).b,
                                    m_vecColor.at(i).a };
            m_D3DEffect->SetVector("g_diffuse", &vec4Color);
            m_D3DEffect->SetTexture("g_tex", m_vecTexture.at(i));
            m_D3DEffect->CommitChanges();
            m_D3DMesh->DrawSubset(i);
        }
        m_D3DEffect->EndPass();
    }
    m_D3DEffect->End();
}

