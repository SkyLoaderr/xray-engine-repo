#include "stdafx.h"
#pragma hdrstop

#include "LeftBar.h"
#include "BottomBar.h"
#include "UI_Main.h"
#include "main.h"
#include "UI_Tools.h"
#include "FolderLib.h"
#include "EditObject.h"
#include "Motion.h"
#include "PropertiesList.h"
#include "BonePart.h"
#include "FolderLib.h"
#include "NumericVector.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma link "ElTreeAdvEdit"
#pragma link "ElPgCtl"
#pragma link "MXCtrls"
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

    frmMain->paLeftBar->Width = paLeftBar->Width+2;
    frmMain->sbToolsMin->Left = paLeftBar->Width-frmMain->sbToolsMin->Width-3;
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::miRecentFilesClick(TObject *Sender)
{
	TMenuItem* MI = dynamic_cast<TMenuItem*>(Sender); R_ASSERT(MI&&(MI->Tag==0x1001));
    AnsiString fn = MI->Caption;
    if (FS.exist(fn.c_str()))	UI.Command(COMMAND_LOAD,(u32)fn.c_str());
    else						ELog.DlgMsg(mtError, "Error reading file '%s'",fn.c_str());
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::fsStorageSavePlacement(TObject *Sender)
{
//.    Tools.m_ObjectProps->SaveParams(fsStorage);
    Tools.m_ItemProps->SaveParams(fsStorage);
    Tools.m_PreviewObject.SaveParams(fsStorage);
    Tools.m_RenderObject.SaveParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::fsStorageRestorePlacement(TObject *Sender)
{
//.    Tools.m_ObjectProps->RestoreParams(fsStorage);
    Tools.m_ItemProps->RestoreParams(fsStorage);
    Tools.m_PreviewObject.RestoreParams(fsStorage);
    Tools.m_RenderObject.RestoreParams(fsStorage);
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
    if (ebRenderEngineStyle->Down&&!Tools.IsVisualPresent()) SetRenderStyle(false);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSaveClick(TObject *Sender)
{
	UI.Command( COMMAND_SAVE );
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

void __fastcall TfraLeftBar::ImageEditor1Click(TObject *Sender)
{
	UI.Command( COMMAND_IMAGE_EDITOR );
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

void __fastcall TfraLeftBar::ebMakePreviewClick(TObject *Sender)
{
	UI.Command( COMMAND_MAKE_PREVIEW );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebResetAnimationClick(TObject *Sender)
{
	UI.Command( COMMAND_RESET_ANIMATION );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmSceneFile,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneCommands1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmImages,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebPreviewObjectClickMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmPreviewObject,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvMotionsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button==mbRight)	FHelper.ShowPPMenu(pmShaderList,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void TfraLeftBar::AddMotion(LPCSTR full_name, bool bLoadMode)
{
//..	TElTreeItem* node = FHelper.AppendObject(tvMotions,full_name);
//    if (!bLoadMode){
//	    if (node&&node->Parent) node->Parent->Expand(false);
//    	node->Selected = true;
//		tvMotions->Selected = node;
//    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CreateFolder1Click(TObject *Sender)
{
//.	AnsiString folder;
//    AnsiString start_folder;
//    FHelper.MakeName(tvMotions->Selected,0,start_folder,true);
//    FHelper.GenerateFolderName(tvMotions,tvMotions->Selected,folder);
//    folder = start_folder+folder;
//	TElTreeItem* node = FHelper.AppendFolder(tvMotions,folder.c_str());
//    if (tvMotions->Selected) tvMotions->Selected->Expand(false);
//    tvMotions->EditItem(node,-1);
///	Tools.MotionModified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvMotionsStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
	FHelper.StartDrag(Sender,DragObject);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvMotionsDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	FHelper.DragOver(Sender,Source,X,Y,State,Accept);
}
//---------------------------------------------------------------------------
void __fastcall TfraLeftBar::RenameItem(LPCSTR p0, LPCSTR p1)
{
    Tools.RenameMotion(p0,p1);
}
//---------------------------------------------------------------------------
void __fastcall TfraLeftBar::tvMotionsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	FHelper.DragDrop(Sender,Source,X,Y,RenameItem);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Import1Click(TObject *Sender)
{
	UI.Command( COMMAND_IMPORT );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Load1Click(TObject *Sender)
{
	UI.Command( COMMAND_LOAD );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Clear1Click(TObject *Sender)
{
	UI.Command( COMMAND_CLEAR );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Save2Click(TObject *Sender)
{
	UI.Command( COMMAND_SAVE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::SaevAs1Click(TObject *Sender)
{
	UI.Command( COMMAND_SAVEAS );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebBonePartClick(TObject *Sender)
{
	frmBonePart->Run(Tools.CurrentObject());
    UI.Command(COMMAND_UPDATE_PROPERTIES);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::miExportOGFClick(TObject *Sender)
{
	UI.Command( COMMAND_EXPORT_OGF );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRenderStyleClick(TObject *Sender)
{
	if (Sender==ebRenderEngineStyle){
		if (!Tools.IsVisualPresent()) UI.Command( COMMAND_MAKE_PREVIEW );
        if (!Tools.IsVisualPresent()) SetRenderStyle(false);
        else						  SetRenderStyle(true);
    }
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void TfraLeftBar::SetRenderStyle(bool bEngineStyle)
{
    if (Tools.IsVisualPresent()&&bEngineStyle) 	ebRenderEngineStyle->Down = true;
    else 										ebRenderEditorStyle->Down = true;
	Tools.PlayMotion();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Custom1Click(TObject *Sender)
{
	UI.Command( COMMAND_SELECT_PREVIEW_OBJ, false );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::none1Click(TObject *Sender)
{
	UI.Command( COMMAND_SELECT_PREVIEW_OBJ, true );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Preferences1Click(TObject *Sender)
{
	UI.Command( COMMAND_PREVIEW_OBJ_PREF );
}
//---------------------------------------------------------------------------






