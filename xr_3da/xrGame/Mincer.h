/////////////////////////////////////////////////////
// Аномальная зона: "мясорубка"
// При попадании живого объекта в зону происходит 
// электрический разряд
// Зона восстанавливает заряд через определенное время
// (через m_dwPeriod заряжается с 0 до m_fMaxPower)
//
/////////////////////////////////////////////////////
#pragma once
#include "gravizone.h"
#include "telewhirlwind.h"

class CMincer :	public CBaseGraviZone
{
private:
	typedef CBaseGraviZone inherited;
	CTeleWhirlwind m_telekinetics;
public:
	CMincer(void);
	virtual ~CMincer(void);
	virtual CTelekinesis& Telekinesis		(){return m_telekinetics;}
	virtual void SwitchZoneState			(EZoneState new_state);
	virtual void Load						(LPCSTR section);
	virtual void Affect						(CObject* O){};
	virtual BOOL net_Spawn					(LPVOID DC);

};