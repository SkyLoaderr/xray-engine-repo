#include "stdafx.h"
#pragma hdrstop

#include "gamepersistent.h"

#include "..\xr_ioconsole.h"

CGamePersistent::CGamePersistent(void)
{
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

void CGamePersistent::OnAppCycleStart()
{
	// load game materials
	string256		fn_mtl;
	if (FS.exist(fn_mtl, "$game_data$","gamemtl.xr"))
		GMLib.Load	(fn_mtl);

	__super::OnAppCycleStart	();
}

void CGamePersistent::OnAppCycleEnd	()
{
	__super::OnAppCycleEnd		();

	GMLib.Unload				();
}

void CGamePersistent::OnFrame		()
{
	VERIFY	(pDemoFile);
	
	if (Device.dwTimeGlobal>uTime2Change)
	{
		// Change level + play demo
		if			(pDemoFile->elapsed()<3)	pDemoFile->seek(0);		// cycle
		
		// Read params
		string512			params;
		pDemoFile->r_string	(params);
		string256			o_server, o_client, o_demo;	u32 o_time;
		sscanf				(params,"%[^,],%[^,],%[^,],%d",o_server,o_client,o_demo,&o_time);

		// Start new level + demo
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
	Console.Execute		(cmd);
	xr_free				(demo);
	uTime2Change		= Device.TimerAsync() + u32(P2)*1000;
}
