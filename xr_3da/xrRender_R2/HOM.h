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
	BOOL					bEnabled;

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
