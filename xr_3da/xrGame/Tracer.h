// Tracer.h: interface for the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACER_H__AC263474_060A_489E_B84A_E879CD630A1E__INCLUDED_)
#define AFX_TRACER_H__AC263474_060A_489E_B84A_E879CD630A1E__INCLUDED_
#pragma once

class CTracer  
{
	struct Bullet
	{
		Fvector target;
		Fvector	dir;
		Fvector	pos_head;
		Fvector	pos_trail;
		float	speed_head;
		float	speed_trail;
		float	width;

		float	life_time;
	};

protected:
	Shader*				sh_Tracer;
	CVertexStream*		VS;

	vector<Bullet>		bullets;
public:
	void		Add		(const Fvector& from, const Fvector& to, float bullet_speed, float trail_speed_factor, float start_length, float width);
	void		Render	();

	CTracer();
	~CTracer();
};

#endif // !defined(AFX_TRACER_H__AC263474_060A_489E_B84A_E879CD630A1E__INCLUDED_)
