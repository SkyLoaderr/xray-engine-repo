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

#include "ui_customtools.h"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraSpawnPoint : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paCurrent;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TFormStorage *fsStorage;
	TExtBtn *ebCurObj;
	TPanel *paCommands;
	TLabel *Label1;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn3;
	TExtBtn *ExtBtn4;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebCurObjClick(TObject *Sender);
	void __fastcall ExtBtn3Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraSpawnPoint(TComponent* Owner);
    AnsiString GetCurrentEntity(BOOL bForceSelect=FALSE);
};
//---------------------------------------------------------------------------
#endif
