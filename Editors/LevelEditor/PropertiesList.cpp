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
	    for (PropValueIt it=m_Values.begin(); it!=m_Values.end(); it++)
    		_DELETE	(*it);
		m_Values.clear();
    }
}
//---------------------------------------------------------------------------
void __fastcall TProperties::ResetValues()
{
    for (PropValueIt it=m_Values.begin(); it!=m_Values.end(); it++)
        (*it)->ResetValue();
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

TProperties* TProperties::CreateForm(TWinControl* parent, TAlign align, TOnModifiedEvent modif, TOnItemFocused focused)
{
	TProperties* props = new TProperties(parent);
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
    _DELETE(m_BMCheck);
    _DELETE(m_BMDot);
    _DELETE(m_BMEllipsis);
}
//---------------------------------------------------------------------------

TElTreeItem* __fastcall TProperties::AddItem(TElTreeItem* parent, LPCSTR key, LPVOID value, PropValue* prop)
{
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
}

//CBlender* CSHEngineTools::AppendBlender(CLASS_ID cls_id, LPCSTR folder_name, CBlender* parent){
/*
void __fastcall TProperties::AddItems(TElTreeItem* parent, CStream& data)
{
	// parse data
    DWORD type;
    char key[255];
	TElTreeItem* marker_parent=parent;

    while (!data.Eof()){
        int sz=0;
        type = data.Rdword();
        data.RstringZ(key);
        switch(type){
        case xrPID_MARKER:			marker_parent=AddMarker(marker_parent,key,0); break;
        case xrPID_INTEGER:{	
        	sz=sizeof(xrP_Integer);	
            xrP_Integer* V = (xrP_Integer*)data.Pointer();
            AddIntItem(marker_parent,PROP_INTEGER,key,MakeIntValue(&V->value,V->min,V->max,1)); 
        }break;
        case xrPID_FLOAT:{ 
        	sz=sizeof(xrP_Float); 	
            xrP_Float* V = (xrP_Float*)data.Pointer();
            AddItem(marker_parent,PROP_FLOAT,key,MakeFloatValue(&V->value,V->min,V->max,0.01f,2)); 
        }break;
        case xrPID_BOOL:{ 	
        	sz=sizeof(xrP_BOOL); 	
            xrP_BOOL* V = (xrP_BOOL*)data.Pointer();
            AddItem(marker_parent,PROP_BOOLEAN,key,MakeBOOLValue(&V->value)); 
        }break;
        case xrPID_TOKEN:{ 	
        	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*(((xrP_TOKEN*)data.Pointer())->Count);
            xrP_TOKEN* V = (xrP_TOKEN*)data.Pointer();
            AddItem(marker_parent,PROP_TOKEN3,key,MakeTokenValue3(&V->IDselected,V->Count,(TokenValue3::Item*)(LPBYTE(data.Pointer()) + sizeof(xrP_TOKEN))));
        }break;
        default: THROW2("xrPID_????");
        }
        data.Advance(sz);
    }
    data.Seek(0);
}
*/
//---------------------------------------------------------------------------

void __fastcall TProperties::AssignValues(PropValueVec& values, bool full_expand, const AnsiString& title)
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
    m_Values				= values;
	for (PropValueIt it=m_Values.begin(); it!=m_Values.end(); it++){
    	PropValue* prop		= *it;
        prop->item			= FHelper.AppendObject(tvProperties,(*it)->key);
        prop->item->Tag	    = (int)prop;
        prop->item->UseStyles=true;
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
    	PropValue* prop 			= (PropValue*)Item->Tag;
        DWORD type 					= prop->type;
        if (prop->bEnabled){
            Surface->Font->Color 	= clBlack;
            Surface->Font->Style 	= TFontStyles();
        }else{
            Surface->Font->Color 	= clSilver;
            Surface->Font->Style 	= TFontStyles()<< fsBold;
        }
        if (prop->IsDiffValues()){ 
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
            DrawText	(Surface->Handle, "(mixed)", -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        }else{
            switch(type){
            case PROP_MARKER:
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
                ColorValue* V=(ColorValue*)prop;
                Surface->Brush->Color = (TColor)V->GetValue().get_windows();
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
                U32Value* V=(U32Value*)prop;
                Surface->Brush->Color = (TColor)rgb2bgr(V->GetValue());
                Surface->FillRect(R);
            }break;
            case PROP_FLAG:{
                FlagValue* V=(FlagValue*)prop;
                Surface->CopyMode = cmSrcAnd;//cmSrcErase;
                if (V->GetValue())	Surface->Draw(R.Left,R.Top+3,m_BMCheck);
                else				Surface->Draw(R.Left,R.Top+3,m_BMDot);
            }break;
            case PROP_BOOLEAN:{
                BOOLValue* V=(BOOLValue*)prop;
                Surface->CopyMode = cmSrcAnd;//cmSrcErase;
                if (V->GetValue())	Surface->Draw(R.Left,R.Top+3,m_BMCheck);
                else			   	Surface->Draw(R.Left,R.Top+3,m_BMDot);
            }break;
            case PROP_WAVE:
            case PROP_LIBSOUND:
            case PROP_LIGHTANIM:
            case PROP_LIBOBJECT:
            case PROP_GAMEOBJECT:
            case PROP_ENTITY:
            case PROP_TEXTURE:
            case PROP_TEXTURE2:
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
                    DrawText(Surface->Handle, prop->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
                }
            break;
	        };
        }
        // show LW edit
        if (prop->bEnabled){
            switch(type){
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
		PropValue* prop = (PropValue*)item->Tag;
        if (!prop) return;
		pmEnum->Tag = (int)item;
        switch(prop->type){
		case PROP_FLAG:{
        	FlagValue*	V 				= (FlagValue*)prop;
            BOOL new_val 				= !V->GetValue();
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){
				if (V->OnChange) V->OnChange(V);
                Modified();
	            RefreshForm();
            }
        }break;
		case PROP_BOOLEAN:{
        	BOOLValue* V				= (BOOLValue*)prop;
            BOOL new_val 				= !V->GetValue();
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){
				if (V->OnChange) V->OnChange(V);
                Modified();
	            RefreshForm();
            }
        }break;
		case PROP_TOKEN:{
            pmEnum->Items->Clear();
            TokenValue* T 	= (TokenValue*)prop;
            xr_token* token_list = T->token;
			TMenuItem* mi 	= new TMenuItem(0);
			mi->Caption 	= "-";
			pmEnum->Items->Add(mi);
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
            TokenValue2* T = (TokenValue2*)prop;
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
            TokenValue3* T 	= (TokenValue3*)prop;
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
            ListValue* T = (ListValue*)prop;
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
        case PROP_LIBSOUND:
        case PROP_LIGHTANIM:
        case PROP_LIBOBJECT:
        case PROP_GAMEOBJECT:
        case PROP_ENTITY:
        case PROP_TEXTURE:
        case PROP_ESHADER:
        case PROP_CSHADER:		CustomTextClick(item);      break;
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
    };
}
//---------------------------------------------------------------------------
void __fastcall TProperties::PMItemClick(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        TElTreeItem* item = (TElTreeItem*)pmEnum->Tag;
		PropValue* prop = (PropValue*)item->Tag;
        switch(prop->type){
		case PROP_TOKEN:{
        	TokenValue* V			= (TokenValue*)prop;
            xr_token* token_list   	= V->token;
            DWORD new_val			= token_list[mi->Tag].id;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){	
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_TOKEN2:{
        	TokenValue2* V			= (TokenValue2*)prop;
            DWORD new_val			= mi->Tag;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){	
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_TOKEN3:{
        	TokenValue3* V			= (TokenValue3*)prop;
            DWORD new_val			= V->items[mi->Tag].ID;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){	
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_LIST:{
        	ListValue* V		 	= (ListValue*)prop;
            AStringVec& lst		 	= V->items;
            string256 new_val;	strcpy(new_val,lst[mi->Tag].c_str());
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){	
				if (V->OnChange) V->OnChange(V);
	            Modified();
            }
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_TEXTURE2:{
        	TextValue* V		 	= (TextValue*)prop; 
			AnsiString edit_val	 	= V->GetValue();
			if (V->OnBeforeEdit) 	V->OnBeforeEdit(V,&edit_val);
            LPCSTR new_val 		 	= 0;
        	if (mi->Tag==0){
            	new_val 		 	= TfrmChoseItem::SelectTexture(false,edit_val.c_str(),true);
            }else if (mi->Tag>=2){
            	new_val			 	= TEXTUREString[mi->Tag];
            }
            if (new_val){
                edit_val		 	= new_val;
                if (V->OnAfterEdit) V->OnAfterEdit(V,&edit_val);
                if (V->ApplyValue(edit_val.c_str())){
					if (V->OnChange) V->OnChange(V);
	                Modified();
                }
                item->ColumnText->Strings[0]= V->GetText();
            }
        }break;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::WaveFormClick(TElTreeItem* item)
{
	PropValue* prop = (PropValue*)item->Tag;
    R_ASSERT(PROP_WAVE==prop->type);

    WaveValue* V			= (WaveValue*)prop;
    WaveForm edit_val		= V->GetValue();
	if (V->OnBeforeEdit)V->OnBeforeEdit(V,&edit_val);

	if (TfrmShaderFunction::Run(&edit_val)==mrOk){
        if (V->OnAfterEdit) V->OnAfterEdit(V,&edit_val);
        if (V->ApplyValue(edit_val)){
			if (V->OnChange) V->OnChange(V);
        	Modified();
        }
        item->ColumnText->Strings[0]= V->GetText();
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::ColorClick(TElTreeItem* item)
{
	PropValue* prop = (PropValue*)item->Tag;
    switch (prop->type){
    case PROP_FCOLOR:{
        ColorValue* V		= (ColorValue*)prop;
        Fcolor edit_val		= V->GetValue();
        if (V->OnBeforeEdit)V->OnBeforeEdit(V,&edit_val);

        DWORD ev 			= edit_val.get();
        if (SelectColor(&ev)){
	        edit_val.set	(ev);
            if (V->OnAfterEdit) V->OnAfterEdit(V,&edit_val);
            if (V->ApplyValue(edit_val)){
				if (V->OnChange) V->OnChange(V);
                item->RedrawItem(true);
                Modified();
            }
        }
    }break;
    case PROP_COLOR:{
        U32Value* V			= (U32Value*)prop;
        DWORD edit_val		= V->GetValue();
        if (V->OnBeforeEdit)V->OnBeforeEdit(V,&edit_val);

        if (SelectColor(&edit_val)){
            if (V->OnAfterEdit) V->OnAfterEdit(V,&edit_val);
            if (V->ApplyValue(edit_val)){	
				if (V->OnChange) V->OnChange(V);
                item->RedrawItem(true);
                Modified();
            }
        }
    }break;
    default: THROW2("Unsupported type");
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::VectorClick(TElTreeItem* item)
{
    VectorValue* V 	= (VectorValue*)item->Tag;
    VERIFY(V->type==PROP_VECTOR);
    Fvector edit_val= V->GetValue();
	if (V->OnBeforeEdit) 	V->OnBeforeEdit(V,&edit_val);
    Fvector mn={V->lim_mn,V->lim_mn,V->lim_mn},mx={V->lim_mx,V->lim_mx,V->lim_mx};
	if (NumericVectorRun(AnsiString(item->Text).c_str(),&edit_val,V->dec,&edit_val,&mn,&mx)){
        if (V->OnAfterEdit) V->OnAfterEdit(V,&edit_val);
        if (V->ApplyValue(edit_val)){
			if (V->OnChange) V->OnChange(V);
			item->RedrawItem(true);
            Modified();
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::CustomTextClick(TElTreeItem* item)
{
	TextValue* V			= (TextValue*)item->Tag;
	AnsiString edit_val		= V->GetValue();
	if (V->OnBeforeEdit) 	V->OnBeforeEdit(V,&edit_val);
    LPCSTR new_val=0;
    switch (V->type){
    case PROP_ESHADER:		new_val	= TfrmChoseItem::SelectShader(edit_val.c_str());			break;
	case PROP_CSHADER:		new_val	= TfrmChoseItem::SelectShaderXRLC(edit_val.c_str());		break;
    case PROP_TEXTURE:		new_val = TfrmChoseItem::SelectTexture(false,edit_val.c_str(),true);break;
	case PROP_LIGHTANIM:	new_val = TfrmChoseItem::SelectLAnim(false,0,edit_val.c_str());		break;
    case PROP_LIBOBJECT:	new_val = TfrmChoseItem::SelectObject(false,0,edit_val.c_str());	break;
    case PROP_GAMEOBJECT:	new_val = TfrmChoseItem::SelectGameObject(false,0,edit_val.c_str());break;
    case PROP_ENTITY:		new_val = TfrmChoseItem::SelectEntity(edit_val.c_str());			break;
    case PROP_LIBSOUND:		new_val = TfrmChoseItem::SelectSound(false,edit_val.c_str(),true);	break;
    default: THROW2("Unknown prop");
    }
    if (new_val){
        edit_val			= new_val;
        if (V->OnAfterEdit) V->OnAfterEdit(V,&edit_val);
        if (V->ApplyValue(edit_val.c_str())){	
			if (V->OnChange) V->OnChange(V);
        	Modified();
        }
        item->ColumnText->Strings[0]= V->GetText();
    }
}
//---------------------------------------------------------------------------

void __fastcall TProperties::CustomAnsiTextClick(TElTreeItem* item)
{
	ATextValue* V		= (ATextValue*)item->Tag;
	AnsiString edit_val		= V->GetValue();
	if (V->OnBeforeEdit) 	V->OnBeforeEdit(V,&edit_val);
    LPCSTR new_val=0;
    switch (V->type){
    case PROP_A_ESHADER:	new_val	= TfrmChoseItem::SelectShader(edit_val.c_str());			break;
	case PROP_A_CSHADER:	new_val	= TfrmChoseItem::SelectShaderXRLC(edit_val.c_str());		break;
    case PROP_A_TEXTURE:	new_val = TfrmChoseItem::SelectTexture(false,edit_val.c_str(),true);break;
    default: THROW2("Unknown props");
    }
    if (new_val){
        edit_val				= new_val;
        if (V->OnAfterEdit) 	V->OnAfterEdit(V,&edit_val);
        if (V->ApplyValue(edit_val)){ 
			if (V->OnChange) V->OnChange(V);
        	Modified();
        }
        item->ColumnText->Strings[0]= V->GetText();
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
	PropValue* prop = (PropValue*)item->Tag;
    switch (prop->type){
    case PROP_U8:{
        U8Value* V 			= (U8Value*)prop; VERIFY(V);
        u8 edit_val        	= V->GetValue();
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_U16:{
        U16Value* V 		= (U16Value*)prop; VERIFY(V);
        u16 edit_val        = V->GetValue();
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_U32:{
        U32Value* V 		= (U32Value*)prop; VERIFY(V);
        u32 edit_val        = V->GetValue();
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_S8:{
        S8Value* V 			= (S8Value*)prop; VERIFY(V);
        s8 edit_val        	= V->GetValue();
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_S16:{
        S16Value* V 		= (S16Value*)prop; VERIFY(V);
        s16 edit_val        = V->GetValue();
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_S32:{
        S32Value* V 		= (S32Value*)prop; VERIFY(V);
        s32 edit_val        = V->GetValue();
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=0.01f;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_FLOAT:{
        FloatValue* V 		= (FloatValue*)prop; VERIFY(V);
        float edit_val		= V->GetValue();
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
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
	TElTreeItem* item = (TElTreeItem*)seNumber->Tag;
    if (item){
		PropValue* prop = (PropValue*)item->Tag;
        seNumber->Update();
	    switch (prop->type){
    	case PROP_U8:{
	        U8Value* V 	= (U8Value*)prop; VERIFY(V);
            u8 new_val	= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){ 
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
        }break;
    	case PROP_U16:{
	        U16Value* V = (U16Value*)prop; VERIFY(V);
            u16 new_val	= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){ 
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
        }break;
    	case PROP_U32:{
	        U32Value* V = (U32Value*)prop; VERIFY(V);
            u32 new_val	= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){ 
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
        }break;
    	case PROP_S8:{
	        S8Value* V 	= (S8Value*)prop; VERIFY(V);
            s8 new_val	= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){ 
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
        }break;
    	case PROP_S16:{
	        S16Value* V = (S16Value*)prop; VERIFY(V);
            s16 new_val	= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){ 
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
        }break;
    	case PROP_S32:{
	        S32Value* V = (S32Value*)prop; VERIFY(V);
            s32 new_val	= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){ 
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
        }break;
	    case PROP_FLOAT:{
	        FloatValue* V 	= (FloatValue*)prop; VERIFY(V);
            float new_val	= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){ 
				if (V->OnChange) V->OnChange(V);
            	Modified();
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
	PropValue* prop = (PropValue*)item->Tag;
    switch (prop->type){
    case PROP_A_TEXT:{
        ATextValue* V	= (ATextValue*)prop; VERIFY(V);
        AnsiString edit_val	= V->GetValue();
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
	    edText->Text 		= edit_val;
    }break;
    case PROP_TEXT:{
        TextValue* V 		= (TextValue*)prop; VERIFY(V);
		AnsiString edit_val	= V->GetValue();
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
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
	TElTreeItem* item = (TElTreeItem*)edText->Tag;
    if (item){
		PropValue* prop = (PropValue*)item->Tag;
        edText->Update();
	    switch (prop->type){
        case PROP_A_TEXT:{
	        ATextValue* V 	= (ATextValue*)prop; VERIFY(V);
			AnsiString new_val	= edText->Text;
            edText->MaxLength	= 0;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val)){ 
				if (V->OnChange) V->OnChange(V);
            	Modified();
            }
            item->ColumnText->Strings[0] = V->GetText();
        }break;
    	case PROP_TEXT:{
	        TextValue* V 		= (TextValue*)prop; VERIFY(V);
            edText->MaxLength	= V->lim;
			AnsiString new_val	= edText->Text;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (V->ApplyValue(new_val.c_str())){ 
				if (V->OnChange) V->OnChange(V);
	            Modified();
            }
            item->ColumnText->Strings[0] = V->GetText();
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
		PropValue* prop = (PropValue*)item->Tag;
        edText->Update();
	    switch (prop->type){
    	case PROP_TEXT:{
	        TextValue* V 	= (TextValue*)prop;
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

