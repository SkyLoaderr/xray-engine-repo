//////////////////////////////////////////////////////////////////////
// RocketLauncher.h:	интерфейс дл€ семейства объектов 
//						стрел€ющих гранатами и ракетами
//////////////////////////////////////////////////////////////////////


#pragma once

class CCustomRocket;

class CRocketLauncher
{
public:
	CRocketLauncher		();
	~CRocketLauncher	();

	virtual void Load	(LPCSTR section);

	virtual void AttachRocket	(u16 rocket_id, CGameObject* parent_rocket_launcher);
	virtual void DetachRocket	(u16 rocket_id);

	virtual void SpawnRocket	(LPCSTR rocket_section, CGameObject* parent_rocket_launcher);
	virtual void LaunchRocket	(const Fmatrix& xform,  const Fvector& vel, const Fvector& angular_vel);

protected:			   
	CCustomRocket*	m_pRocket;
	
	//начальна€ скорость, придаваема€ ракете во врем€
	//старта
	float			m_fLaunchSpeed;
};