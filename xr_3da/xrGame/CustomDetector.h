#pragma once
#include "inventory_item_object.h"
#include "../feel_touch.h"
#include "customzone.h"

//�������� ���� ����
struct ZONE_TYPE
{
	//�������� ������ ����������� �����
	float		min_freq;
	float		max_freq;
	//���� ������� ��������� �� ���������� ����
	ref_sound	detect_snd;
	shared_str	zone_map_location;
};

//�������� ����, ������������ ����������
struct ZONE_INFO
{
	u32 snd_time;
	//������� ������� ������ �������
	float cur_freq;
};

class CInventoryOwner;

class CCustomDetector :
	public CInventoryItemObject,
	public Feel::Touch
{
	typedef	CInventoryItemObject	inherited;
public:
	CCustomDetector(void);
	virtual ~CCustomDetector(void);

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
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

			void TurnOn				();
			void TurnOff			();
			bool IsWorking			() {return m_bWorking;}

	virtual void OnMoveToBelt		();
	virtual void OnMoveToRuck		();

protected:
	void StopAllSounds				();
	void UpdateMapLocations			();
	void AddRemoveMapSpot			(CCustomZone* pZone, bool bAdd);

	bool m_bWorking;

	float m_fRadius;
	float m_fBuzzerRadius;

	//���� ������ ������� �����
	CActor*				m_pCurrentActor;
	CInventoryOwner*	m_pCurrentInvOwner;

	//���������� �� ������������� �����
	DEFINE_MAP(CLASS_ID, ZONE_TYPE, ZONE_TYPE_MAP, ZONE_TYPE_MAP_IT);
	ZONE_TYPE_MAP m_ZoneTypeMap;
	
	//������ ������������ ��� � ���������� � ���
	DEFINE_MAP(CCustomZone*, ZONE_INFO, ZONE_INFO_MAP, ZONE_INFO_MAP_IT);
	ZONE_INFO_MAP m_ZoneInfoMap;
	
	//�����
	ref_sound m_noise, m_buzzer;
#ifdef DEBUG
	virtual void		OnRender	();
#endif
protected:
	u32					m_ef_detector_type;

public:
	virtual u32			ef_detector_type	() const;
};
