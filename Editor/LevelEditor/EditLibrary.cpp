//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditLibrary.h"
#include "Library.h"
#include "UI_Main.h"
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
#include "SceneObject.h"
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
FileMap TfrmEditLibrary::modif_map;
bool TfrmEditLibrary::bFinalExit = false;
bool TfrmEditLibrary::bExitResult= true;

//---------------------------------------------------------------------------
__fastcall TfrmEditLibrary::TfrmEditLibrary(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
	m_pEditObject = new CSceneObject();
    m_Props = TfrmPropertiesEObject::CreateProperties();
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
CSceneObject* __fastcall TfrmEditLibrary::RayPick(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf){
	if (!form) return 0;
	if (form->cbPreview->Checked){
    	float dist=flt_max;
        if (form->m_pEditObject->RayPick(dist,start,direction,pinf)){
        	R_ASSERT(pinf&&pinf->e_mesh&&pinf->e_obj);
//S        	TfrmPropertiesObject::OnPick(*pinf);
			pinf->s_obj=form->m_pEditObject;
	        return form->m_pEditObject;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::OnRender(){
	if (!form) return;
	if (form->cbPreview->Checked){
        form->m_pEditObject->OnFrame();
        form->m_pEditObject->RenderSingle();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ZoomObject(){
	if (!form) return;
	if (form->cbPreview->Checked){
        // get bbox&transform
		Fbox bb;
        if (form->m_pEditObject->GetBox(bb))
    	    Device.m_Camera.ZoomExtents(bb);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormShow(TObject *Sender)
{
	modif_map.clear();
    
    InitObjects();
    ebSave->Enabled = false;
    UI.BeginEState(esEditLibrary);
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
    if (!bFinalExit&&ebSave->Enabled){
	    bFinalExit = false;
        UI.SetStatus("Objects reloading...");
        FilePairIt it=modif_map.begin();
		FilePairIt _E=modif_map.end();
        for (;it!=_E;it++)
        	Lib.ReloadObject(it->first.c_str());
        UI.SetStatus("");
    }
    _DELETE(form->m_pEditObject);

    TfrmPropertiesEObject::DestroyProperties(m_Props);

	form = 0;
	_DELETE(m_Thm);
	Action = caFree;

	Scene.unlock();

    UI.EndEState(esEditLibrary);
    UI.Command(COMMAND_CLEAR);
    // remove directional light
	Device.LightEnable(0,false);
	Device.LightEnable(1,false);
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
    CanClose = true;
    if (ebSave->Enabled){
    	int res = ELog.DlgMsg(mtConfirmation, "Library was change. Do you want save?");
		if (res==mrCancel) CanClose = false;
		if (res==mrYes) ebSaveClick(0);
    }
    bExitResult = CanClose;
}
//---------------------------------------------------------------------------
bool TfrmEditLibrary::FinalClose(){
	if (!form) return true;
    bFinalExit = true;
    form->Close();
    return bExitResult;
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::OnModified(){
	if (!form) return;
    form->ebSave->Enabled = true;
    if (form->m_pEditObject->GetReference()) modif_map.insert(make_pair(form->m_pEditObject->GetRefName(),0));
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
        Engine.FS.m_Objects.Update(obj_fn);
        Engine.FS.m_Objects.Update(thm_fn);
        if (Engine.FS.Exist(thm_fn.c_str())){
        	// если версии совпадают
            int obj_age 		= Engine.FS.GetFileAge(obj_fn);
            int thm_age 		= Engine.FS.GetFileAge(thm_fn);
            if (obj_age&&(obj_age==thm_age)){
	            m_Thm 			= new EImageThumbnail(nm.c_str(),EImageThumbnail::EITObject);
            }else{
            	ELog.Msg(mtError,"Update object thumbnail. Stale data.");
            }
        }

        if (cbPreview->Checked||m_Props->Visible){
        	if (m_Props->IsModified()&&m_pEditObject->GetReference())
            	modif_map.insert(make_pair(m_pEditObject->GetRefName(),0));
            m_pEditObject->SetReference(nm.c_str());
//            ZoomObject();
		    ebMakeThm->Enabled = true;
        }

        m_LastSelection = nm;
    }
    if (m_Thm&&m_Thm->Valid())	pbImagePaint(Sender);
    else                        pbImage->Repaint();
    UpdateObjectProperties();
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
		m_pEditObject->SetReference(name.c_str());
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
    if (node&&FOLDER::IsObject(node)){
	    AnsiString name;
    	FOLDER::MakeName(node,0,name,false);
		m_pEditObject->SetReference(name.c_str());
        UpdateObjectProperties();
        m_Props->ShowProperties();
    }else{
        ELog.DlgMsg(mtInformation,"Select object to edit.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebSaveClick(TObject *Sender)
{
	ebSave->Enabled = false;
    Lib.Save();
	form->tvObjectsItemFocused(0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebCancelClick(TObject *Sender)
{
    Close();
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
            Engine.FS.m_Objects.Update(obj_name);
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

void __fastcall TfrmEditLibrary::ResetSelected()
{
//S
/*
	if (form){
		Lib.RemoveEditObject(form->m_SelectedObject);
	    TfrmPropertiesObject::HideProperties();
    }
*/
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::RefreshSelected()
{
//S
/*
	if (form){
		Lib.RemoveEditObject(form->m_SelectedObject);
		form->tvObjectsItemFocused(0);
    }
*/
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
		if (Engine.FS.GetSaveName(Engine.FS.m_GameDO,fn)){
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

void __fastcall TfrmEditLibrary::ebExportHOMClick(TObject *Sender)
{
    TElTreeItem* node = tvObjects->Selected;
    if (node&&FOLDER::IsObject(node)){
    	AnsiString name; FOLDER::MakeName(node,0,name,false);
        int age;
   	    CEditableObject* obj = Lib.CreateEditObject(name.c_str(),&age);
    	if (obj){
		    AnsiString save_nm;
        	if (Engine.FS.GetSaveName(Engine.FS.m_GameDO,save_nm)){
//------------------------------------------------------------------------------
				CFS_Memory FS;
                FS.open_chunk(0);
                FS.Wdword(0);
                FS.close_chunk();
                FS.open_chunk(1);
				EditMeshVec& m_lst=obj->Meshes();
				for (EditMeshIt m_it=m_lst.begin(); m_it!=m_lst.end(); m_it++){
					for (SurfFacesPairIt sf_it=(*m_it)->m_SurfFaces.begin(); sf_it!=(*m_it)->m_SurfFaces.end(); sf_it++){
                    	BOOL b2Sided = sf_it->first->_2Sided();
                        INTVec& i_lst= sf_it->second;
                        for (INTIt i_it=i_lst.begin(); i_it!=i_lst.end(); i_it++){
                        	st_Face& F = (*m_it)->m_Faces[*i_it];
                            for (int k=0; k<3; k++)
	                            FS.Wvector((*m_it)->m_Points[F.pv[k].pindex]);
                            FS.Wdword(b2Sided);
                        }
                    }
                }
                FS.close_chunk();
                save_nm = ChangeFileExt(save_nm,".hom");
                FS.SaveTo(save_nm.c_str(),0);
				ELog.DlgMsg(mtInformation, "Export complete.");

//------------------------------------------------------------------------------
//                if (!obj->ExportHOM()){
//                    ELog.DlgMsg(mtInformation, "Can't export object '%s'.", obj->GetName());
//                }
            }else{
	            ELog.DlgMsg(mtError,"Export canceled.");
            }
	    }else{
            ELog.DlgMsg(mtError,"Can't load object.");
        }
		Lib.RemoveEditObject(obj);
    }else{
        ELog.DlgMsg(mtInformation, "Select object to export.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebImportClick(TObject *Sender)
{
    AnsiString open_nm, save_nm, nm;
    if (Engine.FS.GetOpenName(Engine.FS.m_Import,open_nm,true)){
    	// remove selected object
        ResetSelected();
		// load
    	AStringVec lst;
        SequenceToList(lst,open_nm.c_str());
		bool bNeedUpdate=false;
		bool bNeedBreak=false;
		AnsiString path;
        for (AStringIt it=lst.begin(); it!=lst.end(); it++){
            nm = *it;
            nm = nm.Delete(1,strlen(Engine.FS.m_Import.m_Path));
            CEditableObject* O = new CEditableObject(nm.c_str());
            if (O->Load(it->c_str())){
                O->m_ObjVer.f_age = Engine.FS.GetFileAge(*it);
                save_nm = ChangeFileExt(nm,".object");
                if (Engine.FS.GetSaveName(Engine.FS.m_Objects,save_nm,path.c_str())){
                	path = ExtractFilePath(save_nm);
                    O->SaveObject(save_nm.c_str());
                    Engine.FS.MarkFile(*it);
                    bNeedUpdate=true;
                }else bNeedBreak=true;
            }else{
            	ELog.DlgMsg(mtError,"Can't load file '%s'.",it->c_str());
            }
            _DELETE(O);
            if (bNeedBreak) break;
        }
        if (bNeedUpdate){
			Lib.RefreshLibrary();
			InitObjects();
        }
        // refresh selected
		RefreshSelected();
    }
}
//---------------------------------------------------------------------------

void TfrmEditLibrary::UpdateObjectProperties()
{
	m_Props->UpdateProperties(m_pEditObject);
}



