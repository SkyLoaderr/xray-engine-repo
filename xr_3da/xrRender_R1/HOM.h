// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class occTri;

class CHOM  
{
private:
	CDB::MODEL*				m_pModel;
	occTri*					m_pTris;
	vector<u32>				m_ZB;

	/*
	Shader*					h_Shader;
	SGeometry*				h_Geom;

	IDirect3DTexture9*		dbg_surf;
	CTexture*				dbg_texture;
	Shader*					dbg_shader;
	SGeometry*				dbg_geom;
	*/
	
	void					Render_DB	(CFrustum& base);
public:
	void					Load		();
	void					Unload		();
	void					Render		(CFrustum& base);
	void					Render_ZB	();
	void					Debug		();

	BOOL					visible		(vis_data& vis);
	BOOL					visible		(Fbox& B);
	BOOL					visible		(sPoly& P);
	
	CHOM();
	~CHOM();
};
