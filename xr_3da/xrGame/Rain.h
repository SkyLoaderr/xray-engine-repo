// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_)
#define AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_
#pragma once

#include "..\\render.h"

class CEffect_Rain		: public IEventReceiver
{
private:
	struct	Item
	{
		Fvector			P;
		Fvector			Phit;
		Fvector			D;
		float			fSpeed;
		float			fTime_Life;
		float			fTime_Hit;
	};
	struct	Particle
	{
		Particle		*next,*prev;
		Fmatrix			mXForm;
		Fsphere			bounds;
		float			time;
	};
	enum	States
	{
		stIdle		= 0,
		stStarting,
		stWorking,
		stStopping
	};
private:
	// Control
	EVENT							control_start;
	EVENT							control_stop;

	// Visualization	(rain)
	ref_shader						SH_Rain;
	ref_geom						hGeom_Rain;

	// Visualization	(drops)
	IRender_DetailModel*			DM_Drop;
	ref_geom						hGeom_Drops;
	
	// Data and logic
	xr_vector<Item>					items;
	States							state;

	// Particles
	xr_vector<Particle>				particle_pool;
	Particle*						particle_active;
	Particle*						particle_idle;

	// Sounds
	ref_sound							snd_Ambient;
	float							snd_Ambient_volume;

	// Utilities
	void							p_create		();
	void							p_destroy		();

	void							p_remove		(Particle* P, Particle* &LST);
	void							p_insert		(Particle* P, Particle* &LST);
	int								p_size			(Particle* LST);
	Particle*						p_allocate		();
	void							p_free			(Particle* P);

	// Some methods
	void							Born			(Item& dest, float radius, float height);
	void							RayTest			(Item& dest, float height);
	void							Hit				(Fvector& pos);
public:
	virtual void					OnDeviceCreate	();
	virtual void					OnDeviceDestroy	();
	virtual void					OnEvent			(EVENT E, u64 P1, u64 P2);

	void							Render			();

	CEffect_Rain					();
	~CEffect_Rain					();
};

#endif // !defined(AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_)
