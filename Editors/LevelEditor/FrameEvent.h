//---------------------------------------------------------------------------
#ifndef FrameEventH
#define FrameEventH
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
class TfraEvent : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paCommands;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TExtBtn *ebAppendForm;
	TExtBtn *ebRemoveForm;
	TExtBtn *ebFormMode;
	TBevel *Bevel2;
	TFormStorage *fsStorage;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebAppendFormClick(TObject *Sender);
	void __fastcall ebRemoveFormClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraEvent(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
