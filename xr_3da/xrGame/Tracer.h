// Tracer.h: interface for the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACER_H__AC263474_060A_489E_B84A_E879CD630A1E__INCLUDED_)
#define AFX_TRACER_H__AC263474_060A_489E_B84A_E879CD630A1E__INCLUDED_
#pragma once

class CBulletManager;

class CTracer
{
	friend CBulletManager;
public:
	CTracer();
	~CTracer();

	void	Render	(FVF::V*&verts,  
					const Fvector& center, const Fvector& dir,
					float length, float width);

protected:
	ref_shader			sh_Tracer;
	ref_geom			sh_Geom;
};

#endif // !defined(AFX_TRACER_H__AC263474_060A_489E_B84A_E879CD630A1E__INCLUDED_)
