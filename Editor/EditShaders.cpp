//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditShaders.h"
#include "UI_Main.h"
#include "Shader.h"
#include "xrshader.h"
#include "library.h"
#include "EditObject.h"
#include "PropertiesShader.h"
#include "scene.h"
#include "main.h"
#include "EditMesh.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "RXCtrls"
#pragma link "RxMenus"
#pragma link "Placemnt"
#pragma resource "*.dfm"
TfrmEditShaders *frmEditShaders=0;
static AnsiString g_ExternalObject = "";
//---------------------------------------------------------------------------
void frmEditShadersEditShaders(){
	if (frmEditShaders){
    	frmEditShaders->SetFocus();
    }else{
		frmEditShaders = new TfrmEditShaders(0);
	    frmEditShaders->EditShaders();
    }
}

//---------------------------------------------------------------------------
__fastcall TfrmEditShaders::TfrmEditShaders(TComponent* Owner)
    : TForm(Owner)
{
    m_SelectedShader = 0;
    FEditNode = 0;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::EditShaders()
{
	init_cam_hpb.set(UI->Device.m_Camera.GetHPB());
	init_cam_pos.set(UI->Device.m_Camera.GetPosition());
    UI->Device.m_Camera.Set(0,0,0, 2,6,-11);
    // scene locking
	Scene->lock();
    Show();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::ZoomObject(){
    CEditObject* O = m_TestObject->GetReference();
    if (O){
    	Fbox bb;
        O->GetBox(bb);
        UI->Device.m_Camera.ZoomExtents(bb);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::OnRender(){
	if (rgTestObject->ItemIndex==4){
    	if (m_TestObject&&cbPreview->Checked){
            CEditObject* O = m_TestObject->GetReference();
            if (O) O->RenderSingle(precalc_identity);
        }
    }else{ 
    	if (m_SelectedShader&&m_TestObject&&cbPreview->Checked){
            CEditObject* O = m_TestObject->GetReference();
            if (O){
                st_Surface* surf = *O->FirstSurface();
                if (surf){
                    if (surf->shader){
                        if (strcmp(surf->shader->shader->cName,m_SelectedShader->cName)!=0){
                            UI->Device.Shader.Delete(surf->shader);
                            surf->shader = UI->Device.Shader.Create(m_SelectedShader->cName,surf->textures);
                        }
                    }else
                        surf->shader = UI->Device.Shader.Create(m_SelectedShader->cName,surf->textures);
                }
            	O->RenderSingle(precalc_identity);
            }
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::OnIdle(){
	if (frmPropertiesShader) frmPropertiesShader->OnIdle();
	if (rgTestObject->ItemIndex==4){
    	if (m_TestObject&&cbPreview->Checked){
            CEditObject* O = m_TestObject->GetReference();
            if (O&&ebDropper->Down){ 
                POINT pt,wpt;
                GetCursorPos( &pt );

                SHORT ks=GetAsyncKeyState(VK_LBUTTON);
                SHORT ls=GetAsyncKeyState(VK_LMENU);

                if (ks&0x0001){
	                TWinControl* ctr = FindVCLWindow(pt);
	                if (ctr!=frmMain->D3DWindow) return;
                    Fvector S, D;
                    Fvector2 CP;
                    pt = UI->GetD3DWindow()->ScreenToClient(pt);
                    CP.set(float(pt.x),float(pt.y));
                    UI->Device.m_Camera.MouseRayFromPoint(S, D, CP );

                    float dist=flt_max;
                    SRayPickInfo pinf;
                    if (O->RayPick(dist,S,D,precalc_identity,&pinf)){
						m_TestExternSurface=pinf.mesh->GetSurfaceByFaceID(pinf.rp_inf.id);
						if (!ls&0x0001) ebDropper->Down = false;
                        ebAssignShader->Enabled = true;
                        lbExternSelMat->Caption = m_TestExternSurface->name;
                        lbExternSelShader->Caption = m_TestExternSurface->shader->shader->cName;
                        tvShaders->Selected = tvShaders->Items->LookForItem(0,m_TestExternSurface->shader->shader->cName,0,0,false,true,false,false,true);
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::FormShow(TObject *Sender)
{
    InitShaderFolder();
    ebSave->Enabled = false;
    UI->BeginEState(esEditShaders);
    m_SelectedShader = 0;
	m_TestObject = 0;
    m_TestExternSurface = 0;
    gbExtern->Hide();
	lbExternSelMat->Caption = "...";
    lbExternSelShader->Caption = "...";
	ebAssignShader->Enabled = false;
    ebSaveExternObject->Enabled = false;
    ebDropper->Down = false;

    SHLib->Backup();

    rgTestObjectClick(Sender);

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
void __fastcall TfrmEditShaders::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
	frmEditShaders=0;
	UI->Device.m_Camera.Set(init_cam_hpb,init_cam_pos);
	Scene->unlock();
	if (m_TestObject) m_TestObject->UnloadObject();
	m_TestObject = 0;
    m_TestExternSurface = 0;
    UI->EndEState(esEditShaders);
    UI->Command(COMMAND_CLEAR);
    // remove directional light
	UI->Device.LightEnable(0,false);
	UI->Device.LightEnable(1,false);
}
//---------------------------------------------------------------------------
void TfrmEditShaders::CloseEditShaders(bool bReload){
    if (ebSave->Enabled){
    	int res = Log->DlgMsg(mtConfirmation, "Library was change. Do you want save?");
		if (res==mrCancel) return;
		if (res==mrYes)
            ebSaveClick(0);
        else{
            if (bReload){
		        UI->SetStatus("Library reloading...");
				SHLib->RestoreBackup();
		        UI->SetStatus("");
            }
        }
    }
    Close();
}
//---------------------------------------------------------------------------
void TfrmEditShaders::FinalClose(){
    if (ebSave->Enabled){
	    if (Log->DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Library was change. Do you want save?")==mrYes)
    	    ebSaveClick(0);
    }
    Close();
}
//---------------------------------------------------------------------------
void TfrmEditShaders::OnModified(){
    ebSave->Enabled = true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Folder routines
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::tvShadersItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
	m_SelectedShader = 0;
    if (cbPreview->Checked&&Item->Data&&UI->ContainEState(esEditShaders))
        m_SelectedShader = SHLib->FindShader(Item->Text);
    UI->RedrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::cbPreviewClick(TObject *Sender)
{
    TElTreeItem *itm = tvShaders->Selected;
    if (cbPreview->Checked&&itm&&itm->Data&&UI->ContainEState(esEditShaders))
        m_SelectedShader = SHLib->FindShader(itm->Text);
    UI->RedrawScene();
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditShaders::FindFolder(const char* s)
{
    for ( TElTreeItem* node = tvShaders->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditShaders::FindShader(const char* s)
{
    for ( TElTreeItem* node = tvShaders->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditShaders::AddFolder(const char* s)
{
    TElTreeItem* node = 0;
    if (s[0]!=0){
        node = tvShaders->Items->AddObject(0,s,0);
        node->ParentStyle = false;
        node->Bold = true;
    }
    return node;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditShaders::AddShader(TElTreeItem* node, const char* name)
{
    TElTreeItem* obj_node = tvShaders->Items->AddChildObject(node, name, (void*)1);
    obj_node->ParentStyle = false;
    obj_node->Bold = false;
    return obj_node;
}
//---------------------------------------------------------------------------
void TfrmEditShaders::InitShaderFolder(const char* nm)
{
	tvShaders->IsUpdating = true;
    tvShaders->Selected = 0;
    tvShaders->Items->Clear();
/*
    for (FolderPairIt fp_it=m_Folders.begin(); fp_it!=m_Folders.end(); fp_it++){
    	AddFolder(fp_it->first);
    }
*/
    for(ShaderIt it=SHLib->m_Shaders.begin(); it!=SHLib->m_Shaders.end(); it++)
		AddShader(0,it->cName);

    if (nm){
		tvShaders->Selected = tvShaders->Items->LookForItem(0,nm,0,0,false,true,false,false,true);
		m_SelectedShader = SHLib->FindShader(AnsiString(nm));
    }

	tvShaders->Sort(true);

	tvShaders->IsUpdating = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::tvShadersTryEdit(TObject *Sender,
      TElTreeItem *Item, TElHeaderSection *Section, TFieldTypes &CellType,
      bool &CanEdit){
    FEditNode = Item;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::tvShadersItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
    if (FEditNode){
        for ( TElTreeItem* pNode = FEditNode->GetFirstChild(); pNode; pNode = FEditNode->GetNextChild(pNode))
            if (pNode->Data){
//                SH_ShaderDef* _pT = (SH_ShaderDef*)pNode->Data;
//                _pT->SetFolderName(FEditNode->Text);

                OnModified();
            }
        FEditNode = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) 	ebCancel->Click();
    else if (Key==VK_DELETE)ebRemoveShader->Click();
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditShaders::ebPropertiesShaderClick(TObject *Sender)
{
    SH_ShaderDef* _pT;
    TElTreeItem* pNode = tvShaders->Selected;
    if (pNode && pNode->Data){
        _pT = SHLib->FindShader(pNode->Text);
        if (_pT){
            if (frmPropertiesShaderRun(_pT)==mrOk){
				tvShaders->Sort(true);
                OnModified();
            }
        }else{
            MessageDlg("Select shader to edit.", mtConfirmation, TMsgDlgButtons() << mbOK, 0);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::ebSaveClick(TObject *Sender)
{
    if (!SHLib->Validate())
        MessageDlg("Validation failed! Incorrect Library!", mtError, TMsgDlgButtons() << mbOK, 0);
    else{
        ebSave->Enabled = false;
        // folders
//        SHLib->m_Folders.clear();
//        AnsiString F,N;
//	    for ( TElTreeItem* node = tvShaders->Items->GetFirstNode(); node; node = node->GetNext())
//        	if (node->Data)
//            	if (node->Parent) SHLib->m_Folders[node->Parent->Text].push_back(node->Text);
        // save
        SHLib->Backup();
        SHLib->SaveLibrary();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::ebValidateClick(TObject *Sender)
{
    if (!SHLib->Validate()) MessageDlg("Validation failed!", mtError, TMsgDlgButtons() << mbOK, 0);
    else                    MessageDlg("Validation OK!", mtInformation, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditShaders::ebNewShaderClick(TObject *Sender)
{
    AnsiString name;
    SHLib->GenerateName(name);
	m_SelectedShader = SHLib->AddShader(name);
	SHLib->SaveLibrary();
	UI->Device.ReloadShaders();
    tvShaders->Selected = AddShader(0, name.c_str());
	tvShaders->Sort(true);
	ebPropertiesShaderClick(Sender);
    OnModified();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::ebCloneShaderClick(TObject *Sender)
{
    TElTreeItem* pNode = tvShaders->Selected;
    if (pNode){
        SH_ShaderDef* sh_src = SHLib->FindShader(pNode->Text); VERIFY(sh_src);
        if (sh_src){
            AnsiString name, pref;
            pref = sh_src->cName;
            SHLib->GenerateName(name,&pref);
            m_SelectedShader = SHLib->AddShader(name,sh_src);
            SHLib->SaveLibrary();
            UI->Device.ReloadShaders();
            tvShaders->Selected = AddShader(0, name.c_str());
			tvShaders->Sort(true);
            ebPropertiesShaderClick(Sender);
            OnModified();
        }
    }else{
		Log->DlgMsg(mtInformation, "At first selected item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::ebRemoveShaderClick(TObject *Sender)
{
    TElTreeItem* pNode = tvShaders->Selected;
    if (pNode){
    	if (pNode->Text=="null") return;
        if (MessageDlg("Delete selected item?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) == mrYes){
            SHLib->DeleteShader(pNode->Text);
	        SHLib->SaveLibrary();
	        UI->Device.ReloadShaders();
            pNode->Delete();
            cbPreviewClick(Sender);
            OnModified();
        }
    }else{
		Log->DlgMsg(mtInformation, "At first selected item.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditShaders::ebCancelClick(TObject *Sender)
{
    CloseEditShaders(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::tvShadersDblClick(TObject *Sender)
{
    ebPropertiesShaderClick(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditShaders::miNewFolderClick(TObject *Sender)
{
    TElTreeItem* pNode = AddFolder("New folder");
    pNode->EditText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::miEditFolderClick(TObject *Sender)
{
    if (tvShaders->Selected&&!tvShaders->Selected->Data) tvShaders->Selected->EditText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::miDeleteFolderClick(TObject *Sender)
{
    if (tvShaders->Selected&&!tvShaders->Selected->Data){
		if (MessageDlg("Delete selected folder and inside objects?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) == mrYes){
		    TElTreeItem* fld=tvShaders->Selected;
            for ( TElTreeItem* pNode = fld->GetFirstChild(); pNode; pNode = fld->GetNextChild(pNode))
                if (pNode->Data){
	                SHLib->DeleteShader(pNode->Text);
                    OnModified();
                }
	     	fld->Delete();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::tvShadersDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
    TElTreeItem* node;
    node = tvShaders->GetItemAtY(Y);
    if (node&&(!node->Data)&&(!node->IsUnder(FDragItem)))FDragItem->MoveTo(node);
    else if (node&&(!node->IsUnder(FDragItem)))          FDragItem->MoveTo(node->Parent);
    if (FDragItem&&FDragItem->Data){
        OnModified();
//        node=(node->Parent)?node->Parent:node;
//        ((CShaderDef*)(FDragItem->Data))->SetFolderName(node->Text);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::tvShadersDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    TElTreeItem* node;
    Accept = false;
    if (Source != tvShaders) return;
    node = tvShaders->GetItemAtY(Y);
    if (node&&(!node->IsUnder(FDragItem))) Accept = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::tvShadersStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
    FDragItem = tvShaders->ItemFocused;
}
//---------------------------------------------------------------------------

extern bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails);

void __fastcall TfrmEditShaders::tvShadersKeyPress(TObject *Sender, char &Key)
{
	if (Key==VK_DELETE)		ebRemoveShaderClick(Sender);
	else if (Key==VK_INSERT)ebNewShaderClick(Sender);
    else{
        TElTreeItem* node = tvShaders->Items->LookForItemEx(tvShaders->Selected,-1,false,false,false,&Key,LookupFunc);
        if (!node) node = tvShaders->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
        if (node){
            tvShaders->Selected = node;
            tvShaders->EnsureVisible(node);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::ebReloadShadersClick(TObject *Sender)
{
	AnsiString sh_nm;
	if (tvShaders->Selected) sh_nm = tvShaders->Selected->Text;
	SHLib->Reload();
    m_SelectedShader = 0;
    InitShaderFolder(sh_nm.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::rgTestObjectClick(TObject *Sender)
{
    AnsiString fn;
    if(rgTestObject->ItemIndex!=4) gbExtern->Hide(); 
	switch(rgTestObject->ItemIndex){
    	case 0: fn="$ShaderTest_Plane"; 	break;
    	case 1: fn="$ShaderTest_Box"; 		break;
    	case 2: fn="$ShaderTest_Sphere"; 	break;
    	case 3: fn="$ShaderTest_Teapot";	break;
    	case 4: fn=g_ExternalObject; lbSelectObject->Caption=g_ExternalObject.IsEmpty()?AnsiString("..."):g_ExternalObject; gbExtern->Show(); 	break;
        default: THROW2("Failed select test object.");
    }
    m_TestObject = Lib->SearchObject(fn.c_str());
    if (!m_TestObject&&!fn.IsEmpty()){
       	Log->DlgMsg(mtError,"System object '%s.object' can't find in object library. Preview disabled.",fn.c_str());
        g_ExternalObject = "";
    }
	m_TestExternSurface = 0;
	ebDropper->Down = false;
    
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::ebMergeClick(TObject *Sender)
{
	AnsiString fn;
    if( FS.GetOpenName( &FS.m_GameRoot, fn ) ){
    	int cnt;
    	cnt=SHLib->Merge(fn.c_str());
        Log->DlgMsg(mtInformation,"Append %d shader(s)",cnt);
        AnsiString sh_nm;
        if (tvShaders->Selected) sh_nm = tvShaders->Selected->Text;
        m_SelectedShader = 0;
        InitShaderFolder(sh_nm.c_str());
		SHLib->SaveLibrary();
		UI->Device.ReloadShaders();
        OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::ebExternSelectClick(TObject *Sender)
{
	if (m_TestObject) m_TestObject->UnloadObject();
    LPCSTR N = TfrmChoseItem::SelectObject(false,true,0,0);
    if (!N) return;

    m_TestObject 		= Lib->SearchObject(N);
    if (m_TestObject) 	g_ExternalObject = m_TestObject->GetRefName();
    else				g_ExternalObject = "";
    lbSelectObject->Caption = g_ExternalObject.IsEmpty()?AnsiString("..."):g_ExternalObject;

    m_TestExternSurface = 0;
	lbExternSelMat->Caption = "...";
    lbExternSelShader->Caption = "...";
	ebAssignShader->Enabled = false;
    ebSaveExternObject->Enabled = false;
	ebDropper->Down = false;
    UI->RedrawScene();
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditShaders::ebAssignShaderClick(TObject *Sender)
{
	if (m_TestExternSurface&&cbPreview->Checked&&m_SelectedShader){
        if (m_TestExternSurface->shader){
            if (strcmp(m_TestExternSurface->shader->shader->cName,m_SelectedShader->cName)!=0){
                UI->Device.Shader.Delete(m_TestExternSurface->shader);
                m_TestExternSurface->shader = UI->Device.Shader.Create(m_SelectedShader->cName,m_TestExternSurface->textures);
            }
        }else
            m_TestExternSurface->shader = UI->Device.Shader.Create(m_SelectedShader->cName,m_TestExternSurface->textures);
	    ebSaveExternObject->Enabled = true;
        lbExternSelShader->Caption = m_TestExternSurface->shader->shader->cName;
    }
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::ebSaveExternObjectClick(TObject *Sender)
{
	if (m_TestObject){
    	m_TestObject->SaveObject();
	    ebSaveExternObject->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditShaders::ExtBtn1Click(TObject *Sender)
{
    for(ShaderIt it=SHLib->m_Shaders.begin(); it!=SHLib->m_Shaders.end(); it++)
    	it->R.iPriority = 1;
	SHLib->SaveLibrary();
    Log->DlgMsg(mtInformation,"All Priority = 1");
}
//---------------------------------------------------------------------------

