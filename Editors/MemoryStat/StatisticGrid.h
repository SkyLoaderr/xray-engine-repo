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
	TFormStorage *fsStorage;
	TPanel *Panel1;
	TPanel *paQuickSearchEdit;
	TPanel *paQuickSearchCaption;
	TEdit *edQuickSearch;
	TLabel *lbQuickSearchCaption;
	TPanel *paItemsCount;
	void __fastcall sgDataHeaderColumnClick(TObject *Sender,
          int SectionIndex);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall paQuickSearchEditResize(TObject *Sender);
	void __fastcall edQuickSearchKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
public:		// User declarations
	struct SStatHeader{
    	enum EColumnType{
        	sctText,
            sctInteger,
            sctFloat
        };
    	AnsiString 		caption;
        EColumnType		type;
        SStatHeader(AnsiString c, EColumnType t){caption=c; type=t;}
    };
    DEFINE_VECTOR(SStatHeader,SHVec,SHVecIt);
public:		// User declarations
    void Prepare		(AnsiString title, SHVec& columns);
    void AppendItem		(AStringVec& columns);
    void SortByColumn	(int num, bool ascend);
public:		// User declarations
	__fastcall TfrmStatistic(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmStatistic *frmStatistic;
//---------------------------------------------------------------------------
#endif
