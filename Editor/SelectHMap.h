//---------------------------------------------------------------------------


#ifndef SelectHMapH
#define SelectHMapH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include "RXSpin.hpp"
//---------------------------------------------------------------------------
class TfraSelectHMap : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
        TSpeedButton *SpeedButton19;
    TRxSpinEdit *seMCenterX;
    TLabel *Label1;
    TRxSpinEdit *seMCenterY;
    TRxSpinEdit *seMCenterZ;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *Label8;
    TLabel *Label9;
    TRxSpinEdit *seMDimAlt;
    TLabel *Label11;
    TLabel *Label14;
    TLabel *Label16;
    TRxSpinEdit *seMSlotSize;
    TLabel *Label17;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall seMCenterXChange(TObject *Sender);
    void __fastcall seMCenterYChange(TObject *Sender);
    void __fastcall seMCenterZChange(TObject *Sender);
    void __fastcall seMDimAltChange(TObject *Sender);
    void __fastcall seMSlotSizeChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraSelectHMap(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraSelectHMap *fraSelectHMap;
//---------------------------------------------------------------------------
#endif
