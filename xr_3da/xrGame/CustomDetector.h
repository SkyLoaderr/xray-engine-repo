#pragma once
#include "inventory.h"
#include "..\feel_touch.h"
#include "customzone.h"

class CCustomDetector :
	public CInventoryItem,
	public Feel::Touch
{
typedef	CInventoryItem	inherited;
public:
	CCustomDetector(void);
	virtual ~CCustomDetector(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();

	virtual void feel_touch_new		(CObject* O);
	virtual void feel_touch_delete	(CObject* O);
	virtual BOOL feel_touch_contact	(CObject* O);

	void SoundCreate				(sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy				(sound& dest);

	FLOAT m_radius, m_buzzer_radius;
	xr_list<CCustomZone*>		m_zones;
	xr_map<CLASS_ID, sound*>	m_sounds;
	xr_map<CCustomZone*, u32>	m_times;
	sound m_noise, m_buzzer;

	virtual void		OnRender				( );
};
