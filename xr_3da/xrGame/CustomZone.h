#pragma once
#include "gameobject.h"
#include "..\feel_touch.h"
#include "actor.h"

struct SPPInfo {
	f32 blur, gray, noise, noise_scale;
	u32 r, g, b, a;
};

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

	virtual void Postprocess(f32 val) {}

	void SoundCreate(sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(sound& dest);


	// debug
//#ifdef DEBUG
	virtual void		OnRender				( );
//#endif

	f32 Power(f32 dist);

	set<CObject*> m_inZone;
	CActor *m_pLocalActor;
	f32 m_maxPower, m_attn;
	u32 m_period;
	BOOL m_ready;
	sound m_ambient;


	//virtual	void Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space){}
};
