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

	Fvector							sml_normal;
	CFrustum						sml_clipper;
	sPoly							sml_poly_dest;
	sPoly							sml_poly_src;

	xrXRC							xrc;
	CDB::Collector					sml_collector;
	xr_vector<u32>					sml_adjacency;

	xrCriticalSection				lock;
private:
	void		BuildMatrix			(Fmatrix &dest, float invsz, const Fvector& from);
	void		RecurseTri			(u32 T,	Fmatrix &mView, wallmark	&W);
	void		AddWallmark_internal(CDB::TRI* tri, const Fvector &contact_point, ref_shader hTexture, float sz);

	wallmark*	wm_allocate			(ref_shader&	S	);
	void		wm_render			(wallmark*	W, FVF::LIT* &V);
	void		wm_destroy			(wallmark*	W	);
public:
	void		AddWallmark			(CDB::TRI* tri, const Fvector &contact_point, ref_shader hTexture, float sz);
	void		Render				();

	CWallmarksEngine				();
	~CWallmarksEngine				();
};
