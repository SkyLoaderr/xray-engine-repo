// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_)
#define AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_
#pragma once

class CEffect_Rain	: public CEventBase
{
private:
	struct	Item
	{
		Fvector	P;
		Fvector D;
		float	fSpeed;
		float	fLifetime;
	};
	enum	States
	{
		stIdle		= 0,
		stStarting,
		stWorking,
		stStopping
	};
private:
	EVENT			control_start;
	EVENT			control_stop;
	vector<Item>	items;
	States			state;

	// Sounds
	int				snd_Ambient;
	C2DSound*		snd_Ambient_control;
	float			snd_Ambient_volume;

	void			Born		(Item& dest, float radius, float height);
	void			Hit			(Fvector& pos);
public:
	virtual void	OnEvent		(EVENT E, DWORD P1, DWORD P2);

	void			Render		();

	CEffect_Rain	();
	~CEffect_Rain	();
};

#endif // !defined(AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_)
