//////////////////////////////////////////////////////////////////////
// SleepEffector.h:		��������, ������� ����������� �� ����� ���
//						������                           
//////////////////////////////////////////////////////////////////////


#pragma once

#include "../effectorPP.h"
#include "../effector.h"


//////////////////////////////////////////////////////////////////////////
// CSleepEffector
//////////////////////////////////////////////////////////////////////////
class CSleepEffectorPP : public CEffectorPP {
	typedef CEffectorPP inherited;	

	SPPInfo state;			// current state
	float	m_total;		// total PP time
	float	m_attack;		// attack time in percents	[0..1]
	float	m_release;		// release time in percents	[0..1]

public:
	CSleepEffectorPP	(const SPPInfo &ppi, float life_time, float attack_time = 0.0f, float release_time = 0.0f);
	virtual	BOOL	Process					(SPPInfo& pp);

	//������, ������� ���������� ���������, ��� �� ��� ����
	//� ������� ������� ���������� �� ����������
	bool	m_bSleeping;

	//��������������� ���������� � true �� ������ ���������
	//������
	bool	m_bFallAsleep;
};


struct SSleepEffector {
	SPPInfo	ppi;
	float	time;
	float	time_attack;
	float	time_release;
};