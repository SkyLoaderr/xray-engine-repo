//------------------------------------------------------------------------------
// - Events:
//	- OnBeforeEdit
// 	- OnDrawValue
//------------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "PropertiesList.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

#include "main.h"
#include "xr_tokens.h"
#include "ShaderFunction.h"
#include "ColorPicker.h"
#include "ChoseForm.h"
#include "FolderLib.h"
#include "NumericVector.h"
#include "TextForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_edit"
#pragma link "ElTreeStdEditors"
#pragma link "ElXPThemedControl"
#pragma link "multi_edit"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTree"
#pragma link "ElTreeStdEditors"
#pragma link "ElXPThemedControl"
#pragma link "multi_edit"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTreeAdvEdit"
#pragma resource "*.dfm"

#define TSTRING_COUNT 	4
const LPSTR TEXTUREString[TSTRING_COUNT]={"Custom...","-","$null","$base0"};

LPCSTR 	TokenValue::GetText(){
	int draw_val 	= *val;
    if (OnDrawValue)OnDrawValue(this, &draw_val);
	for(int i=0; token[i].name; i++) if (token[i].id==draw_val) return token[i].name;
    return 0;
}
LPCSTR 	TokenValue2::GetText(){
	DWORD draw_val 	= *val;
    if (OnDrawValue)OnDrawValue(this, &draw_val);
    if ((draw_val<0)||(draw_val>items.size())) return "";
	return items[draw_val].c_str();
}
LPCSTR TokenValue3::GetText(){
	VERIFY(*val<cnt);
	DWORD draw_val 	= *val;
    if (OnDrawValue)OnDrawValue(this, &draw_val);
    return items[draw_val].str;
}
AnsiString prop_draw_text;
LPCSTR	ListValue::GetText(){
    prop_draw_text=val;
    if (OnDrawValue)OnDrawValue(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
LPCSTR 	TextValue::GetText(){
    prop_draw_text=val;
    if (OnDrawValue)OnDrawValue(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
LPCSTR 	AnsiTextValue::GetText(){
    prop_draw_text=*val;
    if (OnDrawValue)OnDrawValue(this, &prop_draw_text);
    return prop_draw_text.c_str();
}

//---------------------------------------------------------------------------
TElTreeItem* __fastcall TfrmProperties::BeginEditMode(LPCSTR section)
{
	iFillMode++;
	tvProperties->IsUpdating = true;
	CancelEditControl();
    if (section) return FOLDER::FindFolder(tvProperties,section);
    return 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::EndEditMode(TElTreeItem* expand_node)
{
	iFillMode--;
	if (expand_node) expand_node->Expand(true);
    tvProperties->IsUpdating = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::BeginFillMode(const AnsiString& title, LPCSTR section)
{
	iFillMode++;
	tvProperties->IsUpdating = true;
	CancelEditControl();
    if (section){
    	TElTreeItem* node = FOLDER::FindFolder(tvProperties,section);
        if (node){
			ClearParams(node);
        	node->Delete();
        }
    }else{
		FOLDER::MakeFullName(tvProperties->Selected,0,last_selected_item);
        ClearParams();
	    tvProperties->Items->Clear();
    }

    Caption = title;
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::EndFillMode(bool bFullExpand)
{
	iFillMode--;
    bModified=false;
	if (bFullExpand) tvProperties->FullExpand();
    tvProperties->IsUpdating = false;
    tvProperties->Selected = FOLDER::FindItem(tvProperties,last_selected_item.c_str());
};
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::ResetProperties()
{
    for (PropValIt it=m_Params.begin(); it!=m_Params.end(); it++)
        (*it)->Reset();
}
//---------------------------------------------------------------------------
void TfrmProperties::ClearParams(TElTreeItem* node)
{
	if (node){
    	//S когда будут все итемы удалить у каждого
    	for (TElTreeItem* item=node; item; item=item->GetNext()){
			PropValue* V = (PropValue*)GetItemData(item);
            if (V){
	            PropValIt it=find(m_Params.begin(),m_Params.end(),V); VERIFY(it!=m_Params.end());
    	        if (it){
					m_Params.erase(it);
					_DELETE(V);
                }
            }
		}
    }else{
	    for (PropValIt it=m_Params.begin(); it!=m_Params.end(); it++)
    		_DELETE(*it);
		m_Params.clear();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::ClearProperties()
{
	tvProperties->IsUpdating = true;
    tvProperties->Items->Clear();
    tvProperties->IsUpdating = false;
    ClearParams();
}
//---------------------------------------------------------------------------

__fastcall TfrmProperties::TfrmProperties(TComponent* Owner)
	: TForm(Owner)
{
	bModified 		= false;
	iFillMode 		= 0;
    DEFINE_INI		(fsStorage);
	m_BMCheck 		= new Graphics::TBitmap();
    m_BMDot 		= new Graphics::TBitmap();
    m_BMEllipsis 	= new Graphics::TBitmap();
	m_BMCheck->LoadFromResourceName((DWORD)HInstance,"CHECK");
	m_BMDot->LoadFromResourceName((DWORD)HInstance,"DOT");
	m_BMEllipsis->LoadFromResourceName((DWORD)HInstance,"ELLIPSIS");
    seNumber->Parent= tvProperties;
    seNumber->Hide	();
}
//---------------------------------------------------------------------------

TfrmProperties* TfrmProperties::CreateProperties	(TWinControl* parent, TAlign align, TOnModifiedEvent modif, TOnItemFocused focused)
{
	TfrmProperties* props = new TfrmProperties(parent);
    props->OnModifiedEvent 	= modif;
    props->OnItemFocused    = focused;
    if (parent){
		props->Parent = parent;
    	props->Align = align;
	    props->BorderStyle = bsNone;
        props->ShowProperties();
        props->fsStorage->Active = false;
    }
	return props;
}

void TfrmProperties::DestroyProperties(TfrmProperties*& props)
{
	VERIFY(props);
	props->ClearProperties();
	props->Close();
    _DELETE(props);
}
void __fastcall TfrmProperties::ShowProperties(){
	Show();
}

void __fastcall TfrmProperties::ShowPropertiesModal(){
	ShowModal();
}

void __fastcall TfrmProperties::HideProperties(){
	Hide();
}

void __fastcall TfrmProperties::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    _DELETE(m_BMCheck);
    _DELETE(m_BMDot);
    _DELETE(m_BMEllipsis);
}
//---------------------------------------------------------------------------

TElTreeItem* __fastcall TfrmProperties::AddItem(TElTreeItem* parent, EPropType type, LPCSTR key, LPVOID value){
	R_ASSERT(iFillMode>0);
    TElTreeItem* TI     = tvProperties->Items->AddChildObject(parent,key,(TObject*)value);
    TI->Tag	            = type;
    TI->UseStyles 		= true;
    TElCellStyle* CS    = TI->AddStyle();
    CS->OwnerProps 		= true;
    CS->CellType 		= sftUndef;

    switch (type){
    case PROP_MARKER:		break;
    case PROP_MARKER2:		TI->ColumnText->Add(AnsiString((LPSTR)value)); 		break;
    case PROP_BOOL:			CS->Style = ElhsOwnerDraw; 							break;
    case PROP_WAVE:			TI->ColumnText->Add("[Wave]");						CS->Style = ElhsOwnerDraw; break;
    case PROP_COLOR:		CS->Style = ElhsOwnerDraw; 							break;
    case PROP_ANSI_TEXT:
    case PROP_TEXT:
    case PROP_FLAG:
    case PROP_TOKEN:
    case PROP_TOKEN2:
    case PROP_TOKEN3:
    case PROP_LIST:
    case PROP_VECTOR:
    case PROP_FLOAT:
    case PROP_TEXTURE:
	case PROP_TEXTURE2:
    case PROP_SH_ENGINE:
    case PROP_SH_COMPILE:
    case PROP_ANSI_TEXTURE:
    case PROP_ANSI_SH_ENGINE:
    case PROP_ANSI_SH_COMPILE:
    case PROP_INTEGER:{		PropValue*  P=(PropValue*)value; TI->ColumnText->Add(P->GetText()); CS->Style = ElhsOwnerDraw; }break;
    default: THROW2("PROP_????");
    }
    return TI;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::tvPropertiesClick(TObject *Sender)
{
	int HS;
    TElTreeItem* 	Item;
    TPoint			P;

  	GetCursorPos(&P);
  	P = tvProperties->ScreenToClient(P);
  	Item = tvProperties->GetItemAt(P.x, P.y, TSTItemPart(0), HS);
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
        case PROP_WAVE:{
            R.Right	-=	12;
            R.Left 	+= 	1;
            AnsiString name = Item->ColumnText->Strings[0];
    		DrawText	(Surface->Handle, name.IsEmpty()?"[none]":name.c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            R.Left	= R.Right;
        	Surface->CopyMode = cmSrcAnd;//cmSrcErase;
            Surface->Draw(R.Left+2,R.Top+5,m_BMEllipsis);
        }break;
        case PROP_COLOR:{
			Surface->Brush->Style = bsSolid;
 			Surface->Brush->Color = TColor(0x00000000);
            Surface->FrameRect(R);
            R.Right	-=	1;
            R.Left 	+= 	1;
            R.Top	+=	1;
            R.Bottom-= 	1;
		    Surface->Brush->Color = (TColor)rgb2bgr(*(LPDWORD)Item->Data);
            Surface->FillRect(R);
        }break;
        case PROP_FLAG:{
        	FlagValue* V=(FlagValue*)Item->Data;
        	Surface->CopyMode = cmSrcAnd;//cmSrcErase;
            if (*V->val&V->mask)Surface->Draw(R.Left,R.Top+3,m_BMCheck);
            else				Surface->Draw(R.Left,R.Top+3,m_BMDot);
        }break;
        case PROP_BOOL:
        	Surface->CopyMode = cmSrcAnd;//cmSrcErase;
            if (*(BOOL*)Item->Data)	Surface->Draw(R.Left,R.Top+3,m_BMCheck);
            else				   	Surface->Draw(R.Left,R.Top+3,m_BMDot);
        break;
        case PROP_TEXTURE:
        case PROP_TEXTURE2:
        case PROP_ANSI_TEXTURE:
        case PROP_ANSI_SH_ENGINE:
        case PROP_ANSI_SH_COMPILE:
        case PROP_SH_ENGINE:
        case PROP_SH_COMPILE:
            R.Right	-=	12;
            R.Left 	+= 	1;
    		DrawText	(Surface->Handle, ((PropValue*)Item->Data)->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            R.Left 	= 	R.Right;
        	Surface->CopyMode = cmSrcAnd;
            Surface->Draw(R.Left,R.Top+5,m_BMEllipsis);
        break;
		case PROP_TOKEN:
        case PROP_TOKEN2:
        case PROP_TOKEN3:
        case PROP_LIST:
            R.Right	-=	12;
            R.Left 	+= 	1;
    		DrawText	(Surface->Handle, ((PropValue*)Item->Data)->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            R.Left 	= 	R.Right;
            R.Right += 	10;
            DrawArrow	(Surface, eadDown, R, clWindowText, true);
        break;
        case PROP_ANSI_TEXT:
        case PROP_TEXT:
        	if (edText->Tag!=(int)Item){
                R.Right-= 1;
                R.Left += 1;
                DrawText(Surface->Handle, ((PropValue*)Item->Data)->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            }else{
            	if (!edText->Visible) ShowLWText(R);
            }
        break;
        case PROP_VECTOR:
            R.Right-= 1;
            R.Left += 1;
            DrawText(Surface->Handle, ((PropValue*)Item->Data)->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        break;
        case PROP_INTEGER:
        case PROP_FLOAT:
        	if (seNumber->Tag!=(int)Item){
	            R.Right-= 1;
    	        R.Left += 1;
    			DrawText(Surface->Handle, ((PropValue*)Item->Data)->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            }else{
            	if (!seNumber->Visible) ShowLWNumber(R);
            }
        break;
        };
  	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::tvPropertiesMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	CancelEditControl();
	int HS;
	TElTreeItem* item = tvProperties->GetItemAt(X,Y,TSTItemPart(0),HS);
  	if ((HS==1)&&(Button==mbLeft)){
    	DWORD type = (DWORD)item->Tag;
		pmEnum->Tag = (int)item;
        switch(type){
		case PROP_FLAG:{
        	FlagValue*	V 				= (FlagValue*)item->Data;
            DWORD old_val 				= *V->val;
            DWORD new_val 				= *V->val;
            if (new_val&V->mask)new_val &=~V->mask;
            else				new_val |= V->mask;
			if (V->OnAfterEdit) V->OnAfterEdit(item,V,&new_val);
			*V->val 			= new_val;
            if (new_val!=old_val){
                Modified();
	            RefreshProperties();
            }
        }break;
		case PROP_BOOL:{
        	BOOL& V=*(BOOL*)item->Data;
            V=!V;
			Modified();
            RefreshProperties();
        }break;
		case PROP_TOKEN:{
            pmEnum->Items->Clear();
            TokenValue* T = (TokenValue*)item->Data;
            xr_token* token_list = T->token;
			for(int i=0; token_list[i].name; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = token_list[i].name;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        case PROP_TOKEN2:{
            pmEnum->Items->Clear();
            TokenValue2* T = (TokenValue2*)item->Data;
            AStringVec& lst = T->items;
			for(AStringIt it=lst.begin(); it!=lst.end(); it++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = *it;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
		case PROP_TOKEN3:{
            pmEnum->Items->Clear();
            TokenValue3* T = (TokenValue3*)item->Data;
            for (DWORD i=0; i<T->cnt; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = T->items[i].str;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
		case PROP_LIST:{
            pmEnum->Items->Clear();
            ListValue* T = (ListValue*)item->Data;
            AStringVec& lst = T->items;
			for(AStringIt it=lst.begin(); it!=lst.end(); it++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = *it;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        case PROP_VECTOR: 			VectorClick(item); 			break;
        case PROP_WAVE: 			CustomClick(item); 			break;
        case PROP_COLOR: 			ColorClick(item); 			break;
        case PROP_TEXTURE:
        case PROP_SH_ENGINE:
        case PROP_SH_COMPILE:		CustomTextClick(item);      break;
        case PROP_ANSI_TEXTURE:
        case PROP_ANSI_SH_ENGINE:
        case PROP_ANSI_SH_COMPILE:	CustomAnsiTextClick(item);	break;
        case PROP_INTEGER:
        case PROP_FLOAT:
        	PrepareLWNumber(item);
        break;
        case PROP_ANSI_TEXT:
        case PROP_TEXT:
        	PrepareLWText(item);
        break;
		case PROP_TEXTURE2:{
            pmEnum->Items->Clear();
            for (DWORD i=0; i<TSTRING_COUNT; i++){
                TMenuItem* mi = new TMenuItem(0);
                mi->Caption = TEXTUREString[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        };
        switch(type){
        case PROP_TOKEN:
        case PROP_TOKEN2:
        case PROP_TOKEN3:
        case PROP_LIST:
        case PROP_TEXTURE2:
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
		case PROP_TOKEN:{
        	TokenValue* V				= (TokenValue*)item->Data;
            xr_token* token_list 	   	= V->token;
            DWORD old_val				= *V->val;
            DWORD new_val				= token_list[mi->MenuIndex].id;
			if (V->OnAfterEdit) V->OnAfterEdit(item,V,&new_val);
			*V->val						= new_val;
            if (old_val	!= new_val)		Modified();
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_TOKEN2:{
        	TokenValue2* V				= (TokenValue2*)item->Data;
            DWORD old_val				= *V->val;
            DWORD new_val				= mi->MenuIndex;
			if (V->OnAfterEdit) V->OnAfterEdit(item,V,&new_val);
			*V->val						= new_val;
            if (old_val	!= new_val)		Modified();
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_TOKEN3:{
        	TokenValue3* V				= (TokenValue3*)item->Data;
            DWORD old_val				= *V->val;
            DWORD new_val				= V->items[mi->MenuIndex].ID;
			if (V->OnAfterEdit) V->OnAfterEdit(item,V,&new_val);
			*V->val						= new_val;
            if (old_val	!= new_val)		Modified();
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_LIST:{
        	ListValue* V				= (ListValue*)item->Data;
            AStringVec& lst				= V->items;
            string256 old_val;	strcpy(old_val,V->val);
            string256 new_val;	strcpy(new_val,lst[mi->MenuIndex].c_str());
			if (V->OnAfterEdit) V->OnAfterEdit(item,V,&new_val);
			strcpy(V->val,new_val);
            if (strcmp(old_val,new_val))Modified();
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_TEXTURE2:{
        	TextValue* V				= (TextValue*)item->Data; 
			AnsiString old_val			= V->val;
			AnsiString edit_val			= V->val;
			if (V->OnBeforeEdit) 		V->OnBeforeEdit(item,V,&edit_val);
            LPCSTR new_val 				= 0;
        	if (mi->MenuIndex==0){
            	new_val 				= TfrmChoseItem::SelectTexture(false,edit_val.c_str(),true);
            }else if (mi->MenuIndex>=2){
            	new_val					= TEXTUREString[mi->MenuIndex];
            }
            if (new_val){
                edit_val			= new_val;
                if (V->OnAfterEdit) V->OnAfterEdit(item,V,&edit_val);
                strcpy(V->val,edit_val.c_str());
                if (edit_val!=old_val)	Modified();
                item->ColumnText->Strings[0]= V->GetText();
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
			Modified();
    }break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::ColorClick(TElTreeItem* item)
{
	DWORD type = item->Tag;
    LPDWORD color = (LPDWORD)item->Data;
    VERIFY(type==PROP_COLOR);
	if (SelectColor(color)){
    	item->RedrawItem(true);
		Modified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::VectorClick(TElTreeItem* item)
{
	DWORD type = item->Tag;
    VERIFY(type==PROP_VECTOR);

    VectorValue* V = (VectorValue*)item->Data;
    Fvector edit_val=*V->val;
    Fvector old_val=*V->val;
	if (V->OnBeforeEdit) 	V->OnBeforeEdit(item,V,&edit_val);
    Fvector mn={V->lim_mn,V->lim_mn,V->lim_mn},mx={V->lim_mx,V->lim_mx,V->lim_mx};
	if (NumericVectorRun(AnsiString(item->Text).c_str(),&edit_val,V->dec,&edit_val,&mn,&mx)){
        if (V->OnAfterEdit) V->OnAfterEdit(item,V,&edit_val);
		*V->val = edit_val;
        if (!old_val.similar(edit_val)){
			item->RedrawItem(true);
            Modified();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::CustomTextClick(TElTreeItem* item)
{
	TextValue* V			= (TextValue*)item->Data;
	AnsiString old_val		= V->val;
	AnsiString edit_val		= V->val;
	if (V->OnBeforeEdit) 	V->OnBeforeEdit(item,V,&edit_val);
    LPCSTR new_val=0;
    switch (item->Tag){
    case PROP_SH_ENGINE:	new_val	= TfrmChoseItem::SelectShader(edit_val.c_str());		break;
	case PROP_SH_COMPILE:	new_val	= TfrmChoseItem::SelectShaderXRLC(edit_val.c_str());	break;
    case PROP_TEXTURE:		new_val = TfrmChoseItem::SelectTexture(false,edit_val.c_str(),true);break;
    default: THROW2("Unknown props");
    }
    if (new_val){
        edit_val				= new_val;
        if (V->OnAfterEdit) 	V->OnAfterEdit(item,V,&edit_val);
		strcpy(V->val,edit_val.c_str());
        if (edit_val!=old_val)	Modified();
        item->ColumnText->Strings[0]= V->GetText();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::CustomAnsiTextClick(TElTreeItem* item)
{
	AnsiTextValue* V		= (AnsiTextValue*)item->Data;
	AnsiString old_val		= *V->val;
	AnsiString edit_val		= *V->val;
	if (V->OnBeforeEdit) 	V->OnBeforeEdit(item,V,&edit_val);
    LPCSTR new_val=0;
    switch (item->Tag){
    case PROP_ANSI_SH_ENGINE:	new_val	= TfrmChoseItem::SelectShader(edit_val.c_str());			break;
	case PROP_ANSI_SH_COMPILE:	new_val	= TfrmChoseItem::SelectShaderXRLC(edit_val.c_str());		break;
    case PROP_ANSI_TEXTURE:		new_val = TfrmChoseItem::SelectTexture(false,edit_val.c_str(),true);break;
    default: THROW2("Unknown props");
    }
    if (new_val){
        edit_val				= new_val;
        if (V->OnAfterEdit) 	V->OnAfterEdit(item,V,&edit_val);
		*V->val					= edit_val;
        if (old_val!=edit_val)	Modified();
        item->ColumnText->Strings[0]= V->GetText();
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// LW style inplace editor
//---------------------------------------------------------------------------
void TfrmProperties::CancelLWNumber()
{
    HideLWNumber();
}

void TfrmProperties::HideLWNumber()
{
    seNumber->Tag	= 0;
    seNumber->Hide	();
}
//---------------------------------------------------------------------------
void TfrmProperties::PrepareLWNumber(TElTreeItem* item)
{
	DWORD type 		= item->Tag;
    switch (type){
    case PROP_INTEGER:{
        IntValue* V 		= (IntValue*)item->Data; VERIFY(V);
        int edit_val        = *V->val;
        if (V->OnBeforeEdit) V->OnBeforeEdit(item,V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_FLOAT:{
        FloatValue* V 		= (FloatValue*)item->Data; VERIFY(V);
        float edit_val		= *V->val;
        if (V->OnBeforeEdit) V->OnBeforeEdit(item,V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.1f;
	    seNumber->Decimal  	= V->dec;
    	seNumber->ValueType	= vtFloat;
	    seNumber->Value 	= edit_val;
    }break;
    }
    seNumber->Tag 	= (int)item;
    tvProperties->Refresh();
}
void TfrmProperties::ShowLWNumber(TRect& R)
{
    seNumber->Left 	= R.Left;
    seNumber->Top  	= R.Top+tvProperties->HeaderHeight;
    seNumber->Width	= R.Right-R.Left+2;
    seNumber->Height= R.Bottom-R.Top+2;
    seNumber->Show();
    seNumber->SetFocus();
}

void TfrmProperties::ApplyLWNumber()
{
	TElTreeItem* item = (TElTreeItem*)seNumber->Tag;
    if (item){
		DWORD type = item->Tag;
        seNumber->Update();
	    switch (type){
    	case PROP_INTEGER:{
	        IntValue* V 	= (IntValue*)item->Data; VERIFY(V);
            int old_val		= *V->val;
            int new_val		= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(item,V,&new_val);
			*V->val 		= new_val;
            if (old_val != new_val)	Modified();
            item->ColumnText->Strings[0] = V->GetText();
        }break;
	    case PROP_FLOAT:{
	        FloatValue* V 	= (FloatValue*)item->Data; VERIFY(V);
            float old_val	= *V->val;
            float new_val	= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(item,V,&new_val);
			*V->val 		= new_val;
		    if (!fsimilar(old_val,new_val)) Modified();
            item->ColumnText->Strings[0] = V->GetText();
        }break;
    	}
    }
}

void __fastcall TfrmProperties::seNumberExit(TObject *Sender)
{
	ApplyLWNumber();
	HideLWNumber();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::seNumberKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (VK_RETURN==Key){
		ApplyLWNumber();
		HideLWNumber();
    }else if (VK_ESCAPE==Key){
		CancelLWNumber();
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Textinplace editor
//---------------------------------------------------------------------------
void TfrmProperties::CancelLWText()
{
    HideLWText();
}

void TfrmProperties::HideLWText()
{
    edText->Tag	= 0;
    edText->Hide	();
}
//---------------------------------------------------------------------------
void TfrmProperties::PrepareLWText(TElTreeItem* item)
{
	DWORD type 		= item->Tag;
    switch (type){
    case PROP_ANSI_TEXT:{
        AnsiTextValue* V	= (AnsiTextValue*)item->Data; VERIFY(V);
		AnsiString edit_val;
    	edit_val=*V->val;
        if (V->OnBeforeEdit) V->OnBeforeEdit(item,V,&edit_val);
	    edText->Text 		= edit_val;
    }break;
    case PROP_TEXT:{
        TextValue* V 		= (TextValue*)item->Data; VERIFY(V);
		AnsiString edit_val;
    	edit_val=V->val;
        if (V->OnBeforeEdit) V->OnBeforeEdit(item,V,&edit_val);
	    edText->Text 		= edit_val;
    }break;
    }
    edText->Tag 	= (int)item;
    tvProperties->Refresh();
}
void TfrmProperties::ShowLWText(TRect& R)
{
    edText->Left 	= R.Left-1;
    edText->Top  	= R.Top+tvProperties->HeaderHeight;
    edText->Width	= R.Right-R.Left+0;
    edText->Height	= R.Bottom-R.Top+2;
    edText->Show	();
    edText->SetFocus();
}

void TfrmProperties::ApplyLWText()
{
	TElTreeItem* item = (TElTreeItem*)edText->Tag;
    if (item){
		DWORD type = item->Tag;
        edText->Update();
	    switch (type){
        case PROP_ANSI_TEXT:{
	        AnsiTextValue* V 	= (AnsiTextValue*)item->Data; VERIFY(V);
			AnsiString new_val	= edText->Text;
            edText->MaxLength	= 0;
			AnsiString old_val	= *V->val;
			if (V->OnAfterEdit) V->OnAfterEdit(item,V,&new_val);
			*V->val				= new_val;
            if (new_val != old_val)	Modified();
            item->ColumnText->Strings[0] = V->GetText();
        }break;
    	case PROP_TEXT:{
	        TextValue* V 		= (TextValue*)item->Data; VERIFY(V);
            edText->MaxLength	= V->lim;
			AnsiString new_val=edText->Text;
			AnsiString old_val=V->val;
			if (V->OnAfterEdit) V->OnAfterEdit(item,V,&new_val);
			strcpy(V->val,new_val.c_str());
            if (new_val != old_val)	Modified();
            item->ColumnText->Strings[0] = V->GetText();
        }break;
    	}
    }
}

void __fastcall TfrmProperties::edTextExit(TObject *Sender)
{
	ApplyLWText();
	HideLWText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::edTextKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (VK_RETURN==Key){
		ApplyLWText();
		HideLWText();
    }else if (VK_ESCAPE==Key){
		CancelLWText();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::edTextDblClick(TObject *Sender)
{
	TElTreeItem* item = (TElTreeItem*)edText->Tag;
    if (item){
		DWORD type = item->Tag;
        edText->Update();
	    switch (type){
    	case PROP_TEXT:{
	        TextValue* V 	= (TextValue*)item->Data; VERIFY(V);
			AnsiString new_val	= edText->Text;
			if (TfrmText::Run(new_val,AnsiString(item->Text).c_str(),false,V->lim)) edText->Text = new_val;
        }break;
    	case PROP_ANSI_TEXT:{
			AnsiString new_val	= edText->Text;
			if (TfrmText::Run(new_val,AnsiString(item->Text).c_str())) edText->Text = new_val;
        }break;
    	}
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::tvPropertiesItemFocused(TObject *Sender)
{
	if (OnItemFocused) OnItemFocused(tvProperties->Selected);
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::tvPropertiesHeaderColumnResize(
      TObject *Sender, int SectionIndex)
{
	ApplyEditControl();
}
//---------------------------------------------------------------------------

void TfrmProperties::ApplyEditControl()
{
	ApplyLWText		();
	HideLWText		();
	ApplyLWNumber	();
	HideLWNumber	();
}
//---------------------------------------------------------------------------

void TfrmProperties::CancelEditControl()
{
	CancelLWNumber();
	CancelLWText();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::FormDeactivate(TObject *Sender)
{
	ApplyEditControl();
}
//---------------------------------------------------------------------------

