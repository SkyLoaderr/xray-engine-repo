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
	TExtBtn *ebCompute;
	TComboBox *cbSectorFront;
	TComboBox *cbSectorBack;
	TLabel *RxLabel1;
	TLabel *RxLabel2;
	TExtBtn *ebPickSectors;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TLabel *APHeadLabel2;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn3;
	TExtBtn *ebComputeAllPortals;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
	void __fastcall ebComputeClick(TObject *Sender);
	void __fastcall cbSectorChange(TObject *Sender);
	void __fastcall ebPickSectorsClick(TObject *Sender);
	void __fastcall ebComputeAllPortalsClick(TObject *Sender);
	void __fastcall ExtBtn3Click(TObject *Sender);
private:	// User declarations
	int iPickSectors;
public:		// User declarations
        __fastcall TfraPortal(TComponent* Owner);
    void SetSector(CSector* obj);
    void ShowCompute(bool bVisible);
};
//---------------------------------------------------------------------------
#endif
