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
#include "xr_trims.h"
#include "ui_main.h"
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
//---------------------------------------------------------------------------
TElTreeItem* __fastcall TProperties::BeginEditMode(LPCSTR section)
{
	tvProperties->IsUpdating = true;
	CancelEditControl();
    if (section) return FHelper.FindFolder(tvProperties,section);
    return 0;
}
//---------------------------------------------------------------------------

void __fastcall TProperties::EndEditMode(TElTreeItem* expand_node)
{
	if (expand_node) expand_node->Expand(true);
    tvProperties->IsUpdating = false;
}
//---------------------------------------------------------------------------

void __fastcall TProperties::BeginFillMode(const AnsiString& title, LPCSTR section)
{
	tvProperties->IsUpdating = true;
	CancelEditControl();
    if (section){
    	TElTreeItem* node = FHelper.FindFolder(tvProperties,section);
        if (node){
			ClearParams(node);
        	node->Delete();
        }
    }else{
		FHelper.MakeFullName(tvProperties->Selected,0,last_selected_item);
        ClearParams();
	    tvProperties->Items->Clear();
    }

    Caption = title;
}
//---------------------------------------------------------------------------
void __fastcall TProperties::EndFillMode(bool bFullExpand)
{
    bModified=false;
    // end fill mode
	if (bFullExpand) tvProperties->FullExpand();
    tvProperties->IsUpdating 	= false;
    FHelper.RestoreSelection	(tvProperties,last_selected_item.c_str());
};
//---------------------------------------------------------------------------
void TProperties::ClearParams(TElTreeItem* node)
{
	if (node){
    	//S когда будут все итемы удалить у каждого
/*
//s        
    	for (TElTreeItem* item=node; item; item=item->GetNext()){
			PropValue* V = (PropValue*)GetItemData(item);
            if (V){
	            PropValuePairIt it=find(m_Values.begin(),m_Values.end(),V); VERIFY(it!=m_Values.end());
    	        if (it){
					m_Values.erase(it);
					_DELETE(V);
                }
            }
		}
*/
    }else{
	    for (PropItemIt it=m_Items.begin(); it!=m_Items.end(); it++)
    		_DELETE	(*it);
		m_Items.clear();
    }
}
//---------------------------------------------------------------------------
void __fastcall TProperties::ResetItems()
{
    for (PropItemIt it=m_Items.begin(); it!=m_Items.end(); it++)
        (*it)->ResetValues();
}
//---------------------------------------------------------------------------
void __fastcall TProperties::ClearProperties()
{
	tvProperties->IsUpdating = true;
    tvProperties->Items->Clear();
    tvProperties->IsUpdating = false;
    ClearParams();
}
//---------------------------------------------------------------------------

