#include "stdafx.h"
#include "ai_bloodsucker_effector.h" 

CBloodsuckerEffector::CBloodsuckerEffector(float time) 
					 : CEffector(cefHit, time, FALSE)
{
	total = time;
}

BOOL CBloodsuckerEffector::Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	float start_fov = PI_DIV_2;
	float max_fov = 5 * PI_DIV_6;

	float factor;
	
//	if ()


	fFov = start_fov + (max_fov - start_fov) * factor;
	
	//	(max_fov - cur_fov) * (total - fLifeTime) / total;
	
	
	//fFov = cur_fov + delta_fov;
	

	return TRUE;
}


