// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WALLMARKSENGINE_H__10192F6D_882B_440F_818C_888FB47AEB02__INCLUDED_)
#define AFX_WALLMARKSENGINE_H__10192F6D_882B_440F_818C_888FB47AEB02__INCLUDED_
#pragma once

const float W_TTL			= 15.f;
const float W_DIST_FADE		= 15.f;
const float	W_DIST_FADE_SQR	= W_DIST_FADE*W_DIST_FADE;
const float I_DIST_FADE_SQR	= 1.f/W_DIST_FADE_SQR;

struct CWallmark 
{
	struct Vertex 
	{
		Fvector P;
		DWORD	C;
		float	tu,tv;
		IC void set(Fvector& V, DWORD _C, float u, float v)
		{
			P.set(V); C=_C; tu=(u+1)*0.5f; tv=(v+1)*0.5f;
		}
	};

	Fsphere			S;
	Shader*			hShader;
	vector<Vertex>	verts;
	float			ttl;
	
	IC void		Create	(Shader* hTex)
	{
		hShader		= hTex;
		ttl			= W_TTL;
	};
	IC void		Draw	(Vertex*	&D) 
	{
		float	a	= 1-ttl/W_TTL;			clamp	(a,0.f,1.f);
		DWORD		C	= iFloor( a * 255.f);
		C = D3DCOLOR_RGBA(128,128,128,C);

		for (Vertex* S=verts.begin(); S!=verts.end(); S++,D++)
		{
			D->P.set(S->P);
			D->C	= C;
			D->tu	= S->tu;
			D->tv	= S->tv;
		}
	};
	IC void		Copy	(CWallmark& from)
	{
		S		= from.S;
		hShader	= hShader;
		verts	= from.verts;
		ttl		= from.ttl;
	}
};

class ENGINE_API	CWallmarksEngine
{
	deque<CWallmark>		marks;
	CVertexStream*			VS;

	void	BuildMatrix		(Fmatrix &dest, float invsz, const Fvector& from);
public:
	void	AddWallmark		(CDB::TRI* pPickedTri, const Fvector &contact_point, Shader* hTexture, float sz);
	void	Render			();

	CWallmarksEngine		();
	~CWallmarksEngine		();
};

#endif // !defined(AFX_WALLMARKSENGINE_H__10192F6D_882B_440F_818C_888FB47AEB02__INCLUDED_)
