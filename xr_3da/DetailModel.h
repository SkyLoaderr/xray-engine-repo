#pragma once

class CDetail 
{
public:
	struct fvfVertexIn
	{
		Fvector P;
		float	u,v;
	};
	struct fvfVertexOut
	{
		Fvector P;
		u32	C;
		float	u,v;
	};
public:
	Fsphere		bv_sphere;
	Fbox		bv_bb;
	u32			flags;	
	float		s_min;
	float		s_max;
	
	Shader*		shader;		// 0=wave, 1=still
	fvfVertexIn	*vertices;
	u32			number_vertices;
	WORD		*indices;
	u32			number_indices;
	
	void		Load		(IReader* S);
	void		Optimize	();
	void		Unload		();
	void		Transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, WORD* iDest, u32 iOffset);
};

