//////////////////////////////////////////////////////////////////////
// ShootingObject.h: интерфейс для семейства стреляющих объектов 
//					 (оружие и осколочные гранаты) 	
//					 обеспечивает набор хитов, звуков рикошетп
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ShootingHitEffector.h"
#include "alife_space.h"

class CCartridge;
class CParticlesObject;
extern const Fvector zero_vel;

#define WEAPON_MATERIAL_NAME "objects\\bullet"

class CShootingObject
{
protected:
	CShootingObject(void);
	virtual ~CShootingObject(void);

	void	reinit	();
	void	reload	(LPCSTR section) {};
	void	Load	(LPCSTR section);

	//текущие значения хита и импульса для выстрела 
	//используются для пробиваемости стен при RayPick
	float		m_fCurrentFireDist;
	float		m_fCurrentHitPower;
	float		m_fCurrentHitImpulse;
	float		m_fCurrentWallmarkSize;
	Fvector		m_vCurrentShootDir;
	Fvector		m_vCurrentShootPos;
	//ID персонажа который иницировал действие
	u16			m_iCurrentParentID;
	//тип наносимого хита
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

	//скорость вылета пули из ствола
	float					m_fStartBulletSpeed;
	//максимальное расстояние стрельбы
	float					fireDistance;

	//рассеивание во время стрельбы
	float					fireDispersionBase;

	//счетчик времени, затрачиваемого на выстрел
	float			fTime;

protected:
	//для сталкеров, чтоб они знали эффективные границы использования 
	//оружия
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
	//включение подсветки во время выстрела
	bool					m_bShotLight;
protected:
	void					Light_Create		();
	void					Light_Destroy		();

	void					Light_Start			();
	void					Light_Render		(const Fvector& P);

	virtual	void			LoadLights			(LPCSTR section, LPCSTR prefix);
	virtual void			RenderLight			();
	virtual void			UpdateLight			();
	virtual void			StopLight			();
	
//////////////////////////////////////////////////////////////////////////
// партикловая система
//////////////////////////////////////////////////////////////////////////
protected:
	//функции родительского объекта
	virtual const Fmatrix&	ParticlesXFORM()	 const = 0;
	virtual IRender_Sector*	Sector() = 0;
	virtual const Fvector&	CurrentFirePoint()	= 0;
	

	////////////////////////////////////////////////
	//общие функции для работы с партиклами оружия
	virtual void			StartParticles		(CParticlesObject*& pParticles, LPCSTR particles_name, const Fvector& pos, const Fvector& vel = zero_vel, bool auto_remove_flag = false);
	virtual void			StopParticles		(CParticlesObject*& pParticles);
	virtual void			UpdateParticles		(CParticlesObject*& pParticles, const Fvector& pos, const  Fvector& vel = zero_vel);

	virtual	void			LoadShellParticles	(LPCSTR section, LPCSTR prefix);
	virtual	void			LoadFlameParticles	(LPCSTR section, LPCSTR prefix);
	
	////////////////////////////////////////////////
	//спецефические функции для партиклов
	//партиклы огня
	virtual void			StartFlameParticles	();
	virtual void			StopFlameParticles	();
	virtual void			UpdateFlameParticles();

	//партиклы дыма
	virtual void			StartSmokeParticles	(const Fvector& play_pos,
												 const Fvector& parent_vel);

	//партиклы полосы от пули
	virtual void			StartShotParticles	();

	//партиклы гильз
	virtual void			OnShellDrop			(const Fvector& play_pos,
												 const Fvector& parent_vel);
protected:
	//имя пратиклов для гильз
	shared_str				m_sShellParticles;
	Fvector					vShellPoint;

protected:
	//имя пратиклов для огня
	shared_str				m_sFlameParticlesCurrent;
	//для выстрела 1м и 2м видом стрельбы
	shared_str				m_sFlameParticles;
	//объект партиклов огня
	CParticlesObject*		m_pFlameParticles;

	//имя пратиклов для дыма
	shared_str				m_sSmokeParticlesCurrent;
	shared_str				m_sSmokeParticles;
	
	//имя партиклов следа от пули
	shared_str				m_sShotParticles;
};
