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

	sgData->SortSection 			= SectionIndex;
	sgData->Sort					(true);
    
    lbQuickSearchCaption->Caption 	= AnsiString().sprintf(" Search by \"%s\":",AnsiString(sgData->HeaderSections->Item[SectionIndex]->Text).c_str());
    paQuickSearchCaption->Width		= lbQuickSearchCaption->Width+6;
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
        switch (it->type){
        case SStatHeader::sctText: 		sect->FieldType = sftText;		break;
        case SStatHeader::sctInteger: 	sect->FieldType = sftNumber;	break;
        case SStatHeader::sctFloat: 	sect->FieldType = sftFloating;	break;
        }
    };
}
//---------------------------------------------------------------------------

void TfrmStatistic::AppendItem(AStringVec& columns)
{
	VERIFY					((int)columns.size()==sgData->HeaderSections->Count);
    TElTreeItem* item		= sgData->Items->AddChildObject(0,columns[0],0);	
    for (u32 k=1; k<columns.size(); ++k)
	    item->ColumnText->Add(columns[k]);
}
//---------------------------------------------------------------------------

void __fastcall TfrmStatistic::FormShow(TObject *Sender)
{
	paItemsCount->Caption 	= AnsiString().sprintf("Items count: %d",sgData->Items->Count);
}
//---------------------------------------------------------------------------

void TfrmStatistic::SortByColumn(int num, bool ascend)
{
	if (sgData->HeaderSections->Count==0) return;
    for (int k=0; k<sgData->HeaderSections->Count; ++k)
    	sgData->HeaderSections->Item[num]->SortMode = hsmNone; 
	clamp							(num,0,sgData->HeaderSections->Count-1);
    sgData->HeaderSections->Item[num]->SortMode 	= ascend?hsmAscend:hsmDescend; 
	sgData->SortSection 			= num;
	sgData->Sort					(true);
    
    lbQuickSearchCaption->Caption 	= AnsiString().sprintf(" Search by \"%s\":",AnsiString(sgData->HeaderSections->Item[num]->Text).c_str());
    paQuickSearchCaption->Width		= lbQuickSearchCaption->Width+6;
}
//---------------------------------------------------------------------------

void __fastcall TfrmStatistic::paQuickSearchEditResize(TObject *Sender)
{
	edQuickSearch->Width = paQuickSearchEdit->Width-2;
}
//---------------------------------------------------------------------------

void __fastcall TfrmStatistic::edQuickSearchKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    {
        AnsiString b	= AnsiString(edQuickSearch->Text).LowerCase();
        for (TElTreeItem* node=sgData->Selected?sgData->Selected:sgData->Items->GetFirstNode(); node; node=node->GetNextSibling()){
			AnsiString a	= AnsiString(node->ColumnText->Strings[sgData->SortSection-1]).LowerCase();
        	if (0!=strstr(a.c_str(),b.c_str())){
	        	sgData->EnsureVisible	(node);
    	        sgData->Selected 		= node;
                return;
            }
        }
    }
}
//---------------------------------------------------------------------------

