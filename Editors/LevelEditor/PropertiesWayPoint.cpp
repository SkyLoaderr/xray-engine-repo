#include "stdafx.h"
#pragma hdrstop

#include "PropertiesWayPoint.h"
#include "WayPoint.h"
#include "Scene.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma link "ElPgCtl"
#pragma link "ElXPThemedControl"
#pragma resource "*.dfm"

TfrmPropertiesWayPoint* TfrmPropertiesWayPoint::form = 0;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesWayPoint::TfrmPropertiesWayPoint(TComponent* Owner)
    : TForm(Owner)
{
	bLoadMode = false;
}
//---------------------------------------------------------------------------

void TfrmPropertiesWayPoint::GetPointInfo(){
	bLoadMode = true;

	VERIFY(!m_Points.empty());
	WPIt _F = m_Points.begin();

	CWayPoint *_S 	= (CWayPoint*)(*_F); _F++;
    cb00000001->ObjFirstInit((_S->m_dwFlags&0x00000001)?cbChecked:cbUnchecked);
    cb00000002->ObjFirstInit((_S->m_dwFlags&0x00000002)?cbChecked:cbUnchecked);
    cb00000004->ObjFirstInit((_S->m_dwFlags&0x00000004)?cbChecked:cbUnchecked);
    cb00000008->ObjFirstInit((_S->m_dwFlags&0x00000008)?cbChecked:cbUnchecked);
    cb00000010->ObjFirstInit((_S->m_dwFlags&0x00000010)?cbChecked:cbUnchecked);
    cb00000020->ObjFirstInit((_S->m_dwFlags&0x00000020)?cbChecked:cbUnchecked);
    cb00000040->ObjFirstInit((_S->m_dwFlags&0x00000040)?cbChecked:cbUnchecked);
    cb00000080->ObjFirstInit((_S->m_dwFlags&0x00000080)?cbChecked:cbUnchecked);
    cb00000100->ObjFirstInit((_S->m_dwFlags&0x00000100)?cbChecked:cbUnchecked);
    cb00000200->ObjFirstInit((_S->m_dwFlags&0x00000200)?cbChecked:cbUnchecked);
    cb00000400->ObjFirstInit((_S->m_dwFlags&0x00000400)?cbChecked:cbUnchecked);
    cb00000800->ObjFirstInit((_S->m_dwFlags&0x00000800)?cbChecked:cbUnchecked);
    cb00001000->ObjFirstInit((_S->m_dwFlags&0x00001000)?cbChecked:cbUnchecked);
    cb00002000->ObjFirstInit((_S->m_dwFlags&0x00002000)?cbChecked:cbUnchecked);
    cb00004000->ObjFirstInit((_S->m_dwFlags&0x00004000)?cbChecked:cbUnchecked);
    cb00008000->ObjFirstInit((_S->m_dwFlags&0x00008000)?cbChecked:cbUnchecked);

	for(;_F!=m_Points.end();_F++){
	    _S 	= (CWayPoint*)(*_F);
        cb00000001->ObjNextInit((_S->m_dwFlags&0x00000001)?cbChecked:cbUnchecked);
        cb00000002->ObjNextInit((_S->m_dwFlags&0x00000002)?cbChecked:cbUnchecked);
        cb00000004->ObjNextInit((_S->m_dwFlags&0x00000004)?cbChecked:cbUnchecked);
        cb00000008->ObjNextInit((_S->m_dwFlags&0x00000008)?cbChecked:cbUnchecked);
        cb00000010->ObjNextInit((_S->m_dwFlags&0x00000010)?cbChecked:cbUnchecked);
        cb00000020->ObjNextInit((_S->m_dwFlags&0x00000020)?cbChecked:cbUnchecked);
        cb00000040->ObjNextInit((_S->m_dwFlags&0x00000040)?cbChecked:cbUnchecked);
        cb00000080->ObjNextInit((_S->m_dwFlags&0x00000080)?cbChecked:cbUnchecked);
        cb00000100->ObjNextInit((_S->m_dwFlags&0x00000100)?cbChecked:cbUnchecked);
        cb00000200->ObjNextInit((_S->m_dwFlags&0x00000200)?cbChecked:cbUnchecked);
        cb00000400->ObjNextInit((_S->m_dwFlags&0x00000400)?cbChecked:cbUnchecked);
        cb00000800->ObjNextInit((_S->m_dwFlags&0x00000800)?cbChecked:cbUnchecked);
        cb00001000->ObjNextInit((_S->m_dwFlags&0x00001000)?cbChecked:cbUnchecked);
        cb00002000->ObjNextInit((_S->m_dwFlags&0x00002000)?cbChecked:cbUnchecked);
        cb00004000->ObjNextInit((_S->m_dwFlags&0x00004000)?cbChecked:cbUnchecked);
        cb00008000->ObjNextInit((_S->m_dwFlags&0x00008000)?cbChecked:cbUnchecked);
    }
	bLoadMode = false;
}

