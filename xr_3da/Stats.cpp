#include "stdafx.h"
#include "xr_hudfont.h"
#pragma hdrstop

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
CStats::CStats()
{
	fFPS		= 30.f;
	fRFPS		= 30.f;
	fTPS		= 0;
	font		= 0;
}

CStats::~CStats()
{
	_DELETE		(font);
}

#define QS if(psDeviceFlags&rsStatistic)
void CStats::Show() 
{
	// Stop timers
	{
		EngineTOTAL.FrameEnd		();	
		Sheduler.FrameEnd			();	
		UpdateClient.FrameEnd		();	
		Physics.FrameEnd			();	
		Animation.FrameEnd			();	
		AI_Think.FrameEnd			();
		AI_Range.FrameEnd			();
		AI_Path.FrameEnd			();
		AI_Node.FrameEnd			();
		AI_Vis.FrameEnd				();
		AI_Vis_Query.FrameEnd		();
		AI_Vis_RayTests.FrameEnd	();
		
		RenderTOTAL.FrameEnd		();
		RenderCALC.FrameEnd			();
		RenderCALC_HOM.FrameEnd		();
		RenderDUMP.FrameEnd			();	
		RenderDUMP_RT.FrameEnd		();
		RenderDUMP_SKIN.FrameEnd	();	
		RenderDUMP_Cached.FrameEnd	();	
		RenderDUMP_HUD.FrameEnd		();	
		RenderDUMP_Glows.FrameEnd	();	
		RenderDUMP_Lights.FrameEnd	();	
		RenderDUMP_WM.FrameEnd		();	
		RenderDUMP_DT_VIS.FrameEnd	();	
		RenderDUMP_DT_Render.FrameEnd();	
		RenderDUMP_DT_Cache.FrameEnd();
		RenderDUMP_Pcalc.FrameEnd	();	
		RenderDUMP_Scalc.FrameEnd	();	
		RenderDUMP_Srender.FrameEnd	();	
		
		Sound.FrameEnd				();
		Input.FrameEnd				();
		clRAY.FrameEnd				();	
		clBOX.FrameEnd				();
		clFRUSTUM.FrameEnd			();
		
		netClient.FrameEnd			();
		netServer.FrameEnd			();
		
		TEST0.FrameEnd				();
		TEST1.FrameEnd				();
		TEST2.FrameEnd				();
		TEST3.FrameEnd				();
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
		static float	r_ps		= 0;
		static float	b_ps		= 0;
		r_ps						= .99f*r_ps + .01f*(clRAY.count/clRAY.result);
		b_ps						= .99f*b_ps + .01f*(clBOX.count/clBOX.result);
		
		CFontHUD&	F = *((CFontHUD*)font);
		F.Color		(0xFFFFFFFF	);
		F.OutSet	(0,0);
		F.OutNext	("FPS/RFPS:    %3.1f/%3.1f",fFPS,fRFPS);
		F.OutNext	("TPS:         %2.2f M",fTPS);
		F.OutNext	("VERT:        %d/%d",dwVert,dwVert/dwCalls);
		F.OutNext	("POLY:        %d/%d",dwPoly,dwPoly/dwCalls);
		F.OutNext	("DIP/DP:      %d",dwCalls);
		F.OutNext	("SH/T/M/C:    %d/%d/%d/%d",dwShader_Codes,dwShader_Textures,dwShader_Matrices,dwShader_Constants);
		F.OutNext	("xforms:      %d",dwXFORMs);
		F.OutSkip	();
		F.OutNext	("*** ENGINE:  %2.2fms",EngineTOTAL.result);	
		F.OutNext	("uSheduled:   %2.2fms",Sheduler.result);
		F.OutNext	("uClients:    %2.2fms",UpdateClient.result);
		F.OutNext	("Physics:     %2.2fms, %d",Physics.result,Physics.count);	
		F.OutNext	("aiThink:     %2.2fms, %d",AI_Think.result,AI_Think.count);	
		F.OutNext	("  aiRange:   %2.2fms, %d",AI_Range.result,AI_Range.count);
		F.OutNext	("  aiPath:    %2.2fms, %d",AI_Path.result,AI_Path.count);
		F.OutNext	("  aiNode:    %2.2fms, %d",AI_Node.result,AI_Node.count);
		F.OutNext	("aiVision:    %2.2fms, %d",AI_Vis.result,AI_Vis.count);
		F.OutNext	("  Query:     %2.2fms",	AI_Vis_Query.result);
		F.OutNext	("  RayCast:   %2.2fms",	AI_Vis_RayTests.result);
		F.OutSkip	();
		F.OutNext	("*** RENDER:  %2.2fms",RenderTOTAL.result);
		F.OutNext	("R_CALC:      %2.2fms",RenderCALC.result);	
		F.OutNext	("  HOM:       %2.2fms",RenderCALC_HOM.result);
		F.OutNext	("  Skeletons: %2.2fms, %d",Animation.result,Animation.count);
		F.OutNext	("R_DUMP:      %2.2fms",RenderDUMP.result);	
		F.OutNext	("  Skinning:  %2.2fms",RenderDUMP_SKIN.result);	
		F.OutNext	("  Cached:	   %2.2fms",RenderDUMP_Cached.result);	
		F.OutNext	("  DT_Vis:    %2.2fms",RenderDUMP_DT_VIS.result);	
		F.OutNext	("  DT_Render: %2.2fms",RenderDUMP_DT_Render.result);	
		F.OutNext	("  DT_Cache:  %2.2fms",RenderDUMP_DT_Cache.result);	
		F.OutNext	("  Wallmarks: %2.2fms",RenderDUMP_WM.result);
		F.OutNext	("  Glows:     %2.2fms",RenderDUMP_Glows.result);	
		F.OutNext	("  Lights:    %2.2fms, %d",RenderDUMP_Lights.result,RenderDUMP_Lights.count);
		F.OutNext	("  RT:        %2.2fms, %d",RenderDUMP_RT.result,RenderDUMP_RT.count);
		F.OutNext	("  HUD:       %2.2fms",RenderDUMP_HUD.result);	
		F.OutNext	("  P_calc:    %2.2fms",RenderDUMP_Pcalc.result);
		F.OutNext	("  S_calc:    %2.2fms",RenderDUMP_Scalc.result);
		F.OutNext	("  S_render:  %2.2fms",RenderDUMP_Srender.result);
		F.OutSkip	();
		F.OutNext	("*** SOUND:   %2.2fms",Sound.result);
		F.OutNext	("  SP/SA:     %d/%d",  dwSND_Played,dwSND_Allocated);
		F.OutSkip	();
		F.OutNext	("Input:       %2.2fms",Input.result);
		F.OutNext	("clRAY:       %2.2fms, %d, %2.0fK",clRAY.result,clRAY.count,r_ps);
		F.OutNext	("clBOX:       %2.2fms, %d, %2.0fK",clBOX.result,clBOX.count,b_ps);
		F.OutNext	("clFRUSTUM:   %2.2fms, %d",clFRUSTUM.result,clFRUSTUM.count);
		F.OutSkip	();
		F.OutNext	("netClient:   %2.2fms, %d",netClient.result,netClient.count);
		F.OutNext	("netServer:   %2.2fms, %d",netServer.result,netServer.count);
		F.OutSkip	();
		F.OutNext	("TEST 0:      %2.2fms, %d",TEST0.result,TEST0.count);
		F.OutNext	("TEST 1:      %2.2fms, %d",TEST1.result,TEST1.count);
		F.OutNext	("TEST 2:      %2.2fms, %d",TEST2.result,TEST2.count);
		F.OutNext	("TEST 3:      %2.2fms, %d",TEST3.result,TEST3.count);
		F.OnRender	();
	}

	{
		EngineTOTAL.FrameStart		();	
		Sheduler.FrameStart			();	
		UpdateClient.FrameStart		();	
		Physics.FrameStart			();	
		Animation.FrameStart		();	
		AI_Think.FrameStart			();
		AI_Range.FrameStart			();
		AI_Path.FrameStart			();
		AI_Node.FrameStart			();
		AI_Vis.FrameStart			();
		AI_Vis_Query.FrameStart		();
		AI_Vis_RayTests.FrameStart	();
		
		RenderTOTAL.FrameStart		();
		RenderCALC.FrameStart		();
		RenderCALC_HOM.FrameStart	();
		RenderDUMP.FrameStart		();	
		RenderDUMP_RT.FrameStart	();
		RenderDUMP_SKIN.FrameStart	();	
		RenderDUMP_Cached.FrameStart();	
		RenderDUMP_HUD.FrameStart	();	
		RenderDUMP_Glows.FrameStart	();	
		RenderDUMP_Lights.FrameStart();	
		RenderDUMP_WM.FrameStart	();	
		RenderDUMP_DT_VIS.FrameStart();	
		RenderDUMP_DT_Render.FrameStart();	
		RenderDUMP_DT_Cache.FrameStart();	
		RenderDUMP_Pcalc.FrameStart	();	
		RenderDUMP_Scalc.FrameStart	();	
		RenderDUMP_Srender.FrameStart();	
		
		Sound.FrameStart			();
		Input.FrameStart			();
		clRAY.FrameStart			();	
		clBOX.FrameStart			();
		clFRUSTUM.FrameStart		();
		
		netClient.FrameStart		();
		netServer.FrameStart		();

		TEST0.FrameStart			();
		TEST1.FrameStart			();
		TEST2.FrameStart			();
		TEST3.FrameStart			();
	}
	dwVert = dwPoly = dwCalls = 0;
	dwSND_Played = dwSND_Allocated = 0;
	dwShader_Codes = dwShader_Textures = dwShader_Matrices = dwShader_Constants = 0;
	dwXFORMs = 0;
}

void CStats::OnDeviceCreate			()
{
	font	= new CFontHUD			();
}
void CStats::OnDeviceDestroy		()
{
	_DELETE	(font);
}
