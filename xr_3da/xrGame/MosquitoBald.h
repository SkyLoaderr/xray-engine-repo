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
	virtual void Postprocess(f32 val);
	virtual bool EnableEffector() {return true;}

protected:
	ref_str m_pHitEffect;

	virtual bool BlowoutState();
};
