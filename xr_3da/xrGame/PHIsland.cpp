#include "stdafx.h"
#include "PHIsland.h"
void	CPHIsland::	Step(dReal step)
{
	if(!b_active) return;
	//dWorldStepFast1	(DWorld(),	fixed_step,	phIterations/*+Random.randI(0,phIterationCycle)*/);
	dWorldQuickStep		(DWorld(),	fixed_step);
	//dWorldStep(DWorld(),fixed_step);
}
