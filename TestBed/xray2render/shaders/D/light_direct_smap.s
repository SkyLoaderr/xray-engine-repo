struct 	a2v
{
  float4 Position: 	POSITION;	// Object-space position
  float4 Tex0: 		TEXCOORD0;	// Texture coordinates
};

struct 	v2p_out
{
  float4 HPos: 		POSITION;	// Clip-space position 	(for rasterization)
  float4 Tex0: 		TEXCOORD0;	// Texture coordinates 	(for sampling maps)
};

struct 	v2p_in
{
  float4 Tex0: 		TEXCOORD0;	// Texture coordinates 	(for sampling maps)
};

struct 	p2f
{
  float4 C: 		COLOR0;		// Final color
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D 	s_position;
uniform sampler2D 	s_normal;
uniform sampler2D 	s_diffuse;
uniform sampler1D 	s_power;
uniform sampler2D 	s_shadowmap;
uniform float4 		light_direction;
uniform float4 		light_color;
uniform float4x4	light_xform;
uniform float2		jitter[8];

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_out v_main	( a2v  	IN )
{
  v2p_out 	OUT;
  OUT.HPos 	= IN.Position;				// pass-through
  OUT.Tex0 	= IN.Tex0;
  return	OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
p2f 	p_main	( v2p_in IN )
{
  p2f		OUT;

  // Sample the fat framebuffer:
  float4 _P		= tex2D 	(s_position, float2(IN.Tex0.x, IN.Tex0.y)); 
  float4 _N		= tex2D 	(s_normal,   float2(IN.Tex0.x, IN.Tex0.y)); 
  
  // Transform position to light/shadow space
  float4 PLS	= mul		(light_xform,float4(_P.x,_P.y,_P.z,1));
  float2 uv0	= float2	(PLS.x/PLS.w,PLS.y/PLS.w);
  float  depth	= PLS.z		+ .001f;
  
  // Sample shadowmap
  float4 s0		= tex2D		(s_shadowmap,uv0+jitter[0]);
  float4 s1		= tex2D		(s_shadowmap,uv0+jitter[1]);
  float4 s2		= tex2D		(s_shadowmap,uv0+jitter[2]);
  float4 s3		= tex2D		(s_shadowmap,uv0+jitter[3]);
  float4 s4		= tex2D		(s_shadowmap,uv0-jitter[0]);
  float4 s5		= tex2D		(s_shadowmap,uv0-jitter[1]);
  float4 s6		= tex2D		(s_shadowmap,uv0-jitter[2]);
  float4 s7		= tex2D		(s_shadowmap,uv0-jitter[3]);
  
  // Compare (if (depth_pixel > depth_smap) then in shadow)
  float4 sC0	= step		(float4(depth-s0.x,depth-s1.x,depth-s2.x,depth-s3.x),0);
  float4 sC1	= step		(float4(depth-s4.x,depth-s5.x,depth-s6.x,depth-s7.x),0);
  float	 shadow	= (dot(sC0,sC0)+dot(sC1,sC1))/8;
  
  // Normal
  float3 N		= float3	(_N.x,_N.y,_N.z);
  
  // Vector to the eye:
  float3 V 		= -normalize(float3(_P.x,_P.y,_P.z));

  // Vector to the light:
  float3 L 		= -float3	(light_direction.x,light_direction.y,light_direction.z);

  // Diffuse = (L • N)
  float D 		= dot		(L, N);

  // Half-angle vector:
  float3 H 		= normalize	(L + V);

  // Specular = (H • N)^m
  float S 		= tex1D		(s_power,	saturate(dot(H, N)));
  
  // Final color
  float4 C		= shadow*light_color*D;
  C.w			= shadow*light_color.w*S;

  OUT.C 	= C;
  return OUT;
}
