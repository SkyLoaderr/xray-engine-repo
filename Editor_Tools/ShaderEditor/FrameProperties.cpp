//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameProperties.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "ElTreeBtnEdit"
#pragma link "ElTreeModalEdit"
#pragma link "ElTreeSpinEdit"
#pragma link "ElTreeStdEditors"
#pragma link "ElXPThemedControl"
#pragma link "MxMenus"
#pragma resource "*.dfm"
TfraProperties *fraProperties;
//---------------------------------------------------------------------------
StringVec BoolData;

__fastcall TfraProperties::TfraProperties(TComponent* Owner)
	: TFrame(Owner)
{
	BoolData.push_back("False");
	BoolData.push_back("True");
}

LPSTR name = "Don't known";
int token = 1;
BOOL B=FALSE;
float f=7.f;
int i=9;

void __fastcall TfraProperties::ShowProperties(LPDWORD data){
	// fill items
	TElTreeItem* node;
    node = AddItem	(0,		BPID_MARKER,	"General"	);
	AddItem			(node,	BPID_STRING,	"Name",		(DWORD*)name);
	node = AddItem 	(0,		BPID_MARKER,	"Params"	);
	AddItem     	(node,	BPID_MATRIX,	"Matrix",	0);
	AddItem    		(node,	BPID_BOOL,		"Strict B2F",(DWORD*)&B);
	AddItem     	(node,	BPID_FLOAT,		"Float",	(DWORD*)&f,(DWORD*)new SFloatParam(-100.f,100.f,1.f));
	AddItem     	(node,	BPID_INTEGER,	"Integer",	(DWORD*)&i,(DWORD*)new SIntegerParam(-200,200,1));
	AddItem			(node,	BPID_COLOR,		"Color",	0);

    tvProperties->FullExpand();

    InplaceFloat->Editor->Color		= 0x00A0A0A0;
    InplaceFloat->Editor->BorderStyle= bsNone;
    InplaceNumber->Editor->Color 	= 0x00A0A0A0;
    InplaceNumber->Editor->BorderStyle= bsNone;
    InplaceEdit->Editor->Color 		= 0x00A0A0A0;
    InplaceEdit->Editor->BorderStyle= bsNone;
    InplaceCustom->Editor->BorderStyle= bsNone;
    InplaceCustom->Editor->Color 	= 0x00A0A0A0;
    InplaceCustom->Editor->BorderStyle= bsNone;
    InplaceCustom->Editor->ButtonGlyph->LoadFromResourceName((DWORD)HInstance,"ELLIPSIS");
    InplaceCustom->Editor->ReadOnly	= true;
}
//---------------------------------------------------------------------------

void __fastcall TfraProperties::HideProperties(){
    for ( TElTreeItem* node = tvProperties->Items->GetFirstNode(); node; node = node->GetNext()){
        switch((DWORD)node->Tag){
        case BPID_INTEGER:
        case BPID_FLOAT:{
            LPVOID p = node->ColumnText->Objects[0];
            delete p;
        }break;
        case BPID_BOOL:
        case BPID_TOKEN:{
        	StringVec& v = *(StringVec*)node->ColumnText->Objects[0];
            v.clear();
        }break;
        }
    }
}

