//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------

#ifndef _INCDEF_DetailObjects_H_
#define _INCDEF_DetailObjects_H_

#include "DetailFormat.h"
#include "DetailModel.h"
#include "Library.h"
#include "customobject.h"
#include "DetailManager.h"

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

DEFINE_MAP			(DWORD,DOVec,ColorIndexMap,ColorIndexPairIt);

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

	IC DWORD			toSlotX			(float x)	{return (x/DETAIL_SLOT_SIZE+0.5f)+dtH.offs_x;}
	IC DWORD			toSlotZ			(float z)	{return (z/DETAIL_SLOT_SIZE+0.5f)+dtH.offs_z;}
	IC float			fromSlotX		(int x)		{return (x-dtH.offs_x)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}
	IC float			fromSlotZ		(int z)		{return (z-dtH.offs_z)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}

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
    void 				InitRender				();
    void				RenderTexture			(float alpha);
    void				InvalidateCache			();
// render part -----------------------------------------------------------------
public:
	class SBase{
		Shader*			shader_blended;
        Shader*			shader_overlap;
        string128		name;
        u32 			w;
        u32 			h;
		U32Vec			data;
	    DEFINE_VECTOR	(FVF::V,TVertVec,TVertIt);
		TVertVec		mesh;
	    CVS*			stream;
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
        	if (Valid()&&(U<(int)w)&&(V<(int)h)){
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
						EDetailManager			();
    virtual 			~EDetailManager			();

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

    void				Render					(int priority, bool strictB2F);
    void				ClearColorIndices		();
    void				ClearSlots				();
    void				ClearBase				();
    void				Clear					();

	int					RaySelect				(bool flag, float& distance, Fvector& start, Fvector& direction);
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

