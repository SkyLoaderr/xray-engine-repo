//---------------------------------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "main.h"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma package(smart_init)
#pragma link "RxMenus"
#pragma link "Placemnt"
#pragma link "TopBar"
#pragma resource "*.dfm"
#include "ui_main.h"
#include "filesystem.h"
#include "library.h"
#include "scene.h"
#include "ObjectList.h"
#include "SceneClassList.h"
#include "EditLibrary.h"
#include "ui_tools.h"
#include "statistic.h"

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
        : TForm(Owner)
{
	if (!UI.Init(paRender, ppRender)){
        MessageDlg("Don't create DirectX device. Editor halted!", mtError, TMsgDlgButtons() << mbOK, 0);
        Application->Terminate();
    }
	FS.Init( Application->ExeName.c_str() );
	Lib.Init();
    Scene.Init();

    sbTargetScene->Tag      = etScene;
    sbTargetObject->Tag     = etObject;
    sbTargetLight->Tag      = etLight;
    sbTargetSound->Tag      = etSound;
    sbTargetPClipper->Tag   = etPClipper;
    sbTargetTexturing->Tag  = etTexturing;
//    sbTargetLand->Tag       = etLandscape;
//    sbTargetHMap->Tag       = etHMap;
//    sbTargetPivot->Tag      = etPivot;

    sbActionSelect->Tag     = eaSelect;
    sbActionAdd->Tag        = eaAdd;
    sbActionMove->Tag       = eaMove;
    sbActionRotate->Tag     = eaRotate;
    sbActionScale->Tag      = eaScale;

    UI.m_Tools->Init(paFrames);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormShow(TObject *Sender)
{
    sbTargetScene->Down = true;     sbTargetScene->Click();
    sbActionSelect->Down = true;    sbActionSelect->Click();
    tmRefresh->Enabled = true;

    UI.bRenderTextures = miRenderTextures->Checked;
    UI.bRenderWire     = miRenderWire->Checked;
    UI.bRenderLights   = miRenderLights->Checked;
    UI.bRenderFilter   = miRenderFilter->Checked;
    UI.bRenderMultiTex = miRenderMultiTex->Checked;
    UI.bDrawLights     = miDrawLights->Checked;
    UI.bDrawSounds     = miDrawSounds->Checked;
    UI.bDrawGrid       = miDrawGrid->Checked;
    UI.bDrawPivot      = miDrawPivot->Checked;
    UI.bDrawStatistic  = miDrawStatistic->Checked;

    if (paLeftBar->Tag > 0) paLeftBar->Parent = paRender;
    else paLeftBar->Parent = frmMain;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = UI.Command(COMMAND_EXIT);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::DisplayHint(TObject *Sender)
{
    cbInfo->Text = " Info: " + GetLongHint(Application->Hint);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsMainForm->IniFileName = buf;
    Application->OnHint = DisplayHint;
    Application->OnIdle = IdleHandler;
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::TargetClick(TObject *Sender)
{
    TExtBtn* btn = dynamic_cast<TExtBtn*>(Sender);
    if (btn){
        btn->Down = true;
        UI.m_Tools->SetTarget(btn->Tag);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ActionClick(TObject *Sender)
{
    TExtBtn* btn = dynamic_cast<TExtBtn*>(Sender);
    if (btn){
        btn->Down = true;
        UI.m_Tools->SetAction(btn->Tag);
    }
}
//---------------------------------------------------------------------------

#define MIN_PANEL_HEIGHT 15
void __fastcall TfrmMain::sbToolsMinClick(TObject *Sender)
{
    if (paLeftBar->Tag > 0){
        paLeftBar->Align  = alLeft;
        paLeftBar->Parent = frmMain;
        paLeftBar->Height = paLeftBar->Tag;
        paLeftBar->Tag    = 0;
    }else{
        paLeftBar->Align  = alNone;
        paLeftBar->Parent = paRender;
        paLeftBar->Tag    = paLeftBar->Height;
        paLeftBar->Height = MIN_PANEL_HEIGHT;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Label1Click(TObject *Sender)
{
    if (paLeftBar->Tag > 0){
        paLeftBar->Align  = alLeft;
        paLeftBar->Parent = frmMain;
        paLeftBar->Height = paLeftBar->Tag;
        paLeftBar->Tag    = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::MainPanelMininmizeClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::MainPanelMaximizeClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::IdleHandler(TObject *Sender, bool &Done)
{
    Done = false;
    UI.Idle();
    if (UI.bDrawStatistic!=paStat->Visible) paStat->Visible = UI.bDrawStatistic;
    if (UI.bDrawStatistic){
        lbPoly->Caption   = IntToStr(UI.stat->lRenderPolyCount);
        lbTLight->Caption = IntToStr(UI.stat->lTotalLight);
        lbRLight->Caption = IntToStr(UI.stat->lLightInScene);
        lbTotalFrame->Caption = IntToStr(UI.stat->lTotalFrame);
    }
}
void __fastcall TfrmMain::RenderResize(TObject *Sender)
{
    UI.Resize();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paRenderMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    ShiftMouse = Shift;
    UI.MouseStart(Shift,X,Y);
    UI.UpdateScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::paRenderMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    UI.MouseProcess(Shift,X,Y);
    UI.UpdateScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::paRenderMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    UI.MouseEnd(Shift,X,Y);
    UI.UpdateScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    ShiftKey = Shift;
    if (!UI.KeyDown(Key, Shift)){
        ApplyShortCut(Key, Shift);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (!UI.KeyUp(Key, Shift)){
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormKeyPress(TObject *Sender, char &Key)
{
    if (!UI.KeyPress(Key, ShiftKey)){
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ApplyShortCut(WORD Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE)     ResetActionToSelect();
    if (Shift.Contains(ssCtrl)){
        if (Key=='C'||Key=='c')	sbCopyClick(0);
        if (Key=='V'||Key=='v') sbPasteClick(0);
        if (Key=='X'||Key=='x') sbCutClick(0);
        if (Key=='Z'||Key=='z') sbUndoClick(0);
        if (Key=='Y'||Key=='y') sbRedoClick(0);
        if (Key=='S'||Key=='s'){
            if (Shift.Contains(ssAlt))  sbSaveAsClick(0);
            else                        sbSaveClick(0);
        }
        if (Key=='O'||Key=='o') sbLoadClick(0);
        if (Key=='N'||Key=='n') sbClearClick(0);
        if (Key==VK_F5) sbRunClick(0);
        if (Key==VK_F7) sbOptionsClick(0);
        if (Key=='A'||Key=='a') UI.Command(COMMAND_SELECT_ALL);
        if (Key=='I'||Key=='i') UI.Command(COMMAND_INVERT_SELECTION_ALL);
        if (Key==VK_DELETE) UI.Command( COMMAND_DELETE_SELECTION );
    }else{
        if (Shift.Contains(ssAlt)){
        // target
        // action
            if (Key=='S'||Key=='s') ActionClick(sbActionSelect);
            if (Key=='A'||Key=='a') ActionClick(sbActionAdd);
            if (Key=='M'||Key=='m') ActionClick(sbActionMove);
            if (Key=='R'||Key=='r') ActionClick(sbActionRotate);
            if (Key=='C'||Key=='c') ActionClick(sbActionScale);
            if (Key=='1') TargetClick(sbTargetScene);
            if (Key=='2') TargetClick(sbTargetObject);
            if (Key=='3') TargetClick(sbTargetLight);
            if (Key=='4') TargetClick(sbTargetSound);
            if (Key=='5') TargetClick(sbTargetPClipper);
            if (Key=='6') TargetClick(sbTargetTexturing);
        }else{
            if (Key=='H'||Key=='h') sbObjectListClick(0);
        }
    }
}

void TfrmMain::UpdateCaption()
{
    Caption.sprintf("Level Editor: %s",UI.GetEditFileName());
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::QualityClick(TObject *Sender)
{
    UI.SetRenderQuality((float)(((TMenuItem*)Sender)->Tag)/100);
    ((TMenuItem*)Sender)->Checked = true;
    UI.Resize();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::paInfoResize(TObject *Sender)
{
    cbInfo->Width = paMessage->Width;
    cbInfo->Left = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbClearClick(TObject *Sender)
{
	UI.Command( COMMAND_CLEAR );
    UpdateCaption();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbLoadClick(TObject *Sender)
{
	UI.Command( COMMAND_LOAD );
    UpdateCaption();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbSaveClick(TObject *Sender)
{
	UI.Command( COMMAND_SAVE );
    UpdateCaption();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbSaveAsClick(TObject *Sender)
{
	UI.Command( COMMAND_SAVEAS );
    UpdateCaption();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbRunClick(TObject *Sender)
{
	UI.Command( COMMAND_BUILD );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbOptionsClick(TObject *Sender)
{
	UI.Command( COMMAND_OPTIONS );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbUpdateAllClick(TObject *Sender)
{
	UI.Command( COMMAND_UPDATE_ALL );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbMenuClick(TObject *Sender)
{
	UI.Command( COMMAND_MENU );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbCutClick(TObject *Sender)
{
	UI.Command( COMMAND_CUT );
    sbPaste->Enabled = true;
    miPaste->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbCopyClick(TObject *Sender)
{
	UI.Command( COMMAND_COPY );
    sbPaste->Enabled = true;
    miPaste->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbPasteClick(TObject *Sender)
{
 	UI.Command( COMMAND_PASTE );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbUndoClick(TObject *Sender)
{
 	UI.Command( COMMAND_UNDO );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbRedoClick(TObject *Sender)
{
 	UI.Command( COMMAND_REDO );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbObjectListClick(TObject *Sender)
{
    frmObjectList->Show();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmRefreshTimer(TObject *Sender)
{
    int i;
    for (i=0; i<frmMain->ComponentCount; i++){
        TComponent* temp = frmMain->Components[i];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
    for (i=0; i<paFrames->ControlCount; i++){
        TControl* frame = paFrames->Controls[i];
        if (dynamic_cast<TFrame*>(frame) != NULL){
            for (int j=0; j<frame->ComponentCount; j++){
            TComponent* temp = frame->Components[j];
            if (dynamic_cast<TExtBtn *>(temp) != NULL)
                ((TExtBtn*)temp)->UpdateMouseInControl();
            }
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::ppRenderPaint(TObject *Sender)
{
    UI.UpdateScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miHideClick(TObject *Sender)
{
    UI.Command(COMMAND_HIDE_SELECTION);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miPropertiesClick(TObject *Sender)
{
    UI.Command(COMMAND_SHOWPROPERTIES, pmObjectContext->Tag);
    UI.UpdateScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miCatPClipperClick(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        if (mi==miCatObject)    UI.Command(COMMAND_SHOWPROPERTIES, OBJCLASS_SOBJECT2);
        if (mi==miCatLight)     UI.Command(COMMAND_SHOWPROPERTIES, OBJCLASS_LIGHT);
        if (mi==miCatSound)     UI.Command(COMMAND_SHOWPROPERTIES, OBJCLASS_SOUND);
        if (mi==miCatPClipper)  UI.Command(COMMAND_SHOWPROPERTIES, OBJCLASS_PCLIPPER);
    }
    UI.UpdateScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ebEditLibClick(TObject *Sender)
{
    if (Scene.ObjCount()){
        MessageDlg("Scene must be empty before edit!", mtError, TMsgDlgButtons() << mbOK, 0);
    }else{
        frmEditLibrary->EditLibrary();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ebDrawClick(TObject *Sender)
{
    POINT pt;
    GetCursorPos(&pt);
    pmDraw->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::ClickDrawMenuItem(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        mi->Checked = !mi->Checked;
        if (mi==miDrawGrid)     UI.bDrawGrid    = mi->Checked;
        if (mi==miDrawPivot)    UI.bDrawPivot   = mi->Checked;
        if (mi==miDrawLights)   UI.bDrawLights  = mi->Checked;
        if (mi==miDrawSounds)   UI.bDrawSounds  = mi->Checked;
        if (mi==miDrawStatistic)UI.bDrawStatistic=mi->Checked;
    }
    UI.UpdateScene();
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::ClickRenderMenuItem(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        mi->Checked = !mi->Checked;
        if (mi==miRenderWire)    UI.bRenderWire     = mi->Checked;
        if (mi==miRenderTextures)UI.bRenderTextures = mi->Checked;
        if (mi==miRenderLights)  UI.bRenderLights   = mi->Checked;
        if (mi==miRenderFilter)  UI.bRenderFilter   = mi->Checked;
        if (mi==miRenderMultiTex)UI.bRenderMultiTex = mi->Checked;
    }
    UI.UpdateScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ebRenderAddBtnClick(TObject *Sender)
{
    POINT pt;
    GetCursorPos(&pt);
    pmRender->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

void ShowObjectContextMenu(const int cls)
{
    int bProp = 0;
    int cls_in = cls;
    int count;
    POINT pt;
    GetCursorPos(&pt);

    if (!Scene.SelectionCount( cls, true )) return;

    if (cls==OBJCLASS_NONE){
        count = Scene.SelectionCount( OBJCLASS_SOBJECT2, true );
        frmMain->miCatObject->Enabled    = !!count;
        if (count>0){bProp++; cls_in = OBJCLASS_SOBJECT2;}

        count = Scene.SelectionCount( OBJCLASS_LIGHT, true );
        frmMain->miCatLight->Enabled     = !!count;
        if (count>0){bProp++; cls_in = OBJCLASS_LIGHT;}

        count = Scene.SelectionCount( OBJCLASS_SOUND, true );
        frmMain->miCatSound->Enabled     = !!count;
        if (count>0){bProp++; cls_in = OBJCLASS_SOUND;}

        count = Scene.SelectionCount( OBJCLASS_PCLIPPER, true );
        frmMain->miCatPClipper->Enabled  = !!Scene.SelectionCount( OBJCLASS_PCLIPPER, true );
        if (count>0){bProp++; cls_in = OBJCLASS_PCLIPPER;}
    }
    if (bProp>1){
        frmMain->pmObjectContext->Tag = cls;
        frmMain->miProperties->Visible = false;
        frmMain->miPropertiesbyCategory->Visible = true;
    }else{
        frmMain->pmObjectContext->Tag = cls_in;
        frmMain->miProperties->Visible = true;
        frmMain->miPropertiesbyCategory->Visible = false;
    }
    UI.UpdateScene(true);
    frmMain->pmObjectContext->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

void ResetActionToSelect()
{
    frmMain->ActionClick(frmMain->sbActionSelect);
}
//---------------------------------------------------------------------------


