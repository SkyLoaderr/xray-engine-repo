//---------------------------------------------------------------------------


#ifndef AddHMapH
#define AddHMapH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
//---------------------------------------------------------------------------
class TfraAddHMap : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
        TSpeedButton *SpeedButton19;
    TSpeedButton *sbLoad;
    TLabel *lbName;
    TOpenPictureDialog *opd;
    TImage *imHMap;
    TBevel *Bevel1;
    TLabel *lbSize;
    TSpeedButton *sbClear;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall sbLoadClick(TObject *Sender);
    void __fastcall sbClearClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraAddHMap(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraAddHMap *fraAddHMap;
//---------------------------------------------------------------------------
#endif
