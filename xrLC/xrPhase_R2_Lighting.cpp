#include "stdafx.h"
#include "xrHemisphere.h"

void __stdcall  hemi_callback	(float x, float y, float z, float E, LPVOID P)
{
	R_Light*	T				= (R_Light*)P;
	T->energy					= E;
	T->direction.set			(x,y,z);
	pBuild->L_hemi.push_back	(*T);
}

void CBuild::xrPhase_R2_Lights	()
{
	L_layers.clear			();
	L_hemi.clear			();

	R_Light						RL;
	RL.type                     = D3DLIGHT_DIRECTIONAL;
	RL.diffuse.set				(1,1,1,1);
	xrHemisphereBuild			(2,FALSE,0.5f,1.f,hemi_callback,&RL);
	for (u32 it=0; it<L_hemi.size(); it++)
	{
		L_hemi[it].diffuse.mul_rgba	(L_hemi[it].energy);
	}
}
