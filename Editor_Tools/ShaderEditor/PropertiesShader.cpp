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

struct SIntegerParam{
	int min;
    int max;
	int increment;
    SIntegerParam(int mn, int mx, int inc):min(mn),max(mx),increment(inc){;}
};

struct SFloatParam{
	float min;
    float max;
	float increment;
    SFloatParam(float mn, float mx, float inc):min(mn),max(mx),increment(inc){;}
};

AStringVec BoolData;

TfrmShaderProperties* TfrmShaderProperties::form=0;
//---------------------------------------------------------------------------
__fastcall TfrmShaderProperties::TfrmShaderProperties(TComponent* Owner)
	: TForm(Owner)
{
	BoolData.push_back("False");
	BoolData.push_back("True");
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
	// free resource
    for ( TElTreeItem* node = tvProperties->Items->GetFirstNode(); node; node = node->GetNext()){
        switch((DWORD)node->Tag){
        case BPID_INTEGER:
        case BPID_FLOAT:{
            LPVOID p = node->ColumnText->Objects[0];
            delete p;
        }break;
        case BPID_BOOL:
        case BPID_TOKEN:{
        	AStringVec& v = *(AStringVec*)node->ColumnText->Objects[0];
            v.clear();
        }break;
        }
    }
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

	LoadProperties();
}
//---------------------------------------------------------------------------

TElTreeItem* __fastcall TfrmShaderProperties::AddItem(TElTreeItem* parent, DWORD type, LPCSTR key, LPDWORD value, LPDWORD p){
	TElTreeItem* TI     = tvProperties->Items->AddChildObject(parent,key,(TObject*)value);
    TI->Tag	            = type;
	TI->UseStyles 		= true;
    TElCellStyle* CS    = TI->AddStyle();
    CS->OwnerProps 		= true;

    switch (type){
    case BPID_MARKER:	CS->CellType = sftUndef;	break;
    case BPID_STRING:	CS->CellType = sftText; 	TI->ColumnText->Add (LPCSTR(value));break;
    case BPID_MATRIX:	CS->CellType = sftCustom;   TI->ColumnText->Add ("[Matrix]");   break;
    case BPID_CONSTANT:	CS->CellType = sftCustom;   TI->ColumnText->Add ("[Color]");	break;
    case BPID_TEXTURE:	CS->CellType = sftCustom;   TI->ColumnText->Add ("[Textures]");	break;
    case BPID_INTEGER:	CS->CellType = sftNumber;	TI->ColumnText->AddObject(AnsiString(*(int*)value),(TObject*)p);	break;
    case BPID_FLOAT:	CS->CellType = sftFloating; TI->ColumnText->AddObject(AnsiString(*(float*)value),(TObject*)p);	break;
    case BPID_TOKEN:	CS->CellType = sftUndef;	TI->ColumnText->AddObject(LPCSTR((*(AStringVec*)p)[*value].c_str()),(TObject*)p); CS->Style = ElhsOwnerDraw; break;
    case BPID_BOOL: 	CS->CellType = sftUndef;    TI->ColumnText->AddObject(LPCSTR(BoolData[*value].c_str()),(TObject*)&BoolData); CS->Style = ElhsOwnerDraw;break;
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
    SFloatParam* P = (SFloatParam*)InplaceFloat->Item->ColumnText->Objects[0];
    E->MinValue = P->min;
    E->MaxValue = P->max;
    E->Increment= P->increment;
    E->LargeIncrement = P->increment*10;
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::InplaceNumberBeforeOperation(
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
        switch(type){
		case BPID_TOKEN:
		case BPID_BOOL:{
            pmEnum->Items->Clear();
            AStringVec& tokens = *(AStringVec*)item->ColumnText->Objects[0];
            for (DWORD i=0; i<tokens.size(); i++){
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
void __fastcall TfrmShaderProperties::PMItemClick(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        TElTreeItem* item = (TElTreeItem*)mi->Tag;
    	DWORD type = (DWORD)item->Tag;
        switch(type){
		case BPID_TOKEN:
		case BPID_BOOL:{
            AStringVec& tokens = *(AStringVec*)item->ColumnText->Objects[0];
            item->ColumnText->Strings[0] = tokens[mi->MenuIndex];
            *(int*)item->Data = mi->MenuIndex;
        }break;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::ebShaderPropertiesClick(
      TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
// *** FORMAT ***
// DWORD	type
// stringZ	name
// []		data

void __fastcall TfrmShaderProperties::LoadProperties(){
	CFS_Memory m_Stream;
	if (form&&SHTools.GetCurrentBlender(m_Stream)){ // fill Tree
        CStream data(m_Stream.pointer(), m_Stream.size());
        data.Advance(sizeof(CBlender_DESC));
        DWORD type;
        char key[255];
        TElTreeItem* marker_node=0;
        TElTreeItem* node;

        char buf[255];
        BP_Integer i;
        BP_Float f;
        BP_BOOL b;
        while (!data.Eof()){
            type = data.Rdword();
            data.RstringZ(key);
            switch(type){
            case BPID_MARKER:	node = form->AddItem(marker_node,type,key); break;
            case BPID_TOKEN: 	break;
            case BPID_STRING:
            case BPID_MATRIX:
            case BPID_CONSTANT:
            case BPID_TEXTURE: 	data.RstringZ(buf);					node = form->AddItem(marker_node,type,key,(LPDWORD)&buf);break;
            case BPID_INTEGER: 	data.Read(&i,sizeof(BP_Integer));	node = form->AddItem(marker_node,type,key,(LPDWORD)&i);	break;
            case BPID_FLOAT: 	data.Read(&f,sizeof(BP_Float)); 	node = form->AddItem(marker_node,type,key,(LPDWORD)&f); break;
            case BPID_BOOL: 	data.Read(&b,sizeof(BP_BOOL)); 		node = form->AddItem(marker_node,type,key,(LPDWORD)&b); break;
            }
            if (type==BPID_MARKER) marker_node = node;
        }
	    form->tvProperties->FullExpand();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::SaveProperties(){
	CFS_Memory m_Stream;
//S заполнить ???    SHTools.GetCurrentBlender(m_Stream);
	CStream data(m_Stream.pointer(), m_Stream.size());
    SHTools.SetCurrentBlender(data);
}
//---------------------------------------------------------------------------

