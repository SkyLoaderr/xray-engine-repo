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
#include "CloseBtn.hpp"
#include "RXCtrls.hpp"

#include "SceneObject.h"
#include "ui_customtools.h"
#include "multi_edit.hpp"
#include "Placemnt.hpp"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraRPoint : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paCommands;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TRxLabel *RxLabel2;
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
