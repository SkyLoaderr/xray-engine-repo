#include "xrServer.h"
//#include "GameSpy/QR2/qr2.h"
#pragma once
#define GAMESPY_GAMENAME		"stalkersc"
#define	GAMESPY_BASEPORT		11111

class xrGameSpyServer	: public xrServer
{
private:
	char							secret_key[9];
	int								m_iReportToMasterServer;

	BOOL							m_bQR2_Initialized;
	void							QR2_Init();
	void							QR2_ShutDown();
public:
	ref_str							HostName;
	ref_str							MapName;
	ref_str							Password;
	int								m_iMaxPlayers;
public:
	xrGameSpyServer					();
	virtual ~xrGameSpyServer		();

	virtual BOOL			Connect				(ref_str& session_name);
	virtual void			Update				();
};

