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

#include "CustomObject.h"
#include "ExtBtn.hpp"
#include "multi_check.hpp"
#include "MXCtrls.hpp"

class TfrmProperties;

class TfrmPropertiesEvent : public TForm
{
__published:	// IDE-managed Components
	TPanel *paProps;
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall cbTargetClassChange(TObject *Sender);
	void __fastcall edOnEnterChange(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
    ObjectList* m_Objects;
	TfrmProperties* m_Props;
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
