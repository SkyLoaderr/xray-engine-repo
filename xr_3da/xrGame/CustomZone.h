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
	virtual void Load(LPCSTR section);
	virtual void net_Destroy();
	virtual void Update(u32 dt);
	virtual void Affect(CObject* O) {}

	virtual void feel_touch_new(CObject* O);
	virtual void feel_touch_delete(CObject* O);
	virtual BOOL feel_touch_contact(CObject* O);


	// debug
//#ifdef DEBUG
	virtual void		OnRender				( );
//#endif

	f32 Power(f32 dist);

	set<CObject*> m_inZone;
	f32 m_maxPower, m_attn;
	u32 m_period;
	BOOL m_ready;


	//virtual	void Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space){}
};
