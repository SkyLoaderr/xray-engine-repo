// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CWallmarksEngine
{
public:
	struct static_wallmark 
	{
		Fsphere				bounds;
		xr_vector<FVF::LIT>	verts;
		float				ttl;
	};
	DEFINE_VECTOR		(static_wallmark*,StaticWMVec,StaticWMVecIt);
	struct wm_slot
	{
		ref_shader		shader;
		StaticWMVec		static_items;
		xr_vector<CSkeletonWallmark*>	skeleton_items;
						wm_slot		(ref_shader sh)	{shader=sh;static_items.reserve(256);skeleton_items.reserve(256);}
	};
	DEFINE_VECTOR		(wm_slot*,WMSlotVec,WMSlotVecIt);
private:
	xr_vector<CSkeletonWallmark*>		skeleton_pool;
	StaticWMVec							static_pool;
	WMSlotVec			marks;
	ref_geom			hGeom;

	Fvector				sml_normal;
	CFrustum			sml_clipper;
	sPoly				sml_poly_dest;
	sPoly				sml_poly_src;

	xrXRC				xrc;
	CDB::Collector		sml_collector;
	xr_vector<u32>		sml_adjacency;

	xrCriticalSection	lock;
private:
	wm_slot*			FindSlot				(ref_shader shader);
	wm_slot*			AppendSlot				(ref_shader shader);
private:
	void				BuildMatrix				(Fmatrix &dest, float invsz, const Fvector& from);
	void				RecurseTri				(u32 T,	Fmatrix &mView, static_wallmark	&W);
	void				AddWallmark_internal	(CDB::TRI* pTri, const Fvector* pVerts, const Fvector &contact_point, ref_shader hTexture, float sz);

	static_wallmark*	static_wm_allocate		();
	void				static_wm_render		(static_wallmark*	W, FVF::LIT* &V);
	void				static_wm_destroy		(static_wallmark*	W	);

	CSkeletonWallmark*	skeleton_wm_allocate	();
	void				skeleton_wm_render		(CSkeletonWallmark*	W, FVF::LIT* &V);
	void				skeleton_wm_destroy		(CSkeletonWallmark*	W	);

	void				clear					();
public:
	CWallmarksEngine							();
	~CWallmarksEngine							();
	// edit wallmarks
	void				AddStaticWallmark		(CDB::TRI* pTri, const Fvector* pVerts, const Fvector &contact_point, ref_shader hTexture, float sz);
	CSkeletonWallmark*	AllocateSkeletonWallmark();
	void				AddSkeletonWallmark		(CSkeletonWallmark* wm);
	void				RemoveSkeletonWallmark	(CSkeletonWallmark*& wm);

	// render
	void				Render					();

	// IO level wallmarks
	void				load_LevelWallmarks		(LPCSTR fn);
	void				unload_LevelWallmarks	();
};
