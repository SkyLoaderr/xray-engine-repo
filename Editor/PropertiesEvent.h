//---------------------------------------------------------------------------

#ifndef PropertiesEventH
#define PropertiesEventH
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
#include "multi_check.hpp"

class TfrmPropertiesEvent : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *gbEventParams;
	TRadioGroup *rgType;
	TRxLabel *RxLabel5;
	TComboBox *cbTargetClass;
	TRxLabel *RxLabel1;
	TRxLabel *RxLabel2;
	TEdit *edOnExit;
	TEdit *edOnEnter;
	TRxLabel *RxLabel3;
	TMultiObjCheck *cbExecuteOnce;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall cbTargetClassChange(TObject *Sender);
	void __fastcall edOnEnterChange(TObject *Sender);
private:	// User declarations
    list<SceneObject*>* m_Objects;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesEvent(TComponent* Owner);
    int __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern int frmPropertiesEventRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
