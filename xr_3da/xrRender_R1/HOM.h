// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class occTri;

class CHOM  
#ifdef DEBUG
	: public pureRender, pureStats
#endif
{
private:
	xrXRC					xrc;
	CDB::MODEL*				m_pModel;
	occTri*					m_pTris;
	BOOL					bEnabled;
	Fmatrix					m_xform;
	Fmatrix					m_xform_01;

	void					Render_DB	(CFrustum&	base);
public:
	void					Load		();
	void					Unload		();
	void					Render		(CFrustum&	base);
	void					Render_ZB	();
//	void					Debug		();

	void					occlude		(Fbox2&		space) { }
	void					Disable		();
	void					Enable		();

	BOOL					visible		(vis_data&	vis);
	BOOL					visible		(Fbox3&		B);
	BOOL					visible		(sPoly&		P);
	BOOL					visible		(Fbox2&		B, float depth);	// viewport-space (0..1)
	
	CHOM();
	~CHOM();

#ifdef DEBUG
	virtual void			OnRender	();
	virtual void			OnStats		();
#endif
};
