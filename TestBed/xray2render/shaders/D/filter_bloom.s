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
  float2 tc0: 		TEXCOORD0;	// Texture coordinates 	(for sampling maps)
};

struct 	p2f
{
  float4 C: 		COLOR0;		// Final color
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D 	s_bloom;
uniform float4 		weight[2];
uniform float2		offset[7];

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

	float3	samples	[15];
	int		s;

	// negative
	for (s=0; s<7; s++)
	{
		samples[s]	= tex2D	(s_bloom, IN.tc0 + offset[s]);
	}
	



	return OUT;
}
