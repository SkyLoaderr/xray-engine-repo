//---------------------------------------------------------------------------
#ifndef PropertiesRPointH
#define PropertiesRPointH
#include "CloseBtn.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "multi_check.hpp"
#include <StdCtrls.hpp>
#include "multi_edit.hpp"
#include "RXCtrls.hpp"
//---------------------------------------------------------------------------
#include "XRShaderDef.h"
#include "FrameEmitter.h"
#include <Forms.hpp>

// refs 
class SceneObject;

class TfrmPropertiesRPoint : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *GroupBox1;
	TRxLabel *RxLabel2;
	TMultiObjSpinEdit *seTeamID;
	TEdit *edName;
	TRxLabel *RxLabel1;
	TRxLabel *RxLabel3;
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
    list<SceneObject*>* m_Objects;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesRPoint(TComponent* Owner);
    static int __fastcall Run(list<SceneObject*>* pObjects, bool& bChange);
    static bool __fastcall Visible(){return !!form;}
    static TfrmPropertiesRPoint* GetForm(){VERIFY(form); return form;}
};
//---------------------------------------------------------------------------
#endif
