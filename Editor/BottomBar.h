//---------------------------------------------------------------------------


#ifndef BottomBarH
#define BottomBarH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CloseBtn.hpp"
#include "Placemnt.hpp"
#include "RxMenus.hpp"
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
	TRxPopupMenu *pmOptions;
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
	TMenuItem *miDrawSectors;
	TMenuItem *miDrawSectorEdgedCHull;
	TMenuItem *miDrawSectorCHull;
	TMenuItem *Portals1;
	TMenuItem *miDrawPortalSimpleModel;
	TPanel *paStatusLabel;
	TMenuItem *miDrawSectorFaces;
	TMenuItem *Animations1;
	TMenuItem *miDrawAnimateLight;
	TPanel *paCameraPos;
	TMenuItem *Objects1;
	TMenuItem *miDrawHiddenMeshes;
	TMenuItem *N5;
	TPanel *paButtons;
	TExtBtn *ebLog;
	TExtBtn *ebStop;
	TMenuItem *miDrawObjectBones;
	TMenuItem *miObjectLoopedAnimation;
	TMenuItem *miDrawSky;
	TMenuItem *miDrawObjectAnimPath;
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
	TMenuItem *miFog;
	TMenuItem *miRealTime;
	TMenuItem *N3;
	TExtBtn *ebStat;
	TPanel *paSel;
	TMenuItem *miDrawSectorEdgedSFaces;
    TPanel *paUICursor;
    TMenuItem *N1;
    TMenuItem *miShowHint;
	TCGauge *cgProgress;
	TMenuItem *DetailObjects1;
	TMenuItem *miDrawDOSlotBoxes;
	TMenuItem *miDrawDOBaseTexture;
	TMenuItem *miDODrawObjects;
    void __fastcall ClickOptionsMenuItem(TObject *Sender);
    void __fastcall QualityClick(TObject *Sender);
    void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall ebLogClick(TObject *Sender);
	void __fastcall ebStopClick(TObject *Sender);
	void __fastcall ebStatClick(TObject *Sender);
	void __fastcall ebOptionsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
public:
	float fStatusProgress;
    float fMaxVal;
public:		// User declarations
    __fastcall TfraBottomBar(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraBottomBar *fraBottomBar;
//---------------------------------------------------------------------------
#endif
