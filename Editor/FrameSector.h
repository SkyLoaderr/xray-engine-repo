//---------------------------------------------------------------------------
#ifndef FrameSectorH
#define FrameSectorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
#include "RXCtrls.hpp"
#include "RxMenus.hpp"

#include "SceneObject.h"
#include "ui_customtools.h"
#include "multi_edit.hpp"
//---------------------------------------------------------------------------
class TfraSector : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paSectorActions;
	TExtBtn *ebAddFace;
	TExtBtn *ebDelFace;
    TExtBtn *ebFaceBoxPick;
	TRxLabel *RxLabel1;
	TRxLabel *RxLabel2;
	TExtBtn *ebUseTies;
	TRxLabel *RxLabel3;
	TMultiObjSpinEdit *seSoftAngle;
	TExtBtn *ebSoftRecursive;
	TPanel *paCommands;
	TRxLabel *RxLabel4;
	TRxLabel *RxLabel6;
	TRxLabel *lbFacesCount;
	TComboBox *cbItems;
	TExtBtn *ebValidate;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TLabel *APHeadLabel2;
	TExtBtn *ExtBtn1;
	TExtBtn *ebCreateNew;
	TExtBtn *ebCreateDefault;
	TExtBtn *ebRemoveDefault;
	TBevel *Bevel1;
	TExtBtn *ExtBtn3;
	TExtBtn *ebAutoUpdateConvex;
	TExtBtn *ebCaptureInside;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
	void __fastcall ebCreateNewClick(TObject *Sender);
	void __fastcall cbItemsChange(TObject *Sender);
	void __fastcall ebCaptureInsideVolumeClick(TObject *Sender);
	void __fastcall ebCreateDefaultClick(TObject *Sender);
	void __fastcall ebRemoveDefaultClick(TObject *Sender);
	void __fastcall ebValidateClick(TObject *Sender);
	void __fastcall ExtBtn3Click(TObject *Sender);
private:	// User declarations
	void __fastcall FindAndSelectSectorInList(const char* name);
public:		// User declarations
        __fastcall TfraSector(TComponent* Owner);
    void OnSectorUpdate();
    void OnChange();
};
//---------------------------------------------------------------------------
#endif
