#pragma once

#include "net_utils.h"
#include "net_messages.h"

#ifdef XR_NETSERVER_EXPORTS
	#define XRNETSERVER_API __declspec(dllexport)
#else
	#define XRNETSERVER_API __declspec(dllimport)
	#pragma comment(lib,	"x:\\xrNetServer"	)
#endif

XRNETSERVER_API extern Flags32	psNET_Flags;
XRNETSERVER_API extern int		psNET_ClientUpdate;
XRNETSERVER_API extern int		psNET_ClientPending;
XRNETSERVER_API extern char		psNET_Name[];
XRNETSERVER_API extern int		psNET_Port;
XRNETSERVER_API extern int		psNET_ServerUpdate;
XRNETSERVER_API extern int		psNET_ServerPending;

IC u32 TimeGlobal(CTimer* timer)
{
	u64	qTime	= timer->GetElapsed_clk();
	return		u32((qTime*u64(1000))/CPU::cycles_per_second);
}
IC u32 TimerAsync(CTimer* timer) 
{
	return TimeGlobal	(timer);
}

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

