#pragma once
#include "customzone.h"

class CMosquitoBald : public CCustomZone
{
private:
    typedef	CCustomZone	inherited;
public:
	CMosquitoBald(void);
	virtual ~CMosquitoBald(void);

	virtual void Load(LPCSTR section);
	//virtual void Update(u32 dt);
	virtual void UpdateCL();
	virtual void Postprocess(f32 val);

	u32 m_pp_time;
	SZonePPInfo m_pp;
	ref_str m_pHitEffect;
};
