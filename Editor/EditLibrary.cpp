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

//---------------------------------------------------------------------------
__fastcall TfrmEditLibrary::TfrmEditLibrary(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ShowEditor()
{
	if (!form){
    	form = new TfrmEditLibrary(0);
		Scene->lock();
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
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormClose(TObject *Sender, TCloseAction &Action)
{
	form = 0;
	_DELETE(m_Thm);
	Action = caFree;

    TfrmPropertiesObject::HideProperties();
	Scene->unlock();
//    Lib->ResetAnimation();
    Lib->SetCurrentObject("");
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
        UI.SetStatus("Library reloading...");
        UI.Command(COMMAND_RELOAD_LIBRARY);
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
        AnsiString nm,fn;
        FOLDER::MakeName		(node,0,nm,false);
        fn						= ChangeFileExt(nm,".thm");
        FS.m_Objects.Update(fn);
        if (FS.Exist(fn.c_str())){
            m_Thm 				= new EImageThumbnail(nm.c_str(),EImageThumbnail::EITObject);
        }

        if (cbPreview->Checked||TfrmPropertiesObject::Visible()){
            m_SelectedObject = Lib->GetEditObject(nm.c_str());
            R_ASSERT(m_SelectedObject);
            ZoomObject();
		    ebMakeThm->Enabled = true;
        }
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
	    m_SelectedObject = Lib->GetEditObject(name.c_str());
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
//    FOLDER::AppendFolder(tvObjects,SKYDOME_FOLDER);
//    FOLDER::AppendFolder(tvObjects,DETAILOBJECT_FOLDER);
    AStringVec& lst = Lib->Objects();
    AnsiString nm;
    for(AStringIt it=lst.begin(); it!=lst.end(); it++){
        nm = ChangeFileExt(*it,"");
        FOLDER::AppendObject(tvObjects,nm.c_str());
    }
	tvObjects->IsUpdating		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) 	ebCancel->Click();
	else if (Key==VK_DELETE)ebDeleteItemClick(Sender);
	else if (Key==VK_INSERT)ebLoadObjectClick(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLibrary::ebPropertiesClick(TObject *Sender)
{
	TElTreeItem* node = tvObjects->Selected;
    if (FOLDER::IsObject(node)){
	    AnsiString name;
    	FOLDER::MakeName(node,0,name,false);
	    m_SelectedObject = Lib->GetEditObject(name.c_str());
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
    Lib->Save();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebLoadObjectClick(TObject *Sender)
{
/*
	int handle;
	char _FileName[MAX_PATH]="";
    if( FS.GetOpenName( &FS.m_Import, _FileName ) ){
    	VERIFY( _FileName );
    	ELog.Msg( mtInformation, "EditLib: loading %s...", _FileName );
        char name[1024];
        char ext[32];
        _splitpath( _FileName, 0, 0, name, ext );
        CLibObject* LO = new CLibObject();
		LO->SetName(name);
        if (LO->ImportFrom(_FileName)){
            LO->m_bNeedSave = true;
            // append to library
            Lib->AddObject(LO);
			// generate object name
        	CEditableObject* O = LO->GetReference();
            char obj_name[MAX_OBJ_NAME];
            Lib->GenerateObjectName(obj_name,O->GetName(),LO);
        	LO->SetName(obj_name);
            // find last folder
            TElTreeItem* node = tvObjects->Selected;
            AnsiString folder;
            FOLDER::MakeName(node,0,folder,true);
			LO->SetName(folder+AnsiString("\\")+obj_name);
		    tvObjects->Selected = FOLDER::AppendObject(tvObjects,LO->GetName());
            ebPropertiesClick(Sender);
            // init folders
            OnModified();
            LO->SaveObject();

			FS.MarkFile(_FileName);
    	}else{
        	_DELETE(LO);
        }
	}
*/
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebDeleteItemClick(TObject *Sender)
{
/*
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode){
		AnsiString full_name;
    	if (FOLDER::IsFolder(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected folder?") == mrYes){
		        for (TElTreeItem* item=pNode->GetFirstChild(); item&&(item->Level>pNode->Level); item=item->GetNext()){
                    FOLDER::MakeName(item,0,full_name,false);
                	if (FOLDER::IsObject(item))
                    	Lib->RemoveObject(full_name.c_str());
                }
	            pNode->Delete();
				OnModified();
        	}
        }
    	if (FOLDER::IsObject(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected object?") == mrYes){
				FOLDER::MakeName(pNode,0,full_name,false);
				Lib->RemoveObject(full_name.c_str());
	            pNode->Delete();
				OnModified();
        	}
        }
    }else{
		ELog.DlgMsg(mtInformation, "At first selected item.");
    }
*/
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebReloadObjectClick(TObject *Sender)
{
/*
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode){
    	if (FOLDER::IsObject(pNode)){
	    	AnsiString name; FOLDER::MakeName(pNode,0,name,false);
    	    CLibObject* obj = Lib->SearchObject(name.c_str());
            if (obj){
				AnsiString fn = obj->GetSrcName();
			    if( FS.GetOpenName( &FS.m_Import, fn ) ){
                	// save old params
                    bool bDynamic  = obj->GetReference()->IsDynamic();
                    AnsiString ltx = obj->GetReference()->GetClassScript();
                    // reload file
                	if (obj->ImportFrom(fn.c_str())){
                    	// restore params
	                    obj->GetReference()->SetDynamic(bDynamic);
    	                obj->GetReference()->GetClassScript() = ltx;

			            obj->m_bNeedSave = true;
                    	ELog.DlgMsg(mtInformation,"Reload successful.");

                        FS.MarkFile(fn);
                    }else{
                    	ELog.DlgMsg(mtInformation,"Reload failed.");
                    }
		            OnModified();
                }
    		}
        }
    }
*/
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebCancelClick(TObject *Sender)
{
    CloseEditLibrary(true);
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
   	    CEditableObject* obj = Lib->GetEditObject(name.c_str());
    	if (obj&&cbPreview->Checked){
            AnsiString obj_name, tex_name;
            obj_name = ChangeFileExt(obj->GetName(),".object");
            tex_name = ChangeFileExt(obj_name,".thm");
            FS.m_Objects.Update(obj_name);
            src_age = FS.GetFileAge(obj_name);
            if (Device.MakeScreenshot(pixels,w,h)){
	            EImageThumbnail tex(tex_name.c_str(),EImageThumbnail::EITObject,false);
    	        tex.CreateFromData(pixels.begin(),w,h);
        	    tex.Save(src_age);
                tvObjectsItemFocused(Sender);
            	ELog.DlgMsg(mtInformation,"Thumbnail created.");
            }else{
	            ELog.DlgMsg(mtError,"Can't make screenshot.");
            }
	    }else{
            ELog.DlgMsg(mtError,"Can't create thumbnail. Set preview mode.");
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebUnloadClick(TObject *Sender)
{
	if (ebSave->Enabled) ebSaveClick(Sender);
	UI.Command(COMMAND_UNLOAD_LIBMESHES);
    RefreshSelected();
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
    AnsiString fn, nm;
    if (FS.GetOpenName(&FS.m_Import,fn)){
    	nm = fn;
        nm = nm.Delete(1,strlen(FS.m_Import.m_Path));
	    CEditableObject* O = new CEditableObject(nm.c_str());
        if (O->Load(fn.c_str())){
            O->m_ObjVer.f_age = FS.GetFileAge(AnsiString(fn));
			fn = ChangeFileExt(nm,".object");
		    if (FS.GetSaveName(&FS.m_Objects,fn)){
            	O->SaveObject(fn.c_str());
                Lib->ReloadLibrary();
                InitObjects();
            }
        }
		_DELETE(O);
    }
}
//---------------------------------------------------------------------------

