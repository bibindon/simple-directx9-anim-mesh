float4x4 g_worldViewProj;
float4 g_lightNorm = {1.f, 1.f, 1.f,1.f};
float g_lightBrightness = 0.5f;
float4 g_diffuse;
float4 g_ambient = { 0.2f, 0.2f, 0.2f, 0.0f };
texture g_tex;

void VertexShader0(
    in  float4 inPos  : POSITION,
    in  float4 inNorm    : NORMAL0,
    in  float4 inTexCood   : TEXCOORD0,

    out float4 outPos : POSITION,
    out float4 outDiffuse  : COLOR0,
    out float4 outTexCood  : TEXCOORD0)
{
    outPos  = mul(inPos, g_worldViewProj);
    inNorm = normalize(inNorm);

    float lightIntensity = g_lightBrightness * dot(inNorm, g_lightNorm);
    outDiffuse = g_diffuse * max(0, lightIntensity) + g_ambient;
    outDiffuse.a = 1.0f;

    outTexCood  = inTexCood;
}

sampler g_texSampler = sampler_state
{
    Texture   = (g_tex);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

void PixelShader0(
    in  float4 inDiffuse     : COLOR0,
    in  float2 inTexCood     : TEXCOORD0,
    out float4 outDiffuse    : COLOR0)
{
    float4 resultColor = (float4)0;
    resultColor = tex2D(g_texSampler, inTexCood);
    outDiffuse = (inDiffuse * resultColor);
}

technique Technique0
{
    pass Pass0
    {
        VertexShader = compile vs_2_0 VertexShader0();
        PixelShader  = compile ps_2_0 PixelShader0();
    }
}

