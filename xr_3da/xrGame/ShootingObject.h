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



class CShootingObject: virtual public CGameObject
{
private:
	typedef CGameObject inherited;
protected: //���� ������ ���� ������� �� ������
	CShootingObject(void);
	virtual ~CShootingObject(void);
public:
	//�������� ��� ���� � �������� (���������������� � CWeapon::Load)
	static u16 bullet_material_idx;
protected:

	//������� ��������� ����� ��������
	static BOOL __stdcall firetrace_callback(Collide::rq_result& result, LPVOID params);

	//��������� �� ������������� �������
	virtual void DynamicObjectHit	(Collide::rq_result& R, u16 target_material);
	//��������� �� ������������ �������
	virtual void StaticObjectHit	(Collide::rq_result& R, u16 target_material);
	//������� �� ���������� �������
	virtual void FireShotmark (const Fvector& vDir,	const Fvector &vEnd, Collide::rq_result& R, u16 target_material);

	//������� �������� ���� � �������� ��� �������� 
	//������������ ��� ������������� ���� ��� RayPick
	float		m_fCurrentFireDist;
	float		m_fCurrentHitPower;
	float		m_fCurrentHitImpulse;
	float		m_fCurrentWallmarkSize;
	CCartridge*	m_pCurrentCartridge;
	Fvector		m_vCurrentShootDir;
	Fvector		m_vCurrentShootPos;
	Fvector		m_vEndPoint;
	//ID ��������� ������� ���������� ��������
	u32			m_iCurrentParentID;

	void		LoadEffector	();
private:
	SShootingEffector m_effector;
};