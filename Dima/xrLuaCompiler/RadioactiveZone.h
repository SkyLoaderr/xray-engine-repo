#pragma once
#include "customzone.h"

class CRadioactiveZone : public CCustomZone
{
private:
    typedef	CCustomZone	inherited;
public:
	CRadioactiveZone(void);
	virtual ~CRadioactiveZone(void);

	virtual void Load(LPCSTR section);
	//virtual void Update(u32 dt);
	virtual void UpdateCL();
	virtual void Affect(CObject* O);

	virtual void Postprocess(f32 val);

	u32 m_time;
	u32 m_pp_time;
	float m_hitImpulseScale;
	SZonePPInfo m_pp;
	LPCSTR m_pHitEffect;
};
