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

    InplaceEngineEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceEngineEdit->Editor->BorderStyle	= bsNone;
    InplaceCompilerEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceCompilerEdit->Editor->BorderStyle= bsNone;
    frmMain->paLeftBar->Width 	= paLeftBar->Width+2;
    frmMain->sbToolsMin->Left 	= paLeftBar->Width-frmMain->sbToolsMin->Width-3;
    bFocusedAffected 			= true;
    
    // events
    tvEngine_->OnStartDrag 		= FHelper.StartDrag;
    tvEngine_->OnDragOver 		= FHelper.DragOver;
    tvCompiler_->OnStartDrag 	= FHelper.StartDrag;
    tvCompiler_->OnDragOver 	= FHelper.DragOver;
    tvMtl_->OnStartDrag 		= FHelper.StartDrag;
    tvMtl_->OnDragOver 			= FHelper.DragOver;
    tvSoundEnv_->OnStartDrag	= FHelper.StartDrag;
    tvSoundEnv_->OnDragOver 	= FHelper.DragOver;
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

void __fastcall TfraLeftBar::ebEngineShaderFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmEngineShadersFile,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmPreviewObject,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebImageCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmImages,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngine_MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Shift.Contains(ssRight)&&Tools.Current()->PopupMenu()) FHelper.ShowPPMenu(Tools.Current()->PopupMenu(),dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PreviewClick(TObject *Sender)
{
	UI.Command( COMMAND_SELECT_PREVIEW_OBJ, dynamic_cast<TMenuItem*>(Sender)->Tag );
}
//---------------------------------------------------------------------------
/*
void TfraLeftBar::InitPalette(TemplateVec& lst)
{
	for (TemplateIt it=lst.begin(); it!=lst.end(); it++){
    	TMenuItem* mi= xr_new<TMenuItem>((TComponent*)0);
		mi->Caption = (*it)->getComment();
		mi->Tag     = int(*it);
		mi->OnClick = TemplateClick;
		pmTemplateList->Items->Add(mi);
	}
}
*/
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineShaderCreateMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmTemplateList,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::TemplateClick(TObject *Sender)
{
/*
	TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    AnsiString folder;
	FHelper.MakeName(tvEngine_->Selected,0,folder,true);
    CBlender* B = Tools.SEngine.AppendBlender(((CBlender*)mi->Tag)->getDescription().CLS,folder.c_str(),0);
	Tools.SEngine.ResetShaders(true);
	Tools.SEngine.SetCurrentBlender(B);
	Tools.SEngine.Modified();
*/
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

void __fastcall TfraLeftBar::ebEngineShaderCloneClick(TObject *Sender)
{
//.
/*
    TElTreeItem* pNode = tvEngine->Selected;
    if (pNode&&FHelper.IsObject(pNode)){
		AnsiString full_name;
		FHelper.MakeName(pNode,0,full_name,false);
        CBlender* B=Tools.SEngine.CloneBlender(full_name.c_str());
		Tools.SEngine.ResetShaders(true);
	    Tools.SEngine.SetCurrentBlender(B);
		Tools.SEngine.Modified();
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
*/
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngine_KeyDown(TObject *Sender, WORD &Key,
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
//.
/*
	TElTreeInplaceAdvancedEdit* IE=0;
    switch(Tools.Current()->ID()){
    case aeEngine: 		IE=InplaceEngineEdit; 	break;
    case aeCompiler: 	IE=InplaceCompilerEdit;	break;
    case aeMtl:			IE=InplaceMaterialEdit;	break;
    case aeSoundEnv:	IE=InplaceSoundEnvEdit;	break;
    }

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
*/
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCreateItemClick(TObject *Sender)
{
    AnsiString folder;
	FHelper.MakeName(Tools.Current()->View()->Selected,0,folder,true);
	Tools.Current()->SetCurrentItem(Tools.Current()->AppendItem(folder.c_str(),0));
	Tools.Current()->Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebMaterialPairCreateClick(TObject *Sender)
{
//.
/*    LPCSTR M=0;
    int cnt = TfrmChoseItem::SelectItem(TfrmChoseItem::smGameMaterial,M,2);
    if (2==cnt){
    	int mtl0,mtl1;
    	GMLib.MtlNameToMtlPair(M,mtl0,mtl1);
		Tools.SMtl.SetCurrentItem(Tools.SMtlPair.AppendItem(mtl0,mtl1,0));
		Tools.SMtl.Modified();
    }else{
    	if (1==cnt) ELog.DlgMsg(mtError,"Select 2 material.");
    }
*/
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebMaterialPairCloneClick(TObject *Sender)
{
//.
/*
    TElTreeItem* pNode = tvMaterialPair->Selected;
    if (pNode&&FHelper.IsObject(pNode)){
    	// find parent
		AnsiString full_name;
		FHelper.MakeName(pNode,0,full_name,false);
        SGameMtlPair* parent = GMLib.GetMaterialPair(full_name.c_str());
        // select pair
        LPCSTR M=0;
        int cnt = TfrmChoseItem::SelectItem(TfrmChoseItem::smGameMaterial,M,2);
        if (2==cnt){
            int mtl0,mtl1;
            GMLib.MtlNameToMtlPair(M,mtl0,mtl1);
            Tools.SMaterial.SetCurrentMaterialPair(Tools.SMaterial.AppendMaterialPair(mtl0,mtl1,parent));
            Tools.SMaterial.Modified();
        }else{
            if (1==cnt) ELog.DlgMsg(mtError,"Select 2 material.");
        }

    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
*/
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::pcShadersChange(TObject *Sender)
{
	Tools.OnChangeEditor(Tools.FindTools(pcShaders->ActivePage));
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
		Tools.Current()->SetCurrentItem(Tools.Current()->AppendItem(0,full_name.c_str()));
		Tools.Current()->Modified();
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
	Tools.m_Props->RestoreParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::fsStorageSavePlacement(TObject *Sender)
{
	Tools.m_Props->SaveParams(fsStorage);
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






