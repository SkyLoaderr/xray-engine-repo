//---------------------------------------------------------------------------


#ifndef ScalePClipperH
#define ScalePClipperH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
//---------------------------------------------------------------------------
class TfraScalePClipper : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
    TExtBtn *SpeedButton19;
        TLabel *Label1;
    TExtBtn *sbAxisW;
    TExtBtn *sbAxisH;
    TExtBtn *sbCSLocal;
    TLabel *Label3;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall Label2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraScalePClipper(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraScalePClipper *fraScalePClipper;
//---------------------------------------------------------------------------
#endif
