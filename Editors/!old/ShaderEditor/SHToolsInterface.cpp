//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHToolsInterface.h"
#include "FolderLib.h"
#include "ui_main.h"

ISHTools::ISHTools(ISHInit& init)
{
	m_bModified			= FALSE;
    m_bLockUpdate		= FALSE;
    Ext					= init;
}

void ISHTools::ViewAddItem(LPCSTR full_name)
{
	FHelper.AppendObject(Ext.tvView,full_name);
}
//---------------------------------------------------------------------------

void ISHTools::ViewSetCurrentItem(LPCSTR full_name)
{
	FHelper.RestoreSelection(Ext.tvView,full_name,false);
}
//---------------------------------------------------------------------------

void ISHTools::ViewClearItemList()
{
    Ext.tvView->Items->Clear();
}
//---------------------------------------------------------------------------

void ISHTools::Modified()
{
	m_bModified=TRUE;
	UI.Command(COMMAND_UPDATE_CAPTION);
    ApplyChanges();
}
//---------------------------------------------------------------------------

bool ISHTools::IfModified()
{
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The '%s' has been modified.\nDo you want to save your changes?",ToolsName());
        switch(mr){
        case mrYes: Save(); m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
        case mrCancel: return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------

void ISHTools::ZoomObject(bool bOnlySel)
{
    Fbox BB;
    BB.set(-5,-5,-5,5,5,5);
    Device.m_Camera.ZoomExtents(BB);
}
//---------------------------------------------------------------------------

