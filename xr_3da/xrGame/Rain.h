// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_)
#define AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_
#pragma once

class CRain  
{
	EVENT	start;
	EVENT	stop;
	BOOL	bWorking;
public:
	void	Update	();
	void	Render	();

	CRain	();
	~CRain	();
};

#endif // !defined(AFX_RAIN_H__5284C8A3_153D_4331_83F8_02165A1B8AF4__INCLUDED_)
