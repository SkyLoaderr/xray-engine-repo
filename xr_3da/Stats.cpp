#include "stdafx.h"
#include "GameFont.h"
#pragma hdrstop

#include "ISpatial.h"
#include "IGame_Persistent.h"
#include "render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStats::CStats()
{
	fFPS		= 30.f;
	fRFPS		= 30.f;
	fTPS		= 0;
	pFont		= 0;
	fMem_calls	= 0;
}

CStats::~CStats()
{
	xr_delete		(pFont);
}

#define QS if(psDeviceFlags&rsStatistic)
void CStats::Show() 
{
	// Stop timers
	{
		EngineTOTAL.FrameEnd		();	
		Sheduler.FrameEnd			();	
		UpdateClient.FrameEnd		();	
		Scripting.FrameEnd			();	
		Physics.FrameEnd			();	
		ph_collision.FrameEnd		();
		ph_core.FrameEnd			();
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

		g_SpatialSpace->stat_insert.FrameEnd();
		g_SpatialSpace->stat_remove.FrameEnd();
	}

	// calc FPS & TPS
	if (Device.fTimeDelta>EPS_S) {
		float fps  = 1.f/Device.fTimeDelta;
		float fOne = 0.3f;
		float fInv = 1.f-fOne;
		fFPS = fInv*fFPS + fOne*fps;

		if (RenderTOTAL.result>EPS_S) {
			fTPS = fInv*fTPS + fOne*float(RCache.stat.polys)/(RenderTOTAL.result*1000.f);
			fRFPS= fInv*fRFPS+ fOne*1000.f/RenderTOTAL.result;
		}
	}
	{
		float mem_count	=	float	(Memory.stat_calls - dwMem_calls);
		dwMem_calls		=	Memory.stat_calls;
		if (mem_count>fMem_calls)	fMem_calls	=	mem_count;
		else						fMem_calls	=	.9f*fMem_calls + .1f*mem_count;
	}

	div_t ddd = div(Device.dwFrame,2000);
	if( ddd.rem < 1000 ){
		pFont->SetColor	(0xFFFFFFFF	);
		pFont->OutSet	(0,0);
		pFont->OutNext	(*eval_line_1);
		pFont->OutNext	(*eval_line_2);
		pFont->OutNext	(*eval_line_3);
		pFont->OnRender	();
	}

	// Show them
	if (psDeviceFlags.test(rsStatistic))
	{
		static float	r_ps		= 0;
		static float	b_ps		= 0;
		r_ps						= .99f*r_ps + .01f*(clRAY.count/clRAY.result);
		b_ps						= .99f*b_ps + .01f*(clBOX.count/clBOX.result);

		CSound_stats				snd;
		::Sound->statistic			(snd);
		
		CGameFont&	F = *((CGameFont*)pFont);
		float			f_base_size	= F.GetSize();

		F.SetColor	(0xFFFFFFFF	);
		F.OutSet	(0,0);
		F.OutNext	("FPS/RFPS:    %3.1f/%3.1f",fFPS,fRFPS);
		F.OutNext	("TPS:         %2.2f M",	fTPS);
		F.OutNext	("VERT:        %d/%d",		RCache.stat.verts,RCache.stat.calls?RCache.stat.verts/RCache.stat.calls:0);
		F.OutNext	("POLY:        %d/%d",		RCache.stat.polys,RCache.stat.calls?RCache.stat.polys/RCache.stat.calls:0);
		F.OutNext	("DIP/DP:      %d",			RCache.stat.calls);
		F.OutNext	("SH/T/M/C:    %d/%d/%d/%d",RCache.stat.states,RCache.stat.textures,RCache.stat.matrices,RCache.stat.constants);
		F.OutNext	("RT/PS/VS:    %d/%d/%d",	RCache.stat.target_rt,RCache.stat.ps,RCache.stat.vs);
		F.OutNext	("DCL/VB/IB:   %d/%d/%d",   RCache.stat.decl,RCache.stat.vb,RCache.stat.ib);
		F.OutNext	("xforms:      %d",			RCache.stat.xforms);
		F.OutSkip	();

#define PPP(a) (100.f*float(a)/float(EngineTOTAL.result))
		F.OutNext	("*** ENGINE:  %2.2fms",EngineTOTAL.result);	
		F.OutNext	("Memory:      %2.2fa",fMem_calls);
		F.OutNext	("uSheduled:   %2.2fms, %2.1f%%",Sheduler.result,		PPP(Sheduler.result));
		F.OutNext	("uClients:    %2.2fms, %2.1f%%",UpdateClient.result,	PPP(UpdateClient.result));
		F.OutNext	("uScripts:    %2.2fms, %d",Scripting.result,Scripting.count);
		F.OutNext	("spInsert:    %2.2fms, %2.1f%%",g_SpatialSpace->stat_insert.result, PPP(g_SpatialSpace->stat_insert.result));
		F.OutNext	("spRemove:    %2.2fms, %2.1f%%",g_SpatialSpace->stat_remove.result, PPP(g_SpatialSpace->stat_remove.result));
		F.OutNext	("Physics:     %2.2fms, %2.1f%%",Physics.result,		PPP(Physics.result));	
		F.OutNext	("  collider:  %2.2fms", ph_collision.result);	
		F.OutNext	("  solver:    %2.2fms, %d",ph_core.result,ph_core.count);	
		F.OutNext	("aiThink:     %2.2fms, %d",AI_Think.result,AI_Think.count);	
		F.OutNext	("  aiRange:   %2.2fms, %d",AI_Range.result,AI_Range.count);
		F.OutNext	("  aiPath:    %2.2fms, %d",AI_Path.result,AI_Path.count);
		F.OutNext	("  aiNode:    %2.2fms, %d",AI_Node.result,AI_Node.count);
		F.OutNext	("aiVision:    %2.2fms, %d",AI_Vis.result,AI_Vis.count);
		F.OutNext	("  Query:     %2.2fms",	AI_Vis_Query.result);
		F.OutNext	("  RayCast:   %2.2fms",	AI_Vis_RayTests.result);
		F.OutSkip	();
								   
#undef  PPP
#define PPP(a) (100.f*float(a)/float(RenderTOTAL.result))
		F.OutNext	("*** RENDER:  %2.2fms",RenderTOTAL.result);
		F.OutNext	("R_CALC:      %2.2fms, %2.1f%%",RenderCALC.result,	PPP(RenderCALC.result));	
		F.OutNext	("  HOM:       %2.2fms, %d",RenderCALC_HOM.result,	RenderCALC_HOM.count);
		F.OutNext	("  Skeletons: %2.2fms, %d",Animation.result,		Animation.count);
		F.OutNext	("R_DUMP:      %2.2fms, %2.1f%%",RenderDUMP.result,	PPP(RenderDUMP.result));	
		F.OutNext	("  Skinning:  %2.2fms",RenderDUMP_SKIN.result);	
		F.OutNext	("  Cached:	   %2.2fms",RenderDUMP_Cached.result);	
		F.OutNext	("  DT_Vis/Cnt:%2.2fms",RenderDUMP_DT_VIS.result,RenderDUMP_DT_Count);	
		F.OutNext	("  DT_Render: %2.2fms",RenderDUMP_DT_Render.result);	
		F.OutNext	("  DT_Cache:  %2.2fms",RenderDUMP_DT_Cache.result);	
		F.OutNext	("  Wallmarks: %2.2fms, %d/%d - %d",RenderDUMP_WM.result,RenderDUMP_WMS_Count,RenderDUMP_WMD_Count,RenderDUMP_WMT_Count);
		F.OutNext	("  Glows:     %2.2fms",RenderDUMP_Glows.result);	
		F.OutNext	("  Lights:    %2.2fms, %d",RenderDUMP_Lights.result,RenderDUMP_Lights.count);
		F.OutNext	("  RT:        %2.2fms, %d",RenderDUMP_RT.result,RenderDUMP_RT.count);
		F.OutNext	("  HUD:       %2.2fms",RenderDUMP_HUD.result);	
		F.OutNext	("  P_calc:    %2.2fms",RenderDUMP_Pcalc.result);
		F.OutNext	("  S_calc:    %2.2fms",RenderDUMP_Scalc.result);
		F.OutNext	("  S_render:  %2.2fms, %d",RenderDUMP_Srender.result,RenderDUMP_Srender.count);
		F.OutSkip	();
		F.OutNext	("*** SOUND:   %2.2fms",Sound.result);
		F.OutNext	("  REND/SIM:  %d/%d",  snd._rendered, snd._simulated);
		F.OutNext	("  HIT/MISS:  %d/%d",  snd._cache_hits, snd._cache_misses);
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
#ifdef	DEBUG
		F.OutSkip	();
		F.OutNext	("dbg_strcmp:  %d",			Memory.stat_strcmp);	Memory.stat_strcmp=0;
#endif

		//////////////////////////////////////////////////////////////////////////
		// Renderer specific
		F.OutSet						(200,0);
		Render->Statistics				(&F);

		//////////////////////////////////////////////////////////////////////////
		// Game specific
		F.OutSet						(400,0);
		g_pGamePersistent->Statistics	(&F);

		//////////////////////////////////////////////////////////////////////////
		// PERF ALERT
		F.SetColor						(color_rgba(255,16,16,255));
		F.OutSet						(300,300);
		F.SetSize						(f_base_size*2);
		if (fFPS<30)					F.OutNext	("FPS       < 30:   %3.1f",	fFPS);
		if (RCache.stat.verts>500000)	F.OutNext	("Verts     > 500k: %d",	RCache.stat.verts);
		if (RCache.stat.polys>500000)	F.OutNext	("Polys     > 500k: %d",	RCache.stat.polys);
		if (RCache.stat.calls>1000)		F.OutNext	("DIP/DP    > 1k:   %d",	RCache.stat.calls);
		if (RCache.stat.textures>1000)	F.OutNext	("T_change  > 500:  %d",	RCache.stat.textures);
		if (RCache.stat.ps>100)			F.OutNext	("PS_change > 100:  %d",	RCache.stat.ps);
		if (RCache.stat.vs>100)			F.OutNext	("VS_change > 100:  %d",	RCache.stat.vs);
		if (RCache.stat.vb>100)			F.OutNext	("VB_change > 100:  %d",	RCache.stat.vb);
		if (RCache.stat.ib>100)			F.OutNext	("IB_change > 100:  %d",	RCache.stat.ib);
		if (RenderDUMP_DT_Count>1000)	F.OutNext	("DT_count  > 1000: %d",	RenderDUMP_DT_Count);
		F.OutSkip						();
		if (fMem_calls>500)				F.OutNext	("MMGR calls > 500: %3.1f",	fMem_calls);
		if (Sheduler.result>3.f)		F.OutNext	("Update     > 3ms:	%3.1f",	Sheduler.result);
		if (UpdateClient.result>3.f)	F.OutNext	("UpdateCL   > 3ms: %3.1f",	UpdateClient.result);
		if (Physics.result>5.f)			F.OutNext	("Physics    > 5ms: %3.1f",	Physics.result);	

		//////////////////////////////////////////////////////////////////////////
		F.SetSize	(f_base_size);
		F.OnRender	();
	}

	{
		EngineTOTAL.FrameStart		();	
		Sheduler.FrameStart			();	
		UpdateClient.FrameStart		();	
		Scripting.FrameStart		();	
		Physics.FrameStart			();	
		ph_collision.FrameStart		();
		ph_core.FrameStart			();
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

		g_SpatialSpace->stat_insert.FrameStart();
		g_SpatialSpace->stat_remove.FrameStart();
	}
	dwSND_Played = dwSND_Allocated = 0;
}

void CStats::OnDeviceCreate			()
{
	pFont	= xr_new<CGameFont>		("stat_font");
	
	if(!pSettings->section_exist("evaluation")
		||!pSettings->line_exist("evaluation","line1")
		||!pSettings->line_exist("evaluation","line2")
		||!pSettings->line_exist("evaluation","line3") )
		Debug.fatal("");

	eval_line_1 = pSettings->r_string_wb("evaluation","line1");
	eval_line_2 = pSettings->r_string_wb("evaluation","line2");
	eval_line_3 = pSettings->r_string_wb("evaluation","line3");
}

void CStats::OnDeviceDestroy		()
{
	xr_delete	(pFont);
}