#define CB_APPLY(a,b) {int p=_S->m_dwFlags&b; a->ObjApply(p); if (p) _S->m_dwFlags|=b; else _S->m_dwFlags&=~b;}

bool TfrmPropertiesWayPoint::ApplyPointInfo(){
    VERIFY( !m_Points.empty() );
	WPIt _F = m_Points.begin();
	for(;_F!=m_Points.end();_F++){
    	CWayPoint *_S = (CWayPoint*)(*_F);
        CB_APPLY(cb00000001,0x00000001);
        CB_APPLY(cb00000002,0x00000002);
        CB_APPLY(cb00000004,0x00000004);
        CB_APPLY(cb00000008,0x00000008);
        CB_APPLY(cb00000010,0x00000010);
        CB_APPLY(cb00000020,0x00000020);
        CB_APPLY(cb00000040,0x00000040);
        CB_APPLY(cb00000080,0x00000080);
        CB_APPLY(cb00000100,0x00000100);
        CB_APPLY(cb00000200,0x00000200);
        CB_APPLY(cb00000400,0x00000400);
        CB_APPLY(cb00000800,0x00000800);
        CB_APPLY(cb00001000,0x00001000);
        CB_APPLY(cb00002000,0x00002000);
        CB_APPLY(cb00004000,0x00004000);
        CB_APPLY(cb00008000,0x00008000);
    }
    return true;
}

void TfrmPropertiesWayPoint::GetWayInfo(){
	bLoadMode = true;

	VERIFY(!m_Objects->empty());
	ObjectIt _F = m_Objects->begin();
    VERIFY( (*_F)->ClassID==OBJCLASS_WAY );

    CWayObject *_S 	= (CWayObject*)(*_F); _F++;
	edName->Text	= _S->Name;
    rgType->ItemIndex=_S->m_Type;

	edName->Enabled 	= (m_Objects->size()==1);
    if (m_Objects->size()>1)edName->Text = "<Multiple selection>";
	for(;_F!=m_Objects->end();_F++){
	    CWayObject *_S 	= (CWayObject*)(*_F);
        if (rgType->ItemIndex!=int(_S->m_Type)) rgType->Font->Style = TFontStyles()<<fsItalic;
	    rgType->ItemIndex=_S->m_Type;
    }
	bLoadMode = false;
}

bool TfrmPropertiesWayPoint::ApplyWayInfo(){
    VERIFY( !m_Objects->empty() );
	ObjectIt _F = m_Objects->begin();
    bool bMultiSel = (m_Objects->size()>1);
	for(;_F!=m_Objects->end();_F++){
    	CWayObject *_O = (CWayObject*)(*_F);
        if (!bMultiSel){
        	if (Scene.FindObjectByName(edName->Text.c_str(),_O)){
            	ELog.DlgMsg(mtError,"Name already exist in scene: '%s'",edName->Text.c_str());
            	return false;
            }
        	_O->Name = edName->Text.c_str();
	    }
		if (!rgType->Font->Style.Contains(fsItalic)) _O->m_Type = EWayType(rgType->ItemIndex);
	}
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesWayPoint::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesWayPoint::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
    pcProps->ActivePage		= bPointMode?tsWayPoint:tsBasic;
    tsBasic->TabVisible		= !bPointMode;
    tsWayPoint->TabVisible 	= bPointMode;
    tsBasic->Hide();
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesWayPoint::OnModified(TObject *Sender)
{
	if (bLoadMode) return;
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesWayPoint::ebOkClick(TObject *Sender)
{
	if (bPointMode){ 	if (!ApplyPointInfo()) return;
	}else{				if (!ApplyWayInfo()) return;}
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesWayPoint::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

int __fastcall TfrmPropertiesWayPoint::Run(ObjectList* pObjects, bool& bChange)
{
	VERIFY(!TfrmPropertiesWayPoint::form);
	form = new TfrmPropertiesWayPoint(0);
	form->m_Objects = pObjects; VERIFY(form->m_Objects);
    form->bPointMode=CWayObject::IsPointMode();
    if (form->bPointMode){
    	if (1!=pObjects->size()){
        	ELog.DlgMsg(mtInformation, "Select one WayObject");
            _DELETE(form);
            return mrCancel;
        }
        ((CWayObject*)pObjects->back())->GetSelectedPoints(form->m_Points);
        if (form->m_Points.empty()){
        	ELog.DlgMsg(mtInformation, "Select at least one WayPoint");
            _DELETE(form);
            return mrCancel;
        }
        form->GetPointInfo();
    }else{
        form->GetWayInfo();
    }
    int res = form->ShowModal();
    bChange = (res==mrOk);
    return res;
}

void __fastcall TfrmPropertiesWayPoint::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
    form = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesWayPoint::rgTypeClick(TObject *Sender)
{
	if (bLoadMode) return;
	OnModified(Sender);
	rgType->Font->Style = TFontStyles();
}
//---------------------------------------------------------------------------

