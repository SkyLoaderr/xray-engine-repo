#include "stdafx.h"
#pragma hdrstop

#include "PropertiesList.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

#include "main.h"
#include "xr_tokens.h"
#include "ShaderFunction.h"
#include "ColorPicker.h"
#include "xr_func.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

const LPSTR BOOLString[2]={"False","True"};

//---------------------------------------------------------------------------
void __fastcall TfrmProperties::BeginFillMode(const AnsiString& title)
{
	bFillMode=true;
	tvProperties->IsUpdating = true;
    tvProperties->Items->Clear();
    Caption = title;
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::EndFillMode()
{
	bFillMode=false;
    bModified=false;
	tvProperties->FullExpand();
    tvProperties->IsUpdating = false;
};
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::Clear()
{
	tvProperties->IsUpdating = true;
    tvProperties->Items->Clear();
    tvProperties->IsUpdating = false;
}
//---------------------------------------------------------------------------

__fastcall TfrmProperties::TfrmProperties(TComponent* Owner)
	: TForm(Owner)
{
	bModified = false;
	bFillMode = false;
    DEFINE_INI(fsStorage);
	m_BMEllipsis = new Graphics::TBitmap();
	m_BMEllipsis->LoadFromResourceName((DWORD)HInstance,"ELLIPSIS");
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::ShowProperties(){
	Show();
}

void __fastcall TfrmProperties::HideProperties(){
	Close();
}

void __fastcall TfrmProperties::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
    _DELETE(m_BMEllipsis);
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::FormShow(TObject *Sender)
{
    InplaceFloat->Editor->Color		= TColor(0x00A0A0A0);
    InplaceFloat->Editor->BorderStyle= bsNone;
    InplaceNumber->Editor->Color 	= TColor(0x00A0A0A0);
    InplaceNumber->Editor->BorderStyle= bsNone;
}
//---------------------------------------------------------------------------


LPCSTR GetToken2(xr_token* token_list, int id)
{
	for(int i=0; token_list[i].name; i++)
    	if (token_list[i].id==id) return token_list[i].name;
    return 0;
}


TElTreeItem* __fastcall TfrmProperties::AddItem(TElTreeItem* parent, DWORD type, LPCSTR key, LPDWORD value, LPDWORD param){
	R_ASSERT(bFillMode);
    TElTreeItem* TI     = tvProperties->Items->AddChildObject(parent,key,(TObject*)value);
    TI->Tag	            = type;
    TI->UseStyles 		= true;
    TElCellStyle* CS    = TI->AddStyle();
    CS->OwnerProps 		= true;

    switch (type){
    case PROP_TYPE:		CS->CellType = sftUndef;	TI->ColumnText->Add (AnsiString((LPSTR)value)); break;
    case PROP_WAVE:		CS->CellType = sftUndef;	TI->ColumnText->Add ("[Wave]");	CS->Style = ElhsOwnerDraw; break;
    case PROP_BOOL:		CS->CellType = sftUndef;	TI->ColumnText->Add (BOOLString[*value]);	CS->Style = ElhsOwnerDraw; break;
    case PROP_FLAG:		CS->CellType = sftUndef;	TI->ColumnText->AddObject (BOOLString[!!((*value)&(DWORD)param)],(TObject*)param);	CS->Style = ElhsOwnerDraw; break;
    case PROP_TOKEN:	CS->CellType = sftUndef;	TI->ColumnText->AddObject (GetToken2((xr_token*)param,*value),(TObject*)param);	CS->Style = ElhsOwnerDraw; break;
    case PROP_MARKER:	CS->CellType = sftUndef;	break;
    case PROP_FLOAT:	CS->CellType = sftFloating; TI->ColumnText->Add	(AnsiString(double(iFloor(double(*(float*)value)*10000))/10000));  break;
    case PROP_INTEGER:	CS->CellType = sftNumber; 	TI->ColumnText->Add	(AnsiString(*((int*)value)));  break;
    case PROP_TEXT:		CS->CellType = sftText; 	TI->ColumnText->Add	((LPSTR)value);  break;
    case PROP_COLOR:	CS->CellType = sftUndef; 	CS->Style = ElhsOwnerDraw; break;
    case PROP_EXEC:		CS->CellType = sftUndef; 	TI->ColumnText->AddObject ("...",(TObject*)param); CS->Style = ElhsOwnerDraw; break;
    default: THROW2("BPID_????");
    }
    return TI;
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::InplaceFloatValidateResult(
      TObject *Sender, bool &InputValid)
{
	TElFloatSpinEdit* E = InplaceFloat->Editor;
	if (PROP_FLOAT==InplaceFloat->Item->Tag){
	    if (!fsimilar(*(float*)InplaceFloat->Item->Data,E->Value)){
		    *(float*)InplaceFloat->Item->Data = E->Value;
            bModified = true;
	    }
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::InplaceFloatBeforeOperation(
      TObject *Sender, bool &DefaultConversion)
{
	TElFloatSpinEdit* E = InplaceFloat->Editor;
	if (PROP_FLOAT==InplaceFloat->Item->Tag){
        E->MinValue = 0.;
        E->MaxValue = 1.;
        E->Increment= 0.01;
        E->LargeIncrement = E->Increment*10;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::InplaceNumberValidateResult(TObject *Sender, bool &InputValid)
{
	TElSpinEdit* E = InplaceNumber->Editor;
	if (PROP_INTEGER==InplaceNumber->Item->Tag){
	    if (*(int*)InplaceNumber->Item->Data != E->Value){
		    *(int*)InplaceNumber->Item->Data = E->Value;
            bModified = true;
	    }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::InplaceNumberBeforeOperation(
      TObject *Sender, bool &DefaultConversion)
{
	TElSpinEdit* E = InplaceNumber->Editor;
//    E->MinValue = P->min;
//    E->MaxValue = P->max;
    E->Increment= 1;
    E->LargeIncrement = E->Increment*10;
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::tvPropertiesClick(TObject *Sender)
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
void __fastcall TfrmProperties::tvPropertiesItemDraw(TObject *Sender,
      TElTreeItem *Item, TCanvas *Surface, TRect &R, int SectionIndex)
{
	TRect  R1;
    int  sid;
	Surface->Brush->Style = bsClear;
    Surface->Font->Color = clBlack;
  	if (SectionIndex == 1){
    	DWORD type = (DWORD)Item->Tag;
        switch(type){
        case PROP_EXEC:
        case PROP_WAVE:
            R.Right	-=	10;
            R.Left 	+= 	1;
    		DrawText	(Surface->Handle, AnsiString(Item->ColumnText->Strings[0]).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        break;
        case PROP_COLOR:{
            R.Right	-=	1;
            R.Left 	+= 	1;
			Surface->Brush->Style = bsSolid;
		    Surface->Brush->Color = rgb2bgr(*(LPDWORD)Item->Data);
            Surface->FillRect(R);
//            R.Left 	= 	R.Right;
//            R.Right += 	10;
//            DrawArrow	(Surface, eadDown, R, clWindowText, true);
        }break;
        case PROP_FLAG:
		case PROP_TOKEN:
        case PROP_BOOL:
            R.Right	-=	10;
            R.Left 	+= 	1;
    		DrawText	(Surface->Handle, AnsiString(Item->ColumnText->Strings[0]).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            R.Left 	= 	R.Right;
            R.Right += 	10;
            DrawArrow	(Surface, eadDown, R, clWindowText, true);
        break;
        };
  	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::tvPropertiesMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	int HS;
	TElTreeItem* item = tvProperties->GetItemAt(X,Y,0,HS);
  	if ((HS==1)&&(Button==mbLeft)){
    	DWORD type = (DWORD)item->Tag;
		pmEnum->Tag = (int)item;
        switch(type){
		case PROP_BOOL:{
            pmEnum->Items->Clear();
            for (DWORD i=0; i<2; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = BOOLString[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
		case PROP_FLAG:{
            pmEnum->Items->Clear();
            for (DWORD i=0; i<2; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = BOOLString[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
		case PROP_TOKEN:{
            pmEnum->Items->Clear();
            xr_token* token_list = (xr_token*)item->ColumnText->Objects[0];
			for(int i=0; token_list[i].name; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = token_list[i].name;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        case PROP_WAVE: CustomClick(item); break;
        case PROP_COLOR: ColorClick(item); break;
        case PROP_EXEC:{
        	PROP_EXEC_CB* T = (PROP_EXEC_CB*)item->Data; VERIFY(T);
            T((LPDWORD)item->ColumnText->Objects[0]);
        }break;
        };
        switch(type){
        case PROP_TOKEN:
        case PROP_FLAG:
		case PROP_BOOL:
            TPoint P; P.x = X; P.y = Y;
            P=tvProperties->ClientToScreen(P);
            pmEnum->Popup(P.x,P.y-10);
            break;
        }
    };
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::PMItemClick(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        TElTreeItem* item = (TElTreeItem*)pmEnum->Tag;
    	DWORD type = (DWORD)item->Tag;
        switch(type){
		case PROP_BOOL:
            if ((*(BOOL*)item->Data) != mi->MenuIndex){
	            item->ColumnText->Strings[0]= mi->Caption;
	            (*(BOOL*)item->Data) = mi->MenuIndex;
		    	bModified = true;
            }
        break;
		case PROP_FLAG:{
            item->ColumnText->Strings[0] 	= mi->Caption;
            DWORD val 						= *(LPDWORD)item->Data;
            DWORD fl 						= (DWORD)item->ColumnText->Objects[0];
            if (mi->MenuIndex) 	val |= fl;
            else				val &=~fl;
            if (val!= *(LPDWORD)item->Data){
            	*(LPDWORD)item->Data = val;
		    	bModified = true;
            }
        }break;
		case PROP_TOKEN:{
            xr_token* token_list 	   		= (xr_token*)item->ColumnText->Objects[0];
            if ((DWORD)(item->Data)	!= token_list[mi->MenuIndex].id){
	            item->ColumnText->Strings[0]= mi->Caption;
    	        *(LPDWORD)(item->Data)		= token_list[mi->MenuIndex].id;
		    	bModified = true;
            }
        }break;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::CustomClick(TElTreeItem* item)
{
	DWORD type = item->Tag;
    switch (type){
    case PROP_WAVE:{
        if (TfrmShaderFunction::Run((WaveForm*)item->Data)==mrOk)
			bModified = true;
    }break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::ColorClick(TElTreeItem* item)
{
	DWORD type = item->Tag;
    LPDWORD color = (LPDWORD)item->Data;
    R_ASSERT(type==PROP_COLOR);
	if (SelectColor(color)){
		bModified = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::InplaceTextValidateResult(TObject *Sender,
      bool &InputValid)
{
	TElEdit* E = InplaceText->Editor;
	if (PROP_TEXT==InplaceText->Item->Tag){
	    if (strcmp((LPSTR)InplaceText->Item->Data,AnsiString(E->Text).c_str())!=0){
		    strcpy((LPSTR)InplaceText->Item->Data,AnsiString(E->Text).c_str());
            bModified = true;
	    }
	}
}
//---------------------------------------------------------------------------

