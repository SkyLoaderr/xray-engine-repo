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
#include "RXCtrls.hpp"
#include "RXSpin.hpp"

#include "SceneObject.h"
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"
#include "Placemnt.hpp"
#include "multi_edit.hpp"

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
    TRxLabel *RxLabel4;
    TPanel *paImage;
    TPaintBox *pbImage;
	TExtBtn *ebSelectShader;
	TRxLabel *lbShader;
	TRxLabel *lbTexture;
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
    list<SceneObject*>* m_Objects;
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
