struct 	a2v
{
  float4 Position: 	POSITION;	// Object-space position
  float4 Normal: 	NORMAL;		// Object-space normal
  float4 TexCoords: TEXCOORD0;	// Texture coordinates
  float4 T:			TEXCOORD1;	// tangent
  float4 B:			TEXCOORD2;	// binormal
};

struct 	v2p_out
{
  float4 HPos: 		POSITION;	// Clip-space position 	(for rasterization)
  float4 Pe: 		TEXCOORD0;	// Eye-space position 	(for lighting)
  float4 Tex0: 		TEXCOORD1;	// Texture coordinates 	(for sampling diffuse+normal map)
  float4 M1:		TEXCOORD2;	// nmap 2 eye - 1
  float4 M2:		TEXCOORD3;	// nmap 2 eye - 2
  float4 M3:		TEXCOORD4;	// nmap 2 eye - 3
};

struct 	v2p_in
{
  float4 Pe: 		TEXCOORD0;	// Eye-space position 	(for lighting)
  float4 Tex0: 		TEXCOORD1;	// Texture coordinates 	(for sampling diffuse+normal map)
  float4 M1:		TEXCOORD2;	// nmap 2 eye - 1
  float4 M2:		TEXCOORD3;	// nmap 2 eye - 2
  float4 M3:		TEXCOORD4;	// nmap 2 eye - 3
};

struct 	p2f
{
  float4 Pe: 		COLOR0;		// Eye-space position 	(for lighting)
  float4 Ne:		COLOR1;		// Eye-space normal		(for lighting)
  float4 C:     	COLOR2;		// Diffuse color + specular mask
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_model2view;
uniform float4x4 	m_model2view2projection;
uniform sampler2D 	s_texture;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_out v_main	( a2v  	IN )
{
  // Calculate eye-space position and normal vectors.
  float3 Pe 	= mul	(m_model2view, 				IN.Position	);
  float3 Ne 	= mul	((float3x3)m_model2view,	IN.Normal	);

  // Write output registers.
  v2p_out 	OUT;
  OUT.HPos 		= mul	(m_model2view2projection,	IN.Position	);
  OUT.Pe 		= float4(Pe.x,Pe.y,Pe.z,0);
  OUT.Ne 		= float4(Ne.x,Ne.y,Ne.z,0);
  OUT.Tex0 		= IN.TexCoords;

  return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
p2f 	p_main	( v2p_in IN )
{
  p2f OUT;

  // No calculations here: just sample texture and out all that shit
  OUT.Pe	= IN.Pe;
  OUT.Ne 	= normalize	(IN.Ne);
  OUT.C 	= tex2D		(s_texture, float2(IN.Tex0.x, IN.Tex0.y));  // float4    (.7,.5,.3,0);
  return OUT;
}
