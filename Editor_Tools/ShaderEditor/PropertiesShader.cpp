#include "stdafx.h"
#pragma hdrstop

#include "PropertiesShader.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

#include "main.h"
#include "Blender.h"
#include "Blender_Params.h"
#include "ShaderTools.h"
#include "xr_tokens.h"
#include "ShaderFunction.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "mxPlacemnt"
#pragma link "ElStatBar"
#pragma link "ExtBtn"
#pragma link "ELVCLUtils"
#pragma resource "*.dfm"

const LPSTR BOOLString[2]={"False","True"};
const LPSTR TEXTUREString[3]={"Custom...","-","$base0"};
const LPSTR MCString[11]={"Custom...","-","$null","$base0","$base1","$base2","$base3","$base4","$base5","$base6","$base7"};

#define BPID_TYPE 	0x1000
#define BPID_WAVE 	0x1001
#define BPID_FLAG 	0x1002
#define BPID_TOKEN2 0x1003

//---------------------------------------------------------------------------
xr_token							mode_token					[ ]={
	{ "Programmable",			  	CMatrix::modeProgrammable  	},
	{ "TCM",					  	CMatrix::modeTCM		  	},
	{ "Spherical Reflection",	  	CMatrix::modeS_refl	   		},
	{ "Cube Reflection",			CMatrix::modeC_refl	  		},
	{ 0,							0							}
};
//---------------------------------------------------------------------------

