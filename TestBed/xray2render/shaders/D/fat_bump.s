struct 	a2v
{
  float4 Position: 	POSITION;	// Object-space position
  float3 N: 		NORMAL;		// Object-space normal
  float2 tc0:		TEXCOORD0;	// Texture coordinates
  float3 T:			TEXCOORD1;	// tangent
  float3 B:			TEXCOORD2;	// binormal
};

struct 	v2p_out
{
  float4 HPos: 		POSITION;	// Clip-space position 	(for rasterization)
  float3 Pe: 		TEXCOORD0;	// Eye-space position 	(for lighting)
  float2 tc0: 		TEXCOORD1;	// Texture coordinates 	(for sampling diffuse+normal map)
  float3 M1:		TEXCOORD2;	// nmap 2 eye - 1
  float3 M2:		TEXCOORD3;	// nmap 2 eye - 2
  float3 M3:		TEXCOORD4;	// nmap 2 eye - 3
};

struct 	v2p_in
{
  float3 Pe: 		TEXCOORD0;	// Eye-space position 	(for lighting)
  float2 tc0: 		TEXCOORD1;	// Texture coordinates 	(for sampling diffuse+normal map)
  float3 M1:		TEXCOORD2;	// nmap 2 eye - 1
  float3 M2:		TEXCOORD3;	// nmap 2 eye - 2
  float3 M3:		TEXCOORD4;	// nmap 2 eye - 3
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
uniform sampler2D 	s_nmap;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_out v_main	( a2v  	IN )
{
	v2p_out 	OUT;
  
	// Positions and TC
	OUT.HPos 		= mul	(m_model2view2projection,	IN.Position	);
	float3 Pe 		= mul	(m_model2view, 				IN.Position	);
	OUT.Pe 			= float4(Pe.x,Pe.y,Pe.z,0);
	OUT.tc0 		= IN.tc0;

	// Calculate the 3x3 transform from tangent space to eye-space
	// TangentToEyeSpace = object2eye * tangent2object
	//					 = object2eye * transpose(object2tangent) (since the inverse of a rotation is its transpose)
	float3x3 xform	= mul	(m_model2view, float3x3(
								IN.T.x,IN.B.y,IN.N.z,
								IN.T.x,IN.B.y,IN.N.z,
								IN.T.x,IN.B.y,IN.N.z) 
							);
  
	// Feed this transform to pixel shader
	OUT.M1 			= xform[0]; 
	OUT.M2 			= xform[1]; 
	OUT.M3 			= xform[2]; 

	return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
p2f 	p_main	( v2p_in IN )
{
  p2f OUT;

  // Transfer position and sample diffuse
  OUT.Pe	= float4	(IN.Pe.x,IN.Pe.y,IN.Pe.z,0);
  OUT.C 	= tex2D		(s_texture, IN.tc0);

  // Sample normal and rotate it by matrix
  float3 N	= tex2D		(s_nmap,	IN.tc0);
  float3 Ne	= mul		(float3x3(
							IN.M1.x,IN.M1.y,IN.M1.z,
							IN.M2.x,IN.M2.y,IN.M2.z,
							IN.M3.x,IN.M3.y,IN.M3.z),
							N);

  float3 NeN= normalize	(Ne);
  OUT.Ne 	= float4	(NeN.x,NeN.y,NeN.z,0);
  return OUT;
}
