#pragma once
#include "inventory_item.h"
#include "../feel_touch.h"
#include "customzone.h"

//описание типа зоны
struct ZONE_TYPE
{
	//интервал частот отыгрывания звука
	float		min_freq;
	float		max_freq;
	//звук реакции детектора на конкретную зону
	ref_sound	detect_snd;
};

//описание зоны, обнаруженной детектором
struct ZONE_INFO
{
	u32 snd_time;
	//текущая частота работы датчика
	float cur_freq;
};

class CInventoryOwner;

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

	void TurnOn() {m_bWorking = true;}
	void TurnOff() {m_bWorking = false;}
	bool IsWorking() {return m_bWorking;}

protected:
	void StopAllSounds				();

	bool m_bWorking;

	float m_fRadius;
	float m_fBuzzerRadius;

	//если хозяин текущий актер
	CActor*				m_pCurrentActor;
	CInventoryOwner*	m_pCurrentInvOwner;

	//информация об онаруживаемых зонах
	DEFINE_MAP(CLASS_ID, ZONE_TYPE, ZONE_TYPE_MAP, ZONE_TYPE_MAP_IT);
	ZONE_TYPE_MAP m_ZoneTypeMap;
	
	//список обнаруженных зон и информация о них
	DEFINE_MAP(CCustomZone*, ZONE_INFO, ZONE_INFO_MAP, ZONE_INFO_MAP_IT);
	ZONE_INFO_MAP m_ZoneInfoMap;
	
	//звуки
	ref_sound m_noise, m_buzzer;
#ifdef DEBUG
	virtual void		OnRender	();
#endif
};
