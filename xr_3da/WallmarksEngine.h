// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "frustum.h"
class ENGINE_API	CWallmarksEngine
{
public:
	class  wallmark 
	{
	public:
		Fsphere				bounds;
		Shader*				shader;
		vector<FVF::LIT>	verts;
		float				ttl;
	};
private:
	vector<wallmark*>				pool;
	vector<wallmark*>				marks;
	CVertexStream*					VS;

	vector<CDB::TRI*>				sml_processed;
	Fvector							sml_normal;
	sPoly							sml_poly_dest;
	sPoly							sml_poly_src;
private:
	void		BuildMatrix			(Fmatrix &dest, float invsz, const Fvector& from);
	void		RecurseTri			(CDB::TRI* T, Fmatrix &mView, wallmark	&W, CFrustum &F);

	wallmark*	wm_allocate			(Shader*	S	);
	void		wm_render			(wallmark*	W, FVF::LIT* &V);
	void		wm_destroy			(wallmark*	W	);
public:
	void		AddWallmark			(CDB::TRI* tri, const Fvector &contact_point, Shader* hTexture, float sz);
	void		Render				();

	CWallmarksEngine				();
	~CWallmarksEngine				();
};
