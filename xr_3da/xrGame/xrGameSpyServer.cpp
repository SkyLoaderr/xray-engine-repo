#include "stdafx.h"
#include "xrGameSpyServer.h"
#include "xrGameSpyServer_callbacks.h"

xrGameSpyServer::xrGameSpyServer()
{
	m_iReportToMasterServer = 0;
	m_bQR2_Initialized = FALSE;

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
}

BOOL xrGameSpyServer::Connect(ref_str &session_name)
{
	BOOL res = xrServer::Connect(session_name);
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
	if (game->Type() != GAME_SINGLE) QR2_Init();

	return res;
}

void			xrGameSpyServer::QR2_Init			()
{	
	//--------- QR2 Init -------------------------/
	//call qr_init with the query port number and gamename, default IP address, and no user data
	if (qr2_init(NULL,NULL,0/*GAMESPY_BASEPORT*/, GAMESPY_GAMENAME, secret_key, (m_iReportToMasterServer == 0)?0:1, 1,
		callback_serverkey, callback_playerkey, callback_teamkey,
		callback_keylist, callback_count, callback_adderror, this) != e_qrnoerror)
	{
		//		_tprintf(_T("Error starting query sockets\n"));
		return;
	}

	// Set a function to be called when we receive a game specific message
	qr2_register_clientmessage_callback(NULL, callback_cm);

	// Set a function to be called when we receive a nat negotiation request
	qr2_register_natneg_callback(NULL, callback_nn);

	/*
	DWORD aStartTime = GetTickCount();
	while ((current_time() - aStartTime) < 60000)
	{
		// An actual game would do something between "thinks"
//		DoGameStuff();

		//check for / process incoming queries
		qr2_think(NULL);
	}
	*/
	Msg("GameSpy::QR2 : Initialized");
	m_bQR2_Initialized = TRUE;
};

void			xrGameSpyServer::QR2_ShutDown()
{
	m_bQR2_Initialized = FALSE;
	qr2_shutdown(NULL);	
};

void			xrGameSpyServer::Update				()
{
	xrServer::Update();

//	if (!m_bQR2_Inited) QR2_Init();
//	else	
	if (m_bQR2_Initialized)
	{
		qr2_think(NULL);
	};
}