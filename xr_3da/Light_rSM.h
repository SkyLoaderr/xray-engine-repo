#pragma once

class CLight_rSM
{
public:
	void	compute_xf_direct	(Fmatrix& L_view, Fmatrix& L_project, Fmatrix& mView, float p_FOV, float p_A, float p_FAR );
};
