//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "topbar.h"
#include "PropertiesList.h"               
#include "ItemList.h"
#include "motion.h"
#include "bone.h"
#include "library.h"
#include "BodyInstance.h"
#include "fmesh.h"
#include "KeyBar.h"
#include "main.h"
#include "folderlib.h"

//------------------------------------------------------------------------------
CActorTools Tools;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

void CActorTools::PreviewModel::RestoreParams(TFormStorage* s)
{          
    m_Props->RestoreParams(s);
    m_LastObjectName	= s->ReadString	("preview_name","");
    int val;
    val					= s->ReadInteger("preview_speed",0); 	m_fSpeed 	= *((float*)&val);
    val					= s->ReadInteger("preview_segment",0); 	m_fSegment	= *((float*)&val);
    m_Flags.set			(s->ReadInteger("preview_flags",0));
    m_ScrollAxis		= (EScrollAxis)s->ReadInteger("preview_scaxis",0);
}

void CActorTools::PreviewModel::SaveParams(TFormStorage* s)
{
    m_Props->SaveParams(s);
    s->WriteString	("preview_name",	m_LastObjectName);
    s->WriteInteger	("preview_speed",	*((int*)&m_fSpeed));
    s->WriteInteger	("preview_segment",	*((int*)&m_fSegment));
    s->WriteInteger	("preview_flags",	m_Flags.get());
    s->WriteInteger	("preview_scaxis",	m_ScrollAxis);
}

void CActorTools::PreviewModel::OnDestroy()
{
	TProperties::DestroyForm(m_Props);
}

void CActorTools::PreviewModel::OnCreate()
{
    m_Props = TProperties::CreateForm(0,alNone);
}

void CActorTools::PreviewModel::Clear()
{
    Lib.RemoveEditObject(m_pObject);
}
void CActorTools::PreviewModel::SelectObject()
{
    LPCSTR fn;
    if (!TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,fn,1,m_LastObjectName.c_str())) return;
    Lib.RemoveEditObject(m_pObject);
    m_pObject = Lib.CreateEditObject(fn);
    if (!m_pObject)	ELog.DlgMsg(mtError,"Object '%s' can't find in object library.",fn);
    else			m_LastObjectName = fn;
}

xr_token		sa_token					[ ]={
	{ "+Z",		CActorTools::PreviewModel::saZp	},
	{ "-Z",		CActorTools::PreviewModel::saZn	},
	{ "+X",		CActorTools::PreviewModel::saXp	},
	{ "-X",		CActorTools::PreviewModel::saXn	},
	{ 0,		0								}
};

void CActorTools::PreviewModel::SetPreferences()
{
	PropItemVec items;
    PHelper.CreateFlag32(items, 	"Scroll", 		&m_Flags, 		pmScroll);
    PHelper.CreateFloat	(items, 	"Speed (m/c)",	&m_fSpeed,		-10000.f,10000.f,0.01f,2);
    PHelper.CreateFloat	(items, 	"Segment (m)",	&m_fSegment,	-10000.f,10000.f,0.01f,2);
    PHelper.CreateToken	(items,		"Scroll axis",	&m_ScrollAxis,	sa_token,sizeof(m_ScrollAxis));
	m_Props->AssignItems(items,true,"Preview prefs");
    m_Props->ShowProperties();
}
void CActorTools::PreviewModel::Render()
{
	if (m_pObject){
        float angle;
    	switch (m_ScrollAxis){
        case saZp: angle = 0;		break;
        case saZn: angle = PI;		break;
        case saXp: angle = PI_DIV_2;break;
        case saXn: angle =-PI_DIV_2;break;
        default: THROW;
        }
		Fmatrix R,T;
       	R.rotateY(angle);
    	T.translate(m_vPosition);
        T.mulA(R);
    	m_pObject->RenderSingle(T);
    }
}
void CActorTools::PreviewModel::Update()
{
    if (m_Flags.is(pmScroll)){
        m_vPosition.z += m_fSpeed*Device.fTimeDelta;
        if (m_vPosition.z>m_fSegment) m_vPosition.z-=m_fSegment;
    }
}

CActorTools::CActorTools()
{
	m_pEditObject		= 0;
    m_bObjectModified	= false;
    m_ObjectItems 		= 0;
    m_ItemProps 		= 0;
    m_bReady			= false;
    m_KeyBar			= 0;
    m_Flags.zero		();
    m_EditMode			= emObject;
}
//---------------------------------------------------------------------------

