#include "xrServer.h"
//#include "GameSpy/QR2/qr2.h"
#pragma once
#define GAMESPY_GAMENAME			"stalkersc"
#define GAMESPY_PRODUCTID			0
#define	GAMESPY_BASEPORT			5447
#define	GAMESPY_MAXCHALLANGESIZE	32

#define	DEDICATED_KEY			100

class xrGameSpyClientData	: public xrClientData
{
public:
	char					m_pChallengeString[GAMESPY_MAXCHALLANGESIZE+1];
	bool					m_bCDKeyAuth;

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
protected:
	virtual bool					NeedToCheckClient	(IClient* CL);
public:
	shared_str							HostName;
	shared_str							MapName;
	shared_str							Password;
	int								m_iMaxPlayers;
	bool							m_bCheckCDKey;

	int								GetPlayersCount					();
	void							OnCDKey_Validation				(ClientID ID, int res, char* errormsg);
public:
	xrGameSpyServer					();
	virtual ~xrGameSpyServer		();

	virtual BOOL			Connect				(shared_str& session_name);
	virtual void			Update				();

//	virtual void			OnCL_Connected		(IClient* C);
	virtual void			OnCL_Disconnected	(IClient* C);
	virtual IClient*		client_Create		();

	virtual u32				OnMessage			(NET_Packet& P, ClientID/*DPNID*/ sender);	// Non-Zero means broadcasting with "flags" as returned
};

