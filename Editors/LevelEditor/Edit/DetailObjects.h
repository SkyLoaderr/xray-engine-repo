//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------

#ifndef _INCDEF_DetailObjects_H_
#define _INCDEF_DetailObjects_H_

#include "CustomObject.h"
#include "DetailFormat.h"
#include "Library.h"

class CFrustum;
class CEditableObject;
class Shader;

struct SIndexDist{
    DWORD 	index;
    float 	dist;
    float	dens[4];
    float	cnt[4];
	SIndexDist(){ZeroMemory(this,sizeof(SIndexDist));}
};
DEFINE_SVECTOR		(SIndexDist,4,SIndexDistVec,SIndexDistIt);

const int		dm_max_objects	= 64;
const int		dm_obj_in_slot	= 4;
const int		dm_size			= 8;
const int		dm_cache_line	= 1+dm_size+1+dm_size+1;
const int		dm_cache_size	= dm_cache_line*dm_cache_line;

const DWORD F_DOV = D3DFVF_XYZ | D3DFVF_TEX1;
class CDetail{
	friend class CDetailManager;
    friend class TfrmDOShuffle;

	struct fvfVertexIn{
		Fvector 		P;
		float			u,v;
        				fvfVertexIn(const Fvector& _P, float _u, float _v){P.set(_P); u=_u; v=_v;};
        void			set(fvfVertexIn& src){P.set(src.P); u=src.u; v=src.v;};
        void			set(const Fvector& _P, float _u, float _v){P.set(_P); u=_u; v=_v;};
        BOOL			similar(fvfVertexIn& V)
        {
            if (!fsimilar	(u,V.u,EPS_L))	return FALSE;
            if (!fsimilar	(v,V.v,EPS_L))	return FALSE;
            if (!P.similar	(V.P,EPS_L))	return FALSE;
            return TRUE;
        }
	};
    DEFINE_VECTOR		(fvfVertexIn,DOVertVec,DOVertIt);
    float				m_fMinScale;
    float				m_fMaxScale;

    float 				m_fDensityFactor;

	// render
    DOVertVec			m_Vertices;
    WORDVec				m_Indices;
	Shader*				m_pShader;
	DWORD				m_dwFlags;
	float				m_fRadius;
    bool				m_bSideFlag;

    // references
    AnsiString			m_sRefs;
	CEditableObject*	m_pRefs;

	int 				_AddVert		(const Fvector& p, float u, float v);
public:
    bool				m_bMarkDel;
public:
						CDetail			();
	virtual             ~CDetail		();

	bool				Load            (CStream&);
	void				Save            (CFS_Base&);
    void				Export			(CFS_Base&);

	bool				Update			(LPCSTR name);

    LPCSTR				GetName			();
	void				OnDeviceCreate	();
	void				OnDeviceDestroy	();
};

DEFINE_VECTOR(CDetail*,DOVec,DOIt);

DEFINE_VECTOR(DetailSlot,DSVec,DSIt);

DEFINE_MAP(DWORD,DOVec,ColorIndexMap,ColorIndexPairIt);

#define DETAIL_SLOT_SIZE_2 	DETAIL_SLOT_SIZE*0.5f
#define DETAIL_SLOT_RADIUS	DETAIL_SLOT_SIZE*0.70710678118654752440084436210485f

