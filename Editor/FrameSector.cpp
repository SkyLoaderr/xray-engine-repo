#include "stdafx.h"
#pragma hdrstop

#include "FrameSector.h"
#include "sceneobject.h"
#include "scene.h"
#include "sector.h"
#include "propertiessector.h"
#include "ui_tools.h"
#include "portal.h"
#include "portalutils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "RXCtrls"
#pragma link "RxMenus"
#pragma link "multi_edit"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraSector::TfraSector(TComponent* Owner)
        : TFrame(Owner)
{
	OnChange();
}
//---------------------------------------------------------------------------
void __fastcall TfraSector::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfraSector::TopClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfraSector::FindAndSelectSectorInList(const char* _name){
    cbItems->ItemIndex=cbItems->Items->IndexOf(AnsiString(_name));
    OnSectorUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TfraSector::OnChange(){
	AnsiString cur="";
    cbItems->Items->Clear();
    ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
    ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++){
    	cbItems->Items->AddObject((*_F)->GetName(),(TObject*)(*_F));
        if ((*_F)->Selected()&&(*_F)->Visible())
        	if(!cur.Length()) cur=(*_F)->GetName();
            else cur="-";
    }
    FindAndSelectSectorInList(cur.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfraSector::OnSectorUpdate(){
	if (cbItems->ItemIndex>-1){
    	CSector* _O=(CSector*)cbItems->Items->Objects[cbItems->ItemIndex];
        AnsiString tmp;
        tmp.sprintf("%d %s",_O->GetSectorFacesCount(),(_O->GetSectorFacesCount()>1?" faces":" face"));
		lbFacesCount->Caption = tmp;
    }else{
		lbFacesCount->Caption = "-";
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraSector::ebCreateNewClick(TObject *Sender)
{
	char namebuffer[MAX_OBJ_NAME];
	Scene->GenObjectName( OBJCLASS_SECTOR, namebuffer );
	CSector* _O = new CSector(namebuffer);
	Scene->SelectObjects(false,OBJCLASS_SECTOR);
	Scene->AddObject( _O );
    FindAndSelectSectorInList(namebuffer);
}
//---------------------------------------------------------------------------

void __fastcall TfraSector::cbItemsChange(TObject *Sender)
{
	if (cbItems->ItemIndex>-1){
    	CSector* _O=(CSector*)cbItems->Items->Objects[cbItems->ItemIndex];
		Scene->SelectObjects(false,OBJCLASS_SECTOR);
        _O->Select(true);
    }
    OnSectorUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TfraSector::ebCaptureInsideVolumeClick(TObject *Sender)
{
	if (cbItems->ItemIndex>-1){
		CSector* sector=(CSector*)cbItems->Items->Objects[cbItems->ItemIndex];
    	sector->CaptureInsideVolume();
	    OnSectorUpdate();
        Scene->UndoSave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraSector::ebCreateDefaultClick(TObject *Sender)
{
    SceneObject* O=Scene->FindObjectByName(DEFAULT_SECTOR_NAME,OBJCLASS_SECTOR);
	if (O) Log->DlgMsg(mtInformation,"Default sector already present. Remove this and try again.");
    else{
		if (!PortalUtils.CreateDefaultSector()) Log->DlgMsg(mtInformation,"Default can't created.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraSector::ebRemoveDefaultClick(TObject *Sender)
{
	if (!PortalUtils.RemoveDefaultSector()) Log->DlgMsg(mtInformation,"Default sector not found.");
}
//---------------------------------------------------------------------------

void __fastcall TfraSector::ebValidateClick(TObject *Sender)
{
	PortalUtils.Validate(true);
}
//---------------------------------------------------------------------------

void __fastcall TfraSector::ExtBtn3Click(TObject *Sender)
{
    ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
    ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++){
		CSector* sector=(CSector*)(*_F);
        if (sector->Selected()&&sector->Visible()) sector->Update(true);
    }
    OnSectorUpdate();
    Scene->UndoSave();
/*	if (cbItems->ItemIndex>-1){
		CSector* sector=(CSector*)cbItems->Items->Objects[cbItems->ItemIndex];
        sector->Update(true);
        OnSectorUpdate();
        Scene->UndoSave();
    }
*/
}
//---------------------------------------------------------------------------

