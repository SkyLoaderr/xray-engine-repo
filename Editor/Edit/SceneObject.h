//----------------------------------------------------
// file: SceneObject.h
//----------------------------------------------------
#ifndef _INCDEF_SceneObject_H_
#define _INCDEF_SceneObject_H_

#include "CustomObject.h"
#include "EditObject.h"
//----------------------------------------------------

class CSceneObject : public CCustomObject {
	CEditObject*	m_pRefs;
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
	IC bool 		RefCompare				(CEditObject *to){return (m_pRefs==to); }
	IC CEditObject*	GetRef					()	{return m_pRefs; }

    // statistics methods
    int 			GetFaceCount			();
	int 			GetVertexCount			();
    int 			GetSurfFaceCount		(const char* surf_name);

    // render methods
	virtual bool 	IsRender				(Fmatrix& parent);
	virtual void 	Render					(Fmatrix& parent, ERenderPriority flag);
	void 			RenderSelection			(Fmatrix& parent);
	void 			RenderEdge				(Fmatrix& parent, CEditMesh* m=0);
	void 			RenderBones				(const Fmatrix& parent);
	void 			RenderAnimation			(const Fmatrix& parent);
	void 			RenderSingle			(Fmatrix& parent);

    // update methods
	virtual void 	RTL_Update				(float dT);
    virtual void	UpdateTransform			();
	void		    LightenObject			();

    // pick methods
    void 			BoxPick					(const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf);
	virtual bool 	RayPick					(float& dist, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf=0);
	virtual bool 	FrustumPick				(const CFrustum& frustum, const Fmatrix& parent);
    virtual bool 	SpherePick				(const Fvector& center, float radius, const Fmatrix& parent);

    // change position/orientation methods
	virtual void 	Move					(Fvector& amount);
	virtual void 	Rotate					(Fvector& center, Fvector& axis, float angle);
	virtual void 	Scale					(Fvector& center, Fvector& amount);
	virtual void 	LocalRotate				(Fvector& axis, float angle);
	virtual void 	LocalScale				(Fvector& amount);
	virtual void 	TranslateToWorld		();

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


