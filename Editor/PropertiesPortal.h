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
#include "RXCtrls.hpp"
#include "RXSpin.hpp"

#include "SceneObject.h"
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"

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
	TRxLabel *RxLabel5;
	TRxLabel *RxLabel1;
	TRxLabel *lbBackSector;
	TRxLabel *lbFrontSector;
	TExtBtn *ebInvertOrientation;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall ebInvertOrientationClick(TObject *Sender);
private:	// User declarations
    list<SceneObject*>* m_Objects;
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
