#include "stdafx.h"
#include "xrGameSpyServer.h"
#include "xrGameSpyServer_callbacks.h"

xrGameSpyServer::xrGameSpyServer()
{
	m_iReportToMasterServer = 0;
	m_bQR2_Initialized = FALSE;
	m_bCDKey_Initialized = FALSE;

	//set the secret key, in a semi-obfuscated manner
	secret_key[0] = 't';
	secret_key[1] = '9';
	secret_key[2] = 'F';
	secret_key[3] = 'j';
	secret_key[4] = '3';
	secret_key[5] = 'M';
	secret_key[6] = 'x';
	secret_key[7] = '\0';	
}

xrGameSpyServer::~xrGameSpyServer()
{
	QR2_ShutDown();
	CDKey_ShutDown();
}

BOOL xrGameSpyServer::Connect(ref_str &session_name)
{
	BOOL res = inherited::Connect(session_name);
	if (!res) return res;


	
	if ( 0 == *(game->get_option_s		(*session_name,"hname",NULL)))
	{
		string1024	CompName;
		DWORD		CompNameSize = 1024;
		if (GetComputerName(CompName, &CompNameSize)) HostName._set(CompName);
	}
	else
		HostName._set(game->get_option_s		(*session_name,"hname",NULL));
	
	if (0 != *(game->get_option_s		(*session_name,"psw",NULL)))
		Password._set(game->get_option_s		(*session_name,"psw",NULL));

	string64	tMapName;
	strcpy(tMapName, *session_name);
	*(strchr(tMapName, '/')) = 0;
	MapName._set(tMapName);// = (session_name);
	

	m_iReportToMasterServer = game->get_option_i		(*session_name,"public",0);
	m_iMaxPlayers	= game->get_option_i		(*session_name,"maxplayers",32);
	//--------------------------------------------//
	if (game->Type() != GAME_SINGLE) 
	{
		QR2_Init();
		CDKey_Init();
	};

	return res;
}

void			xrGameSpyServer::Update				()
{
	inherited::Update();

	if (m_bQR2_Initialized)
	{
		qr2_think(NULL);
	};

	if (m_bCDKey_Initialized)
	{
		gcd_think();
	};
}

int				xrGameSpyServer::GetPlayersCount()
{
	int NumPlayers = client_Count();
	if (!g_pGamePersistent->bDedicatedServer || NumPlayers < 1) return NumPlayers;
	return NumPlayers - 1;
};

void			xrGameSpyServer::OnCL_Connected		(IClient* _CL)
{
	csPlayers.Enter					();
	csPlayers.Leave					();

	inherited::OnCL_Connected(_CL);
};