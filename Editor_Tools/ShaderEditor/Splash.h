//---------------------------------------------------------------------------

#ifndef SplashH
#define SplashH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "RXCtrls.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TfrmSplash : public TForm
{
__published:	// IDE-managed Components
    TImage *Image1;
    TRxLabel *lbStatus;
private:	// User declarations
public:		// User declarations
    __fastcall TfrmSplash(TComponent* Owner);
    void SetStatus(LPSTR log){ if (log){ lbStatus->Caption = log; lbStatus->Repaint(); };}
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSplash *frmSplash;
//---------------------------------------------------------------------------
#endif
