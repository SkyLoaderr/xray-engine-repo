#include "xrServer.h"
//#include "GameSpy/QR2/qr2.h"
#pragma once
#define GAMESPY_GAMENAME			"stalkersc"
#define GAMESPY_PRODUCTID			1067
#define	GAMESPY_BASEPORT			5447
#define	GAMESPY_MAXCHALLANGESIZE	32

#define	DEDICATED_KEY			100

class xrGameSpyClientData	: public xrClientData
{
public:
	char					m_pChallengeString[GAMESPY_MAXCHALLANGESIZE+1];

	xrGameSpyClientData			();
	virtual ~xrGameSpyClientData	();
};

class xrGameSpyServer	: public xrServer
{
private:
	typedef xrServer inherited;
private:
	char							secret_key[9];
	int								m_iReportToMasterServer;

	BOOL							m_bQR2_Initialized;
	void							QR2_Init						();
	void							QR2_ShutDown					();

	BOOL							m_bCDKey_Initialized;
	void							CDKey_Init						();
	void							CDKey_ShutDown					();
	void							SendChallengeString_2_Client	(IClient* C);
	void							CreateRandomChallenge			(char* challenge, int nchars);

	void							CheckAvailableServices			();
public:
	ref_str							HostName;
	ref_str							MapName;
	ref_str							Password;
	int								m_iMaxPlayers;

	int								GetPlayersCount					();
public:
	xrGameSpyServer					();
	virtual ~xrGameSpyServer		();

	virtual BOOL			Connect				(ref_str& session_name);
	virtual void			Update				();

	virtual void			OnCL_Connected		(IClient* C);
	virtual IClient*		client_Create		();
};

