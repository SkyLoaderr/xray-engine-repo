#pragma once

#include "entity.h"
#include "entitycondition.h"

DEFINE_VECTOR(ref_shader, SHADER_VECTOR, SHADER_VECTOR_IT);
DEFINE_VECTOR(ref_str, STR_VECTOR, STR_VECTOR_IT);

class CEntityAlive			: public CEntity, 
							  virtual public CEntityCondition
{
private:
	typedef	CEntity			inherited;			
public:
	EVENT					m_tpEventSay;
	bool					m_bMobility;
	float					m_fAccuracy;
	float					m_fIntelligence;
	//m_PhysicMovementControl
	CPHMovementControl		*m_PhysicMovementControl;
public:
	// General
							CEntityAlive			();
	virtual					~CEntityAlive			();

	// Core events
			void			Init					();
	virtual void			Load					(LPCSTR section);
	virtual void			reinit					();
	virtual void			reload					(LPCSTR section);

	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual void			net_Destroy				();

	virtual void			shedule_Update			(u32 dt);

	virtual CPHMovementControl*	movement_control()	{return m_PhysicMovementControl;}
	virtual void			HitImpulse				(float amount, Fvector& vWorldDir, Fvector& vLocalDir);
	virtual	void			Hit						(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	
	virtual void			g_WeaponBones			(int &L, int &R1, int &R2)										= 0;
	
	virtual float GetfHealth() const { return m_fHealth*100.f; }
	virtual float SetfHealth(float value) {m_fHealth = value/100.f; return value;}
	PropertyGP(GetfHealth,SetfHealth) float fEntityHealth;

	virtual float			g_Health			()	const { return GetHealth()*100.f; }
	virtual float			g_MaxHealth			()	const { return GetMaxHealth()*100.f; }

	virtual float			CalcCondition		(float hit);


	// Visibility related
	virtual	float			ffGetFov				()	const			= 0;	
	virtual	float			ffGetRange				()	const			= 0;	
	virtual	ALife::ERelationType tfGetRelationType	(CEntityAlive *tpEntityAlive)
	{
		if (tpEntityAlive->g_Team() != g_Team())
			return(ALife::eRelationTypeEnemy);
		else
			return(ALife::eRelationTypeFriend);
	};

	virtual void			BuyItem					(LPCSTR buf);
public:
IC	CPHMovementControl* PMovement()
	{
		return m_PhysicMovementControl;
	}
///////////////////////////////////////////////////////////////////////
	virtual u16				PHGetSyncItemsNumber();
	virtual CPHSynchronize*	PHGetSyncItem		(u16 item);
	virtual void			PHUnFreeze			();
	virtual void			PHFreeze			();
///////////////////////////////////////////////////////////////////////

protected:
	//���������� � ��������� �����, ���� ��� ����,
	//������������ �� ������ ���
	DEF_LIST(WOUND_LIST, CWound*);
	//������ ���, �� ������� ������������ ��������
	WOUND_LIST m_ParticlesWoundList;

	virtual void StartFireParticles(CWound* pWound);
	virtual void UpdateFireParticles();
	
	virtual void LoadFireParticles(LPCSTR section);
	//��� ��������� ����, ������� ����� ������ EntityAlive
	static STR_VECTOR m_FireParticlesVector;
	//����������� ����� �������
	static u32	 m_dwMinBurnTime;
	//������ ����, ���� ��������� ��������
	static float m_fStartBurnWoundSize;
	//������ ����, ���� ���������� ��������
	static float m_fStopBurnWoundSize;

	virtual void			BloodyWallmarks			(float P, const Fvector &dir, s16 element, const Fvector& position_in_object_space);
	virtual void			LoadBloodyWallmarks		(LPCSTR section);

	//���������� � �������� �������� �� ������, ����� ��� ���� CEntityAlive
	static SHADER_VECTOR m_BloodMarksVector;
	static float m_fBloodMarkSizeMax;
	static float m_fBloodMarkSizeMin;
	static float m_fBloodMarkDistance;
	static float m_fBloodMarkDispersion;
	static float m_fNominalHit;
};