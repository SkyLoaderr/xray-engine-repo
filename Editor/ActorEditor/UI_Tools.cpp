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
}

void CActorTools::PreviewModel::SaveParams(TFormStorage* s)
{
    m_Props->SaveColumnWidth(s);
    s->WriteString	("preview_name",	m_LastObjectName);
    s->WriteInteger	("preview_speed",	*((int*)&m_fSpeed));
    s->WriteInteger	("preview_segment",	*((int*)&m_fSegment));
    s->WriteInteger	("preview_flags",	m_dwFlags);
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
void CActorTools::PreviewModel::SetPreferences()
{
    m_Props->BeginFillMode("Preview object prefs");
	m_Props->AddItem	(0,PROP_FLAG,	"Scroll",		m_Props->MakeFlagValue(&m_dwFlags,pmScroll));
	m_Props->AddItem	(0,PROP_FLOAT, 	"Speed (m/c)",	m_Props->MakeFloatValue(&m_fSpeed, -10000.f,10000.f,0.01f,2));
	m_Props->AddItem	(0,PROP_FLOAT, 	"Segment (m)",	m_Props->MakeFloatValue(&m_fSegment, -10000.f,10000.f,0.01f,2));
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
	    m_vPosition.z -= m_fSpeed*Device.fTimeDelta;
        if (m_vPosition.z<-m_fSegment)
        	m_vPosition.z+=m_fSegment;
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
}
//---------------------------------------------------------------------------

CActorTools::~CActorTools()
{
}
//---------------------------------------------------------------------------

bool CActorTools::OnCreate(){
	// shader test locking
//	AnsiString fn = "particles.xr"; FS.m_GameRoot.Update(fn);
//	if (FS.CheckLocking(0,fn.c_str(),false,true))
//    	return false;

    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);

    // props
    m_ObjectProps = TfrmProperties::CreateProperties(fraLeftBar->paObjectProps,alClient,ObjectModified);
    m_MotionProps = TfrmProperties::CreateProperties(fraLeftBar->paMotionProps,alClient,MotionModified);
    m_PreviewObject.OnCreate();

    // key bar
	TfrmKeyBar* B = new TfrmKeyBar(frmMain->paMain);
    B->Parent 	= frmMain->paMain;
    B->Align    = alBottom;
    B->Show();


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
	        // render visual
            Device.SetTransform	(D3DTS_WORLD,mTransform);
            switch (m_RenderObject.m_pVisual->Type)
            {
            case MT_SKELETON:{
                CKinematics* pV					= (CKinematics*)m_RenderObject.m_pVisual;
                vector<FBasicVisual*>::iterator I,E;
                I = pV->chields.begin			();
                E = pV->chields.end				();
                for (; I!=E; I++)
                {
                    FBasicVisual* V				= *I;
                    Device.Shader.set_Shader	(V->hShader);
                    V->Render					(1.f);
                }
            }break;
            case MT_HIERRARHY:{
                FHierrarhyVisual* pV			= (FHierrarhyVisual*)m_RenderObject.m_pVisual;
                vector<FBasicVisual*>::iterator I,E;
                I = pV->chields.begin			();
                E = pV->chields.end				();
                for (; I!=E; I++)
                {
                    FBasicVisual* V				= *I;
                    Device.Shader.set_Shader	(V->hShader);
                    V->Render					(1.f);
                }
            }break;
            default:
                Device.Shader.set_Shader		(m_RenderObject.m_pVisual->hShader);
                m_RenderObject.m_pVisual->Render		(1.f);
                break;
            }
        }else{
	        // update transform matrix
    		m_pEditObject->RenderSkeletonSingle(mTransform);
        }
    }
}

void CActorTools::Update(){
	if (!m_bReady) return;
    m_PreviewObject.Update();
	if (m_pEditObject){
    	if (m_RenderObject.IsRenderable()) PKinematics(m_RenderObject.m_pVisual)->Calculate(1.f);
    	m_pEditObject->RTL_Update(Device.fTimeDelta);
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

bool CActorTools::Export(LPCSTR name)
{
	VERIFY(m_bReady);
    if (m_pEditObject&&m_pEditObject->ExportSkeletonOGF(name)) return true;
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
        m_RotateCenter.set( UI.pivot() );
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
        amount.direct( amount, m_MovingYVector, -UI.m_MouseSM * UI.m_DeltaCpH.y );

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
        if (fraLeftBar->ebRenderEngineStyle->Down) PlayMotion();
    }
}

void __fastcall CActorTools::FloatOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
    *(float*)edit_val = deg2rad(*(float*)edit_val);
}

