//---------------------------------------------------------------------------
#ifndef PropertiesSceneObjectH
#define PropertiesSceneObjectH
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "multi_check.hpp"
#include <StdCtrls.hpp>
#include "multi_edit.hpp"
//---------------------------------------------------------------------------
#include <Forms.hpp>
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"

// refs
class CCustomObject;

class TfrmPropertiesSceneObject : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *GroupBox1;
	TLabel *RxLabel2;
	TEdit *edName;
	TLabel *RxLabel1;
	TExtBtn *ebReference;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ebReferenceClick(TObject *Sender);
private:	// User declarations
	static TfrmPropertiesSceneObject* form;
    bool bLoadMode;
    list<CCustomObject*>* m_Objects;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
    AnsiString m_NewReference;
public:		// User declarations
    __fastcall TfrmPropertiesSceneObject(TComponent* Owner);
    static int __fastcall Run(list<CCustomObject*>* pObjects, bool& bChange);
    static bool __fastcall Visible(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
