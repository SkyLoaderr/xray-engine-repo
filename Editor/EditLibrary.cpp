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
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "RXCtrls"
#pragma link "RxMenus"
#pragma link "Placemnt"
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
//    m_EditObject = 0;
//    m_SaveObject = 0;
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
SceneObject* __fastcall TfrmEditLibrary::RayPick(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf){
    CEditObject* O = m_SelectedObject->GetReference();
    if (O){
    	float dist=flt_max;
    	O->RayPick(dist,start,direction,precalc_identity,pinf);
        return O;
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::OnRender(){
	if (m_SelectedObject&&cbPreview->Checked){
    	CEditObject* O = m_SelectedObject->GetReference();
	    if (O){
        	O->RTL_Update(UI->Device.m_FrameDTime);
			O->RenderSingle(precalc_identity);
            if (fraBottomBar->miDrawObjectBones->Checked) O->RenderBones(O->GetTransform());
	    }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::OnIdle(){
	if (m_SelectedObject&&cbPreview->Checked){
    	CEditObject* O = m_SelectedObject->GetReference();
	    if (O&&frmPropertiesObject) frmPropertiesObject->OnIdle();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ZoomObject(){
	if (m_SelectedObject&&cbPreview->Checked){
        CEditObject* O = m_SelectedObject->GetReference();
        if (O){
            Fbox bb;
            O->GetBox(bb);
            UI->Device.m_Camera.ZoomExtents(bb);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormShow(TObject *Sender)
{
    InitObjectFolder();
    ebSave->Enabled = false;
    UI->BeginEState(esEditLibrary);
    m_SelectedObject = 0;
    // add directional light
    Flight L;
    ZeroMemory(&L,sizeof(Flight));
    L.type = D3DLIGHT_DIRECTIONAL;
    L.diffuse.set(1,1,1,1);
    L.direction.set(1,-1,1); L.direction.normalize();
	UI->Device.SetLight(0,L);
	UI->Device.LightEnable(0,true);
    L.diffuse.set(0.5,0.5,0.5,1);
    L.direction.set(1,-1,-1); L.direction.normalize();
	UI->Device.SetLight(1,L);
	UI->Device.LightEnable(1,true);
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
//    if (bEditObjectMode) ebCancelEdit->Click();
	Scene->unlock();
    Lib->ResetAnimation();
    Lib->m_Current = 0;
    UI->EndEState(esEditLibrary);
    UI->Command(COMMAND_CLEAR);
	frmEditLibrary = 0;
    // remove directional light
	UI->Device.LightEnable(0,false);
	UI->Device.LightEnable(1,false);
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::CloseEditLibrary(bool bReload){
    if (ebSave->Enabled){
    	int res = Log->DlgMsg(mtConfirmation, "Library was change. Do you want save?");
		if (res==mrCancel) return;
		if (res==mrYes)
            ebSaveClick(0);
        else{
            if (bReload){
		        UI->SetStatus("Library reloading...");
				UI->Command(COMMAND_RELOAD_LIBRARY);
		        UI->SetStatus("");
            }
        }
    }
    Close();
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::FinalClose(){
    if (ebSave->Enabled){
	    if (Log->DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Library was change. Do you want save?")==mrYes)
    	    ebSaveClick(0);
    }
    Close();
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::OnModified(){
    ebSave->Enabled = true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Folder routines
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::tvObjectsItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
	m_SelectedObject = 0;
    ebMakeThm->Enabled = false;
    if (cbPreview->Checked&&Item->Data&&UI->ContainEState(esEditLibrary)){
        m_SelectedObject = (CLibObject*)Item->Data;
        m_SelectedObject->Refresh();
	    ebMakeThm->Enabled = true;
    }
    UI->RedrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::cbPreviewClick(TObject *Sender)
{
    TElTreeItem *itm = tvObjects->Selected;
	m_SelectedObject = 0;
    ebMakeThm->Enabled = false;
    if (cbPreview->Checked&&itm&&itm->Data&&UI->ContainEState(esEditLibrary)){
        m_SelectedObject = (CLibObject*)itm->Data;
	    ebMakeThm->Enabled = true;
    }
    UI->RedrawScene();
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditLibrary::FindFolder(const char* s)
{
    for ( TElTreeItem* node = tvObjects->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditLibrary::FindObject(void *obj)
{
    for ( TElTreeItem* node = tvObjects->Items->GetFirstNode(); node; node = node->GetNext())
		if (node->Data==obj) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditLibrary::AddFolder(const char* s)
{
    TElTreeItem* node = 0;
    if (s[0]!=0){
        node = tvObjects->Items->AddObject(0,s,0);
        node->ParentStyle = false;
        node->Bold = true;
    }
    return node;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditLibrary::AddObject(TElTreeItem* node, const char* name, void* obj)
{
    TElTreeItem* obj_node = tvObjects->Items->AddChildObject(node, name, obj);
    obj_node->ParentStyle = false;
    obj_node->Bold = false;
    return obj_node;
}
//---------------------------------------------------------------------------
void TfrmEditLibrary::InitObjectFolder()
{
	SendMessage(tvObjects->Handle,WM_SETREDRAW,0,0);
    tvObjects->Items->Clear();
    AddFolder(SKYDOME_FOLDER);
    AddFolder(DETAILOBJECT_FOLDER);
    for(LibObjIt it=Lib->FirstObj(); it!=Lib->LastObj(); it++){
        CLibObject* _O = *it;
        TElTreeItem* node = FindFolder(_O->GetFolderName());
        if (!node) node = AddFolder(_O->GetFolderName());
        AddObject(node,(*it)->GetRefName(),(void*)(*it));
    }
	SendMessage(tvObjects->Handle,WM_SETREDRAW,1,0);
	tvObjects->Repaint();
//    tvObjects->FullExpand();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::tvObjectsTryEdit(TObject *Sender,
      TElTreeItem *Item, TElHeaderSection *Section, TFieldTypes &CellType,
      bool &CanEdit){
    FEditNode = Item;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::tvObjectsItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
    if (FEditNode){
        for ( TElTreeItem* pNode = FEditNode->GetFirstChild(); pNode; pNode = FEditNode->GetNextChild(pNode))
            if (pNode->Data){
                CLibObject* _pT = (CLibObject*)pNode->Data;
                _pT->SetFolderName(FEditNode->Text);
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
	else if (Key==VK_DELETE)ebDeleteObjectClick(Sender);
	else if (Key==VK_INSERT)ebLoadObjectClick(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLibrary::ebPropertiesClick(TObject *Sender)
{
    CLibObject* _pT;
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode && pNode->Data){
        _pT = (CLibObject*) pNode->Data;
        if (_pT){
	        _pT->Refresh();
            ObjectList objset;
            CEditObject* O = _pT->GetReference();
            if (!O){
            	Log->DlgMsg(mtError, "Object '%s.object' can't found in \\Meshes directory.", _pT->GetRefName());
                return;
            }
			objset.push_back(O);
            bool bChange;
            if (frmPropertiesObjectRun(&objset,bChange)==mrOk){
            	_pT->m_bNeedSave = true;
                OnModified();
				_pT->m_RefName = O->GetName();
                TElTreeItem* pObjectNode = FindObject(_pT);
                if (pObjectNode) pObjectNode->Text = _pT->GetRefName();
            }else{
            	// если были изменения и нажата отмена - перегрузить объект
            	if (bChange) _pT->UnloadObject();
            }
        }else{
            Log->DlgMsg(mtInformation,"Select object to edit.");
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebSaveClick(TObject *Sender)
{
    if (!Lib->Validate())
        Log->DlgMsg(mtError,"Validation failed! Incorrect Library!");
    else{
        ebSave->Enabled = false;
        Lib->SaveLibrary();
//        CEditObject* _pT = 0;
//        TElTreeItem* pNode = tvObjects->Selected;
//        if (pNode && pNode->Data) _pT = (CEditObject*) pNode->Data;
//        InitObjectFolder(_pT);
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
    	Log->Msg( mtInformation, "EditLib: loading %s...", _FileName );
        char name[1024];
        char ext[32];
        _splitpath( _FileName, 0, 0, name, ext );
        CLibObject* obj = new CLibObject();
		obj->m_RefName = name;
        if (obj->ImportFrom(_FileName)){
            obj->m_bNeedSave = true;
            // append to library
            Lib->AddObject(obj);
			// generate object name
        	CEditObject* O = obj->GetReference();
            char obj_name[MAX_OBJ_NAME];
            Lib->GenerateObjectName(obj_name,O->GetName(),obj);
        	obj->SetRefName(AnsiString(obj_name));
            strcpy(O->GetName(),obj_name);
            // find last folder
            TElTreeItem* node = tvObjects->Selected;
            TElTreeItem* pParentNode = 0;
            if (node){
                pParentNode = (node->Parent)?node->Parent:node;
                if (!pParentNode->Data){
                    obj->SetFolderName(pParentNode->Text);
                }else{
                    pParentNode=0;
                }
            }
		    tvObjects->Selected = AddObject(pParentNode, obj->GetRefName(), (void*)obj);
            ebPropertiesClick(Sender);
            // init folders
            OnModified();
            obj->SaveObject();

			FS.MarkFile(_FileName);
    	}else{
        	_DELETE(obj);
        }
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebDeleteObjectClick(TObject *Sender)
{
    TElTreeItem* pNode = tvObjects->Selected;
    TElTreeItem* pPrevNode = pNode->GetPrevVisible();
    if (pNode){
    	if (pNode->Data){
            CLibObject* _pT = (CLibObject*)pNode->Data;
            if (_pT)
                if (Log->DlgMsg(mtConfirmation, "Delete selected item?") == mrYes){
                    Lib->RemoveObject( _pT );
                    _DELETE(_pT);
                    OnModified();
                    pNode->Delete();
                    tvObjects->Selected = pPrevNode;
			    	frmEditLibrary->SetFocus();
                    cbPreviewClick(Sender);
                }
        }else{
        	miDeleteFolderClick(Sender);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::ebReloadObjectClick(TObject *Sender)
{
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode){
    	if (pNode->Data){
            CLibObject* obj = (CLibObject*)pNode->Data;
            if (obj){
				AnsiString fn = obj->GetFileName();
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
                    	Log->DlgMsg(mtInformation,"Reload successful.");

                        FS.MarkFile(fn);
                    }else{
                    	Log->DlgMsg(mtInformation,"Reload failed.");
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
    TElTreeItem* pNode = AddFolder("New folder");
    pNode->EditText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::miEditFolderClick(TObject *Sender)
{
    if (tvObjects->Selected&&!tvObjects->Selected->Data) tvObjects->Selected->EditText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::miDeleteFolderClick(TObject *Sender)
{
    if (tvObjects->Selected&&!tvObjects->Selected->Data){
		if (Log->DlgMsg(mtConfirmation, "Delete selected folder and contents?") == mrYes){
		    TElTreeItem* fld=tvObjects->Selected;
            for ( TElTreeItem* pNode = fld->GetFirstChild(); pNode; pNode = fld->GetNextChild(pNode))
                if (pNode->Data){
                    CLibObject* _pT = (CLibObject*)pNode->Data;
	                Lib->RemoveObject( _pT );
	                _DELETE(_pT);
                    OnModified();
                }
	     	fld->Delete();
			frmEditLibrary->SetFocus();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvObjectsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
    TElTreeItem* node;
    node = tvObjects->GetItemAtY(Y);
    if (node&&(!node->Data)&&(!node->IsUnder(FDragItem)))FDragItem->MoveTo(node);
    else if (node&&(!node->IsUnder(FDragItem)))          FDragItem->MoveTo(node->Parent);
    if (FDragItem&&FDragItem->Data){
        OnModified();
        node=(node->Parent)?node->Parent:node;
        ((CLibObject*)(FDragItem->Data))->SetFolderName(node->Text);
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
void __fastcall TfrmEditLibrary::ebSaveObjectOGFClick(TObject *Sender)
{
    CEditObject* _pT = 0;
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode && pNode->Data) _pT = ((CLibObject*)pNode->Data)->GetReference();
    if (_pT){
    	if (!_pT->IsDynamic()){
        	Log->DlgMsg(mtInformation, "Export only dynamic object!");
            return;
        }
        char buf[MAX_PATH];
        strcpy(buf,_pT->GetName());
        if (FS.GetSaveName(&FS.m_GameMeshes,buf)){
            if (!Builder->SaveObjectOGF(buf,_pT)){
                Log->DlgMsg(mtInformation, "Can't save object '%s'.", _pT->GetName());
            }else{
                Log->DlgMsg(mtInformation, "Object '%s' export successfully.", _pT->GetName());
            }
        }
    }else{
        Log->DlgMsg(mtInformation, "Select object before save.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebSaveObjectDOClick(TObject *Sender)
{
    CEditObject* _pT = 0;
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode && pNode->Data) _pT = ((CLibObject*)pNode->Data)->GetReference();
    if (_pT){
    	if (!_pT->IsDynamic()){
        	Log->DlgMsg(mtInformation, "Export only dynamic object!");
            return;
        }
        char buf[MAX_PATH];
        strcpy(buf,_pT->GetName());
        if (FS.GetSaveName(&FS.m_GameDO,buf)){
            if (!Builder->SaveObjectDO(buf,_pT)){
                Log->DlgMsg(mtInformation, "Can't save object '%s'.", _pT->GetName());
            }else{
                Log->DlgMsg(mtInformation, "Object '%s' export successfully.", _pT->GetName());
            }
        }
    }else{
        Log->DlgMsg(mtInformation, "Select object before save.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebSaveObjectSkeletonOGFClick(TObject *Sender){
    CEditObject* _pT = 0;
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode && pNode->Data) _pT = ((CLibObject*)pNode->Data)->GetReference();
    if (_pT){
    	if (!_pT->IsDynamic()){
        	Log->DlgMsg(mtInformation, "Export only dynamic object!");
            return;
        }
        AnsiString buf = AnsiString(_pT->GetName());
        if (FS.GetSaveName(&FS.m_GameMeshes,buf)){
            if (!Builder->SaveObjectSkeletonOGF(buf.c_str(),_pT)){
                Log->DlgMsg(mtInformation, "Can't save object '%s'.", _pT->GetName());
            }else{
            	buf = ChangeFileExt(buf,".ltx");
            	Builder->SaveObjectSkeletonLTX(buf.c_str(),_pT);
                Log->DlgMsg(mtInformation, "Object '%s' export successfully.", _pT->GetName());
            }
        }
    }else{
        Log->DlgMsg(mtInformation, "Select object before save.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebSaveObjectVCFClick(TObject *Sender)
{
    CEditObject* _pT = 0;
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode && pNode->Data) _pT = ((CLibObject*) pNode->Data)->GetReference();
    if (_pT){
    	if (!_pT->IsDynamic()){
        	Log->DlgMsg(mtInformation, "Export only dynamic object!");
            return;
        }
        char buf[MAX_PATH];
        strcpy(buf,_pT->GetName());
        if (FS.GetSaveName(&FS.m_GameCForms,buf)){
            if (!Builder->SaveObjectVCF(buf,_pT)){
                Log->DlgMsg(mtInformation, "Can't save object '%s'.", _pT->GetName());
            }else{
                Log->DlgMsg(mtInformation, "Object '%s' export successfully.", _pT->GetName());
            }
        }
    }else{
        Log->DlgMsg(mtInformation, "Select object before save.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebMakeThmClick(TObject *Sender)
{
	DWORDVec pixels;
    DWORD w,h;
    int src_age = 0;
	if (tvObjects->Selected&&tvObjects->Selected->Data){
        CLibObject* LO = (CLibObject*)tvObjects->Selected->Data;
    	if (LO->IsLoaded()&&cbPreview->Checked){
            AnsiString obj_name, tex_name;
            obj_name = AnsiString(LO->GetRefName())+AnsiString(".object");
            tex_name = "$O_"+AnsiString(LO->GetRefName());
            FS.m_Objects.Update(obj_name);
            src_age = FS.GetFileAge(obj_name);
            if (UI->Device.MakeScreenshot(pixels,w,h)){
	            ETextureThumbnail tex(tex_name.c_str());
    	        tex.CreateFromData(pixels,w,h,src_age,false,false);
        	    tex.Save(src_age);
            	Log->DlgMsg(mtInformation,"Thumbnail created.");
            }else{
	            Log->DlgMsg(mtError,"Can't make screenshot.");
            }
	    }else{
            Log->DlgMsg(mtError,"Can't create thumbnail. Please, set preview mode.");
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::ebUnloadClick(TObject *Sender)
{
	if (ebSave->Enabled) ebSaveClick(Sender);
	UI->Command(COMMAND_UNLOAD_LIBMESHES);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::pcItemTypeClick(TObject *Sender)
{
	InitObjectFolder();
}
//---------------------------------------------------------------------------

extern bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails);

void __fastcall TfrmEditLibrary::tvObjectsKeyPress(TObject *Sender,
      char &Key)
{
	TElTreeItem* node = tvObjects->Items->LookForItemEx(tvObjects->Selected,-1,false,false,false,&Key,LookupFunc);
    if (!node) node = tvObjects->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
    if (node){
    	tvObjects->Selected = node;
		tvObjects->EnsureVisible(node);
    }
}
//---------------------------------------------------------------------------


