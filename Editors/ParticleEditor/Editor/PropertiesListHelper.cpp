#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListHelper.h"
#include "ItemListHelper.h"
#include "ImageThumbnail.h"
//---------------------------------------------------------------------------

CPropHelper PHelper;

void CPropHelper::NameAfterEdit_TI(PropItem* sender, LPVOID edit_val)
{
	FHelper.NameAfterEdit((TElTreeItem*)sender->tag,((TextValue*)sender->GetFrontValue())->GetValue(),*(AnsiString*)edit_val);
}
//---------------------------------------------------------------------------
void CPropHelper::NameAfterEdit(PropItem* sender, LPVOID edit_val)
{
	LHelper.NameAfterEdit((ListItem*)sender->tag,((TextValue*)sender->GetFrontValue())->GetValue(),*(AnsiString*)edit_val);
}
//---------------------------------------------------------------------------

void CPropHelper::NameBeforeEdit(PropItem* sender, LPVOID edit_val)
{
	AnsiString& N = *(AnsiString*)edit_val;
	int cnt=_GetItemCount(N.c_str(),'\\');
	N = _SetPos(N.c_str(),cnt-1,'\\');
}
//------------------------------------------------------------------------------
void CPropHelper::NameDraw(PropValue* sender, LPVOID draw_val)
{
	AnsiString& N = *(AnsiString*)draw_val;
	int cnt=_GetItemCount(N.c_str(),'\\');
	N = _SetPos(N.c_str(),cnt-1,'\\');
}
//------------------------------------------------------------------------------


