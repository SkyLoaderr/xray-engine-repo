//---------------------------------------------------------------------------


#ifndef RotateObjectH
#define RotateObjectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
//---------------------------------------------------------------------------
class TfraRotateObject : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
    TExtBtn *SpeedButton19;
        TLabel *Label1;
        TLabel *Label5;
    TExtBtn *sbASnap;
    TExtBtn *sbCSLocal;
    TLabel *Label3;
    TExtBtn *sbAxisX;
    TExtBtn *sbAxisY;
    TExtBtn *sbAxisZ;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall Label2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraRotateObject(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraRotateObject *fraRotateObject;
//---------------------------------------------------------------------------
#endif
