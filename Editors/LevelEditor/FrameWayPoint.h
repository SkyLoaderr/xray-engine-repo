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
	TFormStorage *fsStorage;
	TExtBtn *ebRemoveSelected;
	TExtBtn *ebPointMode;
	TPanel *paLink;
	TLabel *Label1;
	TExtBtn *ExtBtn3;
	TExtBtn *ebInvertLink;
	TExtBtn *ebAdd1Link;
	TExtBtn *ebRemoveLinks;
	TExtBtn *ebAdd2Link;
	TBevel *Bevel2;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebAdd1LinksClick(TObject *Sender);
	void __fastcall ebRemoveLinksClick(TObject *Sender);
	void __fastcall ebAdd2LinkClick(TObject *Sender);
	void __fastcall ebInvertLinkClick(TObject *Sender);
	void __fastcall ebRemoveSelectedClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraWayPoint(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
