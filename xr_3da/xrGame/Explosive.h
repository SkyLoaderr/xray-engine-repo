// Explosive.h: ��������� ��� ������������� ��������
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5

#include "../feel_touch.h"
#include "inventory_item.h"
#include "ai_sounds.h"

class CExplosive : public Feel::Touch {

public:
								CExplosive(void);
	virtual						~CExplosive(void);

	virtual void 				Load(LPCSTR section);
	virtual void				Load(CInifile *ini,LPCSTR section);

	virtual void 				net_Destroy		();

	virtual void 				UpdateCL();

	virtual void 				feel_touch_new(CObject* O);

	virtual void 				Explode();
	virtual void 				ExplodeParams(const Fvector& pos, const Fvector& dir);

	static float 				ExplosionEffect(CGameObject* pExpObject,  const Fvector &expl_centre, const float expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions);


	virtual void 				OnEvent (NET_Packet& P, u16 type) ;//{inherited::OnEvent( P, type);}
	virtual void				OnAfterExplosion();
	virtual void				OnBeforeExplosion();
	virtual void 				SetCurrentParentID	(u16 parent_id) {m_iCurrentParentID = parent_id;}
	virtual void 				GenExplodeEvent (const Fvector& pos, const Fvector& normal);
	virtual void 				FindNormal(Fvector& normal);
	virtual CGameObject			*cast_game_object()=0;
protected:
	//ID ��������� ������� ���������� ��������
	u16 m_iCurrentParentID;
	
	bool	m_bReadyToExplode;
	Fvector m_vExplodePos;
	Fvector m_vExplodeDir;

	//��������� ������
	float m_fBlastHit;
	float m_fBlastHitImpulse;
	float m_fBlastRadius;
	
	//��������� � ���������� ��������
	float m_fFragsRadius; 
	float m_fFragHit;
	float m_fFragHitImpulse;
	int	  m_iFragsNum;

	//���� ��������� �����
	ALife::EHitType m_eHitTypeBlast;
	ALife::EHitType m_eHitTypeFrag;

	//������ �������� �������� ����� �������� ������ 
	float m_fUpThrowFactor;

	//������ ���������� ��������
	xr_list<CGameObject*> m_blasted;

	//������� ����������������� ������
	float m_fExplodeDuration;
	//����� ����� ������
	float m_fExplodeDurationMax;
	//���� ��������� ������
	bool  m_bExploding;

	//////////////////////////////////////////////
	//��� ������� ��������
	float					tracerHeadSpeed;
	float					tracerMaxLength;

	//�����
	ref_sound	sndExplode;
	ESoundTypes m_eSoundExplode;

	//������ ������� �� ������
	float		fWallmarkSize;
	
	//������� � ���������
	shared_str		m_sExplodeParticles;
	
	//��������� ������
	IRender_Light*		m_pLight;
	Fcolor				m_LightColor;
	float				m_fLightRange;
	float				m_fLightTime;
	
	virtual	void		StartLight	();
	virtual	void		StopLight	();

	// ��������
	struct {
		float time;
		float amplitude;	
		float period_number;
	} effector;
};
