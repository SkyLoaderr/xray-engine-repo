//---------------------------------------------------------------------------


#ifndef TopBarH
#define TopBarH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
//---------------------------------------------------------------------------
class TfraTopBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paTBEdit;
    TExtBtn *ebEditUndo;
    TExtBtn *ebEditRedo;
    TBevel *Bevel1;
    TBevel *Bevel4;
    TPanel *paTBAction;
    TExtBtn *ebActionMove;
    TExtBtn *ebActionRotate;
    TExtBtn *ebActionScale;
    TBevel *Bevel2;
    TExtBtn *ebActionSelect;
    TExtBtn *ebActionAdd;
    TPanel *paSnap;
    TExtBtn *ebSnap;
    TExtBtn *ebASnap;
    TExtBtn *ebCSLocal;
    TBevel *Bevel3;
    TPanel *paAxis;
    TExtBtn *ebAxisX;
    TExtBtn *ebAxisY;
    TExtBtn *ebAxisZ;
    TExtBtn *ebAxisZX;
    TBevel *Bevel5;
private:	// User declarations
public:		// User declarations
        __fastcall TfraTopBar(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraTopBar *fraTopBar;
//---------------------------------------------------------------------------
#endif
