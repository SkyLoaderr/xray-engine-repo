#pragma once

const u32							DSM_size		= 1024;
const float							DSM_distance_1	= 5.f;
const float							DSM_distance_2	= 15.f;
const float							DSM_d_range		= 100.f;

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
	void							render			();

	CLight_Render_Direct			();
	~CLight_Render_Direct			();
};
