// EntityCondition.h: ����� ��������� ����� ��������
//
//////////////////////////////////////////////////////////////////////
 
#ifndef _ENTITY_CONDITION_H_
#define _ENTITY_CONDITION_H_
#pragma once


#include "gameobject.h"

class CEntityCondition
{
public:
	CEntityCondition(void);
	virtual ~CEntityCondition(void);

	virtual void Load(LPCSTR section);

	float GetHealth() {return m_fHealth;}
	float GetPower() {return m_fPower;}
	float GetSatiety() {return m_fSatiety;}
	float GetRadiation() {return m_fRadiation;}

	float GetMaxHealth() {return m_fHealthMax;}
	float GetHealthLost() {return m_fHealthLost;}

	void ChangeHealth(float value);
	void ChangePower(float value);
	void ChangeSatiety(float value);
	void ChangeRadiation(float value);

	void ChangeBleeding(float percent);

	//hit_power �������� �� 0 �� 100 (��������� �����������)
	virtual void ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element = 0);
	//���������� ��������� � �������� �������
	virtual void UpdateCondition();

	//��������! ��� �����
	virtual void Sleep(float hours);
	
	virtual void GoSleep();
	virtual void Awoke();
	
	//�������� ������ ����� �� ���� �������� ��� 
	virtual float BleedingSpeed();

	virtual CObject* GetWhoHitLastTime() {return m_pWho;}

protected:
	virtual void UpdateHealth();
	virtual void UpdatePower();
	virtual void UpdateSatiety();
	virtual void UpdateRadiation();


	//��� �������� ��������� �������� ���,
	//������������ ����� ���� ��� ������� ���
	//� �������� ������ ����� �� ����
	DEFINE_MAP (s16, float, WOUND_MAP, WOUND_PAIR_IT);
	WOUND_MAP m_mWound;

	//��� �������� �� 0 �� 1			
	float m_fHealth;				//��������
	float m_fPower;					//����
	float m_fSatiety;				//������� (�������)
	float m_fRadiation;				//���� ������������� ���������

	//������������ ��������
	float m_fHealthMax;				
	float m_fPowerMax;					
	float m_fSatietyMax;				
	float m_fRadiationMax;				

	//�������� ��������� ���������� �� ������ ����������
	float m_fDeltaHealth;
	float m_fDeltaPower;
	float m_fDeltaSatiety;
	float m_fDeltaRadiation;

	//�������� ��������� ������������� ���������
	//� �������
	float m_fV_Health;
	float m_fV_Power;				
	float m_fV_Satiety;	
	float m_fV_Radiation;

	//������������ ��������� ��������� ���������� �� ����� ���
	float m_fK_SleepHealth;
	float m_fK_SleepPower;				
	float m_fK_SleepSatiety;	
	float m_fK_SleepRadiation;

	//������� ���������
	float m_fCurrentSleepHealth;
	float m_fCurrentSleepPower;
	float m_fCurrentSleepSatiety;
	float m_fCurrentSleepRadiation;


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


	//����� ����, ������������� �� ���������� �������� � ����
	float m_fHealthHitPart;
	float m_fPowerHitPart;

	//������������ �� ������� ����������� ���
	//��� ��������������� ���� �����������
	//(��� �������� �������� � ����������� ��������)
	float m_fK_Burn;
	float m_fK_Strike;
	float m_fK_Shock;
	float m_fK_Wound;
	float m_fK_Radiation;
	float m_fK_Telepatic;

	//��������� ���
	bool m_bIsSleeping;

	//������ �������� �� ���������� ����
	float m_fHealthLost;
	//��� ����� ��������� ���
	CObject* m_pWho;


	//��� ������������ ������� 
	u64 m_iLastTimeCalled;
	u64 m_iDeltaTime;
	bool m_bTimeValid;
};

#endif //_ENTITY_CONDITION_H_