//---------------------------------------------------------------------------

#ifndef PropertiesDetailObjectH
#define PropertiesDetailObjectH
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
#include "DOShuffle.h"

#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"
#include "multi_color.hpp"
#include "multi_edit.hpp"


class TfrmPropertiesDO : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *GroupBox2;
	TRxLabel *RxLabel5;
	TRxLabel *RxLabel6;
	TMultiObjSpinEdit *seScaleMinY;
	TMultiObjSpinEdit *seScaleMaxY;
	TRxLabel *RxLabel1;
	TMultiObjSpinEdit *seDensityFactor;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
private:	// User declarations
    DDVec* m_Data;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesDO(TComponent* Owner);
    int __fastcall Run(DDVec& SrcData, bool& bChange);
};
//---------------------------------------------------------------------------
extern int frmPropertiesSectorRun(DDVec& SrcData, bool& bChange);
//---------------------------------------------------------------------------
#endif
