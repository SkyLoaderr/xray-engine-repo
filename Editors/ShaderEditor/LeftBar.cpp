#include "stdafx.h"
#pragma hdrstop

#include "LeftBar.h"
#include "BottomBar.h"
#include "UI_Main.h"
#include "main.h"
#include "Blender.h"
#include "xr_trims.h"
#include "UI_Tools.h"
#include "FolderLib.h"
#include "PropertiesList.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma link "ElTreeAdvEdit"
#pragma link "ElPgCtl"
#pragma resource "*.dfm"
TfraLeftBar *fraLeftBar;

#define MIN_PANEL_HEIGHT 15
//---------------------------------------------------------------------------
void __fastcall PanelMinimizeClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }else{
        pa->Tag    = pa->Height;
        pa->Height = MIN_PANEL_HEIGHT;
    }
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}
void __fastcall PanelMaximizeOnlyClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}

//---------------------------------------------------------------------------
__fastcall TfraLeftBar::TfraLeftBar(TComponent* Owner)
        : TFrame(Owner)
{
	DEFINE_INI(fsStorage);

    InplaceEdit->Editor->Color			= TColor(0x00A0A0A0);
    InplaceEdit->Editor->BorderStyle	= bsNone;
    frmMain->paLeftBar->Width 			= paLeftBar->Width+2;
    frmMain->sbToolsMin->Left 			= paLeftBar->Width-frmMain->sbToolsMin->Width-3;
    bFocusedAffected 					= true;
    
    // events
    tvEngine->OnStartDrag 	= FHelper.StartDrag;
    tvEngine->OnDragOver 	= FHelper.DragOver;
    tvCompiler->OnStartDrag = FHelper.StartDrag;
    tvCompiler->OnDragOver 	= FHelper.DragOver;
    tvMtl->OnStartDrag 		= FHelper.StartDrag;
    tvMtl->OnDragOver 		= FHelper.DragOver;
    tvSoundEnv->OnStartDrag	= FHelper.StartDrag;
    tvSoundEnv->OnDragOver 	= FHelper.DragOver;
}
//---------------------------------------------------------------------------

void UpdatePanel(TPanel* p){
    if (p){
        for (int j=0; j<p->ControlCount; j++){
            TExtBtn* btn = dynamic_cast<TExtBtn *>(p->Controls[j]);
            if (btn) btn->UpdateMouseInControl();
        }
    }
}
//---------------------------------------------------------------------------

