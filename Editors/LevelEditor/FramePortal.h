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

#include "ui_customtools.h"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
//refs
class CSector;
//---------------------------------------------------------------------------
class TfraPortal : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paCommands;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TExtBtn *ebInvertOrient;
	TExtBtn *ebComputeAllPortals;
	TExtBtn *ebComputeSelPortals;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
	void __fastcall ebComputeClick(TObject *Sender);
	void __fastcall ebComputeAllPortalsClick(TObject *Sender);
	void __fastcall ebInvertOrientClick(TObject *Sender);
private:	// User declarations
	int iPickSectors;
public:		// User declarations
        __fastcall TfraPortal(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
