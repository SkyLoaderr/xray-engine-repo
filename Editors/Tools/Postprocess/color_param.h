//---------------------------------------------------------------------------

#ifndef color_paramH
#define color_paramH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include "RXSpin.hpp"
//---------------------------------------------------------------------------
class TForm7 : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TRxSpinEdit *WorkTime;
    TPanel *ColorPanel;
    TSpeedButton *AddButton;
    TSpeedButton *DeleteButton;
    TColorDialog *ColorDialog;
    TImageList *ImageList;
    void __fastcall ColorPanelClick(TObject *Sender);
    void __fastcall LinearFuncButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    int         m_iFunction;
    float       tr, cr, br;
    float       tg, cg, bg;
    float       tb, cb, bb;
    __fastcall TForm7(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm7 *Form7;
//---------------------------------------------------------------------------
#endif
