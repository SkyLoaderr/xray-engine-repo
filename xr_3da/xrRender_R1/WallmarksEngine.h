// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CWallmarksEngine
{
public:
	struct wallmark 
	{
		Fsphere				bounds;
		xr_vector<FVF::LIT>	verts;
		float				ttl;
	};
	DEFINE_VECTOR	(wallmark*,WMVec,WMVecIt);
	struct wm_slot
	{
		ref_shader			shader;
		WMVec				items;
							wm_slot	(	ref_shader sh)		{shader=sh;items.reserve(256);}
	};
	DEFINE_VECTOR	(wm_slot*,WMSVec,WMSVecIt);
private:
	WMVec							pool;
	WMSVec							marks;
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
	wm_slot*						FindSlot	(ref_shader shader);
	wm_slot*						AppendSlot	(ref_shader shader);
private:
	void		BuildMatrix			(Fmatrix &dest, float invsz, const Fvector& from);
	void		RecurseTri			(u32 T,	Fmatrix &mView, wallmark	&W);
	void		AddWallmark_internal(CDB::TRI* pTri, const Fvector* pVerts, const Fvector &contact_point, ref_shader hTexture, float sz);

	wallmark*	wm_allocate			();
	void		wm_render			(wallmark*	W, FVF::LIT* &V);
	void		wm_destroy			(wallmark*	W	);

	void		clear				();
public:
	void		AddWallmark			(CDB::TRI* pTri, const Fvector* pVerts, const Fvector &contact_point, ref_shader hTexture, float sz);
	void		Render				();

	void		load_LevelWallmarks	(LPCSTR fn);
	void		unload_LevelWallmarks();

	CWallmarksEngine				();
	~CWallmarksEngine				();
};
