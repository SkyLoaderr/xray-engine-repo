// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CWallmarksEngine
{
public:
	class  wallmark 
	{
	public:
		Fsphere				bounds;
		ref_shader			shader;
		xr_vector<FVF::LIT>	verts;
		float				ttl;
	};
private:
	xr_vector<wallmark*>			pool;
	xr_vector<wallmark*>			marks;
	ref_geom						hGeom;

	xr_vector<CDB::TRI*>			sml_processed;
	Fvector							sml_normal;
	sPoly							sml_poly_dest;
	sPoly							sml_poly_src;
private:
	void		BuildMatrix			(Fmatrix &dest, float invsz, const Fvector& from);
	void		RecurseTri			(CDB::TRI* T, Fmatrix &mView, wallmark	&W, CFrustum &F);

	wallmark*	wm_allocate			(ref_shader&	S	);
	void		wm_render			(wallmark*	W, FVF::LIT* &V);
	void		wm_destroy			(wallmark*	W	);
public:
	void		AddWallmark			(CDB::TRI* tri, const Fvector &contact_point, ref_shader hTexture, float sz);
	void		Render				();

	CWallmarksEngine				();
	~CWallmarksEngine				();
};
