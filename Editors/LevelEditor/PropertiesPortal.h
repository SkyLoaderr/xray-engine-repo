//---------------------------------------------------------------------------

#ifndef PropertiesPortalH
#define PropertiesPortalH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"

#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"

//refs
class CPortal;

class TfrmPropertiesPortal : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *gbOrientation;
	TLabel *RxLabel5;
	TLabel *RxLabel1;
	TLabel *lbBackSector;
	TLabel *lbFrontSector;
	TExtBtn *ebInvertOrientation;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall ebInvertOrientationClick(TObject *Sender);
private:	// User declarations
    list<CCustomObject*>* m_Objects;
    CPortal* m_CurPortal;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesPortal(TComponent* Owner);
    int __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern void frmPropertiesPortalRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
