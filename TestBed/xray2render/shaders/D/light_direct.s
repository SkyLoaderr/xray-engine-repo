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

  // Normal, vector2eye, vector2light
  float3 N		= float3	(_N.x,_N.y,_N.z);
  float3 V 		= -normalize(float3(_P.x,_P.y,_P.z));
  float3 L 		= -float3	(light_direction.x,light_direction.y,light_direction.z);

  // Diffuse = (L • N)
  float	l_D 	= saturate	(dot	(L, N));

  // Specular = (H • N)^m
  float l_S 	= tex1D		(s_power,	saturate(dot(normalize(L + V), N)));
  
  // Final color
  OUT.C 		= light_color *		float4(l_D,l_D,l_D,l_S);
  return OUT;
}
