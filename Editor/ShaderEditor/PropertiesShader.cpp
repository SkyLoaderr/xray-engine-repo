#include "stdafx.h"
#pragma hdrstop

#include "PropertiesShader.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

#include "main.h"
#include "Blender.h"
#include "Blender_Params.h"
#include "UI_Tools.h"
#include "xr_tokens.h"
#include "ShaderFunction.h"
#include "ChoseForm.h"
#include "UI_Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "mxPlacemnt"
#pragma link "ElStatBar"
#pragma link "ExtBtn"
#pragma link "ELVCLUtils"
#pragma link "multi_edit"
#pragma resource "*.dfm"

#define TSTRING_COUNT 	4
#define MCSTRING_COUNT 	11

const LPSTR BOOLString[2]={"False","True"};
const LPSTR TEXTUREString[TSTRING_COUNT]={"Custom...","-","$null","$base0"};
const LPSTR MCString[MCSTRING_COUNT]={"Custom...","-","$null","$base0","$base1","$base2","$base3","$base4","$base5","$base6","$base7"};

#define BPID_TYPE 	0x1000
#define BPID_WAVE 	0x1001
#define BPID_FLAG 	0x1002
#define BPID_TOKEN2 0x1003
#define BPID_FLOAT2 0x1004
#define BPID_BOOL2  0x1005

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
bool TfrmShaderProperties::m_bFreeze = false;
bool TfrmShaderProperties::m_bModified = false;
//---------------------------------------------------------------------------
__fastcall TfrmShaderProperties::TfrmShaderProperties(TComponent* Owner)
	: TForm(Owner)
{
	DEFINE_INI(fsStorage);
	m_BMEllipsis = new Graphics::TBitmap();
	m_BMEllipsis->LoadFromResourceName((DWORD)HInstance,"ELLIPSIS");
    seNumber->Parent = tvProperties;
    seNumber->Hide();
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
	CMatrix* M = Tools.Engine.FindMatrix(name,true);
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
	CConstant* C = Tools.Engine.FindConstant(name,true);
    R_ASSERT(C);
    AddItem(parent,BPID_WAVE,	"R",				(LPDWORD)&C->_R);
    AddItem(parent,BPID_WAVE,	"G",				(LPDWORD)&C->_G);
    AddItem(parent,BPID_WAVE,	"B",				(LPDWORD)&C->_B);
    AddItem(parent,BPID_WAVE,	"A",				(LPDWORD)&C->_A);
}

LPCSTR GetToken2(xr_token* token_list, int id)
{
	for(int i=0; token_list[i].name; i++)
    	if (token_list[i].id==id) return token_list[i].name;
    return 0;
}


BP_TOKEN::Item& GetTokenItem(BP_TOKEN* p, int idx)
{
    R_ASSERT(p&&(idx>=0)&&(idx<p->Count));
    return *(BP_TOKEN::Item*)(LPBYTE(p) + sizeof(BP_TOKEN)+sizeof(BP_TOKEN::Item)*idx);
}

