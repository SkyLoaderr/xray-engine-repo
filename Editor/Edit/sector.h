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
	DWORDVec Face_IDs;
    BOOLVec mask;
public:
    CSectorItem		();
    CSectorItem		(CEditObject* o, CEditMesh* m);
    void GetTransform(Fmatrix& parent);
    bool IsItem		(const char* O, const char* M);
    IC bool IsItem	(CEditObject* o, CEditMesh* m){ return (o==object)&&(m==mesh); }
    IC void Add		(int f_id){
		if (find(Face_IDs.begin(),Face_IDs.end(),f_id)==Face_IDs.end()){
            if (!PortalUtils.IsFaceUsed(object,mesh,f_id)){
	        	Face_IDs.push_back(f_id);
	            mask[f_id]=true;
            }
        }
    }
    IC void Add		(DWORDVec& fl){
    	DWORDVec temp=fl;
    	PortalUtils.TestUsedFaces(object,mesh,temp);
//    	Face_IDs.insert(Face_IDs.end(),temp.begin(),temp.end());
//    	Unique();
        for (DWORDIt it=temp.begin(); it!=temp.end(); it++){
        	if (!mask[*it]){
            	mask[*it] = true;
                Face_IDs.push_back(*it);
            }
        }
//        for (DWORDIt it=fl.begin(); it!=fl.end(); it++)
//        	Add		(*it);
    }
    IC void Del		(int f_id){
    	DWORDIt it=find(Face_IDs.begin(),Face_IDs.end(),f_id);
        if (it==Face_IDs.end()) return;
        Face_IDs.erase(it);
		mask[f_id]=false;
    }
    IC void Del		(DWORDVec& fl){
    	sort(Face_IDs.begin(),Face_IDs.end());
    	sort(fl.begin(),fl.end());
		DWORDVec new_fl(Face_IDs.size());
        DWORDIt end_it=set_difference(Face_IDs.begin(),Face_IDs.end(),fl.begin(), fl.end(), new_fl.begin());
        Face_IDs.clear();
        Face_IDs.insert(Face_IDs.end(),new_fl.begin(),end_it);
        for (DWORDIt it=fl.begin(); it!=fl.end(); it++) mask[*it]=false;
    }
    IC void Unique	(){
    	sort(Face_IDs.begin(),Face_IDs.end());
    	DWORDIt i=unique(Face_IDs.begin(),Face_IDs.end());
        Face_IDs.erase(i,Face_IDs.end());
    }
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

	void			AddFace		(CEditObject* O, CEditMesh* M, int f_id);
	void			AddFaces	(CEditObject* O, CEditMesh* M, DWORDVec& fl);
	void			DelFace		(CEditObject* O, CEditMesh* M, int f_id);
	void			DelFaces	(CEditObject* O, CEditMesh* M, DWORDVec& fl);

    bool			IsFaceUsed	(CEditObject* O, CEditMesh* M, int f_id);
    void			TestUsedFaces(CEditObject* O, CEditMesh* M, DWORDVec& fl);
    bool			IsDefault	(){return m_bDefault;}

    void			CaptureInsideVolume();
    void			CaptureAllUnusedFaces();
    int 			GetSectorFacesCount();
	EVisible		TestCHullSphereIntersection(const Fvector&P, float R);
};

#endif /*_INCDEF_Sector_H_*/

