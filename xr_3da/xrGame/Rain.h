// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_)
#define AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_
#pragma once

class CEffect_Rain	: public CEventBase, public pureDeviceDestroy, public pureDeviceCreate
{
private:
	struct	Item
	{
		Fvector	P;
		Fvector D;
		float	fSpeed;
		float	fLifetime;
	};
	struct	Particle
	{
		F
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
	EVENT			control_start;
	EVENT			control_stop;

	// Visualization
	Shader*			SH;
	CVertexStream*	VS;
	
	// Data and logic
	vector<Item>	items;
	States			state;

	// Sounds
	int				snd_Ambient;
	C2DSound*		snd_Ambient_control;
	float			snd_Ambient_volume;

	void			Born			(Item& dest, float radius, float height);
	void			Hit				(Fvector& pos);
public:
	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
	virtual void	OnEvent			(EVENT E, DWORD P1, DWORD P2);

	void			Render			();

	CEffect_Rain	();
	~CEffect_Rain	();
};

#endif // !defined(AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_)
