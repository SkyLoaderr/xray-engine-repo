//----------------------------------------------------
// file: CSceneObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SceneObject.h"
#include "bottombar.h"
#include "scene.h"
#include "library.h"
#include "EditMesh.h"
#include "motion.h"
#include "ui_main.h"
#include "d3dutils.h"
#include "render.h"
#include "PropertiesListHelper.h"

#define BLINK_TIME 300.f

//----------------------------------------------------
CSceneObject::CSceneObject(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct	(data);
}

void CSceneObject::Construct(LPVOID data)
{
	ClassID		= OBJCLASS_SCENEOBJECT;

    m_ReferenceName = "";
	m_pReference 	= 0;
    m_Version		= 0;

    m_TBBox.invalidate();
    m_iBlinkTime	= 0;

    m_Flags.zero	();
}

CSceneObject::~CSceneObject()
{
	Lib.RemoveEditObject(m_pReference);
}
//----------------------------------------------------

void CSceneObject::EvictObject()
{
	if (m_pReference) m_pReference->EvictObject();
}

//----------------------------------------------------
void CSceneObject::Select(BOOL flag)
{
	inherited::Select(flag);
    if (flag) m_iBlinkTime=Device.dwTimeGlobal+BLINK_TIME+Device.dwTimeDelta;
}

//----------------------------------------------------
void CSceneObject::GetFaceWorld(CEditableMesh* M, int idx, Fvector* verts)
{
	const Fvector* PT[3];
	M->GetFacePT(idx, PT);
	_Transform().transform_tiny(verts[0],*PT[0]);
    _Transform().transform_tiny(verts[1],*PT[1]);
	_Transform().transform_tiny(verts[2],*PT[2]);
}

int CSceneObject::GetFaceCount()
{
	return m_pReference?m_pReference->GetFaceCount():0;
}

int CSceneObject::GetSurfFaceCount(const char* surf_name)
{
	return m_pReference?m_pReference->GetSurfFaceCount(surf_name):0;
}

int CSceneObject::GetVertexCount()
{
	return m_pReference?m_pReference->GetVertexCount():0;
}

void CSceneObject::OnUpdateTransform()
{
	inherited::OnUpdateTransform();
    // update bounding volume
    if (m_pReference){
    	m_TBBox.set		(m_pReference->GetBox());
    	m_TBBox.xform	(_Transform());
    }
}

bool CSceneObject::GetBox( Fbox& box )
{
	if (!m_pReference) return false;
	box.set(m_TBBox);
	return true;
}

bool CSceneObject::GetUTBox( Fbox& box )
{
	if (!m_pReference) return false;
    box.set(m_pReference->GetBox());
	return true;
}

bool __inline CSceneObject::IsRender()
{
	if (!m_pReference) return false;
    return ::Render->occ_visible(m_TBBox);
}

void CSceneObject::Render(int priority, bool strictB2F)
{
	inherited::Render(priority,strictB2F);
    if (!m_pReference) return;
    Scene.TurnLightsForObject(this);
	m_pReference->Render(_Transform(), priority, strictB2F);
    if (Selected()){
    	if (1==priority){
            if (false==strictB2F){
                Device.SetShader(Device.m_WireShader);
                RCache.set_xform_world(_Transform());
                u32 clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                DU.DrawSelectionBox(m_pReference->GetBox(),&clr);
            }else{
                if (m_iBlinkTime>(int)Device.dwTimeGlobal){
    	            RenderSelection(D3DCOLOR_ARGB(iFloor(sqrtf(float(m_iBlinkTime-Device.dwTimeGlobal)/BLINK_TIME)*48),255,255,255));
        	        UI.RedrawScene();
            	}
            }
        }
    }
}

void CSceneObject::RenderSingle()
{
	if (!m_pReference) return;
	m_pReference->RenderSingle(_Transform());
}

void CSceneObject::RenderBones()
{
	if (!m_pReference) return;
	m_pReference->RenderBones(_Transform());
}

