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
		samples[s]		= tex2D	(s_bloom, IN.tc0 + offset[s]);
	}

	// center
	samples[7]	= tex2D	(s_bloom, IN.tc0);
	
	// positive
	for (s=0; s<7; s++)
	{
		samples[8+s]	= tex2D	(s_bloom, IN.tc0 - offset[s]);
	}
	
	// final weight
	float3 final		= 
		samples[0]	*weight[0].x + 
		samples[1]	*weight[0].y + 
		samples[2]	*weight[0].z + 
		samples[3]	*weight[0].w + 
		samples[4]	*weight[1].x + 
		samples[5]	*weight[1].y + 
		samples[6]	*weight[1].z + 
		samples[7]	*weight[1].w + 
		samples[8]	*weight[1].z + 
		samples[9]	*weight[1].y + 
		samples[10]	*weight[1].x + 
		samples[11]	*weight[1].x + 

	return OUT;
}
