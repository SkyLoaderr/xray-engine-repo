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

#define MAX_DETOBJECTS 4
struct SDetailObject{
	CEditObject*	obj;
    DWORD			flag;
};

struct SDOClusterDef{
    // properties
    SDetailObject	m_DetObj[MAX_DETOBJECTS];
    int  			m_Density;	// штук на метр
    //random scale
    Fvector2		m_YScale;	// min/max value
    Fvector2		m_XZScale;	// min/max value
    s32				m_ScaleSeed;
    // random rotate
    s32				m_RotateSeed;

    SDOClusterDef	(){
    	ZeroMemory	(m_DetObj,4*sizeof(SDetailObject));
        m_Density	= 10;
        m_YScale.set(1.f,1.f);
        m_XZScale.set(1.f,1.f);
        m_ScaleSeed	= Random.randF();
        m_RotateSeed= Random.randF();
    }
};

#pragma pack( push,1 )
struct SDOCeil{
	BYTE			m_Alt:6;
	BYTE			m_Obj:2;
};
#pragma pack( pop )

DEFINE_VECTOR(SDOCeil,DOCeilVec,DOCeilIt);

class CDOCluster: public SceneObject, public SDOClusterDef{
    Fvector         m_Position;
    float			m_MinHeight;
    float			m_MaxHeight;
    DOCeilVec		m_Objects;

    void            DrawCluster		(Fcolor& c);
    void			DrawObjects		();
	DWORD 			DetermineColor	(CEditMesh* mesh, int id, float u, float v);
    void			ChangeDensity	(int new_density);
public:
	                CDOCluster		();
	                CDOCluster  	(char *name);
    void            Construct   	();
	virtual         ~CDOCluster  	();

    const Fvector&	Position		(){return m_Position;}

	virtual void    Render      	(Fmatrix& parent, ERenderPriority flag);
	virtual bool    RayPick		   	(float& distance, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum, const Fmatrix& parent);
	virtual void    Move        	(Fvector& amount);
  	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);
	virtual bool    GetBox      	(Fbox& box);

    bool     		AppendCluster	(int density);
	void 			Update			();
};

class CDetail{
	struct fvfVertexIn{
		Fvector 		P;
		float			u,v;
        void			set(fvfVertexIn& src){P.set(src.P); u=src.u; v=src.v;};
	};
    DEFINE_VECTOR		(fvfVertexIn,DOVertVec,DOVertIt);
	// render
    DOVertVec			m_Vertices;
	Shader*				m_pShader;
	DWORD				m_dwFlags;
	float				m_fRadius;

    // references
	CEditObject*		m_pRefs;

    DWORD				m_dwColor;
public:
						CDetail			();
	virtual             ~CDetail		();

	bool				Load            (CStream&);
	void				Save            (CFS_Base&);
    void				Export			(CFS_Base&);

	bool				Update			(DWORD color, LPCSTR name);

    IC LPCSTR			GetName			(){R_ASSERT(m_pRefs); return m_pRefs->GetName();}
};

DEFINE_VECTOR(CDetail*,DOVec,DOIt);

DEFINE_VECTOR(DetailSlot,DSVec,DSIt);

DEFINE_MAP(DWORD,DOVec,ColorIndexMap,ColorIndexPairIt);

#define DETAIL_SLOT_SIZE_2 DETAIL_SLOT_SIZE*0.5f

class CDetailManager{
	DetailHeader		m_Header;
    DSVec		 		m_Slots;
	DOVec				m_Objects;
    ColorIndexMap		m_ColorIndices;

    Fbox				m_BBox;

	IC DWORD			toSlotX			(float x)	{return (x/DETAIL_SLOT_SIZE+0.5f)+m_Header.offs_x;}
	IC DWORD			toSlotZ			(float z)	{return (z/DETAIL_SLOT_SIZE+0.5f)+m_Header.offs_z;}
	IC float			fromSlotX		(int x)		{return (x-m_Header.offs_x)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}
	IC float			fromSlotZ		(int z)		{return (z-m_Header.offs_z)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}

	DWORD 				DetermineColor	(CEditMesh* mesh, int id, float u, float v);
    void				GenerateOneSlot	(int x, int z, DetailSlot& slot);

	struct SIndexDist{
		DWORD 	index;
	    float 	dist;
	}IDS[3];
    int IDS_count;

	void 				FindClosestApproach(const Fcolor& C);
	DWORD				GetColor		(float x, float z);
public:
//	DetailSlot*			m_SelSlot;
	ETextureCore*		m_pBaseTexture;
public:
						CDetailManager	();
    virtual 			~CDetailManager	();

    void				Load            (CStream&);
    void				Save            (CFS_Base&);
    void				Export          (CFS_Base&);

    bool				GenerateSlots	(LPCSTR tex_name);

    void				AppendObject	(DWORD color, LPCSTR name);
    void				RemoveObject	(DWORD color, LPCSTR name);
    void				ChangeObjectColor(LPCSTR name, DWORD dest_color);

    int					ObjCount		(){return m_Slots.size();}
    void				Render			(ERenderPriority flag);
    void				Clear			();

    bool				Valid			(){return !!m_Slots.size();}
};
#endif /*_INCDEF_DetailObjects_H_*/

