//---------------------------------------------------------------------------
#ifndef FrameWayPointH
#define FrameWayPointH
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
#include "mxPlacemnt.hpp"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraWayPoint : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paCommands;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TExtBtn *ebAutoLink;
	TExtBtn *ebAddLinks;
	TBevel *Bevel1;
	TExtBtn *ebRemoveLinks;
	TFormStorage *fsStorage;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebAddLinksClick(TObject *Sender);
	void __fastcall ebRemoveLinksClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraWayPoint(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
