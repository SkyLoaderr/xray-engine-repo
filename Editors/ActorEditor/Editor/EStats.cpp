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
	dwLevelFaceCount	= 0;
	dwLevelVertexCount	= 0;
	dwLevelSelFaceCount	= 0;
	dwLevelSelVertexCount=0;
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

        RenderDUMP_SKIN.FrameEnd	();

		Animation.FrameEnd			();	
		Input.FrameEnd				();
		clRAY.FrameEnd				();
		clBOX.FrameEnd				();
        clFRUSTUM.FrameEnd			();

        RenderDUMP_RT.FrameEnd		();

		RenderDUMP_DT_VIS.FrameEnd	();	
		RenderDUMP_DT_Render.FrameEnd();	
		RenderDUMP_DT_Cache.FrameEnd();
	}

	// calc FPS & TPS
	CDraw&	DPS = Device.Primitive;
	if (Device.fTimeDelta>EPS_S) {
		float fps  = 1.f/Device.fTimeDelta;
		float fOne = 0.3f;
		float fInv = 1.f-fOne;
		fFPS = fInv*fFPS + fOne*fps;

		if (RenderTOTAL.result>EPS_S) {
			fTPS = fInv*fTPS + fOne*float(DPS.stat_polys)/(RenderTOTAL.result*1000.f);
			fRFPS= fInv*fRFPS+ fOne*1000.f/RenderTOTAL.result;
		}
	}

	// Show them
	if (psDeviceFlags & rsStatistic)
	{
	    CFontBase& 	F = *font;
		F.Color		(0xFFFFFFFF	);
		F.OutSet	(5,5);
		F.OutNext	("FPS/RFPS:     %3.1f/%3.1f",	fFPS,fRFPS);
		F.OutNext	("TPS:          %2.2f M",		fTPS);
		F.OutNext	("VERT:         %d",				DPS.stat_verts);
		F.OutNext	("POLY:         %d",				DPS.stat_polys);
		F.OutNext	("DIP/DP:       %d",				DPS.stat_calls);
		F.OutNext	("SH/T/M/C:     %d/%d/%d/%d",	dwShader_Codes,dwShader_Textures,dwShader_Matrices,dwShader_Constants);
		F.OutNext	("LIGHT S/T:    %d/%d",			dwLightInScene,dwTotalLight);
		F.OutNext	("Skeletons:    %2.2fms, %d",	Animation.result,Animation.count);
		F.OutNext	("Skinning:     %2.2fms",		RenderDUMP_SKIN.result);
		F.OutSkip	();
        F.OutNext	("Render:       %2.2fms",		RenderDUMP_RT.result);
		F.OutNext	("Input:        %2.2fms",		Input.result);
		F.OutNext	("clRAY:        %2.2fms, %d",	clRAY.result,clRAY.count);
		F.OutNext	("clBOX:        %2.2fms, %d",	clBOX.result,clBOX.count);
        F.OutNext	("clFRUSTUM:    %2.2fms, %d",	clFRUSTUM.result,clFRUSTUM.count);
		F.OutSkip	();
		F.OutSkip	();
        F.OutNext	("Level summary:");
        F.OutNext	(" Total Faces: %d",				dwLevelFaceCount);
        F.OutNext	(" Total Verts: %d",				dwLevelVertexCount);
        F.OutNext	(" Sel Faces:   %d",				dwLevelSelFaceCount);
        F.OutNext	(" Sel Verts:   %d",				dwLevelSelVertexCount);
	}

	{
		Animation.FrameStart		();	
		RenderTOTAL.FrameStart		();

		Input.FrameStart			();
		clRAY.FrameStart			();
		clBOX.FrameStart			();
		clFRUSTUM.FrameStart		();

		RenderDUMP_SKIN.FrameStart	();
		RenderDUMP_RT.FrameStart	();

		RenderDUMP_DT_VIS.FrameStart();	
		RenderDUMP_DT_Render.FrameStart();	
		RenderDUMP_DT_Cache.FrameStart();	
	}
	dwShader_Codes = dwShader_Textures = dwShader_Matrices = dwShader_Constants = 0;
	dwSND_Played = dwSND_Allocated = 0;
    dwTotalLight = dwLightInScene = 0;

	DPS.stat_polys	= 0;
	DPS.stat_verts	= 0;
	DPS.stat_calls	= 0;
	DPS.stat_vs		= 0;
	DPS.stat_vb		= 0;
	DPS.stat_ib		= 0;

	dwLevelFaceCount		= 0;
	dwLevelVertexCount		= 0;
	dwLevelSelFaceCount		= 0;
	dwLevelSelVertexCount	= 0;
}
