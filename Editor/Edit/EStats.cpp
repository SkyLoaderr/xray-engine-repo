#include "stdafx.h"
#pragma hdrstop

#include "Stats.h"
#include "hw.h"
#include "xr_customfont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStats::CStats()
{
	fFPS		= 30.f;
	fRFPS		= 30.f;
	fTPS		= 0;
}

CStats::~CStats()
{

}

#define QS if(psDeviceFlags&rsStatistic)
void CStats::Show(CFontBase* font)
{
	// Stop timers
	{
		RenderTOTAL.FrameEnd		();
		RenderCALC.FrameEnd			();

		Input.FrameEnd				();
		clRAY.FrameEnd				();
		clBOX.FrameEnd				();
	}

	// calc FPS & TPS
	if (Device.fTimeDelta>EPS_S) {
		float fps  = 1.f/Device.fTimeDelta;
		float fOne = 0.3f;
		float fInv = 1.f-fOne;
		fFPS = fInv*fFPS + fOne*fps;

		if (RenderTOTAL.result>EPS_S) {
			fTPS = fInv*fTPS + fOne*float(dwPoly)/(RenderTOTAL.result*1000.f);
			fRFPS= fInv*fRFPS+ fOne*1000.f/RenderTOTAL.result;
		}
	}

	// Show them
	if (psDeviceFlags & rsStatistic) 
	{
	    CFontBase& 	F = *font;
		F.Color		(0xFFFFFFFF	);
		F.OutSet	(5,5);
		F.OutNext	("FPS/RFPS:    %3.1f/%3.1f",fFPS,fRFPS);
		F.OutNext	("TPS:         %2.2f M",fTPS);
		F.OutNext	("VERT:        %d/%d",dwVert,dwCalls?dwVert/dwCalls:0);
		F.OutNext	("POLY:        %d/%d",dwPoly,dwCalls?dwPoly/dwCalls:0);
		F.OutNext	("DIP/DP:      %d",dwCalls);
		F.OutNext	("SH Changes:  %d",dwShader_Changes);
		F.OutNext	("TEX Changes: %d",dwTexture_Changes);
		F.OutNext	("LIGHT S/T:   %d/%d",dwLightInScene,dwTotalLight);
		F.OutSkip	();
		F.OutNext	("Input:       %2.2fms",Input.result);
		F.OutNext	("clRAY:       %2.2fms, %d",clRAY.result,clRAY.count);
		F.OutNext	("clBOX:       %2.2fms, %d",clBOX.result,clBOX.count);
		F.OutSkip	();
	}

	{
		RenderTOTAL.FrameStart		();
		
		Input.FrameStart			();
		clRAY.FrameStart			();
		clBOX.FrameStart			();
	}
	dwVert = dwPoly = dwCalls = dwShader_Changes = dwTexture_Changes = 0;
	dwSND_Played = dwSND_Allocated = 0;
    dwTotalLight = dwLightInScene = 0;
}
