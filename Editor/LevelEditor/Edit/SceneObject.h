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
	typedef CCustomObject inherited;
    int				m_iBlinkTime;           

    // internal use
    float 			m_fRadius;
    Fvector 		m_Center; 			// ����� � ������� �����������
public:
    // constructor/destructor methods
					CSceneObject			();
					CSceneObject			(char *name);
	virtual 		~CSceneObject			();

	virtual void 	Select					(BOOL flag);
	void 			Construct				();

    IC bool			CheckVersion			()  {return m_pRefs?(m_ObjVer==m_pRefs->m_ObjVer):false;}
    // get object properties methods
	IC bool 		RefCompare				(CEditableObject *to){return m_pRefs?!!(m_pRefs==to):false; }
	IC bool 		RefCompare				(LPCSTR ref){return m_pRefs?(strcmp(ref,m_pRefs->GetName())==0):false; }
	IC CEditableObject*	GetReference		()	{return m_pRefs; }
	CEditableObject*SetReference			(LPCSTR ref_name);
	IC EditMeshVec* Meshes					() {return m_pRefs?&m_pRefs->Meshes():0;}
    IC LPCSTR		GetRefName				() {return m_pRefs?m_pRefs->GetName():0;}

    // statistics methods
	IC bool 		IsDynamic     			()	{return (m_pRefs?m_pRefs->IsFlag(CEditableObject::eoDynamic):false); }
    int 			GetFaceCount			();
    void			GetFaceWorld			(CEditableMesh* M, int idx, Fvector* verts);
	int 			GetVertexCount			();
    int 			GetSurfFaceCount		(const char* surf_name);

    // render methods
	virtual bool 	IsRender				();
	virtual void 	Render					(int priority, bool strictB2F);
	void 			RenderSelection			(DWORD color=0x40E64646);
	void 			RenderEdge				(CEditableMesh* m=0, DWORD color=0xFFC0C0C0);
	void 			RenderBones				();
	void 			RenderAnimation			();
	void 			RenderSingle			();

    // update methods           
	virtual void 	OnFrame					();
    virtual void	OnUpdateTransform		();

    // 
	void		    LightenObject			();

    // pick methods
    bool 			BoxPick					(const Fbox& box, SBoxPickInfoVec& pinf);
	virtual bool 	RayPick					(float& dist, Fvector& S, Fvector& D, SRayPickInfo* pinf=0);
	virtual bool 	FrustumPick				(const CFrustum& frustum);
    virtual bool 	SpherePick				(const Fvector& center, float radius);

    // change position/orientation methods
	virtual void 	Scale					(Fvector& center, Fvector& amount);
	virtual void 	LocalScale				(Fvector& amount);

    // get orintation/bounding volume methods
	virtual bool 	GetBox					(Fbox& box);
    void 			GetFullTransformToWorld	(Fmatrix& m);
    void 			GetFullTransformToLocal	(Fmatrix& m);
    IC const Fvector& GetCenter				(){return m_Center;}
    IC float		GetRadius				(){return m_fRadius;}

    // load/save methods
  	virtual bool 	Load					(CStream&);
	virtual void 	Save					(CFS_Base&);
};
//----------------------------------------------------
#endif /*_INCDEF_EditObject_H_*/


