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
	EVENT			control;
	vector<Item>	items;
	BOOL			bWorking;
public:
	void			Render		();

	CEffect_Rain	();
	~CEffect_Rain	();
};

#endif // !defined(AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_)
