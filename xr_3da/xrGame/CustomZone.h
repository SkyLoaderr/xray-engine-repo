#pragma once
#include "gameobject.h"
#include "..\feel_touch.h"
#include "actor.h"

class CCustomZone :
	public CGameObject,
	public Feel::Touch
{
typedef	CGameObject	inherited;
public:
	CCustomZone(void);
	virtual ~CCustomZone(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	virtual void		Update				(u32 dt);

	virtual void		feel_touch_new		(CObject* O);
	virtual void		feel_touch_delete	(CObject* O);
	virtual BOOL		feel_touch_contact	(CObject* O);

	// debug
//#ifdef DEBUG
	virtual void		OnRender				( );
//#endif

	f32 Power(f32 dist);

	CActor *m_pA;
	f32 m_maxPower, m_attn;
	u32 m_period, m_time;


	virtual	void Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space){}
};