//---------------------------------------------------------------------------
TElTreeItem* __fastcall TfraProperties::AddItem(TElTreeItem* parent, BPID type, LPCSTR key, LPDWORD value, LPDWORD p){
	TElTreeItem* TI     = tvProperties->Items->AddChildObject(parent,key,(TObject*)value);
    TI->Tag	            = type;
	TI->UseStyles 		= true;
    TElCellStyle* CS    = TI->AddStyle();
    CS->OwnerProps 		= true;

    switch (type){
    case BPID_MARKER:	CS->CellType = sftUndef;	break;
    case BPID_STRING:	CS->CellType = sftText; 	TI->ColumnText->Add (LPCSTR(value));break;
    case BPID_MATRIX:	CS->CellType = sftCustom;   TI->ColumnText->Add ("[Matrix]");   break;
    case BPID_COLOR:	CS->CellType = sftCustom;   TI->ColumnText->Add ("[Color]");	break;
    case BPID_INTEGER:	CS->CellType = sftNumber;	TI->ColumnText->AddObject(AnsiString(*(int*)value),(TObject*)p);	break;
    case BPID_FLOAT:	CS->CellType = sftFloating; TI->ColumnText->AddObject(AnsiString(*(float*)value),(TObject*)p);	break;
    case BPID_TOKEN:	CS->CellType = sftUndef;	TI->ColumnText->AddObject(LPCSTR((*(StringVec*)p)[*value].c_str()),(TObject*)p); CS->Style = ElhsOwnerDraw; break;
    case BPID_BOOL: 	CS->CellType = sftUndef;    TI->ColumnText->AddObject(LPCSTR(BoolData[*value].c_str()),(TObject*)&BoolData); CS->Style = ElhsOwnerDraw;break;
    }
    return TI;
}
//---------------------------------------------------------------------------
void __fastcall TfraProperties::InplaceEditAfterOperation(TObject *Sender,
      bool &Accepted, bool &DefaultConversion)
{
	TElEdit* E = InplaceEdit->Editor;
    strcpy((LPSTR)InplaceEdit->Item->Data,AnsiString(E->Text).c_str());
}
//---------------------------------------------------------------------------
void __fastcall TfraProperties::InplaceFloatAfterOperation(TObject *Sender,
      bool &Accepted, bool &DefaultConversion)
{
	TElFloatSpinEdit* E = InplaceFloat->Editor;
    *(float*)InplaceFloat->Item->Data = E->Value;
}
//---------------------------------------------------------------------------
void __fastcall TfraProperties::InplaceNumberAfterOperation(
      TObject *Sender, bool &Accepted, bool &DefaultConversion)
{
	TElSpinEdit* E = InplaceNumber->Editor;
    *(int*)InplaceNumber->Item->Data = E->Value;
}
//---------------------------------------------------------------------------
void __fastcall TfraProperties::InplaceFloatBeforeOperation(
      TObject *Sender, bool &DefaultConversion)
{
	TElFloatSpinEdit* E = InplaceFloat->Editor;
    SFloatParam* P = (SFloatParam*)InplaceFloat->Item->ColumnText->Objects[0];
    E->MinValue = P->min;
    E->MaxValue = P->max;
    E->Increment= P->increment;
    E->LargeIncrement = P->increment*10;
}
//---------------------------------------------------------------------------
void __fastcall TfraProperties::InplaceNumberBeforeOperation(
      TObject *Sender, bool &DefaultConversion)
{
	TElSpinEdit* E = InplaceNumber->Editor;
    SIntegerParam* P = (SIntegerParam*)InplaceNumber->Item->ColumnText->Objects[0];
    E->MinValue = P->min;
    E->MaxValue = P->max;
    E->Increment= P->increment;
    E->LargeIncrement = P->increment*10;
}
//---------------------------------------------------------------------------
void __fastcall TfraProperties::tvPropertiesClick(TObject *Sender)
{
	int HS;
    TElTreeItem* 	Item;
    TPoint			P;

  	GetCursorPos(&P);
  	P = tvProperties->ScreenToClient(P);
  	Item = tvProperties->GetItemAt(P.x, P.y, 0, HS);
	if (HS==1)
    	tvProperties->EditItem(Item, HS);
}
//---------------------------------------------------------------------------
void __fastcall TfraProperties::tvPropertiesItemDraw(TObject *Sender,
      TElTreeItem *Item, TCanvas *Surface, TRect &R, int SectionIndex)
{
	TRect  R1;
    int  sid;
	Surface->Brush->Style = bsClear;
    Surface->Font->Color = clBlack;
  	if (SectionIndex == 1){
    	DWORD type = (DWORD)Item->Tag;
        switch(type){
		case BPID_TOKEN:
		case BPID_BOOL:
            R.Right-=10;
            R.Left += 1;
    		DrawText(Surface->Handle, AnsiString(Item->ColumnText->Strings[0]).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            R.Left = R.Right;
            R.Right += 10;
            DrawArrow(Surface, eadDown, R, clWindowText, true);
        break;
        };
  	}
}
//---------------------------------------------------------------------------
void __fastcall TfraProperties::tvPropertiesMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	int HS;
	TElTreeItem* item = tvProperties->GetItemAt(X,Y,0,HS);
  	if ((HS==1)&&(Button==mbLeft)){
    	DWORD type = (DWORD)item->Tag;
        switch(type){
		case BPID_TOKEN:
		case BPID_BOOL:{
            pmEnum->Items->Clear();
            StringVec& tokens = *(StringVec*)item->ColumnText->Objects[0];
            for (int i=0; i<tokens.size(); i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = tokens[i];
                mi->Tag     = (int)item;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
            TPoint P; P.x = X; P.y = Y;
            P=tvProperties->ClientToScreen(P);
            pmEnum->Canvas->Brush->Color = clBlack;
            pmEnum->Canvas->Brush->Color = clBlack;
            pmEnum->Popup(P.x,P.y-10);
        }
        break;
        };
    };
}
//---------------------------------------------------------------------------
void __fastcall TfraProperties::PMItemClick(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        TElTreeItem* item = (TElTreeItem*)mi->Tag;
    	DWORD type = (DWORD)item->Tag;
        switch(type){
		case BPID_TOKEN:
		case BPID_BOOL:{
            StringVec& tokens = *(StringVec*)item->ColumnText->Objects[0];
            item->ColumnText->Strings[0] = tokens[mi->MenuIndex];
            *(int*)item->Data = mi->MenuIndex;
        }break;
        }
    }
}
//---------------------------------------------------------------------------
