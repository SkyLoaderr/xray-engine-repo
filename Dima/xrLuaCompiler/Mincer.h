/////////////////////////////////////////////////////
// Аномальная зона: "мясорубка"
// При попадании живого объекта в зону происходит 
// электрический разряд
// Зона восстанавливает заряд через определенное время
// (через m_period заряжается с 0 до m_maxPower)
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

	virtual void Load(LPCSTR section);
	//virtual void Update(u32 dt);
	virtual void UpdateCL();
	virtual void Affect(CObject* O);

	virtual void Postprocess(f32 val);

protected:
	float Power(float dist, float mass);		
	float Impulse(float power, float mass);


	float m_hitImpulseScale;

	u32 m_time, m_pp_time;
	//float m_hitImpulseScale;
	SZonePPInfo m_pp;


	//текущий заряд зоны
	float m_fCurrentPower;
	//минимальный заряд, при котором может произойти разряд
	float m_fPowerMin;
	
	//удельный разряд, выдаваемы зоной 
	float m_fSpecificDischarge;
	
	//зона разряжается на кого-то
	bool m_bDischarging;
};
