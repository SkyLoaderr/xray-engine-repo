#pragma once

#include "net_utils.h"
#include "net_messages.h"
#include "xrSyncronize.h"

class ENGINE_API IClientStatistic
{
	DPN_CONNECTION_INFO	ci_last;
	DWORD				mps_recive, mps_receive_base;
	DWORD				mps_send,	mps_send_base;
	DWORD				dwBaseTime;
public:
	IClientStatistic()	{	ZeroMemory(this,sizeof(*this));	dwBaseTime=Device.dwTimeGlobal; }

	void		Update				(DPN_CONNECTION_INFO& CI);

	IC DWORD	getPing				()	{ return ci_last.dwRoundTripLatencyMS;	}
	IC DWORD	getBPS				()	{ return ci_last.dwThroughputBPS;		}
	IC DWORD	getDroppedCount		()	{ return ci_last.dwPacketsDropped;		}
	IC DWORD	getMPS_Receive		()  { return mps_recive;	}
	IC DWORD	getMPS_Send			()	{ return mps_send;		}
};

