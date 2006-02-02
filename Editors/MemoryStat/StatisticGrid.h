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
	TFormStorage *fsStorage;
	void __fastcall sgDataHeaderColumnClick(TObject *Sender,
          int SectionIndex);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	struct SStatHeader{
    	AnsiString 		caption;
        bool			is_text;
        SStatHeader(AnsiString c, bool t){caption=c; is_text=t;}
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
