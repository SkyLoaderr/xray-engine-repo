///////////////////////////////////////////////////////////////
// BastArtifact.h
// BastArtifact - �������� �������
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"
#include "../feel_touch.h"

#include "entity_alive.h"


DEFINE_VECTOR (CEntityAlive*, ALIVE_LIST, ALIVE_LIST_it);


class CBastArtifact : public CArtifact,
					  public Feel::Touch
{
private:
	typedef CArtifact inherited;
public:
	CBastArtifact(void);
	virtual ~CBastArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();
	virtual void shedule_Update		(u32 dt);
	virtual BOOL net_Spawn			(LPVOID DC);

	virtual	void Hit				(float P, Fvector &dir,	
										CObject* who, s16 element,
										Fvector position_in_object_space, 
										float impulse, 
										ALife::EHitType hit_type = eHitTypeWound);

	virtual bool Useful();


	virtual void feel_touch_new	(CObject* O);
	virtual void feel_touch_delete	(CObject* O);
	virtual BOOL feel_touch_contact	(CObject* O);

	bool IsAttacking() {return NULL!=m_AttakingEntity;}

protected:
	static void __stdcall ObjectContactCallback(bool& do_colide,dContact& c);
	//������������ ������� � ���������
	void BastCollision(CEntityAlive* pEntityAlive);


	//��������� ���������
	
	//��������� �������� �������� ����� ��������� 
	//�������� �������� ��������������
	float m_fImpulseThreshold;
	
	float m_fEnergy;
	float m_fEnergyMax;
	float m_fEnergyDecreasePerTime;
	ref_str	m_sParticleName;


	float m_fRadius;
	float m_fStrikeImpulse;

	//����, ���� ��� �������� ������� ��� 
	//� ������ ����� ��������� ������
	bool m_bStrike;	

	//������ ����� ������� � ���� ������������ ���������
	ALIVE_LIST m_AliveList;
	//��, ��� �� �������
	CEntityAlive* m_pHitedEntity; 
	//�� ��� �������
	CEntityAlive* m_AttakingEntity;
};