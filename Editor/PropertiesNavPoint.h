//---------------------------------------------------------------------------

#ifndef PropertiesNavPointH
#define PropertiesNavPointH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include "RXSpin.hpp"
#include "multiobj.hpp"
#include "SceneObject.h"
#include "RXCtrls.hpp"
#include "CloseBtn.hpp"
//---------------------------------------------------------------------------
class TfrmPropertiesNavPoint : public TForm
{
__published:	// IDE-managed Components
    TButton *btApply;
    TButton *btOk;
    TButton *btCancel;
    TGroupBox *GroupBox1;
    TMultiObjCheck *cbTypeStandart;
    TMultiObjCheck *cbTypeItem;
    void __fastcall btApplyClick(TObject *Sender);
    void __fastcall btOkClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall cbTypeStandartClick(TObject *Sender);
    void __fastcall cbTypeItemClick(TObject *Sender);
private:	// User declarations
    list<SceneObject*>* m_Objects;
    void GetObjectsInfo();
    void ApplyObjectsInfo();
public:		// User declarations
    __fastcall TfrmPropertiesNavPoint(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPropertiesNavPoint *frmPropertiesNavPoint;
//---------------------------------------------------------------------------
#endif
