//---------------------------------------------------------------------------


#ifndef BottomBarH
#define BottomBarH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include "MxMenus.hpp"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "CGAUGES.h"
//---------------------------------------------------------------------------
class TfraBottomBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paBottomBar;
    TPanel *paInfo;
    TPanel *paMenu;
	TExtBtn *ebOptions;
	TFormStorage *fsStorage;
	TMxPopupMenu *pmOptions;
    TMenuItem *Quality1;
	TMenuItem *N25;
	TMenuItem *N50;
	TMenuItem *N75;
	TMenuItem *N100;
	TMenuItem *N125;
	TMenuItem *N150;
	TMenuItem *N200;
    TMenuItem *N2;
    TMenuItem *miDrawGrid;
    TMenuItem *N4;
    TPanel *paStatus;
	TPanel *paStatusLabel;
	TPanel *paCamera;
	TMenuItem *Objects1;
	TPanel *paButtons;
	TExtBtn *ebLog;
	TMenuItem *miDrawObjectBones;
	TMenuItem *Render1;
	TMenuItem *FillMode1;
	TMenuItem *miRenderFillSolid;
	TMenuItem *miRenderFillWireframe;
	TMenuItem *miRenderFillPoint;
	TMenuItem *ShadeMode1;
	TMenuItem *miRenderShadeGouraud;
	TMenuItem *miRenderShadeFlat;
	TMenuItem *miRenderEdgedFaces;
	TMenuItem *miRenderHWTransform;
	TMenuItem *miRenderLinearFilter;
	TMenuItem *miRenderWithTextures;
	TMenuItem *N7;
	TMenuItem *N8;
	TMenuItem *miLightScene;
	TMenuItem *miRealTime;
	TMenuItem *N3;
	TExtBtn *ebStat;
    TPanel *paUICursor;
	TCGauge *cgProgress;
	TPanel *paGridSquareSize;
	TMenuItem *miShowHint;
	TMenuItem *N1;
	TMenuItem *miDrawObjectsPivot;
    void __fastcall ClickOptionsMenuItem(TObject *Sender);
    void __fastcall QualityClick(TObject *Sender);
    void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall ebLogClick(TObject *Sender);
	void __fastcall ebStatClick(TObject *Sender);
	void __fastcall ebOptionsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
public:
	float fStatusProgress;
    float fMaxVal;
    AnsiString sProgressTitle;
public:		// User declarations
    __fastcall TfraBottomBar(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraBottomBar *fraBottomBar;
//---------------------------------------------------------------------------
#endif
