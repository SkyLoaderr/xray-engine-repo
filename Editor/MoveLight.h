//---------------------------------------------------------------------------


#ifndef MoveLightH
#define MoveLightH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"

//---------------------------------------------------------------------------
class TfraMoveLight : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
    TExtBtn *SpeedButton19;
        TLabel *Label1;
        TLabel *Label5;
    TExtBtn *sbSnap;
    TExtBtn *sbLockX;
    TExtBtn *sbLockY;
    TExtBtn *sbLockZ;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall Label2Click(TObject *Sender);
    void __fastcall sbLockClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraMoveLight(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraMoveLight *fraMoveLight;
//---------------------------------------------------------------------------
#endif