void __fastcall CActorTools::FloatOnBeforeEdit(PropValue* sender, LPVOID edit_val)
{
    *(float*)edit_val = rad2deg(*(float*)edit_val);
}

void __fastcall CActorTools::FloatOnDraw(PropValue* sender, LPVOID draw_val)
{
    *(float*)draw_val = rad2deg(*(float*)draw_val);
}

void CActorTools::FillObjectProperties()
{
	R_ASSERT(m_pEditObject);
    m_ObjectProps->BeginFillMode();
    TElTreeItem* M=0;
    TElTreeItem* N=0;
	m_ObjectProps->AddItem(0,PROP_FLAG,	"Make progressive",	m_ObjectProps->MakeFlagValue(&m_pEditObject->GetFlags(),CEditableObject::eoProgressive));
    M = m_ObjectProps->AddItem(0,PROP_MARKER,	"Transformation");
    {
	    N = m_ObjectProps->AddItem(M,PROP_MARKER,	"Rotate (Grd)");
        {
			m_ObjectProps->AddItem(N,PROP_FLOAT, 	"Yaw", 		m_ObjectProps->MakeFloatValue(&m_pEditObject->a_vRotate.y,	-10000.f,10000.f,0.01f,2,FloatOnAfterEdit,FloatOnBeforeEdit,FloatOnDraw));
			m_ObjectProps->AddItem(N,PROP_FLOAT, 	"Pitch", 	m_ObjectProps->MakeFloatValue(&m_pEditObject->a_vRotate.x,	-10000.f,10000.f,0.01f,2,FloatOnAfterEdit,FloatOnBeforeEdit,FloatOnDraw));
			m_ObjectProps->AddItem(N,PROP_FLOAT, 	"Heading", 	m_ObjectProps->MakeFloatValue(&m_pEditObject->a_vRotate.z,	-10000.f,10000.f,0.01f,2,FloatOnAfterEdit,FloatOnBeforeEdit,FloatOnDraw));
        }
	    N = m_ObjectProps->AddItem(M,PROP_MARKER,	"Offset");
        {
			m_ObjectProps->AddItem(N,PROP_FLOAT, 	"X", 		m_ObjectProps->MakeFloatValue(&m_pEditObject->a_vPosition.x,	-100000.f,100000.f,0.01f,2));
			m_ObjectProps->AddItem(N,PROP_FLOAT, 	"Y", 		m_ObjectProps->MakeFloatValue(&m_pEditObject->a_vPosition.y,	-100000.f,100000.f,0.01f,2));
			m_ObjectProps->AddItem(N,PROP_FLOAT, 	"Z", 		m_ObjectProps->MakeFloatValue(&m_pEditObject->a_vPosition.z,	-100000.f,100000.f,0.01f,2));
        }
    }
    M = m_ObjectProps->AddItem(0,PROP_MARKER,		"Surfaces");
    {
        for (SurfaceIt& it=m_pEditObject->FirstSurface(); it!=m_pEditObject->LastSurface(); it++){
            CSurface* S=*it;
            N = m_ObjectProps->AddItem(M,PROP_MARKER,	S->_Name());
            {
                m_ObjectProps->AddItem(N,PROP_S_SH_ENGINE,	"Shader",	&S->m_ShaderName);
//                m_ObjectProps->AddItem(N,PROP_S_SH_COMPILE,	"Compile",	&S->m_ShaderXRLCName);
                m_ObjectProps->AddItem(N,PROP_S_TEXTURE,		"Texture",	&S->m_Texture);
            }
        }
    }
    M = m_ObjectProps->AddItem(0,PROP_MARKER,	"Geometry");
    {
        m_ObjectProps->AddItem(M,PROP_MARKER2,"Vertex Count",	AnsiString(m_pEditObject->GetVertexCount()).c_str());
        m_ObjectProps->AddItem(M,PROP_MARKER2,"Face Count",		AnsiString(m_pEditObject->GetFaceCount()).c_str());
        for (EditMeshIt& it=m_pEditObject->FirstMesh(); it!=m_pEditObject->LastMesh(); it++){
            CEditableMesh* S=*it;
            N = m_ObjectProps->AddItem(M,PROP_MARKER,	S->GetName());
            {
                m_ObjectProps->AddItem(N,PROP_MARKER2,"Vertex Count",	AnsiString(S->GetVertexCount()).c_str());
                m_ObjectProps->AddItem(N,PROP_MARKER2,"Face Count",		AnsiString(S->GetFaceCount()).c_str());
            }
        }
    }
	m_ObjectProps->EndFillMode(false);
}

