#ifndef _INCDEF_TempObject_H_
#define _INCDEF_TempObject_H_

#include "xrSheduler.h"

class ENGINE_API FBasicVisual;

class ENGINE_API CTempObject:public CSheduled{
protected:
	FBasicVisual*		m_pVisual;
	int					m_iLifeTime;
	BOOL				m_bAutoRemove;
public:
						CTempObject		();
	virtual				~CTempObject	();

	IC BOOL				Alive			(){return m_iLifeTime>0;}
	IC BOOL				IsAutomatic		(){return m_bAutoRemove;}
	IC FBasicVisual*	Visual			(){return m_pVisual;}
	IC void				SetLifeTime		(float life_time){m_iLifeTime=iFloor(life_time*1000);}
	
	virtual void		Update			(DWORD dt);
	virtual BOOL		Ready			() {return TRUE;}
};
#endif /*_INCDEF_TempObject_H_*/

