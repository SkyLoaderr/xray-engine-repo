//---------------------------------------------------------------------------

#ifndef PropertiesSoundH
#define PropertiesSoundH
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
//---------------------------------------------------------------------------
class TfrmPropertiesSound : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TStaticText *StaticText1;
    TPanel *Panel1;
    TButton *btApply;
    TButton *btOk;
    TButton *btCancel;
    TMultiObjSpinEdit *seRange;
    void __fastcall btApplyClick(TObject *Sender);
    void __fastcall btOkClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
    list<SceneObject*>* m_Objects;
    void GetObjectsInfo();
    void ApplyObjectsInfo();
public:		// User declarations
    __fastcall TfrmPropertiesSound(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPropertiesSound *frmPropertiesSound;
//---------------------------------------------------------------------------
#endif
