// Explosive.h: ��������� ��� ������������� ��������
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5
#define EXPLODE_TIME_MAX 5000


#include "../feel_touch.h"
#include "inventory_item.h"
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

	virtual void OnH_B_Chield		() {inherited::OnH_B_Chield();}
	virtual void OnH_B_Independent	() {inherited::OnH_B_Independent();}
	virtual void OnEvent (NET_Packet& P, u16 type) ;//{inherited::OnEvent( P, type);}
	virtual	void Hit	(float P, Fvector &dir,	CObject* who, s16 element,
						 Fvector position_in_object_space, float impulse, 
						 ALife::EHitType hit_type = ALife::eHitTypeWound)	{inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);}
	
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
	u32	m_dwExplodeDuration;
	//����� ������
	u32 m_dwExplodeDurationMax;

	//////////////////////////////////////////////
	//��� ������� ��������
	float					tracerHeadSpeed;
	float					tracerTrailCoeff;
	float					tracerStartLength;
	float					tracerWidth;
	float					tracerDist;

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
	float m_lightRange;
	u32 m_lightTime;

public:
	virtual void			net_Import			(NET_Packet& P);					// import from server
	virtual void			net_Export			(NET_Packet& P);					// export to server
	virtual void			make_Interpolation	();
	virtual void			PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr			(); // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr			(); // actions & operations after phisic correction-prediction steps
	virtual void			OnH_A_Chield		();
	virtual	void			reinit				();
	virtual	void			reload				(LPCSTR section);
	virtual void			create_physic_shell	();
	virtual void			activate_physic_shell();
	virtual void			setup_physic_shell	();
};
