// Explosive.h: ��������� ��� ������������� ��������
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5
#define EXPLODE_TIME_MAX 5000


#include "../feel_touch.h"
#include "inventory.h"
#include "ShootingObject.h"

class CExplosive:
	virtual public CInventoryItem,
	public CShootingObject,
    public Feel::Touch
{
private:
	typedef CInventoryItem inherited;
public:
	CExplosive(void);
	virtual ~CExplosive(void);

	virtual void Load(LPCSTR section);
	
	virtual BOOL net_Spawn(LPVOID DC) = 0;
	virtual void net_Destroy() = 0;

	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	virtual void Explode();
	virtual void Destroy()	=	0;

	void SoundCreate(ref_sound& dest, LPCSTR name, int iType, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(ref_sound& dest);

	virtual void OnH_B_Chield		() {inherited::OnH_B_Chield();}
	virtual void OnH_B_Independent	() {inherited::OnH_B_Independent();}
	virtual void OnEvent (NET_Packet& P, u16 type) {inherited::OnEvent( P, type);}
	virtual	void Hit	(float P, Fvector &dir,	CObject* who, s16 element,
						 Fvector position_in_object_space, float impulse, 
						 ALife::EHitType hit_type = eHitTypeWound)	{inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);}
	
	virtual void renderable_Render() = 0;



protected:
	//��������� ������
	float m_fBlastHit;
	float m_fBlastRadius;
	
	//��������� � ���������� ��������
	float m_fFragsRadius; 
	float m_fFragHit;
	int	  m_iFragsNum;

	//������ �������� �������� ����� �������� ������ 
	float m_fUpThrowFactor;

	//������ ���������� ��������
	xr_list<CGameObject*> m_blasted;

	//����������������� ������
	u32	m_expoldeTime;

	//////////////////////////////////////////////
	//��� ������� ��������
	float					tracerHeadSpeed;
	float					tracerTrailCoeff;
	float					tracerStartLength;
	float					tracerWidth;

	//�����
	ref_sound	sndExplode;
	ESoundTypes m_eSoundExplode;

	//������� �� ������
	float		fWallmarkSize;
	
	//������� � ���������
	ref_str m_sExplodeParticles;
	
	//���������
	IRender_Light*		m_pLight;
	Fcolor m_lightColor;
	f32 m_lightRange;
	u32 m_lightTime;
};
