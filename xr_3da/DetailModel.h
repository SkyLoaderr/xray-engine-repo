#pragma once

class ENGINE_API CDetail 
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
		DWORD	C;
		float	u,v;
	};
public:
	Fsphere		bv_sphere;
	Fbox		bv_bb;
	DWORD		flags;	
	float		s_min;
	float		s_max;
	
	Shader*		shader;	
	fvfVertexIn	*vertices;
	DWORD		number_vertices;
	WORD		*indices;
	DWORD		number_indices;
	
	void		Load		(CStream* S);
	void		Optimize	();
	void		Unload		();
	void		Transfer	(Fmatrix& mXform, fvfVertexOut* vDest, DWORD C, WORD* iDest, DWORD iOffset);
};

