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
#include "xr_trims.h"
#include "xr_tokens.h"
#include "PropertiesList.h"
#include "motion.h"
#include "bone.h"
#include "library.h"
#include "BodyInstance.h"
#include "fmesh.h"
#include "KeyBar.h"
#include "main.h"

//------------------------------------------------------------------------------
CActorTools Tools;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

void CActorTools::PreviewModel::RestoreParams(TFormStorage* s)
{          
    m_Props->RestoreColumnWidth(s);
    m_LastObjectName	= s->ReadString	("preview_name","");
    int val;
    val					= s->ReadInteger("preview_speed",0); 	m_fSpeed 	= *((float*)&val);
    val					= s->ReadInteger("preview_segment",0); 	m_fSegment	= *((float*)&val);
    m_dwFlags			= s->ReadInteger("preview_flags",0);
    m_ScrollAxis		= s->ReadInteger("preview_scaxis",0);
}

void CActorTools::PreviewModel::SaveParams(TFormStorage* s)
{
    m_Props->SaveColumnWidth(s);
    s->WriteString	("preview_name",	m_LastObjectName);
    s->WriteInteger	("preview_speed",	*((int*)&m_fSpeed));
    s->WriteInteger	("preview_segment",	*((int*)&m_fSegment));
    s->WriteInteger	("preview_flags",	m_dwFlags);
    s->WriteInteger	("preview_scaxis",	m_ScrollAxis);
}

void CActorTools::PreviewModel::OnDestroy()
{
	TfrmProperties::DestroyProperties(m_Props);
}

void CActorTools::PreviewModel::OnCreate()
{
    m_Props = TfrmProperties::CreateProperties(0,alNone);
}

void CActorTools::PreviewModel::Clear()
{
    Lib.RemoveEditObject(m_pObject);
}
void CActorTools::PreviewModel::SelectObject()
{
    LPCSTR fn=m_LastObjectName.c_str();
    fn=TfrmChoseItem::SelectObject(false,0,fn);
    if (!fn) return;
    Lib.RemoveEditObject(m_pObject);
    m_pObject = Lib.CreateEditObject(fn);
    if (!m_pObject)	ELog.DlgMsg(mtError,"Object '%s' can't find in object library.",fn);
    else			m_LastObjectName = fn;
}

xr_token		sa_token					[ ]={
	{ "+Z",		CActorTools::PreviewModel::saZp	},
	{ "-Z",		CActorTools::PreviewModel::saZn	},
	{ 0,		0								}
};

void CActorTools::PreviewModel::SetPreferences()
{
    m_Props->BeginFillMode("Preview prefs");
	m_Props->AddItem	(0,PROP_FLAG,	"Scroll",		m_Props->MakeFlagValue(&m_dwFlags,pmScroll));
	m_Props->AddItem	(0,PROP_FLOAT, 	"Speed (m/c)",	m_Props->MakeFloatValue(&m_fSpeed, -10000.f,10000.f,0.01f,2));
	m_Props->AddItem	(0,PROP_FLOAT, 	"Segment (m)",	m_Props->MakeFloatValue(&m_fSegment, -10000.f,10000.f,0.01f,2));
    m_Props->AddItem	(0,PROP_TOKEN,	"Scroll axis",	m_Props->MakeTokenValue(&m_ScrollAxis,sa_token));
    m_Props->EndFillMode();
    m_Props->ShowProperties();
}
void CActorTools::PreviewModel::Render()
{
	if (m_pObject){
		Fmatrix T;
    	T.translate(m_vPosition);
    	m_pObject->RenderSingle(T);
    }
}
void CActorTools::PreviewModel::Update()
{
    if (m_dwFlags&pmScroll){
    	switch (m_ScrollAxis){
        case saZp:
		    m_vPosition.z -= m_fSpeed*Device.fTimeDelta;
    	    if (m_vPosition.z<-m_fSegment) m_vPosition.z+=m_fSegment;
        break;
        case saZn:
		    m_vPosition.z += m_fSpeed*Device.fTimeDelta;
    	    if (m_vPosition.z>m_fSegment) m_vPosition.z-=m_fSegment;
        break;
        default: THROW;
        }
    }
}

