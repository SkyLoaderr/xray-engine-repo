#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_main.h"
#include "FrameRPoint.h"
#include "Scene.h"
#include "ChoseForm.h"
#include "SpawnPoint.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraSpawnPoint::TfraSpawnPoint(TComponent* Owner)
        : TFrame(Owner)
{
    DEFINE_INI(fsStorage);
    m_Current = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfraSpawnPoint::OnEnter()
{
    m_Items 				= TItemList::CreateForm(paItems, alClient, 0);
    m_Items->OnItemsFocused	= OnItemFocused;
    m_Items->LoadSelection	(fsStorage);
    ListItemsVec items;
    LHelper.CreateItem		(items,RPOINT_CHOOSE_NAME,0,0,RPOINT_CHOOSE_NAME);
    CInifile::Root& data 	= pSettings->sections();
    for (CInifile::RootIt it=data.begin(); it!=data.end(); it++){
    	LPCSTR val;
    	if (it->line_exist	("$spawn",&val))
	    	LHelper.CreateItem(items,val,0,0,it->Name);
    }
    m_Items->AssignItems	(items,false,"Spawns",true);
	fsStorage->RestoreFormPlacement();
}
//---------------------------------------------------------------------------
void __fastcall TfraSpawnPoint::OnExit()
{
    m_Items->SaveSelection	(fsStorage);
	fsStorage->SaveFormPlacement();
    TItemList::DestroyForm	(m_Items);
}
//---------------------------------------------------------------------------
void __fastcall TfraSpawnPoint::OnItemFocused(ListItemsVec& items)
{
	VERIFY(items.size()<=1);
    m_Current 			= 0;
    for (ListItemsIt it=items.begin(); it!=items.end(); it++)
        m_Current 		= (LPCSTR)(*it)->m_Object;
    UI.Command			(COMMAND_RENDER_FOCUS);
}
//------------------------------------------------------------------------------
void __fastcall TfraSpawnPoint::PaneMinClick(TObject *Sender)
{
    PanelMinMaxClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawnPoint::ExpandClick(TObject *Sender)
{
    PanelMaximizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawnPoint::ebAttachObjectClick(TObject *Sender)
{
	if (ebAttachObject->Down) UI.Command(COMMAND_CHANGE_ACTION, eaAdd);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawnPoint::evDetachObjectClick(TObject *Sender)
{
    ObjectList lst;
    if (Scene.GetQueryObjects(lst,OBJCLASS_SPAWNPOINT,1,1,0)){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
	        CSpawnPoint* O = dynamic_cast<CSpawnPoint*>(*it); R_ASSERT(O);
        	O->DetachObject();
        }
    }
}
//---------------------------------------------------------------------------

