#pragma once

class ENGINE_API CSimulatorTSS
{
	DWORD	cache		[5][30];
public:
//	CSimulatorTSS()		{ Invalidate();	}
	IC void	Invalidate	()	{ memset(cache,0xff,sizeof(cache)); }
	IC void Set			(DWORD S, DWORD N, DWORD V) 
	{
		R_ASSERT(S<4 && N<30);
		if (cache[S][N] != V) 
		{
			cache[S][N] = V;
			HW.pDevice->SetTextureStageState(S,(D3DTEXTURESTAGESTATETYPE)N,V);
		}
	}
	IC void SetColor	(DWORD S, DWORD A1, DWORD OP, DWORD A2)
	{
		Set(S,D3DTSS_COLOROP,	OP);
		switch (OP)
		{
		case D3DTOP_DISABLE:
			break;
		case D3DTOP_SELECTARG1:
			Set(S,D3DTSS_COLORARG1,	A1);
			break;
		case D3DTOP_SELECTARG2:
			Set(S,D3DTSS_COLORARG2,	A2);
			break;
		default:
			Set(S,D3DTSS_COLORARG1,	A1);
			Set(S,D3DTSS_COLORARG2,	A2);
			break;
		}
	}
	IC void SetAlpha	(DWORD S, DWORD A1, DWORD OP, DWORD A2)
	{
		Set(S,D3DTSS_ALPHAOP,	OP);
		switch (OP)
		{
		case D3DTOP_DISABLE:
			break;
		case D3DTOP_SELECTARG1:
			Set(S,D3DTSS_ALPHAARG1,	A1);
			break;
		case D3DTOP_SELECTARG2:
			Set(S,D3DTSS_ALPHAARG2,	A2);
			break;
		default:
			Set(S,D3DTSS_ALPHAARG1,	A1);
			Set(S,D3DTSS_ALPHAARG2,	A2);
			break;
		}
	}
};

class ENGINE_API CSimulatorState
{
	DWORD	cache		[172];
public:
//	CSimulatorState()	{ Invalidate();	}
	IC void	Invalidate	()	{ memset(cache,0xff,sizeof(cache)); }
	IC void Set			(DWORD N, DWORD V)	
	{
		R_ASSERT(N<172);
		if (cache[N] != V)
		{
			cache[N] = V;
			HW.pDevice->SetRenderState((D3DRENDERSTATETYPE)N,V);
		}
	}
};

class ENGINE_API CSimulatorRS
{
public:
	CSimulatorTSS		TSS;
	CSimulatorState		RS;
public:
	CSimulatorRS()		{ Invalidate();	}
	IC void Invalidate	()	{ TSS.Invalidate(); RS.Invalidate(); }
	IC void SetTSS		(DWORD S, DWORD N, DWORD V) { TSS.Set(S,N,V);	}
	IC void SetRS		(DWORD N, DWORD V)			{ RS.Set(N,V);		}
};
