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

#include "ExtBtn.hpp"
#include "multi_check.hpp"
#include "MXCtrls.hpp"

class TfrmOneEventAction;

class TfrmPropertiesEvent : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TPanel *Panel3;
	TExtBtn *ebAppendAction;
	TExtBtn *ebMultiClear;
	TScrollBox *sbActions;
	TBevel *Bevel1;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall cbTargetClassChange(TObject *Sender);
	void __fastcall edOnEnterChange(TObject *Sender);
	void __fastcall ebAppendActionClick(TObject *Sender);
	void __fastcall ebMultiClearClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
    ObjectList* m_Objects;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
    DEFINE_VECTOR(TfrmOneEventAction*,ActionFormVec,ActionFormIt);
    ActionFormVec m_ActionForms;
public:		// User declarations
    __fastcall TfrmPropertiesEvent(TComponent* Owner);
    int __fastcall Run(ObjectList* pObjects, bool& bChange);
    void RemoveAction(TfrmOneEventAction* action);
};
//---------------------------------------------------------------------------
extern int frmPropertiesEventRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
