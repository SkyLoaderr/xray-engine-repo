#pragma once
#include "mosquitobald.h"

class CObjectAnimator;

class CTorridZone :public CMosquitoBald
{
private:
	typedef	CCustomZone	inherited;
	CObjectAnimator		*m_animator;
public:
						CTorridZone			();
	virtual				~CTorridZone		();
	virtual void		UpdateCL			();
	virtual void		shedule_Update		(u32 dt);
	BOOL				net_Spawn			(LPVOID DC);


	virtual bool		IsVisibleForZones() { return true;		}
/*	virtual void Load(LPCSTR section);
	virtual void Postprocess(f32 val);
	virtual bool EnableEffector() {return true;}


	virtual void Affect(CObject* O);

protected:
	virtual bool BlowoutState();
*/
};