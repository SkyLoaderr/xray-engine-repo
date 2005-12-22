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
#include "PhysicsShellHolder.h"
#include "clsid_game.h"
class CMincer :	
	public CBaseGraviZone,
	public CPHDestroyableNotificator
{
private:
	typedef CBaseGraviZone inherited;
	CTeleWhirlwind m_telekinetics;
	shared_str	   m_torn_particles;
	ref_sound	   m_tearing_sound;
	float		   m_fActorBlowoutRadiusPercent;
public:
	CMincer(void);
	virtual ~CMincer(void);
	virtual CTelekinesis& Telekinesis			(){return m_telekinetics;}
	//	virtual void SwitchZoneState			(EZoneState new_state);
	virtual void OnStateSwitch					(EZoneState new_state);
	virtual		BOOL	feel_touch_contact		(CObject* O);
	virtual void	feel_touch_new				(CObject* O);
	virtual void	Load						(LPCSTR section);
	virtual bool	BlowoutState				();
	virtual void	AffectPullDead				(CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist){}
	virtual void	AffectPullAlife				(CEntityAlive* EA,const Fvector& throw_in_dir,float dist);
	virtual void	AffectThrow					(SZoneObjectInfo* O, CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist);
	virtual void	ThrowInCenter				(Fvector& C);
	virtual BOOL	net_Spawn					(CSE_Abstract* DC);
	virtual void	net_Destroy					();
	virtual void	Center						(Fvector& C) const;
	virtual void	OnOwnershipTake				(u16 id);
	virtual	void	NotificateDestroy			(CPHDestroyableNotificate *dn);
	virtual float	BlowoutRadiusPercent		(CPhysicsShellHolder* GO){return GO->CLS_ID!=CLSID_OBJECT_ACTOR? m_fBlowoutRadiusPercent:m_fActorBlowoutRadiusPercent;}
};