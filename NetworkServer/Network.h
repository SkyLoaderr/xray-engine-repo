// Network.h: interface for the CNetwork class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORK_H__D342CD80_1529_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_NETWORK_H__D342CD80_1529_11D4_B4E3_4854E82A090D__INCLUDED_
#pragma once

#define		PING_UPDATE_TIME	1.f

enum ENGINE_API EProtocol {
	dpTCP=0,
	dpIPX,
	dpSERIAL,
	dpMODEM2MODEM,
};

// network message
struct ENGINE_API	GAMEMSG
{
    BYTE	dwType;
	DWORD	dwTimeStamp;
};

struct ENGINE_API SPlayer {
    DPID	id;
	char	name[32];
	DWORD	dwPing;

	IC BOOL	operator < (const SPlayer &other) {
		return id<other.id;
	}
};
IC BOOL operator < (const SPlayer &A, const SPlayer &B)
{	return A.id<B.id; }

class ENGINE_API CNetwork
{
friend BOOL FAR PASCAL _EnumConnectionsCallback	(LPCGUID,VOID*,DWORD,LPCDPNAME,DWORD,VOID*);
protected:
	LPDIRECTPLAY4A      pDPlay;
	DPID                dpLocalPlayerID;

	EProtocol			curProtocol;
	void*				pConnection;
	void*				pGUID;				// only for connection search

	DWORD				tDiff;				// difference between local time and game time
	char				MessageData[1024];	// setted up than trying to get/send message
	DWORD				dwRSize;			// size of received message

	void				CreateDPlay		(void);
	BOOL				SelectProtocol	(EProtocol P);
	BOOL				ApplyProtocol	(void);

	vector<SPlayer>		players;
	float				fTime;				// time to 'pinging'
public:

	// some interface
	DWORD				GetTimestamp	(void)	{ return timeGetTime()-tDiff;		}
	DWORD				GetLocalPlayerID(void)	{ return DWORD(dpLocalPlayerID);	}
	const char*			GetNameByID		(DPID id);

	virtual	void		HandleSystemMessages	(DPMSG_GENERIC *pMsg);
	virtual void		OnPlayerCreate			(DPID id)							{};
	virtual void		OnPlayerDestroy			(DPID id)							{};

	// pinging - returns TRUE if statistic updated
	BOOL				OnMove					(float fTimeDelta);

	CNetwork			();
	virtual ~CNetwork	();
};

#endif // !defined(AFX_NETWORK_H__D342CD80_1529_11D4_B4E3_4854E82A090D__INCLUDED_)
