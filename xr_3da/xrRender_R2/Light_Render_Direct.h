#pragma once

class CLight_Render_Direct
{
private:
	CRT*							rt_smap;	// 32bit, (depth)	(eye-space)
	IDirect3DSurface9*				rt_ZB;

	SGeometry*						g_debug;
	Shader*							s_debug;
public:
	void							Create		();
	void							Destroy		();

	CLight_Render_Direct			();
	~CLight_Render_Direct			();
};