class CDetailManager:
	public pureDeviceCreate,
	public pureDeviceDestroy
{
	friend class TfrmDOShuffle;

	DetailHeader		m_Header;
    DSVec		 		m_Slots;
	DOVec				m_Objects;

    ObjectList			m_SnapObjects;

    Fbox				m_BBox;

	IC DWORD			toSlotX			(float x)	{return (x/DETAIL_SLOT_SIZE+0.5f)+m_Header.offs_x;}
	IC DWORD			toSlotZ			(float z)	{return (z/DETAIL_SLOT_SIZE+0.5f)+m_Header.offs_z;}
	IC float			fromSlotX		(int x)		{return (x-m_Header.offs_x)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}
	IC float			fromSlotZ		(int z)		{return (z-m_Header.offs_z)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}

    void				UpdateSlotBBox	(int x, int z, DetailSlot& slot);

    void				GetSlotRect		(Frect& rect, int sx, int sz);
    void				GetSlotTCRect	(Irect& rect, int sx, int sz);
    BYTE				GetRandomObject	(DWORD color_index);
    BYTE				GetObject		(ColorIndexPairIt& CI, BYTE id);

	void 				CalcClosestCount(int part, const Fcolor& C, SIndexDistVec& best);
	void 				FindClosestIndex(const Fcolor& C, SIndexDistVec& best);

    DetailSlot&			GetSlot			(DWORD sx, DWORD sz);
public:
// render part -----------------------------------------------------------------
	int					m_Dither		[16][16];

	struct	SlotItem
	{
		Fvector			P;
		float			scale;
		float			phase_x;
		float			phase_z;
		DWORD			C;
        DWORD			flag;

		float			scale_calculated;
		Fmatrix			mRotY;
	};
	struct	SlotPart
	{
		DWORD		   	id;
		vector<SlotItem>items;
	};
	enum	SlotType
	{
		stReady			= 0,// Ready to use
		stInvalid,			// Invalid cache entry
		stPending,			// Pending for decompression

		stFORCEDWORD 	= 0xffffffff
	};
	struct Slot
	{
		DWORD		  	type;
		int			  	sx,sz;
		DWORD		  	dwFrameUsed;	// LRU cache
		Fbox		  	BB;
		SlotPart	  	G[dm_obj_in_slot];
	};

	CVertexStream*		VS;
	svector<Slot,dm_cache_size>					m_Cache;
	svector<vector<SlotItem*>,dm_max_objects> 	m_Visible;

    void 				InitRender				();
	void				Decompress				(int sx, int sz, Slot& D);
	Slot&				Query					(int sx, int sz);
	DetailSlot&			QueryDB					(int sx, int sz);
	void				UpdateCache				(int limit);
	void				RenderObjects			(const Fvector& EYE);
    void				RenderTexture			(float alpha);
    void				InvalidateCache			();
// render part -----------------------------------------------------------------
public:
	class SBase{
		Shader*			shader_blended;
        Shader*			shader_overlap;
        string128		name;
        int 			w;
        int 			h;
		DWORDVec		data;
	    DEFINE_VECTOR	(FVF::V,TVertVec,TVertIt);
		TVertVec		mesh;
	    CVertexStream*	stream;
    public:
        				SBase				();
        IC bool			Valid				(){return (w>0)&&(h>0)&&(!!data.size());}
    	IC void			Clear				(){name[0]=0; w=0; h=0; stream=0; data.clear(); mesh.clear(); DestroyShader();}
        void			CreateRMFromObjects	(const Fbox& box, ObjectList& lst);
        void			Render				();
        void			CreateShader		();
        void			DestroyShader		();
        void			RecreateShader		(){DestroyShader();CreateShader();}
        bool			LoadImage			(LPCSTR nm);
		IC LPCSTR 		GetName				(){ return name; }
		IC bool 		GetColor			(DWORD& color, int U, int V){
        	if (Valid()&&(U<w)&&(V<h)){
    			color 	= data[V*w+U];
    			return true;
            }
            return false;
	    }
        IC float 		GetUFromX			(float x, const Fbox& box){
			R_ASSERT(Valid());
			return 		(x-box.min.x)/(box.max.x-box.min.x);
        }
        IC int			GetPixelUFromX		(float x, const Fbox& box){
        	int U		= iFloor(GetUFromX(x,box)*(w-1)+0.5f); if (U<0) U=0;
			return U;
        }
        IC float 		GetVFromZ			(float z, const Fbox& box){
			R_ASSERT(Valid());
			return 		1.f-(z-box.min.z)/(box.max.z-box.min.z);
        }
        IC int			GetPixelVFromZ		(float z, const Fbox& box){
			int V 		= iFloor(GetVFromZ(z,box)*(h-1)+0.5f); if (V<0) V=0;
			return V;
        }
    };

    ColorIndexMap		m_ColorIndices;
	BYTEVec				m_Selected; 
    SBase				m_Base;

    float				m_fDensity;

    void				SaveColorIndices		(CFS_Base&);
    bool				LoadColorIndices		(CStream&);
public:
						CDetailManager			();
    virtual 			~CDetailManager			();

    bool				Load            		(CStream&);
    void				Save            		(CFS_Base&);
    bool				Export          		(LPCSTR fn);

    bool				UpdateHeader			();
    bool				UpdateSlots  			();
    bool				UpdateBaseTexture		(LPCSTR tex_name=0);
    bool				UpdateSlotObjects		(int x, int z);
    bool				UpdateObjects			(bool bUpdateTex, bool bUpdateSelectedOnly);
    bool				Initialize				(LPCSTR tex_name);
    bool				Reinitialize			();
    void				InvalidateSlots			();

    CDetail*			AppendObject			(LPCSTR name, bool bTestUnique=true);
    int					RemoveObjects			(bool bOnlyMarked=false);
    CDetail*			FindObjectByName		(LPCSTR name);
    void				MarkAllObjectsAsDel		();

    void				RemoveColorIndices		();
	void				AppendIndexObject		(DWORD color,LPCSTR name,bool bTestUnique=true);
    CDetail*			FindObjectInColorIndices(DWORD index, LPCSTR name);
    void				ExportColorIndices		(LPCSTR fname);
    void				ImportColorIndices		(LPCSTR fname);

    int					ObjCount				(){return m_Slots.size();}
    void				Render					(int priority, bool strictB2F);
    void				Clear					(bool bOnlySlots=false);

	int					RaySelect				(bool flag, float& distance, Fvector& start, Fvector& direction);
	int					FrustumSelect			(bool flag);
	int 				SelectObjects           (bool flag);
	int 				InvertSelection         ();
	int 				SelectionCount          (bool testflag);

    bool				Valid					(){return !!m_Slots.size()||!!m_Objects.size();}

    void				RemoveFromSnapList		(CCustomObject* O);

	virtual void		OnDeviceCreate			();
	virtual void		OnDeviceDestroy			();
};
#endif /*_INCDEF_DetailObjects_H_*/