CActorTools::~CActorTools()
{
}
//---------------------------------------------------------------------------

bool CActorTools::OnCreate()
{
    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);

    // props
    m_ObjectItems 	= TItemList::CreateForm(fraLeftBar->paObjectProps,alClient,OnObjectItemFocused);
    m_ItemProps 	= TProperties::CreateForm(fraLeftBar->paItemProps,alClient,OnMotionDefsModified);
    m_PreviewObject.OnCreate();

    // key bar
	m_KeyBar 		= TfrmKeyBar::CreateKeyBar(frmMain->paMain);

    m_bReady = true;

    ChangeAction(eaSelect);

    return true;
}

void CActorTools::OnDestroy()
{
	VERIFY(m_bReady);
    m_bReady			= false;

	// unlock
	TItemList::DestroyForm(m_ObjectItems);
	TProperties::DestroyForm(m_ItemProps);
    m_PreviewObject.OnDestroy();

    m_PreviewObject.Clear();
    m_RenderObject.Clear();
	xr_delete(m_pEditObject);
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
}
//---------------------------------------------------------------------------

bool CActorTools::IfModified(){
    if (IsModified()){
        int mr = ELog.DlgMsg(mtConfirmation, "The '%s' has been modified.\nDo you want to save your changes?",UI.GetEditFileName());
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; else{m_bObjectModified = false;}break;
        case mrNo: m_bObjectModified = false; break;
        case mrCancel: return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------

void CActorTools::OnObjectModified()
{
	m_bObjectModified 		= true;
    m_Flags.set				(flUpdateGeometry,TRUE);
    OnGeometryModified		();
	UI.Command				(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CActorTools::SetPreviewObjectPrefs()
{
	m_PreviewObject.SetPreferences();
}
//---------------------------------------------------------------------------

void CActorTools::SelectPreviewObject(bool bClear){
	if (bClear){ m_PreviewObject.Clear(); return; }
    m_PreviewObject.SelectObject();
}
//---------------------------------------------------------------------------

void CActorTools::GetStatTime(float& a, float& b, float& c)
{
	if (m_RenderObject.IsRenderable()&&fraLeftBar->ebRenderEngineStyle->Down&&m_RenderObject.m_pBlend){
    	a = 0;
		b = m_RenderObject.m_pBlend->timeTotal/m_RenderObject.m_pBlend->speed;
        c = m_RenderObject.m_pBlend->timeCurrent/m_RenderObject.m_pBlend->speed;
        if (c>b){
        	int cnt=iFloor(c/b);
        	c-=(cnt*b);
        }
    }else{
    	if (fraLeftBar->ebRenderEditorStyle->Down&&m_pEditObject&&m_pEditObject->GetActiveSMotion()){
			st_AnimParam& P=m_pEditObject->m_SMParam;
		    a = P.min_t;
    		b = P.max_t;
            c = P.t;
        }else{
        	a = 0;
            b = 0;
            c = 0;
        }
    }
}
//---------------------------------------------------------------------------

void CActorTools::Render()
{
	if (!m_bReady) return;
    m_PreviewObject.Render();
	if (m_pEditObject){
        if (m_RenderObject.IsRenderable()&&fraLeftBar->ebRenderEngineStyle->Down){
			for (int k=0; k<4; k++){ 
            	Device.Models.Render(m_RenderObject.m_pVisual,Fidentity,k,false,m_RenderObject.m_fLOD);
            	Device.Models.Render(m_RenderObject.m_pVisual,Fidentity,k,true,m_RenderObject.m_fLOD);
            }
        }else{
	        // update transform matrix
    		m_pEditObject->RenderSkeletonSingle(m_AVTransform);
        }
    }
}
//---------------------------------------------------------------------------

void CActorTools::OnFrame()
{
	if (!m_bReady) return;
    if (m_KeyBar) m_KeyBar->UpdateBar();
    m_PreviewObject.Update();
	if (m_pEditObject){
    	// update matrix
        Fmatrix	mTranslate,mRotate;
        mRotate.setHPB			(m_pEditObject->a_vRotate.y, m_pEditObject->a_vRotate.x, m_pEditObject->a_vRotate.z);
        mTranslate.translate	(m_pEditObject->a_vPosition);
        m_AVTransform.mul		(mTranslate,mRotate);

    	if (m_RenderObject.IsRenderable()&&m_pEditObject->IsSkeleton())
        	PKinematics(m_RenderObject.m_pVisual)->Calculate(1.f);
    	m_pEditObject->OnFrame();
    }
    if (m_Flags.is(flRefreshShaders)){
    	m_Flags.set(flRefreshShaders,FALSE);
        m_pEditObject->OnDeviceDestroy();
    }
    if (m_Flags.is(flRefreshSubProps)){
    	m_Flags.set(flRefreshSubProps,FALSE);
        ListItemsVec items;
        m_ObjectItems->GetSelected(items);
		OnObjectItemFocused(items);
    }
	if (m_Flags.is(flRefreshProps))
        RealUpdateProperties();
}

void CActorTools::ZoomObject(bool bSelOnly)
{
	VERIFY(m_bReady);
    if (m_pEditObject)
        Device.m_Camera.ZoomExtents(m_pEditObject->GetBox());
}

void CActorTools::OnDeviceCreate()
{
	VERIFY(m_bReady);
    // add directional light
    Flight L;
    ZeroMemory(&L,sizeof(Flight));
    L.type = D3DLIGHT_DIRECTIONAL;
    L.diffuse.set(1,1,1,1);
    L.direction.set(1,-1,1); L.direction.normalize();
	Device.SetLight(0,L);
	Device.LightEnable(0,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,-1); L.direction.normalize();
	Device.SetLight(1,L);
	Device.LightEnable(1,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(1,-1,-1); L.direction.normalize();
	Device.SetLight(2,L);
	Device.LightEnable(2,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,1); L.direction.normalize();
	Device.SetLight(3,L);
	Device.LightEnable(3,true);

	L.diffuse.set(1.0,0.8,0.7,1);
    L.direction.set(0,1,0); L.direction.normalize();
	Device.SetLight(4,L);
	Device.LightEnable(4,true);

    if (m_pEditObject){
    	m_pEditObject->OnDeviceCreate();
        MakePreview();
    }
}

void CActorTools::OnDeviceDestroy(){
    if (m_pEditObject){
    	m_pEditObject->OnDeviceDestroy();
        m_RenderObject.DeleteVisual();
    }
}

void CActorTools::Clear()
{
	VERIFY(m_bReady);

    // delete visuals
    xr_delete(m_pEditObject);
    m_RenderObject.Clear();
//	m_PreviewObject.Clear();
    m_ObjectItems->ClearList();
    m_ItemProps->ClearProperties();

	m_bObjectModified 	= false;
	m_Flags.set			(flUpdateGeometry|flUpdateMotionDefs|flUpdateMotionKeys,FALSE);
    m_EditMode			= emObject;
    
    UI.RedrawScene();
}

bool CActorTools::Load(LPCSTR name)
{
	VERIFY(m_bReady);
	CEditableObject* O = xr_new<CEditableObject>(name);
	if (O->Load(name)){
    	O->m_Flags.set(CEditableObject::eoDynamic,TRUE);
//    	if (O->m_Flags.is(CEditableObject::eoDynamic))
        {
            xr_delete(m_pEditObject);
            m_pEditObject = O;
            // delete visual
            m_RenderObject.Clear();
            fraLeftBar->SetRenderStyle(false);
            return true;
        }
//        else{ ELog.DlgMsg(mtError,"Can't load non dynamic object '%s'.",name); }
    }else{
       	ELog.DlgMsg(mtError,"Can't load object file '%s'.",name);
    }
    xr_delete(O);

    return false;
}

bool CActorTools::Save(LPCSTR name)
{
	VERIFY(m_bReady);
    if (m_pEditObject){
    	m_pEditObject->SaveObject(name);
		m_bObjectModified = false;
        return true;
    }
	return false;
}

bool CActorTools::ExportOGF(LPCSTR name)
{
	VERIFY(m_bReady);
    if (m_pEditObject&&m_pEditObject->ExportOGF(name)) return true;
    return false;
}

void CActorTools::Reload()
{
	VERIFY(m_bReady);
    // visual part
}

void CActorTools::OnShowHint(AStringVec& SS)
{
}

void CActorTools::ChangeAction(EAction action)
{
	switch(action){
    case eaSelect: m_bHiddenMode=false; break;
    case eaAdd:
    case eaMove:
    case eaRotate:
    case eaScale:  m_bHiddenMode=true; break;
    }
    m_Action = action;
    switch(m_Action){
        case eaSelect:  UI.GetD3DWindow()->Cursor = crCross;     break;
        case eaAdd:     UI.GetD3DWindow()->Cursor = crArrow;     break;
        case eaMove:    UI.GetD3DWindow()->Cursor = crSizeAll;   break;
        case eaRotate:  UI.GetD3DWindow()->Cursor = crSizeWE;    break;
        case eaScale:   UI.GetD3DWindow()->Cursor = crVSplit;    break;
        default:        UI.GetD3DWindow()->Cursor = crHelp;
    }
    UI.RedrawScene();
    fraTopBar->ChangeAction(m_Action);
}

extern AnsiString MakeFullBoneName(BoneVec& lst, CBone* bone);
bool __fastcall CActorTools::MouseStart(TShiftState Shift)
{
	switch(m_Action){
    case eaSelect:
    	switch (m_EditMode){
        case emBone:{
	        CBone* B 	= m_pEditObject->PickBone(UI.m_CurrentRStart,UI.m_CurrentRNorm,m_AVTransform);
            bool bVal 	= B?Shift.Contains(ssAlt)?false:(Shift.Contains(ssCtrl)?!B->Selected():true):false;
            SelectListItem(BONES_PREFIX,B?MakeFullBoneName(m_pEditObject->Bones(),B).c_str():0,bVal,Shift.Contains(ssCtrl)||Shift.Contains(ssAlt),true);
        }break;
        }
    return false;
    case eaAdd:
    break;
    case eaMove:{
        if (fraTopBar->ebAxisY->Down){
            m_MovingXVector.set(0,0,0);
            m_MovingYVector.set(0,1,0);
        }else{
            m_MovingXVector.set( Device.m_Camera.GetRight() );
            m_MovingXVector.y = 0;
            m_MovingYVector.set( Device.m_Camera.GetDirection() );
            m_MovingYVector.y = 0;
            m_MovingXVector.normalize_safe();
            m_MovingYVector.normalize_safe();
        }
        m_MovingReminder.set(0,0,0);
    }break;
    case eaRotate:{
        m_RotateCenter.set(0,0,0);
        m_RotateVector.set(0,0,0);
        if (fraTopBar->ebAxisX->Down) m_RotateVector.set(1,0,0);
        else if (fraTopBar->ebAxisY->Down) m_RotateVector.set(0,1,0);
        else if (fraTopBar->ebAxisZ->Down) m_RotateVector.set(0,0,1);
        m_fRotateSnapAngle = 0;
    }break;
    }
	return m_bHiddenMode;
}

bool __fastcall CActorTools::MouseEnd(TShiftState Shift)
{
	switch(m_Action){
    case eaSelect: 	break;
    case eaAdd: 	break;
    case eaMove:{
    	switch (m_EditMode){
        case emObject:
//.            m_ObjectItems->RefreshForm();
        break;
        case emBone:
	        OnBoneModified();
        break;
        }
    }break;
    case eaRotate:{
    	switch (m_EditMode){
        case emObject:
//.            m_ObjectItems->RefreshForm();
        break;
        case emBone:
	        OnBoneModified();
        break;
        }
    }break;
    case eaScale:{
    	switch (m_EditMode){
        case emBone:
	        OnBoneModified();
        break;
        }
    }break;
    }
	return true;
}

void __fastcall CActorTools::OnBoneModified(void)
{
	RefreshSubProperties	();
	m_bObjectModified 		= true;
	UI.Command				(COMMAND_UPDATE_CAPTION);
}

void __fastcall CActorTools::MouseMove(TShiftState Shift)
{
	if (!m_pEditObject) return;
	switch(m_Action){
    case eaSelect: 	break;
    case eaAdd: 	break;
    case eaMove:{
    	Fvector amount;
        amount.mul( m_MovingXVector, UI.m_MouseSM * UI.m_DeltaCpH.x );
        amount.mad( amount, m_MovingYVector, -UI.m_MouseSM * UI.m_DeltaCpH.y );

        if( fraTopBar->ebMSnap->Down ){
        	CHECK_SNAP(m_MovingReminder.x,amount.x,UI.movesnap());
        	CHECK_SNAP(m_MovingReminder.y,amount.y,UI.movesnap());
        	CHECK_SNAP(m_MovingReminder.z,amount.z,UI.movesnap());
        }

        if (!fraTopBar->ebAxisX->Down&&!fraTopBar->ebAxisZX->Down) amount.x = 0.f;
        if (!fraTopBar->ebAxisZ->Down&&!fraTopBar->ebAxisZX->Down) amount.z = 0.f;
        if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;
    	switch (m_EditMode){
        case emObject:
            m_pEditObject->a_vPosition.add(amount);
			OnMotionKeysModified();
//            m_ObjectProps->RefreshForm();
//            OnObjectModified();
        break;
        case emBone:{
        	BoneVec lst;
	        if (m_pEditObject->GetSelectedBones(lst))
            	for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                	(*b_it)->ShapeMove(amount);
//	        OnBoneModified();
        }break;
        }
    }break;
    case eaRotate:{
        float amount = -UI.m_DeltaCpH.x * UI.m_MouseSR;
        if( fraTopBar->ebASnap->Down ) CHECK_SNAP(m_fRotateSnapAngle,amount,UI.anglesnap());
    	switch (m_EditMode){
        case emObject:
            m_pEditObject->a_vRotate.mad(m_RotateVector,amount);
			OnMotionKeysModified();
//            m_ObjectProps->RefreshForm();
//            OnObjectModified();
        break;
        case emBone:{
        	BoneVec lst;
            Fvector rot;
            rot.mul(m_RotateVector,amount);
	        if (m_pEditObject->GetSelectedBones(lst)){
            	if (Shift.Contains(ssCtrl)){
                    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                        (*b_it)->ShapeRotate(rot);
                }else{
                    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                        (*b_it)->BoneRotate(rot);
                }
            }
//	        OnBoneModified();
        }break;
        }
    }break;
    case eaScale:{
        float dy = UI.m_DeltaCpH.x * UI.m_MouseSS;
        if (dy>1.f) dy=1.f; else if (dy<-1.f) dy=-1.f;

        Fvector amount;
        amount.set( dy, dy, dy );

        if (fraTopBar->ebNonUniformScale->Down){
            if (!fraTopBar->ebAxisX->Down && !fraTopBar->ebAxisZX->Down) amount.x = 0.f;
            if (!fraTopBar->ebAxisZ->Down && !fraTopBar->ebAxisZX->Down) amount.z = 0.f;
            if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;
        }
    	switch (m_EditMode){
        case emBone:{
        	BoneVec lst;
	        if (m_pEditObject->GetSelectedBones(lst))
            	for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                	(*b_it)->ShapeScale(amount);
//	        OnBoneModified();
        }break;
        }
//        m_EditObject->t_vScale.add(amount);
    }break;
    }
//    m_PSProps->fraEmitter->GetInfoFirst(m_EditPS.m_DefaultEmitter);
}

void CActorTools::WorldMotionRotate(const Fvector& R)
{
	R_ASSERT(m_pEditObject&&m_pEditObject->GetActiveSMotion());
	CSMotion* M = m_pEditObject->GetActiveSMotion();
    int rootId = m_pEditObject->GetRootBoneID();
    M->WorldRotate(rootId,R.y,R.x,R.z);
    OnMotionKeysModified();
}

CSMotion* CActorTools::GetCurrentMotion()
{
	return m_pEditObject?m_pEditObject->GetActiveSMotion():0;
}

void CActorTools::SetCurrentMotion(LPCSTR name)
{
	if (m_pEditObject){
    	m_pEditObject->SetActiveSMotion(m_pEditObject->FindSMotionByName(name));
        PlayMotion();
    }
}

void CActorTools::GetCurrentFog(u32& fog_color, float& s_fog, float& e_fog)
{
	s_fog		= UI.ZFar();
	e_fog		= UI.ZFar();
	fog_color	= DEFAULT_CLEARCOLOR;
}

LPCSTR CActorTools::GetInfo()
{
	return 0;
}

bool CActorTools::Pick(TShiftState Shift)
{
    return false;
}

