#pragma once

#include "entity.h"
#include "entitycondition.h"

DEFINE_VECTOR(ref_shader, SHADER_VECTOR, SHADER_VECTOR_IT);
DEFINE_VECTOR(shared_str, STR_VECTOR, STR_VECTOR_IT);

class CEntityAlive			: public CEntity, 
							  virtual public CEntityCondition
{
private:
	typedef	CEntity			inherited;			
public:
	virtual CEntityAlive*				cast_entity_alive		()						{return this;}
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
			void			init					();
	virtual void			Load					(LPCSTR section);
	virtual void			reinit					();
	virtual void			reload					(LPCSTR section);

	//object serialization
	virtual void			save				(NET_Packet &output_packet);
	virtual void			load				(IReader &input_packet);


	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual void			net_Destroy				();
	virtual	BOOL			net_SaveRelevant		();

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
	virtual	ALife::ERelationType tfGetRelationType	(const CEntityAlive *tpEntityAlive) const;
	
//	virtual void			BuyItem					(LPCSTR buf);
	virtual bool			human_being				() const
	{
		return				(false);
	}
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
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	//������ ���, �� ������� ������������ ��������
	WOUND_VECTOR m_ParticleWounds;


	virtual void StartFireParticles(CWound* pWound);
	virtual void UpdateFireParticles();
    	
	virtual void LoadFireParticles(LPCSTR section);
public:	
	static  void UnloadFireParticles	();
protected:
	//��� ��������� ����, ������� ����� ������ EntityAlive
	static STR_VECTOR* m_pFireParticlesVector;
	//����������� ����� �������
	static u32	 m_dwMinBurnTime;
	//������ ����, ���� ��������� ��������
	static float m_fStartBurnWoundSize;
	//������ ����, ���� ���������� ��������
	static float m_fStopBurnWoundSize;


	virtual void			BloodyWallmarks			(float P, const Fvector &dir, s16 element, const Fvector& position_in_object_space);
	static  void			LoadBloodyWallmarks		(LPCSTR section);
public:	
	static  void			UnloadBloodyWallmarks	();
protected:
	virtual void			PlaceBloodWallmark		(const Fvector& dir, const Fvector& start_pos, 
														float trace_dist, float wallmark_size,
														SHADER_VECTOR& wallmarks_vector);

	//���������� � �������� �������� �� ������, ����� ��� ���� CEntityAlive
	static SHADER_VECTOR* m_pBloodMarksVector;
	static float m_fBloodMarkSizeMax;
	static float m_fBloodMarkSizeMin;
	static float m_fBloodMarkDistance;
	static float m_fNominalHit;

	//��������� ������ �����
	static SHADER_VECTOR* m_pBloodDropsVector;
	//������ ��� � ������� ������ �����
	
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	WOUND_VECTOR m_BloodWounds;
	//������ ����, ���� ������ ������ �����
	static float m_fStartBloodWoundSize;
	//������ ����, ���� ���������� �����
	static float m_fStopBloodWoundSize;
	static float m_fBloodDropSize;

	//���������� ���, � ��������� ������� �� �������� �����
	virtual void			StartBloodDrops			(CWound* pWound);
	virtual void			UpdateBloodDrops		();

};