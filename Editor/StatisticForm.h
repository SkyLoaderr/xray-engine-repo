//---------------------------------------------------------------------------

#ifndef StatisticFormH
#define StatisticFormH
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
#include "RxMenus.hpp"
#include <Menus.hpp>
#include "Placemnt.hpp"

class TfrmStatistic : public TForm
{
__published:	// IDE-managed Components
	TLabel *lbRLight;
	TLabel *lbTLight;
	TLabel *lbPoly;
	TLabel *lbTotalFrame;
	TLabel *lbUndoCount;
	TRxLabel *RxLabel5;
	TRxLabel *RxLabel4;
	TRxLabel *RxLabel3;
	TRxLabel *RxLabel1;
	TRxLabel *RxLabel2;
	TFormStorage *fsStatistic;
	TBevel *Bevel1;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
	static TfrmStatistic* frmStatistic;
public:		// User declarations
    __fastcall TfrmStatistic(TComponent* Owner);
    static void __fastcall ShowStatistic();
    static void __fastcall HideStatistic();
    static bool __fastcall Visible(){return !!frmStatistic;}
    static TfrmStatistic* GetForm(){VERIFY(frmStatistic); return frmStatistic;}
};
//---------------------------------------------------------------------------
#endif
