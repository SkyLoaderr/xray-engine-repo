#pragma once

struct st_VERT;
void RegisterVERT(st_VERT* v);

struct st_VERT {
	Fvector		P;
	Fvector		O;
	Fvector		N;
	DWORD		sm_group;
	float		u,v;
	DWORD		bone;

	st_VERT()
	{
		P.set	(0,0,0);
		N.set	(0,1,0);
		u = v = 0;
		bone	= BONE_NONE;
		RegisterVERT(this);
	}
	void	SetUV(float _u, float _v)
	{
		u = _u; v = _v;
	}
	void	SetBone(DWORD B) 
	{
		if (bone==BONE_NONE)
		{
			bone = B;
		} else R_ASSERT(bone==B);
	}
	BOOL	similar(st_VERT& V)
	{
		if (bone!=V.bone)			return FALSE;
		if (!fsimilar	(u,V.u))	return FALSE;
		if (!fsimilar	(v,V.v))	return FALSE;
		if (!O.similar	(V.O))		return FALSE;
		if (!N.similar	(V.N))		return FALSE;
		return TRUE;
	}
};

DEF_VECTOR(vVERT,st_VERT*);

