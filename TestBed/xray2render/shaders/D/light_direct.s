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
  float4 P	= tex2D 	(s_position, float2(IN.Tex0.x, IN.Tex0.y)); 
  float4 N 	= tex2D 	(s_normal,   float2(IN.Tex0.x, IN.Tex0.y)); 

  // Vector to the eye:
  float4 V 	= normalize(-P);

  // Vector to the light:
  float4 L 	= -light_direction;

  // Diffuse = (L • N)
  float D 	= max(0,dot(L, N));

  // Half-angle vector:
  float4 H 	= normalize(L + V);

  // Specular = (H • N)^m
  float S 	= pow(max(0,dot(H, N)), 16);

  OUT.C 	= float4(light_color.x*D,light_color.y*D,light_color.z*D,S);
  return OUT;
}
