#include "stdafx.h"
#pragma hdrstop

#include "tss_def.h"

DWORD	SimulatorStates::record	()
{
	CHK_DX(HW.pDevice->BeginStateBlock());
	for (DWORD it=0; it<States.size(); it++)
	{
		State& S = States[it];
		switch (S.type) 
		{
		case 0:	CHK_DX(HW.pDevice->SetRenderState((D3DRENDERSTATETYPE)S.v1,S.v2));	break;
		case 1: CHK_DX(HW.pDevice->SetTextureStageState(S.v1,(D3DTEXTURESTAGESTATETYPE)S.v2,S.v3));	break;
		}
	}
	DWORD SB = 0;
	CHK_DX(HW.pDevice->EndStateBlock(&SB));
	return SB;
}
