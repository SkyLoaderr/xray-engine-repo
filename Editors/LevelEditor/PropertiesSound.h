//---------------------------------------------------------------------------

#ifndef PropertiesSoundH
#define PropertiesSoundH
//---------------------------------------------------------------------------
#include "ExtBtn.hpp"
#include "multi_edit.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>


//---------------------------------------------------------------------------
class TfrmPropertiesSound : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TPanel *Panel1;
    TButton *btApply;
    TButton *btOk;
    TButton *btCancel;
    TMultiObjSpinEdit *seRange;
	TLabel *RxLabel2;
    TBevel *Bevel2;
    TEdit *edName;
	TLabel *RxLabel1;
    TExtBtn *ebLink;
    TExtBtn *ebUnLink;
    void __fastcall btApplyClick(TObject *Sender);
    void __fastcall btOkClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebLinkClick(TObject *Sender);
    void __fastcall ebUnLinkClick(TObject *Sender);
private:	// User declarations
    list<CCustomObject*>* m_Objects;
    void GetObjectsInfo();
    void ApplyObjectsInfo();
    bool bMultipleSelection;
public:		// User declarations
    __fastcall TfrmPropertiesSound(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern void frmPropertiesSoundRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
