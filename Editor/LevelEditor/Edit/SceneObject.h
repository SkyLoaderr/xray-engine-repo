//----------------------------------------------------
// file: SceneObject.h
//----------------------------------------------------
#ifndef _INCDEF_SceneObject_H_
#define _INCDEF_SceneObject_H_

#include "CustomObject.h"
#include "EditObject.h"
//----------------------------------------------------

class CSceneObject : public CCustomObject {
	CEditableObject*m_pRefs;
    st_Version		m_ObjVer;
protected:                             
    // orientation
    Fvector 		vScale;
    Fvector 		vRotate;
    Fvector 		vPosition;
	Fmatrix 		mTransform;

    // internal use
    float 			m_fRadius;
    Fvector 		m_Center; 			// центр в мировых координатах

    void			UpdateTransform			(const Fvector& T, const Fvector& R, const Fvector& S);
public:
    // constructor/destructor methods
					CSceneObject			();
					CSceneObject			(char *name);
	virtual 		~CSceneObject			();

	void 			Construct				();

    IC bool			CheckVersion			()  {return (m_ObjVer==m_pRefs->m_ObjVer);}
    // get object properties methods
	IC bool 		RefCompare				(CEditableObject *to){return !!(m_pRefs==to); }
	IC bool 		RefCompare				(LPCSTR ref){return strcmp(ref,m_pRefs->GetName())==0; }
	IC CEditableObject*	GetReference		()	{return m_pRefs; }
	CEditableObject*SetReference			(LPCSTR ref_name);
	IC EditMeshVec& Meshes					() {VERIFY(m_pRefs); return m_pRefs->Meshes();}
    IC LPCSTR		GetRefName				() {VERIFY(m_pRefs); return m_pRefs->GetName();}

    // statistics methods
	IC bool 		IsDynamic     			()	{return (m_pRefs->IsDynamic()); }
    int 			GetFaceCount			();
	int 			GetVertexCount			();
    int 			GetSurfFaceCount		(const char* surf_name);

    // render methods
	virtual bool 	IsRender				();
	virtual void 	Render					(int priority, bool strictB2F);
	void 			RenderSelection			();
	void 			RenderEdge				(CEditableMesh* m=0);
	void 			RenderBones				();
	void 			RenderAnimation			();
	void 			RenderSingle			();

    // update methods
	virtual void 	RTL_Update				(float dT);
    virtual void	UpdateTransform			();
	void		    LightenObject			();

    // pick methods
    void 			BoxPick					(const Fbox& box, SBoxPickInfoVec& pinf);
	virtual bool 	RayPick					(float& dist, Fvector& S, Fvector& D, SRayPickInfo* pinf=0);
	virtual bool 	FrustumPick				(const CFrustum& frustum);
    virtual bool 	SpherePick				(const Fvector& center, float radius);

    // change position/orientation methods
	virtual void 	Move					(Fvector& amount);
	virtual void 	Rotate					(Fvector& center, Fvector& axis, float angle);
	virtual void 	Scale					(Fvector& center, Fvector& amount);
	virtual void 	LocalRotate				(Fvector& axis, float angle);
	virtual void 	LocalScale				(Fvector& amount);

    // get orintation/bounding volume methods
	virtual bool 	GetBox					(Fbox& box);
    void 			GetFullTransformToWorld	(Fmatrix& m);
    void 			GetFullTransformToLocal	(Fmatrix& m);
    IC const Fvector& GetCenter				(){return m_Center;}
    IC float		GetRadius				(){return m_fRadius;}
    IC const Fmatrix& GetTransform			(){return mTransform;}
    IC const Fvector& GetPosition			(){return vPosition;}
    IC const Fvector& GetRotate				(){return vRotate;}
    IC const Fvector& GetScale				(){return vScale;}
    IC Fvector& 	Position				(){return vPosition;}
    IC Fvector& 	Rotate					(){return vRotate;}
    IC Fvector& 	Scale					(){return vScale;}

    virtual bool 	GetPosition				(Fvector& pos){pos.set(vPosition); return true; }
    virtual bool 	GetRotate				(Fvector& rot){rot.set(vRotate); return true; }
    virtual bool 	GetScale				(Fvector& scale){scale.set(vScale); return true; }

    virtual void 	SetPosition				(Fvector& pos){vPosition.set(pos);}
    virtual void 	SetRotate				(Fvector& rot){vRotate.set(rot);}
    virtual void 	SetScale				(Fvector& scale){vScale.set(scale);}

    // load/save methods
  	virtual bool 	Load					(CStream&);
	virtual void 	Save					(CFS_Base&);
};
//----------------------------------------------------
#endif /*_INCDEF_EditObject_H_*/


