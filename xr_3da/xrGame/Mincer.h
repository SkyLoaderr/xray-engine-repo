#pragma once
#include "customzone.h"

class CMincer :
	public CCustomZone
{
typedef	CCustomZone	inherited;
public:
	CMincer(void);
	virtual ~CMincer(void);

	virtual void Load(LPCSTR section);
	//virtual void Update(u32 dt);
	virtual void UpdateCL();
	virtual void Affect(CObject* O);

	virtual void Postprocess(f32 val);

	u32 m_time, m_pp_time;
	//float m_hitImpulseScale;
	SZonePPInfo m_pp;
};
