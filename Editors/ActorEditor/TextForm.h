//---------------------------------------------------------------------------
#ifndef TextFormH
#define TextFormH
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "multi_check.hpp"
#include <StdCtrls.hpp>
#include "multi_edit.hpp"
//---------------------------------------------------------------------------
#include <Forms.hpp>
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
#include "ElACtrls.hpp"

// refs
class CCustomObject;

class TfrmText : public TForm
{
__published:	// IDE-managed Components
	TPanel *paBottomBar;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TFormStorage *fsStorage;
	TElAdvancedMemo *mmText;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
	void __fastcall mmTextChange(TObject *Sender);
private:	// User declarations
	static TfrmText*  form;
public:		// User declarations
    __fastcall TfrmText(TComponent* Owner);
    static bool __fastcall Run(AnsiString& text, LPCSTR caption="Text", bool bReadOnly=false, int lim=0);
};
//---------------------------------------------------------------------------
#endif
