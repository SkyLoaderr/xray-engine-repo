//---------------------------------------------------------------------------


#ifndef SelectObjectH
#define SelectObjectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
//---------------------------------------------------------------------------
class TfraSelectObject : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
    TExtBtn *SpeedButton19;
        TLabel *Label1;
    TExtBtn *sbInvert;
    TExtBtn *sbSelectAll;
    TExtBtn *sbDeselectAll;
        TLabel *Label3;
    TExtBtn *sbSelectByClass;
    TExtBtn *sbDeselectByClass;
        TPanel *Panel1;
        TLabel *Label4;
    TExtBtn *SpeedButton1;
    TExtBtn *sbResolve;
    TExtBtn *sbProperties;
    TExtBtn *sbRemove;
    TLabel *Label5;
    TExtBtn *sbSelectByRefs;
    TExtBtn *sbDeselectByRefs;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall sbInvertClick(TObject *Sender);
    void __fastcall sbSelectAllClick(TObject *Sender);
    void __fastcall sbDeselectAllClick(TObject *Sender);
    void __fastcall sbSelectByClassClick(TObject *Sender);
    void __fastcall sbDeselectByClassClick(TObject *Sender);
    void __fastcall sbSelectByRefsClick(TObject *Sender);
    void __fastcall sbDeselectByRefsClick(TObject *Sender);
    void __fastcall sbResolveClick(TObject *Sender);
    void __fastcall sbPropertiesClick(TObject *Sender);
    void __fastcall sbRemoveClick(TObject *Sender);
    void __fastcall Label2Click(TObject *Sender);
private:	// User declarations
    void SelByRefObject  ( bool flag );
    void SelByClassObject( bool flag );
    void ResolveObjects  ();
public:		// User declarations
        __fastcall TfraSelectObject(TComponent* Owner);
    void RunEditor       ();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraSelectObject *fraSelectObject;
//---------------------------------------------------------------------------
#endif
