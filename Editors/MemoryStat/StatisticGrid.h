//---------------------------------------------------------------------------

#ifndef StatisticGridH
#define StatisticGridH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ElTree.hpp"
#include "ElTreeGrids.hpp"
#include "ElXPThemedControl.hpp"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "mxPlacemnt.hpp"
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TfrmStatistic : public TForm
{
__published:	// IDE-managed Components
	TElTreeStringGrid *sgData;
	TPanel *paCaptionMem;
	TStatusBar *sbStatus;
	TFormStorage *FormStorage1;
	void __fastcall sgDataHeaderColumnClick(TObject *Sender,
          int SectionIndex);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmStatistic(TComponent* Owner);
    void ClearItems();
    void AppendItem(u32 count, u32 mem_pure, u32 mem_alloc, LPCSTR name);
    void SortByColumn(int num, bool accend);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmStatistic *frmStatistic;
//---------------------------------------------------------------------------
#endif
