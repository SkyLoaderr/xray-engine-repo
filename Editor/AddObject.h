//---------------------------------------------------------------------------


#ifndef AddObjectH
#define AddObjectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
#include "multiobj.hpp"
#include "RXCtrls.hpp"
#include "RXSpin.hpp"
//---------------------------------------------------------------------------
class TfraAddObject : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
    TExtBtn *SpeedButton19;
        TPanel *Panel1;
        TLabel *Label4;
    TExtBtn *SpeedButton1;
    TLabel *lbSelectObjectName;
    TLabel *Label3;
    TLabel *Label5;
    TExtBtn *sbSnap;
    TExtBtn *sbQSnap;
    TExtBtn *sbReference;
    TExtBtn *ExtBtn1;
    TPanel *paBrush;
    TLabel *Label1;
    TExtBtn *ExtBtn2;
    TMultiObjSpinEdit *seBrushWidth;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel3;
    TMultiObjSpinEdit *seBrushHeight;
    TExtBtn *ebRandomPlace;
    TRxLabel *RxLabel4;
    TMultiObjSpinEdit *seBrushObjectCount;
    TMultiObjCheck *cbUseBrush;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall sbSelectClick(TObject *Sender);
    void __fastcall Label2Click(TObject *Sender);
    void __fastcall cbUseBrushClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraAddObject(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraAddObject *fraAddObject;
//---------------------------------------------------------------------------
#endif
