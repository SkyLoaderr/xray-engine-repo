//-----------------------------------------------------------------------------

texture ShadowMap;
texture SpotLight;
texture ShadowColor;

//-----------------------------------------------------------------------------

float4x4 WorldViewProj : WorldViewProj;
float4x4 WorldIT : WorldIT;
float4x4 TexTransform;
float3   LightVec;

//-----------------------------------------------------------------------------

struct VS_INPUT {
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
};

struct VS_OUTPUT {
    float4 Position  : POSITION;
    float4 TexCoord0 : TEXCOORD0;
    float3 Color0 : COLOR0;
};

//-----------------------------------------------------------------------------

sampler ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = Linear;  
    MagFilter = Linear;
    MipFilter = None;
    AddressU  = Clamp;
    AddressV  = Clamp;
};

sampler ShadowColorSampler = sampler_state
{
    Texture = <ShadowColor>;
    MinFilter = Linear;  
    MagFilter = Linear;
    MipFilter = None;
    AddressU  = Clamp;
    AddressV  = Clamp;
};

//----------------------------------------------------------------------------

VS_OUTPUT QuadVS(VS_INPUT IN)
{
   VS_OUTPUT OUT;
   OUT = (VS_OUTPUT)0;
   float2 Position = (IN.Position.xz / 1600.0f) + 0.5f;
   OUT.Position = float4(Position*0.45 + 0.5, 1, 1);
   OUT.TexCoord0 = float4(Position.x, 1.0-Position.y, 0, 0);
   return OUT;
}

VS_OUTPUT ShadowMapVS(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    OUT.Position = mul(IN.Position, WorldViewProj);
    OUT.TexCoord0 = float4(OUT.Position.zzz, OUT.Position.w);
    return OUT;
}

//-----------------------------------------------------------------------------

VS_OUTPUT MainVS(VS_INPUT IN)
{
    VS_OUTPUT OUT;
  
    float3 worldNormal = normalize(mul(IN.Normal, (float3x3)WorldIT));
    float ldotn = max(dot(LightVec, worldNormal), 0.0);
    OUT.Color0 = ldotn;

    OUT.TexCoord0 = mul(IN.Position, TexTransform);
    
    OUT.Position = mul(IN.Position, WorldViewProj);
 
    return OUT;
}

//-----------------------------------------------------------------------------

float4 DrawShadowPS(VS_OUTPUT IN) : COLOR
{
    return float4(tex2D(ShadowColorSampler, IN.TexCoord0).rgb, 1.0);
}

float4 BlackPS(VS_OUTPUT IN) : COLOR
{
    return float4(0,0,0,1);
}

float4 OutputDepthPS(VS_OUTPUT IN) : COLOR
{
    float depth = IN.TexCoord0.b / IN.TexCoord0.a;
    return float4(depth.xxx, 1.0);
}

//-----------------------------------------------------------------------------

float4 MainPS(VS_OUTPUT IN) : COLOR 
{
    float3 shadow    = tex2D(ShadowMapSampler, IN.TexCoord0).rgb;
    float3 color = ((shadow*0.2+0.8) * IN.Color0)*0.9 + 0.1;
    
    return float4(color, 1.0);
}

//-----------------------------------------------------------------------------

technique UseHardwareShadowMap
{
    pass P0
    {
        VertexShader = compile vs_1_1 MainVS();
        PixelShader = compile ps_1_1 MainPS();
    }
}

technique GenHardwareShadowMap20
{
    pass P0
    {
        VertexShader = compile vs_1_1 ShadowMapVS();
        PixelShader = compile ps_2_0 OutputDepthPS();
    }
}

technique GenHardwareShadowMap11
{
    pass P0
    {
        VertexShader = compile vs_1_1 ShadowMapVS();
        PixelShader = compile ps_1_1 BlackPS();
    }
}


technique DrawHardwareShadowMap
{
    Pass P0
    {
        VertexShader = compile vs_1_1 QuadVS();
        PixelShader = compile ps_1_1 DrawShadowPS();
    }
}

//-----------------------------------------------------------------------------

