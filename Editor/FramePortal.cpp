#include "stdafx.h"
#pragma hdrstop

#include "FramePortal.h"
#include "sceneobject.h"
#include "scene.h"
#include "portal.h"
#include "propertiessector.h"
#include "ui_tools.h"
#include "sector.h"
#include "scene.h"
#include "portalutils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "RXCtrls"
#pragma link "RxMenus"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraPortal::TfraPortal(TComponent* Owner)
        : TFrame(Owner)
{
	iPickSectors=0;
	ebPickSectors->Down=false;
    OnChange();
}
//---------------------------------------------------------------------------
void __fastcall TfraPortal::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfraPortal::TopClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfraPortal::FindAndSelectPortalInList(const char* _name){
    cbItems->ItemIndex=cbItems->Items->IndexOf(AnsiString(_name));
    cbItemsChange(0);
}
//---------------------------------------------------------------------------
void __fastcall TfraPortal::OnChange(){
	AnsiString cur="";
    cbItems->Items->Clear();
    ObjectIt _F = Scene->FirstObj(OBJCLASS_PORTAL);
    ObjectIt _E = Scene->LastObj(OBJCLASS_PORTAL);
    for(;_F!=_E;_F++){
    	cbItems->Items->AddObject((*_F)->GetName(),(TObject*)(*_F));
        if ((*_F)->Selected()&&(*_F)->Visible())
        	if(!cur.Length()) cur=(*_F)->GetName();
            else cur="-";
    }
    FindAndSelectPortalInList(cur.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ebCreateNewClick(TObject *Sender)
{
	char namebuffer[MAX_OBJ_NAME];
	Scene->GenObjectName( OBJCLASS_PORTAL, namebuffer );
	CPortal* _O = new CPortal(namebuffer);
	Scene->SelectObjects(false,OBJCLASS_PORTAL);
	Scene->AddObject( _O );
    FindAndSelectPortalInList(namebuffer);
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::cbItemsChange(TObject *Sender)
{
	if (cbItems->ItemIndex>-1){
    	CPortal* _O=(CPortal*)cbItems->Items->Objects[cbItems->ItemIndex];
		Scene->SelectObjects(false,OBJCLASS_PORTAL);
        _O->Select(true);
	    lbFrontSector->Caption 	= _O->m_SectorFront->GetName();
    	lbBackSector->Caption 	= _O->m_SectorBack->GetName();
    }else{
	    lbFrontSector->Caption 	= "...";
    	lbBackSector->Caption 	= "...";
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::SetSector(CSector* obj){
	if(iPickSectors==0){
    	cbSectorFront->ItemIndex=cbSectorFront->Items->IndexOf(obj->GetName());
		iPickSectors++;
        cbSectorChange(NULL);
    }else{
    	cbSectorBack->ItemIndex=cbSectorBack->Items->IndexOf(obj->GetName());
    	if (cbSectorBack->ItemIndex!=cbSectorFront->ItemIndex){
			iPickSectors++;
            ebPickSectors->Down=false;
	        cbSectorChange(NULL);
        }else{
        	cbSectorBack->ItemIndex=-1;
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TfraPortal::ebComputeAllPortalsClick(TObject *Sender)
{
	int cnt=PortalUtils.CalculateAllPortals();
	if (cnt) Log->DlgMsg(mtInformation,"Calculated '%d' portal(s).",cnt);
    ShowCompute(true);
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ebComputeClick(TObject *Sender)
{
	PortalUtils.CalculatePortals(cbSectorFront->Items->Strings[cbSectorFront->ItemIndex].c_str(),
    							 cbSectorBack->Items->Strings[cbSectorBack->ItemIndex].c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ShowCompute(bool bVisible){
	if (bVisible){
    	paCompute->Show();
//		AnsiString curF=cbSectorFront->Items->Strings[cbSectorFront->ItemIndex];
//		AnsiString curB=cbSectorBack->Items->Strings[cbSectorBack->ItemIndex];
        cbSectorFront->Items->Clear();
        cbSectorBack->Items->Clear();
        ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
        ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
        for(;_F!=_E;_F++){
            cbSectorFront->Items->AddObject((*_F)->GetName(),(TObject*)(*_F));
            cbSectorBack->Items->AddObject((*_F)->GetName(),(TObject*)(*_F));
            if ((*_F)->Selected()&&(cbSectorFront->ItemIndex==-1)) cbSectorFront->ItemIndex=cbSectorFront->Items->Count-1;
            else if ((*_F)->Selected()&&(cbSectorBack->ItemIndex==-1)) cbSectorBack->ItemIndex=cbSectorBack->Items->Count-1;
        }
//        cbSectorFront->ItemIndex=cbSectorFront->Items->IndexOf(curF);
//        cbSectorBack->ItemIndex	=cbSectorBack->Items->IndexOf(curB);
        cbSectorChange(NULL);
    }else{
    	paCompute->Hide();
    }
}

void __fastcall TfraPortal::cbSectorChange(TObject *Sender)
{
    if ((cbSectorFront->ItemIndex>-1)&&(cbSectorBack->ItemIndex>-1)&&(cbSectorFront->Items->Strings[cbSectorFront->ItemIndex]!=cbSectorBack->Items->Strings[cbSectorBack->ItemIndex]))
        	ebCompute->Enabled=true;
	else 	ebCompute->Enabled=false;
	Scene->SelectObjects(false,OBJCLASS_SECTOR);
	if (cbSectorFront->ItemIndex>-1){
    	CSector* _O=(CSector*)cbSectorFront->Items->Objects[cbSectorFront->ItemIndex];
        _O->Select(true);
    }
	if (cbSectorBack->ItemIndex>-1){
    	CSector* _O=(CSector*)cbSectorBack->Items->Objects[cbSectorBack->ItemIndex];
        _O->Select(true);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ebPickSectorsClick(TObject *Sender)
{
	iPickSectors=0;
	cbSectorFront->ItemIndex=-1;
	cbSectorBack->ItemIndex=-1;
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ExtBtn3Click(TObject *Sender)
{
	CPortal* _O=(CPortal*)cbItems->Items->Objects[cbItems->ItemIndex];
    if (_O) _O->InvertOrientation();
}
//---------------------------------------------------------------------------

