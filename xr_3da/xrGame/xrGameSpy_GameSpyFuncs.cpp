#include "stdafx.h"
#include "xrGameSpyServer.h"
#include "xrGameSpyServer_callbacks.h"

#include "xrMessages.h"
#include "level.h"

//------------------------------ Available --------------------------
void			xrGameSpyServer::CheckAvailableServices		()
{
	GSIACResult result;

	// check that the game's backend is available
	GSIStartAvailableCheck(GAMESPY_GAMENAME);
	while((result = GSIAvailableCheckThink()) == GSIACWaiting)
		msleep(5);
	if(result != GSIACAvailable)
	{
		Msg("The backend is not available\n");
		return;
	}
};

/////////////////////// QR2 ///////////////////////////////////////
void			xrGameSpyServer::QR2_Init			()
{	
		qr2_register_key(DEDICATED_KEY,   ("dedicated")  );
		qr2_register_key(FFIRE_KEY,   ("friendlyfire")  );
	//--------- QR2 Init -------------------------/
	//call qr_init with the query port number and gamename, default IP address, and no user data
	if (qr2_init(NULL,NULL,GAMESPY_BASEPORT, GAMESPY_GAMENAME, secret_key, (m_iReportToMasterServer == 0)?0:1, 1,
		callback_serverkey, callback_playerkey, callback_teamkey,
		callback_keylist, callback_count, callback_adderror, this) != e_qrnoerror)
	{
		//		_tprintf(_T("Error starting query sockets\n"));
		Msg("GameSpy::QR2 : Failes to Initialize!");
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
	int res = gcd_init_qr2(NULL, GAMESPY_PRODUCTID);
	if (res == -1)
	{
		Msg("GameSpy::CDKey : Failes to Initialize!");
		return;
	};
	Msg("GameSpy::CDKey : Initialized");
	m_bCDKey_Initialized = TRUE;
};

void			xrGameSpyServer::CDKey_ShutDown()
{
	gcd_shutdown();
	m_bCDKey_Initialized = FALSE;
};

//generate a rand nchar challenge
void	xrGameSpyServer::CreateRandomChallenge(char* challenge, int nchars)
{
	if (nchars > GAMESPY_MAXCHALLANGESIZE) nchars = GAMESPY_MAXCHALLANGESIZE;
	challenge[nchars] = 0;
	while (nchars--)
	{
		challenge[nchars] = char('a' + ::Random.randI(26));
	};
}
void			xrGameSpyServer::SendChallengeString_2_Client (IClient* C)
{
	if (!C) return;
	xrGameSpyClientData* pClient = (xrGameSpyClientData*) C;

    CreateRandomChallenge(pClient->m_pChallengeString, 8);
	//--------- Send Respond ---------------------------------------------
	NET_Packet P;

	P.w_begin	(M_GAMESPY_CDKEY_VALIDATION_CHALLENGE);
	P.w_stringZ(pClient->m_pChallengeString);
	SendTo(pClient->ID, P);
}

void			xrGameSpyServer::OnCDKey_Validation				(ClientID ID, int res, char* errormsg)
{
	xrGameSpyClientData* CL = (xrGameSpyClientData*)  ID_to_client(ID);
	if (0 != res)
	{
		Msg("GameSpy::CDKey: Validation successful - <%s>", errormsg);
	}
	else
	{
		Msg("GameSpy::CDKey: Validation failed - <%s>", errormsg);
	}
	SendConnectResult(CL, u8(res), errormsg);
};
