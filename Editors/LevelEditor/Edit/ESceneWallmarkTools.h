//---------------------------------------------------------------------------

#ifndef ESceneWallmarkToolsH
#define ESceneWallmarkToolsH

#include "ESceneCustomMTools.H"

class ESceneWallmarkTools: public ESceneCustomMTools
{
	typedef ESceneCustomMTools inherited;
public:    
	DEFINE_VECTOR		(FVF::LIT,LITVertVec,LITVertVecIt);
	struct wallmark 
	{
    	enum{
        	flSelected	= (1<<0)
        };
        Flags8			flags;
        Fbox			bbox;
		Fsphere			bounds;
		LITVertVec		verts;
	};
	DEFINE_VECTOR		(wallmark*,WMVec,WMVecIt);
	struct wm_slot
	{
		shared_str			tex_name;
		ref_shader		shader;
		WMVec			items;
						wm_slot	(shared_str tname)		{tex_name=tname;shader.create("effects\\wallmark",*tex_name);items.reserve(256);}
	};
	DEFINE_VECTOR		(wm_slot*,WMSVec,WMSVecIt);
	WMSVec				marks;
	WMVec				pool;
private:
	ref_geom			hGeom;

	Fvector				sml_normal;
	CFrustum			sml_clipper;
	sPoly				sml_poly_dest;
	sPoly				sml_poly_src;

	CDB::Collector 		sml_collector;
	xr_vector<u32>		sml_adjacency;

	wm_slot*			FindSlot				(shared_str texture);
	wm_slot*			AppendSlot				(shared_str texture);

    void 				RecurseTri				(u32 t, Fmatrix &mView, wallmark &W);
	void 				BuildMatrix				(Fmatrix &mView, float invsz, const Fvector& from);
//	void 				AddWallmark_internal	(ref_shader hShader, const Fvector& pt, const Fvector& n, float sz);
private:
	wallmark*			wm_allocate				();
	void				wm_render				(wallmark*	W, FVF::LIT* &V);
	void				wm_destroy				(wallmark*	W);
private:
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
	enum{
    	flDrawWallmark	= (1<<0),
    };
    Flags32				m_Flags;

    float				m_MarkSize;
    float				m_MarkRotate;
    shared_str			m_TexName;

    int					ObjectCount				();
public:
						ESceneWallmarkTools    	();
	virtual        	 	~ESceneWallmarkTools 	();

    virtual void		OnObjectRemove			(CCustomObject* O){}
    virtual	void		UpdateSnapList			(){}
	virtual ObjectList*	GetSnapList				(){return 0;}

	// selection manipulate
	virtual int			RaySelect				(int flag, float& distance, const Fvector& start, const Fvector& direction, BOOL bDistanceOnly);
	virtual int	   		FrustumSelect			(int flag, const CFrustum& frustum);
	virtual void   		SelectObjects           (bool flag);
	virtual void   		InvertSelection         ();
	virtual void		RemoveSelection         ();
	virtual int    		SelectionCount          (bool testflag);
	virtual void		ShowObjects				(bool flag, bool bAllowSelectionFlag=false, bool bSelFlag=true){}

    virtual void		Clear					(bool bOnlyNodes=false); 

	// definition
    IC LPCSTR			ClassName				(){return "wallmark";}
    IC LPCSTR			ClassDesc				(){return "Wallmark";}
    IC int				RenderPriority			(){return 10;}

    // validation
    virtual bool   		Valid					();
    virtual bool		Validate				(){return true;}
    virtual bool   		IsNeedSave				();

    // events
	virtual void   		OnFrame					();
    virtual void   		OnRender				(int priority, bool strictB2F);

    // IO
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);
    virtual bool   		Export          		(LPCSTR fn);

	// device dependent funcs    
	virtual void		OnDeviceCreate			();
	virtual void		OnDeviceDestroy			();

	virtual void		OnSynchronize			();

	virtual bool 		GetSummaryInfo			(SSceneSummary* inf){return false;} 

	// properties
    virtual void		FillProp          		(LPCSTR pref, PropItemVec& items);

    // utils
	BOOL 				AddWallmark				(const Fvector& start, const Fvector& dir);
};
#endif
