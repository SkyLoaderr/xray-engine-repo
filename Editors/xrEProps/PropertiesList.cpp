//------------------------------------------------------------------------------
// - Events:
//	- OnBeforeEdit
// 	- OnDrawValue
//------------------------------------------------------------------------------

/* TODO 1 -oAlexMX -cTODO: ref_str вместо AnsiString в качестве edit_val*/
/* TODO 1 -oAlexMX -cTODO: Select for plItemFolders mode*/

#include "stdafx.h"
#pragma hdrstop

#include "PropertiesList.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

#include "ShaderFunction.h"
#include "ColorPicker.h"
#include "ChoseForm.h"
#include "FolderLib.h"
#include "NumericVector.h"
#include "TextForm.h"
#include "ui_main.h"
#include "EThumbnail.h"
#include "ItemList.h"

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
#pragma link "ExtBtn"
#pragma resource "*.dfm"

#define TSTRING_COUNT 	4
const LPSTR TEXTUREString[TSTRING_COUNT]={"Custom...","-","$null","$base0"};
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
        // store
        FolderStorage.clear	();
        FolderStore			();
        // clear
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
	m_BMCheck->LoadFromResourceName		((u32)HInstance,"CHECK");
	m_BMDot->LoadFromResourceName		((u32)HInstance,"DOT");
	m_BMEllipsis->LoadFromResourceName	((u32)HInstance,"ELLIPSIS");
    seNumber->Parent= tvProperties;
    seNumber->Hide	();
    edText->Parent	= tvProperties;
    edText->Hide	();
    m_Flags.zero	();
    m_Folders		= 0;
}
//---------------------------------------------------------------------------

TProperties* TProperties::CreateForm(const AnsiString& title, TWinControl* parent, TAlign align, TOnModifiedEvent modif, TOnItemFocused focused, TOnCloseEvent on_close, u32 flags)
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
	props->Caption				= title;	
    props->fsStorage->IniSection= title;
    props->m_Flags.set			(flags);
    if (props->m_Flags.is_any(plItemFolders)){
    	if (props->m_Flags.is(plIFTop)){
            props->paFolders->Align	= alTop;
            props->spFolders->Align	= alTop;
            props->spFolders->Top	= props->paFolders->Top+props->paFolders->Height;
        }else{
            props->paFolders->Align	= alLeft;
            props->spFolders->Align	= alLeft;
            props->spFolders->Left	= props->paFolders->Left+props->paFolders->Width;
        }
    	props->spFolders->Show	();
    	props->paFolders->Show	();
        props->paFolders->Refresh();
    	props->m_Folders		= TItemList::CreateForm("Folders",props->paFolders,alClient,TItemList::ilSuppressIcon|TItemList::ilFolderStore|TItemList::ilSuppressStatus);
        props->m_Folders->OnItemFocused = props->OnFolderFocused;
    }else{
    	props->spFolders->Hide	();
    	props->paFolders->Hide	();
    }
	return props;
}

