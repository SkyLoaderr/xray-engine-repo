#ifndef TSS_H
#define TSS_H
#pragma once

#include "tss_def.h"

class ENGINE_API CSimulatorTSS
{
	u32	cache		[8][30];
public:
	IC void	Invalidate	()	{ memset(cache,0xff,sizeof(cache)); }
	IC void Set			(SimulatorStates& container, u32 S, u32 N, u32 V) 
	{
		R_ASSERT(S<8 && N<30);
		if (cache[S][N] != V) 
		{
			cache[S][N]		= V;
			container.set_TSS(S,N,V);
		}
	}
	IC void SetColor	(SimulatorStates& container, u32 S, u32 A1, u32 OP, u32 A2)
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
	IC void SetColor3	(SimulatorStates& container, u32 S, u32 A1, u32 OP, u32 A2, u32 A3)
	{
		SetColor		(container,S,A1,OP,A2);
		Set				(container,S,D3DTSS_COLORARG0,A3);
	}
	IC void SetAlpha	(SimulatorStates& container, u32 S, u32 A1, u32 OP, u32 A2)
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
	IC void SetAlpha3	(SimulatorStates& container, u32 S, u32 A1, u32 OP, u32 A2, u32 A3)
	{
		SetAlpha		(container,S,A1,OP,A2);
		Set				(container,S,D3DTSS_ALPHAARG0,A3);
	}
};

class ENGINE_API CSimulatorRS
{
	u32	cache		[256];
public:
	IC void	Invalidate	()	{ memset(cache,0xff,sizeof(cache)); }
	IC void Set			(SimulatorStates& container, u32 N, u32 V)	
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
	IC void				SetTSS		(u32 S, u32 N, u32 V) { TSS.Set(container,S,N,V);	}
	IC void				SetColor	(u32 S, u32 a, u32 b, u32 c)			{TSS.SetColor(container,S,a,b,c);}
	IC void				SetColor3	(u32 S, u32 a, u32 b, u32 c, u32 d)	{TSS.SetColor3(container,S,a,b,c,d);}
	IC void				SetAlpha	(u32 S, u32 a, u32 b, u32 c)			{TSS.SetAlpha(container,S,a,b,c);}
	IC void				SetAlpha3	(u32 S, u32 a, u32 b, u32 c, u32 d)	{TSS.SetAlpha3(container,S,a,b,c,d);}
	IC void				SetRS		(u32 N, u32 V)			{ RS.Set(container,N,V);	}
	IC SimulatorStates&	GetContainer()							{ return container; }
};

#endif