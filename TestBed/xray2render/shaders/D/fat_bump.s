struct 	a2v
{
  float4 Position: 	POSITION;	// Object-space position
  float3 N: 		NORMAL;		// Object-space normal
  float2 tc0:		TEXCOORD0;	// Texture coordinates
  float3 T:			TANGENT;	// tangent
  float3 B:			BINORMAL;	// binormal
};

struct 	v2p_out
{
  float4 HPos: 		POSITION;	// Clip-space position 	(for rasterization)
  float2 tc0: 		TEXCOORD0;	// Texture coordinates 	(for sampling diffuse+normal map)
  float3 Pe: 		TEXCOORD1;	// Eye-space position 	(for lighting)
  float3 M1:		TEXCOORD2;	// nmap 2 eye - 1
  float3 M2:		TEXCOORD3;	// nmap 2 eye - 2
  float3 M3:		TEXCOORD4;	// nmap 2 eye - 3
};

struct 	v2p_in
{
  half2	tc0: 		TEXCOORD0;	// Texture coordinates 	(for sampling diffuse+normal map)
  half3	Pe: 		TEXCOORD1;	// Eye-space position 	(for lighting)
  half3 M1:			TEXCOORD2;	// nmap 2 eye - 1
  half3 M2:			TEXCOORD3;	// nmap 2 eye - 2
  half3 M3:			TEXCOORD4;	// nmap 2 eye - 3
};

struct 	p2f
{
  half4 Pe: 		COLOR0;		// Eye-space position 	(for lighting)
  half4 Ne:			COLOR1;		// Eye-space normal		(for lighting)
  half4 C:     		COLOR2;		// Diffuse color + specular mask
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform float3x4 	m_model2view;
uniform float4x4 	m_model2view2projection;
uniform sampler2D 	s_texture;
uniform sampler2D 	s_nmap;
uniform samplerCUBE s_NCM;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_out v_main	( a2v  	IN )
{
	v2p_out 		OUT;
  
	// Positions and TC
	OUT.HPos 		= mul	(m_model2view2projection,	IN.Position	);
	float3 Pe 		= mul	(m_model2view, 				IN.Position	);
	OUT.Pe 			= float4(Pe.x,Pe.y,Pe.z,0);
	OUT.tc0 		= IN.tc0;

	// Calculate the 3x3 transform from tangent space to eye-space
	// TangentToEyeSpace = object2eye * tangent2object
	//					 = object2eye * transpose(object2tangent) (since the inverse of a rotation is its transpose)
	float3x3 xform	= mul	((float3x3)m_model2view, 
								float3x3(
								IN.T.x,IN.B.x,IN.N.x,
								IN.T.y,IN.B.y,IN.N.y,
								IN.T.z,IN.B.z,IN.N.z
								)
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
  OUT.Pe	= half4		(IN.Pe.x,IN.Pe.y,IN.Pe.z,0);
  half4 D	= tex2D		(s_texture, IN.tc0);				// Diffuse map only

  // Sample normal and rotate it by matrix
  half4 Nu	= tex2D		(s_nmap,	IN.tc0);				// Unsigned normal and gloss
  half3 Ns	= ((half3)Nu)*2 - half3(1,1,1);					// Signed normal
  half3 Ne	= mul		(half3x3(IN.M1, IN.M2, IN.M3), Ns);
  half3 NeN	= normalize	(Ne);								// texCUBE	(s_NCM,Ne);	
  OUT.Ne 	= half4		(NeN.x,	NeN.y,	NeN.z,	0);
  OUT.C		= half4		(D.x,	D.y,	D.z,	Nu.w);
  return OUT;
}
