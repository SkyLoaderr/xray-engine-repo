//---------------------------------------------------------------------------

#ifndef BuildProgressH
#define BuildProgressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmBuildProgress : public TForm
{
__published:	// IDE-managed Components
    TButton *btStop;
    TProgressBar *pb;
    TLabel *Label1;
    TLabel *lbStage;
private:	// User declarations
public:		// User declarations
    __fastcall TfrmBuildProgress(TComponent* Owner);
    void SetProgress(float v){ pb->Position = v; Repaint(); }
    void AddProgress(float dv){ pb->Position += dv; Repaint(); }
    void SetStage( char *stage ){ lbStage->Caption = stage; Repaint(); }
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmBuildProgress *frmBuildProgress;
//---------------------------------------------------------------------------
#endif
