//---------------------------------------------------------------------------


#ifndef SelectLightH
#define SelectLightH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
//---------------------------------------------------------------------------
class TfraSelectLight : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
    TExtBtn *SpeedButton19;
        TLabel *Label1;
    TExtBtn *sbInvert;
    TExtBtn *sbSelectAll;
    TExtBtn *sbDeselectAll;
        TPanel *Panel1;
        TLabel *Label4;
    TExtBtn *SpeedButton1;
    TExtBtn *sbProperties;
    TExtBtn *sbRemove;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall sbInvertClick(TObject *Sender);
    void __fastcall sbSelectAllClick(TObject *Sender);
    void __fastcall sbDeselectAllClick(TObject *Sender);
    void __fastcall sbRemoveClick(TObject *Sender);
    void __fastcall Label4Click(TObject *Sender);
    void __fastcall sbPropertiesClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraSelectLight(TComponent* Owner);
    void RunEditor       ();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraSelectLight *fraSelectLight;
//---------------------------------------------------------------------------
#endif
