#pragma once
#include "customzone.h"

class CRadioactiveZone : public CCustomZone
{
private:
	typedef	CCustomZone	inherited;
public:
	CRadioactiveZone(void);
	virtual ~CRadioactiveZone(void);

	virtual void Load			(LPCSTR section);
	virtual void Affect			(SZoneObjectInfo* O);
	virtual bool EnableEffector	()	{return true;}
protected:
	virtual bool BlowoutState	();
};
