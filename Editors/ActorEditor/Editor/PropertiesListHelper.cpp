#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListHelper.h"
#include "ImageThumbnail.h"
//---------------------------------------------------------------------------

CPropHelper PHelper;
AnsiString CPropHelper::XKey;

void CPropHelper::DrawThumbnail(TCanvas *Surface, TRect &R, LPCSTR fname)
{
	if (fname&&fname[0]){
        EImageThumbnail* m_Thm 	= xr_new<EImageThumbnail>(fname,EImageThumbnail::EITTexture);
        if (m_Thm->Valid()) 	m_Thm->Draw(Surface,R,true);
        xr_delete				(m_Thm);
    }
}
//---------------------------------------------------------------------------

