#include "stdafx.h"
#include "PHSynchronize.h"


bool	physics_net_interpolation_mode=false;	

void	PHNetInterpolationModeON()
{
	physics_net_interpolation_mode=true;
}
void	PHNetInterpolationModeOFF()
{
	physics_net_interpolation_mode=false;
}
bool	PHNetInterpolationMode()
{
	return physics_net_interpolation_mode;
}