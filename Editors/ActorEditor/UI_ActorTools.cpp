//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_ActorTools.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "PropertiesList.h"               
#include "motion.h"
#include "bone.h"
#include "library.h"
#include "fmesh.h"
#include "KeyBar.h"
#include "main.h"
#include "folderlib.h"
#include "SkeletonAnimated.h"
#include "ItemList.h"

CActorTools*&	ATools=(CActorTools*)Tools;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

void CActorTools::PreviewModel::RestoreParams(TFormStorage* s)
{          
    m_Props->RestoreParams(s);
    m_LastObjectName	= s->ReadString	("preview_name","");
    int val;
    val					= s->ReadInteger("preview_speed",0); 	m_fSpeed 	= *((float*)&val);
    val					= s->ReadInteger("preview_segment",0); 	m_fSegment	= *((float*)&val);
    m_Flags.assign		(s->ReadInteger("preview_flags",0));
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
    m_Props = TProperties::CreateForm("Preview prefs",0,alNone);
}

void CActorTools::PreviewModel::Clear()
{
    Lib.RemoveEditObject(m_pObject);
}
void CActorTools::PreviewModel::SelectObject()
{
    LPCSTR fn;
    if (!TfrmChoseItem::SelectItem(smObject,fn,1,m_LastObjectName.c_str())) return;
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
    PHelper().CreateFlag32		(items, 	"Scroll", 		&m_Flags, 		pmScroll);
    PHelper().CreateFloat			(items, 	"Speed (m/c)",	&m_fSpeed,		-10000.f,10000.f,0.01f,2);
    PHelper().CreateFloat			(items, 	"Segment (m)",	&m_fSegment,	-10000.f,10000.f,0.01f,2);
    PHelper().CreateToken32		(items,		"Scroll axis",	(u32*)&m_ScrollAxis,	sa_token);
	m_Props->AssignItems		(items);
    m_Props->ShowProperties		();
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

#include "EThumbnail.h"
void _SynchronizeTextures()
{   
	FS_QueryMap M_THUM;
    FS.file_list(M_THUM,_textures_,FS_ListFiles|FS_ClampExt,".thm");

    FS_QueryPairIt it	= M_THUM.begin();
	FS_QueryPairIt _E 	= M_THUM.end();
	for (; it!=_E; it++){
		ETextureThumbnail* THM=0;
        THM = xr_new<ETextureThumbnail>(it->first.c_str());
        STextureParams& fmt = THM->_Format();
        if (fmt.material==STextureParams::tmOrenNayar_Blin){
        	fmt.material=STextureParams::tmBlin_Phong;
	        THM->Save(0,0);
        }
	    xr_delete(THM);
    }
}


CActorTools::CActorTools()
{
	//. _SynchronizeTextures();
    m_Props 			= 0;
	m_pEditObject		= 0;
    m_bObjectModified	= false;
    m_ObjectItems 		= 0;
    m_bReady			= false;
    m_KeyBar			= 0;
    m_Flags.zero		();
    m_EditMode			= emObject;
    fFogness			= 0.9f;
    dwFogColor			= 0xffffffff;
}
//---------------------------------------------------------------------------

CActorTools::~CActorTools()
{
}
//---------------------------------------------------------------------------

bool CActorTools::OnCreate()
{
	inherited::OnCreate();
    // props
	m_ClipMaker		= TClipMaker::CreateForm();
    m_ObjectItems 	= TItemList::CreateForm("",fraLeftBar->paObjectProps,alClient,TItemList::ilDragCustom|TItemList::ilMultiSelect|TItemList::ilSuppressStatus);
	m_ObjectItems->SetOnItemsFocusedEvent(TOnILItemsFocused(this,&CActorTools::OnObjectItemFocused));
    m_Props 		= TProperties::CreateForm("",fraLeftBar->paItemProps,alClient,TOnModifiedEvent().bind(this,&CActorTools::OnItemModified));
    m_PreviewObject.OnCreate();

    // key bar
	m_KeyBar 		= TfrmKeyBar::CreateKeyBar(frmMain->paMain);

    return true;
}

void CActorTools::OnDestroy()
{
	inherited::OnDestroy();

    TClipMaker::DestroyForm	(m_ClipMaker);
	TItemList::DestroyForm	(m_ObjectItems);
	TProperties::DestroyForm(m_Props);
    m_PreviewObject.OnDestroy();

    m_PreviewObject.Clear();
    m_RenderObject.Clear();
	xr_delete(m_pEditObject);
}
//---------------------------------------------------------------------------

void CActorTools::Modified()
{
	m_bObjectModified	= true;
    ExecCommand			(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

bool CActorTools::IfModified(){
    if (IsModified()){
        int mr = ELog.DlgMsg(mtConfirmation, "The '%s' has been modified.\nDo you want to save your changes?",GetEditFileName());
        switch(mr){
        case mrYes: if (!ExecCommand(COMMAND_SAVE)) return false; else{m_bObjectModified = false;}break;
        case mrNo: m_bObjectModified = false; break;
        case mrCancel: return false;
        }
    }
    return true;   
}
//---------------------------------------------------------------------------

void CActorTools::OnObjectModified()
{
    m_Flags.set				(flUpdateGeometry,TRUE);
    OnGeometryModified		();
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
			SAnimParams& P=m_pEditObject->m_SMParam;
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
//#include "d3dutils.h"
//DU.DrawIdentSpherePart(false,true,0xffffffff,0xffff);

void CActorTools::Render()
{
	if (!m_bReady) return;
	PrepareLighting			();
    m_PreviewObject.Render	();
	if (m_pEditObject){
        if (m_RenderObject.IsRenderable()&&fraLeftBar->ebRenderEngineStyle->Down){
        	::Render->model_RenderSingle(m_RenderObject.m_pVisual,Fidentity,m_RenderObject.m_fLOD);
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
        	PKinematics(m_RenderObject.m_pVisual)->CalculateBones(1.f);
    	m_pEditObject->OnFrame();
    }
    if (m_Flags.is(flRefreshShaders)){
    	m_Flags.set(flRefreshShaders,FALSE);
        m_pEditObject->OnDeviceDestroy();
    }
    if (m_Flags.is(flRefreshSubProps)){
    	m_Flags.set(flRefreshSubProps,FALSE);
        ListItemsVec items;
        m_ObjectItems->GetSelected(0,items,false);
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

void CActorTools::PrepareLighting()
{
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
}

void CActorTools::OnDeviceCreate()
{
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
    m_CurrentMotion		= "";
    // delete visuals
    xr_delete(m_pEditObject);
    m_RenderObject.Clear();
//	m_PreviewObject.Clear();
    m_ObjectItems->ClearList();
    m_Props->ClearProperties();
    m_ClipMaker->HideEditor();

	m_bObjectModified 	= false;
	m_Flags.set			(flUpdateGeometry|flUpdateMotionDefs|flUpdateMotionKeys,FALSE);
    m_EditMode			= emObject;
    
    UI->RedrawScene		();
}

bool CActorTools::Import(LPCSTR initial, LPCSTR obj_name)
{
    std::string 	full_name;
    if (initial)	FS.update_path	(full_name,initial,obj_name);
    else			full_name 		= obj_name;
    
	VERIFY(m_bReady);
	CEditableObject* O = xr_new<CEditableObject>(obj_name);
	if (O->Load(full_name.c_str())){
        O->m_Flags.set(CEditableObject::eoDynamic,TRUE);
        xr_delete(m_pEditObject);
        m_pEditObject = O;
        // delete visual
        m_RenderObject.Clear();
        fraLeftBar->SetRenderStyle(false);

        UpdateProperties();
        return true;
    }else{
       	ELog.DlgMsg(mtError,"Can't load object file '%s'.",obj_name);
    }
    xr_delete(O);

    return false;
}

bool CActorTools::Load(LPCSTR initial, LPCSTR obj_name)
{
    std::string 	full_name;
    if (initial)	FS.update_path	(full_name,initial,obj_name);
    else			full_name		= obj_name;

	VERIFY(m_bReady);
	CEditableObject* O = xr_new<CEditableObject>(obj_name);
	if (FS.exist(full_name.c_str())&&O->Load(full_name.c_str())){
        xr_delete(m_pEditObject);
        m_pEditObject 			= O;
        m_pEditObject->Optimize ();
        // delete visual
        m_RenderObject.Clear();
        fraLeftBar->SetRenderStyle(false); 

        UpdateProperties();
        return true;
    }else{
       	ELog.DlgMsg(mtError,"Can't load object file '%s'.",obj_name);
    }
    xr_delete(O);

    return false;
}

bool CActorTools::Save(LPCSTR initial, LPCSTR obj_name, bool bInternal)
{
    std::string full_name;
    if (initial)	FS.update_path	(full_name,initial,obj_name);
    else			full_name 		= obj_name;
	VERIFY(m_bReady);
    if (m_pEditObject){
    	EFS.MarkFile				(full_name.c_str(),true);
    	m_pEditObject->SaveObject	(full_name.c_str());
		if (!bInternal) m_bObjectModified = false;
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

bool CActorTools::ExportOMF(LPCSTR name)
{
	VERIFY(m_bReady);
    if (m_pEditObject&&m_pEditObject->ExportOMF(name)) return true;
    return false;
}

#include "EDetailModel.h"
bool CActorTools::ExportDM(LPCSTR name)
{
	VERIFY(m_bReady);
    if (m_pEditObject){
    	EDetail DM;
        if (!DM.Update(m_pEditObject->GetName())) return false;
        DM.Export(name);
        return true;
    }
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

void CActorTools::OnItemModified()
{
	switch(m_EditMode){
    case emObject:      OnObjectModified();		break;
    case emMotion: 		OnMotionDefsModified();	break;
    case emBone:      	OnBoneModified();		break;
    case emSurface:     OnObjectModified();		break;
    case emMesh:		break;
    }
}

void __fastcall CActorTools::OnBoneModified(void)
{
	Modified				();
	RefreshSubProperties	();
    UndoSave				();
}

extern AnsiString MakeFullBoneName(CBone* bone);
bool __fastcall CActorTools::MouseStart(TShiftState Shift)
{
	inherited::MouseStart(Shift);
	switch(m_Action){
    case etaSelect:
    	switch (m_EditMode){
        case emBone:{
	        CBone* B 	= m_pEditObject->PickBone(UI->m_CurrentRStart,UI->m_CurrentRNorm,m_AVTransform);
            bool bVal 	= B?Shift.Contains(ssAlt)?false:(Shift.Contains(ssCtrl)?!B->Selected():true):false;
            SelectListItem(BONES_PREFIX,B?MakeFullBoneName(B).c_str():0,bVal,Shift.Contains(ssCtrl)||Shift.Contains(ssAlt),true);
        }break;
        }
    break;
    case etaAdd:
    break;
    case etaMove:	break;
    case etaRotate:	break;
    }
	return m_bHiddenMode;
}

bool __fastcall CActorTools::MouseEnd(TShiftState Shift)
{
	inherited::MouseEnd(Shift);
	switch(m_Action){
    case etaSelect: 	break;
    case etaAdd: 	break;
    case etaMove:{
    	switch (m_EditMode){
        case emObject:
			if (Shift.Contains(ssCtrl))
	        	OnMotionKeysModified();	
        break;
        case emBone:	
			if (Shift.Contains(ssCtrl))
        		OnBoneModified();
            if (Shift.Contains(ssAlt))
        		OnBoneModified();
        break;
        }
    }break;
    case etaRotate:{
    	switch (m_EditMode){
        case emObject:	
			if (Shift.Contains(ssCtrl))
        		OnMotionKeysModified();	
        break;
        case emBone:	
			if (Shift.Contains(ssCtrl))
	        	OnBoneModified();		
            if (Shift.Contains(ssAlt))
        		OnBoneModified();
        break;
        }
    }break;
    case etaScale:{
    	switch (m_EditMode){
        case emBone:	
			if (Shift.Contains(ssCtrl))
		        OnBoneModified();		
        break;
        }
    }break;
    }
	return true;
}

void __fastcall CActorTools::MouseMove(TShiftState Shift)
{
	inherited::MouseMove(Shift);
	if (!m_pEditObject) return;
	switch(m_Action){
    case etaSelect: 	break;
    case etaAdd: 	break;
    case etaMove:{
    	switch (m_EditMode){
        case emObject:
			if (Shift.Contains(ssCtrl))
	            m_pEditObject->a_vPosition.add(m_MoveAmount);
        break;
        case emBone:
            BoneVec lst;
            if (m_pEditObject->GetSelectedBones(lst)){
                if (Shift.Contains(ssCtrl)){
                    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                        (*b_it)->ShapeMove(m_MoveAmount);
                }else if (Shift.Contains(ssAlt)){
                    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                        (*b_it)->BindMove(m_MoveAmount);
                    m_pEditObject->OnBindTransformChange();
                    RefreshSubProperties();
                }
            }
        break;
        }
    }break;
    case etaRotate:{
    	switch (m_EditMode){
        case emObject:
			if (Shift.Contains(ssCtrl))             
	            m_pEditObject->a_vRotate.mad(m_RotateVector,m_RotateAmount);
        break;
        case emBone:{
            BoneVec lst;
            Fvector rot;
            rot.mul(m_RotateVector,m_RotateAmount);
            if (m_pEditObject->GetSelectedBones(lst)){
                if (Shift.Contains(ssCtrl)){
                    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                        (*b_it)->ShapeRotate(rot);
                }else if (Shift.Contains(ssAlt)){
                    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                        (*b_it)->BindRotate(rot);
                    m_pEditObject->OnBindTransformChange();
	            	RefreshSubProperties();
                }else{
                    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                        (*b_it)->BoneRotate(m_RotateVector,m_RotateAmount);
	            	RefreshSubProperties();
                }
            }
        }break;
        }
    }break;
    case etaScale:{
    	switch (m_EditMode){
        case emBone:
        	if (Shift.Contains(ssCtrl)){
                BoneVec lst;
                if (m_pEditObject->GetSelectedBones(lst))
                    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
                        (*b_it)->ShapeScale(m_ScaleAmount);
			}
        break;
        }
    }break;
    }
}

void CActorTools::WorldMotionRotate(const Fvector& R)
{
	R_ASSERT(m_pEditObject&&(!m_CurrentMotion.IsEmpty()));
	CSMotion* M = m_pEditObject->FindSMotionByName(m_CurrentMotion.c_str());
    int rootId = m_pEditObject->GetRootBoneID();
    M->WorldRotate(rootId,R.y,R.x,R.z);
    OnMotionKeysModified();
}

CSMotion* CActorTools::GetCurrentMotion()
{
	return m_pEditObject?m_pEditObject->FindSMotionByName(m_CurrentMotion.c_str()):0;
}

CSMotion* CActorTools::FindMotion(LPCSTR name)
{
	return m_pEditObject?m_pEditObject->FindSMotionByName(name):0;
}

void CActorTools::SetCurrentMotion(LPCSTR name)
{
	if (m_pEditObject){
        if (m_CurrentMotion!=name){
        	m_CurrentMotion	= name;
            CSMotion* M 	= m_pEditObject->FindSMotionByName(name);
            if (M)			m_pEditObject->SetActiveSMotion(M);
            PlayMotion		();
        }
    }
}

bool CActorTools::Pick(TShiftState Shift)
{
    return false;
}

bool CActorTools::RayPick(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt, Fvector* n)
{
    if (m_PreviewObject.m_pObject)
    {
		SRayPickInfo pinf;
		if (m_PreviewObject.m_pObject->RayPick(dist,start,dir,Fidentity,&pinf)){
        	if (pt) pt->set(pinf.pt); 
            if (n){	
                const Fvector* PT[3];
                pinf.e_mesh->GetFacePT(pinf.inf.id, PT);
            	n->mknormal(*PT[0],*PT[1],*PT[2]);
            }
            return true;
        }else return false;
    }else
    {
    	Fvector np; np.mad(start,dir,dist);
    	if ((start.y>0)&&(np.y<0.f)){
            if (pt) pt->set(start); 
            if (n)	n->set(0.f,1.f,0.f);
            return true;
        }else return false;
    }
}

#include "ClipEditor.h"
void CActorTools::ShowClipMaker()
{
	if (CurrentObject()&&CurrentObject()->IsSkeleton()&&CurrentObject()->IsAnimated())
    	m_ClipMaker->ShowEditor(CurrentObject());
}

bool CActorTools::IsEngineMode()
{
    return (fraLeftBar->ebRenderEngineStyle->Down);
}

LPCSTR CActorTools::GetInfo()
{
	return 0;
}

void CActorTools::OptimizeMotions()
{
	if (m_pEditObject){
		m_pEditObject->OptimizeSMotions	();
	 	Modified						();
	    UndoSave						();
    }
}
