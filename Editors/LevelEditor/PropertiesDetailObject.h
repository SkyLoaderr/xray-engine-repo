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

#include "DOShuffle.h"

#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"
#include "multi_color.hpp"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "multi_check.hpp"


class TfrmPropertiesDO : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *GroupBox2;
	TLabel *RxLabel5;
	TLabel *RxLabel6;
	TMultiObjSpinEdit *seScaleMinY;
	TMultiObjSpinEdit *seScaleMaxY;
	TLabel *RxLabel1;
	TMultiObjSpinEdit *seDensityFactor;
	TMultiObjCheck *cbNoWaving;
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
