//---------------------------------------------------------------------------


#ifndef RotatePClipperH
#define RotatePClipperH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
//---------------------------------------------------------------------------
class TfraRotatePClipper : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
    TExtBtn *SpeedButton19;
        TLabel *Label1;
    TExtBtn *sbAxisX;
    TExtBtn *sbAxisY;
        TLabel *Label5;
    TExtBtn *sbASnap;
    TExtBtn *sbAxisZ;
    TExtBtn *sbCSLocal;
    TLabel *Label3;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall Label2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraRotatePClipper(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraRotatePClipper *fraRotatePClipper;
//---------------------------------------------------------------------------
#endif