void TfraLeftBar::UpdateBar(){
    int i, j;
    for (i=0; i<fraLeftBar->ComponentCount; i++){
        TComponent* temp = fraLeftBar->Components[i];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSaveClick(TObject *Sender)
{
	UI.Command( COMMAND_SAVE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebReloadClick(TObject *Sender)
{
	UI.Command( COMMAND_RELOAD );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRefreshTexturesClick(TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_TEXTURES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PanelMimimizeClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
    UpdateBar();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PanelMaximizeClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
    UpdateBar();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEditorPreferencesClick(TObject *Sender)
{
	UI.Command(COMMAND_EDITOR_PREF);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebResetAnimationClick(TObject *Sender)
{
	UI.Command( COMMAND_RESET_ANIMATION );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCustomFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmCustomFile,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmEngineShadersFile,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebImageCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmImages,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngineMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Shift.Contains(ssRight)&&Tools.Current()->PopupMenu()) FHelper.ShowPPMenu(Tools.Current()->PopupMenu(),dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CreateFolder1Click(TObject *Sender)
{
    AnsiString 	folder;
    AnsiString 	start_folder;
    FHelper.MakeName(Tools.Current()->View()->Selected,0,start_folder,true);
    FHelper.GenerateFolderName(Tools.Current()->View(),Tools.Current()->View()->Selected,folder);
    folder = start_folder+folder;
    TElTreeItem* node = FHelper.AppendFolder(Tools.Current()->View(),folder.c_str());
    if (Tools.Current()->View()->Selected) Tools.Current()->View()->Selected->Expand(false);
    Tools.Current()->View()->EditItem(node,-1);
    Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExpandAll1Click(TObject *Sender)
{
	Tools.Current()->View()->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CollapseAll1Click(TObject *Sender)
{
	Tools.Current()->View()->FullCollapse();
}
//---------------------------------------------------------------------------

BOOL __fastcall TfraLeftBar::RemoveItem(LPCSTR p0)
{
	Tools.Current()->RemoveItem(p0);
    return TRUE;
}
//---------------------------------------------------------------------------
void TfraLeftBar::AfterRemoveItem()
{
//.    case aeEngine: 		
//.        Tools.SEngine.ResetCurrentBlender();
//.        Tools.SEngine.ApplyChanges();
//.        Tools.SEngine.ResetShaders();
	Tools.Current()->ResetCurrentItem();
	Tools.Current()->Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvItemFocused(TObject *Sender)
{
	if (!bFocusedAffected) return;
   	AnsiString name;
   	FHelper.MakeName(Tools.Current()->View()->Selected, 0, name, false);
	Tools.Current()->SetCurrentItem(name.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngineKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
   	if (Key==VK_DELETE) ebRemoveItemClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Rename1Click(TObject *Sender)
{
	TElTreeItem* node = Tools.Current()->View()->Selected;
    if (node) Tools.Current()->View()->EditItem(node,-1);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::InplaceEditValidateResult(TObject *Sender, bool &InputValid)
{
	TElTreeInplaceAdvancedEdit* IE=InplaceEdit;

    AnsiString new_text	= AnsiString(IE->Editor->Text).LowerCase();
    IE->Editor->Text 	= new_text;

    TElTreeItem* node = IE->Item;
    for (TElTreeItem* item=node->GetFirstSibling(); item; item=item->GetNextSibling()){
        if ((item->Text==new_text)&&(item!=IE->Item)){
            InputValid = false;
            return;
        }
    }
    AnsiString full_name;
    if (FHelper.IsFolder(node)){
        for (item=node->GetFirstChild(); item&&(item->Level>node->Level); item=item->GetNext()){
            if (FHelper.IsObject(item)){
                FHelper.MakeName(item,0,full_name,false);
                Tools.Current()->RenameItem	(full_name.c_str(),new_text.c_str(),node->Level);
            }
        }
    }else if (FHelper.IsObject(node)){
        FHelper.MakeName(node,0,full_name,false);
        Tools.Current()->RenameItem	(full_name.c_str(),new_text.c_str(),node->Level);
    }
    Tools.Current()->View()->Selected=node;
	Tools.Current()->Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCreateItemClick(TObject *Sender)
{
    AnsiString folder;
	FHelper.MakeName(Tools.Current()->View()->Selected,0,folder,true);
    Tools.Current()->AppendItem(folder.c_str(),0);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::pcShadersChange(TObject *Sender)
{
	Tools.OnChangeEditor(Tools.FindTools(pcShaders->ActivePage));
    InplaceEdit->Tree = Tools.Current()->View();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRemoveItemClick(TObject* Sender)
{
//	bFocusedAffected = false;
	TElTree* tv = Tools.Current()->View(); VERIFY(tv);
	FHelper.RemoveItem(tv,tv->Selected,RemoveItem,AfterRemoveItem);
//	bFocusedAffected = true;
//	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCloneItemClick(TObject *Sender)
{
    TElTreeItem* pNode = Tools.Current()->View()->Selected;
    if (pNode&&FHelper.IsObject(pNode)){
		AnsiString full_name;
		FHelper.MakeName(pNode,0,full_name,false);
        Tools.Current()->AppendItem(0,full_name.c_str());
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::RenameItem(LPCSTR p0, LPCSTR p1)
{
	Tools.Current()->RenameItem(p0,p1);
	Tools.Current()->Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::OnDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	FHelper.DragDrop(Sender,Source,X,Y,RenameItem);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::fsStorageRestorePlacement(TObject *Sender)
{
	Tools.m_ItemProps->RestoreParams(fsStorage);
	Tools.m_PreviewProps->RestoreParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::fsStorageSavePlacement(TObject *Sender)
{
	Tools.m_ItemProps->SaveParams(fsStorage);
	Tools.m_PreviewProps->SaveParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ImageEditor1Click(TObject *Sender)
{
	UI.Command( COMMAND_IMAGE_EDITOR );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Refresh1Click(TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_TEXTURES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Checknewtextures1Click(TObject *Sender)
{
	UI.Command( COMMAND_CHECK_TEXTURES );
}
//---------------------------------------------------------------------------

#include "SHEngineTools.h"
void __fastcall TfraLeftBar::PreviewObjClick(TObject *Sender)
{
	CSHEngineTools* tools = (CSHEngineTools*)Tools.FindTools(aeEngine); R_ASSERT(tools);
    tools->PreviewObjClick(Sender);
}
//---------------------------------------------------------------------------

