//---------------------------------------------------------------------------

#ifndef PropertiesSectorH
#define PropertiesSectorH
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
#include "multi_color.hpp"
#include "MXCtrls.hpp"

class TfrmPropertiesSector : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *gbSector;
	TLabel *RxLabel5;
	TMultiObjColor *mcSectorColor;
	TBevel *Bevel2;
	TLabel *RxLabel1;
	TLabel *Label1;
	TEdit *edName;
	TLabel *lbContains;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall mcSectorColorMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations
    list<CCustomObject*>* m_Objects;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesSector(TComponent* Owner);
    int __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern int frmPropertiesSectorRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
