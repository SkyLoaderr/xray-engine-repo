
#ifndef PHINTERPOLATON_H
#define PHINTERPOLATON_H

class CPHInterpolation {

public:
CPHInterpolation();
void SetBody(dBodyID body);
static const PH_INTERPOLATION_POINTS=2;
void InterpolatePosition(Fvector& pos);
void	UpdatePositions();
void	UpdateRotations();
void InterpolateRotation(Fmatrix& rot);
private:
	dBodyID m_body;
	deque<Fvector>					qPositions;
	deque<Fquaternion>				qRotations;
	//deque<double>					qTimes;
	//Fvector vUpdatePositions[PH_INTERPOLATION_POINTS];
	//float fUpdateTimes[PH_INTERPOLATION_POINTS];
	//float fTimeDelta;
};
#endif