void CSceneObject::RenderEdge(CEditableMesh* mesh, u32 color)
{
	if (!m_pReference) return;
    if (::Render->occ_visible(m_TBBox))
		m_pReference->RenderEdge(_Transform(), mesh, color);
}

void CSceneObject::RenderSelection(u32 color)
{
	if (!m_pReference) return;
	m_pReference->RenderSelection(_Transform(),0,color);
}

bool CSceneObject::FrustumPick(const CFrustum& frustum)
{
	if (!m_pReference) return false;
    if (::Render->occ_visible(m_TBBox))
		return m_pReference->FrustumPick(frustum, _Transform());
    return false;
}

bool CSceneObject::SpherePick(const Fvector& center, float radius)
{
	if (!m_pReference) return false;
    float fR; Fvector vC;
	m_TBBox.getsphere(vC,fR);
	float R=radius+fR;
    float dist_sqr=center.distance_to_sqr(vC);
    if (dist_sqr<R*R) return true;
    return false;
}

bool CSceneObject::RayPick(float& dist, const Fvector& S, const Fvector& D, SRayPickInfo* pinf)
{
	if (!m_pReference) return false;
    if (::Render->occ_visible(m_TBBox))
		if (m_pReference->RayPick(dist, S, D, _ITransform(), pinf)){
        	if (pinf) pinf->s_obj = this;
            return true;
        }
	return false;
}

void CSceneObject::RayQuery(SPickQuery& pinf)
{
	if (!m_pReference) return;
    m_pReference->RayQuery(_Transform(), _ITransform(), pinf);
}

void CSceneObject::BoxQuery(SPickQuery& pinf)
{
	if (!m_pReference) return;
    m_pReference->BoxQuery(_Transform(), _ITransform(), pinf);
}

bool CSceneObject::BoxPick(const Fbox& box, SBoxPickInfoVec& pinf)
{
	if (!m_pReference) return false;
	return m_pReference->BoxPick(this, box, _ITransform(), pinf);
}

void CSceneObject::GetFullTransformToWorld( Fmatrix& m )
{
    m.set( _Transform() );
}

void CSceneObject::GetFullTransformToLocal( Fmatrix& m )
{
    m.invert(_Transform());
}

CEditableObject* CSceneObject::UpdateReference()
{
	Lib.RemoveEditObject(m_pReference);
	m_pReference	= (m_ReferenceName.IsEmpty())?0:Lib.CreateEditObject(m_ReferenceName.c_str());
    return m_pReference;
}

CEditableObject* CSceneObject::SetReference(LPCSTR ref_name)
{
	m_ReferenceName	= ref_name;
    return UpdateReference();
}

void CSceneObject::OnFrame()
{
	inherited::OnFrame();
	if (!m_pReference) return;
	if (m_pReference) m_pReference->OnFrame();
	if (psDeviceFlags.is(rsStatistic)){
    	if (IsStatic()||IsMUStatic()||Selected()){
            Device.Statistic.dwLevelSelFaceCount 	+= GetFaceCount();
            Device.Statistic.dwLevelSelVertexCount 	+= GetVertexCount();
        }
    }
}

void __fastcall CSceneObject::ReferenceChange(PropValue* sender)
{
	UpdateReference	();
}

void CSceneObject::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp	(pref,items);
    PropValue* V		= PHelper.CreateALibObject	(items,FHelper.PrepareKey(pref,"Reference"),	&m_ReferenceName); 
    V->OnChangeEvent 	= ReferenceChange;
}

bool CSceneObject::GetSummaryInfo(SSceneSummary* inf)
{
	CEditableObject* E 	= GetReference(); R_ASSERT(E);
    E->GetSummaryInfo	(inf);
	return true;
}

void CSceneObject::OnShowHint(AStringVec& dest)
{
	inherited::OnShowHint(dest);
    dest.push_back(AnsiString("Reference: ")+m_ReferenceName);
}
//----------------------------------------------------

