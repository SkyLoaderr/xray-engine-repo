#pragma once
#include "gameobject.h"
#include "..\feel_touch.h"
#include "actor.h"

struct SZonePPInfo {
	f32 duality_h, duality_v, blur, gray, noise, noise_scale;
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
	//virtual void Update(u32 dt);
	virtual void UpdateCL();
	virtual void Affect(CObject* O) {}

	virtual void feel_touch_new(CObject* O);
	virtual void feel_touch_delete(CObject* O);
	virtual BOOL feel_touch_contact(CObject* O);

	virtual void Postprocess(f32 val) {}

	void SoundCreate(ref_sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(ref_sound& dest);


	// debug
//#ifdef DEBUG
	virtual void		OnRender				( );
//#endif

	f32 Power(f32 dist);

	xr_set<CObject*> m_inZone;
	CActor *m_pLocalActor;
	f32 m_maxPower, m_attn;
	u32 m_period;
	BOOL m_ready;
	ref_sound m_ambient;

	char m_effectsSTR[255];
	xr_vector<LPCSTR> m_effects;
	xr_list<CPGObject*> m_effectsPSs;

	//virtual	void Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space){}
};
