//---------------------------------------------------------------------------
#ifndef FrameDetObjH
#define FrameDetObjH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "CustomObject.h"
#include "ui_customtools.h"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include "multi_color.hpp"
// refs
struct SDOClusterDef;
//---------------------------------------------------------------------------
class TfraDetailObject : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paSelect;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TFormStorage *fsStorage;
	TPanel *paCommand;
	TLabel *Label1;
	TExtBtn *ExtBtn3;
	TExtBtn *ExtBtn1;
	TExtBtn *ebReinitializeSlotObjects;
	TExtBtn *ExtBtn6;
	TExtBtn *ExtBtn4;
	TLabel *Label2;
	TMultiObjSpinEdit *seDensity;
	TBevel *Bevel2;
	TExtBtn *ebResetSlots;
	TExtBtn *ExtBtn7;
	TExtBtn *ebClearDetails;
	TPanel *paBaseTexture;
	TLabel *Label3;
	TExtBtn *ExtBtn8;
	TExtBtn *ebBaseBlended;
	TLabel *RxLabel16;
	TMultiObjColor *mcBrushColor;
	TLabel *Label4;
	TMultiObjSpinEdit *seBrushSize;
	TExtBtn *ebPaintMode;
	TBevel *Bevel1;
	TPanel *paObjects;
	TLabel *Label5;
	TExtBtn *ExtBtn9;
	TExtBtn *ExtBtn5;
	TExtBtn *ebExportColorIndices;
	TExtBtn *ebImportColorIndices;
	TBevel *Bevel3;
	TBevel *Bevel4;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
	void __fastcall ExtBtn5Click(TObject *Sender);
	void __fastcall ebUpdateObjectsClick(TObject *Sender);
	void __fastcall ebReinitializeSlotObjectsClick(TObject *Sender);
	void __fastcall ExtBtn6Click(TObject *Sender);
	void __fastcall seDensityExit(TObject *Sender);
	void __fastcall seDensityKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall seDensityLWChange(TObject *Sender, int Val);
	void __fastcall ebInvalidateCacheClick(TObject *Sender);
	void __fastcall ebResetSlotsClick(TObject *Sender);
	void __fastcall ebClearDetailsClick(TObject *Sender);
	void __fastcall ebBaseBlendedClick(TObject *Sender);
	void __fastcall ebImportColorIndicesClick(TObject *Sender);
	void __fastcall ebExportColorIndicesClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraDetailObject(TComponent* Owner);
    void __fastcall OnEnter();
    void __fastcall OnExit();
};
//---------------------------------------------------------------------------
#endif
