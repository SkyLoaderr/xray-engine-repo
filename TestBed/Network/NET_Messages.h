#pragma once
#pragma pack(push,1)

IC DWORD	net_flags	(BOOL bReliable=FALSE, BOOL bSequental=TRUE, BOOL bHighPriority=FALSE)
{
	return 
		(bReliable?DPNSEND_GUARANTEED:DPNSEND_NOCOMPLETE) | 
		(bSequental?0:DPNSEND_NONSEQUENTIAL) | 
		(bHighPriority?DPNSEND_PRIORITY_HIGH:0)
		;
}

struct	MSYS_CONFIG	// server 2 client - first packet ever, uncompressed
{
	DWORD			sign1;	// 0x12071980;
	DWORD			sign2;	// 0x26111975;
	WORD			send	[256];
	WORD			receive	[256];
};

enum	MESSAGES	// generic(system) messages
{
	_CL_CONNECT=0,	
	_SV_ACCEPT,	
	_SV_DECLINE,
	_SV_SPAWN,
	_CL_READY,

	_FORCEDWORD	=DWORD(-1)
};

#pragma pack(pop)
