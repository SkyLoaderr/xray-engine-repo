//////////////////////////////////////////////////////////////////////////////////
//	Module			:	MPC functions											//
//	File			:	mpc.cpp													//
//	Creation date	:	22.01.2002												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "misc.h"
#include "mpc.h"

TVOMPC taMPC[STAGE_COUNT][HIGH_MPC_PLY];
TVOMPC taEC[STAGE_COUNT][HIGH_EC_PLY];

f64 PERCENTILE = THRESHOLD_90_PERCENTS;

void vfLoadMPCParameters()
{
	vfDualPrintF("Loading MPC parameters...");
		
	FILE *fTmp = fopen("patterns\\mpc.dat","rb");
	for (int i=0; i<STAGE_COUNT; i++)
		fread(taMPC[i],HIGH_MPC_PLY,sizeof(TVOMPC),fTmp);
	fclose(fTmp);

	/**
	fTmp = fopen("patterns\\ec.dat","rb");
	for ( i=0; i<STAGE_COUNT; i++)
		fread(taEC[i],HIGH_EC_PLY,sizeof(TVOMPC),fTmp);
	fclose(fTmp);
	/**/

	vfDualPrintF("completed\n  61 stage x %d depths = %.1fMb (%d bytes)\n",HIGH_MPC_PLY,sizeof(taMPC)/1048576.0,sizeof(taMPC));
}