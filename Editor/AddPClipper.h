#ifndef AddPClipperH
#define AddPClipperH
#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include "CloseBtn.hpp"
#include "multiobj.hpp"
//---------------------------------------------------------------------------
class TfraAddPClipper : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
    TExtBtn *SpeedButton19;
    TLabel *Label4;
    TExtBtn *sbSnap;
    TPanel *paFixed;
    TLabel *Label6;
    TExtBtn *ExtBtn1;
    TLabel *Label5;
    TLabel *Label1;
    TLabel *Label3;
    TMultiObjSpinEdit *seSizeW;
    TMultiObjSpinEdit *seSizeH;
    TCheckBox *cbFixed;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall Label2Click(TObject *Sender);
    void __fastcall cbFixedClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfraAddPClipper(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraAddPClipper *fraAddPClipper;
//---------------------------------------------------------------------------
#endif
