#include "CycleConstStorage.h"
#ifndef PHINTERPOLATON_H
#define PHINTERPOLATON_H

class CPHInterpolation {

public:
CPHInterpolation();
void SetBody(dBodyID body);
static const PH_INTERPOLATION_POINTS=2;
void	InterpolatePosition	(Fvector& pos);
void	UpdatePositions		();
void	UpdateRotations		();
void	ResetPositions		();
void	ResetRotations		();
void	InterpolateRotation	(Fmatrix& rot);
private:
	dBodyID m_body;
	CCycleConstStorage<Fvector,PH_INTERPOLATION_POINTS>				qPositions;
	CCycleConstStorage<Fquaternion,PH_INTERPOLATION_POINTS>			qRotations;
	//xr_deque<double>				qTimes;
	//Fvector vUpdatePositions[PH_INTERPOLATION_POINTS];
	//float fUpdateTimes[PH_INTERPOLATION_POINTS];
	//float fTimeDelta;
};
#endif