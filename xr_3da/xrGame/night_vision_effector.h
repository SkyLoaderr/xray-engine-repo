#pragma once

#include "../effectorPP.h"
#include "CameraEffector.h"


class CNightVisionEffector;

class CNightVisionEffectorPP: public CEffectorPP 
{
private:
	typedef CEffectorPP inherited;	
	friend CNightVisionEffector;

private:
	SPPInfo state;			// current state
	
	float	m_attack;		// attack time in sec
	float	m_release;		// release time in sec
	float	m_total;		// release time in sec
	enum {FADE_IN, LOOP, FADE_OUT}	m_status;

public:
	CNightVisionEffectorPP		(const SPPInfo &ppi, float attack_time, float release_time);
	virtual	BOOL	Process		(SPPInfo& pp);
};



class CNightVisionEffector
{
public:
	CNightVisionEffector		();
	virtual	void	Load		(LPCSTR);
	virtual	bool	Start		();
	virtual	bool	Stop		();
private:
	SPPInfo state;
	float	m_attack;
	float	m_release;
	CNightVisionEffectorPP* p_effector;
};