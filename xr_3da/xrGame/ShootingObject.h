//////////////////////////////////////////////////////////////////////
// ShootingObject.h: интерфейс дл€ семейства стрел€ющих объектов 
//					 (оружие и осколочные гранаты) 	
//					 обеспечивает набор хитов, звуков рикошетп
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
protected: //чтоб нельз€ было вызвать на пр€мую
	CShootingObject(void);
	virtual ~CShootingObject(void);
public:
	//материал дл€ пули и осколков (инициализируетс€ в CWeapon::Load)
	static u16 bullet_material_idx;
protected:

	//функци€ обработки хитов объектов
	static BOOL __stdcall firetrace_callback(Collide::rq_result& result, LPVOID params);

	//попадание по динамическому объекту
	virtual void DynamicObjectHit	(Collide::rq_result& R, u16 target_material);
	//попадание по статическому объекту
	virtual void StaticObjectHit	(Collide::rq_result& R, u16 target_material);
	//ќтметка на пораженном объекте
	virtual void FireShotmark (const Fvector& vDir,	const Fvector &vEnd, Collide::rq_result& R, u16 target_material);

	//текущие значени€ хита и импульса дл€ выстрела 
	//используютс€ дл€ пробиваемости стен при RayPick
	float		m_fCurrentFireDist;
	float		m_fCurrentHitPower;
	float		m_fCurrentHitImpulse;
	float		m_fCurrentWallmarkSize;
	CCartridge*	m_pCurrentCartridge;
	Fvector		m_vCurrentShootDir;
	Fvector		m_vCurrentShootPos;
	Fvector		m_vEndPoint;
	//ID персонажа который иницировал действие
	u32			m_iCurrentParentID;
	//тип наносимого хита
	ALife::EHitType m_fCurrentHitType;
};