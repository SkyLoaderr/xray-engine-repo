//---------------------------------------------------------------------------

#ifndef PropertiesGlowH
#define PropertiesGlowH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"
#include "CustomObject.h"

#include "ExtBtn.hpp"
#include "multi_edit.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"

class CGlow;
//---------------------------------------------------------------------------
class TfrmPropertiesGlow : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
    TMultiObjSpinEdit *seRange;
    TExtBtn *ebApply;
	TLabel *RxLabel4;
    TPanel *paImage;
    TPaintBox *pbImage;
	TExtBtn *ebSelectShader;
	TLabel *lbShader;
	TLabel *lbTexture;
	TExtBtn *ebSelectTexture;
	TFormStorage *fsStorage;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall ebApplyClick(TObject *Sender);
    void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall OnModified(TObject *Sender);
	void __fastcall seRangeKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall seRangeLWChange(TObject *Sender, int Val);
	void __fastcall ebSelectShaderClick(TObject *Sender);
	void __fastcall ebSelectTextureClick(TObject *Sender);
private:	// User declarations
    CGlow* m_Glow;
    list<CCustomObject*>* m_Objects;
    void GetObjectsInfo();
    bool ApplyObjectsInfo();
public:		// User declarations
    __fastcall TfrmPropertiesGlow(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern void frmPropertiesGlowRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
