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
#include "ShaderFunction.h"
#include "ColorPicker.h"
#include "ChoseForm.h"
#include "FolderLib.h"
#include "NumericVector.h"
#include "TextForm.h"
#include "ui_main.h"
#include "ImageThumbnail.h"

#ifdef _LEVEL_EDITOR
	#include "Scene.h"
#endif
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
#pragma link "ElBtnCtl"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

#define TSTRING_COUNT 	4
const LPSTR TEXTUREString[TSTRING_COUNT]={"Custom...","-","$null","$base0"};
//---------------------------------------------------------------------------
TElTreeItem* __fastcall TProperties::BeginEditMode(LPCSTR section)
{
	LockUpdating		();
	CancelEditControl	();
    if (section) return FHelper.FindFolder(tvProperties,section);
    return 0;
}
//---------------------------------------------------------------------------

void __fastcall TProperties::EndEditMode(TElTreeItem* expand_node)
{
	if (expand_node) expand_node->Expand(true);
	UnlockUpdating		();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::BeginFillMode(const AnsiString& title, LPCSTR section)
{
	LockUpdating		();
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
    }

    Caption = title;
}
//---------------------------------------------------------------------------
void __fastcall TProperties::EndFillMode(bool bFullExpand)
{
    bModified=false;
    // end fill mode
	if (bFullExpand) tvProperties->FullExpand();
	UnlockUpdating		();
    FHelper.RestoreSelection	(tvProperties,last_selected_item,false);
};
//---------------------------------------------------------------------------
void TProperties::ClearParams(TElTreeItem* node)
{
	if (node){
    	THROW2("ClearParams - node");
    	//S когда будут все итемы удалить у каждого
/*
//s
    	for (TElTreeItem* item=node; item; item=item->GetNext()){
			PropValue* V = (PropValue*)GetItemData(item);
            if (V){
	            PropValuePairIt it=std::find(m_Values.begin(),m_Values.end(),V); VERIFY(it!=m_Values.end());
    	        if (it){
					m_Values.erase(it);
					xr_delete(V);
                }
            }
		}
*/
    }else{
	    if (tvProperties->Selected) FHelper.MakeFullName(tvProperties->Selected,0,last_selected_item);
	    for (PropItemIt it=m_Items.begin(); it!=m_Items.end(); it++)
    		xr_delete	(*it);
		m_Items.clear();
		LockUpdating				();
	    tvProperties->Items->Clear	();
		UnlockUpdating				();
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
	HideExtBtn			();
	CancelEditControl	();
    ClearParams			();
}
//---------------------------------------------------------------------------
void __fastcall TProperties::SelectItem(const AnsiString& full_name)
{
	m_FirstClickItem	= FHelper.RestoreSelection	(tvProperties,FHelper.ExpandItem(tvProperties,full_name),false);
}
//---------------------------------------------------------------------------

__fastcall TProperties::TProperties(TComponent* Owner) : TForm(Owner)
{
	m_FirstClickItem= 0;
	bModified 		= false;
    DEFINE_INI		(fsStorage);
	m_BMCheck 		= xr_new<Graphics::TBitmap>();
    m_BMDot 		= xr_new<Graphics::TBitmap>();
    m_BMEllipsis 	= xr_new<Graphics::TBitmap>();
	m_BMCheck->LoadFromResourceName((u32)HInstance,"CHECK");
	m_BMDot->LoadFromResourceName((u32)HInstance,"DOT");
	m_BMEllipsis->LoadFromResourceName((u32)HInstance,"ELLIPSIS");
    seNumber->Parent= tvProperties;
    seNumber->Hide	();
}
//---------------------------------------------------------------------------

TProperties* TProperties::CreateForm(TWinControl* parent, TAlign align, TOnModifiedEvent modif, TOnItemFocused focused, TOnCloseEvent on_close)
{
	TProperties* props 			= xr_new<TProperties>(parent);
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

TProperties* TProperties::CreateModalForm(const AnsiString& title, PropItemVec& items, bool bFullExpand, TOnModifiedEvent modif, TOnItemFocused focused, TOnCloseEvent on_close)
{
	TProperties* props 			= xr_new<TProperties>((TComponent*)0);
    props->OnModifiedEvent 		= modif;
    props->OnItemFocused    	= focused;
    props->OnCloseEvent			= on_close;
    props->AssignItems			(items,bFullExpand,title);
	props->ShowPropertiesModal	();
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
    xr_delete(props);
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
	HideExtBtn			();
	CancelEditControl	();
    if (OnCloseEvent) 	OnCloseEvent();
	ClearParams			();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::FormDestroy(TObject *Sender)
{
    xr_delete		(m_BMCheck);
    xr_delete		(m_BMDot);
    xr_delete		(m_BMEllipsis);
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
	LockUpdating		();
   	HideExtBtn			();
	CancelEditControl	();
    // clear values
//    if (tvProperties->Selected) FHelper.MakeFullName(tvProperties->Selected,0,last_selected_item);
    ClearParams();
    tvProperties->Items->Clear();
    // fill values
	string128 fld;
    m_Items					= items;
	for (PropItemIt it=m_Items.begin(); it!=m_Items.end(); it++){
    	PropItem* prop		= *it;
        prop->item			= FHelper.AppendObject(tvProperties,prop->key); R_ASSERT3(prop->item,"Duplicate properties key found:",prop->key.c_str());
        prop->item->Tag	    = (int)prop;
        prop->item->UseStyles=true;
        prop->item->CheckBoxEnabled = prop->m_Flags.is(PropItem::flShowCB);
        prop->item->ShowCheckBox 	= prop->m_Flags.is(PropItem::flShowCB);
        prop->item->CheckBoxState 	= (TCheckBoxState)prop->m_Flags.is(PropItem::flCBChecked);
        // if thumbnail draw
        if (prop->m_Flags.is(PropItem::flDrawThumbnail)){
        	prop->item->Height 		= 64;
        	prop->item->OwnerHeight = !miDrawThumbnails->Checked;
        }                             
        // if canvas value
        if (PROP_CANVAS==prop->type){
        	prop->item->Height 		= ((CanvasValue*)prop->GetFrontValue())->height;
        	prop->item->OwnerHeight = false;
        }
        // set style
        TElCellStyle* CS    = prop->item->AddStyle();
        CS->OwnerProps 		= true;
        CS->CellType 		= sftUndef;
        CS->Style 			= ElhsOwnerDraw;
        prop->item->ColumnText->Add(prop->GetText());
    }

    // end fill mode
    bModified=false;
	if (full_expand) tvProperties->FullExpand();

    SelectItem		(last_selected_item);

	UnlockUpdating	();

    Caption 		= title;
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

void TProperties::OutBOOL(BOOL val, TCanvas* Surface, const TRect& R, bool bEnable)
{
	if (bEnable){
	    Surface->CopyMode 	= cmSrcAnd;//cmSrcErase;
    	if (val)			Surface->Draw(R.Left,R.Top+3,m_BMCheck);
	    else 				Surface->Draw(R.Left,R.Top+3,m_BMDot);
    }
}

void TProperties::OutText(LPCSTR text, TCanvas* Surface, TRect R, bool bEnable, TGraphic* g, bool bArrow)
{
	if (bEnable&&(g||bArrow)){
	    R.Right	-=	g->Width+2;
    	R.Left 	+= 	1;
    }else{
        R.Right-= 1;
        R.Left += 1;
    }
    DrawText	(Surface->Handle, text, -1, &R, DT_LEFT | DT_SINGLELINE);
	if (bEnable){
        if (g){
            R.Left 	= 	R.Right;
            Surface->CopyMode = cmSrcAnd;
            Surface->Draw(R.Left+1,R.Top+5,g);
        }else if (bArrow){
            R.Left 	= 	R.Right;
            R.Right += 	10;
            DrawArrow	(Surface, eadDown, R, clWindowText, true);
        }
    }
}

void DrawButton(TRect R, TCanvas* Surface, LPCSTR caption, bool bDown, bool bSelected)
{
    TColor a 				= bDown?TColor(0x00707070):TColor(0x00A0A0A0);
    TColor b 				= bDown?TColor(0x00C0C0C0):TColor(0x00707070);
    R.Bottom				+= 	1;
    Surface->Pen->Color 	= a;                     
    Surface->MoveTo			(R.left,R.bottom);
    Surface->LineTo			(R.left,R.top);
    Surface->LineTo			(R.right,R.top);
    Surface->Pen->Color 	= b;
    Surface->LineTo			(R.right,R.bottom);
    Surface->LineTo			(R.left-1,R.bottom);
    R.Left 					+= 	2;
    R.Right					-=	1;
    R.Bottom				-= 	1;
    R.Top					+=	1;
	Surface->Brush->Style 	= bsSolid;
	Surface->Brush->Color 	= bSelected?TColor(0x00858585):TColor(0x00808080);
	Surface->FillRect		(R);
    Surface->Font->Color 	= clBlack;
    DrawText				(Surface->Handle, caption, -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}
void DrawButtons(TRect R, TCanvas* Surface, AStringVec& lst, int down_btn, bool bSelected)
{
	if (!lst.empty()){
        TRect r				= R;
        float dx 			= float(R.Width())/float(lst.size());
        for (AStringIt it=lst.begin(); it!=lst.end(); it++){
        	int k			= it-lst.begin();
    		r.left			= R.left+iFloor(k*dx);
    		r.right			= r.left+dx-1;
            DrawButton		(r,Surface,it->c_str(),(down_btn==k),bSelected);
        }
    }
}

void __fastcall TProperties::tvPropertiesItemDraw(TObject *Sender,
      TElTreeItem *Item, TCanvas *Surface, TRect &R, int SectionIndex)
{
	TRect  R1;
	Surface->Brush->Style = bsClear;
  	if (SectionIndex == 1){
    	PropItem* prop 				= (PropItem*)Item->Tag;
        u32 type 					= prop->type;
        if (prop->Enabled()){
            Surface->Font->Color 	= clBlack;
            Surface->Font->Style 	= TFontStyles();           
        }else{
            Surface->Font->Color 	= clSilver;
            Surface->Font->Style 	= TFontStyles()<< fsBold;
        }
        // check mixed
        prop->CheckMixed();
        // out caption mixed 
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
            TRect src_rect 	= R;
            prop->draw_rect	= R;
            switch(type){
            case PROP_BUTTON:{
                ButtonValue* V			= dynamic_cast<ButtonValue*>(prop->GetFrontValue()); R_ASSERT(V);
                DrawButtons				(R,Surface,V->value,V->btn_num,Item->Selected);
            }break;
            case PROP_CAPTION:
                Surface->Font->Color 	= clSilver;
                R.Right-= 1;
                R.Left += 1;
                DrawText	(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            break;
            case PROP_CANVAS:{
                Surface->Font->Color 	= clSilver;
                R.Right-= 1;
                R.Left += 1;
                CanvasValue* val = dynamic_cast<CanvasValue*>(prop->GetFrontValue()); R_ASSERT(val);
                if (val->OnDrawCanvasEvent)
	                val->OnDrawCanvasEvent(val,Surface,R);
                DrawText	(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            }break;
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
                u32 C 	= (U32Value::TYPE)V->GetValue();
                Surface->Brush->Color = (TColor)rgb2bgr(C);
                Surface->FillRect(R);
            }break;
            case PROP_FLAG8:{
                Flag8Value* V		= dynamic_cast<Flag8Value*>(prop->GetFrontValue()); R_ASSERT(V);
                OutBOOL				(V->GetValueEx(),Surface,R,prop->Enabled());
            }break;
            case PROP_FLAG16:{
                Flag16Value* V		= dynamic_cast<Flag16Value*>(prop->GetFrontValue()); R_ASSERT(V);
                OutBOOL				(V->GetValueEx(),Surface,R,prop->Enabled());
            }break;
            case PROP_FLAG32:{
                Flag32Value* V		= dynamic_cast<Flag32Value*>(prop->GetFrontValue()); R_ASSERT(V);
                OutBOOL				(V->GetValueEx(),Surface,R,prop->Enabled());
            }break;
            case PROP_BOOLEAN:{
                BOOLValue* V		= dynamic_cast<BOOLValue*>(prop->GetFrontValue()); R_ASSERT(V);
                OutBOOL				(V->GetValue(),Surface,R,prop->Enabled());
            }break;
            case PROP_TEXTURE:
            case PROP_TEXTURE2:
            case PROP_A_TEXTURE:
                OutText(prop->GetText(),Surface,R,prop->Enabled(),m_BMEllipsis);
                if (miDrawThumbnails->Checked){ 
                    R.top+=tvProperties->ItemIndent;
                    FHelper.DrawThumbnail	(Surface,R,prop->GetText(),EImageThumbnail::EITTexture);
                }
            break;
            case PROP_WAVE:
            case PROP_LIBPS:
            case PROP_LIBPE:
            case PROP_LIBPARTICLES:
            case PROP_SOUNDSRC:
            case PROP_SOUNDENV:
            case PROP_LIGHTANIM:
            case PROP_LIBOBJECT:
            case PROP_GAMEOBJECT:
            case PROP_ENTITY:
            case PROP_SCENE_ITEM:
            case PROP_GAMEMTL:
            case PROP_A_LIBOBJECT:
            case PROP_A_GAMEMTL:
            case PROP_A_LIBPS:
            case PROP_A_LIBPE:
            case PROP_A_LIBPARTICLES:
            case PROP_A_SOUNDSRC:
            case PROP_A_SOUNDENV:
            case PROP_A_ESHADER:
            case PROP_A_CSHADER:
            case PROP_ESHADER:
            case PROP_CSHADER:
                OutText(prop->GetText(),Surface,R,prop->Enabled(),m_BMEllipsis);
            break;
            case PROP_TOKEN:
            case PROP_TOKEN2:
            case PROP_TOKEN3:
            case PROP_TOKEN4:
            case PROP_LIST:
                OutText(prop->GetText(),Surface,R,prop->Enabled(),m_BMEllipsis);
            break;
            case PROP_A_TEXT:
            case PROP_TEXT:
                if (edText->Tag!=(int)Item)
                    OutText(prop->GetText(),Surface,R,prop->Enabled(),m_BMEllipsis);
            break;
            case PROP_VECTOR:
                OutText(prop->GetText(),Surface,R,prop->Enabled());
            break;
            case PROP_U8:
            case PROP_U16:
            case PROP_U32:
            case PROP_S8:
            case PROP_S16:
            case PROP_S32:
            case PROP_FLOAT:
                if (seNumber->Tag!=(int)Item)
                    OutText(prop->GetText(),Surface,R,prop->Enabled());
            break;
            default:
                THROW2("Unknown prop type");
            };
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
            // show ext button (if needed)
            if ((pbExtBtn->Tag==(int)prop)&&(!pbExtBtn->Visible)) ShowExtBtn(R);
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
  	if (item){
    	if ((HC==1)&&(Button==mbLeft)){
//        	Log("Shift",(int)Shift.Contains(ssDouble));
            PropItem* prop = (PropItem*)item->Tag;
            // Проверить чтобы не нажимать 2 раза для кнопок
            if (prop&&(PROP_BUTTON==prop->type)) m_FirstClickItem=item;
            if (m_FirstClickItem==item){
				if (!prop||(prop&&!prop->Enabled())) return;
                pmEnum->Tag = (int)item;
                switch(prop->type){
                case PROP_CAPTION: break;
                case PROP_CANVAS: break;
                case PROP_BUTTON:{
                    ButtonValue* FV				= dynamic_cast<ButtonValue*>(prop->GetFrontValue()); R_ASSERT(FV);
                    int btn_num					= iFloor((X-prop->draw_rect.left)*(float(FV->value.size())/float(prop->draw_rect.Width())));
                    for (PropItem::PropValueIt it=prop->values.begin(); it!=prop->values.end(); it++){
                        ButtonValue* V			= dynamic_cast<ButtonValue*>(*it); R_ASSERT(V);
                        V->btn_num				= btn_num;
                    }
                    item->RedrawItem			(true);
                }break;
                case PROP_FLAG8:{
                    Flag8Value* V				= dynamic_cast<Flag8Value*>(prop->GetFrontValue()); R_ASSERT(V);
                    Flags8 new_val 				= V->GetValue(); new_val.invert(V->mask);
                    prop->OnAfterEdit			(&new_val);
                    if (prop->ApplyValue(&new_val)){
                        Modified				();
                        RefreshForm				();
                    }
                }break;
                case PROP_FLAG16:{
                    Flag16Value* V				= dynamic_cast<Flag16Value*>(prop->GetFrontValue()); R_ASSERT(V);
                    Flags16 new_val 			= V->GetValue(); new_val.invert(V->mask);
                    prop->OnAfterEdit			(&new_val);
                    if (prop->ApplyValue(&new_val)){
                        Modified				();
                        RefreshForm				();
                    }
                }break;
                case PROP_FLAG32:{
                    Flag32Value* V				= dynamic_cast<Flag32Value*>(prop->GetFrontValue()); R_ASSERT(V);
                    Flags32 new_val 			= V->GetValue(); new_val.invert(V->mask);
                    prop->OnAfterEdit			(&new_val);
                    if (prop->ApplyValue(&new_val)){
                        Modified				();
                        RefreshForm				();
                    }
                }break;
                case PROP_BOOLEAN:{
                    BOOLValue* V				= dynamic_cast<BOOLValue*>(prop->GetFrontValue()); R_ASSERT(V);
                    BOOL new_val 				= !V->GetValue();
                    prop->OnAfterEdit			(&new_val);
                    if (prop->ApplyValue(&new_val)){
                        Modified				();
                        RefreshForm				();
                    }
                }break;
                case PROP_TOKEN:{
                    pmEnum->Items->Clear();
                    TokenValue* T				= dynamic_cast<TokenValue*>(prop->GetFrontValue()); R_ASSERT(T);
                    xr_token* token_list 		= T->token;
                    TMenuItem* mi 				= xr_new<TMenuItem>((TComponent*)0);
                    mi->Caption 				= "-";
                    pmEnum->Items->Add			(mi);
                    for(int i=0; token_list[i].name; i++){
                        mi 			= xr_new<TMenuItem>((TComponent*)0);
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
                    TMenuItem* mi 	= xr_new<TMenuItem>((TComponent*)0);
                    mi->Caption 	= "-";
                    pmEnum->Items->Add(mi);
                    for(AStringIt it=lst.begin(); it!=lst.end(); it++){
                        mi 			= xr_new<TMenuItem>((TComponent*)0);
                        mi->Tag		= it-lst.begin();
                        mi->Caption = *it;
                        mi->OnClick = PMItemClick;
                        pmEnum->Items->Add(mi);
                    }
                }break;
                case PROP_TOKEN3:{
                    pmEnum->Items->Clear();
                    TokenValue3* T	= dynamic_cast<TokenValue3*>(prop->GetFrontValue()); R_ASSERT(T);
                    TMenuItem* mi 	= xr_new<TMenuItem>((TComponent*)0);
                    mi->Caption 	= "-";
                    pmEnum->Items->Add(mi);
                    for (u32 i=0; i<T->cnt; i++){
                        mi 			= xr_new<TMenuItem>((TComponent*)0);
                        mi->Tag		= i;
                        mi->Caption = T->items[i].str;
                        mi->OnClick = PMItemClick;
                        pmEnum->Items->Add(mi);
                    }
                }break;
                case PROP_TOKEN4:{
                    pmEnum->Items->Clear();
                    TokenValue4* T	= dynamic_cast<TokenValue4*>(prop->GetFrontValue()); R_ASSERT(T);
                    TMenuItem* mi 	= xr_new<TMenuItem>((TComponent*)0);
                    mi->Caption 	= "-";
                    pmEnum->Items->Add(mi);
                    for (TokenValue4::ItemVec::const_iterator it=T->items->begin(); it!=T->items->end(); it++){
                        mi 			= xr_new<TMenuItem>((TComponent*)0);
                        mi->Tag		= it-T->items->begin();
                        mi->Caption = it->str;
                        mi->OnClick = PMItemClick;
                        pmEnum->Items->Add(mi);
                    }
                }break;
                case PROP_LIST:{
                    pmEnum->Items->Clear();
                    ListValue* T				= dynamic_cast<ListValue*>(prop->GetFrontValue()); R_ASSERT(T);
                    AStringVec& lst = T->items;
                    TMenuItem* mi	= xr_new<TMenuItem>((TComponent*)0);
                    mi->Caption 	= "-";
                    pmEnum->Items->Add(mi);
                    for(AStringIt it=lst.begin(); it!=lst.end(); it++){
                        mi 			= xr_new<TMenuItem>((TComponent*)0);
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
	            case PROP_LIBPE:
                case PROP_LIBPARTICLES:
                case PROP_SOUNDSRC:
                case PROP_SOUNDENV:
                case PROP_LIGHTANIM:
                case PROP_LIBOBJECT:
                case PROP_GAMEOBJECT:
                case PROP_ENTITY:
                case PROP_SCENE_ITEM:
                case PROP_TEXTURE:
                case PROP_ESHADER:
                case PROP_CSHADER:		CustomTextClick(item);      break;
                case PROP_A_LIBOBJECT:
                case PROP_A_GAMEMTL:
                case PROP_A_LIBPS:
	            case PROP_A_LIBPE:
                case PROP_A_LIBPARTICLES:
                case PROP_A_SOUNDSRC:
                case PROP_A_SOUNDENV:
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
                	if ((X-prop->draw_rect.left)<(prop->draw_rect.Width()-(m_BMEllipsis->Width+2)))	PrepareLWText(item);
                    else								                   							ExecTextEditor(prop);
                break;
                case PROP_TEXTURE2:{
                    pmEnum->Items->Clear();
                    TMenuItem* mi	= xr_new<TMenuItem>((TComponent*)0);
                    mi->Caption 	= "-";
                    pmEnum->Items->Add(mi);
                    for (u32 i=0; i<TSTRING_COUNT; i++){
                        mi = xr_new<TMenuItem>((TComponent*)0);
                        mi->Tag		= i;
                        mi->Caption = TEXTUREString[i];
                        mi->OnClick = PMItemClick;
                        pmEnum->Items->Add(mi);
                    }
                }break;
                default:
                    THROW2("Unknown prop type");
                };
                switch(prop->type){
                case PROP_TOKEN:
                case PROP_TOKEN2:
                case PROP_TOKEN3:
                case PROP_TOKEN4:
                case PROP_LIST:
                case PROP_TEXTURE2:
                    TPoint P; P.x = X; P.y = Y;
                    P=tvProperties->ClientToScreen(P);
                    pmEnum->Popup(P.x,P.y-10);
                    break;
                };
            }
	        if (prop&&prop->OnClickEvent) prop->OnClickEvent(prop);
        }else if (Button==mbRight){
            TPoint P; P.x = X; P.y = Y;
            P=tvProperties->ClientToScreen(P);
            pmItems->Popup(P.x,P.y-10);
        }
    };
    m_FirstClickItem = item;
}
//---------------------------------------------------------------------------

void __fastcall TProperties::tvPropertiesMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if (tvProperties->Selected){
		TElTreeItem* item 	= tvProperties->Selected;
		PropItem* prop 		= (PropItem*)item->Tag;
		if (!prop||(prop&&!prop->Enabled())) return;
        switch(prop->type){
        case PROP_BUTTON:{
        	if (Shift.Contains(ssLeft)){
                ButtonValue* V				= dynamic_cast<ButtonValue*>(prop->GetFrontValue()); R_ASSERT(V);
                Y							-= tvProperties->HeaderHeight;
                int btn_num					= -1;
                if (((Y>prop->draw_rect.top)&&(Y<prop->draw_rect.bottom))&&((X>prop->draw_rect.left)&&(Y<prop->draw_rect.right)))
                	btn_num					= iFloor((X-prop->draw_rect.left)*(float(V->value.size())/float(prop->draw_rect.Width())));
                for (PropItem::PropValueIt it=prop->values.begin(); it!=prop->values.end(); it++){
                    ButtonValue* V			= dynamic_cast<ButtonValue*>(*it); R_ASSERT(V);
                    V->btn_num				= btn_num;
                }
                item->RedrawItem			(true);
            }
        }break;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::tvPropertiesMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (tvProperties->Selected){
		TElTreeItem* item 	= tvProperties->Selected;
		PropItem* prop 		= (PropItem*)item->Tag;
		if (!prop||(prop&&!prop->Enabled())) return;
        switch(prop->type){
        case PROP_BUTTON:{
        	if (Button==mbLeft){
				bool bRes = false;
                for (PropItem::PropValueIt it=prop->Values().begin(); it!=prop->Values().end(); it++){
                    ButtonValue* V			= dynamic_cast<ButtonValue*>(*it); R_ASSERT(V);
                    bRes 					|= V->OnBtnClick();
                    V->btn_num				= -1;
                    if (V->m_Flags.is(ButtonValue::flFirstOnly)) break;
                }
                if (bRes){
                    Modified			();
                    RefreshForm			();
                }
                item->RedrawItem	(true);
            }
        }break;
        }
    }
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
            u32 new_val				= token_list[mi->Tag].id;
			prop->OnAfterEdit(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified			();
            }
			item->ColumnText->Strings[0]= prop->GetText();
        }break;
		case PROP_TOKEN2:{
            u32 new_val				= mi->Tag;
			prop->OnAfterEdit		(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified			();
            }
			item->ColumnText->Strings[0]= prop->GetText();
        }break;
		case PROP_TOKEN3:{
			TokenValue3* T			= dynamic_cast<TokenValue3*>(prop->GetFrontValue()); R_ASSERT(T);
            u32 new_val				= T->items[mi->Tag].ID;
			prop->OnAfterEdit		(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified			();
            }
			item->ColumnText->Strings[0]= prop->GetText();
        }break;
		case PROP_TOKEN4:{
			TokenValue4* T			= dynamic_cast<TokenValue4*>(prop->GetFrontValue()); R_ASSERT(T);
            u32 new_val				= (*T->items)[mi->Tag].ID;
			prop->OnAfterEdit		(&new_val);
            if (prop->ApplyValue(&new_val)){
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

        u32 ev 				= edit_val.get();
        if (SelectColor(&ev)){
	        edit_val.set	(ev);
            prop->OnAfterEdit(&edit_val);
            if (prop->ApplyValue(&edit_val)){
                item->RedrawItem(true);
                Modified		();
            }
        }
    }break;
    case PROP_COLOR:{
		U32Value* V			= dynamic_cast<U32Value*>(prop->GetFrontValue()); R_ASSERT(V);
        u32 edit_val		= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);

        if (SelectColor(&edit_val)){
            prop->OnAfterEdit(&edit_val);
            if (prop->ApplyValue(&edit_val)){
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
    AStringVec items;
    bool bIgnoreExt 		= false;
    switch (prop->type){
    case PROP_ESHADER:		mode = TfrmChoseItem::smShader;							break;
	case PROP_CSHADER:		mode = TfrmChoseItem::smShaderXRLC;						break;
    case PROP_TEXTURE:		mode = TfrmChoseItem::smTexture;	bIgnoreExt = true; 	break;
	case PROP_LIGHTANIM:	mode = TfrmChoseItem::smLAnim;		                    break;
    case PROP_LIBOBJECT:	mode = TfrmChoseItem::smObject;		                    break;
    case PROP_GAMEOBJECT:	mode = TfrmChoseItem::smGameObject;	                    break;
    case PROP_ENTITY:		mode = TfrmChoseItem::smEntity;		                    break;
    case PROP_SOUNDSRC:		mode = TfrmChoseItem::smSoundSource;bIgnoreExt = true; 	break;
    case PROP_SOUNDENV:		mode = TfrmChoseItem::smSoundEnv;						break;
    case PROP_LIBPS:		mode = TfrmChoseItem::smPS;			                    break;
    case PROP_LIBPE:		mode = TfrmChoseItem::smPE;			                    break;
    case PROP_LIBPARTICLES:	mode = TfrmChoseItem::smParticles;	                    break;
    case PROP_GAMEMTL:		mode = TfrmChoseItem::smGameMaterial; 					break;
    case PROP_SCENE_ITEM:{ 
#ifdef _LEVEL_EDITOR
		SceneItemValue* SI	= dynamic_cast<SceneItemValue*>(V); R_ASSERT(SI);
        ObjectList& lst 	= Scene.ListObj(SI->clsID);
        for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	if ((*_F)->OnChooseQuery(SI->specific.c_str())) items.push_back((*_F)->Name);
#endif
     	mode = TfrmChoseItem::smCustom;
    }break;
    default: THROW2("Unknown prop");
    }
    if (TfrmChoseItem::SelectItem(mode,new_val,prop->subitem,edit_val.c_str(),bIgnoreExt,&items)){
        edit_val			= new_val;
        prop->OnAfterEdit	(&edit_val);
        if (prop->ApplyValue(edit_val.c_str())){
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
    case PROP_A_SOUNDSRC:  	mode = TfrmChoseItem::smSoundSource;bIgnoreExt = true;  break;
    case PROP_A_SOUNDENV:	mode = TfrmChoseItem::smSoundEnv;						break;
    case PROP_A_LIBPS:		mode = TfrmChoseItem::smPS;			                    break;
	case PROP_A_LIBPE:		mode= TfrmChoseItem::smPE;			                    break;
    case PROP_A_LIBPARTICLES:mode= TfrmChoseItem::smParticles;	                    break;
    case PROP_A_GAMEMTL:	mode = TfrmChoseItem::smGameMaterial; 					break;
    default: THROW2("Unknown prop type");
    }
    if (TfrmChoseItem::SelectItem(mode,new_val,prop->subitem,edit_val.c_str(),bIgnoreExt)){
        edit_val				= new_val;
        prop->OnAfterEdit	(&edit_val);
        if (prop->ApplyValue(&edit_val)){
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
    if (seNumber->Visible&&Visible) 	tvProperties->SetFocus();
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
	seNumber->ButtonWidth = seNumber->Height;
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
            	Modified		();
            }
        }break;
    	case PROP_U16:{
			U16Value* V			= dynamic_cast<U16Value*>(prop->GetFrontValue()); R_ASSERT(V);
            u16 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified		();
            }
        }break;
    	case PROP_U32:{
			U32Value* V			= dynamic_cast<U32Value*>(prop->GetFrontValue()); R_ASSERT(V);
            u32 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified		();
            }
        }break;
    	case PROP_S8:{
			S8Value* V			= dynamic_cast<S8Value*>(prop->GetFrontValue()); R_ASSERT(V);
            s8 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified		();
            }
        }break;
    	case PROP_S16:{
			S16Value* V			= dynamic_cast<S16Value*>(prop->GetFrontValue()); R_ASSERT(V);
            s16 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified		();
            }
        }break;
    	case PROP_S32:{
			S32Value* V			= dynamic_cast<S32Value*>(prop->GetFrontValue()); R_ASSERT(V);
            s32 new_val			= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified		();
            }
        }break;
	    case PROP_FLOAT:{
			FloatValue* V		= dynamic_cast<FloatValue*>(prop->GetFrontValue()); R_ASSERT(V);
            float new_val		= seNumber->Value;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){
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

void TProperties::ShowExtBtn(TRect& R)
{
    pbExtBtn->Height= R.Bottom-R.Top+2;
    pbExtBtn->Width	= pbExtBtn->Height;
    pbExtBtn->Left 	= R.Right-pbExtBtn->Width+1;
    pbExtBtn->Top  	= R.Top+tvProperties->HeaderHeight+1;
    R.Right			-= pbExtBtn->Width+1;
    pbExtBtn->Show	();
}

void TProperties::HideExtBtn()
{
    pbExtBtn->Tag	= 0;
	pbExtBtn->Hide	();
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
    if (edText->Visible&&Visible) 	tvProperties->SetFocus();
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
		edText->MaxLength	= 0;
    }break;
    case PROP_TEXT:{
		TextValue* V		= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
		AnsiString edit_val	= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
		edText->Text 		= edit_val;
		edText->MaxLength	= V->lim;
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
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified();
            }
            item->ColumnText->Strings[0] = prop->GetText();
        }break;
    	case PROP_TEXT:{
			TextValue* V		= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
			AnsiString new_val	= edText->Text;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(new_val.c_str())){
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
	TElTreeItem* item 		= (TElTreeItem*)edText->Tag;
    if (item){
		PropItem* prop 		= (PropItem*)item->Tag;
        edText->Update();
	    switch (prop->type){
    	case PROP_TEXT:{
			TextValue* V	= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
			AnsiString new_val	= edText->Text;
			if (TfrmText::RunEditor(new_val,AnsiString(item->Text).c_str(),false,V->lim))
             	edText->Text = new_val;
            ApplyLWText();
            HideLWText();
        }break;
    	case PROP_A_TEXT:{
			AnsiString new_val	= edText->Text;
			if (TfrmText::RunEditor(new_val,AnsiString(item->Text).c_str()))
            	edText->Text = new_val;
            ApplyLWText();
            HideLWText();
        }break;
    	}
    }
}
//---------------------------------------------------------------------------

void TProperties::ExecTextEditor(PropItem* prop)
{
    if (prop){
	    switch (prop->type){
    	case PROP_TEXT:{
            TextValue* V	= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
            AnsiString edit_val	= V->GetValue();
            prop->OnBeforeEdit	(&edit_val);
            if (TfrmText::RunEditor(edit_val,AnsiString(prop->Item()->Text).c_str(),false,V->lim)){
                prop->OnAfterEdit	(&edit_val);
                if (prop->ApplyValue(edit_val.c_str()))
                    Modified		();
                prop->Item()->ColumnText->Strings[0] = prop->GetText();
            }
        }break;
    	case PROP_A_TEXT:{
            ATextValue* V	= dynamic_cast<ATextValue*>(prop->GetFrontValue()); R_ASSERT(V);
            AnsiString edit_val	= V->GetValue();
            prop->OnBeforeEdit	(&edit_val);
            if (TfrmText::RunEditor(edit_val,AnsiString(prop->Item()->Text).c_str(),false)){
                prop->OnAfterEdit	(&edit_val);
                if (prop->ApplyValue(&edit_val))
                    Modified();
                prop->Item()->ColumnText->Strings[0] = prop->GetText();
            }
        }break;
    	}
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::tvPropertiesItemFocused(TObject *Sender)
{
	if (OnItemFocused) 	OnItemFocused(tvProperties->Selected);
    HideExtBtn			();
	if (tvProperties->Selected){
        PropItem* prop 		= (PropItem*)tvProperties->Selected->Tag;
        if (prop&&prop->m_Flags.is(PropItem::flShowExtBtn))
            pbExtBtn->Tag	= (int)prop;
        if (prop&&prop->OnItemFocused)prop->OnItemFocused(prop);
    }
	m_FirstClickItem 	= 0;
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
	CancelLWNumber	();
	CancelLWText	();
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

void __fastcall TProperties::pbExtBtnClick(TObject *Sender)
{
	TElPopupButton* btn = dynamic_cast<TElPopupButton*>(Sender);
	PropItem* prop = (PropItem*)btn->Tag; R_ASSERT(prop);//&&prop->OnExtBtnClick);
//.	prop->OnExtClick();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::fsStorageRestorePlacement(TObject *Sender)
{
    RestoreParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TProperties::fsStorageSavePlacement(TObject *Sender)
{
	SaveParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TProperties::ExpandSelected1Click(TObject *Sender)
{
	if (tvProperties->Selected) tvProperties->Selected->Expand(false);
}
//---------------------------------------------------------------------------

void __fastcall TProperties::CollapseSelected1Click(TObject *Sender)
{
	if (tvProperties->Selected) tvProperties->Selected->Collapse(false);
}
//---------------------------------------------------------------------------

void __fastcall TProperties::ExpandAll1Click(TObject *Sender)
{
	tvProperties->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::CollapseAll1Click(TObject *Sender)
{
	tvProperties->FullCollapse();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::miDrawThumbnailsClick(TObject *Sender)
{
	RefreshForm();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::RefreshForm()
{
	LockUpdating		();
    for (PropItemIt it=m_Items.begin(); it!=m_Items.end(); it++){
    	PropItem* prop = *it;
    	if (prop->m_Flags.is(PropItem::flDrawThumbnail)) 
        	prop->item->OwnerHeight = !miDrawThumbnails->Checked;
    }
	UnlockUpdating		();
	tvProperties->Repaint();
}




