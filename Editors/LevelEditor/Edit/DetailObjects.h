//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------

#ifndef DetailObjectsH
#define DetailObjectsH

#include "DetailFormat.h"
#include "DetailModel.h"
#include "Library.h"
#include "customobject.h"
#include "DetailManager.h"
#include "Custom2DProjector.h"

class CFrustum;
class CEditableObject;
class Shader;

struct SIndexDist{
    u32 	index;
    float 	dist;
    float	dens[4];
    float	cnt[4];
	SIndexDist(){ZeroMemory(this,sizeof(SIndexDist));}
};
DEFINE_SVECTOR		(SIndexDist,4,SIndexDistVec,SIndexDistIt);

DEFINE_MAP			(u32,DOVec,ColorIndexMap,ColorIndexPairIt);

#define DETAIL_SLOT_SIZE_2 	DETAIL_SLOT_SIZE*0.5f
#define DETAIL_SLOT_RADIUS	DETAIL_SLOT_SIZE*0.7071f

class EDetailManager:
	public CDetailManager,
	public pureDeviceCreate,
	public pureDeviceDestroy
{
	friend class TfrmDOShuffle;

    ObjectList			m_SnapObjects;

    Fbox				m_BBox;

	IC u32			toSlotX			(float x)	{return (x/DETAIL_SLOT_SIZE+0.5f)+dtH.offs_x;}
	IC u32			toSlotZ			(float z)	{return (z/DETAIL_SLOT_SIZE+0.5f)+dtH.offs_z;}
	IC float			fromSlotX		(int x)		{return (x-dtH.offs_x)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}
	IC float			fromSlotZ		(int z)		{return (z-dtH.offs_z)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}

    void				UpdateSlotBBox	(int x, int z, DetailSlot& slot);

    void				GetSlotRect		(Frect& rect, int sx, int sz);
    void				GetSlotTCRect	(Irect& rect, int sx, int sz);
    u8				GetRandomObject	(u32 color_index);
    u8				GetObject		(ColorIndexPairIt& CI, u8 id);

	void 				CalcClosestCount(int part, const Fcolor& C, SIndexDistVec& best);
	void 				FindClosestIndex(const Fcolor& C, SIndexDistVec& best);

    DetailSlot&			GetSlot			(u32 sx, u32 sz);
public:
// render part -----------------------------------------------------------------
    void 				InitRender				();
    void				RenderTexture			(float alpha);
    void				InvalidateCache			();
// render part -----------------------------------------------------------------
public:
    ColorIndexMap		m_ColorIndices;
	U8Vec				m_Selected;
    CCustom2DProjector	m_Base;

    float				m_fDensity;

    void				SaveColorIndices		(IWriter&);
    bool				LoadColorIndices		(IReader&);
public:
						EDetailManager			();
    virtual 			~EDetailManager			();

    bool				Load            		(IReader&);
    void				Save            		(IWriter&);
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
	void				AppendIndexObject		(u32 color,LPCSTR name,bool bTestUnique=true);
    CDetail*			FindObjectInColorIndices(u32 index, LPCSTR name);
    void				ExportColorIndices		(LPCSTR fname);
    void				ImportColorIndices		(LPCSTR fname);

    void				Render					(int priority, bool strictB2F);
    void				ClearColorIndices		();
    void				ClearSlots				();
    void				ClearBase				();
    void				Clear					();

	int					RaySelect				(bool flag, float& distance, const Fvector& start, const Fvector& direction);
	int					FrustumSelect			(bool flag);
	int 				SelectObjects           (bool flag);
	int 				InvertSelection         ();
	int 				SelectionCount          (bool testflag);

    bool				Valid					(){return dtSlots||objects.size();}

    void				RemoveFromSnapList		(CCustomObject* O);
	virtual ObjectList*	GetSnapObjects			(){return &m_SnapObjects;}

	virtual void		OnDeviceCreate			();
	virtual void		OnDeviceDestroy			();
};
#endif /*_INCDEF_DetailObjects_H_*/

