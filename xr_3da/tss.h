#pragma once

#include "tss_def.h"

class ENGINE_API CSimulatorTSS
{
	DWORD	cache		[8][30];
public:
	IC void	Invalidate	()	{ memset(cache,0xff,sizeof(cache)); }
	IC void Set			(SimulatorStates& container, DWORD S, DWORD N, DWORD V) 
	{
		R_ASSERT(S<8 && N<30);
		if (cache[S][N] != V) 
		{
			cache[S][N]		= V;
			container.set_TSS(S,N,V);
		}
	}
	IC void SetColor	(SimulatorStates& container, DWORD S, DWORD A1, DWORD OP, DWORD A2)
	{
		Set(container,S,D3DTSS_COLOROP,	OP);
		switch (OP)
		{
		case D3DTOP_DISABLE:
			break;
		case D3DTOP_SELECTARG1:
			Set(container,S,D3DTSS_COLORARG1,	A1);
			break;
		case D3DTOP_SELECTARG2:
			Set(container,S,D3DTSS_COLORARG2,	A2);
			break;
		default:
			Set(container,S,D3DTSS_COLORARG1,	A1);
			Set(container,S,D3DTSS_COLORARG2,	A2);
			break;
		}
	}
	IC void SetAlpha	(SimulatorStates& container, DWORD S, DWORD A1, DWORD OP, DWORD A2)
	{
		Set(container,S,D3DTSS_ALPHAOP,	OP);
		switch (OP)
		{
		case D3DTOP_DISABLE:
			break;
		case D3DTOP_SELECTARG1:
			Set(container,S,D3DTSS_ALPHAARG1,	A1);
			break;
		case D3DTOP_SELECTARG2:
			Set(container,S,D3DTSS_ALPHAARG2,	A2);
			break;
		default:
			Set(container,S,D3DTSS_ALPHAARG1,	A1);
			Set(container,S,D3DTSS_ALPHAARG2,	A2);
			break;
		}
	}
};

class ENGINE_API CSimulatorRS
{
	DWORD	cache		[256];
public:
	IC void	Invalidate	()	{ memset(cache,0xff,sizeof(cache)); }
	IC void Set			(SimulatorStates& container, DWORD N, DWORD V)	
	{
		R_ASSERT(N<256);
		if (cache[N] != V)
		{
			cache[N] = V;
			container.set_RS(N,V);
		}
	}
};

class ENGINE_API CSimulator
{
public:
	CSimulatorTSS		TSS;
	CSimulatorRS		RS;
	SimulatorStates		container;
public:
						CSimulator	()							{ Invalidate(); }
	IC void				Invalidate	()							{ TSS.Invalidate(); RS.Invalidate(); container.clear(); }
	IC void				SetTSS		(DWORD S, DWORD N, DWORD V) { TSS.Set(container,S,N,V);	}
	IC void				SetColor	(DWORD S, DWORD a, DWORD b, DWORD c) {TSS.SetColor(container,S,a,b,c);}
	IC void				SetAlpha	(DWORD S, DWORD a, DWORD b, DWORD c) {TSS.SetAlpha(container,S,a,b,c);}
	IC void				SetRS		(DWORD N, DWORD V)			{ RS.Set(container,N,V);	}
	IC SimulatorStates&	GetContainer()							{ return container; }
};

