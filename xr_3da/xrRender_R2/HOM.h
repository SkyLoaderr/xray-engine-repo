// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class occTri;

class CHOM  
{
private:
	xrXRC					xrc;
	CDB::MODEL*				m_pModel;
	occTri*					m_pTris;
	xr_vector<u32>			m_ZB;
	BOOL					bEnabled;

	/*
	ref_shader					h_Shader;
	ref_geom				h_Geom;

	IDirect3DTexture9*		dbg_surf;
	CTexture*				dbg_texture;
	ref_shader					dbg_shader;
	ref_geom				dbg_geom;
	*/
	
	void					Render_DB	(CFrustum& base);
public:
	void					Load		();
	void					Unload		();
	void					Render		(CFrustum& base);
	void					Render_ZB	();
	void					Debug		();

	void					Disable		();
	void					Enable		();

	BOOL					visible		(vis_data& vis);
	BOOL					visible		(Fbox& B);
	BOOL					visible		(sPoly& P);
	
	CHOM();
	~CHOM();
};
