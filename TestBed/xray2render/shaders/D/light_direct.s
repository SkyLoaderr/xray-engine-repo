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
uniform float4 		light_direction;
uniform float4 		light_color;

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
  float3 P		= float3	(_P.x,_P.y,_P.z);
  float3 N		= float3	(_N.x,_N.y,_N.z);

  // Vector to the eye:
  float3 V 		= -normalize(P);

  // Vector to the light:
  float3 L 		= -float3	(light_direction.x,light_direction.y,light_direction.z);

  // Diffuse = (L • N)
  float l_D		= saturate(dot		(L, N);

  // Half-angle vector:
  float3 H 		= normalize	(L + V);

  // Specular = (H • N)^m
  float S 		= tex1D		(s_power,	saturate(dot(H, N))); //pow		(saturate(dot(H, N)), 32);
  
  // Final color
  OUT.C 		= light_color *		float4(l_D,l_D,l_D,l_S);
  return OUT;
}
