// EntityCondition.h: ����� ��������� ����� ��������
//
//////////////////////////////////////////////////////////////////////
 
#ifndef _ENTITY_CONDITION_H_
#define _ENTITY_CONDITION_H_
#pragma once

class CWound;
class NET_Packet;
class CEntityAlive;
class CLevel;

#include "hit_immunity.h"
#include "Hit.h"
#include "Level.h"

class CEntityConditionSimple
{
	float					m_fHealth;
	float					m_fHealthMax;
public:
							CEntityConditionSimple	();
	virtual					~CEntityConditionSimple	();

	IC float				GetHealth				() const			{return m_fHealth;}
	IC float 				GetMaxHealth			() const			{return m_fHealthMax;}
	IC float&				health					()					{return	m_fHealth;}
	IC float&				max_health				()					{return	m_fHealthMax;}
};

class CEntityCondition: public CEntityConditionSimple, public CHitImmunity
{
private:
	bool					m_use_limping_state;
	CEntityAlive			*m_object;

public:
							CEntityCondition		(CEntityAlive *object);
	virtual					~CEntityCondition		(void);

	virtual void			LoadCondition			(LPCSTR section);

	//serialization
	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

	IC float					GetPower				() const			{return m_fPower;}	
	IC float					GetSatiety				() const			{return m_fSatiety;}
	IC float					GetRadiation			() const			{return m_fRadiation;}
	IC float					GetPsyHealth			() const			{return m_fPsyHealth;}

	IC float 					GetCircumspection		() const			{return m_fCircumspection;}
	IC float 					GetEntityMorale			() const			{return m_fEntityMorale;}

	IC float 					GetHealthLost			() const			{return m_fHealthLost;}

	virtual bool 			IsLimping				() const;

	void 					ChangeHealth			(float value);
	void 					ChangePower				(float value);
	void 					ChangeSatiety			(float value);
	void 					ChangeRadiation			(float value);
	void 					ChangePsyHealth			(float value);
	virtual void 			ChangeAlcohol			(float value){};

	IC void					MaxPower				()					{m_fPower = m_fPowerMax;};
	IC void					SetMaxPower				(float val)			{m_fPowerMax = val; clamp(m_fPowerMax,0.1f,1.0f);};
	IC float				GetMaxPower				() const			{return m_fPowerMax;};

	void 					ChangeBleeding			(float percent);

	void 					ChangeCircumspection	(float value);
	void 					ChangeEntityMorale		(float value);

	//hit_power �������� �� 0 �� 100 (��������� �����������)
///	virtual CWound*			ConditionHit			(CObject* who, float hit_power, ALife::EHitType hit_type, u16 element = u16(-1));
	virtual CWound*			ConditionHit			(SHit* pHDS);
	//���������� ��������� � �������� �������
	virtual void			UpdateCondition			();
	void					UpdateWounds			();
	void					UpdateConditionTime		();
	IC void					SetConditionDeltaTime	(u64 DeltaTime) { m_iDeltaTime = DeltaTime; };

	
	//�������� ������ ����� �� ���� �������� ��� 
	float					BleedingSpeed			();

	CObject*				GetWhoHitLastTime		() {return m_pWho;}
	u16						GetWhoHitLastTimeID		() {return m_iWhoID;}

	CWound*					AddWound				(float hit_power, ALife::EHitType hit_type, u16 element);

	IC void 				SetCanBeHarmedState		(bool CanBeHarmed) 			{m_bCanBeHarmed = CanBeHarmed;}
	IC bool					CanBeHarmed				() const					{return OnServer() && m_bCanBeHarmed;};
	
	void					ClearWounds();
protected:
	void					UpdateHealth			();
	void					UpdatePower				();
	void					UpdateSatiety			(float k=1.0f);
	void					UpdateRadiation			(float k=1.0f);
	void					UpdatePsyHealth			(float k=1.0f);

	void					UpdateCircumspection	();
	void					UpdateEntityMorale		();


	//��������� ���� ���� � ����������� �� �������� �������
	//(������ ��� InventoryOwner)
	float					HitOutfitEffect			(float hit_power, ALife::EHitType hit_type, s16 element, float AP);
	//��������� ������ ��� � ����������� �� �������� �������
	float					HitPowerEffect			(float power_loss);
	
	//��� �������� ��������� �������� ���,
	//������������ ����� ���� ��� ������� ���
	//� �������� ������ ����� �� ����
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	WOUND_VECTOR			m_WoundVector;
	//������� ������� ���
	

	//��� �������� �� 0 �� 1			
//	float* m_fHealth;				//��������
	float m_fPower;					//����
	float m_fSatiety;				//������� (�������)
	float m_fRadiation;				//���� ������������� ���������
	float m_fPsyHealth;				//��������

	float m_fCircumspection;		//���������������	
	float m_fEntityMorale;			//������

	//������������ ��������
//	float* m_fHealthMax;
	float m_fPowerMax;
	float m_fSatietyMax;
	float m_fRadiationMax;
	float m_fPsyHealthMax;

	float m_fCircumspectionMax;
	float m_fEntityMoraleMax;

	//�������� ��������� ���������� �� ������ ����������
	float m_fDeltaHealth;
	float m_fDeltaPower;
	float m_fDeltaSatiety;
	float m_fDeltaRadiation;
	float m_fDeltaPsyHealth;

	float m_fDeltaCircumspection;
	float m_fDeltaEntityMorale;

	//�������� ��������� ������������� ���������
	//� �������
	float m_fV_Health;
	float m_fV_Power;
	float m_fV_Satiety;
	float m_fV_Radiation;
	float m_fV_PsyHealth;

	float m_fV_Circumspection;
	float m_fV_EntityMorale;



	//�������� �������������� �������� � ����
	//�� ���� �������
	float m_fV_SatietyPower;
	float m_fV_SatietyHealth;

	//����������� �������� ������� (� ��������� 0..1), ����� �������� 
	//�������� ����������� ��������
	float m_fSatietyCritical;

	//����������� ��������� �������� �������� �������� �� ��������
	//(��� ������������ ���� ���������)
	float m_fV_RadiationHealth;

	//�������� ������ ����� � �������� ���� �� ������������� ���� 
	float m_fV_Bleeding;
	//�������� ��������� ����
	float m_fV_WoundIncarnation;
	float m_fMinWoundSize;
	//����, ����������� �� �� ��� ���� ������������
	bool m_bIsBleeding;


	//����� ����, ������������� �� ���������� �������� � ����
	float m_fHealthHitPart;
	float m_fPowerHitPart;



	//������ �������� �� ���������� ����
	float m_fHealthLost;
	//��� ����� ��������� ���
	CObject* m_pWho;
	u16		m_iWhoID;


	//��� ������������ ������� 
	u64 m_iLastTimeCalled;
	u64 m_iDeltaTime;
	bool m_bTimeValid;

	//��� �������� ���������� �� DamageManager
	float m_fHitBoneScale;
	float m_fWoundBoneScale;

	float m_limping_threshold;

	bool m_bCanBeHarmed;

public:
	virtual void					reinit				();
	
	IC const	u64					delta_time			() const 	{return		(m_iDeltaTime);			}
	IC const	WOUND_VECTOR&		wounds				() const	{return		(m_WoundVector);		}
	IC float&						radiation			()			{return		(m_fRadiation);			}
	IC float&						hit_bone_scale		()			{return		(m_fHitBoneScale);		}
	IC float&						wound_bone_scale	()			{return		(m_fWoundBoneScale);	}
};

#endif //_ENTITY_CONDITION_H_