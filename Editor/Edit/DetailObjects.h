//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------

#ifndef _INCDEF_DetailObjects_H_
#define _INCDEF_DetailObjects_H_

#include "SceneObject.h"
#include "DetailFormat.h"
#include "EditObject.h"

class CFrustum;
class CEditObject;
class Shader;

struct SIndexDist{
    DWORD 	index;
    float 	dist;
    float	density[4];
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
        void			set(fvfVertexIn& src){P.set(src.P); u=src.u; v=src.v;};
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
	CEditObject*		m_pRefs;
public:
    bool				m_bMarkDel;
public:
						CDetail			();
	virtual             ~CDetail		();

	bool				Load            (CStream&);
	void				Save            (CFS_Base&);
    void				Export			(CFS_Base&);

	bool				Update			(LPCSTR name);

    IC LPCSTR			GetName			(){R_ASSERT(m_pRefs); return m_pRefs->GetName();}
};

DEFINE_VECTOR(CDetail*,DOVec,DOIt);

DEFINE_VECTOR(DetailSlot,DSVec,DSIt);

DEFINE_MAP(DWORD,DOVec,ColorIndexMap,ColorIndexPairIt);

#define DETAIL_SLOT_SIZE_2 	DETAIL_SLOT_SIZE*0.5f
#define DETAIL_SLOT_RADIUS	DETAIL_SLOT_SIZE*0.70710678118654752440084436210485f

class CDetailManager{
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

	void 				CalcClosestCount(int part, const Fcolor& C, SIndexDistVec& best);
	void 				FindClosestIndex(const Fcolor& C, SIndexDistVec& best);

	bool 				GetColor		(DWORD& color, int U, int V);

    DWORD				GetUFromX		(float x);
    DWORD				GetVFromZ		(float z);

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
    ColorIndexMap		m_ColorIndices;
	BOOLVec				m_Selected;
	Shader*				m_pBaseShader;
	ETextureCore*		m_pBaseTexture;
public:
						CDetailManager			();
    virtual 			~CDetailManager			();

    bool				Load_V1					(CStream&);
    bool				Load            		(CStream&);
    void				Save            		(CFS_Base&);
    void				Export          		(LPCSTR fn);

    bool				UpdateBBox				();
    bool				UpdateBaseTexture		(LPCSTR tex_name=0);
    bool				UpdateSlotObjects		(int x, int z);
    bool				UpdateObjects			(bool bUpdateTex, bool bUpdateSelectedOnly);
    bool				Initialize				(LPCSTR tex_name);
    bool				Reinitialize			();
    void				InvalidateSlots			();

    CDetail*			AppendObject			(LPCSTR name, bool bTestUnique=true);
    void				RemoveObjects			(bool bOnlyMarked=false);
    CDetail*			FindObjectByName		(LPCSTR name);
    void				MarkAllObjectsAsDel		();

    void				RemoveColorIndices		();
	void				AppendIndexObject		(DWORD color,LPCSTR name,bool bTestUnique=true);
    CDetail*			FindObjectInColorIndices(DWORD index, LPCSTR name);

    int					ObjCount				(){return m_Slots.size();}
    void				Render					(ERenderPriority flag);
    void				Clear					();

	void				RayPickSelect			(float& distance, Fvector& start, Fvector& direction);
	int					FrustumSelect			(bool flag);
	int 				SelectObjects           (bool flag);
	int 				InvertSelection         ();
	int 				SelectionCount          (bool testflag);

    bool				Valid					(){return !!m_Slots.size()||!!m_Objects.size();}
};
#endif /*_INCDEF_DetailObjects_H_*/

