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
uniform sampler2D 	s_accumulator;

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

  OUT.C 	= tex2D		(s_diffuse, float2(IN.Tex0.x, IN.Tex0.y)); 
  return OUT;
}
