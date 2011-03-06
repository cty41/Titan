//
// Default non-skinned shader
//
#include "light_scattering_constants.h"

// material reflectivity
float4 k_d : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse

texture tex0 : TEXTURE;

// transformations
float4 vCameraPos: worldcamerapos;
float4 sun_vec: sunvector = {0.578f,0.578f,0.578f,0.0f};
float4 sun_color: suncolor = {0.578f,0.578f,0.578f,0.0f};

// Matrix Pallette
static const int MAX_MATRICES = 8;
float4x3    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;
float4x4    mViewProj : VIEWPROJECTION;

struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;
    float3    Normal            : NORMAL;
    float2    Tex                : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 vI   : COLOR0;
    float4 vE   : COLOR1;
    float2 Tex  : TEXCOORD0;
    float4 NdL  : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT i, uniform int NumBones)
{
    VS_OUTPUT   o;
    float3      Pos = 0.0f;
    float3      Normal = 0.0f;    
    float       LastWeight = 0.0f;
     
    // Compensate for lack of UBYTE4 on Geforce3
    int4 IndexVector = D3DCOLORtoUBYTE4(i.BlendIndices);

    // cast the vectors to arrays for use in the for loop below
    float BlendWeightsArray[4] = (float[4])i.BlendWeights;
    int   IndexArray[4]        = (int[4])IndexVector;
    
    // calculate the pos/normal using the "normal" weights 
    //        and accumulate the weights to calculate the last weight
    for (int iBone = 0; iBone < NumBones-1; iBone++)
    {
        LastWeight = LastWeight + BlendWeightsArray[iBone];
        
        Pos += mul(i.Pos, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        Normal += mul(i.Normal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
    }
    LastWeight = 1.0f - LastWeight; 

    // Now that we have the calculated weight, add in the final influence
    Pos += (mul(i.Pos, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);
    Normal += (mul(i.Normal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); 
    
    // transform position from world space into view and then projection space
    o.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);

    // normalize normals
    Normal = normalize(Normal);

    // copy the input texture coordinate through
    o.Tex  = i.Tex.xy;

    float ndl = dot(Normal, sun_vec);
    o.NdL = float4(ndl, ndl, ndl, 1.0f);

    float3 eyeVector = vCameraPos - o.Pos; 
    float s = length(eyeVector);
    eyeVector = normalize(eyeVector);

    atmosphericLighting(
        eyeVector,
        sun_vec,
        Normal,
        sun_color,
        s,
        o.vE,
        o.vI);

    return o;
}

sampler LinearSamp0 = sampler_state 
{
    texture = <tex0>;
    AddressU  = clamp;        
    AddressV  = clamp;
    AddressW  = clamp;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

float4 PS(VS_OUTPUT In) : COLOR
{
        float4 texColor = tex2D(LinearSamp0, In.Tex );            

        texColor.xyz = texColor.xyz * In.NdL.xyz;

    return (texColor*In.vE) + In.vI;
}

int CurNumBones = 2;
VertexShader vsArray[4] = { compile vs_2_0 VS(1), 
                            compile vs_2_0 VS(2),
                            compile vs_2_0 VS(3),
                            compile vs_2_0 VS(4)
                          };

technique TVertexAndPixelShader
{
    pass P0
    {
         CULLMODE = CCW;
        ZENABLE = TRUE;
        ZWRITEENABLE = TRUE;
        ZFUNC = LESSEQUAL;

        AlphaBlendEnable = false;
        AlphaTestEnable = true;
        AlphaRef = 127;
        AlphaFunc = Greater;

       // shaders
        VertexShader = (vsArray[CurNumBones]);
        PixelShader  = compile ps_2_0 PS();
    }  
}