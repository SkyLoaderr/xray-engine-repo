#pragma once
#include "customzone.h"

class CObjectAnimator;

class CTorridZone :public CCustomZone
{
private:
	typedef	CCustomZone	inherited;
	CObjectAnimator		*m_animator;
public:

	virtual void UpdateCL();
	virtual void shedule_Update(u32 dt);
	BOOL net_Spawn(LPVOID DC);
/*	virtual void Load(LPCSTR section);
	virtual void Postprocess(f32 val);
	virtual bool EnableEffector() {return true;}


	virtual void Affect(CObject* O);

protected:
	virtual bool BlowoutState();
*/
};