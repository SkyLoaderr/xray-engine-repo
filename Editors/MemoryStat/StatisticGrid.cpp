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

void TfrmStatistic::Prepare(AnsiString title, SHVec& columns)
{
    Hide						();
	Caption						= title;
	sgData->Items->Clear		();
	fsStorage->IniSection		= title;
	sgData->HeaderSections->Clear();
    for (SHVecIt it=columns.begin(); it!=columns.end(); it++){
	    TElHeaderSection* sect 	= sgData->HeaderSections->AddSection();
        sect->Text 				= it->caption;
        sect->FieldType			= it->is_text?sftText:sftNumber;
    };
}
//---------------------------------------------------------------------------

void TfrmStatistic::AppendItem(AStringVec& columns)
{
	VERIFY					(columns.size()==sgData->HeaderSections->Count);
    TElTreeItem* item		= sgData->Items->AddChildObject(0,columns[0],0);	
    for (int k=1; k<columns.size(); ++k)
	    item->ColumnText->Add(columns[k]);
}
//---------------------------------------------------------------------------

void __fastcall TfrmStatistic::FormShow(TObject *Sender)
{
    sbStatus->Panels->Items[0]->Text = AnsiString().sprintf("Items count: %d",sgData->Items->Count);
}
//---------------------------------------------------------------------------

void TfrmStatistic::SortByColumn(int num, bool ascend)
{
	if (sgData->HeaderSections->Count==0) return;
    for (int k=0; k<sgData->HeaderSections->Count; ++k)
    	sgData->HeaderSections->Item[num]->SortMode = hsmNone; 
	clamp						(num,0,sgData->HeaderSections->Count-1);
    sgData->HeaderSections->Item[num]->SortMode 	= ascend?hsmAscend:hsmDescend; 
	sgData->SortSection 		= num;
	sgData->Sort				(true);
}
//---------------------------------------------------------------------------

