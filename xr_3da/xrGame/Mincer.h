/////////////////////////////////////////////////////
// Аномальная зона: "мясорубка"
// При попадании живого объекта в зону происходит 
// электрический разряд
// Зона восстанавливает заряд через определенное время
// (через m_dwPeriod заряжается с 0 до m_fMaxPower)
//
/////////////////////////////////////////////////////
#pragma once
#include "customzone.h"

class CMincer :
	public CCustomZone
{
typedef	CCustomZone	inherited;
public:
	CMincer(void);
	virtual ~CMincer(void);

	virtual BOOL net_Spawn(LPVOID DC);

	//virtual void Update(u32 dt);
	virtual void UpdateCL();
	virtual void Affect(CObject* O);

	virtual void Postprocess(f32 val);
	virtual bool EnableEffector() {return true;}

protected:
	float Power(float dist, float mass);		
	float Impulse(float power, float mass);


	float m_fHitImpulseScale;

	u32 m_dwDeltaTime;


	//текущий заряд зоны
	float m_fCurrentPower;
	//минимальный заряд, при котором может произойти разряд
	float m_fPowerMin;
	
	//удельный разряд, выдаваемы зоной 
	float m_fSpecificDischarge;
	
	//зона разряжается на кого-то
	bool m_bDischarging;
};
