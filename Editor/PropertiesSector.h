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
#include "RXCtrls.hpp"
#include "RXSpin.hpp"

#include "SceneObject.h"
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"
#include "multi_color.hpp"

class TfrmPropertiesSector : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *gbSector;
	TRxLabel *RxLabel5;
	TMultiObjColor *mcSectorColor;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall mcSectorColorMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations
    list<SceneObject*>* m_Objects;
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
