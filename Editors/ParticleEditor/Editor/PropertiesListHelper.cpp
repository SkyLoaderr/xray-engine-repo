#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListHelper.h"
#include "ItemListHelper.h"
#include "ImageThumbnail.h"
//---------------------------------------------------------------------------

CPropHelper PHelper;

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

void CPropHelper::DrawThumbnail(TCanvas *Surface, TRect &R, LPCSTR fname)
{
	if (fname&&fname[0]){
        EImageThumbnail* m_Thm 	= xr_new<EImageThumbnail>(fname,EImageThumbnail::EITTexture);
        int dw = R.Width()-R.Height();
        if (dw>=0) R.right		-= dw;
        if (m_Thm->Valid()) 	m_Thm->Draw(Surface,R,true);
        xr_delete				(m_Thm);
    }
}
//---------------------------------------------------------------------------

