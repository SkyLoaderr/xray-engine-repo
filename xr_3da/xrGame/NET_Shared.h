#pragma once

#include "net_utils.h"
#include "net_messages.h"

class XRNETSERVER_API IClientStatistic
{
	DPN_CONNECTION_INFO	ci_last;
	u32					mps_recive, mps_receive_base;
	u32					mps_send,	mps_send_base;
	u32					dwBaseTime;
	CTimer*				device_timer;
public:
			IClientStatistic	(CTimer* timer):device_timer(timer)	{ ZeroMemory(this,sizeof(*this));	dwBaseTime=TimeGlobal(device_timer); }

	void	Update				(DPN_CONNECTION_INFO& CI);

	IC u32	getPing				()	{ return ci_last.dwRoundTripLatencyMS;	}
	IC u32	getBPS				()	{ return ci_last.dwThroughputBPS;		}
	IC u32	getDroppedCount		()	{ return ci_last.dwPacketsDropped;		}
	IC u32	getMPS_Receive		()  { return mps_recive;	}
	IC u32	getMPS_Send			()	{ return mps_send;		}
};

