//---------------------------------------------------------------------------
#ifndef KeyBarH
#define KeyBarH
#include "Gradient.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "ElTrackBar.hpp"
#include "ElXPThemedControl.hpp"
#include <StdCtrls.hpp>
#include "MXCtrls.hpp"
#include "multi_edit.hpp"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
//refs
class TfrmKeyBar : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *Panel3;
	TStaticText *stStartTime;
	TPanel *Panel4;
	TPanel *Panel5;
	TStaticText *stEndTime;
	TLabel *Label1;
	TMultiObjSpinEdit *seLOD;
	TBevel *Bevel1;
	TGradient *Gradient1;
	TMxLabel *lbCurrentTime;
	void __fastcall seLODLWChange(TObject *Sender, int Val);
	void __fastcall seLODKeyPress(TObject *Sender, char &Key);
	void __fastcall seLODExit(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfrmKeyBar(TComponent* Owner);
    static TfrmKeyBar* CreateKeyBar(TWinControl* parent);
    void UpdateBar();
};
//---------------------------------------------------------------------------
#endif
