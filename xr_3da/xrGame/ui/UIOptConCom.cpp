#include "StdAfx.h"
#include "../../xr_ioconsole.h"
#include "../../xr_ioc_cmd.h"
#include "UIOptConCom.h"
#include "../../xrcore/xrCore.h"

xr_token g_GameModes	[] = {
	{ "Deathmatch",			0	},
	{ "Team Deathmatch",	1	},
	{ "Artefacthunt",		2	},
	{ 0,					0	}
};


CUIOptConCom::CUIOptConCom(){
			
}


void CUIOptConCom::Init(){
	// common
	strcpy(m_playerName, Core.UserName);
	CMD3(CCC_String,	"mm_net_player_name",				m_playerName,	64);

	// server
	m_iMaxPlayers = 32;
	CMD4(CCC_Integer,	"mm_net_srv_maxplayers",			&m_iMaxPlayers,	2, 32);
	CMD3(CCC_Token,		"mm_net_srv_gamemode",				&m_curGameMode,	g_GameModes);
	m_uNetSrvParams.assign(0x0);
	m_uNetSrvParams.set(flNetDamageBlockIndicator, true);
	m_uNetSrvParams.set(flNetSpecTeamOnly, true);
	m_uNetSrvParams.set(flNetSpecFirstEye, true);
	m_uNetSrvParams.set(flNetSpecLookAt, true);
	m_uNetSrvParams.set(flNetSpecFreeLook, true);
	CMD3(CCC_Mask,		"mm_mm_net_srv_dedicated",			&m_uNetSrvParams,	flNetSrvDedicated);
	CMD3(CCC_Mask,		"mm_net_con_publicserver",			&m_uNetSrvParams,	flNetConPublicServer);
	CMD3(CCC_Mask,		"mm_net_con_maprotation",			&m_uNetSrvParams,	flNetConMaprotation);
	CMD3(CCC_Mask,		"mm_net_con_spectator_on",			&m_uNetSrvParams,	flNetConSpectatorOn);
	CMD3(CCC_Mask,		"mm_net_con_allowvoting",			&m_uNetSrvParams,	flNetConAllowVoting);
	CMD3(CCC_Mask,		"mm_net_con_damageblockindicator",	&m_uNetSrvParams,	flNetDamageBlockIndicator);
	CMD3(CCC_Mask,		"mm_net_srv_autoteambalance",		&m_uNetSrvParams,	flNetAutoTeamBalance);
	CMD3(CCC_Mask,		"mm_net_srv_friendlyindicators",	&m_uNetSrvParams,	flNetFriendlyIndicators);
	CMD3(CCC_Mask,		"mm_net_srv_friendlynames",			&m_uNetSrvParams,	flNetFriendlyNames);
	CMD3(CCC_Mask,		"mm_net_srv_noanomalies",			&m_uNetSrvParams,	flNetNoAnomalies);

	CMD3(CCC_Mask,		"mm_net_spec_teamonly",				&m_uNetSrvParams,	flNetSpecTeamOnly);
	CMD3(CCC_Mask,		"mm_net_spec_freefly",				&m_uNetSrvParams,	flNetSpecFreeFly);
	CMD3(CCC_Mask,		"mm_net_spec_firsteye",				&m_uNetSrvParams,	flNetSpecFirstEye);
	CMD3(CCC_Mask,		"mm_net_spec_lookat",				&m_uNetSrvParams,	flNetSpecLookAt);
	CMD3(CCC_Mask,		"mm_net_spec_freelook",				&m_uNetSrvParams,	flNetSpecFreeLook);

	m_iNetConSpectator = 20;
	CMD4(CCC_Integer,	"mm_net_con_spectator",				&m_iNetConSpectator,	1, 32);
	m_iFriendlyFire = 100;
	CMD4(CCC_Integer,	"mm_net_srv_friendlyfire",			&m_iFriendlyFire,		1, 200);
	m_iArtefactsNum = 10;
	CMD4(CCC_Integer,	"mm_net_srv_artefactsnum",			&m_iArtefactsNum,		1, 32);
	CMD4(CCC_Integer,	"mm_net_force_respawn",				&m_iNetForceRespawn,	0, 100);
	m_iNetReinforcement = 30;
	CMD4(CCC_Integer,	"mm_net_reinforcement",				&m_iNetReinforcement,	0, 100);
	m_iNetFreezeTime = 5;
	CMD4(CCC_Integer,	"mm_net_respawn_freezetime",		&m_iNetFreezeTime,		0, 100);
	CMD4(CCC_Integer,	"mm_net_respawn_damageblock",		&m_iNetDamageBlock,		0, 100);
	CMD4(CCC_Integer,	"mm_net_srv_fraglimit",				&m_iNetFragLimit,		0, 100);
	CMD4(CCC_Integer,	"mm_net_srv_timelimit",				&m_iNetTimeLimit,		0, 100);
	CMD4(CCC_Integer,	"mm_net_srv_artefactstay",			&m_iNetArtefactStay,	0, 100);
	m_iNetArtefactDelay = 30;
	CMD4(CCC_Integer,	"mm_net_srv_artefactdelay",			&m_iNetArtefactDelay,	0, 100);
	m_iNetAnomalyTime = 5;
	CMD4(CCC_Integer,	"mm_net_srv_anomalytime",			&m_iNetAnomalyTime,		1, 100);
	m_iNetWarmUpTime = 0;
	CMD4(CCC_Integer,	"mm_net_srv_warmuptime",			&m_iNetWarmUpTime,		0, 3600000);
	m_iNetWeatherRate = 1;
	CMD4(CCC_Integer,	"mm_net_weather_rateofchange",		&m_iNetWeatherRate,		0, 100);
	strcpy(m_serverName, Core.CompName);
	CMD3(CCC_String,	"mm_net_srv_name",					m_serverName,	64);
	CMD3(CCC_String,	"mm_net_con_maprotlist",			m_maprotlist,	64);
};