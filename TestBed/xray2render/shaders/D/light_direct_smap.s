struct 	a2v
{
  float4 Position: 	POSITION;	// Object-space position
  float4 tc0: 		TEXCOORD0;	// Texture coordinates
};

struct 	v2p_out
{
  float4 HPos: 		POSITION;	// Clip-space position 	(for rasterization)
  float2 tc0: 		TEXCOORD0;	// Texture coordinates 	(for sampling maps)
};

struct 	v2p_in
{
  half2 tc0: 		TEXCOORD0;	// Texture coordinates 	(for sampling maps)
};

struct 	p2f
{
  half4 C: 			COLOR0;		// Final color
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D 	s_position;
uniform sampler2D 	s_normal;
uniform sampler2D 	s_diffuse;
uniform sampler1D 	s_power;
uniform sampler2D 	s_shadowmap;
uniform half3 		light_direction;
uniform half4 		light_color;
uniform half4x4		light_xform;
uniform half4		jitter[6];

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_out v_main	( a2v  	IN )
{
  v2p_out 	OUT;
  OUT.HPos 	= IN.Position;				// pass-through
  OUT.tc0 	= IN.tc0;
  return	OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
p2f 	p_main	( v2p_in IN )
{
  p2f		OUT;

  // Sample the fat framebuffer:
  half4 _P		= tex2D 	(s_position, IN.tc0); 
  half4 _N		= tex2D 	(s_normal,   IN.tc0); 
  
  // Transform position to light/shadow space
  half4 PLS		= mul		(light_xform,float4(_P.x,_P.y,_P.z,1));
  half2	uv0		= half2		(PLS.x/PLS.w,PLS.y/PLS.w);
  half  depth	= PLS.z;
  
  // 1. Sample shadowmap 
  // 2. Compare (if (depth_pixel > depth_smap) then in shadow)
  half4 s0,s1,s2,s3,sC;
  half3 sA;
  half4 one		= float4	(1,1,1,1);
 
  s0			= tex2D		(s_shadowmap,uv0+half2(jitter[0].x,jitter[0].y));
  s1			= tex2D		(s_shadowmap,uv0+half2(jitter[0].w,jitter[0].z));
  s2			= tex2D		(s_shadowmap,uv0+half2(jitter[1].x,jitter[1].y));
  s3			= tex2D		(s_shadowmap,uv0+half2(jitter[1].w,jitter[1].z));
  sC			= step		(float4(depth-s0.x,depth-s1.x,depth-s2.x,depth-s3.x),	0);
  sA.x			= dot		(sC,one);

  s0			= tex2D		(s_shadowmap,uv0+half2(jitter[2].x,jitter[2].y));
  s1			= tex2D		(s_shadowmap,uv0+half2(jitter[2].w,jitter[2].z));
  s2			= tex2D		(s_shadowmap,uv0+half2(jitter[3].x,jitter[3].y));
  s3			= tex2D		(s_shadowmap,uv0+half2(jitter[3].w,jitter[3].z));
  sC			= step		(float4(depth-s0.x,depth-s1.x,depth-s2.x,depth-s3.x),	0);
  sA.y			= dot		(sC,one);

  s0			= tex2D		(s_shadowmap,uv0+half2(jitter[4].x,jitter[4].y));
  s1			= tex2D		(s_shadowmap,uv0+half2(jitter[4].w,jitter[4].z));
  s2			= tex2D		(s_shadowmap,uv0+half2(jitter[5].x,jitter[5].y));
  s3			= tex2D		(s_shadowmap,uv0+half2(jitter[5].w,jitter[5].z));
  sC			= step		(float4(depth-s0.x,depth-s1.x,depth-s2.x,depth-s3.x),	0);
  sA.z			= dot		(sC,one);
  
  half	 shadow	= (sA.x + sA.y + sA.z)/12;
  
  // Normal, vector2eye, vector2light
  half3 N		= half3		(_N.x,_N.y,_N.z);
  half3 V 		= -normalize(half3(_P.x,_P.y,_P.z));
  half3 L 		= -half3	(light_direction.x,light_direction.y,light_direction.z);

  // Diffuse = (L • N)
  half	l_D 	= saturate	(dot	(L, N));

  // Specular = (H • N)^m
  half	l_S 	= tex1D		(s_power,	saturate(dot(normalize(L + V), N)));
  
  // Final color
  OUT.C 		= shadow * light_color * half4(l_D,l_D,l_D,l_S);
  return OUT;
}
