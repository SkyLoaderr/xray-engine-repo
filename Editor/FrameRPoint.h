//---------------------------------------------------------------------------
#ifndef FrameRPointH
#define FrameRPointH
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
#include "mxPlacemnt.hpp"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraRPoint : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paCommands;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TMxLabel *RxLabel2;
	TMultiObjSpinEdit *seTeamID;
	TFormStorage *fsStorage;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraRPoint(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
