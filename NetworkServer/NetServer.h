// NetworkServer.h: interface for the CNetworkServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKSERVER_H__4C7DE8C4_F881_49CC_A463_356927018258__INCLUDED_)
#define AFX_NETWORKSERVER_H__4C7DE8C4_F881_49CC_A463_356927018258__INCLUDED_
#pragma once

#include "Network.h"

class CNetworkServer : public CNetwork  
{
public:
	CNetworkServer			(EProtocol P, char *session, DWORD dwFlags);
	virtual ~CNetworkServer	();

	virtual void			OnPlayerCreate			(DPID id);
	virtual void			OnPlayerDestroy			(DPID id);

	void					OnFrame	();
};

#endif // !defined(AFX_NETWORKSERVER_H__4C7DE8C4_F881_49CC_A463_356927018258__INCLUDED_)
