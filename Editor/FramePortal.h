//---------------------------------------------------------------------------
#ifndef FramePortalH
#define FramePortalH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "CustomObject.h"
#include "ui_customtools.h"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
//refs
class CSector;
//---------------------------------------------------------------------------
class TfraPortal : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paCompute;
	TPanel *paCommands;
	TMxLabel *RxLabel4;
	TComboBox *cbItems;
	TExtBtn *ebCompute;
	TComboBox *cbSectorFront;
	TComboBox *cbSectorBack;
	TMxLabel *RxLabel1;
	TMxLabel *RxLabel2;
	TExtBtn *ebPickSectors;
	TExtBtn *ebComputeAllPortals;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TLabel *APHeadLabel2;
	TExtBtn *ExtBtn1;
	TMxLabel *RxLabel5;
	TMxLabel *RxLabel3;
	TMxLabel *lbFrontSector;
	TMxLabel *lbBackSector;
	TExtBtn *ExtBtn3;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
	void __fastcall ebCreateNewClick(TObject *Sender);
	void __fastcall cbItemsChange(TObject *Sender);
	void __fastcall ebComputeClick(TObject *Sender);
	void __fastcall cbSectorChange(TObject *Sender);
	void __fastcall ebPickSectorsClick(TObject *Sender);
	void __fastcall ebComputeAllPortalsClick(TObject *Sender);
	void __fastcall ExtBtn3Click(TObject *Sender);
private:	// User declarations
	int iPickSectors;
	void __fastcall FindAndSelectPortalInList(const char* name);
public:		// User declarations
        __fastcall TfraPortal(TComponent* Owner);
    void OnChange();
    void SetSector(CSector* obj);
    void ShowCompute(bool bVisible);
};
//---------------------------------------------------------------------------
#endif