CActorTools::CActorTools()
{
	m_pEditObject		= 0;
    m_bObjectModified	= false;
    m_bMotionModified	= false;
    m_ObjectProps 		= 0;
    m_MotionProps 		= 0;
    m_bReady			= false;
    m_KeyBar			= 0;
}
//---------------------------------------------------------------------------

CActorTools::~CActorTools()
{
}
//---------------------------------------------------------------------------

bool CActorTools::OnCreate(){
    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);

    // props
    m_ObjectProps = TfrmProperties::CreateProperties(fraLeftBar->paObjectProps,alClient,ObjectModified);
    m_MotionProps = TfrmProperties::CreateProperties(fraLeftBar->paMotionProps,alClient,MotionModified);
    m_PreviewObject.OnCreate();

    // key bar
	m_KeyBar 		= TfrmKeyBar::CreateKeyBar(frmMain->paMain);

    m_bReady = true;

//	m_PSProps->SetCurrent(0);
    ChangeAction(eaSelect);

	// lock
//    FS.LockFile(0,fn.c_str());
    return true;
}

void CActorTools::OnDestroy(){
	VERIFY(m_bReady);
    m_bReady			= false;

	// unlock
	TfrmProperties::DestroyProperties(m_ObjectProps);
	TfrmProperties::DestroyProperties(m_MotionProps);
    m_PreviewObject.OnDestroy();

    m_PreviewObject.Clear();
    m_RenderObject.Clear();
	_DELETE(m_pEditObject);
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
}
//---------------------------------------------------------------------------

