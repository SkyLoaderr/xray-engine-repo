/////////////////////////////////////////////////////
// ���������� ����: "���������"
// ��� ��������� ������ ������� � ���� ���������� 
// ������������� ������
// ���� ��������������� ����� ����� ������������ �����
// (����� m_dwPeriod ���������� � 0 �� m_fMaxPower)
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
	shared_str	   m_torn_particles;
	ref_sound	   m_tearing_sound;
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
	virtual void AffectPullAlife			(CEntityAlive* EA,const Fvector& throw_in_dir,float dist);
	virtual void AffectThrow				(CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist);
	virtual void ThrowInCenter				(Fvector& C);
	virtual BOOL net_Spawn					(CSE_Abstract* DC);
	virtual void net_Destroy				();
	virtual void Center						(Fvector& C) const;
	virtual void OnOwnershipTake			(u16 id);

};