//---------------------------------------------------------------------------

#ifndef FrameDPatchH
#define FrameDPatchH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "CustomObject.h"
#include "ui_customtools.h"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
class ETextureCore;
//---------------------------------------------------------------------------
class TfraDPatch : public TFrame
{
__published:	// IDE-managed Components
    TMxLabel *RxLabel4;
	TPanel *paImage;
	TPaintBox *pbImage;
    TMxLabel *RxLabel1;
    TMxLabel *RxLabel3;
    TMxLabel *RxLabel5;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TMultiObjSpinEdit *seBrushDPatchAmount;
	TMultiObjSpinEdit *seDPatchMin;
	TMultiObjSpinEdit *seDPatchMax;
	TExtBtn *ebSelectShader;
	TMxLabel *lbDPShader;
	TExtBtn *ebSelectTexture;
	TMxLabel *lbDPTexture;
	TFormStorage *fsStorage;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
    void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall ebSelectShaderClick(TObject *Sender);
	void __fastcall ebSelectTextureClick(TObject *Sender);
private:	// User declarations
    friend class TUI_DPatchTools;
    friend class TUI_ControlDPatchAdd;
    void DrawTexture(ETextureCore* tex, TPaintBox* pb, TPanel* pa);
public:		// User declarations
    static AnsiString sTexture, sShader;
	__fastcall TfraDPatch(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
