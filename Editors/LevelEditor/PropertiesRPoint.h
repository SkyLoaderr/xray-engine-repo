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

#include "RPoint.h"

// refs
class CCustomObject;

class TfrmPropertiesRPoint : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *GroupBox1;
	TEdit *edName;
	TLabel *RxLabel1;
	TPanel *paEntity;
	TLabel *Label3;
	TExtBtn *ebEntityRefs;
	TMultiObjCheck *cbActive;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ebEntityRefsClick(TObject *Sender);
	void __fastcall ebTypeClick(TObject *Sender);
private:	// User declarations
	static TfrmPropertiesRPoint* form;
    static bool bLoadMode;
    list<CCustomObject*>* m_Objects;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
    bool bMultiSel;
    bool bTypeChanged;
    bool bEntityChanged;
    void __fastcall SetType(CRPoint::EType type);
public:		// User declarations
    __fastcall TfrmPropertiesRPoint(TComponent* Owner);
    static int __fastcall Run(list<CCustomObject*>* pObjects, bool& bChange);
    static bool __fastcall Visible(){return !!form;}
    static TfrmPropertiesRPoint* GetForm(){VERIFY(form); return form;}
};
//---------------------------------------------------------------------------
#endif
