//////////////////////////////////////////////////////////////////////
// ShootingObject.h: ��������� ��� ��������� ���������� �������� 
//					 (������ � ���������� �������) 	
//					 ������������ ����� �����, ������ ��������
//////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "ShootingHitEffector.h"

class CCartridge;
#define WEAPON_MATERIAL_NAME "objects\\bullet"



class CShootingObject
{
private:
	typedef CGameObject inherited;
protected: //���� ������ ���� ������� �� ������
	CShootingObject(void);
	virtual ~CShootingObject(void);

	void	reinit	();
	void	reload	(LPCSTR section) {};
	void	Load	(LPCSTR section);

	//������� �������� ���� � �������� ��� �������� 
	//������������ ��� ������������� ���� ��� RayPick
	float		m_fCurrentFireDist;
	float		m_fCurrentHitPower;
	float		m_fCurrentHitImpulse;
	float		m_fCurrentWallmarkSize;
	Fvector		m_vCurrentShootDir;
	Fvector		m_vCurrentShootPos;
	//ID ��������� ������� ���������� ��������
	u16			m_iCurrentParentID;
	//��� ���������� ����
	ALife::EHitType m_eCurrentHitType;


//////////////////////////////////////////////////////////////////////////
// Fire Params
//////////////////////////////////////////////////////////////////////////
protected:
	virtual void			LoadFireParams		(LPCSTR section, LPCSTR prefix);
	virtual void			FireBullet			(const Fvector& pos, 
        										const Fvector& dir, 
												float fire_disp,
												const CCartridge& cartridge,
												u16 parent_id,
												u16 weapon_id);

	virtual void			FireStart			();
	virtual void			FireEnd				();
public:
	IC BOOL					IsWorking			()	const	{return bWorking;}

protected:
	// Weapon fires now
	bool					bWorking;

	float					fTimeToFire;
	int						iHitPower;
	float					fHitImpulse;

	//�������� ������ ���� �� ������
	float					m_fStartBulletSpeed;
	//������������ ���������� ��������
	float					fireDistance;

	//����������� �� ����� ��������
	float					fireDispersionBase;

	//��������
	float					tracerHeadSpeed;
	float					tracerTrailCoeff;
	float					tracerStartLength;
	float					tracerWidth;
	u32						tracerFrame;


	//������� �������, �������������� �� �������
	float			fTime;

protected:
	//��� ���������, ���� ��� ����� ����������� ������� ������������� 
	//������
	float					m_fMinRadius;
	float					m_fMaxRadius;


//////////////////////////////////////////////////////////////////////////
// Lights
//////////////////////////////////////////////////////////////////////////
protected:
	Fcolor					light_base_color;
	float					light_base_range;
	Fcolor					light_build_color;
	float					light_build_range;
	IRender_Light*			light_render;
	float					light_var_color;
	float					light_var_range;
	float					light_lifetime;
	u32						light_frame;
	float					light_time;
	//��������� ��������� �� ����� ��������
	bool					m_bShotLight;
protected:
	void					Light_Start			();
	void					Light_Render		(const Fvector& P);

	virtual	void			LoadLights			(LPCSTR section, LPCSTR prefix);
	virtual void			RenderLight();
	virtual void			UpdateLight();
	
//////////////////////////////////////////////////////////////////////////
// ����������� �������
//////////////////////////////////////////////////////////////////////////
protected:
	//������� ������������� �������
	virtual const Fmatrix&	ParticlesXFORM()	 const = 0;
	virtual IRender_Sector*	Sector() = 0;
	virtual const Fvector&	CurrentFirePoint()	= 0;
	

	////////////////////////////////////////////////
	//����� ������� ��� ������ � ���������� ������
	virtual void			StartParticles		(CParticlesObject*& pParticles, LPCSTR particles_name, const Fvector& pos, const Fvector& vel = zero_vel, bool auto_remove_flag = false);
	virtual void			StopParticles		(CParticlesObject*& pParticles);
	virtual void			UpdateParticles		(CParticlesObject*& pParticles, const Fvector& pos, const  Fvector& vel = zero_vel);

	virtual	void			LoadShellParticles	(LPCSTR section, LPCSTR prefix);
	virtual	void			LoadFlameParticles	(LPCSTR section, LPCSTR prefix);
	
	////////////////////////////////////////////////
	//������������� ������� ��� ���������
	//�������� ����
	virtual void			StartFlameParticles	();
	virtual void			StopFlameParticles	();
	virtual void			UpdateFlameParticles();

	//�������� ����
	virtual void			StartSmokeParticles	(const Fvector& play_pos,
												 const Fvector& parent_vel);
	//�������� �����
	virtual void			OnShellDrop			(const Fvector& play_pos,
												 const Fvector& parent_vel);
protected:
	//��� ��������� ��� �����
	ref_str				m_sShellParticles;
	Fvector				vShellPoint;

protected:
	//��� ��������� ��� ����
	ref_str				m_sFlameParticlesCurrent;
	//��� �������� 1� � 2� ����� ��������
	ref_str				m_sFlameParticles;
	//������ ��������� ����
	CParticlesObject*	m_pFlameParticles;

	//��� ��������� ��� ����
	ref_str				m_sSmokeParticlesCurrent;
	ref_str				m_sSmokeParticles;

};