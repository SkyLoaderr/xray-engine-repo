// EntityCondition.h: класс состояния живой сущности
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

	float GetCircumspection() {return m_fCircumspection;}
	float GetEntityMorale() {return m_fEntityMorale;}

	float GetMaxHealth() {return m_fHealthMax;}
	float GetHealthLost() {return m_fHealthLost;}

	virtual bool IsLimping();

	void ChangeHealth(float value);
	void ChangePower(float value);
	void ChangeSatiety(float value);
	void ChangeRadiation(float value);

	void ChangeBleeding(float percent);

	void ChangeCircumspection(float value);
	void ChangeEntityMorale(float value);

	//hit_power задается от 0 до 100 (сложилось исторически)
	virtual void ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element = 0);
	//обновления состояния с течением времени
	virtual void UpdateCondition();

	//временно! сон часов
	virtual void Sleep(float hours);
	
	virtual void GoSleep();
	virtual void Awoke();
	
	//скорость потери крови из всех открытых ран 
	virtual float BleedingSpeed();

	virtual CObject* GetWhoHitLastTime() {return m_pWho;}

protected:
	virtual void UpdateHealth();
	virtual void UpdatePower();
	virtual void UpdateSatiety();
	virtual void UpdateRadiation();

	virtual void UpdateCircumspection();
	virtual void UpdateEntityMorale();

	//изменение силы хита в зависимости от надетого костюма
	//(только для InventoryOwner)
	virtual float HitOutfitEffect(float hit_power, ALife::EHitType hit_type);


	//для подсчета состояния открытых ран,
	//запоминается кость куда был нанесен хит
	//и скорость потери крови из раны
	DEFINE_MAP (s16, float, WOUND_MAP, WOUND_PAIR_IT);
	WOUND_MAP m_mWound;

	//все величины от 0 до 1			
	float m_fHealth;				//здоровье
	float m_fPower;					//сила
	float m_fSatiety;				//сытость (энергия)
	float m_fRadiation;				//доза радиактивного облучения

	float m_fCircumspection;		//настороженность	
	float m_fEntityMorale;			//мораль

	//максимальные величины
	float m_fHealthMax;				
	float m_fPowerMax;					
	float m_fSatietyMax;				
	float m_fRadiationMax;				

	float m_fCircumspectionMax;		
	float m_fEntityMoraleMax;				

	//величины изменения параметров на каждом обновлении
	float m_fDeltaHealth;
	float m_fDeltaPower;
	float m_fDeltaSatiety;
	float m_fDeltaRadiation;

	float m_fDeltaCircumspection;
	float m_fDeltaEntityMorale;

	//скорости изменения характеристик состояния
	//в секунду
	float m_fV_Health;
	float m_fV_Power;				
	float m_fV_Satiety;	
	float m_fV_Radiation;

	float m_fV_Circumspection;		
	float m_fV_EntityMorale;

	//коэффициенты скоростей изменения параметров во время сна
	float m_fK_SleepHealth;
	float m_fK_SleepPower;				
	float m_fK_SleepSatiety;	
	float m_fK_SleepRadiation;

	//текущие состояния
	float m_fCurrentSleepHealth;
	float m_fCurrentSleepPower;
	float m_fCurrentSleepSatiety;
	float m_fCurrentSleepRadiation;


	//скорости восстановления здоровья и силы
	//за счет сытости
	float m_fV_SatietyPower;
	float m_fV_SatietyHealth;

	//критическое значение сытости (в процентах 0..1), после которого 
	//начинает уменьшаться здоровье
	float m_fSatietyCritical;

	//максимально возможная скорость убывания здоровья от радиации
	//(при максимальной дозе облучения)
	float m_fV_RadiationHealth;

	//скорость потери крови в открытой ране от максимального хита 
	float m_fV_Bleeding;
	//скорость заживания раны
	float m_fV_WoundIncarnation;


	//части хита, затрачиваемые на уменьшение здоровья и силы
	float m_fHealthHitPart;
	float m_fPowerHitPart;

	//коэффициенты на которые домножается хит
	//при соответствующем типе воздействия
	//(для защитных костюмов и специфичных животных)
	float m_fK_Burn;
	float m_fK_Strike;
	float m_fK_Shock;
	float m_fK_Wound;
	float m_fK_Radiation;
	float m_fK_Telepatic;

	//состояние сна
	bool m_bIsSleeping;

	//потеря здоровья от последнего хита
	float m_fHealthLost;
	//кто нанес последний хит
	CObject* m_pWho;


	//для отслеживания времени 
	u64 m_iLastTimeCalled;
	u64 m_iDeltaTime;
	bool m_bTimeValid;
};

#endif //_ENTITY_CONDITION_H_