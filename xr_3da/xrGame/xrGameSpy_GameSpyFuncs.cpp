#include "stdafx.h"
#include "xrGameSpyServer.h"
#include "xrGameSpyServer_callbacks.h"

/////////////////////// QR2 ///////////////////////////////////////
void			xrGameSpyServer::QR2_Init			()
{	
	qr2_register_key(DEDICATED_KEY,   ("dedicated")  );
	//--------- QR2 Init -------------------------/
	//call qr_init with the query port number and gamename, default IP address, and no user data
	if (qr2_init(NULL,NULL,GAMESPY_BASEPORT, GAMESPY_GAMENAME, secret_key, (m_iReportToMasterServer == 0)?0:1, 1,
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

//------------------------------- CD_Key -----------------------------

void			xrGameSpyServer::CDKey_Init			()
{

};

void			xrGameSpyServer::CDKey_ShutDown()
{
};
