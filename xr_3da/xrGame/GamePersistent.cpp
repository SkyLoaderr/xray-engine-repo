#include "stdafx.h"
#pragma hdrstop

#include "gamepersistent.h"
#include "../fmesh.h"
#include "../xr_ioconsole.h"
#include "gamemtllib.h"

#ifndef _EDITOR
#	include "ai_debug.h"
#	include "profiler.h"
#endif

static	void *	ode_alloc	(size_t size)								{ return xr_malloc(size);			}
static	void *	ode_realloc	(void *ptr, size_t oldsize, size_t newsize)	{ return xr_realloc(ptr,newsize);	}
static	void	ode_free	(void *ptr, size_t size)					{ return xr_free(ptr);				}

CGamePersistent::CGamePersistent(void)
{
	// 
	// dSetAllocHandler		(ode_alloc		);
	// dSetReallocHandler	(ode_realloc	);
	// dSetFreeHandler		(ode_free		);

	// 
	BOOL	bDemoMode	= (0!=strstr(Core.Params,"-demomode "));
	if (bDemoMode)
	{
		string256	fname;
		LPCSTR		name	=	strstr(Core.Params,"-demomode ") + 10;
		sscanf				(name,"%s",fname);
		R_ASSERT2			(fname[0],"Missing filename for 'demomode'");
		Msg					("- playing in demo mode '%s'",fname);
		pDemoFile			=	FS.r_open	(fname);
		Device.seqFrame.Add	(this);
		eDemoStart			=	Engine.Event.Handler_Attach("GAME:demo",this);	
		uTime2Change		=	0;
	} else
	{
		pDemoFile			=	NULL;
		eDemoStart			=	NULL;
	}
}

CGamePersistent::~CGamePersistent(void)
{
	FS.r_close					(pDemoFile);
	Device.seqFrame.Remove		(this);
	Engine.Event.Handler_Detach	(eDemoStart,this);
}

void CGamePersistent::RegisterModel(IRender_Visual* V)
{
	// Check types
	switch (V->Type){
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:{
		u16 def_idx		= GMLib.GetMaterialIdx("default_object");
		R_ASSERT2		(GMLib.GetMaterialByIdx(def_idx)->Flags.is(SGameMtl::flDynamic),"'default_object' - must be dynamic");
		CKinematics* K	= PKinematics(V); VERIFY(K);
		int cnt = K->LL_BoneCount();
		for (u16 k=0; k<cnt; k++){
			CBoneData& bd	= K->LL_GetData(k); 
			if (*(bd.game_mtl_name)){
				bd.game_mtl_idx	= GMLib.GetMaterialIdx(*bd.game_mtl_name);
				R_ASSERT2(GMLib.GetMaterialByIdx(bd.game_mtl_idx)->Flags.is(SGameMtl::flDynamic),"Required dynamic game material");
			}else{
				bd.game_mtl_idx	= def_idx;
			}
		}
	}break;
	}
}

void CGamePersistent::OnAppCycleStart()
{
	// load game materials
	GMLib.Load					();

	__super::OnAppCycleStart	();
}

void CGamePersistent::OnAppCycleEnd	()
{
	__super::OnAppCycleEnd		();

	GMLib.Unload				();
}

void CGamePersistent::OnFrame		()
{
	__super::OnFrame	();

	if	(0==pDemoFile)	return;
	
	if	(Device.dwTimeGlobal>uTime2Change)
	{
		// Change level + play demo
		if			(pDemoFile->elapsed()<3)	pDemoFile->seek(0);		// cycle
		
		// Read params
		string512			params;
		pDemoFile->r_string	(params);
		string256			o_server, o_client, o_demo;	u32 o_time;
		sscanf				(params,"%[^,],%[^,],%[^,],%d",o_server,o_client,o_demo,&o_time);

		// Start _new level + demo
		Engine.Event.Defer	("KERNEL:disconnect");
		Engine.Event.Defer	("KERNEL:start",size_t(xr_strdup(_Trim(o_server))),size_t(xr_strdup(_Trim(o_client))));
		Engine.Event.Defer	("GAME:demo",	size_t(xr_strdup(_Trim(o_demo))), u64(o_time));
		uTime2Change		= 0xffffffff;	// Block changer until Event received
	}
}

void CGamePersistent::OnEvent(EVENT E, u64 P1, u64 P2)
{
	string256			cmd;
	LPCSTR				demo	= LPCSTR(P1);
	sprintf				(cmd,"demo_play %s",demo);
	Console->Execute	(cmd);
	xr_free				(demo);
	uTime2Change		= Device.TimerAsync() + u32(P2)*1000;
}

void CGamePersistent::Statistics	(CGameFont* F)
{
#ifndef _EDITOR
	profiler().show_stats	(F,!!psAI_Flags.test(aiStats));
#endif
}