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
	CMD3(CCC_String,	"net_palyer_name",				m_playerName,	64);

	// client
	m_uNetFilters.assign(0xffffffff);
	CMD3(CCC_Mask,	"net_connection_lan",				&m_uNetFilters,		flNetConnectionLan);
	CMD3(CCC_Mask,	"net_filter_empty",					&m_uNetFilters,		flNetFilterEmpty);
	CMD3(CCC_Mask,	"net_filter_full",					&m_uNetFilters,		flNetFilterFull);
	CMD3(CCC_Mask,	"net_filter_with_password",			&m_uNetFilters,		flNetFilterWithPassword);
	CMD3(CCC_Mask,	"net_filter_without_password",		&m_uNetFilters,		flNetFilterWithoutPassword);
	CMD3(CCC_Mask,	"net_filter_without_friendlyfire",	&m_uNetFilters,		flNetFilterWithoutFriendlyFire);
	CMD3(CCC_Mask,	"net_filter_without_punkbusier",	&m_uNetFilters,		flNetFilterWithoutPunkbusier);
	CMD3(CCC_Mask,	"net_filter_listenservers",			&m_uNetFilters,		flNetFilterListenServers);

	// server
	m_iMaxPlayers = 32;
	CMD4(CCC_Integer,	"net_srv_maxplayers",			&m_iMaxPlayers,	2, 32);
	CMD3(CCC_Token,		"net_srv_gamemode",				&m_curGameMode,	g_GameModes);
	m_uNetSrvParams.assign(0x0);
	CMD3(CCC_Mask,		"net_srv_dedicated",			&m_uNetSrvParams,	flNetSrvDedicated);

	strcpy(m_serverName, Core.CompName);
	CMD3(CCC_String,	"net_srv_name",				m_serverName,	64);




//	CMD4(CCC_Mask,	"net_con_publicserver",				&m_uNetFilters,	0, 1);
//	CMD4(CCC_Mask,	"net_con_maprotation",				&m_uNetFilters,	0, 1);
//	CMD4(CCC_Mask,	"net_con_spectator_on",				&m_uNetFilters,	0, 1);
	m_iNetConSpectator = 1;
//	CMD4(CCC_Mask,	"net_con_spectator",				&m_iNetConSpectator,				1, 60);
};