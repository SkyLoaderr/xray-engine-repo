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
#pragma resource "*.dfm"

const LPSTR BOOLString[2]={"False","True"};

LPCSTR	FlagValue::GetText(){
	DWORD draw_val 	= *val;
    if (OnDrawValue)OnDrawValue(this, &draw_val);
	return BOOLString[!!((draw_val)&mask)];
}
LPCSTR 	TokenValue::GetText(){
	DWORD draw_val 	= *val;
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
LPCSTR	ListValue::GetText(){
    static AnsiString draw_text = val;
    if (OnDrawValue)OnDrawValue(this, &draw_text);
    return draw_text.c_str();
}

//---------------------------------------------------------------------------
void __fastcall TfrmProperties::BeginFillMode(const AnsiString& title, LPCSTR section)
{
	bFillMode=true;
	tvProperties->IsUpdating = true;
    if (section){
    	TElTreeItem* node = FOLDER::FindFolder(tvProperties,section);
        if (node) node->Delete();
    }else{
	    tvProperties->Items->Clear();
    }
    Caption = title;
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::EndFillMode(bool bFullExpand)
{
	bFillMode=false;
    bModified=false;
	if (bFullExpand) tvProperties->FullExpand();
    tvProperties->IsUpdating = false;
};
//---------------------------------------------------------------------------
void __fastcall TfrmProperties::ClearProperties()
{
	tvProperties->IsUpdating = true;
    tvProperties->Items->Clear();
    tvProperties->IsUpdating = false;
    for (PropValIt it=m_Params.begin(); it!=m_Params.end(); it++)
    	_DELETE(*it);
	m_Params.clear();
}
//---------------------------------------------------------------------------

__fastcall TfrmProperties::TfrmProperties(TComponent* Owner)
	: TForm(Owner)
{
	bModified = false;
	bFillMode = false;
    DEFINE_INI(fsStorage);
//	m_BMEllipsis = new Graphics::TBitmap();
//	m_BMEllipsis->LoadFromResourceName((DWORD)HInstance,"ELLIPSIS");
    seNumber->Parent = tvProperties;
    seNumber->Hide();
}
//---------------------------------------------------------------------------

TfrmProperties* TfrmProperties::CreateProperties	(TWinControl* parent, TAlign align, TOnModifiedEvent modif)
{
	TfrmProperties* props = new TfrmProperties(parent);
    props->OnModifiedEvent = modif;
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
//	ClearProperties();

//	Action = caFree;
//    _DELETE(m_BMEllipsis);
}
//---------------------------------------------------------------------------

TElTreeItem* __fastcall TfrmProperties::AddItem(TElTreeItem* parent, DWORD type, LPCSTR key, LPVOID value){
	R_ASSERT(bFillMode);
    TElTreeItem* TI     = tvProperties->Items->AddChildObject(parent,key,(TObject*)value);
    TI->Tag	            = type;
    TI->UseStyles 		= true;
    TElCellStyle* CS    = TI->AddStyle();
    CS->OwnerProps 		= true;

    switch (type){
    case PROP_MARKER:	CS->CellType = sftUndef;	break;
    case PROP_MARKER2:	CS->CellType = sftUndef;	TI->ColumnText->Add(AnsiString((LPSTR)value)); break;
    case PROP_WAVE:		CS->CellType = sftUndef;	TI->ColumnText->Add("[Wave]");	CS->Style = ElhsOwnerDraw; break;
    case PROP_BOOL:		CS->CellType = sftUndef;	TI->ColumnText->Add(BOOLString[*(LPDWORD)value]);	CS->Style = ElhsOwnerDraw; break;
    case PROP_FLAG:
    case PROP_TOKEN:
    case PROP_TOKEN2:
    case PROP_LIST:
    case PROP_FLOAT:
    case PROP_INTEGER:{	CS->CellType = sftUndef; 	PropValue*  P=(PropValue*)value; 	TI->ColumnText->Add(P->GetText()); 	CS->Style = ElhsOwnerDraw; }break;
    case PROP_TEXT:		CS->CellType = sftText; 	TI->ColumnText->Add((LPSTR)value);  break;
    case PROP_COLOR:	CS->CellType = sftUndef; 	CS->Style = ElhsOwnerDraw; break;
    case PROP_TEXTURE:	CS->CellType = sftUndef; 	TI->ColumnText->Add((LPSTR)value);  CS->Style = ElhsOwnerDraw; break;
    case PROP_SH_ENGINE:CS->CellType = sftUndef; 	TI->ColumnText->Add((LPSTR)value);  CS->Style = ElhsOwnerDraw; break;
    case PROP_SH_COMPILE:CS->CellType = sftUndef; 	TI->ColumnText->Add((LPSTR)value);  CS->Style = ElhsOwnerDraw; break;
    case PROP_S_TEXTURE:	CS->CellType = sftUndef;TI->ColumnText->Add(*(AnsiString*)value);  CS->Style = ElhsOwnerDraw; break;
    case PROP_S_SH_ENGINE:	CS->CellType = sftUndef;TI->ColumnText->Add(*(AnsiString*)value);  CS->Style = ElhsOwnerDraw; break;
    case PROP_S_SH_COMPILE:	CS->CellType = sftUndef;TI->ColumnText->Add(*(AnsiString*)value);  CS->Style = ElhsOwnerDraw; break;
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
        case PROP_S_TEXTURE:
        case PROP_S_SH_ENGINE:
        case PROP_S_SH_COMPILE:
        case PROP_TEXTURE:
        case PROP_SH_ENGINE:
        case PROP_SH_COMPILE:
        case PROP_WAVE:{
            R.Right	-=	1;
            R.Left 	+= 	1;
            AnsiString name = Item->ColumnText->Strings[0];
    		DrawText	(Surface->Handle, name.IsEmpty()?"[none]":name.c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        }break;
        case PROP_COLOR:{
			Surface->Brush->Style = bsSolid;
		    Surface->Brush->Color = 0x00000000;
            Surface->FrameRect(R);
            R.Right	-=	1;
            R.Left 	+= 	1;
            R.Top	+=	1;
            R.Bottom-= 	1;
		    Surface->Brush->Color = rgb2bgr(*(LPDWORD)Item->Data);
            Surface->FillRect(R);
        }break;
        case PROP_BOOL:
            R.Right	-=	10;
            R.Left 	+= 	1;
    		DrawText	(Surface->Handle, AnsiString(Item->ColumnText->Strings[0]).c_str(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            R.Left 	= 	R.Right;
            R.Right += 	10;
            DrawArrow	(Surface, eadDown, R, clWindowText, true);
        break;
        case PROP_FLAG:
		case PROP_TOKEN:
        case PROP_TOKEN2:
        case PROP_LIST:
            R.Right	-=	10;
            R.Left 	+= 	1;
    		DrawText	(Surface->Handle, ((PropValue*)Item->Data)->GetText(), -1, &R, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            R.Left 	= 	R.Right;
            R.Right += 	10;
            DrawArrow	(Surface, eadDown, R, clWindowText, true);
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
	CancelLWNumber();
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
        case PROP_WAVE: 	CustomClick(item); 	break;
        case PROP_COLOR: 	ColorClick(item); 	break;
        case PROP_TEXTURE: 	TextureClick(item);	break;
        case PROP_SH_ENGINE:ShaderEngineClick(item); 	break;
        case PROP_SH_COMPILE:ShaderCompileClick(item); 	break;
        case PROP_S_TEXTURE: 	STextureClick(item);	break;
        case PROP_S_SH_ENGINE:	SShaderEngineClick(item); 	break;
        case PROP_S_SH_COMPILE:	SShaderCompileClick(item); 	break;
        case PROP_INTEGER:
        case PROP_FLOAT:
        	PrepareLWNumber(item);
        break;
        };
        switch(type){
        case PROP_TOKEN:
        case PROP_TOKEN2:
        case PROP_LIST:
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
                Modified();
            }
        break;
		case PROP_FLAG:{
        	FlagValue*	V 				= (FlagValue*)item->Data;
            DWORD new_val 				= *V->val;
            if (mi->MenuIndex) 	new_val |= V->mask;
            else				new_val &=~V->mask;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (new_val!= *V->val){
            	*V->val 	= new_val;
                Modified();
            }
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_TOKEN:{
        	TokenValue* V				= (TokenValue*)item->Data;
            xr_token* token_list 	   	= V->token;
            DWORD new_val				= token_list[mi->MenuIndex].id;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (*V->val	!= new_val){
    	        *V->val		= token_list[mi->MenuIndex].id;
                Modified();
            }
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_TOKEN2:{
        	TokenValue2* V				= (TokenValue2*)item->Data;
            AStringVec& lst				= V->items;
            DWORD new_val				= mi->MenuIndex;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (*V->val	!= new_val){
    	        *V->val		= mi->MenuIndex;
                Modified();                          
            }
			item->ColumnText->Strings[0]= V->GetText();
        }break;
		case PROP_LIST:{
        	ListValue* V				= (ListValue*)item->Data;
            AStringVec& lst				= V->items;
            string256 new_val;
            strcpy(new_val,lst[mi->MenuIndex].c_str());
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (strcmp(V->val,new_val)){
    	        strcpy(V->val,new_val);
                Modified();
            }
			item->ColumnText->Strings[0]= V->GetText();
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

void __fastcall TfrmProperties::TextureClick(TElTreeItem* item)
{
	VERIFY(PROP_TEXTURE==item->Tag);
    LPSTR tex = (LPSTR)item->Data;
    LPCSTR name = TfrmChoseItem::SelectTexture(false,tex);
    if (name&&name[0]&&strcmp(tex,name)!=0){
        strcpy(tex,name);
        item->ColumnText->Strings[0]= name;
		Modified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::ShaderEngineClick(TElTreeItem* item)
{
	VERIFY(PROP_SH_ENGINE==item->Tag);
    LPSTR sh = (LPSTR)item->Data;
    LPCSTR name = TfrmChoseItem::SelectShader(sh);
    if (name&&name[0]&&strcmp(sh,name)!=0){
        strcpy(sh,name);
        item->ColumnText->Strings[0]= name;
		Modified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::ShaderCompileClick(TElTreeItem* item)
{
	VERIFY(PROP_SH_COMPILE==item->Tag);
    LPSTR sh = (LPSTR)item->Data;
    LPCSTR name = TfrmChoseItem::SelectShaderXRLC(sh);
    if (name&&name[0]&&strcmp(sh,name)!=0){
        strcpy(sh,name);
        item->ColumnText->Strings[0]= name;
		Modified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::STextureClick(TElTreeItem* item)
{
	VERIFY(PROP_S_TEXTURE==item->Tag);
    AnsiString& tex = *(AnsiString*)item->Data;
    LPCSTR name = TfrmChoseItem::SelectTexture(false,tex.c_str());
    if (name&&name[0]&&strcmp(tex.c_str(),name)!=0){
        tex=name;
        item->ColumnText->Strings[0]= name;
		Modified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::SShaderEngineClick(TElTreeItem* item)
{
	VERIFY(PROP_S_SH_ENGINE==item->Tag);
    AnsiString& sh = *(AnsiString*)item->Data;
    LPCSTR name = TfrmChoseItem::SelectShader(sh.c_str());
    if (name&&name[0]&&strcmp(sh.c_str(),name)!=0){
        sh=name;
        item->ColumnText->Strings[0]= name;
		Modified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmProperties::SShaderCompileClick(TElTreeItem* item)
{
	VERIFY(PROP_S_SH_COMPILE==item->Tag);
    AnsiString& sh = *(AnsiString*)item->Data;
    LPCSTR name = TfrmChoseItem::SelectShaderXRLC(sh.c_str());
    if (name&&name[0]&&strcmp(sh.c_str(),name)!=0){
        sh=name;
        item->ColumnText->Strings[0]= name;
		Modified();
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
			Modified();
	    }
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// LW style inplace editor
//---------------------------------------------------------------------------
void TfrmProperties::CancelLWNumber()
{
/*
не нужно вызывать After
	TElTreeItem* item = (TElTreeItem*)seNumber->Tag;
    if (item){
		PropValue* V = (PropValue*)item->Data; VERIFY(V);
		if (V->OnAfterEdit) V->OnAfterEdit(V);
    }
*/
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
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=float(V->inc)/100;
	    seNumber->Decimal  	= 0;
    	seNumber->ValueType	= vtInt;
	    seNumber->Value 	= edit_val;
    }break;
    case PROP_FLOAT:{
        FloatValue* V 		= (FloatValue*)item->Data; VERIFY(V);
        float edit_val		= *V->val;
        if (V->OnBeforeEdit) V->OnBeforeEdit(V,&edit_val);
		seNumber->MinValue 	= V->lim_mn;
        seNumber->MaxValue 	= V->lim_mx;
	    seNumber->Increment	= V->inc;
        seNumber->LWSensitivity=V->inc/100;
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
    seNumber->Top  	= R.Top+18;
    seNumber->Width	= R.Right-R.Left+2;
    seNumber->Height= R.Bottom-R.Top+3;
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
            int new_val		= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
            if (*V->val != new_val){
                *V->val 	= new_val;
				Modified();
            }
            item->ColumnText->Strings[0] = V->GetText();
        }break;
	    case PROP_FLOAT:{
	        FloatValue* V 	= (FloatValue*)item->Data; VERIFY(V);
            float new_val	= seNumber->Value;
			if (V->OnAfterEdit) V->OnAfterEdit(V,&new_val);
		    if (!fsimilar(*V->val,new_val)){
                *V->val 	= new_val;
				Modified();
		    }
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

void __fastcall TfrmProperties::seNumberLWChange(TObject *Sender, int Val)
{
	ApplyLWNumber();
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

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
void __fastcall TfrmProperties::FillFromStream(CFS_Memory& stream, DWORD advance)
{
    CStream data(stream.pointer(), stream.size());
    data.Advance(advance);
    DWORD type;
    char key[255];
    TElTreeItem* M=0;
    TElTreeItem* node;
/*
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
        if (type==BPID_MARKER) M = 0;
        node = form->AddItem(M,type,key,(LPDWORD)data.Pointer());
        if (type==BPID_MARKER) M = node;
        data.Advance(sz);
    }
*/
}
//---------------------------------------------------------------------------



