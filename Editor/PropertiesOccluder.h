//---------------------------------------------------------------------------

#ifndef PropertiesOccluderH
#define PropertiesOccluderH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include "RXSpin.hpp"
#include "SceneObject.h"
#include "multi_edit.hpp"
#include "CloseBtn.hpp"
#include "RXCtrls.hpp"
//---------------------------------------------------------------------------
class TfrmPropertiesOccluder : public TForm
{
__published:	// IDE-managed Components
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TGroupBox *gbOrientation;
	TMultiObjSpinEdit *sePointCount;
	TRxLabel *RxLabel4;
	TRxLabel *RxLabel1;
	TMultiObjSpinEdit *MultiObjSpinEdit1;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ebCancelClick(TObject *Sender);
	void __fastcall ebOkClick(TObject *Sender);
private:	// User declarations
    list<SceneObject*>* m_Objects;
    void GetObjectsInfo();
    void ApplyObjectsInfo();
public:		// User declarations
    __fastcall TfrmPropertiesOccluder(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern void frmPropertiesOccluderRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
