#pragma once
#include "customzone.h"

class CMosquitoBald :
	public CCustomZone
{
typedef	CCustomZone	inherited;
public:
	CMosquitoBald(void);
	~CMosquitoBald(void);

	virtual void Load(LPCSTR section);
	virtual void Update(u32 dt);
	virtual void Affect(CObject* O);

	virtual void Postprocess(f32 val);

	u32 m_time, m_pp_time;
	float m_hitImpulseScale;
	SPPInfo m_pp;
};
