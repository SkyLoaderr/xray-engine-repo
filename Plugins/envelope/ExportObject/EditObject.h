//----------------------------------------------------
// file: CEditObject.h
//
// Library Object объязательно должен иметь единичную матрицу!!!
//----------------------------------------------------
#ifndef _INCDEF_Object_H_
#define _INCDEF_Object_H_

#include "SceneObject.h"
#include "..\Bone.h"
#include "..\Motion.h"
//----------------------------------------------------
struct 	SPickInfo;
class 	CEditMesh;
class 	CFrustum;
class 	Shader;

// refs 
struct st_ObjectDB;

struct st_Surface{
	char			name[MAX_OBJ_NAME];
    AStringVec		textures;       //
    AStringVec		vmaps;			// одинаковый размер массивов!!!
	string			shader;
    int				sideflag;
    DWORD			dwFVF;
    bool			has_alpha; 		// use internal (only for render)

	DWORD			mat_id;			// use internal 
    				st_Surface		(){ZeroMemory(this,sizeof(st_Surface));}
};
DEFINE_VECTOR	(st_Surface*,SurfaceVec,SurfaceIt);
DEFINE_VECTOR	(CEditMesh*,EditMeshVec,EditMeshIt);
DEFINE_VECTOR	(CBone*,BoneVec,BoneIt);

class CEditObject : public SceneObject {
	friend class CEditMesh;
    friend class TfrmPropertiesObject;
    friend class CSector;
    friend class TUI_ControlSectorAdd;
    
	string			m_ClassScript;

	SurfaceVec		m_Surfaces;
	EditMeshVec		m_Meshes;
	BoneVec		m_Bones;
protected:
	// options
	int 			m_DynamicObject;

    // orientation
    Fvector 		vScale;
    Fvector 		vRotate;
    Fvector 		vPosition;
	Fmatrix 		mTransform;

	// bounding volume
	Fbox 			m_Box;
    // internal use
    float 			m_fRadius;
    Fvector 		m_Center; 			// центр в мировых координатах

    st_ObjVer		m_LibRefVer;
	CEditObject 	*m_LibRef;
    bool 			bLibItem;

    void 			ClearGeometry			();
public:
    // constructor/destructor methods
					CEditObject				(bool bLib=false);
					CEditObject				(char *name, bool bLib=false);
  	virtual 		~CEditObject			();

	void 			Construct				();

    IC EditMeshIt	FirstMesh				()	{return m_Meshes.begin();}
    IC EditMeshIt	LastMesh				()	{return m_Meshes.end();}
    IC int			MeshCount				()	{return m_Meshes.size();}
    IC SurfaceIt	FirstSurface			()	{return m_Surfaces.begin();}
    IC SurfaceIt	LastSurface				()	{return m_Surfaces.end();}
    IC int			SurfaceCount			()	{return m_Surfaces.size();}
	IC BoneVec&		GetBones				()	{return m_Bones; }
    
    // get object properties methods
	IC bool 		IsLibItem     			()	{return bLibItem; }
	IC bool 		IsReference   			()	{return (m_LibRef!=0); }
	IC bool 		IsSceneItem   			()	{return ((m_LibRef==0)&&!bLibItem); }
	IC bool 		IsDynamic     			()	{return (m_DynamicObject!=0); }
	IC char 		*GetRefName   			()	{VERIFY( m_LibRef );return m_LibRef->m_Name; }
	IC bool 		RefCompare				(CEditObject *to){return (m_LibRef==to); }
	IC CEditObject*	GetRef					()	{return m_LibRef; }
	IC string&		GetClassScript			()	{return m_ClassScript; }

    // statistics methods
    int 			GetFaceCount			();
	int 			GetVertexCount			();

    // update methods
//	virtual void 	RTL_Update				(float dT);
	void 			UpdateBox				();
    void 			UpdateTransform			();

    // change position/orientation methods
	virtual void 	Move					(Fvector& amount);
	virtual void 	Rotate					(Fvector& center, Fvector& axis, float angle);
	virtual void 	Scale					(Fvector& center, Fvector& amount);
	virtual void 	LocalRotate				(Fvector& axis, float angle);
	virtual void 	LocalScale				(Fvector& amount);
//	virtual void 	TranslateToWorld		();

    // get orintation/bounding volume methods
	virtual bool 	GetBox					(Fbox& box);
    void 			GetFullTransformToWorld	(Fmatrix& m);
    void 			GetFullTransformToLocal	(Fmatrix& m);
    IC const Fvector& GetCenter				(){return m_Center;}
    IC float		GetRadius				(){return m_fRadius;}
    IC const Fvector& GetPosition			(){return vPosition;}
    IC const Fvector& GetRotate				(){return vRotate;}
    IC const Fvector& GetScale				(){return vScale;}
    IC Fvector& 	Position				(){return vPosition;}
    IC Fvector& 	Rotate					(){return vRotate;}
    IC Fvector& 	Scale					(){return vScale;}

    // clone/copy methods
    void			RemoveMesh				(CEditMesh* mesh);

    // load/save methods
//	bool 			ReloadObject			(int handle);
//	void 			LoadMeshDef				(FSChunkDef *chunk);
//	bool 			Load					(char* fname);
//	bool 			Load					(int handle);
//	virtual bool 	Load					(FSChunkDef *chunk);
	virtual void 	Save					(CFS_Base&);
	void 			SaveObject				(const char* fname);

    // contains methods
    CEditMesh* 		FindMeshByName			(const char* name, CEditMesh* Ignore=0);
    bool 			ContainsMesh			(const CEditMesh* m);
	st_Surface*		FindSurfaceByName		(const char* surf_name, int* s_id=0);
	void			VerifyMeshNames			();

	// import routines
	bool			Import_LWO				(st_ObjectDB *I);
};
//----------------------------------------------------
#endif /*_INCDEF_EditObject_H_*/