__fastcall TProperties::TProperties(TComponent* Owner)
	: TForm(Owner)
{
	bModified 		= false;
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

TProperties* TProperties::CreateForm(TWinControl* parent, TAlign align, TOnModifiedEvent modif, TOnItemFocused focused, TOnCloseEvent on_close)
{
	TProperties* props 			= new TProperties(parent);
    props->OnModifiedEvent 		= modif;
    props->OnItemFocused    	= focused;
    props->OnCloseEvent			= on_close;
    if (parent){
		props->Parent 			= parent;
    	props->Align 			= align;
	    props->BorderStyle 		= bsNone;
        props->ShowProperties	();
        props->fsStorage->Active= false;
    }
	return props;
}

void TProperties::DestroyForm(TProperties*& props)
{
	VERIFY(props);
	// apply edit controls
	props->ApplyEditControl();
    // destroy forms
	props->ClearProperties();
	props->Close();
    _DELETE(props);
}
void __fastcall TProperties::ShowProperties(){
	Show();
}

void __fastcall TProperties::ShowPropertiesModal(){
	ShowModal();
}

void __fastcall TProperties::HideProperties(){
	Hide();
}

void __fastcall TProperties::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    if (OnCloseEvent) OnCloseEvent();
	ClearParams	();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::FormDestroy(TObject *Sender)
{
    _DELETE		(m_BMCheck);
    _DELETE		(m_BMDot);
    _DELETE		(m_BMEllipsis);
}
//---------------------------------------------------------------------------

TElTreeItem* __fastcall TProperties::AddItem(TElTreeItem* parent, LPCSTR key, LPVOID value, PropValue* prop)
{
	THROW2("AddItem");
/*    
	prop->InitFirst		(value);
    prop->item			= tvProperties->Items->AddChild(parent,key);
    prop->item->Tag	    = (int)prop;
    prop->item->UseStyles=true;
    TElCellStyle* CS    = prop->item->AddStyle();
    CS->OwnerProps 		= true;
    CS->CellType 		= sftUndef;
    CS->Style 			= ElhsOwnerDraw;
    prop->item->ColumnText->Add(prop->GetText());

    return prop->item;
*/
	return 0;
}
//---------------------------------------------------------------------------

void __fastcall TProperties::AssignItems(PropItemVec& items, bool full_expand, const AnsiString& title)
{
	// begin fill mode
	tvProperties->IsUpdating = true;
	CancelEditControl();
    // clear values
    if (tvProperties->Selected) FHelper.MakeFullName(tvProperties->Selected,0,last_selected_item);
    ClearParams();
    tvProperties->Items->Clear();
    // fill values
	string128 fld;
    m_Items					= items;
	for (PropItemIt it=m_Items.begin(); it!=m_Items.end(); it++){
    	PropItem* prop		= *it;
        prop->item			= FHelper.AppendObject(tvProperties,prop->key);
        prop->item->Tag	    = (int)prop;
        prop->item->UseStyles=true;
        prop->item->CheckBoxEnabled = prop->m_Flags.is(PropItem::flShowCB);
        prop->item->ShowCheckBox 	= prop->m_Flags.is(PropItem::flShowCB);
        prop->item->CheckBoxState 	= (TCheckBoxState)prop->m_Flags.is(PropItem::flCBChecked);
        TElCellStyle* CS    = prop->item->AddStyle();
        CS->OwnerProps 		= true;
        CS->CellType 		= sftUndef;
        CS->Style 			= ElhsOwnerDraw;
        prop->item->ColumnText->Add(prop->GetText());
    }

    // end fill mode
    bModified=false;
	if (full_expand) tvProperties->FullExpand();

    TElTreeItem* sel_node	= FHelper.ExpandItem(tvProperties,last_selected_item.c_str());

    tvProperties->IsUpdating= false;

    FHelper.RestoreSelection(tvProperties,sel_node);

    Caption = title;
}
//---------------------------------------------------------------------------

void __fastcall TProperties::tvPropertiesClick(TObject *Sender)
{
	TSTItemPart 	IP=(TSTItemPart)0;
    int				HC=0;
    TElTreeItem* 	Item;
    TPoint			P;

  	GetCursorPos(&P);
  	P = tvProperties->ScreenToClient(P);
  	Item = tvProperties->GetItemAt(P.x, P.y, IP, HC);
	if (HC==1)
    	tvProperties->EditItem(Item, HC);
}
//---------------------------------------------------------------------------
void __fastcall TProperties::tvPropertiesItemDraw(TObject *Sender,
      TElTreeItem *Item, TCanvas *Surface, TRect &R, int SectionIndex)
{
	TRect  R1;
	Surface->Brush->Style = bsClear;
  	if (SectionIndex == 1){
    	PropItem* prop 				= (PropItem*)Item->Tag;
        DWORD type 					= prop->type;
        if (!prop->m_Flags.is(PropItem::flDisabled)){
            Surface->Font->Color 	= clBlack;
            Surface->Font->Style 	= TFontStyles();
        }else{
            Surface->Font->Color 	= clSilver;
            Surface->Font->Style 	= TFontStyles()<< fsBold;
        }
        if (prop->m_Flags.is(PropItem::flMixed)){ 
            TColor C 		= Surface->Brush->Color;
            TBrushStyle S 	= Surface->Brush->Style;
            Surface->Brush->Style = bsSolid;
            Surface->Brush->Color = Item->Selected?(TColor)0x00A0A0A0:(TColor)0x008E8E8E;
            TRect r	=	R;
            r.Left 	-= 	1;
            r.Right	+=	1;
            r.Bottom+= 	2;
            r.Top	-=	1;
            Surface->FillRect(r);
            Surface->Brush->Color 	= C;
            Surface->Brush->Style	= S;
            R.Right-= 1;
            R.Left += 1;
            DrawText	(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        }else{
            TRect src_rect = R;
            switch(type){
            case PROP_CAPTION:
                Surface->Font->Color = clSilver;
                R.Right-= 1;
                R.Left += 1;
                DrawText	(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            break;
            case PROP_FCOLOR:{
                Surface->Brush->Style = bsSolid;
                Surface->Brush->Color = TColor(0x00000000);
                Surface->FrameRect(R);
                R.Right	-=	1;
                R.Left 	+= 	1;
                R.Top	+=	1;
                R.Bottom-= 	1;
                ColorValue* V			= dynamic_cast<ColorValue*>(prop->GetFrontValue()); R_ASSERT(V);
                Surface->Brush->Color 	= (TColor)(V->GetValue()).get_windows();
                Surface->FillRect(R);
            }break;
            case PROP_COLOR:{
                Surface->Brush->Style = bsSolid;
                Surface->Brush->Color = TColor(0x00000000);
                Surface->FrameRect(R);
                R.Right	-=	1;
                R.Left 	+= 	1;
                R.Top	+=	1;
                R.Bottom-= 	1;
                U32Value* V=dynamic_cast<U32Value*>(prop->GetFrontValue()); R_ASSERT(V);
                u32* C 	= (U32Value::TYPE*)V->GetValue();
                Surface->Brush->Color = (TColor)rgb2bgr(*C);
                Surface->FillRect(R);
            }break;
            case PROP_FLAG8:{
                Flag8Value* V		= dynamic_cast<Flag8Value*>(prop->GetFrontValue()); R_ASSERT(V);
                Surface->CopyMode 	= cmSrcAnd;//cmSrcErase;
                if (V->GetValueEx())Surface->Draw(R.Left,R.Top+3,m_BMCheck);
                else 				Surface->Draw(R.Left,R.Top+3,m_BMDot);
            }break;
            case PROP_FLAG16:{
                Flag16Value* V		= dynamic_cast<Flag16Value*>(prop->GetFrontValue()); R_ASSERT(V);
                Surface->CopyMode 	= cmSrcAnd;//cmSrcErase;
                if (V->GetValueEx())Surface->Draw(R.Left,R.Top+3,m_BMCheck);
                else 				Surface->Draw(R.Left,R.Top+3,m_BMDot);
            }break;
            case PROP_FLAG32:{
                Flag32Value* V		= dynamic_cast<Flag32Value*>(prop->GetFrontValue()); R_ASSERT(V);
                Surface->CopyMode 	= cmSrcAnd;//cmSrcErase;
                if (V->GetValueEx())Surface->Draw(R.Left,R.Top+3,m_BMCheck);
                else 				Surface->Draw(R.Left,R.Top+3,m_BMDot);
            }break;
            case PROP_BOOLEAN:{
                Surface->CopyMode = cmSrcAnd;//cmSrcErase;
                BOOLValue* V		= dynamic_cast<BOOLValue*>(prop->GetFrontValue()); R_ASSERT(V);
                if (V->GetValue()) 	Surface->Draw(R.Left,R.Top+3,m_BMCheck);
                else			   	Surface->Draw(R.Left,R.Top+3,m_BMDot);
            }break;
            case PROP_WAVE:
            case PROP_LIBPS:
            case PROP_LIBSOUND:
            case PROP_LIGHTANIM:
            case PROP_LIBOBJECT:
            case PROP_GAMEOBJECT:
            case PROP_ENTITY:
            case PROP_TEXTURE:
            case PROP_TEXTURE2:
			case PROP_GAMEMTL:
            case PROP_A_LIBOBJECT:
            case PROP_A_GAMEMTL:
            case PROP_A_LIBPS:
            case PROP_A_LIBSOUND:
            case PROP_A_TEXTURE:
            case PROP_A_ESHADER:
            case PROP_A_CSHADER:
            case PROP_ESHADER:
            case PROP_CSHADER:
                R.Right	-=	12;
                R.Left 	+= 	1;
                DrawText	(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
                R.Left 	= 	R.Right;
                Surface->CopyMode = cmSrcAnd;
                Surface->Draw(R.Left+1,R.Top+5,m_BMEllipsis);
            break;
            case PROP_TOKEN:
            case PROP_TOKEN2:
            case PROP_TOKEN3:
            case PROP_LIST:
                R.Right	-=	12;
                R.Left 	+= 	1;
                DrawText	(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
                R.Left 	= 	R.Right;
                R.Right += 	10;
                DrawArrow	(Surface, eadDown, R, clWindowText, true);
            break;
            case PROP_A_TEXT:
            case PROP_TEXT:
                if (edText->Tag!=(int)Item){
                    R.Right-= 1;
                    R.Left += 1;
                    DrawText(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
                }
            break;
            case PROP_VECTOR:
                R.Right-= 1;
                R.Left += 1;
                DrawText(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            break;
            case PROP_U8:
            case PROP_U16:
            case PROP_U32:
            case PROP_S8:
            case PROP_S16:
            case PROP_S32:
            case PROP_FLOAT:
                if (seNumber->Tag!=(int)Item){
                    R.Right-= 1;
                    R.Left += 1;
                    if (type==PROP_FLOAT){
                    	int y=0;
                    }
                    DrawText(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
                }
            break;
	        };
            // draw btn border
	        if (prop->m_Flags.is(PropItem::flDrawBtnBorder)){
            	src_rect.left+=1;
            	src_rect.top+=1;
            	src_rect.right-=1;
            	src_rect.bottom-=1;
	            Surface->Brush->Style = bsSolid;
    	        Surface->Brush->Color = Item->Selected?(TColor)0x00A0A0A0:(TColor)0x008E8E8E;
                DrawEdge(Canvas->Handle,&src_rect,BDR_RAISEDINNER,BF_MIDDLE);
//	            Surface->FillRect(src_rect);
            }
        }
        // show LW edit
        if (!prop->m_Flags.is(PropItem::flDisabled)){
            switch(type){
            case PROP_A_TEXT:
            case PROP_TEXT:
                if (edText->Tag==(int)Item) if (!edText->Visible) ShowLWText(R);
            break;
            case PROP_U8:
            case PROP_U16:
            case PROP_U32:
            case PROP_S8:
            case PROP_S16:
            case PROP_S32:
            case PROP_FLOAT:
                if (seNumber->Tag==(int)Item) if (!seNumber->Visible) ShowLWNumber(R);
            break;
            };
        }
  	}
}
//---------------------------------------------------------------------------
void __fastcall TProperties::tvPropertiesMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	CancelEditControl();
    
	TSTItemPart 	IP=(TSTItemPart)0;
    int				HC=0;
	TElTreeItem* item = tvProperties->GetItemAt(X,Y,IP,HC);
  	if (item&&(HC==1)&&(Button==mbLeft)){
    	if (!item->Enabled) return;
		PropItem* prop = (PropItem*)item->Tag;
        if (!prop||prop->m_Flags.is(PropItem::flDisabled)) return;
		pmEnum->Tag = (int)item;
        switch(prop->type){
		case PROP_FLAG8:{
			Flag8Value* V				= dynamic_cast<Flag8Value*>(prop->GetFrontValue()); R_ASSERT(V);
            BOOL new_val 				= !V->GetValueEx();
			prop->OnAfterEdit			(&new_val);
            if (prop->ApplyValue(&new_val)){
				prop->OnChange			();
                Modified				();
	            RefreshForm				();
            }
        }break;
		case PROP_FLAG16:{
			Flag16Value* V				= dynamic_cast<Flag16Value*>(prop->GetFrontValue()); R_ASSERT(V);
            BOOL new_val 				= !V->GetValueEx();
			prop->OnAfterEdit			(&new_val);
            if (prop->ApplyValue(&new_val)){
				prop->OnChange			();
                Modified				();
	            RefreshForm				();
            }
        }break;
		case PROP_FLAG32:{
			Flag32Value* V				= dynamic_cast<Flag32Value*>(prop->GetFrontValue()); R_ASSERT(V);
            BOOL new_val 				= !V->GetValueEx();
			prop->OnAfterEdit			(&new_val);
            if (prop->ApplyValue(&new_val)){
				prop->OnChange			();
                Modified				();
	            RefreshForm				();
            }
        }break;
		case PROP_BOOLEAN:{
			BOOLValue* V				= dynamic_cast<BOOLValue*>(prop->GetFrontValue()); R_ASSERT(V);
            BOOL new_val 				= !V->GetValue();
			prop->OnAfterEdit			(&new_val);
            if (prop->ApplyValue(&new_val)){
				prop->OnChange			();
                Modified				();
	            RefreshForm				();
            }
        }break;
		case PROP_TOKEN:{
            pmEnum->Items->Clear();
			TokenValue* T				= dynamic_cast<TokenValue*>(prop->GetFrontValue()); R_ASSERT(T);
            xr_token* token_list 		= T->token;
			TMenuItem* mi 				= new TMenuItem(0);
			mi->Caption 				= "-";
			pmEnum->Items->Add			(mi);
			for(int i=0; token_list[i].name; i++){
                mi 			= new TMenuItem(0);
                mi->Tag		= i;
                mi->Caption = token_list[i].name;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        case PROP_TOKEN2:{
            pmEnum->Items->Clear();
			TokenValue2* T	= dynamic_cast<TokenValue2*>(prop->GetFrontValue()); R_ASSERT(T);
            AStringVec& lst = T->items;
			TMenuItem* mi 	= new TMenuItem(0);
			mi->Caption 	= "-";
			pmEnum->Items->Add(mi);
			for(AStringIt it=lst.begin(); it!=lst.end(); it++){
                mi 			= new TMenuItem(0);
                mi->Tag		= it-lst.begin();
                mi->Caption = *it;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
		case PROP_TOKEN3:{
            pmEnum->Items->Clear();
			TokenValue3* T	= dynamic_cast<TokenValue3*>(prop->GetFrontValue()); R_ASSERT(T);
			TMenuItem* mi 	= new TMenuItem(0);
			mi->Caption 	= "-";
			pmEnum->Items->Add(mi);
            for (DWORD i=0; i<T->cnt; i++){
                mi 			= new TMenuItem(0);
                mi->Tag		= i;
                mi->Caption = T->items[i].str;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
		case PROP_LIST:{
            pmEnum->Items->Clear();
			ListValue* T				= dynamic_cast<ListValue*>(prop->GetFrontValue()); R_ASSERT(T);
            AStringVec& lst = T->items;
            TMenuItem* mi	= new TMenuItem(0);
			mi->Caption 	= "-";
			pmEnum->Items->Add(mi);
			for(AStringIt it=lst.begin(); it!=lst.end(); it++){
                mi = new TMenuItem(0);
                mi->Tag		= it-lst.begin();
                mi->Caption = *it;
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        case PROP_VECTOR: 			VectorClick(item); 			break;
        case PROP_WAVE: 			WaveFormClick(item); 		break;
        case PROP_FCOLOR: 			
        case PROP_COLOR: 			ColorClick(item); 			break;
		case PROP_GAMEMTL:
        case PROP_LIBPS:
        case PROP_LIBSOUND:
        case PROP_LIGHTANIM:
        case PROP_LIBOBJECT:
        case PROP_GAMEOBJECT:
        case PROP_ENTITY:
        case PROP_TEXTURE:
        case PROP_ESHADER:
        case PROP_CSHADER:		CustomTextClick(item);      break;
        case PROP_A_LIBOBJECT:
        case PROP_A_GAMEMTL:
        case PROP_A_LIBPS:
		case PROP_A_LIBSOUND:
        case PROP_A_TEXTURE:
        case PROP_A_ESHADER:
        case PROP_A_CSHADER:	CustomAnsiTextClick(item);	break;
        case PROP_U8:
        case PROP_U16:
        case PROP_U32:
        case PROP_S8:
        case PROP_S16:
        case PROP_S32:
        case PROP_FLOAT:
        	PrepareLWNumber(item);
        break;
        case PROP_A_TEXT:
        case PROP_TEXT:
        	PrepareLWText(item);
        break;
		case PROP_TEXTURE2:{
            pmEnum->Items->Clear();
            TMenuItem* mi	= new TMenuItem(0);
			mi->Caption 	= "-";
			pmEnum->Items->Add(mi);
            for (DWORD i=0; i<TSTRING_COUNT; i++){
                mi = new TMenuItem(0);
                mi->Tag		= i;
                mi->Caption = TEXTUREString[i];
                mi->OnClick = PMItemClick;
                pmEnum->Items->Add(mi);
            }
        }break;
        };
        switch(prop->type){
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
        prop->OnClick();
    };
}
//---------------------------------------------------------------------------
void __fastcall TProperties::PMItemClick(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        TElTreeItem* item = (TElTreeItem*)pmEnum->Tag;
		PropItem* prop = (PropItem*)item->Tag;
        switch(prop->Type()){
		case PROP_TOKEN:{
			TokenValue* T			= dynamic_cast<TokenValue*>(prop->GetFrontValue()); R_ASSERT(T);
            xr_token* token_list   	= T->token;
            DWORD new_val			= token_list[mi->Tag].id;
			prop->OnAfterEdit(&new_val);
            if (prop->ApplyValue(&new_val)){	
				prop->OnChange		();
            	Modified			();
            }
			item->ColumnText->Strings[0]= prop->GetText();
        }break;
		case PROP_TOKEN2:{
            DWORD new_val			= mi->Tag;
			prop->OnAfterEdit		(&new_val);
            if (prop->ApplyValue(&new_val)){	
				prop->OnChange		();
            	Modified			();
            }
			item->ColumnText->Strings[0]= prop->GetText();
        }break;
		case PROP_TOKEN3:{
			TokenValue3* T			= dynamic_cast<TokenValue3*>(prop->GetFrontValue()); R_ASSERT(T);
            DWORD new_val			= T->items[mi->Tag].ID;
			prop->OnAfterEdit		(&new_val);
            if (prop->ApplyValue(&new_val)){	
				prop->OnChange		();
            	Modified			();
            }
			item->ColumnText->Strings[0]= prop->GetText();
        }break;
		case PROP_LIST:{
			ListValue* T			= dynamic_cast<ListValue*>(prop->GetFrontValue()); R_ASSERT(T);
            AStringVec& lst		 	= T->items;
            string256 new_val;	strcpy(new_val,lst[mi->Tag].c_str());
			prop->OnAfterEdit		(&new_val);
            if (prop->ApplyValue(new_val)){	
				prop->OnChange		();
	            Modified			();
            }
			item->ColumnText->Strings[0]= prop->GetText();
        }break;
		case PROP_TEXTURE2:{
			TextValue* T			= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(T);
			AnsiString edit_val	 	= T->GetValue();
			prop->OnBeforeEdit		(&edit_val);
            LPCSTR new_val 		 	= 0;
            bool bRes				= false;
        	if (mi->Tag==0){
            	bRes				= TfrmChoseItem::SelectItem(TfrmChoseItem::smTexture,new_val,prop->subitem,edit_val.c_str(),true);
            }else if (mi->Tag>=2){
            	new_val			 	= TEXTUREString[mi->Tag];
            }
            if (bRes){
                edit_val		 	= new_val;
                prop->OnAfterEdit	(&edit_val);
                if (prop->ApplyValue(edit_val.c_str())){
					prop->OnChange	();
	                Modified		();
                }
                item->ColumnText->Strings[0]= prop->GetText();
            }
        }break;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::WaveFormClick(TElTreeItem* item)
{
	PropItem* prop 			= (PropItem*)item->Tag;
    R_ASSERT(PROP_WAVE==prop->type);

	WaveValue* V			= dynamic_cast<WaveValue*>(prop->GetFrontValue()); R_ASSERT(V);
    WaveForm edit_val		= V->GetValue();
	prop->OnBeforeEdit		(&edit_val);

	if (TfrmShaderFunction::Run(&edit_val)==mrOk){
        prop->OnAfterEdit	(&edit_val);
        if (prop->ApplyValue(&edit_val)){
			prop->OnChange	();
        	Modified		();
        }
        item->ColumnText->Strings[0]= prop->GetText();
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::ColorClick(TElTreeItem* item)
{
	PropItem* prop = (PropItem*)item->Tag;
    switch (prop->type){
    case PROP_FCOLOR:{
		ColorValue* V		= dynamic_cast<ColorValue*>(prop->GetFrontValue()); R_ASSERT(V);
        Fcolor edit_val		= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);

        DWORD ev 			= edit_val.get();
        if (SelectColor(&ev)){
	        edit_val.set	(ev);
            prop->OnAfterEdit(&edit_val);
            if (prop->ApplyValue(&edit_val)){
				prop->OnChange	();
                item->RedrawItem(true);
                Modified		();
            }
        }
    }break;
    case PROP_COLOR:{
		U32Value* V			= dynamic_cast<U32Value*>(prop->GetFrontValue()); R_ASSERT(V);
        DWORD edit_val		= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);

        if (SelectColor(&edit_val)){
            prop->OnAfterEdit(&edit_val);
            if (prop->ApplyValue(&edit_val)){	
				prop->OnChange	();
                item->RedrawItem(true);
                Modified		();
            }
        }
    }break;
    default: THROW2("Unsupported type");
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::VectorClick(TElTreeItem* item)
{
    PropItem* prop 	= (PropItem*)item->Tag;
	VectorValue* V	= dynamic_cast<VectorValue*>(prop->GetFrontValue()); R_ASSERT(V);
    VERIFY(prop->type==PROP_VECTOR);
    Fvector edit_val= V->GetValue();
	prop->OnBeforeEdit(&edit_val);
	if (NumericVectorRun(AnsiString(item->Text).c_str(),&edit_val,V->dec,&edit_val,&V->lim_mn,&V->lim_mx)){
        prop->OnAfterEdit(&edit_val);
        if (prop->ApplyValue(&edit_val)){
			prop->OnChange	();
			item->RedrawItem(true);
            Modified		();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::CustomTextClick(TElTreeItem* item)
{
	PropItem* prop			= (PropItem*)item->Tag;
	TextValue* V			= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
	AnsiString edit_val		= V->GetValue();
	prop->OnBeforeEdit		(&edit_val);
    LPCSTR new_val			= 0;
    TfrmChoseItem::ESelectMode mode;
    bool bIgnoreExt 		= false;
    switch (prop->type){
    case PROP_ESHADER:		mode = TfrmChoseItem::smShader;							break;
	case PROP_CSHADER:		mode = TfrmChoseItem::smShaderXRLC;						break;
    case PROP_TEXTURE:		mode = TfrmChoseItem::smTexture;	bIgnoreExt = true; 	break;
	case PROP_LIGHTANIM:	mode = TfrmChoseItem::smLAnim;		                    break;
    case PROP_LIBOBJECT:	mode = TfrmChoseItem::smObject;		                    break;
    case PROP_GAMEOBJECT:	mode = TfrmChoseItem::smGameObject;	                    break;
    case PROP_ENTITY:		mode = TfrmChoseItem::smEntity;		                    break;
    case PROP_LIBSOUND:		mode = TfrmChoseItem::smSound;		bIgnoreExt = true; 	break;
    case PROP_LIBPS:		mode = TfrmChoseItem::smPS;			                    break;
    case PROP_GAMEMTL:		mode = TfrmChoseItem::smGameMaterial; 					break;
    default: THROW2("Unknown prop");
    }
    if (TfrmChoseItem::SelectItem(mode,new_val,prop->subitem,edit_val.c_str(),bIgnoreExt)){
        edit_val			= new_val;
        prop->OnAfterEdit	(&edit_val);
        if (prop->ApplyValue(edit_val.c_str())){	
			prop->OnChange	();
        	Modified		();
        }
        item->ColumnText->Strings[0]= prop->GetText();
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::CustomAnsiTextClick(TElTreeItem* item)
{
	PropItem* prop			= (PropItem*)item->Tag;
	ATextValue* V			= dynamic_cast<ATextValue*>(prop->GetFrontValue()); R_ASSERT(V);
	AnsiString edit_val		= V->GetValue();
	prop->OnBeforeEdit		(&edit_val);
    LPCSTR new_val			= 0;
    TfrmChoseItem::ESelectMode mode;
    bool bIgnoreExt 		= false;
    switch (prop->type){
    case PROP_A_ESHADER:   	mode = TfrmChoseItem::smShader;		                    break;
	case PROP_A_CSHADER:   	mode = TfrmChoseItem::smShaderXRLC;	                    break;
    case PROP_A_TEXTURE:   	mode = TfrmChoseItem::smTexture;	bIgnoreExt = true;  break;
    case PROP_A_LIBOBJECT:	mode = TfrmChoseItem::smObject;		                    break;
    case PROP_A_LIBSOUND:  	mode = TfrmChoseItem::smSound;		bIgnoreExt = true;  break;
    case PROP_A_LIBPS:		mode = TfrmChoseItem::smPS;			                    break;
    case PROP_A_GAMEMTL:	mode = TfrmChoseItem::smGameMaterial; 					break;
    default: THROW2("Unknown prop");
    }
    if (TfrmChoseItem::SelectItem(mode,new_val,prop->subitem,edit_val.c_str(),bIgnoreExt)){
        edit_val				= new_val;
        prop->OnAfterEdit	(&edit_val);
        if (prop->ApplyValue(&edit_val)){ 
			prop->OnChange	();
        	Modified();
        }
        item->ColumnText->Strings[0]= prop->GetText();
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// LW style inplace editor
//---------------------------------------------------------------------------
void TProperties::CancelLWNumber()
{
    HideLWNumber();
}

void TProperties::HideLWNumber()
{
    seNumber->Tag	= 0;
    seNumber->Hide	();
}
//---------------------------------------------------------------------------
void TProperties::PrepareLWNumber(TElTreeItem* item)
{
	PropItem* prop = (PropItem*)item->Tag;
    switch (prop->type){
    case PROP_U8:{
		U8Value* V			= dynamic_cast<U8Value*>(prop->GetFrontValue()); R_ASSERT(V);
        u8 edit_val        	= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_U16:{
		U16Value* V			= dynamic_cast<U16Value*>(prop->GetFrontValue()); R_ASSERT(V);
        u16 edit_val        = V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_U32:{
		U32Value* V			= dynamic_cast<U32Value*>(prop->GetFrontValue()); R_ASSERT(V);
        u32 edit_val        = V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_S8:{
		S8Value* V			= dynamic_cast<S8Value*>(prop->GetFrontValue()); R_ASSERT(V);
        s8 edit_val        	= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_S16:{
		S16Value* V			= dynamic_cast<S16Value*>(prop->GetFrontValue()); R_ASSERT(V);
        s16 edit_val        = V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_S32:{
		S32Value* V			= dynamic_cast<S32Value*>(prop->GetFrontValue()); R_ASSERT(V);
        s32 edit_val        = V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_FLOAT:{
		FloatValue* V		= dynamic_cast<FloatValue*>(prop->GetFrontValue()); R_ASSERT(V);
        float edit_val		= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
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
void TProperties::ShowLWNumber(TRect& R)
{
    seNumber->Left 	= R.Left;
    seNumber->Top  	= R.Top+tvProperties->HeaderHeight;
    seNumber->Width	= R.Right-R.Left+2;
    seNumber->Height= R.Bottom-R.Top+2;
    seNumber->Show();
    seNumber->SetFocus();
}

void TProperties::ApplyLWNumber()
{
	TElTreeItem* item 			= (TElTreeItem*)seNumber->Tag;
    if (item){
		PropItem* prop 			= (PropItem*)item->Tag;
        seNumber->Update();
	    switch (prop->type){
    	case PROP_U8:{
			U8Value* V			= dynamic_cast<U8Value*>(prop->GetFrontValue()); R_ASSERT(V);
            u8 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){ 
				prop->OnChange	();
            	Modified		();
            }
        }break;
    	case PROP_U16:{
			U16Value* V			= dynamic_cast<U16Value*>(prop->GetFrontValue()); R_ASSERT(V);
            u16 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){ 
				prop->OnChange	();
            	Modified		();
            }
        }break;
    	case PROP_U32:{
			U32Value* V			= dynamic_cast<U32Value*>(prop->GetFrontValue()); R_ASSERT(V);
            u32 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){ 
				prop->OnChange	();
            	Modified		();
            }
        }break;
    	case PROP_S8:{
			S8Value* V			= dynamic_cast<S8Value*>(prop->GetFrontValue()); R_ASSERT(V);
            s8 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){ 
				prop->OnChange	();
            	Modified		();
            }
        }break;
    	case PROP_S16:{
			S16Value* V			= dynamic_cast<S16Value*>(prop->GetFrontValue()); R_ASSERT(V);
            s16 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){ 
				prop->OnChange	();
            	Modified		();
            }
        }break;
    	case PROP_S32:{
			S32Value* V			= dynamic_cast<S32Value*>(prop->GetFrontValue()); R_ASSERT(V);
            s32 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){ 
				prop->OnChange	();
            	Modified		();
            }
        }break;
	    case PROP_FLOAT:{
			FloatValue* V		= dynamic_cast<FloatValue*>(prop->GetFrontValue()); R_ASSERT(V);
            float new_val		= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){ 
				prop->OnChange	();
            	Modified		();
            }
        }break;
        default: THROW2("Wrong switch.");
    	}
		item->ColumnText->Strings[0] = prop->GetText();
    }
}

void __fastcall TProperties::seNumberExit(TObject *Sender)
{
	ApplyLWNumber();
	HideLWNumber();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::seNumberKeyDown(TObject *Sender, WORD &Key,
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
void TProperties::CancelLWText()
{
    HideLWText();
}

void TProperties::HideLWText()
{
    edText->Tag		= 0;
    edText->Hide	();
}
//---------------------------------------------------------------------------
void TProperties::PrepareLWText(TElTreeItem* item)
{
	PropItem* prop 		= (PropItem*)item->Tag;
    switch (prop->type){
    case PROP_A_TEXT:{
		ATextValue* V	= dynamic_cast<ATextValue*>(prop->GetFrontValue()); R_ASSERT(V);
        AnsiString edit_val	= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
	    edText->Text 		= edit_val;
    }break;
    case PROP_TEXT:{
		TextValue* V		= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
		AnsiString edit_val	= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
	    edText->Text 		= edit_val;
    }break;
    }
    edText->Tag 	= (int)item;
    tvProperties->Refresh();
}
void TProperties::ShowLWText(TRect& R)
{
    edText->Left 	= R.Left-1;
    edText->Top  	= R.Top+tvProperties->HeaderHeight;
    edText->Width	= R.Right-R.Left+0;
    edText->Height	= R.Bottom-R.Top+2;
    edText->Show	();
    edText->SetFocus();
}

void TProperties::ApplyLWText()
{
	TElTreeItem* item 			= (TElTreeItem*)edText->Tag;
    if (item){
		PropItem* prop 			= (PropItem*)item->Tag;
        edText->Update();
	    switch (prop->type){
        case PROP_A_TEXT:{
			ATextValue* V		= dynamic_cast<ATextValue*>(prop->GetFrontValue()); R_ASSERT(V);
			AnsiString new_val	= edText->Text;
            edText->MaxLength	= 0;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){ 
				prop->OnChange	();
            	Modified();
            }
            item->ColumnText->Strings[0] = prop->GetText();
        }break;
    	case PROP_TEXT:{
			TextValue* V		= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
            edText->MaxLength	= V->lim;
			AnsiString new_val	= edText->Text;
			prop->OnAfterEdit	(&new_val);
            if (V->ApplyValue(new_val.c_str())){ 
				prop->OnChange	();
	            Modified		();
            }
            item->ColumnText->Strings[0] = prop->GetText();
        }break;
    	}
    }
}

void __fastcall TProperties::edTextExit(TObject *Sender)
{
	ApplyLWText();
	HideLWText();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::edTextKeyDown(TObject *Sender, WORD &Key,
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

void __fastcall TProperties::edTextDblClick(TObject *Sender)
{
	TElTreeItem* item = (TElTreeItem*)edText->Tag;
    if (item){
		PropItem* prop 		= (PropItem*)item->Tag;
        edText->Update();
	    switch (prop->type){
    	case PROP_TEXT:{
			TextValue* V	= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
			AnsiString new_val	= edText->Text;
			if (TfrmText::Run(new_val,AnsiString(item->Text).c_str(),false,V->lim)) edText->Text = new_val;
        }break;
    	case PROP_A_TEXT:{
			AnsiString new_val	= edText->Text;
			if (TfrmText::Run(new_val,AnsiString(item->Text).c_str())) edText->Text = new_val;
        }break;
    	}
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::tvPropertiesItemFocused(TObject *Sender)
{
	if (OnItemFocused) OnItemFocused(tvProperties->Selected);
}
//---------------------------------------------------------------------------

void __fastcall TProperties::tvPropertiesHeaderColumnResize(
      TObject *Sender, int SectionIndex)
{
	ApplyEditControl();
}
//---------------------------------------------------------------------------

void TProperties::ApplyEditControl()
{
	ApplyLWText		();
	HideLWText		();
	ApplyLWNumber	();
	HideLWNumber	();
}
//---------------------------------------------------------------------------

void TProperties::CancelEditControl()
{
	CancelLWNumber();
	CancelLWText();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::FormDeactivate(TObject *Sender)
{
	ApplyEditControl();
}
//---------------------------------------------------------------------------



void __fastcall TProperties::FormShow(TObject *Sender)
{
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void __fastcall TProperties::tvPropertiesItemChange(TObject *Sender,
      TElTreeItem *Item, TItemChangeMode ItemChangeMode)
{
	if (Item&&(icmCheckState==ItemChangeMode)){
		PropItem* prop 			= (PropItem*)Item->Tag;
	    if (prop){
        	prop->m_Flags.set	(PropItem::flCBChecked,Item->Checked);
        	prop->OnChange		();
			Modified			();
    	}
	    tvProperties->Refresh	();
    }
}
//---------------------------------------------------------------------------


