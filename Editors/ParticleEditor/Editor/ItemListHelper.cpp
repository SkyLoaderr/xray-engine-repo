#include "stdafx.h"
#pragma hdrstop

#include "ItemListHelper.h"
#include "ImageThumbnail.h"
//---------------------------------------------------------------------------

CListHelper LHelper;
AnsiString CListHelper::XKey;

void CListHelper::DrawThumbnail(TCanvas *Surface, TRect &R, LPCSTR fname)
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

