// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CWallmarksEngine
{
public:
	enum EWMType{
		wmtFirst		= 0,
		wmtStatic		= 0,
		wmtSkeleton,
		wmtLevel,
		wmtLast
	};
	struct wm_slot_custom
	{
		ref_shader		shader;
						wm_slot_custom	(ref_shader sh)	{shader=sh;}
		virtual void	render			(ref_geom hGeom, ref_shader shader, u32& w_offset, FVF::LIT*& w_verts, FVF::LIT*& w_start);
		virtual void	clear			()=0;
		bool			operator ==		(ref_shader sh) const {return shader==sh;}
	};
	struct wm_slot_static:public wm_slot_custom
	{
		struct static_wallmark 
		{
			Fsphere				bounds;
			xr_vector<FVF::LIT>	verts;
			float				ttl;
		};
		DEFINE_VECTOR		(static_wallmark*,StaticWMVec,StaticWMVecIt);
		StaticWMVec		pool;
		StaticWMVec		items;
						wm_slot_static	(ref_shader sh):wm_slot_custom(sh){items.reserve(256);}
		virtual void	render			(ref_geom hGeom, ref_shader shader, u32& w_offset, FVF::LIT*& w_verts, FVF::LIT*& w_start);
		virtual void	clear			(){
//.			for (StaticWMVecIt m_it=items.begin(); m_it!=items.end(); m_it++)
//.				static_wm_destroy	(*m_it);
		}
	};
	struct wm_slot_skeleton:public wm_slot_custom
	{
		xr_vector<CSkeletonWallmark*>	items;
						wm_slot_skeleton(ref_shader sh):wm_slot_custom(sh){items.reserve(256);}
		virtual void	render			(ref_geom hGeom, ref_shader shader, u32& w_offset, FVF::LIT*& w_verts, FVF::LIT*& w_start);
		virtual void	clear			();
	};
	struct wm_slot_level:public wm_slot_static
	{
						wm_slot_level	(ref_shader sh):wm_slot_static(sh){}
		virtual void	render			(ref_geom hGeom, ref_shader shader, u32& w_offset, FVF::LIT*& w_verts, FVF::LIT*& w_start);
		virtual void	clear			();
	};
	DEFINE_VECTOR		(wm_slot_custom*,	WMSlotVec,WMSlotVecIt);
	DEFINE_SVECTOR		(WMSlotVec,wmtLast,WMTSlotVec,WMTSlotVecIt);
private:
	WMTSlotVec			slots;
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
	template			<class T>
	T*					FindSlot				(EWMType type, ref_shader shader)
	{
		WMSlotVec& slot_vec	= marks[type];
		WMSlotVecIt it		= std::find(slot_vec.begin(),slot_vec.end(),shader);
		return				(it!=slot_vec.end())?(T*)*it:0;
	}
	template			<class T>
	T*					AppendSlot				(EWMType type, ref_shader shader)
	{
		T* slot			= xr_new<T>(shader);
		WMSlotVec& slot_vec	= marks[type];
		slot_vec.push_back	(slot);
		return slot;
	}
private:
	void				BuildMatrix				(Fmatrix &dest, float invsz, const Fvector& from);
	void				RecurseTri				(u32 T,	Fmatrix &mView, static_wallmark	&W);
	void				AddWallmark_internal	(CDB::TRI* pTri, const Fvector* pVerts, const Fvector &contact_point, ref_shader hTexture, float sz);

	static_wallmark*	static_wm_allocate		();
	void				static_wm_destroy		(static_wallmark*	W	);

	void				skeleton_wm_render		(CSkeletonWallmark*	W, FVF::LIT* &V);

	void				clear					();
public:
	CWallmarksEngine							();
	~CWallmarksEngine							();
	// edit wallmarks
	void				AddStaticWallmark		(CDB::TRI* pTri, const Fvector* pVerts, const Fvector &contact_point, ref_shader hTexture, float sz);
	void				AddSkeletonWallmark		(CSkeletonWallmark* wm);
	void				AddSkeletonWallmark		(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size);

	// render
	void				Render					();

	// IO level wallmarks
	void				load_LevelWallmarks		(LPCSTR fn);
	void				unload_LevelWallmarks	();
};
