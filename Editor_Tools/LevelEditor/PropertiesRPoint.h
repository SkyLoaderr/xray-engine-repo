//---------------------------------------------------------------------------
#ifndef PropertiesRPointH
#define PropertiesRPointH
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

class TfrmPropertiesRPoint : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *GroupBox1;
	TLabel *RxLabel2;
	TMultiObjSpinEdit *seTeamID;
	TEdit *edName;
	TLabel *RxLabel1;
	TLabel *RxLabel3;
	TMultiObjSpinEdit *seHeading;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
	static TfrmPropertiesRPoint* form;
    static bool bLoadMode;
    list<CCustomObject*>* m_Objects;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesRPoint(TComponent* Owner);
    static int __fastcall Run(list<CCustomObject*>* pObjects, bool& bChange);
    static bool __fastcall Visible(){return !!form;}
    static TfrmPropertiesRPoint* GetForm(){VERIFY(form); return form;}
};
//---------------------------------------------------------------------------
#endif
