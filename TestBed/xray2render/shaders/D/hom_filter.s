struct 	a2v
{
	float4 Position: 	POSITION;	// Object-space position
	float4 tc0: 		TEXCOORD0;	// Texture coordinates
	float4 tc1: 		TEXCOORD1;	// Texture coordinates
	float4 tc2: 		TEXCOORD2;	// Texture coordinates
	float4 tc3: 		TEXCOORD3;	// Texture coordinates
	float4 tc4: 		TEXCOORD4;	// Texture coordinates 
	float4 tc5: 		TEXCOORD5;	// Texture coordinates 
	float4 tc6: 		TEXCOORD6;	// Texture coordinates 
	float4 tc7: 		TEXCOORD7;	// Texture coordinates 
};

struct 	v2p_out
{
	float4 HPos: 		POSITION;	// Clip-space position 	(for rasterization)
	float4 tc0: 		TEXCOORD0;	// Texture coordinates
	float4 tc1: 		TEXCOORD1;	// Texture coordinates
	float4 tc2: 		TEXCOORD2;	// Texture coordinates
	float4 tc3: 		TEXCOORD3;	// Texture coordinates
	float4 tc4: 		TEXCOORD4;	// Texture coordinates 
	float4 tc5: 		TEXCOORD5;	// Texture coordinates 
	float4 tc6: 		TEXCOORD6;	// Texture coordinates 
	float4 tc7: 		TEXCOORD7;	// Texture coordinates 
};

struct 	v2p_in
{
	half4	tc0: 		TEXCOORD0;	// Texture coordinates
	half4	tc1: 		TEXCOORD1;	// Texture coordinates
	half4	tc2: 		TEXCOORD2;	// Texture coordinates
	half4	tc3: 		TEXCOORD3;	// Texture coordinates
	half4	tc4: 		TEXCOORD4;	// Texture coordinates
	half4	tc5: 		TEXCOORD5;	// Texture coordinates
	half4	tc6: 		TEXCOORD6;	// Texture coordinates
	half4	tc7: 		TEXCOORD7;	// Texture coordinates
};

struct 	p2f
{
	half4	C: 			COLOR0;		// final maximum
};

//////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D 		s_source;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_out v_main	( a2v  	IN )
{
	v2p_out 	OUT;
	OUT.HPos 	= IN.Position;				// pass-through
	OUT.tc0 	= IN.tc0;
	OUT.tc1 	= IN.tc1;
	OUT.tc2 	= IN.tc2;
	OUT.tc3 	= IN.tc3;
	OUT.tc4 	= IN.tc4;
	OUT.tc5 	= IN.tc5;
	OUT.tc6 	= IN.tc6;
	OUT.tc7 	= IN.tc7;
	return		OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
p2f 	p_main	( v2p_in IN )
{
  p2f		OUT;

  // Sample the source buffer (first 8 samples):
  half _F0		= tex2D 	(s_source,	IN.tc0.xy); 
  half _F1		= tex2D 	(s_source,	IN.tc0.wz); 
  half _F2		= tex2D 	(s_source,	IN.tc1.xy); 
  half _F3		= tex2D 	(s_source,	IN.tc1.wz); 
  half _F4		= tex2D 	(s_source,	IN.tc2.xy); 
  half _F5		= tex2D 	(s_source,	IN.tc2.wz); 
  half _F6		= tex2D 	(s_source,	IN.tc3.xy); 
  half _F7		= tex2D 	(s_source,	IN.tc3.wz); 
  half4 _fR		= half4		(max(_F0,_F1),max(_F2,_F3),max(_F4,_F5),max(_F6,_F7));
 
  // Sample the source buffer (second 8 samples):
  half _S0		= tex2D 	(s_source,	IN.tc4.xy); 
  half _S1		= tex2D 	(s_source,	IN.tc4.wz); 
  half _S2		= tex2D 	(s_source,	IN.tc5.xy); 
  half _S3		= tex2D 	(s_source,	IN.tc5.wz); 
  half _S4		= tex2D 	(s_source,	IN.tc6.xy); 
  half _S5		= tex2D 	(s_source,	IN.tc6.wz); 
  half _S6		= tex2D 	(s_source,	IN.tc7.xy); 
  half _S7		= tex2D 	(s_source,	IN.tc7.wz); 
  half4 _sR		= half4		(max(_S0,_S1),max(_S2,_S3),max(_S4,_S5),max(_S6,_S7));
 
  // mix 2 results
  half4	R		= max(_fR,_sR);
 
  // Final color
  OUT.C 		= max(R.x,max(R.y,max(R.z,R.w)));
  return OUT;
}