LPCSTR GetToken(BP_TOKEN* p, int id)
{
	for(int i=0; p->Count; i++){
		BP_TOKEN::Item& I = GetTokenItem(p,i);
    	if (I.ID==id) return I.str;
    }
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
    case BPID_BOOL2:	CS->CellType = sftUndef;	TI->ColumnText->Add (BOOLString[*value]);	CS->Style = ElhsOwnerDraw; break;
    case BPID_FLAG:		CS->CellType = sftUndef;	TI->ColumnText->AddObject (BOOLString[!!((*value)&(DWORD)param)],(TObject*)param);	CS->Style = ElhsOwnerDraw; break;
    case BPID_TOKEN2:	CS->CellType = sftUndef;	TI->ColumnText->AddObject (GetToken2((xr_token*)param,*value),(TObject*)param);	CS->Style = ElhsOwnerDraw; break;
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
    case BPID_TEXTURE:	CS->CellType = sftUndef;    TI->ColumnText->Add (AnsiString((LPSTR)value));					break;
    case BPID_INTEGER:	CS->CellType = sftUndef;	TI->ColumnText->Add	(AnsiString(((BP_Integer*)value)->value));	CS->Style = ElhsOwnerDraw; break;
    case BPID_FLOAT:{  	CS->CellType = sftUndef;	BP_Float* F=(BP_Float*)value;
    					AnsiString s,fmt; fmt.sprintf("%%.%df",2); s.sprintf(fmt.c_str(),F->value);
                        TI->ColumnText->Add(s); CS->Style = ElhsOwnerDraw; }break;
    case BPID_FLOAT2:{  CS->CellType = sftUndef;	float F=*(float*)value;
    					AnsiString s,fmt; fmt.sprintf("%%.%df",2); s.sprintf(fmt.c_str(),F);
                        TI->ColumnText->Add(s); CS->Style = ElhsOwnerDraw; }break;
    case BPID_BOOL: 	CS->CellType = sftUndef;    TI->ColumnText->Add	(BOOLString[((BP_BOOL*)value)->value]); 	CS->Style = ElhsOwnerDraw; break;
    case BPID_TOKEN:	CS->CellType = sftUndef; 	TI->ColumnText->Add	(GetToken((BP_TOKEN*)value,((BP_TOKEN*)value)->IDselected));	break;
	default: THROW2("BPID_????");
    }
    return TI;
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
        case BPID_BOOL2:
            R.Right	-=	10;
            R.Left 	+= 	1;
    		DrawText	(Surface->Handle, AnsiString(Item->ColumnText->Strings[0]).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            R.Left 	= 	R.Right;
            R.Right += 	10;
            DrawArrow	(Surface, eadDown, R, clWindowText, true);
        break;
        case BPID_INTEGER:
        case BPID_FLOAT:
        case BPID_FLOAT2:
        	if (seNumber->Tag!=(int)Item){
	            R.Right-=10;
    	        R.Left += 1;
    			DrawText(Surface->Handle, AnsiString(Item->ColumnText->Strings[0]).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            }else{
            	if (!seNumber->Visible) ShowLWNumber(R);
            }
        break;
        };
  	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// LW style inplace editor
//---------------------------------------------------------------------------
void TfrmShaderProperties::HideLWNumber()
{
    seNumber->Tag	= 0;
    seNumber->Hide	();
}
//---------------------------------------------------------------------------
void TfrmShaderProperties::PrepareLWNumber(TElTreeItem* item)
{
	DWORD type 		= item->Tag;
    switch (type){
    case BPID_FLOAT2:{
        float* V 			= (float*)item->Data; VERIFY(V);
        seNumber->MinValue 	= 0.;
        seNumber->MaxValue	= 1.;
        seNumber->Increment	= 0.01f;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 2;
    	seNumber->ValueType	= vtFloat;
	    seNumber->Value 	= *V;
    }break;
    case BPID_FLOAT:{
        BP_Float* V 		= (BP_Float*)item->Data; VERIFY(V);
        seNumber->MinValue 	= V->min;
        seNumber->MaxValue 	= V->max;
        seNumber->Increment	= (V->max-V->min)/100.;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 2;
    	seNumber->ValueType	= vtFloat;
	    seNumber->Value 	= V->value;
    }break;
    case BPID_INTEGER:{
    	BP_Integer* V 		= (BP_Integer*)item->Data; VERIFY(V);
		seNumber->MinValue 	= V->min;
        seNumber->MaxValue 	= V->max;
	    seNumber->Increment	= 1;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= V->value;
    }break;
    }
    seNumber->Tag 	= (int)item;
    tvProperties->Refresh();
}
void TfrmShaderProperties::ShowLWNumber(TRect& R)
{
    seNumber->Left 	= R.Left;
    seNumber->Top  	= R.Top+18;
    seNumber->Width	= R.Right-R.Left+2;
    seNumber->Height= R.Bottom-R.Top+3;
    seNumber->Show();
    seNumber->SetFocus();
}

void TfrmShaderProperties::ApplyLWNumber()
{
	TElTreeItem* item = (TElTreeItem*)seNumber->Tag;
    if (item){
		DWORD type = item->Tag;
	    switch (type){
		case BPID_FLOAT2:{
            item->ColumnText->Strings[0] = seNumber->Text;
	    	if (!fsimilar(*(float*)item->Data,seNumber->Value)){
		    	*(float*)item->Data = seNumber->Value;
	            Modified();
		    }
        }break;
		case BPID_FLOAT:{
        	BP_Float* V = (BP_Float*)item->Data;
            item->ColumnText->Strings[0] = seNumber->Text;
		    if (!fsimilar(V->value,seNumber->Value)){
			    V->value = seNumber->Value;
	            Modified();
		    }
    	}break;
		case BPID_INTEGER:{
        	BP_Integer* V = (BP_Integer*)item->Data;
            item->ColumnText->Strings[0] = seNumber->Text;
    		if (V->value != seNumber->Value){
	    		V->value = seNumber->Value;
	            Modified();
	    	}
        }break;
        }
    }
}

void __fastcall TfrmShaderProperties::seNumberExit(TObject *Sender)
{
	ApplyLWNumber();
	HideLWNumber();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::seNumberLWChange(TObject *Sender,
      int Val)
{
	ApplyLWNumber();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::seNumberKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (VK_RETURN==Key){
		ApplyLWNumber();
		HideLWNumber();
    }else if (VK_ESCAPE==Key){
		HideLWNumber();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderProperties::tvPropertiesMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	HideLWNumber();
	int HS;
	TElTreeItem* item = tvProperties->GetItemAt(X,Y,0,HS);
  	if ((HS==1)&&(Button==mbLeft)){
    	DWORD type = (DWORD)item->Tag;
		pmEnum->Tag = (int)item;
        switch(type){
		case BPID_BOOL2:
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
                mi->Caption = GetTokenItem(tokens,i).str;
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
            for (DWORD i=0; i<TSTRING_COUNT; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = TEXTUREString[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        case BPID_CONSTANT:
        case BPID_MATRIX:{
            pmEnum->Items->Clear();
            for (DWORD i=0; i<MCSTRING_COUNT; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = MCString[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        case BPID_INTEGER:
        case BPID_FLOAT:
        case BPID_FLOAT2:
        	PrepareLWNumber(item);
        break;
        };
        switch(type){
        case BPID_CONSTANT:
        case BPID_MATRIX:
        case BPID_TOKEN2:
        case BPID_FLAG:
		case BPID_BOOL2:
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
	            Modified();
            }
        break;
		case BPID_BOOL2:
            if ((*(BOOL*)item->Data) != mi->MenuIndex){
	            item->ColumnText->Strings[0]= mi->Caption;
	            (*(BOOL*)item->Data) = mi->MenuIndex;
	            Modified();
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
	            Modified();
            }
        }break;
		case BPID_TOKEN:{
        	BP_TOKEN* token_list			= (BP_TOKEN*)item->Data;
        	if (token_list->IDselected != GetTokenItem(token_list,mi->MenuIndex).ID){
	            item->ColumnText->Strings[0]= mi->Caption;
    	        token_list->IDselected 		= GetTokenItem(token_list,mi->MenuIndex).ID;
	            Modified();
            }
        }break;
		case BPID_TOKEN2:{
            xr_token* token_list 	   		= (xr_token*)item->ColumnText->Objects[0];
            if ((DWORD)(item->Data)	!= token_list[mi->MenuIndex].id){
	            item->ColumnText->Strings[0]= mi->Caption;
    	        *(LPDWORD)(item->Data)		= token_list[mi->MenuIndex].id;
	            Modified();
            }
        }break;
		case BPID_TEXTURE:{
			LPSTR tex = (LPSTR)item->Data;
        	if (mi->MenuIndex==0){
            	LPCSTR name = TfrmChoseItem::SelectTexture(false,tex);
//S оставить нужные фолдеры
            	if (name&&name[0]&&strcmp(tex,name)!=0){
					strcpy(tex,name);
		            item->ColumnText->Strings[0]= name;
		            Modified();
                }
            }else if (mi->MenuIndex>=2){
            	if (strcmp(tex,TEXTUREString[mi->MenuIndex])!=0){
					strcpy(tex,TEXTUREString[mi->MenuIndex]);
		            item->ColumnText->Strings[0]= TEXTUREString[mi->MenuIndex];
		            Modified();
                }
            }
        }break;
        case BPID_CONSTANT:{
			LPSTR nm = (LPSTR)item->Data;
        	if (mi->MenuIndex==0){
            	if (*nm=='$'){
//            	if (strcmp(nm,MCString[mi->MenuIndex])!=0){
					strcpy(nm,Tools.Engine.AppendConstant());
		            Modified();
                }
            }else if (mi->MenuIndex>=2){
            	if (*nm!='$') Tools.Engine.RemoveConstant(nm);
            	if (strcmp(nm,MCString[mi->MenuIndex])!=0){
					strcpy(nm,MCString[mi->MenuIndex]);
		            item->ColumnText->Strings[0]= MCString[mi->MenuIndex];
                }
				Modified();
            }
        }break;
        case BPID_MATRIX:{
			LPSTR nm = (LPSTR)item->Data;
        	if (mi->MenuIndex==0){
            	if (*nm=='$'){
					strcpy(nm,Tools.Engine.AppendMatrix());
		            item->ColumnText->Strings[0]= MCString[mi->MenuIndex];
                    AddMatrix(item,nm);
		            Modified();
                }
            }else if (mi->MenuIndex>=2){
            	if (*nm!='$'){
                	Tools.Engine.RemoveMatrix(nm);
                    RemoveMatrix(item);
                }
            	if (strcmp(nm,MCString[mi->MenuIndex])!=0){
					strcpy(nm,MCString[mi->MenuIndex]);
		            item->ColumnText->Strings[0]= MCString[mi->MenuIndex];
                }
				Modified();
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
	if (form&&!m_bFreeze){
    	switch(Tools.ActiveEditor()){
        case aeEngine:
            if (Tools.Engine.m_CurrentBlender){ // fill Tree
                CStream data(Tools.Engine.m_BlenderStream.pointer(), Tools.Engine.m_BlenderStream.size());
                data.Advance(sizeof(CBlender_DESC));
                DWORD type;
                char key[255];
                TElTreeItem* marker_node=0;
                TElTreeItem* node;
                form->tvProperties->IsUpdating = true;
                form->tvProperties->Items->Clear();

                form->AddItem(0,BPID_TYPE,"Type",(LPDWORD)Tools.Engine.m_CurrentBlender->getComment());

                while (!data.Eof()){
                    int sz=0;
                    type = data.Rdword();
                    data.RstringZ(key);
                    switch(type){
                    case BPID_MARKER:	break;
                    case BPID_TOKEN: 	sz=sizeof(BP_TOKEN)+sizeof(BP_TOKEN::Item)*(((BP_TOKEN*)data.Pointer())->Count); break;
                    case BPID_MATRIX:	sz=sizeof(string64);	break;
                    case BPID_CONSTANT:	sz=sizeof(string64); 	break;
                    case BPID_TEXTURE: 	sz=sizeof(string64); 	break;
                    case BPID_INTEGER: 	sz=sizeof(BP_Integer);	break;
                    case BPID_FLOAT: 	sz=sizeof(BP_Float); 	break;
                    case BPID_BOOL: 	sz=sizeof(BP_BOOL); 	break;
			        default: THROW2("BPID_????");
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
        break;
        case aeCompiler:
            if (Tools.Compiler.m_LibShader){ // fill Tree
                DWORD type;
                char key[255];
                TElTreeItem* marker_node=0;
                form->tvProperties->IsUpdating = true;
                form->tvProperties->Items->Clear();

                form->AddItem(0,BPID_TYPE,"Type",LPDWORD("Compiler shader"));

                Shader_xrLC& L = Tools.Compiler.m_EditShader;
			    form->AddItem(marker_node,BPID_FLOAT2,	"Translucency",	(LPDWORD)&L.vert_translucency );
			    form->AddItem(marker_node,BPID_FLOAT2,	"Ambient",		(LPDWORD)&L.vert_ambient );
			    form->AddItem(marker_node,BPID_FLOAT2,	"LM density",	(LPDWORD)&L.lm_density );
			    marker_node = form->AddItem(0,BPID_MARKER,	"Flags");
			    form->AddItem(marker_node,BPID_FLAG,	"Collision", 	(LPDWORD)&L.flags, (LPDWORD)Shader_xrLC::flCollision );
                form->AddItem(marker_node,BPID_FLAG,	"Rendering",    (LPDWORD)&L.flags, (LPDWORD)Shader_xrLC::flRendering );
                form->AddItem(marker_node,BPID_FLAG,	"OptimizeUV",   (LPDWORD)&L.flags, (LPDWORD)Shader_xrLC::flOptimizeUV );
                form->AddItem(marker_node,BPID_FLAG,	"Vertex light", (LPDWORD)&L.flags, (LPDWORD)Shader_xrLC::flLIGHT_Vertex );
                form->AddItem(marker_node,BPID_FLAG,	"Cast shadow",  (LPDWORD)&L.flags, (LPDWORD)Shader_xrLC::flLIGHT_CastShadow );
                form->AddItem(marker_node,BPID_FLAG,	"Sharp",        (LPDWORD)&L.flags, (LPDWORD)Shader_xrLC::flLIGHT_Sharp );

                form->tvProperties->FullExpand();
                form->tvProperties->IsUpdating = false;
            }else{
                form->tvProperties->Items->Clear();
            }
        break;
        }
	    m_bModified = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::CustomClick(TElTreeItem* item)
{
	DWORD type = item->Tag;
    switch (type){
    case BPID_WAVE:{
        if (TfrmShaderFunction::Run((WaveForm*)item->Data)==mrOk)
			Modified();
    }break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::ebPropertiesApplyClick(
      TObject *Sender)
{
	UI.Command(COMMAND_APPLY_CHANGES);
	Modified();
}
//---------------------------------------------------------------------------

void TfrmShaderProperties::Modified()
{
	m_bModified = true;
    Tools.Modified();
	UI.Command(COMMAND_APPLY_CHANGES);
}


