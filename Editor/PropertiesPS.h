//---------------------------------------------------------------------------
#ifndef PropertiesPSH
#define PropertiesPSH
//---------------------------------------------------------------------------
#include "FrameEmitter.h"
#include <Forms.hpp>
#include "ExtBtn.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>

// refs
class CCustomObject;

class TfrmPropertiesPS : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *GroupBox2;
	TfraEmitter *fraEmitter;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ebBirthFuncMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall seEmitterKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall seEmitterLWChange(TObject *Sender, int Val);
private:	// User declarations
	static TfrmPropertiesPS* form;
	static bool bLoadMode;

    WaveForm m_BirthFunc;

    list<CCustomObject*>* m_Objects;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesPS(TComponent* Owner);
    static int __fastcall Run(list<CCustomObject*>* pObjects, bool& bChange);
    static bool __fastcall Visible(){return !!form;}
    static TfrmPropertiesPS* GetForm(){VERIFY(form); return form;}
};
//---------------------------------------------------------------------------
#endif
