//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "StatisticGrid.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "ElTreeGrids"
#pragma link "ElXPThemedControl"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
TfrmStatistic *frmStatistic;
//---------------------------------------------------------------------------
__fastcall TfrmStatistic::TfrmStatistic(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmStatistic::sgDataHeaderColumnClick(
      TObject *Sender, int SectionIndex)
{
	TElSSortMode mode	 	= hsmDescend;
    for (int k=0; k<sgData->HeaderSections->Count; ++k){
        if (sgData->HeaderSections->Item[k]->SortMode!=hsmNone)
        	if (k!=SectionIndex)
	            mode	 	= sgData->HeaderSections->Item[k]->SortMode;
            else
                if (sgData->HeaderSections->Item[k]->SortMode==hsmAscend)	mode = hsmDescend; 
                else														mode = hsmAscend; 
        if (k!=SectionIndex)
            sgData->HeaderSections->Item[k]->SortMode = hsmNone;
    }
    sgData->HeaderSections->Item[SectionIndex]->SortMode = mode; 

	sgData->SortSection = SectionIndex;
	sgData->Sort		(true);
}
//---------------------------------------------------------------------------

void TfrmStatistic::ClearItems()
{
	sgData->Items->Clear	();
}
//---------------------------------------------------------------------------

void TfrmStatistic::AppendItem(u32 count, u32 mem_pure, u32 mem_alloc, LPCSTR name)
{
    TElTreeItem* item		= sgData->Items->AddChildObject(0,AnsiString().sprintf("%d",count),0);	
    item->ColumnText->Add	(AnsiString().sprintf("%d",mem_pure));
    item->ColumnText->Add	(AnsiString().sprintf("%d",mem_alloc));
    item->ColumnText->Add	(name);
}
//---------------------------------------------------------------------------

void __fastcall TfrmStatistic::FormShow(TObject *Sender)
{
    sbStatus->Panels->Items[0]->Text = AnsiString().sprintf("Items count: %d",sgData->Items->Count);
    SortByColumn				(2,false);
}
//---------------------------------------------------------------------------

void TfrmStatistic::SortByColumn(int num, bool accend)
{
	if (sgData->HeaderSections->Count==0) return;
    for (int k=0; k<sgData->HeaderSections->Count; ++k)
    	sgData->HeaderSections->Item[num]->SortMode = hsmNone; 
	clamp						(num,0,sgData->HeaderSections->Count-1);
    sgData->HeaderSections->Item[num]->SortMode 	= accend?hsmAccend:hsmDescend; 
	sgData->SortSection 		= num;
	sgData->Sort				(true);
}
//---------------------------------------------------------------------------