TProperties* TProperties::CreateModalForm(const AnsiString& title, bool bShowButtonsBar, TOnModifiedEvent modif, TOnItemFocused focused, TOnCloseEvent on_close, u32 flags)
{
	TProperties* props 			= xr_new<TProperties>((TComponent*)0);
    props->OnModifiedEvent 		= modif;
    props->OnItemFocused    	= focused;
    props->OnCloseEvent			= on_close;
    props->paButtons->Visible	= bShowButtonsBar;
	props->Caption				= title;	
    props->fsStorage->IniSection= title;
    props->m_Flags.set			(flags);
    if (props->m_Flags.is(plItemFolders)){
    	if (props->m_Flags.is(plIFTop)){
            props->paFolders->Align	= alTop;
            props->spFolders->Align	= alTop;
            props->spFolders->Top	= props->paFolders->Top+props->paFolders->Height;
        }else{
            props->paFolders->Align	= alLeft;
            props->spFolders->Align	= alLeft;
            props->spFolders->Left	= props->paFolders->Left+props->paFolders->Width;
        }
    	props->spFolders->Show	();
    	props->paFolders->Show	();
        props->paFolders->Refresh();
    	props->m_Folders		= TItemList::CreateForm("Folders",props->paFolders,alClient,TItemList::ilSuppressIcon|TItemList::ilFolderStore|TItemList::ilSuppressStatus);
        props->m_Folders->OnItemFocused = props->OnFolderFocused;
    }else{
    	props->spFolders->Hide	();
    	props->paFolders->Hide	();
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
    xr_delete(props);
}
void __fastcall TProperties::ShowProperties()
{
	Show					();
}

int __fastcall TProperties::ShowPropertiesModal()
{
	return ShowModal		();
}

void __fastcall TProperties::HideProperties()
{
	Hide();
}

int __fastcall TProperties::EditPropertiesModal(PropItemVec& values, LPCSTR title, bool bShowButtonsBar, TOnModifiedEvent modif, TOnItemFocused focused, TOnCloseEvent close, u32 flags)
{
	TProperties* P 	= CreateModalForm(title,bShowButtonsBar,modif,focused,close,flags);
    P->AssignItems	(values);
    int res 		= P->ShowPropertiesModal();
    DestroyForm		(P);
    return res;
}

void __fastcall TProperties::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	HideExtBtn			();
	ApplyEditControl	();
    if (Visible&&OnCloseEvent) 	OnCloseEvent();
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

void TProperties::FillElItems(PropItemVec& items, LPCSTR startup_pref)
{
	m_ViewItems		= items;
    tvProperties->Items->Clear();
	for (PropItemIt it=m_ViewItems.begin(); it!=m_ViewItems.end(); it++){
    	PropItem* prop		= *it;
        AnsiString 	key 	= prop->key;
	    if (m_Flags.is(plItemFolders)){
        	if (startup_pref&&startup_pref[0]){
                AnsiString k	= key;		
                LPCSTR k0		= k.c_str();
                LPCSTR k1		= startup_pref;
                while (k0[0]&&k1[0]&&(k0[0]==k1[0]))	{k0++;k1++; if(k1[0]=='\\')key=k0+1;}
                if ((k0[0]!='\\')&&(k1[0]!=0))	continue;
            }else{
            	if (1!=_GetItemCount(key.c_str(),'\\')) continue;
            }
        }
        prop->item			= FHelper.AppendObject(tvProperties,key,false,false); R_ASSERT3(prop->item,"Duplicate properties key found:",key.c_str());
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
        // main text set style
        prop->item->MainStyle->Style= ElhsOwnerDraw;
        
        // set style
        TElCellStyle* CS    = prop->item->AddStyle();
        CS->OwnerProps 		= true;
        CS->CellType 		= sftUndef;
        CS->Style 			= ElhsOwnerDraw;
        prop->item->ColumnText->Add(prop->GetText());
    }
    if (m_Flags.is(plFullExpand)) tvProperties->FullExpand();
    if (m_Flags.is(plFullSort)){
        tvProperties->ShowColumns	= false;
    	tvProperties->Sort			(true);
        tvProperties->SortMode 		= smAdd;
        tvProperties->ShowColumns	= true;
    }

    FolderRestore		();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::AssignItems(PropItemVec& items)
{
	// begin fill mode
	LockUpdating		();
   	HideExtBtn			();
	CancelEditControl	();

    // clear values
//    if (tvProperties->Selected) FHelper.MakeFullName(tvProperties->Selected,0,last_selected_item);
    ClearParams();

    // copy values
    m_Items				= items;

    // folder
    ListItemsVec		folder_items;

    if (m_Flags.is(plItemFolders)){
        for (PropItemIt it=m_Items.begin(); it!=m_Items.end(); it++){
            PropItem* prop 	= *it;
            int cnt 		= _GetItemCount(prop->key.c_str(),'\\');
            if (cnt>1){	
                AnsiString 	folder;
                _ReplaceItem(prop->key.c_str(),cnt-1,"",folder,'\\');
                if (0==LHelper.FindItem(folder_items,folder.c_str())){
                	PropItem* P		= PHelper.FindItem(m_Items,prop->key.c_str());
                    ListItem* I		= LHelper.CreateItem(folder_items,folder.c_str(),0);
                    if (P) I->prop_color = P->prop_color;
                }
            }
        }
    }

    // create EL items
    if (m_Flags.is(plItemFolders))	m_Folders->AssignItems	(folder_items,m_Flags.is(plFullExpand),m_Flags.is(plFullSort));
    else							FillElItems				(m_Items);

    // end fill mode
    bModified			= false;

	UnlockUpdating	();

    SelectItem		(last_selected_item);
}
//---------------------------------------------------------------------------

void TProperties::FolderStore()
{
    if (m_Flags.is(plFolderStore)&&tvProperties->Items->Count){
        for (TElTreeItem* item=tvProperties->Items->GetFirstNode(); item; item=item->GetNext()){
            if (item->ChildrenCount)
            {
                AnsiString nm;
                FHelper.MakeFullName(item,0,nm);
                SFolderStore 		st_item;
                st_item.expand		= item->Expanded;
                FolderStorage[nm]		= st_item;
            }
        }
    }
}
void TProperties::FolderRestore()
{
    if (m_Flags.is(plFolderStore)&&!FolderStorage.empty()){
        for (TElTreeItem* item=tvProperties->Items->GetFirstNode(); item; item=item->GetNext()){
            if (item->ChildrenCount){   
                AnsiString nm;
                FHelper.MakeFullName		(item,0,nm);
                FolderStorePairIt it 		= FolderStorage.find(nm);
                if (it!=FolderStorage.end()){
                    SFolderStore& st_item 	= it->second;
                    if (st_item.expand) 	item->Expand	(false);
                    else					item->Collapse	(false);
                }
            }
        }
    }
}
void __fastcall TProperties::OnFolderFocused(TElTreeItem* item)
{
	AnsiString s, lfsi;
    if (tvProperties->Selected) FHelper.MakeFullName(tvProperties->Selected,0,lfsi);
	FHelper.MakeFullName(item,0,s);
    LockUpdating	();
    FolderStore		();
    FillElItems		(m_Items, s.c_str());
    UnlockUpdating	();
    if (lfsi.Length()){
		AnsiString 	lfsi_new,new_part;
        int cnt		= _GetItemCount(s.c_str(),'\\');
        if (cnt)	_GetItem(s.c_str(),cnt-1,new_part,'\\');
        _ReplaceItem(lfsi.c_str(),0,new_part.c_str(),lfsi_new,'\\');
	    SelectItem	(lfsi_new);
	}
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

void TProperties::OutBOOL(BOOL val, TCanvas* Surface, TRect& R, bool bEnable)
{
	if (bEnable){
        Surface->CopyMode 	= cmSrcAnd;//cmSrcErase;
        if (val)			Surface->Draw(R.Left,R.Top+3,m_BMCheck);
        else 				Surface->Draw(R.Left,R.Top+3,m_BMDot);
    }else{
	    DrawText			(Surface->Handle, val?"on":"off", -1, &R, DT_LEFT | DT_SINGLELINE);
    }
}

void TProperties::OutText(LPCSTR text, TCanvas* Surface, TRect& R, bool bEnable, TGraphic* g, bool bArrow)
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
//.	Surface->Font->Color 	= clBlack;
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
    PropItem* prop 					= (PropItem*)Item->Tag;
    if (!prop)						return;

	TRect  R1;
	Surface->Brush->Style 			= bsClear;
	if (SectionIndex == 0){
        Surface->Font->Color 		= prop->prop_color;
        DrawText					(Surface->Handle, AnsiString(Item->Text).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}else if (SectionIndex == 1){
        u32 type 					= prop->type;
        if (prop->Enabled()){
            Surface->Font->Color 	= prop->val_color;
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
            case PROP_VCOLOR:{
                Surface->Brush->Style = bsSolid;
                Surface->Brush->Color = TColor(0x00000000);
                Surface->FrameRect(R);
                R.Right	-=	1;
                R.Left 	+= 	1;
                R.Top	+=	1;
                R.Bottom-= 	1;
                VectorValue* V			= dynamic_cast<VectorValue*>(prop->GetFrontValue()); R_ASSERT(V);
                Fcolor C; C.set			(V->value->z,V->value->y,V->value->x,0.f);
                Surface->Brush->Color 	= (TColor)C.get();
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
            case PROP_FLAG:{
                FlagValueCustom*  V	= dynamic_cast<FlagValueCustom*>(prop->GetFrontValue()); R_ASSERT(V);
                if (V->HaveCaption())	OutText(prop->GetText(),Surface,R,prop->Enabled(),m_BMEllipsis);
                else	        		OutBOOL	(V->GetValueEx(),Surface,R,prop->Enabled());
            }break;
            case PROP_BOOLEAN:{
                BOOLValue* V		= dynamic_cast<BOOLValue*>(prop->GetFrontValue()); R_ASSERT(V);
                OutBOOL				(V->GetValue(),Surface,R,prop->Enabled());
            }break;
            case PROP_CHOOSE:{
                OutText(prop->GetText(),Surface,R,prop->Enabled(),m_BMEllipsis);
                ChooseValueCustom* V= dynamic_cast<ChooseValueCustom*>(prop->GetFrontValue());
                switch(V->choose_mode){
                case smTexture:
                    if (miDrawThumbnails->Checked){ 
                        R.top	+=	tvProperties->LineHeight-4;
                        R.left 	= 	R.Right-(R.bottom-R.top);
                        FHelper.DrawThumbnail	(Surface,R,prop->GetText(),EImageThumbnail::ETTexture);
                    }
                break;
                }
            }break;
            case PROP_TEXTURE2:
                OutText(prop->GetText(),Surface,R,prop->Enabled(),m_BMEllipsis);
                if (miDrawThumbnails->Checked){ 
                    R.top+=tvProperties->LineHeight-4;
                    R.left 	= 	R.Right-(R.bottom-R.top);
                    FHelper.DrawThumbnail	(Surface,R,prop->GetText(),EImageThumbnail::ETTexture);
                }
            break;
            case PROP_WAVE:
                OutText(prop->GetText(),Surface,R,prop->Enabled(),m_BMEllipsis);
            break;
            case PROP_TOKEN:
            case PROP_A_TOKEN:
            case PROP_TOKEN2:
            case PROP_TOKEN3:
            case PROP_SH_TOKEN:
            case PROP_LIST:
                OutText(prop->GetText(),Surface,R,prop->Enabled(),m_BMEllipsis);
            break;
            case PROP_TIME:{
                FloatValue* V = dynamic_cast<FloatValue*>(prop->GetFrontValue()); R_ASSERT(V);
                OutText(FloatTimeToStrTime(V->GetValue()).c_str(),Surface,R,prop->Enabled());
            }break;
            case PROP_R_TEXT:
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
            case PROP_TIME:
            case PROP_R_TEXT:
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

template <class T>
BOOL FlagOnEdit				(PropItem* prop, BOOL& bRes)
{                                                     
    FlagValue<_flags<T> >* V= dynamic_cast<FlagValue<_flags<T> >*>(prop->GetFrontValue());
    if (!V)					return FALSE;
    _flags<T> new_val 		= V->GetValue(); 
    prop->OnBeforeEdit		(&new_val);
    new_val.invert			(V->mask);
    prop->OnAfterEdit	 	(&new_val);
    bRes = prop->ApplyValue	(&new_val);
    return TRUE;
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
                case PROP_FLAG:{
                    BOOL bRes 					= FALSE;
                    if (!FlagOnEdit<u8>(prop,bRes))
                        if (!FlagOnEdit<u16>(prop,bRes))
                            if (!FlagOnEdit<u32>(prop,bRes))
                                Debug.fatal		("Unknown flag type");
                    if (bRes){
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
                    TokenValueCustom* T			= dynamic_cast<TokenValueCustom*>(prop->GetFrontValue()); R_ASSERT(T);
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
                case PROP_A_TOKEN:{
                    pmEnum->Items->Clear();
                    ATokenValueCustom* T		= dynamic_cast<ATokenValueCustom*>(prop->GetFrontValue()); R_ASSERT(T);
                    ATokenVec* token_list 		= T->token;
                    TMenuItem* mi 				= xr_new<TMenuItem>((TComponent*)0);
                    mi->Caption 				= "-";
                    pmEnum->Items->Add			(mi);
                    for(ATokenIt it=token_list->begin(); it!=token_list->end(); it++){
                        mi 			= xr_new<TMenuItem>((TComponent*)0);
                        mi->Tag		= it-token_list->begin();
                        mi->Caption = it->name;
                        mi->OnClick = PMItemClick;
                        pmEnum->Items->Add(mi);
                    }
                }break;
                case PROP_TOKEN2:{
                    pmEnum->Items->Clear();
                    TokenValue2Custom* T	= dynamic_cast<TokenValue2Custom*>(prop->GetFrontValue()); R_ASSERT(T);
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
                    TokenValue3Custom* T	= dynamic_cast<TokenValue3Custom*>(prop->GetFrontValue()); R_ASSERT(T);
                    TMenuItem* mi 	= xr_new<TMenuItem>((TComponent*)0);
                    mi->Caption 	= "-";
                    pmEnum->Items->Add(mi);
                    for (TokenValue3Custom::ItemVec::const_iterator it=T->items->begin(); it!=T->items->end(); it++){
                        mi 			= xr_new<TMenuItem>((TComponent*)0);
                        mi->Tag		= it-T->items->begin();
                        mi->Caption = it->str;
                        mi->OnClick = PMItemClick;
                        pmEnum->Items->Add(mi);
                    }
                }break;
                case PROP_SH_TOKEN:{
                    pmEnum->Items->Clear();
                    TokenValueSH* T	= dynamic_cast<TokenValueSH*>(prop->GetFrontValue()); R_ASSERT(T);
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
                case PROP_VECTOR: 			VectorClick		(item); 	break;
                case PROP_WAVE: 			WaveFormClick	(item); 	break;
                case PROP_VCOLOR:
                case PROP_FCOLOR:
                case PROP_COLOR: 			ColorClick		(item); 	break;
                case PROP_CHOOSE:			ChooseClick		(item); 	break;
                case PROP_U8:
                case PROP_U16:
                case PROP_U32:
                case PROP_S8:
                case PROP_S16:
                case PROP_S32:
                case PROP_FLOAT:
                    PrepareLWNumber(item);
                break;
                case PROP_R_TEXT:
                case PROP_A_TEXT:
                case PROP_TEXT:
                	if ((X-prop->draw_rect.left)<(prop->draw_rect.Width()-(m_BMEllipsis->Width+2)))	PrepareLWText(item);
                    else								                   							ExecTextEditor(prop);
                break;
                case PROP_TIME:
                	PrepareLWText(item);
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
                case PROP_A_TOKEN:
                case PROP_TOKEN2:
                case PROP_TOKEN3:
                case PROP_SH_TOKEN:
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
				bool bRes 	= false;
                bool bSafe	= false;
                for (PropItem::PropValueIt it=prop->Values().begin(); it!=prop->Values().end(); it++){
                    ButtonValue* V			= dynamic_cast<ButtonValue*>(*it); R_ASSERT(V);
                    if (V->btn_num>-1){
	                    bRes 				|= V->OnBtnClick(bSafe);
    	                V->btn_num			= -1;
        	            if (V->m_Flags.is(ButtonValue::flFirstOnly)) break;
                    }
                }
                if (bRes){
                    Modified			();
                    if (!bSafe)			RefreshForm		();
                }
                if (!bSafe)				item->RedrawItem(true);
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
			TokenValueCustom* T		= dynamic_cast<TokenValueCustom*>(prop->GetFrontValue()); R_ASSERT(T);
            xr_token* token_list   	= T->token;
            u32 new_val				= token_list[mi->Tag].id;
			prop->OnAfterEdit(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified			();
            }
			item->ColumnText->Strings[0]= prop->GetText();
        }break;
		case PROP_A_TOKEN:{
			ATokenValueCustom* T	= dynamic_cast<ATokenValueCustom*>(prop->GetFrontValue()); R_ASSERT(T);
            ATokenVec* token_list   = T->token;
            u32 new_val				= (*token_list)[mi->Tag].id;
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
			TokenValue3Custom* T	= dynamic_cast<TokenValue3Custom*>(prop->GetFrontValue()); R_ASSERT(T);
            u32 new_val				= (*T->items)[mi->Tag].ID;
			prop->OnAfterEdit		(&new_val);
            if (prop->ApplyValue(&new_val)){
            	Modified			();
            }
			item->ColumnText->Strings[0]= prop->GetText();
        }break;
		case PROP_SH_TOKEN:{
			TokenValueSH* T			= dynamic_cast<TokenValueSH*>(prop->GetFrontValue()); R_ASSERT(T);
            u32 new_val				= T->items[mi->Tag].ID;
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
            bool bRes				= true;
        	if (mi->Tag==0){
            	bRes				= TfrmChoseItem::SelectItem(smTexture,new_val,prop->subitem,edit_val.c_str());//,true);
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
    case PROP_VCOLOR:{
		VectorValue* V		= dynamic_cast<VectorValue*>(prop->GetFrontValue()); R_ASSERT(V);
        Fvector edit_val	= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
		Fcolor C; 			C.set(edit_val.x,edit_val.y,edit_val.z,1.f);
        u32 ev 				= C.get();
        if (SelectColor(&ev)){
        	C.set			(ev);
	        edit_val.set	(C.r,C.g,C.b);
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

void __fastcall TProperties::ChooseClick(TElTreeItem* item)
{
	PropItem* prop			= (PropItem*)item->Tag;
	PropValue* V			= prop->GetFrontValue(); R_ASSERT(V);
    AnsiString edit_val;
	ChooseValueCustom* CV	= 0;
	TextValue* TV			= dynamic_cast<TextValue*>(prop->GetFrontValue());	
    if (TV){ 
    	edit_val			= TV->GetValue();
        CV					= dynamic_cast<ChooseValueCustom*>(TV); R_ASSERT(CV);
    }else{
        ATextValue* AV		= dynamic_cast<ATextValue*>(prop->GetFrontValue());
        if (AV){ 
        	edit_val		= AV->GetValue();
			CV				= dynamic_cast<ChooseValueCustom*>(AV); R_ASSERT(CV);
        }else{
			RTextValue* RV	= dynamic_cast<RTextValue*>(prop->GetFrontValue());
            if (RV){
            	edit_val	= *RV->GetValue();
				CV			= dynamic_cast<ChooseValueCustom*>(RV); R_ASSERT(CV);
            }else THROW		("Unknown choose value type");
        }
    }
	if (!edit_val.Length()) edit_val = CV->start_path;
	prop->OnBeforeEdit		(&edit_val);
    LPCSTR new_val			= 0;
    ChooseItemVec 			items;
    if (CV->choose_mode==smCustom)
    	if (CV->OnChooseEvent) CV->OnChooseEvent(V,items);
    if (TfrmChoseItem::SelectItem(CV->choose_mode,new_val,prop->subitem,edit_val.c_str(),&items)){
        edit_val			= new_val;
        prop->OnAfterEdit	(&edit_val);
        if (prop->ApplyValue(edit_val.c_str())){
        	Modified		();
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
	// последовательность важна (может быть 2 Apply)
    seNumber->Tag	= 0;
    if (seNumber->Visible&&Visible) 	tvProperties->SetFocus();
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
    seNumber->Tag				= 0;
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
	// последовательность важна (может быть 2 Apply)
    edText->Tag		= 0;
    if (edText->Visible&&Visible) 	tvProperties->SetFocus();
    edText->Hide	();
}
//---------------------------------------------------------------------------
void TProperties::PrepareLWText(TElTreeItem* item)
{
	PropItem* prop 		= (PropItem*)item->Tag;
    switch (prop->type){
    case PROP_R_TEXT:{
		RTextValue* V	= dynamic_cast<RTextValue*>(prop->GetFrontValue()); R_ASSERT(V);
        AnsiString edit_val	= *V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
        edText->EditMask	= "";
		edText->Text 		= edit_val;
		edText->MaxLength	= 0;
    }break;
    case PROP_A_TEXT:{
		ATextValue* V	= dynamic_cast<ATextValue*>(prop->GetFrontValue()); R_ASSERT(V);
        AnsiString edit_val	= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
        edText->EditMask	= "";
		edText->Text 		= edit_val;
		edText->MaxLength	= 0;
    }break;
    case PROP_TEXT:{
		TextValue* V		= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
		AnsiString edit_val	= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
        edText->EditMask	= "";
		edText->Text 		= edit_val;
		edText->MaxLength	= V->lim;
    }break;
    case PROP_TIME:{
		FloatValue* V		= dynamic_cast<FloatValue*>(prop->GetFrontValue()); R_ASSERT(V);
        float edit_val		= V->GetValue();
        prop->OnBeforeEdit	(&edit_val);
        edText->EditMask	= "!90:00:00;1;_";
		edText->Text 		= FloatTimeToStrTime(edit_val);
		edText->MaxLength	= 0;
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
    edText->Tag					= 0;
    if (item){
		PropItem* prop 			= (PropItem*)item->Tag;
        edText->Update();
	    switch (prop->type){
        case PROP_R_TEXT:{
			RTextValue* V		= dynamic_cast<RTextValue*>(prop->GetFrontValue()); R_ASSERT(V);
			AnsiString new_val	= edText->Text;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(new_val.c_str())){
            	Modified();
            }
            item->ColumnText->Strings[0] = prop->GetText();
        }break;
        case PROP_A_TEXT:{
			ATextValue* V		= dynamic_cast<ATextValue*>(prop->GetFrontValue()); R_ASSERT(V);
			AnsiString new_val	= edText->Text;
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(new_val.c_str())){
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
        case PROP_TIME:{
			FloatValue* V		= dynamic_cast<FloatValue*>(prop->GetFrontValue()); R_ASSERT(V);
			float new_val		= StrTimeToFloatTime(edText->Text.c_str());
			prop->OnAfterEdit	(&new_val);
            if (prop->ApplyValue(&new_val)){
	            Modified		();
			}
            item->ColumnText->Strings[0] = FloatTimeToStrTime(V->GetValue());
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
    	case PROP_R_TEXT:{
			AnsiString new_val	= edText->Text;
			if (TfrmText::RunEditor(new_val,AnsiString(item->Text).c_str()))
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
    	case PROP_TEXT:{
			TextValue* V	= dynamic_cast<TextValue*>(prop->GetFrontValue()); R_ASSERT(V);
			AnsiString new_val	= edText->Text;
			if (TfrmText::RunEditor(new_val,AnsiString(item->Text).c_str(),false,V->lim))
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
    	case PROP_R_TEXT:{
            RTextValue* V	= dynamic_cast<RTextValue*>(prop->GetFrontValue()); R_ASSERT(V);
            AnsiString edit_val	= *V->GetValue();
            prop->OnBeforeEdit	(&edit_val);
            if (TfrmText::RunEditor(edit_val,AnsiString(prop->Item()->Text).c_str(),false)){
                prop->OnAfterEdit	(&edit_val);
                if (prop->ApplyValue(edit_val.c_str()))
                    Modified();
                prop->Item()->ColumnText->Strings[0] = prop->GetText();
            }
        }break;
    	case PROP_A_TEXT:{
            ATextValue* V	= dynamic_cast<ATextValue*>(prop->GetFrontValue()); R_ASSERT(V);
            AnsiString edit_val	= V->GetValue();
            prop->OnBeforeEdit	(&edit_val);
            if (TfrmText::RunEditor(edit_val,AnsiString(prop->Item()->Text).c_str(),false)){
                prop->OnAfterEdit	(&edit_val);
                if (prop->ApplyValue(edit_val.c_str()))
                    Modified();
                prop->Item()->ColumnText->Strings[0] = prop->GetText();
            }
        }break;
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

bool __fastcall TProperties::IsModified()
{
	ApplyEditControl();
	return bModified;
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
	UI->CheckWindowPos(this);
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
    	if (prop&&prop->item&&prop->m_Flags.is(PropItem::flDrawThumbnail)) 
        	prop->item->OwnerHeight = !miDrawThumbnails->Checked;
    }
	UnlockUpdating		();
	tvProperties->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TProperties::ebOKClick(TObject *Sender)
{
	ModalResult = mrOk;	
}
//---------------------------------------------------------------------------

void __fastcall TProperties::ebCancelClick(TObject *Sender)
{
	ModalResult = mrCancel;	
}
//---------------------------------------------------------------------------

void __fastcall TProperties::tvPropertiesShowLineHint(TObject *Sender,
      TElTreeItem *Item, TElHeaderSection *Section, TElFString &Text,
      THintWindow *HintWindow, TPoint &MousePos, bool &DoShowHint)
{
    PropItem* prop 				= (PropItem*)Item->Tag;
    if (prop){
//    	HintWindow->Brush->Color= clGray;
		Text					= prop->GetText();
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::tvPropertiesCompareItems(TObject *Sender,
      TElTreeItem *Item1, TElTreeItem *Item2, int &res)
{
	u32 type1 = (u32)Item1->Data;
	u32 type2 = (u32)Item2->Data;
    if (type1==type2){
        if (Item1->Text<Item2->Text) 		res = -1;
        else if (Item1->Text>Item2->Text) 	res =  1;
        else if (Item1->Text==Item2->Text) 	res =  0;
    }else if (type1==TYPE_FOLDER)	    	res = -1;
    else if (type2==TYPE_FOLDER)	    	res =  1;
}
//---------------------------------------------------------------------------


