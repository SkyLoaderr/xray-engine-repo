#pragma once

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
