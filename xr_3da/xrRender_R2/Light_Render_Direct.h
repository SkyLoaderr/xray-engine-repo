#pragma once

const u32							DSM_size		= 1024;
const float							DSM_distance	= 50.f;
const float							DSM_d_range		= 100.f;

class CLight_Render_Direct
{
private:
	CRT*							rt_smap;	// 32bit, (depth)	(eye-space)
	IDirect3DSurface9*				rt_ZB;

	SGeometry*						g_debug;
	Shader*							s_debug;
public:
	Fmatrix							L_view;
	Fmatrix							L_project;
public:
	void							Create			();
	void							Destroy			();
	void							compute_xf_1	();

	CLight_Render_Direct			();
	~CLight_Render_Direct			();
};
