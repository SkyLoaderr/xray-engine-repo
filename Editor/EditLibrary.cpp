//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditLibrary.h"
#include "Library.h"
#include "UI_Main.h"
#include "propertiesobject.h"
#include "library.h"
#include "ui_tools.h"
#include "EditObject.h"
#include "EditObject.h"
#include "Main.h"
#include "Scene.h"
#include "Builder.h"
#include "Texture.h"
#include "BottomBar.h"
#include "FolderLib.h"
#include "ImageManager.h"
#include "ImageThumbnail.h"
#include "DetailObjects.h"
#include "xr_trims.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "ElHeader"
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"

TfrmEditLibrary* TfrmEditLibrary::form=0;
AnsiString TfrmEditLibrary::m_LastSelection;

//---------------------------------------------------------------------------
__fastcall TfrmEditLibrary::TfrmEditLibrary(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ShowEditor()
{
	if (!form){
    	form = new TfrmEditLibrary(0);
		Scene.lock();
    }
    form->Show();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::HideEditor()
{
	if (form) form->Close();
}
//---------------------------------------------------------------------------
CEditableObject* __fastcall TfrmEditLibrary::RayPick(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf){
	if (!form) return 0;
	if (form->m_SelectedObject&&form->cbPreview->Checked){
    	float dist=flt_max;
    	form->m_SelectedObject->RayPick(dist,start,direction,precalc_identity,pinf);
        if (pinf&&pinf->e_mesh&&pinf->e_obj) TfrmPropertiesObject::OnPick(*pinf);
        return form->m_SelectedObject;
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::OnRender(){
	if (!form) return;
	if (form->m_SelectedObject&&form->cbPreview->Checked){
        form->m_SelectedObject->RTL_Update(Device.fTimeDelta);
        // update transform matrix
        Fmatrix	mTransform,mScale,mTranslate,mRotate;
        mRotate.setHPB			(form->m_SelectedObject->t_vRotate.y, form->m_SelectedObject->t_vRotate.x, form->m_SelectedObject->t_vRotate.z);
        mScale.scale			(form->m_SelectedObject->t_vScale);
        mTranslate.translate	(form->m_SelectedObject->t_vPosition);
        mTransform.mul			(mTranslate,mRotate);
        mTransform.mul			(mScale);
        form->m_SelectedObject->RenderSingle(mTransform);
        if (fraBottomBar->miDrawObjectBones->Checked) form->m_SelectedObject->RenderBones(mTransform);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ZoomObject(){
	if (!form) return;
	if (form->m_SelectedObject&&form->cbPreview->Checked){
		Fbox& bb = form->m_SelectedObject->GetBox();
        Device.m_Camera.ZoomExtents(bb);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormShow(TObject *Sender)
{
    InitObjects();
    ebSave->Enabled = false;
    UI.BeginEState(esEditLibrary);
    m_SelectedObject = 0;
    // add directional light
    Flight L;
    ZeroMemory(&L,sizeof(Flight));
    L.type = D3DLIGHT_DIRECTIONAL;
    L.diffuse.set(1,1,1,1);
    L.direction.set(1,-1,1); L.direction.normalize();
	Device.SetLight(0,L);
	Device.LightEnable(0,true);
    L.diffuse.set(0.5,0.5,0.5,1);
    L.direction.set(1,-1,-1); L.direction.normalize();
	Device.SetLight(1,L);
	Device.LightEnable(1,true);

    if (!m_LastSelection.IsEmpty()){
    	TElTreeItem *node=FOLDER::FindObject(tvObjects,m_LastSelection.c_str());
	    if (node){
    	    tvObjects->Selected = node;
        	tvObjects->EnsureVisible(node);
	    }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormClose(TObject *Sender, TCloseAction &Action)
{
	form = 0;
	_DELETE(m_Thm);
	Action = caFree;

    TfrmPropertiesObject::HideProperties();
	Scene.unlock();
//    Lib.ResetAnimation();
    UI.EndEState(esEditLibrary);
    UI.Command(COMMAND_CLEAR);
    // remove directional light
	Device.LightEnable(0,false);
	Device.LightEnable(1,false);
}
//---------------------------------------------------------------------------
bool TfrmEditLibrary::CloseEditLibrary(bool bReload){
    if (ebSave->Enabled){
    	int res = ELog.DlgMsg(mtConfirmation, "Library was change. Do you want save?");
		if (res==mrCancel) return false;
		if (res==mrYes) ebSaveClick(0);
    }
    if (bReload){
        UI.SetStatus("Objects reloading...");
        UI.Command(COMMAND_RELOAD_OBJECTS);
        UI.SetStatus("");
    }
    Close();
    return true;
}
//---------------------------------------------------------------------------
bool TfrmEditLibrary::FinalClose(){
	if (!form) return true;
	return form->CloseEditLibrary(false);
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::OnModified(){
	if (!form) return;
    form->ebSave->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvObjectsItemFocused(TObject *Sender)
{
    TElTreeItem* node = tvObjects->Selected;

	_DELETE(m_Thm);
    ebMakeThm->Enabled = false;
    if (node&&FOLDER::IsObject(node)&&UI.ContainEState(esEditLibrary)){
        // change thm
        AnsiString nm,obj_fn,thm_fn;
        FOLDER::MakeName		(node,0,nm,false);

        obj_fn					= ChangeFileExt(nm,".object");
        thm_fn					= ChangeFileExt(nm,".thm");
        FS.m_Objects.Update(obj_fn);
        FS.m_Objects.Update(thm_fn);
        if (FS.Exist(thm_fn.c_str())){
        	// если версии совпадают
            int obj_age 		= FS.GetFileAge(obj_fn);
            int thm_age 		= FS.GetFileAge(thm_fn);
            if (obj_age&&(obj_age==thm_age)){
	            m_Thm 			= new EImageThumbnail(nm.c_str(),EImageThumbnail::EITObject);
            }else{
            	ELog.Msg(mtError,"Update object thumbnail. Stale data.");
            }
        }

        if (cbPreview->Checked||TfrmPropertiesObject::Visible()){
        	Lib.RemoveEditObject(m_SelectedObject);
            m_SelectedObject = Lib.CreateEditObject(nm.c_str());
            R_ASSERT(m_SelectedObject);
            ZoomObject();
		    ebMakeThm->Enabled = true;
        }
        m_LastSelection = nm;
    }
    if (m_Thm&&m_Thm->Valid())	pbImagePaint(Sender);
    else                        pbImage->Repaint();
	TfrmPropertiesObject::SetCurrent(m_SelectedObject);

    UI.RedrawScene();
}

//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::cbPreviewClick(TObject *Sender)
{
    TElTreeItem *node = tvObjects->Selected;
    ebMakeThm->Enabled = false;
    if (cbPreview->Checked&&node&&FOLDER::IsObject(node)){
	    AnsiString name;
    	FOLDER::MakeName(node,0,name,false);
		Lib.RemoveEditObject(m_SelectedObject);
	    m_SelectedObject = Lib.CreateEditObject(name.c_str());
    	R_ASSERT(m_SelectedObject);
	    ebMakeThm->Enabled = true;
        ZoomObject();
    }
    UI.RedrawScene();
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::InitObjects()
{
	tvObjects->IsUpdating		= true;
    tvObjects->Items->Clear();
    FileMap& lst = Lib.Objects();
    FilePairIt it=lst.begin();
    FilePairIt _E=lst.end();   
    for(; it!=_E; it++)
        FOLDER::AppendObject(tvObjects,it->first.c_str());
	tvObjects->IsUpdating		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) 	ebCancel->Click();
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLibrary::ebPropertiesClick(TObject *Sender)
{
	TElTreeItem* node = tvObjects->Selected;
    if (FOLDER::IsObject(node)){
	    AnsiString name;
    	FOLDER::MakeName(node,0,name,false);
		Lib.RemoveEditObject(m_SelectedObject);
	    m_SelectedObject = Lib.CreateEditObject(name.c_str());
    	R_ASSERT(m_SelectedObject);
        TfrmPropertiesObject::SetCurrent(m_SelectedObject);
        TfrmPropertiesObject::ShowProperties();
    }else{
        ELog.DlgMsg(mtInformation,"Select object to edit.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebSaveClick(TObject *Sender)
{
	ebSave->Enabled = false;
    Lib.Save();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebCancelClick(TObject *Sender)
{
    CloseEditLibrary(ebSave->Enabled);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvObjectsDblClick(TObject *Sender)
{
	ebPropertiesClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebMakeThmClick(TObject *Sender)
{
	DWORDVec pixels;
    DWORD w,h;
    int src_age = 0;
	if (tvObjects->Selected&&FOLDER::IsObject(tvObjects->Selected)){
    	AnsiString name; FOLDER::MakeName(tvObjects->Selected,0,name,false);
        int age;
   	    CEditableObject* obj = Lib.CreateEditObject(name.c_str(),&age);
    	if (obj&&cbPreview->Checked){
            AnsiString obj_name, tex_name;
            obj_name = ChangeFileExt(obj->GetName(),".object");
            tex_name = ChangeFileExt(obj_name,".thm");
            FS.m_Objects.Update(obj_name);
            src_age = age;
            if (Device.MakeScreenshot(pixels,w,h)){
	            EImageThumbnail tex(tex_name.c_str(),EImageThumbnail::EITObject,false);
    	        tex.CreateFromData(pixels.begin(),w,h);
        	    tex.Save(src_age);
                tvObjectsItemFocused(Sender);
            	ELog.Msg(mtInformation,"Thumbnail created.");
            }else{
	            ELog.DlgMsg(mtError,"Can't make screenshot.");
            }
	    }else{
            ELog.DlgMsg(mtError,"Can't create thumbnail. Set preview mode.");
        }
		Lib.RemoveEditObject(obj);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::RefreshSelected()
{
	if (form){
	    form->m_SelectedObject = 0;
		form->tvObjectsItemFocused(0);
    }
}
//---------------------------------------------------------------------------
extern bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails);
void __fastcall TfrmEditLibrary::tvObjectsKeyPress(TObject *Sender, char &Key){
	TElTreeItem* node = tvObjects->Items->LookForItemEx(tvObjects->Selected,-1,false,false,false,&Key,LookupFunc);
    if (!node) node = tvObjects->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
    if (node){
    	tvObjects->Selected = node;
		tvObjects->EnsureVisible(node);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::pbImagePaint(TObject *Sender)
{
    if (m_Thm) m_Thm->Draw(paImage,pbImage,true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebExportDOClick(TObject *Sender)
{
    TElTreeItem* node = tvObjects->Selected;
    if (node&&FOLDER::IsObject(node)){
    	AnsiString fn;
		if (FS.GetSaveName(&FS.m_GameDO,fn)){
		    AnsiString name;
    		FOLDER::MakeName(node,0,name,false);
			// make detail
    	    CDetail DO;
        	if (DO.Update(name.c_str())){
		        CFS_Memory F;
    		    DO.Export(F);
        		F.SaveTo(fn.c_str(),0);
            }
        }
    }else{
        ELog.DlgMsg(mtInformation,"Select object to export.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebImportClick(TObject *Sender)
{
    AnsiString open_nm, save_nm, nm;
    if (FS.GetOpenName(&FS.m_Import,open_nm,true)){
    	AStringVec lst;
        SequenceToList(lst,open_nm.c_str());
		bool bNeedUpdate=false;
		bool bNeedBreak=false;
        for (AStringIt it=lst.begin(); it!=lst.end(); it++){
            nm = *it;
            nm = nm.Delete(1,strlen(FS.m_Import.m_Path));
            CEditableObject* O = new CEditableObject(nm.c_str());
            if (O->Load(it->c_str())){
                O->m_ObjVer.f_age = FS.GetFileAge(*it);
                save_nm = ChangeFileExt(nm,".object");
                if (FS.GetSaveName(&FS.m_Objects,save_nm)){
                    O->SaveObject(save_nm.c_str());
                    FS.MarkFile(*it);
                    bNeedUpdate=true;
                }else bNeedBreak=true;
            }
            _DELETE(O);
            if (bNeedBreak) break;
        }
        if (bNeedUpdate){
			Lib.RefreshLibrary();
			InitObjects();
        }
    }
}
//---------------------------------------------------------------------------