bool CActorTools::IfModified(){
    if (IsModified()){
        int mr = ELog.DlgMsg(mtConfirmation, "The '%s' has been modified.\nDo you want to save your changes?",UI.GetEditFileName());
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; else{ m_bObjectModified = false; m_bMotionModified = false; }break;
        case mrNo: m_bObjectModified = false; m_bMotionModified = false; break;
        case mrCancel: return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------

void CActorTools::ObjectModified(){
	m_bObjectModified = true;
	UI.Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CActorTools::SetPreviewObjectPrefs(){
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

void CActorTools::Render(){
	if (!m_bReady) return;
    m_PreviewObject.Render();
	if (m_pEditObject){
    	// update matrix
        Fmatrix	mTransform,mTranslate,mRotate;
        mRotate.setHPB			(m_pEditObject->a_vRotate.y, m_pEditObject->a_vRotate.x, m_pEditObject->a_vRotate.z);
        mTranslate.translate	(m_pEditObject->a_vPosition);
        mTransform.mul			(mTranslate,mRotate);
        if (m_RenderObject.IsRenderable()&&fraLeftBar->ebRenderEngineStyle->Down){
        	m_RenderObject.Render(mTransform);
        }else{
	        // update transform matrix
    		m_pEditObject->RenderSkeletonSingle(mTransform);
        }
    }
}
//---------------------------------------------------------------------------

void CActorTools::Update(){
	if (!m_bReady) return;
    if (m_KeyBar) m_KeyBar->UpdateBar();
    m_PreviewObject.Update();
	if (m_pEditObject){
    	if (m_RenderObject.IsRenderable()&&m_pEditObject->IsSkeleton())
        	PKinematics(m_RenderObject.m_pVisual)->Calculate(1.f);
    	m_pEditObject->OnFrame();
    }
}

void CActorTools::ZoomObject(){
	VERIFY(m_bReady);
    if (m_pEditObject)
        Device.m_Camera.ZoomExtents(m_pEditObject->GetBox());
}

void CActorTools::OnDeviceCreate(){
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

void CActorTools::Clear(){
	VERIFY(m_bReady);

    // delete visuals
    _DELETE(m_pEditObject);
    m_RenderObject.Clear();
//	m_PreviewObject.Clear();
    m_ObjectProps->ClearProperties();
    m_MotionProps->ClearProperties();
    fraLeftBar->SkeletonPartEnabled(false);

	m_bObjectModified = false;
	m_bMotionModified = false;

    UI.RedrawScene();
}

bool CActorTools::Load(LPCSTR name)
{
	VERIFY(m_bReady);
	CEditableObject* O = new CEditableObject(name);
	if (O->Load(name)&&O->IsFlag(CEditableObject::eoDynamic)){
//    	O->SetFlag(CEditableObject::eoDynamic);
    	_DELETE(m_pEditObject);
        m_pEditObject = O;
        // delete visual
        m_RenderObject.Clear();
        fraLeftBar->SetRenderStyle(false);
		fraLeftBar->SkeletonPartEnabled(m_pEditObject->IsSkeleton());
        return true;
    }
    _DELETE(O);

    return false;
}

bool CActorTools::Save(LPCSTR name)
{
	VERIFY(m_bReady);
    if (m_pEditObject){
    	m_pEditObject->SaveObject(name);
		m_bObjectModified = false; m_bMotionModified = false;
        return true;
    }
	return false;
}

bool CActorTools::ExportSkeleton(LPCSTR name)
{
	VERIFY(m_bReady);
    if (m_pEditObject&&m_pEditObject->ExportSkeletonOGF(name)) return true;
    return false;
}

bool CActorTools::ExportObject(LPCSTR name)
{
	VERIFY(m_bReady);
    if (m_pEditObject&&m_pEditObject->ExportObjectOGF(name)) return true;
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

bool __fastcall CActorTools::MouseStart(TShiftState Shift)
{
	switch(m_Action){
    case eaSelect: return false;
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
	return true;
}

void __fastcall CActorTools::MouseMove(TShiftState Shift)
{
	if (!m_pEditObject) return;
	switch(m_Action){
    case eaSelect: return;
    case eaAdd: break;
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
		m_pEditObject->a_vPosition.add(amount);
        m_ObjectProps->RefreshProperties();
        ObjectModified();
    }break;
    case eaRotate:{
        float amount = -UI.m_DeltaCpH.x * UI.m_MouseSR;
        if( fraTopBar->ebASnap->Down ) CHECK_SNAP(m_fRotateSnapAngle,amount,UI.anglesnap());
        m_pEditObject->a_vRotate.mad(m_RotateVector,amount);
        m_ObjectProps->RefreshProperties();
        ObjectModified();
    }break;
    case eaScale:{
/*        float dy = UI.m_DeltaCpH.x * UI.m_MouseSS;
        if (dy>1.f) dy=1.f; else if (dy<-1.f) dy=-1.f;

        Fvector amount;
        amount.set( dy, dy, dy );

        if (fraTopBar->ebNonUniformScale->Down){
            if (!fraTopBar->ebAxisX->Down && !fraTopBar->ebAxisZX->Down) amount.x = 0.f;
            if (!fraTopBar->ebAxisZ->Down && !fraTopBar->ebAxisZX->Down) amount.z = 0.f;
            if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;
        }
        m_EditObject->t_vScale.add(amount);
*/    }break;
    }
//    m_PSProps->fraEmitter->GetInfoFirst(m_EditPS.m_DefaultEmitter);
}

void CActorTools::SetCurrentMotion(LPCSTR name)
{
	if (m_pEditObject){
    	m_pEditObject->SetActiveSMotion(m_pEditObject->FindSMotionByName(name));
        PlayMotion();
    }
}

void __fastcall CActorTools::RotateOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	Fvector* V = (Fvector*)edit_val;
	V->x = deg2rad(V->x);
	V->y = deg2rad(V->y);
	V->z = deg2rad(V->z);
	UI.RedrawScene();
}

void __fastcall CActorTools::RotateOnBeforeEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	Fvector* V = (Fvector*)edit_val;
	V->x = rad2deg(V->x);
	V->y = rad2deg(V->y);
	V->z = rad2deg(V->z);
}

void __fastcall CActorTools::RotateOnDraw(PropValue* sender, LPVOID draw_val)
{
	Fvector* V = (Fvector*)draw_val;
	V->x = rad2deg(V->x);
	V->y = rad2deg(V->y);
	V->z = rad2deg(V->z);
}
void __fastcall CActorTools::OnAfterTransformation(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	UI.RedrawScene();
}
void __fastcall CActorTools::OnAfterShaderEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	AnsiString new_name = *(AnsiString*)edit_val;
	TElTreeItem* parent	= item->Parent; VERIFY(parent);
	CSurface* surf 		= (CSurface*)parent->Data;	VERIFY(surf);
    surf->DeleteShader	();
    surf->ED_SetShader	(new_name.c_str());
    surf->CreateShader	();
}
//---------------------------------------------------------------------------

void __fastcall CActorTools::OnAfterTextureEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	AnsiString new_name = *(AnsiString*)edit_val;
	TElTreeItem* parent	= item->Parent; VERIFY(parent);
	CSurface* surf 		= (CSurface*)parent->Data; 	VERIFY(surf);
    surf->DeleteShader	();
    surf->SetTexture	(new_name.c_str());
    surf->CreateShader	();
}
//---------------------------------------------------------------------------
void CActorTools::FillObjectProperties()
{
	R_ASSERT(m_pEditObject);
    m_ObjectProps->BeginFillMode();
    TElTreeItem* M=0;
    TElTreeItem* N=0;
	m_ObjectProps->AddItem(0,PROP_FLAG,	"Make progressive",	m_ObjectProps->MakeFlagValue(&m_pEditObject->GetFlags(),CEditableObject::eoProgressive));
    M = m_ObjectProps->AddItem(0,PROP_MARKER,	"Transformation");
    {
		m_ObjectProps->AddItem	(M,PROP_VECTOR,	"Position",	m_ObjectProps->MakeVectorValue(&m_pEditObject->a_vPosition,	-10000,	10000,0.01,2,OnAfterTransformation));
		m_ObjectProps->AddItem	(M,PROP_VECTOR,	"Rotation",	m_ObjectProps->MakeVectorValue(&m_pEditObject->a_vRotate,	-10000,	10000,0.1,1,RotateOnAfterEdit,RotateOnBeforeEdit,RotateOnDraw));
    }
    // surfaces
    M = m_ObjectProps->AddItem(0,PROP_MARKER,		"Surfaces");
    for (SurfaceIt s_it=m_pEditObject->FirstSurface(); s_it!=m_pEditObject->LastSurface(); s_it++){
        CSurface* SURF=*s_it;
        N=m_ObjectProps->AddItem(M,PROP_MARKER,SURF->_Name(),SURF);
        m_ObjectProps->AddItem(N,PROP_ANSI_SH_ENGINE,	"Shader",	m_ObjectProps->MakeAnsiTextValue(&SURF->m_ShaderName, 	OnAfterShaderEdit));
//        m_ObjectProps->AddItem(N,PROP_ANSI_SH_COMPILE,	"Compile",	m_ObjectProps->MakeAnsiTextValue(&SURF->m_ShaderXRLCName));
        m_ObjectProps->AddItem(N,PROP_ANSI_TEXTURE,		"Texture",	m_ObjectProps->MakeAnsiTextValue(&SURF->m_Texture,		OnAfterTextureEdit));
    }
    // geometry
    M = m_ObjectProps->AddItem	(0,PROP_MARKER,	"Summary");
    AnsiString t; t.sprintf("V: %d, F: %d",m_pEditObject->GetVertexCount(),m_pEditObject->GetFaceCount());
    m_ObjectProps->AddItem		(M,PROP_MARKER2,"Object",t.c_str());
    M = m_ObjectProps->AddItem	(M,PROP_MARKER,	"Meshes");
    for (EditMeshIt m_it=m_pEditObject->FirstMesh(); m_it!=m_pEditObject->LastMesh(); m_it++){
        CEditableMesh* MESH=*m_it;
        t.sprintf("V: %d, F: %d",MESH->GetVertexCount(),MESH->GetFaceCount());
        m_ObjectProps->AddItem(M,PROP_MARKER2,MESH->GetName(),t.c_str());
    }
	m_ObjectProps->EndFillMode(false);
}
                               

