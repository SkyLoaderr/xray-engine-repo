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
TfrmEditLibrary *frmEditLibrary=0;

//---------------------------------------------------------------------------
void frmEditLibraryEditLibrary(){
	if (frmEditLibrary){
    	frmEditLibrary->SetFocus();
    }else{
		frmEditLibrary = new TfrmEditLibrary(0);
    	frmEditLibrary->EditLibrary();
    }
}
//---------------------------------------------------------------------------
__fastcall TfrmEditLibrary::TfrmEditLibrary(TComponent* Owner)
    : TForm(Owner)
{
    m_SelectedObject = 0;
    FEditNode = 0;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::EditLibrary()
{
	Scene->lock();
    Show();
}
//---------------------------------------------------------------------------
CEditableObject* __fastcall TfrmEditLibrary::RayPick(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf){
	if (!m_SelectedObject) return 0;
    CEditableObject* O = m_SelectedObject->GetReference();
    if (O){
    	float dist=flt_max;
    	O->RayPick(dist,start,direction,precalc_identity,pinf);
        if (pinf) TfrmPropertiesObject::Pick(*pinf);
        return O;
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::OnRender(){
	if (m_SelectedObject&&cbPreview->Checked){
    	CEditableObject* O = m_SelectedObject->GetReference();
	    if (O){
        	O->RTL_Update(Device.fTimeDelta);
		    // update transform matrix
			Fmatrix	mTransform,mScale,mTranslate,mRotate;
			mRotate.setHPB			(O->t_vRotate.y, O->t_vRotate.x, O->t_vRotate.z);
			mScale.scale			(O->t_vScale);
			mTranslate.translate	(O->t_vPosition);
			mTransform.mul			(mTranslate,mRotate);
			mTransform.mul			(mScale);
			O->RenderSingle			(mTransform);
            if (fraBottomBar->miDrawObjectBones->Checked) O->RenderBones(mTransform);
	    }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ZoomObject(){
	if (m_SelectedObject&&cbPreview->Checked){
        CEditableObject* O = m_SelectedObject->GetReference();
        if (O){
            Fbox& bb = O->GetBox();
            Device.m_Camera.ZoomExtents(bb);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormShow(TObject *Sender)
{
    InitObjectFolder();
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
	Action = caFree;

    TfrmPropertiesObject::HideProperties();
	Scene->unlock();
    Lib->ResetAnimation();
    Lib->m_Current = 0;
    UI.EndEState(esEditLibrary);
    UI.Command(COMMAND_CLEAR);
	frmEditLibrary = 0;
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
	return CloseEditLibrary(false);
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::OnModified(){
    ebSave->Enabled = true;
    // test name changing
    if (m_SelectedObject){
    	TElTreeItem* node = FOLDER::FindObject(tvObjects,m_SelectedObject->m_Name.c_str());
        VERIFY(node);
		if (strcmp(m_SelectedObject->GetName(),AnsiString(node->Text).c_str())!=0)
        	node->Text = m_SelectedObject->GetName();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvObjectsItemFocused(TObject *Sender)
{
    CLibObject* new_selection=0;

    TElTreeItem* node = tvObjects->Selected;

    ebMakeThm->Enabled = false;
    if (FOLDER::IsObject(node)&&UI.ContainEState(esEditLibrary)){
    	AnsiString name; FOLDER::MakeName(node,0,name,false);
        new_selection = Lib->SearchObject(name.c_str());
        new_selection->Refresh();
	    ebMakeThm->Enabled = true;
    }
	TfrmPropertiesObject::SetCurrent(new_selection);
    m_SelectedObject = new_selection;
	ZoomObject();

    UI.RedrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::cbPreviewClick(TObject *Sender)
{
    TElTreeItem *itm = tvObjects->Selected;
	m_SelectedObject = 0;
    ebMakeThm->Enabled = false;
    if (cbPreview->Checked&&itm&&FOLDER::IsObject(itm)&&UI.ContainEState(esEditLibrary)){
    	AnsiString name; FOLDER::MakeName(itm,0,name,false);
        m_SelectedObject = Lib->SearchObject(name.c_str());
	    ebMakeThm->Enabled = true;
        ZoomObject();
    }
    UI.RedrawScene();
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::InitObjectFolder()
{
	tvObjects->IsUpdating		= true;
    tvObjects->Items->Clear();
    FOLDER::AppendFolder(tvObjects,SKYDOME_FOLDER);
    FOLDER::AppendFolder(tvObjects,DETAILOBJECT_FOLDER);
    for(LibObjIt it=Lib->FirstObj(); it!=Lib->LastObj(); it++){
        CLibObject* _O = *it;
        FOLDER::AppendObject(tvObjects,_O->GetName());
    }
	tvObjects->IsUpdating		= false;
//    tvObjects->FullExpand();
}
//---------------------------------------------------------------------------
//S
/*
void __fastcall TfrmEditLibrary::tvObjectsTryEdit(TObject *Sender,
      TElTreeItem *Item, TElHeaderSection *Section, TFieldTypes &CellType,
      bool &CanEdit){
    FEditNode = Item;
}
*/
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::tvObjectsItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
    if (FEditNode){
        for ( TElTreeItem* pNode = FEditNode->GetFirstChild(); pNode; pNode = FEditNode->GetNextChild(pNode))
            if (pNode->Data){
//S                CLibObject* _pT = (CLibObject*)pNode->D a t a???;
//S                _pT->SetFolderName(FEditNode->Text);
                OnModified();
            }
        FEditNode = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
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
    if (m_SelectedObject){
        m_SelectedObject->Refresh();
        TfrmPropertiesObject::SetCurrent(m_SelectedObject);
        TfrmPropertiesObject::ShowProperties();
    }else{
        ELog.DlgMsg(mtInformation,"Select object to edit.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebSaveClick(TObject *Sender)
{
    if (!Lib->Validate())
        ELog.DlgMsg(mtError,"Validation failed! Incorrect Library!");
    else{
        ebSave->Enabled = false;
        Lib->SaveLibrary();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebValidateClick(TObject *Sender)
{
    if (!Lib->Validate())   MessageDlg("Validation failed!", mtError, TMsgDlgButtons() << mbOK, 0);
    else                    MessageDlg("Validation OK!", mtInformation, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLibrary::ebLoadObjectClick(TObject *Sender)
{
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
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebDeleteItemClick(TObject *Sender)
{
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
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebReloadObjectClick(TObject *Sender)
{
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


void __fastcall TfrmEditLibrary::miNewFolderClick(TObject *Sender)
{
    TElTreeItem* pNode = FOLDER::AppendFolder(tvObjects,"New folder");
    pNode->EditText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::miEditFolderClick(TObject *Sender)
{
    if (tvObjects->Selected&&FOLDER::IsFolder(tvObjects->Selected)) tvObjects->Selected->EditText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvObjectsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
//S
    TElTreeItem* node;
    node = tvObjects->GetItemAtY(Y);
    if (node&&(!node->Data)&&(!node->IsUnder(FDragItem)))FDragItem->MoveTo(node);
    else if (node&&(!node->IsUnder(FDragItem)))          FDragItem->MoveTo(node->Parent);
    if (FDragItem&&FDragItem->Data){
        OnModified();
        node=(node->Parent)?node->Parent:node;
//S        ((CLibObject*)(FDragItem->Data))->SetFolderName(node->Text);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvObjectsDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    TElTreeItem* node;
    Accept = false;
    if (!FDragItem->Data) return;
    if (Source != tvObjects) return;
    node = tvObjects->GetItemAtY(Y);
    if (node&&(!node->IsUnder(FDragItem))) Accept = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvObjectsStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
    FDragItem = tvObjects->ItemFocused;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebMakeThmClick(TObject *Sender)
{
	DWORDVec pixels;
    DWORD w,h;
    int src_age = 0;
	if (tvObjects->Selected&&FOLDER::IsObject(tvObjects->Selected)){
    	AnsiString name; FOLDER::MakeName(tvObjects->Selected,0,name,false);
   	    CLibObject* LO = Lib->SearchObject(name.c_str());
    	if (LO->IsLoaded()&&cbPreview->Checked){
            AnsiString obj_name, tex_name;
            obj_name = AnsiString(LO->GetName())+AnsiString(".object");
            tex_name = ChangeFileExt(obj_name,".thm");
            FS.m_Objects.Update(obj_name);
            src_age = FS.GetFileAge(obj_name);
            if (Device.MakeScreenshot(pixels,w,h)){
	            EImageThumbnail tex(tex_name.c_str(),EImageThumbnail::EITObject,false);
    	        tex.CreateFromData(pixels.begin(),w,h);
        	    tex.Save(src_age);
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
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::pcItemTypeClick(TObject *Sender)
{
	InitObjectFolder();
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


