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
#define	SM_SIZE	1024.f

float 	sample	(float4 tc, float x, float y)
{
    	return	tex2Dproj(ShadowMapSampler, float4(tc.xy+float2(x,y)*tc.w,tc.zw)).x;
}
float 	samplex	(float4 tc, float4 o)
{
		o*=tc.w;
    	return	tex2Dproj(ShadowMapSampler, float4(tc.xy + o.xy,tc.zw)).x+
    		tex2Dproj(ShadowMapSampler, float4(tc.xy + o.wz,tc.zw)).x;
}

float 	shadow9	(float4 tc)
{
	float2 	one 	= .75f/SM_SIZE;
    	float 	shadow	= 	0;		
		shadow	+=	sample(tc,-one.x,-one.y);
		shadow	+=	sample(tc,     0,-one.y);
		shadow	+=	sample(tc,+one.x,-one.y);
		shadow	+=	sample(tc,-one.x,0);
		shadow	+=	sample(tc,     0,0);	// central
		shadow	+=	sample(tc,+one.x,0);
		shadow	+=	sample(tc,-one.x,+one.y);
		shadow	+=	sample(tc,     0,+one.y);
		shadow	+=	sample(tc,+one.x,+one.y);
		shadow	/=	9.f;
	return	shadow;
}
float 	shadow12(float4 tc)
{
	float4 	J,jitter[6];
	float 	scale 	= (2.f/SM_SIZE)/11.f;
	J=float4 (11, 0,  0,  0 )-11; J*=scale;	jitter[0]=float4(J.x,J.y,-J.y,-J.x);
	J=float4 (19, 3,  0,  0 )-11; J*=scale;	jitter[1]=float4(J.x,J.y,-J.y,-J.x);
	J=float4 (22, 11, 0,  0 )-11; J*=scale;	jitter[2]=float4(J.x,J.y,-J.y,-J.x);
	J=float4 (19, 19, 0,  0 )-11; J*=scale;	jitter[3]=float4(J.x,J.y,-J.y,-J.x);
	J=float4 (9,  7,  15, 9 )-11; J*=scale;	jitter[4]=float4(J.x,J.y, J.w, J.z);
	J=float4 (13, 15, 7,  13)-11; J*=scale;	jitter[5]=float4(J.x,J.y, J.w, J.z);
	float 	shadow 	= 	0;
		shadow	+=      samplex	(tc,jitter[0]);
		shadow	+=      samplex	(tc,jitter[1]);
		shadow	+=      samplex	(tc,jitter[2]);
		shadow	+=      samplex	(tc,jitter[3]);
		shadow	+=      samplex	(tc,jitter[4]);
		shadow	+=      samplex	(tc,jitter[5]);
		shadow	/=	12.f;
	return	shadow;
}

float4 MainPS(VS_OUTPUT IN) : COLOR 
{
	float4 	tc 	= 	IN.TexCoord0;
    	float 	shadow	= 	shadow12(tc);
    	float3 	color	= 	shadow*IN.Color0;
    
    return float4(color, 1.0);
}

//-----------------------------------------------------------------------------

technique UseHardwareShadowMap
{
    pass P0
    {
        VertexShader = compile vs_1_1 MainVS();
        PixelShader = compile ps_2_0 MainPS();
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

