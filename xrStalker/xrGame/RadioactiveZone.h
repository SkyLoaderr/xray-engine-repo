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
	virtual bool EnableEffector() {return true;}

	u32 m_dwDeltaTime;
	float m_fHitImpulseScale;
	ref_str m_pHitEffect;
};
