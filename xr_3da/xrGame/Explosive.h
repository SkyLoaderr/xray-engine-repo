// Explosive.h: ��������� ��� ������������� ��������
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5
#define EXPLODE_TIME_MAX 5000


#include "../feel_touch.h"
#include "inventory.h"

class CExplosive:
	virtual public CInventoryItem,
    public Feel::Touch
{
public:
	CExplosive(void);
	virtual ~CExplosive(void);

	virtual void Load(LPCSTR section);
	
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	virtual void Explode();
	virtual void FragWallmark(const Fvector& vDir, 
							  const Fvector &vEnd, 
							  Collide::rq_result& R);

	virtual void Destroy()	=	0;

	void SoundCreate(ref_sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(ref_sound& dest);


protected:
	//��������� ������
	float m_blast;
	float m_blastR;
	
	//��������� � ���������� ��������
	float m_fragsR; 
	float m_fragHit;
	int	  m_frags;

	//������ �������� �������� ����� �������� ������ 
	float m_fUpThrowFactor;

	//������ ���������� ��������
	xr_list<CGameObject*> m_blasted;

	//����������������� ������
	u32	m_expoldeTime;

	//������� �� ������
	ref_str		pstrWallmark;
	ref_shader	hWallmark;
	float		fWallmarkSize;
	
	//������� � ���������
	xr_vector<ref_str>	m_effects;
	IRender_Light*		m_pLight;
	Fcolor m_lightColor;
	f32 m_lightRange;
	u32 m_lightTime;

	//�����
	ref_sound	sndRicochet[SND_RIC_COUNT], sndExplode, sndCheckout;
	ESoundTypes m_eSoundRicochet, m_eSoundExplode, m_eSoundCheckout;
};
