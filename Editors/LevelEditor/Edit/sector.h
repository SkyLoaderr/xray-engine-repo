#ifndef SectorH
#define SectorH

#include "portalutils.h"                                 
enum EVisible{
	fvNone,
    fvPartialInside,
    fvPartialOutside,
    fvFully
};

#define DEFAULT_SECTOR_NAME "Sector_DEFAULT"
// refs
class CEditableMesh;

class CSectorItem{
	friend class CSector;
	friend class SceneBuilder;
    friend class CPortalUtils;
	CSceneObject* object;
	CEditableMesh* mesh;
public:
    CSectorItem		();
    CSectorItem		(CSceneObject* o, CEditableMesh* m);
    void GetTransform(Fmatrix& parent);
    bool IsItem		(const char* O, const char* M);
    IC bool IsItem	(CSceneObject* o, CEditableMesh* m){ return (o==object)&&(m==mesh); }
};

DEFINE_VECTOR(CSectorItem,SItemVec,SItemIt);

class CSector : public CCustomObject {
	friend class TfrmPropertiesSector;
	friend class SceneBuilder;
    friend class CPortalUtils;
    friend class CPortal;
	typedef CCustomObject inherited;

    BYTE			m_bDefault;
    bool			m_bHasLoadError;

    Fbox			m_Box;

    Fvector 		m_SectorCenter;
    float 			m_SectorRadius;

	Fcolor			sector_color;
	SItemVec 		sector_items;
	bool 			FindSectorItem(CSceneObject* o, CEditableMesh* m, SItemIt& it);
	bool 			FindSectorItem(const char* O, const char* M, SItemIt& it);
	void 			LoadSectorDef( CStream* F );

    // only for build
    int				sector_num;
	void 			OnDestroy	();
public:
					CSector		(LPVOID data, LPCSTR name);
	void 			Construct	(LPVOID data);
	virtual 		~CSector	();

    virtual void 	Render		(int priority, bool strictB2F);
	virtual bool 	RayPick 	(float& distance,Fvector& start,Fvector& direction,
								SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick	(const CFrustum& frustum);
    virtual bool 	SpherePick	(const Fvector& center, float radius);
	virtual bool 	GetBox		(Fbox& box);
	virtual void 	Move		( Fvector& amount ); // need for Shift Level
	virtual void 	OnSceneUpdate();
    void			UpdateVolume();

    // file system function
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual void	FillProp	(LPCSTR pref, PropItemVec& values);

	bool			AddMesh		(CSceneObject* O, CEditableMesh* M); // возвращает добавлен ли объект
	bool	  		DelMesh		(CSceneObject* O, CEditableMesh* M); // возвращает false если объект удален

    bool			IsDefault	(){return m_bDefault;}
    bool			Contains	(CSceneObject* O, CEditableMesh* M){SItemIt it; return FindSectorItem(O,M,it);}

    void			CaptureInsideVolume();
    void			CaptureAllUnusedMeshes();
    void			GetCounts	(int* objects, int* meshes, int* faces);
    bool			IsEmpty		();

    EVisible		Intersect	(const Fvector& center, float radius);
	EVisible 		Intersect	(const Fbox& box);
};

#endif /*_INCDEF_Sector_H_*/

