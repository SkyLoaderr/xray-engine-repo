struct 	a2v
{
	float4	Position: 	POSITION;	// Object-space position
};

struct 	v2p_out
{
	float4	HPos: 		POSITION;	// Clip-space position 	(for rasterization)
	float	Depth:		TEXCOORD0;	// Light-space depth 	()
};

struct 	v2p_in
{
	float	Depth: 		TEXCOORD0;	// Light-space depth 	()
};

struct 	p2f
{
	float4	C:     		COLOR;		// Light-space depth 	()
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float4x4 		m_model2view2projection;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_out v_main			( a2v  	IN )
{
  // Write output registers.
  v2p_out 		OUT;
  float4	P	= mul	(m_model2view2projection,	IN.Position	);
  OUT.HPos 		= P;
  OUT.Depth		= P.z;

  return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
p2f 	p_main	( v2p_in IN )
{
  p2f		OUT;

  OUT.C		= IN.Depth;
  return	OUT;
}
