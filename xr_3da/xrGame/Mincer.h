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
#ifdef DEBUG
	virtual void OnRender					();
#endif
	virtual CTelekinesis& Telekinesis		(){return m_telekinetics;}
	virtual void SwitchZoneState			(EZoneState new_state);
	virtual void feel_touch_new				(CObject* O);
	virtual void Load						(LPCSTR section);
	virtual bool BlowoutState				();
	virtual void AffectPullDead				(CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist){}
	virtual void AffectThrow				(CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist);
	virtual void ThrowInCenter				(Fvector& C);
	virtual BOOL net_Spawn					(LPVOID DC);
	virtual void Center						(Fvector& C) const;

};