//----------------------------------------------------
// file: Light.h
//----------------------------------------------------

#ifndef _INCDEF_Sector_H_
#define _INCDEF_Sector_H_

#include "SceneObject.h"
#include "portalutils.h"
enum EVisible{
	fvNone,
    fvPartialInside,
    fvPartialOutside,
    fvFully
};

#define DEFAULT_SECTOR_NAME "Sector_DEFAULT"
// refs
class CEditObject;
class CEditMesh;

class CSectorItem{
	friend class CSector;
	friend class SceneBuilder;
    friend class CPortalUtils;
	CEditObject* object;
	CEditMesh* mesh;
public:
    CSectorItem		();
    CSectorItem		(CEditObject* o, CEditMesh* m);
    void GetTransform(Fmatrix& parent);
    bool IsItem		(const char* O, const char* M);
    IC bool IsItem	(CEditObject* o, CEditMesh* m){ return (o==object)&&(m==mesh); }
};

#pragma pack(push,1)
struct CHFace {
	union{
		DWORD p[3];
		struct { DWORD p0,p1,p2; };
	};
    CHFace(DWORD _p0, DWORD _p1, DWORD _p2) {p0=_p0;p1=_p1;p2=_p2;}
    CHFace(){;}
};
#pragma pack(pop)

DEFINE_VECTOR(CHFace,CHFaceVec,CHFaceIt);
DEFINE_VECTOR(CSectorItem,SItemVec,SItemIt);

class CSector : public SceneObject {
	friend class TfrmPropertiesSector;
	friend class SceneBuilder;
    friend class CPortalUtils;
    friend class CPortal;

    BYTE			m_bDefault;
    BYTE			m_bNeedUpdateCHull;
    bool			m_bHasLoadError;

	FvectorVec		m_CHSectorVertices;
    CHFaceVec		m_CHSectorFaces;
    PlaneVec		m_CHSectorPlanes;
    Fvector 		m_SectorCenter;
    float 			m_SectorRadius;
    void 			MakeCHull	();
	bool 			RenderCHull	(DWORD color, bool bSolid, bool bEdge, bool bCull);

	Fcolor			sector_color;
	SItemVec 		sector_items;
	bool 			FindSectorItem(CEditObject* o, CEditMesh* m, SItemIt& it);
	bool 			FindSectorItem(const char* O, const char* M, SItemIt& it);
	void 			LoadSectorDef( CStream* F );

    void 			UpdatePlanes();

    // only for build
    int				sector_num;
public:
					CSector		();
					CSector		(char *name);
	void 			Construct	();
	virtual 		~CSector	();

    virtual void 	Render		(Fmatrix& parent, ERenderPriority flag);
	virtual bool 	RayPick 	(float& distance,Fvector& start,Fvector& direction,
								Fmatrix& parent, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick	(const CFrustum& frustum, const Fmatrix& parent);
    virtual bool 	SpherePick	(const Fvector& center, float radius, const Fmatrix& parent);
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual bool 	GetBox		(Fbox& box);
	virtual void 	Move		( Fvector& amount ); // need for Shift Level
	virtual void 	OnSceneUpdate();
	virtual void 	OnDestroy	( );
    void			Update		(bool bNeedCreateCHull);
    void			SectorChanged(bool bNeedCreateCHull);

	void			AddMesh		(CEditObject* O, CEditMesh* M);
	void			DelMesh		(CEditObject* O, CEditMesh* M);

    bool			IsDefault	(){return m_bDefault;}
    bool			Contains	(CEditObject* O, CEditMesh* M){SItemIt it; return FindSectorItem(O,M,it);}

    void			CaptureInsideVolume();
    void			CaptureAllUnusedFaces();
    int 			GetSectorFacesCount();
	EVisible		TestCHullSphereIntersection(const Fvector&P, float R);
};

#endif /*_INCDEF_Sector_H_*/