TfrmShaderProperties* TfrmShaderProperties::form=0;
//---------------------------------------------------------------------------
__fastcall TfrmShaderProperties::TfrmShaderProperties(TComponent* Owner)
	: TForm(Owner)
{
    char buf[MAX_PATH] = {"shader_ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
	m_BMEllipsis = new Graphics::TBitmap();
	m_BMEllipsis->LoadFromResourceName((DWORD)HInstance,"ELLIPSIS");
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
    _DELETE(m_BMEllipsis);
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::FormShow(TObject *Sender)
{
    InplaceFloat->Editor->Color		= TColor(0x00A0A0A0);
    InplaceFloat->Editor->BorderStyle= bsNone;
    InplaceNumber->Editor->Color 	= TColor(0x00A0A0A0);
    InplaceNumber->Editor->BorderStyle= bsNone;

	InitProperties();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::RemoveMatrix(TElTreeItem* parent){
	tvProperties->IsUpdating = true;
	for (TElTreeItem* item=parent->GetLastChild(); item;){
    	TElTreeItem* node;
        node=parent->GetPrevChild(item);
        item->Delete();
        item=node;
    }
	tvProperties->IsUpdating = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::AddMatrix(TElTreeItem* parent, LPSTR name){
	CMatrix* M = SHTools.FindMatrix(name,true);
    R_ASSERT(M);
	tvProperties->IsUpdating = true;
    AddItem(parent,BPID_TOKEN2,	"Mode",				&M->dwMode,	(LPDWORD)mode_token);
    AddItem(parent,BPID_FLAG,	"Scale enabled",	&M->tcm,	(LPDWORD)CMatrix::tcmScale);
    AddItem(parent,BPID_WAVE,	"Scale U",			(LPDWORD)&M->scaleU);
    AddItem(parent,BPID_WAVE,	"Scale V",			(LPDWORD)&M->scaleV);
    AddItem(parent,BPID_FLAG,	"Rotate enabled",	&M->tcm,		(LPDWORD)CMatrix::tcmRotate);
    AddItem(parent,BPID_WAVE,	"Rotate",			(LPDWORD)&M->rotate);
    AddItem(parent,BPID_FLAG,	"Scroll enabled",	&M->tcm,		(LPDWORD)CMatrix::tcmScroll);
    AddItem(parent,BPID_WAVE,	"Scroll U",			(LPDWORD)&M->scrollU);
    AddItem(parent,BPID_WAVE,	"Scroll V",			(LPDWORD)&M->scrollV);
	tvProperties->IsUpdating = false;
}

void __fastcall TfrmShaderProperties::AddConstant(TElTreeItem* parent, LPSTR name){
	CConstant* C = SHTools.FindConstant(name,true);
    R_ASSERT(C);
    AddItem(parent,BPID_WAVE,	"R",				(LPDWORD)&C->_R);
    AddItem(parent,BPID_WAVE,	"G",				(LPDWORD)&C->_G);
    AddItem(parent,BPID_WAVE,	"B",				(LPDWORD)&C->_B);
    AddItem(parent,BPID_WAVE,	"A",				(LPDWORD)&C->_A);
}

LPCSTR GetToken(xr_token* token_list, int id){
	for(int i=0; token_list[i].name; i++)
    	if (token_list[i].id==id) return token_list[i].name;
    return 0;
}


TElTreeItem* __fastcall TfrmShaderProperties::AddItem(TElTreeItem* parent, DWORD type, LPCSTR key, LPDWORD value, LPDWORD param){
	TElTreeItem* TI     = tvProperties->Items->AddChildObject(parent,key,(TObject*)value);
    TI->Tag	            = type;
	TI->UseStyles 		= true;
    TElCellStyle* CS    = TI->AddStyle();
    CS->OwnerProps 		= true;

    switch (type){
    case BPID_TYPE:		CS->CellType = sftUndef;	TI->ColumnText->Add (AnsiString((LPSTR)value)); break;
    case BPID_WAVE:		CS->CellType = sftUndef;	TI->ColumnText->Add ("[Wave]");	CS->Style = ElhsOwnerDraw; break;
    case BPID_FLAG:		CS->CellType = sftUndef;	TI->ColumnText->AddObject (BOOLString[!!((*value)&(DWORD)param)],(TObject*)param);	CS->Style = ElhsOwnerDraw; break;
    case BPID_TOKEN2:	CS->CellType = sftUndef;	TI->ColumnText->AddObject (GetToken((xr_token*)param,*value),(TObject*)param);	CS->Style = ElhsOwnerDraw; break;
    case BPID_MARKER:	CS->CellType = sftUndef;	break;
    case BPID_MATRIX:
    	CS->CellType = sftUndef;
        TI->ColumnText->Add ("");
		if (*((LPSTR)value)=='$') TI->ColumnText->Strings[0] = (LPSTR)value;
        else AddMatrix(TI,(LPSTR)value);
    break;
    case BPID_CONSTANT:
    	CS->CellType = sftUndef;
        TI->ColumnText->Add ("");
		if (*((LPSTR)value)=='$') TI->ColumnText->Strings[0] = (LPSTR)value;
        else AddConstant(TI,(LPSTR)value);
    break;
    case BPID_TEXTURE:	CS->CellType = sftUndef;    TI->ColumnText->Add (AnsiString((LPSTR)value));		break;
    case BPID_INTEGER:	CS->CellType = sftNumber;	TI->ColumnText->Add	(AnsiString(*(int*)value));		break;
    case BPID_FLOAT:	CS->CellType = sftFloating; TI->ColumnText->Add	(AnsiString(*(float*)value));	break;
    case BPID_BOOL: 	CS->CellType = sftUndef;    TI->ColumnText->Add	(BOOLString[((BP_BOOL*)value)->value]); CS->Style = ElhsOwnerDraw; break;
//    case BPID_TOKEN:{	CS->CellType = sftUndef;	BP_TOKEN* T = (BP_TOKEN*)value; TI->ColumnText->Add	(T->elements[T->IDselected]); CS->Style = ElhsOwnerDraw; }break;
	default: THROW2("BPID_????");
    }
    return TI;
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::InplaceFloatAfterOperation(TObject *Sender,
      bool &Accepted, bool &DefaultConversion)
{
	TElFloatSpinEdit* E = InplaceFloat->Editor;
    if (*(float*)InplaceFloat->Item->Data != E->Value){
	    *(float*)InplaceFloat->Item->Data = E->Value;
    	SHTools.Modified();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::InplaceNumberAfterOperation(
      TObject *Sender, bool &Accepted, bool &DefaultConversion)
{
	TElSpinEdit* E = InplaceNumber->Editor;
    if (*(int*)InplaceNumber->Item->Data != E->Value){
	    *(int*)InplaceNumber->Item->Data = E->Value;
    	SHTools.Modified();
    }
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
        case BPID_WAVE:{
            R.Right	-=	10;
            R.Left 	+= 	1;
    		DrawText	(Surface->Handle, AnsiString(Item->ColumnText->Strings[0]).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            R.Left 	= 	R.Right;
//			Surface->CopyMode = cmSrcAnd;//cmSrcErase;
//			Surface->Draw(R.Left,R.Top+5,m_BMEllipsis);
        }break;
        case BPID_FLAG:
		case BPID_TOKEN2:
		case BPID_TOKEN:
		case BPID_BOOL:
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
		case BPID_FLAG:{
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
		case BPID_TOKEN2:{
            pmEnum->Items->Clear();
            xr_token* token_list = (xr_token*)item->ColumnText->Objects[0];
			for(int i=0; token_list[i].name; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = token_list[i].name;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        case BPID_WAVE: CustomClick(item); break;
		case BPID_TEXTURE:{
            pmEnum->Items->Clear();
            for (DWORD i=0; i<3; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = TEXTUREString[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        case BPID_CONSTANT:
        case BPID_MATRIX:{
            pmEnum->Items->Clear();
            for (DWORD i=0; i<11; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = MCString[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        };
        switch(type){
        case BPID_CONSTANT:
        case BPID_MATRIX:
        case BPID_TOKEN2:
        case BPID_FLAG:
		case BPID_BOOL:
		case BPID_TOKEN:
		case BPID_TEXTURE:
            TPoint P; P.x = X; P.y = Y;
            P=tvProperties->ClientToScreen(P);
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
            if (((BP_BOOL*)item->Data)->value != mi->MenuIndex){
	            item->ColumnText->Strings[0]= mi->Caption;
	            ((BP_BOOL*)item->Data)->value= mi->MenuIndex;
		    	SHTools.Modified();
            }
        break;
		case BPID_FLAG:{
            item->ColumnText->Strings[0] 	= mi->Caption;
            DWORD val 						= *(LPDWORD)item->Data;
            DWORD fl 						= (DWORD)item->ColumnText->Objects[0];
            if (mi->MenuIndex) 	val |= fl;
            else				val &=~fl;
            if (val!= *(LPDWORD)item->Data){
            	*(LPDWORD)item->Data = val;
				SHTools.Modified();
            }
        }break;
		case BPID_TOKEN:
        	if (((BP_TOKEN*)item->Data)->IDselected != mi->MenuIndex){
	            item->ColumnText->Strings[0] 		= mi->Caption;
    	        ((BP_TOKEN*)item->Data)->IDselected = mi->MenuIndex;
				SHTools.Modified();
            }
        break;
		case BPID_TOKEN2:{
            xr_token* token_list 	   		= (xr_token*)item->ColumnText->Objects[0];
            if ((DWORD)(item->Data)	!= token_list[mi->MenuIndex].id){
	            item->ColumnText->Strings[0]= mi->Caption;
    	        *(LPDWORD)(item->Data)		= token_list[mi->MenuIndex].id;
				SHTools.Modified();
            }
        }break;
		case BPID_TEXTURE:{
			LPSTR tex = (LPSTR)item->Data;
        	if (mi->MenuIndex==0){
            	LPCSTR name = TfrmChoseItem::SelectTexture(false,tex);
//S оставить нужные фолдеры
            	if (strcmp(tex,name)!=0){
					strcpy(tex,name);
		            item->ColumnText->Strings[0]= name;
			    	SHTools.Modified();
                }
            }else if (mi->MenuIndex==2){
            	if (strcmp(tex,TEXTUREString[mi->MenuIndex])!=0){
					strcpy(tex,TEXTUREString[mi->MenuIndex]);
		            item->ColumnText->Strings[0]= TEXTUREString[mi->MenuIndex];
			    	SHTools.Modified();
                }
            }
        }break;
        case BPID_CONSTANT:{
			LPSTR nm = (LPSTR)item->Data;
        	if (mi->MenuIndex==0){
            	if (strcmp(nm,MCString[mi->MenuIndex])!=0){
					strcpy(nm,SHTools.AppendConstant());
			    	SHTools.Modified();
                }
            }else if (mi->MenuIndex>=2){
            	if (*nm!='$') SHTools.RemoveConstant(nm);
            	if (strcmp(nm,MCString[mi->MenuIndex])!=0){
					strcpy(nm,MCString[mi->MenuIndex]);
		            item->ColumnText->Strings[0]= MCString[mi->MenuIndex];
			    	SHTools.Modified();
                }
            }
        }break;
        case BPID_MATRIX:{
			LPSTR nm = (LPSTR)item->Data;
        	if (mi->MenuIndex==0){
            	if (strcmp(nm,MCString[mi->MenuIndex])!=0){
					strcpy(nm,SHTools.AppendMatrix());
		            item->ColumnText->Strings[0]= MCString[mi->MenuIndex];
                    AddMatrix(item,nm);
			    	SHTools.Modified();
                }
            }else if (mi->MenuIndex>=2){
            	if (*nm!='$'){
                	SHTools.RemoveMatrix(nm);
                    RemoveMatrix(item);
                }
            	if (strcmp(nm,MCString[mi->MenuIndex])!=0){
					strcpy(nm,MCString[mi->MenuIndex]);
		            item->ColumnText->Strings[0]= MCString[mi->MenuIndex];
			    	SHTools.Modified();
                }
            }
        }break;
        }
    }
}
//---------------------------------------------------------------------------

// *** FORMAT ***
// DWORD	type
// stringZ	name
// []		data
void __fastcall TfrmShaderProperties::InitProperties(){
	if (form){
    	if (SHTools.m_CurrentBlender){ // fill Tree
            CStream data(SHTools.m_BlenderStream.pointer(), SHTools.m_BlenderStream.size());
            data.Advance(sizeof(CBlender_DESC));
            DWORD type;
            char key[255];
            TElTreeItem* marker_node=0;
            TElTreeItem* node;
            form->tvProperties->IsUpdating = true;
            form->tvProperties->Items->Clear();

			form->AddItem(0,BPID_TYPE,"Type",(LPDWORD)SHTools.m_CurrentBlender->getComment());

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
                if (type==BPID_MARKER){
                	marker_node = node;
//					marker_node->Expand(true);
                }
                data.Advance(sz);
            }
            form->tvProperties->FullExpand();
            form->tvProperties->IsUpdating = false;
        }else{
	    	form->tvProperties->Items->Clear();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::CustomClick(TElTreeItem* item)
{
	DWORD type = item->Tag;
    switch (type){
    case BPID_WAVE:{
        if (TfrmShaderFunction::Run((WaveForm*)item->Data)==mrOk)
			SHTools.Modified();
    }break;
    }
}
//---------------------------------------------------------------------------

