#include "stdafx.h"
#pragma hdrstop

#include "PropertiesShader.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

#include "main.h"
#include "Blender.h"
#include "Blender_Params.h"
#include "ShaderTools.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "mxPlacemnt"
#pragma link "ElStatBar"
#pragma link "ExtBtn"
#pragma resource "*.dfm"

CFS_Memory TfrmShaderProperties::m_Stream;
const LPSTR BOOLString[2]={"False","True"};

TfrmShaderProperties* TfrmShaderProperties::form=0;
//---------------------------------------------------------------------------
__fastcall TfrmShaderProperties::TfrmShaderProperties(TComponent* Owner)
	: TForm(Owner)
{
    char buf[MAX_PATH] = {"shader_ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::ShowProperties(){
	if (!form){
    	form = new TfrmShaderProperties(0);
    }
	form->Show();
}

void __fastcall TfrmShaderProperties::HideProperties(){
	if (form) form->Close();
}

void __fastcall TfrmShaderProperties::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	form = 0;
	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::FormShow(TObject *Sender)
{
    InplaceFloat->Editor->Color		= TColor(0x00A0A0A0);
    InplaceFloat->Editor->BorderStyle= bsNone;
    InplaceNumber->Editor->Color 	= TColor(0x00A0A0A0);
    InplaceNumber->Editor->BorderStyle= bsNone;
    InplaceEdit->Editor->Color 		= TColor(0x00A0A0A0);
    InplaceEdit->Editor->BorderStyle= bsNone;
    InplaceCustom->Editor->BorderStyle= bsNone;
    InplaceCustom->Editor->Color 	= TColor(0x00A0A0A0);
    InplaceCustom->Editor->BorderStyle= bsNone;
    InplaceCustom->Editor->ButtonGlyph->LoadFromResourceName((DWORD)HInstance,"ELLIPSIS");
    InplaceCustom->Editor->ReadOnly	= true;
    InplaceCustom->Editor->OnButtonClick = CustomClick;

	LoadProperties();
}
//---------------------------------------------------------------------------

TElTreeItem* __fastcall TfrmShaderProperties::AddItem(TElTreeItem* parent, DWORD type, LPCSTR key, LPDWORD value){
	TElTreeItem* TI     = tvProperties->Items->AddChildObject(parent,key,(TObject*)value);
    TI->Tag	            = type;
	TI->UseStyles 		= true;
    TElCellStyle* CS    = TI->AddStyle();
    CS->OwnerProps 		= true;

    switch (type){
    case BPID_MARKER:	CS->CellType = sftUndef;	break;
    case BPID_MATRIX:	CS->CellType = sftCustom;   TI->ColumnText->Add (LPSTR(value));break;//"[Matrix]");   break;
    case BPID_CONSTANT:	CS->CellType = sftCustom;   TI->ColumnText->Add ("[Color]");	break;
    case BPID_TEXTURE:	CS->CellType = sftCustom;   TI->ColumnText->Add ("[Textures]");	break;
    case BPID_INTEGER:	CS->CellType = sftNumber;	TI->ColumnText->Add	(AnsiString(*(int*)value));		break;
    case BPID_FLOAT:	CS->CellType = sftFloating; TI->ColumnText->Add	(AnsiString(*(float*)value));	break;
    case BPID_BOOL: 	CS->CellType = sftUndef;    TI->ColumnText->Add	(BOOLString[((BP_BOOL*)value)->value]); CS->Style = ElhsOwnerDraw; break;
//    case BPID_TOKEN:{	CS->CellType = sftUndef;	BP_TOKEN* T = (BP_TOKEN*)value; TI->ColumnText->Add	(T->elements[T->IDselected]); CS->Style = ElhsOwnerDraw; }break;
	default: THROW2("BPID_????");
    }
    return TI;
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::InplaceEditAfterOperation(TObject *Sender,
      bool &Accepted, bool &DefaultConversion)
{
	TElEdit* E = InplaceEdit->Editor;
    strcpy((LPSTR)InplaceEdit->Item->Data,AnsiString(E->Text).c_str());
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::InplaceFloatAfterOperation(TObject *Sender,
      bool &Accepted, bool &DefaultConversion)
{
	TElFloatSpinEdit* E = InplaceFloat->Editor;
    *(float*)InplaceFloat->Item->Data = E->Value;
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::InplaceNumberAfterOperation(
      TObject *Sender, bool &Accepted, bool &DefaultConversion)
{
	TElSpinEdit* E = InplaceNumber->Editor;
    *(int*)InplaceNumber->Item->Data = E->Value;
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::InplaceFloatBeforeOperation(
      TObject *Sender, bool &DefaultConversion)
{
	TElFloatSpinEdit* E = InplaceFloat->Editor;
    BP_Float* P = (BP_Float*)InplaceFloat->Item->Data;
    E->MinValue = P->min;
    E->MaxValue = P->max;
    E->Increment= (P->max-P->min)/1000.f;
    E->LargeIncrement = E->Increment*10;
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::InplaceNumberBeforeOperation(
      TObject *Sender, bool &DefaultConversion)
{
	TElSpinEdit* E = InplaceNumber->Editor;
    BP_Integer* P = (BP_Integer*)InplaceNumber->Item->Data;
    E->MinValue = P->min;
    E->MaxValue = P->max;
    E->Increment= 1;
    E->LargeIncrement = E->Increment*10;
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::tvPropertiesClick(TObject *Sender)
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
void __fastcall TfrmShaderProperties::tvPropertiesItemDraw(TObject *Sender,
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
void __fastcall TfrmShaderProperties::tvPropertiesMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	int HS;
	TElTreeItem* item = tvProperties->GetItemAt(X,Y,0,HS);
  	if ((HS==1)&&(Button==mbLeft)){
    	DWORD type = (DWORD)item->Tag;
		pmEnum->Tag = (int)item;
        switch(type){
		case BPID_BOOL:{
            pmEnum->Items->Clear();
            for (DWORD i=0; i<2; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = BOOLString[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
		case BPID_TOKEN:{
            pmEnum->Items->Clear();
            BP_TOKEN* tokens = (BP_TOKEN*)item->Data;
            for (DWORD i=0; i<tokens->Count; i++){
                TMenuItem* mi = new TMenuItem(0);
//                mi->Caption = tokens->elements[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        };
        switch(type){
		case BPID_BOOL:
		case BPID_TOKEN:
            TPoint P; P.x = X; P.y = Y;
            P=tvProperties->ClientToScreen(P);
            pmEnum->Canvas->Brush->Color = clBlack;
            pmEnum->Popup(P.x,P.y-10);
            break;
        }
    };
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::PMItemClick(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        TElTreeItem* item = (TElTreeItem*)pmEnum->Tag;
    	DWORD type = (DWORD)item->Tag;
        switch(type){
		case BPID_BOOL:
            item->ColumnText->Strings[0] 	= mi->Caption;
            ((BP_BOOL*)item->Data)->value 	= mi->MenuIndex;
        break;
		case BPID_TOKEN:
            item->ColumnText->Strings[0] 		= mi->Caption;
            ((BP_TOKEN*)item->Data)->IDselected = mi->MenuIndex;
        break;
        }
    }
}
//---------------------------------------------------------------------------

// *** FORMAT ***
// DWORD	type
// stringZ	name
// []		data
void __fastcall TfrmShaderProperties::LoadProperties(){
	m_Stream.clear();
	if (form&&SHTools.GetCurrentBlender(m_Stream)){ // fill Tree
        CStream data(m_Stream.pointer(), m_Stream.size());
        data.Advance(sizeof(CBlender_DESC));
        DWORD type;
        char key[255];
        TElTreeItem* marker_node=0;
        TElTreeItem* node;

        while (!data.Eof()){
	        int sz=0;
            type = data.Rdword();
            data.RstringZ(key);
            switch(type){
            case BPID_MARKER:	break;
            case BPID_TOKEN: 	break;
            case BPID_MATRIX:	sz=sizeof(string64);	break;
            case BPID_CONSTANT:	sz=sizeof(string64); 	break;
            case BPID_TEXTURE: 	sz=sizeof(string64); 	break;
            case BPID_INTEGER: 	sz=sizeof(BP_Integer);	break;
            case BPID_FLOAT: 	sz=sizeof(BP_Float); 	break;
            case BPID_BOOL: 	sz=sizeof(BP_BOOL); 	break;
            }
            if (type==BPID_MARKER) marker_node = 0;
            node = form->AddItem(marker_node,type,key,(LPDWORD)data.Pointer());
            if (type==BPID_MARKER) marker_node = node;
            data.Advance(sz);
        }
	    form->tvProperties->FullExpand();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::SaveProperties(){
//	CFS_Memory m_Stream;
//S заполнить ???    SHTools.GetCurrentBlender(m_Stream);
	CStream data(m_Stream.pointer(), m_Stream.size());
    SHTools.SetCurrentBlender(data);
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::CustomClick(
      TObject *Sender)
{
	DWORD type = InplaceCustom->Item->Tag;
    switch (type){
    case BPID_MATRIX:	type=0; break;
    case BPID_CONSTANT:	type=0; break;
    case BPID_TEXTURE:	type=0; break;
    }
}
//---------------------------------------------------------------------------


