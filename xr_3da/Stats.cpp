// Stats.cpp: implementation of the CStats class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Stats.h"
#include "hw.h"
#include "x_ray.h"
#include "xr_gamefont.h"
#include "xr_smallfont.h"

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
void CStats::Show() 
{
	// Stop timers
	{
		EngineTOTAL.FrameEnd		();	
		UpdateSheduled.FrameEnd		();	
		UpdateClient.FrameEnd		();	
		Physics.FrameEnd			();	
		Animation.FrameEnd			();	
		AI_Range.FrameEnd			();
		AI_Path.FrameEnd			();
		AI_Node.FrameEnd			();
		AI_Vis.FrameEnd				();
		AI_Vis_Query.FrameEnd		();
		AI_Vis_RayTests.FrameEnd	();
		
		RenderTOTAL.FrameEnd		();
		RenderCALC.FrameEnd			();
		RenderDUMP.FrameEnd			();	
		RenderDUMP_SKIN.FrameEnd	();	
		RenderDUMP_Cached.FrameEnd	();	
		RenderDUMP_HUD.FrameEnd		();	
		RenderDUMP_Glows.FrameEnd	();	
		RenderDUMP_Lights.FrameEnd	();	
		RenderDUMP_Shads.FrameEnd	();	
		RenderDUMP_WM.FrameEnd		();	
		RenderDUMP_DT_VIS.FrameEnd	();	
		RenderDUMP_DT_Render.FrameEnd();	
		RenderDUMP_DT_Cache.FrameEnd();
		
		Sound.FrameEnd				();
		Input.FrameEnd				();
		clRAY.FrameEnd				();	
		clBOX.FrameEnd				();

		netClient.FrameEnd			();
		netServer.FrameEnd			();
		
		TEST.FrameEnd				();
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
		CFontSmall&	F = *(pApp->pFont);
		F.Size		(0.016f		);
		F.Color		(0xFFFFFFFF	);
		F.OutSet	(-1.f,-.95f	);
		F.OutNext	("FPS/RFPS:    %3.1f/%3.1f",fFPS,fRFPS);
		F.OutNext	("TPS:         %2.2f M",fTPS);
		F.OutNext	("VERT:        %d/%d",dwVert,dwVert/dwCalls);
		F.OutNext	("POLY:        %d/%d",dwPoly,dwPoly/dwCalls);
		F.OutNext	("DIP/DP:      %d",dwCalls);
		F.OutNext	("SH Changes:  %d",dwShader_Changes);
		F.OutNext	("TEX Changes: %d",dwTexture_Changes);
		F.OutSkip	();
		F.OutNext	("*** ENGINE:  %2.2fms",EngineTOTAL.result);	
		F.OutNext	("uSheduled:   %2.2fms",UpdateSheduled.result);
		F.OutNext	("uClients:    %2.2fms",UpdateClient.result);
		F.OutNext	("Physics:     %2.2fms, %d",Physics.result,Physics.count);	
		F.OutNext	("aiRange:     %2.2fms, %d",AI_Range.result,AI_Range.count);
		F.OutNext	("aiPath:      %2.2fms, %d",AI_Path.result,AI_Path.count);
		F.OutNext	("aiNode:      %2.2fms, %d",AI_Node.result,AI_Node.count);
		F.OutNext	("aiVision:    %2.2fms, %d",AI_Vis.result,AI_Vis.count);
		F.OutNext	("  Query:     %2.2fms",	AI_Vis_Query.result);
		F.OutNext	("  RayCast:   %2.2fms",	AI_Vis_RayTests.result);
		F.OutSkip	();
		F.OutNext	("*** RENDER:  %2.2fms",RenderTOTAL.result);
		F.OutNext	("R_CALC:      %2.2fms",RenderCALC.result);	
		F.OutNext	("  Skeletons: %2.2fms, %d",Animation.result,Animation.count);
		F.OutNext	("R_DUMP:      %2.2fms",RenderDUMP.result);	
		F.OutNext	("  Skinning:  %2.2fms",RenderDUMP_SKIN.result);	
		F.OutNext	("  Cached:	   %2.2fms",RenderDUMP_Cached.result);	
		F.OutNext	("  DT_Vis:    %2.2fms",RenderDUMP_DT_VIS.result);	
		F.OutNext	("  DT_Render: %2.2fms",RenderDUMP_DT_Render.result);	
		F.OutNext	("  DT_Cache:  %2.2fms",RenderDUMP_DT_Cache.result);	
		F.OutNext	("  Shadows:   %2.2fms",RenderDUMP_Shads.result);
		F.OutNext	("  Wallmarks: %2.2fms",RenderDUMP_WM.result);
		F.OutNext	("  Glows:     %2.2fms",RenderDUMP_Glows.result);	
		F.OutNext	("  Lights:    %2.2fms, %d",RenderDUMP_Lights.result,RenderDUMP_Lights.count);
		F.OutNext	("  HUD:       %2.2fms",RenderDUMP_HUD.result);	
		F.OutSkip	();
		F.OutNext	("*** SOUND:   %2.2fms",Sound.result);
		F.OutNext	("  SP/SA:     %d/%d",  dwSND_Played,dwSND_Allocated);
		F.OutSkip	();
		F.OutNext	("Input:       %2.2fms",Input.result);
		F.OutNext	("clRAY:       %2.2fms, %d",clRAY.result,clRAY.count);
		F.OutNext	("clBOX:       %2.2fms, %d",clBOX.result,clBOX.count);
		F.OutSkip	();
		F.OutNext	("netClient:   %2.2fms, %d",netClient.result,netClient.count);
		F.OutNext	("netServer:   %2.2fms, %d",netServer.result,netServer.count);
		F.OutNext	("TEST:        %2.2fms, %d",TEST.result,TEST.count);
	}

	{
		EngineTOTAL.FrameStart		();	
		UpdateSheduled.FrameStart	();	
		UpdateClient.FrameStart		();	
		Physics.FrameStart			();	
		Animation.FrameStart		();	
		AI_Range.FrameStart			();
		AI_Path.FrameStart			();
		AI_Node.FrameStart			();
		AI_Vis.FrameStart			();
		AI_Vis_Query.FrameStart		();
		AI_Vis_RayTests.FrameStart	();
		
		RenderTOTAL.FrameStart		();
		RenderCALC.FrameStart		();
		RenderDUMP.FrameStart		();	
		RenderDUMP_SKIN.FrameStart	();	
		RenderDUMP_Cached.FrameStart();	
		RenderDUMP_HUD.FrameStart	();	
		RenderDUMP_Glows.FrameStart	();	
		RenderDUMP_Lights.FrameStart();	
		RenderDUMP_Shads.FrameStart	();	
		RenderDUMP_WM.FrameStart	();	
		RenderDUMP_DT_VIS.FrameStart();	
		RenderDUMP_DT_Render.FrameStart();	
		RenderDUMP_DT_Cache.FrameStart();	
		
		Sound.FrameStart			();
		Input.FrameStart			();
		clRAY.FrameStart			();	
		clBOX.FrameStart			();
		
		netClient.FrameStart		();
		netServer.FrameStart		();

		TEST.FrameStart				();
	}
	dwVert = dwPoly = dwCalls = dwShader_Changes = dwTexture_Changes = 0;
	dwSND_Played = dwSND_Allocated = 0;
}
