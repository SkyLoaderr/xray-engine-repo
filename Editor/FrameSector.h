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

#include "CustomObject.h"
#include "ui_customtools.h"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
//---------------------------------------------------------------------------
class TfraSector : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paSectorActions;
	TExtBtn *ebAddMesh;
	TExtBtn *ebDelMesh;
    TExtBtn *ebFaceBoxPick;
	TLabel *RxLabel1;
	TPanel *paCommands;
	TLabel *RxLabel4;
	TLabel *RxLabel6;
	TLabel *lbFacesCount;
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
