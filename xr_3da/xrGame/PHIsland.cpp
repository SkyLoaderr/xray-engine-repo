#include "stdafx.h"
#include "PHIsland.h"
void	CPHIsland::	Step(dReal step)
{
	if(!m_flags.is_active()) return;
	//dWorldStepFast1	(DWorld(),	fixed_step,	phIterations/*+Random.randI(0,phIterationCycle)*/);
	if(m_flags.is_exact_integration_prefeared() && nj	<	max_joint_allowed_for_exeact_integration)
		dWorldStep(DWorld(),fixed_step);
	else
		dWorldQuickStep		(DWorld(),	fixed_step);
	//dWorldStep(DWorld(),fixed_step);
}
