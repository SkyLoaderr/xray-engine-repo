//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include "RXCtrls.hpp"
#include "CloseBtn.hpp"
#include "RxMenus.hpp"
#include "Placemnt.hpp"
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include "TopBar.h"
//---------------------------------------------------------------------------
//class TUI;
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
        TPanel *paLeftBar;
        TPanel *paBottomBar;
    TPanel *paScene;
        TLabel *Label2;
    TPanel *paOther;
        TLabel *Label3;
    TPanel *paEdit;
        TLabel *Label4;
    TPanel *paTools;
        TLabel *Label1;
    TPanel *paBuild;
        TLabel *Label5;
    TPanel *paTarget;
        TLabel *Label6;
        TPanel *paInfo;
    TExtBtn *sbSceneMin;
    TExtBtn *sbBuildMin;
    TExtBtn *sbEditMin;
    TExtBtn *sbOtherMin;
    TExtBtn *sbTargetMin;
    TExtBtn *sbToolsMin;
    TPanel *paFrames;
    TPanel *paInfoCapt;
    TPanel *paAction;
    TLabel *Label8;
    TExtBtn *sbActionMin;
    TExtBtn *sbClear;
    TExtBtn *sbLoad;
    TExtBtn *sbSaveAs;
    TExtBtn *sbSave;
    TExtBtn *sbRun;
    TExtBtn *sbOptions;
    TExtBtn *sbCut;
    TExtBtn *sbCopy;
    TExtBtn *sbPaste;
    TExtBtn *sbUndo;
    TExtBtn *sbRedo;
    TExtBtn *sbUpdateAll;
    TExtBtn *sbObjectList;
    TExtBtn *sbTargetObject;
    TExtBtn *sbTargetLight;
    TExtBtn *sbTargetSound;
    TExtBtn *sbActionSelect;
    TExtBtn *sbActionAdd;
    TExtBtn *sbActionMove;
    TExtBtn *sbActionRotate;
    TExtBtn *sbActionScale;
    TTimer *tmRefresh;
    TPanel *paStat;
    TLabel *Label9;
    TExtBtn *ExtBtn1;
    TLabel *RxLabel1;
    TLabel *RxLabel2;
    TLabel *RxLabel3;
    TLabel *lbPoly;
    TLabel *lbTLight;
    TLabel *lbRLight;
    TExtBtn *sbTargetPClipper;
    TLabel *Label10;
    TLabel *lbTotalFrame;
    TExtBtn *sbTargetScene;
    TRxPopupMenu *pmObjectContext;
    TMenuItem *miProperties;
    TMenuItem *miHide;
    TMenuItem *miCopy;
    TMenuItem *miPaste;
    TMenuItem *N1;
    TMenuItem *miCut;
    TRxPopupMenu *pmDraw;
    TRxPopupMenu *pmRender;
    TMenuItem *miDrawGrid;
    TMenuItem *Quality1;
    TMenuItem *N252;
    TMenuItem *N502;
    TMenuItem *N752;
    TMenuItem *N1002;
    TMenuItem *N1252;
    TMenuItem *N1502;
    TMenuItem *N2001;
    TMenuItem *miDrawPivot;
    TMenuItem *miDrawLights;
    TMenuItem *miDrawSounds;
    TMenuItem *N3;
    TMenuItem *N4;
    TMenuItem *miDrawStatistic;
    TMenuItem *miRenderLights;
    TMenuItem *miRenderWire;
    TMenuItem *miRenderTextures;
    TMenuItem *miRenderFilter;
    TFormStorage *fsMainForm;
    TExtBtn *ebRender;
    TExtBtn *ebDraw;
    TPanel *paMessage;
    TComboBox *cbInfo;
    TMenuItem *miPropertiesbyCategory;
    TMenuItem *miCatObject;
    TMenuItem *miCatLight;
    TMenuItem *miCatSound;
    TMenuItem *miCatPClipper;
    TMenuItem *miRenderMultiTex;
    TExtBtn *sbTargetTexturing;
    TExtBtn *ebEditLib;
    TPanel *paMain;
    TPanel *paRender;
    TPaintBox *ppRender;
    TfraTopBar *fraTopBar1;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall TargetClick(TObject *Sender);
    void __fastcall ActionClick(TObject *Sender);
    void __fastcall MainPanelMininmizeClick(TObject *Sender);
    void __fastcall RenderResize(TObject *Sender);
    void __fastcall paRenderMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall paRenderMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall paRenderMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall QualityClick(TObject *Sender);
    void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall sbToolsMinClick(TObject *Sender);
    void __fastcall paInfoResize(TObject *Sender);
    void __fastcall sbClearClick(TObject *Sender);
    void __fastcall sbLoadClick(TObject *Sender);
    void __fastcall sbSaveClick(TObject *Sender);
    void __fastcall sbSaveAsClick(TObject *Sender);
    void __fastcall sbRunClick(TObject *Sender);
    void __fastcall sbOptionsClick(TObject *Sender);
    void __fastcall sbUpdateAllClick(TObject *Sender);
    void __fastcall sbMenuClick(TObject *Sender);
    void __fastcall sbCutClick(TObject *Sender);
    void __fastcall sbCopyClick(TObject *Sender);
    void __fastcall sbPasteClick(TObject *Sender);
    void __fastcall sbUndoClick(TObject *Sender);
    void __fastcall sbRedoClick(TObject *Sender);
    void __fastcall MainPanelMaximizeClick(TObject *Sender);
    void __fastcall Label1Click(TObject *Sender);
    void __fastcall sbObjectListClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tmRefreshTimer(TObject *Sender);
    void __fastcall ppRenderPaint(TObject *Sender);
    void __fastcall ClickDrawMenuItem(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, char &Key);
    void __fastcall miPropertiesClick(TObject *Sender);
    void __fastcall ebEditLibClick(TObject *Sender);
    void __fastcall miHideClick(TObject *Sender);
    void __fastcall ebDrawClick(TObject *Sender);
    void __fastcall ClickRenderMenuItem(TObject *Sender);
    void __fastcall ebRenderAddBtnClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall miCatPClipperClick(TObject *Sender);
private:	// User declarations
        void __fastcall DisplayHint(TObject *Sender);
        void __fastcall IdleHandler(TObject *Sender, bool &Done);
    TShiftState ShiftMouse;
    TShiftState ShiftKey;
    HINSTANCE m_HInstance;

    void __fastcall ApplyShortCut(WORD Key, TShiftState Shift);
    void UpdateCaption();
public:		// User declarations
    __fastcall TfrmMain(TComponent* Owner);
    __inline void SetHInst(HINSTANCE inst){ m_HInstance=inst; }
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;

void ShowObjectContextMenu(const int cls);
void ResetActionToSelect();
void AddToInfoBar(LPSTR s){ if (s){ frmMain->cbInfo->Items->Add(s); frmMain->cbInfo->Text=s;}}
//---------------------------------------------------------------------------
#endif
