#pragma once

class	CLight_Compute_XFORM_and_VIS
{
public:
	void	compute_xf_direct	(light* L);
	BOOL	compute_xf_omni		(u32 m_phase, light* L);		// true, if intersect frustum
	void	compute_xf_spot		(light* L);		// 
	void	render				();
};
