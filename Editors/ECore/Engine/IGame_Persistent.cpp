#include "stdafx.h"
#pragma hdrstop

#include "IGame_Persistent.h"
#include "environment.h"
#ifndef _EDITOR
#	include "IGame_Level.h"
#	include "XR_IOConsole.h"
#	include "Render.h"
#endif

ENGINE_API	IGame_Persistent*		g_pGamePersistent	= NULL;

IGame_Persistent::IGame_Persistent	()
{
	Device.seqAppStart.Add			(this);
	Device.seqAppEnd.Add			(this);
	Device.seqFrame.Add				(this,REG_PRIORITY_HIGH+1);
	Device.seqAppActivate.Add		(this);
	Device.seqAppDeactivate.Add		(this);

	m_pMainUI						= NULL;
}

IGame_Persistent::~IGame_Persistent	()
{
	Device.seqFrame.Remove			(this);
	Device.seqAppStart.Remove		(this);
	Device.seqAppEnd.Remove			(this);
	Device.seqAppActivate.Remove	(this);
	Device.seqAppDeactivate.Remove	(this);
}

void IGame_Persistent::OnAppActivate		()
{
	Device.Pause(FALSE);
}

void IGame_Persistent::OnAppDeactivate		()
{
	if(!bDedicatedServer)
		Device.Pause(TRUE);
}

void IGame_Persistent::OnAppStart	()
{
	if (strstr(Core.Params,"-dedicated"))	bDedicatedServer	= TRUE;
	else									bDedicatedServer	= FALSE;
	Environment.load				();
}

void IGame_Persistent::OnAppEnd		()
{
	Environment.unload				();
	OnGameEnd						();
}

void IGame_Persistent::Start		(LPCSTR op)
{
	string256						prev_type;
	strcpy							(prev_type,m_game_params.m_game_type);
	m_game_params.parse_cmd_line	(op);
	if (0!=xr_strcmp(prev_type,m_game_params.m_game_type)){
		OnGameEnd					();
		OnGameStart					();
	}
}

void IGame_Persistent::Disconnect	()
{
}


void IGame_Persistent::OnGameStart	()
{
#ifndef _EDITOR
	if (0==strstr(Core.Params,"-noprefetch")){
		// prefetch game objects
		ObjectPool.prefetch				();
		Render->models_Prefetch			();
	}
#endif
}

void IGame_Persistent::OnGameEnd	()
{
#ifndef _EDITOR
	ObjectPool.clear				();
	Render->models_Clear			();
#endif
}

void IGame_Persistent::OnFrame		()
{
	Environment.OnFrame				();
#ifndef _EDITOR
	if(Device.dwFrame ==50){
		if(!g_pGameLevel)
			Console->Execute("main_menu on");
	}
#endif
}
