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

class CDetail{
	friend class CDetailManager;
    friend class TfrmDOShuffle;
    friend bool CompareIDFunc(CDetail* d0, CDetail* d1);

	struct fvfVertexIn{
		Fvector 		P;
		float			u,v;
        void			set(fvfVertexIn& src){P.set(src.P); u=src.u; v=src.v;};
	};
    DEFINE_VECTOR		(fvfVertexIn,DOVertVec,DOVertIt);
    float				m_fMinScale;
    float				m_fMaxScale;
    // internal use
	BYTE				m_ID;
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

#define DETAIL_SLOT_SIZE_2 DETAIL_SLOT_SIZE*0.5f

class CDetailManager{
	friend class TfrmDOShuffle;

	DetailHeader		m_Header;
    DSVec		 		m_Slots;
	DOVec				m_Objects;

    Fbox				m_BBox;

	IC DWORD			toSlotX			(float x)	{return (x/DETAIL_SLOT_SIZE+0.5f)+m_Header.offs_x;}
	IC DWORD			toSlotZ			(float z)	{return (z/DETAIL_SLOT_SIZE+0.5f)+m_Header.offs_z;}
	IC float			fromSlotX		(int x)		{return (x-m_Header.offs_x)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}
	IC float			fromSlotZ		(int z)		{return (z-m_Header.offs_z)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}

	DWORD 				DetermineColor	(CEditMesh* mesh, int id, float u, float v);

    void				UpdateSlotBBox	(int x, int z, DetailSlot& slot);

    void				GetSlotRect		(Frect& rect, int sx, int sz);
    void				GetSlotTCRect	(Irect& rect, int sx, int sz);
    BYTE				GetRandomObject	(DWORD color_index, const CRandom& R);

	void 				CalcClosestCount(int part, const Fcolor& C, SIndexDistVec& best);
	void 				FindClosestIndex(const Fcolor& C, SIndexDistVec& best);
    void				SortObjectsByID	();
    BYTE				FindEmptyID		();

	DWORD				GetColor		(float x, float z);
	DWORD 				GetColor		(int U, int V);

    DWORD				GetUFromX		(float x);
    DWORD				GetVFromZ		(float z);
public:
    ColorIndexMap		m_ColorIndices;
//	DetailSlot*			m_SelSlot;
	ETextureCore*		m_pBaseTexture;
public:
						CDetailManager	();
    virtual 			~CDetailManager	();

    bool				Load            (CStream&);
    void				Save            (CFS_Base&);
    void				Export          (CFS_Base&);

    bool				UpdateBBox		();
    bool				UpdateObjects	();
    bool				GenerateSlots	(LPCSTR tex_name);

    CDetail*			AppendObject	(LPCSTR name, bool bTestUnique=true);
    void				RemoveObjects	(bool bOnlyMarked=false);
    CDetail*			FindObjectByName(LPCSTR name);
    void				MarkAllObjectsAsDel();

    void				RemoveColorIndices();
	void				AppendIndexObject(DWORD color,LPCSTR name,bool bTestUnique=true);
    CDetail*			FindObjectInColorIndices(DWORD index, LPCSTR name);

    int					ObjCount		(){return m_Slots.size();}
    void				Render			(ERenderPriority flag);
    void				Clear			();

    bool				Valid			(){return !!m_Slots.size()||!!m_Objects.size();}
};
#endif /*_INCDEF_DetailObjects_H_*/

