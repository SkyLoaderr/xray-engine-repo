struct 	a2v
{
  float4 Position: 	POSITION;	// Object-space position
  float2 tc0: 		TEXCOORD0;	// Texture coordinates
  float2 tc1: 		TEXCOORD1;	// Texture coordinates
  float2 tc2: 		TEXCOORD2;	// Texture coordinates
  float2 tc3: 		TEXCOORD3;	// Texture coordinates
};

struct 	v2p_out
{
  float4 HPos: 		POSITION;	// Clip-space position 	(for rasterization)
  float2 tc0: 		TEXCOORD0;	// Texture coordinates 	(for sampling maps)
  float2 tc1: 		TEXCOORD1;	// Texture coordinates 	(for sampling maps)
  float2 tc2: 		TEXCOORD2;	// Texture coordinates 	(for sampling maps)
  float2 tc3: 		TEXCOORD3;	// Texture coordinates 	(for sampling maps)
};

struct 	v2p_in
{
  half2 tc0: 		TEXCOORD0;	// Texture coordinates 	(for sampling maps)
  half2 tc1: 		TEXCOORD1;	// Texture coordinates 	(for sampling maps)
  half2 tc2: 		TEXCOORD2;	// Texture coordinates 	(for sampling maps)
  half2 tc3: 		TEXCOORD3;	// Texture coordinates 	(for sampling maps)
};

struct 	p2f
{
  half4 C:			COLOR0;		// Final color
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
  OUT.tc0 	= IN.tc0;
  OUT.tc1 	= IN.tc1;
  OUT.tc2 	= IN.tc2;
  OUT.tc3 	= IN.tc3;
  return	OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
p2f 	p_main	( v2p_in IN )
{
	p2f		OUT;
  
	half4		accum,D,L,C,S;
	
	// 1
	half4 D		=	tex2D		(s_diffuse,		IN.tc0);
	half4 L 	=	tex2D		(s_accumulator, IN.tc0);
	C			=	D*L;							// rgb.gloss * light(rgb.specular)
	S			=	half4		(C.w,C.w,C.w,C.w);	// replicated specular
	accum		=	C+S;

	// 2
	half4 D		=	tex2D		(s_diffuse,		IN.tc1);
	half4 L 	=	tex2D		(s_accumulator, IN.tc1);
	C			=	D*L;							// rgb.gloss * light(rgb.specular)
	S			=	half4		(C.w,C.w,C.w,C.w);	// replicated specular
	accum		+=	C+S;
	
	OUT.C 		= accum - half4(1,1,1,1);
	return OUT;
}
