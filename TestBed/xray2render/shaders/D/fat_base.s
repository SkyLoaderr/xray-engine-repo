struct 	a2v
{
  float4 Position: 	POSITION;	// Object-space position
  float4 Normal: 	NORMAL;		// Object-space normal
  float4 tc0:		TEXCOORD0;	// Texture coordinates
};

struct 	v2p_out
{
  float4 HPos: 		POSITION;	// Clip-space position 	(for rasterization)
  float3 Pe: 		TEXCOORD0;	// Eye-space position 	(for lighting)
  float3 Ne: 		TEXCOORD1;	// Eye-space normal	(for lighting)
  float2 tc0: 		TEXCOORD2;	// Texture coordinates 	(for sampling diffuse map)
};

struct 	v2p_in
{
  float3 Pe: 		TEXCOORD0;	// Eye-space position 	(for lighting)
  float3 Ne: 		TEXCOORD1;	// Eye-space normal	(for lighting)
  float2 tc0: 		TEXCOORD2;	// Texture coordinates 	(for sampling diffuse map)
};

struct 	p2f
{
  half4 Pe: 		COLOR0;		// Eye-space position 	(for lighting)
  half4 Ne:			COLOR1;		// Eye-space normal	(for lighting)
  half4 C:     		COLOR2;		// Diffuse color + specular mask
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_model2view;
uniform float4x4 	m_model2view2projection;
uniform sampler2D 	s_texture;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_out v_main	( a2v  	IN )
{
  // Eye-space pos/normal
  v2p_out 	OUT;
  OUT.HPos 		= mul	(m_model2view2projection,	IN.Position	);
  OUT.Pe 		= mul	(m_model2view, 				IN.Position	);
  OUT.Ne 		= mul	((float3x3)m_model2view,	IN.Normal	);
  OUT.tc0 		= IN.tc0;

  return	OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
p2f 	p_main	( v2p_in IN )
{
  p2f		OUT;

  // No calculations here: just sample texture and out all that shit
  OUT.Pe	= float4	(IN.Pe.x,IN.Pe.y,IN.Pe.z,0);
  half3	Ne	= normalize	(IN.Ne);
  OUT.Ne 	= float4	(Ne.x,Ne.y,Ne.z,0);
  OUT.C 	= tex2D		(s_texture, IN.tc0);  // float4    (.7,.5,.3,0);
  return OUT;
}
