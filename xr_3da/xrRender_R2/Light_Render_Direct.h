#pragma once

const u32							DSM_size		= 1024;
const float							DSM_distance_1	= 15.f;
const float							DSM_distance_2	= 100.f;
const float							DSM_d_range		= 100.f;

const u32							PSM_size		= 512;
const float							PSM_near_plane	= .05f;

class CLight_Render_Direct
{
public:
	Fmatrix							L_view;
	Fmatrix							L_project;
	Fmatrix							L_combine;
public:
	void							Create			();
	void							Destroy			();
	void							compute_xf_1	(u32 m_phase);
	void							compute_xf_2	(u32 m_phase);
	void							compute_xfp_1	(u32 m_phase, light* L);
	void							compute_xfp_2	(u32 m_phase, light* L);
	void							render			();

	CLight_Render_Direct			();
	~CLight_Render_Direct			();
};
