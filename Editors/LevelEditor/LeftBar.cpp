#include "stdafx.h"
#pragma hdrstop

#include "LeftBar.h"
#include "BottomBar.h"
#include "UI_Tools.h"
#include "main.h"
#include "ObjectList.h"
#include "scene.h"
#include "EditLibrary.h"
#include "EditorPref.h"
#include "UI_Tools.h"
#include "folderlib.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
TfraLeftBar *fraLeftBar;

//---------------------------------------------------------------------------
__fastcall TfraLeftBar::TfraLeftBar(TComponent* Owner)
        : TFrame(Owner)
{
	ebTargetGroup->Tag		= OBJCLASS_GROUP;       ebViewGroup->Tag		= 0x1000+OBJCLASS_GROUP;
    ebTargetObject->Tag     = OBJCLASS_SCENEOBJECT;	ebViewObject->Tag		= 0x1000+OBJCLASS_SCENEOBJECT;
    ebTargetLight->Tag      = OBJCLASS_LIGHT;       ebViewLight->Tag      	= 0x1000+OBJCLASS_LIGHT;
    ebTargetShape->Tag		= OBJCLASS_SHAPE;       ebViewShape->Tag		= 0x1000+OBJCLASS_SHAPE;
    ebTargetSoundSrc->Tag   = OBJCLASS_SOUND_SRC;   ebViewSoundSrc->Tag  	= 0x1000+OBJCLASS_SOUND_SRC;
    ebTargetSoundEnv->Tag   = OBJCLASS_SOUND_ENV;   ebViewSoundEnv->Tag   	= 0x1000+OBJCLASS_SOUND_ENV;
    ebTargetGlow->Tag       = OBJCLASS_GLOW;        ebViewGlow->Tag       	= 0x1000+OBJCLASS_GLOW;
    ebTargetSpawnPoint->Tag = OBJCLASS_SPAWNPOINT;  ebViewSpawnPoint->Tag 	= 0x1000+OBJCLASS_SPAWNPOINT;
    ebTargetWay->Tag  		= OBJCLASS_WAY;         ebViewWay->Tag  		= 0x1000+OBJCLASS_WAY;
    ebTargetSector->Tag 	= OBJCLASS_SECTOR;      ebViewSector->Tag 		= 0x1000+OBJCLASS_SECTOR;
    ebTargetPortal->Tag		= OBJCLASS_PORTAL;      ebViewPortal->Tag		= 0x1000+OBJCLASS_PORTAL;
    ebTargetPS->Tag			= OBJCLASS_PS;          ebViewPS->Tag			= 0x1000+OBJCLASS_PS;
    ebTargetDO->Tag			= OBJCLASS_DO;          ebViewDO->Tag			= 0x1000+OBJCLASS_DO;
    ebTargetAIMap->Tag		= OBJCLASS_AIMAP;       ebViewAIMap->Tag		= 0x1000+OBJCLASS_AIMAP;

    DEFINE_INI				(fsStorage);
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

void GetHeight(int& h, TForm* f)
{
    if (f){
        TPanel* pa;
        for (int j=0; j<f->ControlCount; j++){
            TComponent* temp = f->Controls[j];
            GetHeight(h,dynamic_cast<TForm*>(temp));
            pa = dynamic_cast<TPanel*>(temp);
            if (pa&&pa->Visible){ h+=pa->Height; UpdatePanel(pa);}
        }
    }
}
//---------------------------------------------------------------------------

void TfraLeftBar::UpdateBar()
{
    int i, j, h=0;
    for (i=0; i<fraLeftBar->ComponentCount; i++){
        TComponent* temp = fraLeftBar->Components[i];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
    for (i=0; i<paFrames->ControlCount; i++)
        GetHeight(h,dynamic_cast<TForm*>(paFrames->Controls[i]));
    
    int hh = fraLeftBar->Height-(paLeftBar->Height+h);
    if (hh<=0) hh = 0; 
    paFrames->Height = h+hh;

    h=0;
    for (j=0; j<paLeftBar->ControlCount; j++){
        TPanel* pa = dynamic_cast<TPanel*>(paLeftBar->Controls[j]);
        if (pa&&pa->Visible) h+=pa->Height;
    }
    paLeftBar->Height = h+2;
    paFrames->Top = paLeftBar->Top+paLeftBar->Height;
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::splEmptyMoved(TObject *Sender)
{
	UpdateBar();
//
}
//---------------------------------------------------------------------------

void TfraLeftBar::MinimizeAllFrames()
{
    for (int i=0; i<paFrames->ControlCount; i++){
		TForm* f = dynamic_cast<TForm*>(paFrames->Controls[i]);
        if (f){
	        for (int j=0; j<f->ControlCount; j++){
                TPanel* pa = dynamic_cast<TPanel*>(f->Controls[j]);
                if (pa) PanelMinimize(pa);
            }
        }
    }
    for (int j=0; j<paLeftBar->ControlCount; j++){
        TPanel* pa = dynamic_cast<TPanel*>(paLeftBar->Controls[j]);
	    if (pa) PanelMinimize(pa);
    }
	UpdateBar();
}
//---------------------------------------------------------------------------

void TfraLeftBar::MaximizeAllFrames()
{
    for (int j=0; j<paLeftBar->ControlCount; j++){
        TPanel* pa = dynamic_cast<TPanel*>(paLeftBar->Controls[j]);
	    if (pa)	PanelMaximize(pa);
    }
    for (int i=0; i<paFrames->ControlCount; i++){
		TForm* f = dynamic_cast<TForm*>(paFrames->Controls[i]);
        if (f){
	        for (int j=0; j<f->ControlCount; j++){
                TPanel* pa = dynamic_cast<TPanel*>(f->Controls[j]);
                if (pa){
                	if (pa->Align==alClient){
                        paFrames->Height-=(pa->Height-pa->Constraints->MinHeight);
                    }else
	                 	PanelMaximize(pa);
                }
            }
        }
    }
	UpdateBar();
}
//---------------------------------------------------------------------------

void TfraLeftBar::ChangeTarget(EObjClass tgt)
{
    for (int i=0; i<paTarget->ControlCount; i++){
    	TExtBtn* B = dynamic_cast<TExtBtn *>(paTarget->Controls[i]);
        if (B&&B->Tag==tgt)	B->Down = true;
    }
    UI.RedrawScene	();
    UpdateBar		();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebClearClick(TObject *Sender)
{
	UI.Command( COMMAND_CLEAR );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebLoadClick(TObject *Sender)
{
	UI.Command( COMMAND_LOAD );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSaveClick(TObject *Sender)
{
	UI.Command( COMMAND_SAVE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSaveAsClick(TObject *Sender)
{
	UI.Command( COMMAND_SAVEAS );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebBuildClick(TObject *Sender)
{
	UI.Command( COMMAND_BUILD );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::miSceneSummaryClick(TObject *Sender)
{
	UI.Command( COMMAND_SCENE_SUMMARY );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebOptionsClick(TObject *Sender)
{
	UI.Command( COMMAND_OPTIONS );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCutClick(TObject *Sender)
{
	UI.Command( COMMAND_CUT );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCopyClick(TObject *Sender)
{
	UI.Command( COMMAND_COPY );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebPasteClick(TObject *Sender)
{
 	UI.Command( COMMAND_PASTE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebUndoClick(TObject *Sender)
{
 	UI.Command( COMMAND_UNDO );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRedoClick(TObject *Sender)
{
 	UI.Command( COMMAND_REDO );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebOpenSelClick(TObject *Sender)
{
 	UI.Command( COMMAND_LOAD_SELECTION );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSaveSelClick(TObject *Sender)
{
 	UI.Command( COMMAND_SAVE_SELECTION );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebValidateSceneClick(TObject *Sender)
{
	UI.Command( COMMAND_VALIDATE_SCENE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRefreshEditorClick(TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_TEXTURES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::RefreshObjects1Click(TObject *Sender)
{
	UI.Command( COMMAND_RELOAD_OBJECTS );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::RefreshLibrary1Click(TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_LIBRARY );
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

void __fastcall TfraLeftBar::UpdateSceneTextures1Click(TObject *Sender)
{
	UI.Command( COMMAND_RELOAD_TEXTURES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::MenuItem9Click(TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_TEXTURES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::MenuItem14Click(TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_SOUND_ENVS );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::UpdateEnvironmentGeometry1Click(
      TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_SOUND_ENV_GEOMETRY );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebObjectListClick(TObject *Sender)
{
	UI.Command( COMMAND_SHOW_OBJECTLIST );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEditLibClick(TObject *Sender)
{
	UI.Command( COMMAND_LIBRARY_EDITOR );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::TargetClick(TObject *Sender)
{
    TExtBtn* btn=dynamic_cast<TExtBtn*>(Sender);    VERIFY(btn);
    UI.Command(COMMAND_CHANGE_TARGET, btn->Down?btn->Tag:OBJCLASS_DUMMY);
    // turn off snap mode
    ebSnapListMode->Down = false;
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebViewClick(TObject *Sender)
{
    TExtBtn* btn=dynamic_cast<TExtBtn*>(Sender);    VERIFY(btn);
    UI.Command(COMMAND_CHANGE_VIEW, btn->Tag, btn->Down);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PanelMimimizeClickClick(TObject *Sender)
{
    PanelMinMaxClick(Sender);
    UpdateBar();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PanelMaximizeClick(TObject *Sender)
{
    ::PanelMaximizeClick(Sender);
    UpdateBar();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEditorPreferencesClick(TObject *Sender)
{
	UI.Command(COMMAND_EDITOR_PREF);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebInvertClick(TObject *Sender)
{
	UI.Command(COMMAND_INVERT_SELECTION_ALL);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSelectAllClick(TObject *Sender)
{
	UI.Command(COMMAND_SELECT_ALL);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebUnselectAllClick(TObject *Sender)
{
	UI.Command(COMMAND_DESELECT_ALL);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebDeleteClick(TObject *Sender)
{
	UI.Command(COMMAND_DELETE_SELECTION);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebHideAllClick(TObject *Sender)
{
	UI.Command(COMMAND_HIDE_ALL,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebUnhideAllClick(TObject *Sender)
{
	UI.Command(COMMAND_HIDE_ALL,TRUE);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebHideSelectedClick(TObject *Sender)
{
	UI.Command(COMMAND_HIDE_SEL,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebHideUnselectedClick(TObject *Sender)
{
	UI.Command(COMMAND_HIDE_UNSEL);
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::ebLockAllClick(TObject *Sender)
{
	UI.Command(COMMAND_LOCK_ALL,TRUE);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebLockSelClick(TObject *Sender)
{
	UI.Command(COMMAND_LOCK_SEL,TRUE);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebLockUnselClick(TObject *Sender)
{
	UI.Command(COMMAND_LOCK_UNSEL,TRUE);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebUnlockAllClick(TObject *Sender)
{
	UI.Command(COMMAND_LOCK_ALL,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebUnlockSelClick(TObject *Sender)
{
	UI.Command(COMMAND_LOCK_SEL,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebUnlockUnselClick(TObject *Sender)
{
	UI.Command(COMMAND_LOCK_UNSEL,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebMakeGameClick(TObject *Sender)
{
	UI.Command( COMMAND_MAKE_GAME );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::MakeDetailsClick(TObject *Sender)
{
	UI.Command( COMMAND_MAKE_DETAILS );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::MakeHOM1Click(TObject *Sender)
{
	UI.Command( COMMAND_MAKE_HOM );
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::MakeAIMap1Click(TObject *Sender)
{
	UI.Command( COMMAND_MAKE_AIMAP );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmSceneFile,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmScene,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCompileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmCompile,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExtBtn2MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmObjects,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebImagesMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmImages,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSoundsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmSounds,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExtBtn7MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmToolsEdit,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExtBtn8MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmToolsSelection,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExtBtn10MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmToolsVisibility,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExtBtn9MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmToolsLocking,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebLightAnimationEditorClick(TObject *Sender)
{
	UI.Command(COMMAND_LANIM_EDITOR);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebClearSnapClick(TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to clear snap objects?")==mrYes)
		UI.Command(COMMAND_CLEAR_SNAP_OBJECTS);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSetSnapClick(TObject *Sender)
{
	UI.Command(COMMAND_SET_SNAP_OBJECTS);
}
//---------------------------------------------------------------------------
void __fastcall TfraLeftBar::miAddSelectedToListClick(TObject *Sender)
{
	UI.Command(COMMAND_ADD_SEL_SNAP_OBJECTS);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::SelectObjectFromList1Click(TObject *Sender)
{
	UI.Command(COMMAND_SELECT_SNAP_OBJECTS);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::RemoveSelectedFromList1Click(TObject *Sender)
{
	UI.Command(COMMAND_DEL_SEL_SNAP_OBJECTS);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::UpdateSnapList()
{
	lbSnapList->Items->Clear();
    ObjectList* lst = Scene.GetSnapList(true);
    if (lst&&!lst->empty()){
    	int idx=0;
        ObjectIt _F=lst->begin();
    	for (;_F!=lst->end(); _F++,idx++){
        	AnsiString s; s.sprintf("%d: %s",idx,(*_F)->Name);
        	lbSnapList->Items->Add(s);
        }
    }
    Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExtBtn1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	FHelper.ShowPPMenu(pmSnapListCommand,dynamic_cast<TExtBtn*>(Sender));
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebUseSnapListClick(TObject *Sender)
{
	UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::miPropertiesClick(TObject *Sender)
{
    UI.Command(COMMAND_SHOW_PROPERTIES);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Quit1Click(TObject *Sender)
{
	UI.Command(COMMAND_QUIT);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebPropertiesClick(TObject *Sender)
{
	UI.Command(COMMAND_SHOW_PROPERTIES);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ImportCompilerErrorsClick(TObject *Sender)
{
	UI.Command(COMMAND_IMPORT_COMPILER_ERROR);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebIgnoreModeClick(TObject *Sender)
{
	UI.Command(COMMAND_UPDATE_PROPERTIES);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::miClearErrorListClick(TObject *Sender)
{
	UI.Command(COMMAND_CLEAR_COMPILER_ERROR);
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::ebSnapListModeClick(TObject *Sender)
{
    TExtBtn* E = dynamic_cast<TExtBtn*>(Sender); VERIFY(E);
	if (E->Down&&(eaSelect!=Tools.GetAction())){
    	ELog.Msg(mtError,"Before modify snap list activate select mode!");
        E->Down = false;
    }
}
//---------------------------------------------------------------------------


