//////////////////////////////////////////////////////////////////////
// CustomRocket.h:	ракета, которой стреляет RocketLauncher 
//					(умеет лететь, светиться и отыгрывать партиклы)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "physic_item.h"
#include "PHObject.h"

class CRocketLauncher;


class CCustomRocket : public CPhysicItem,
					  public CPHUpdateObject
{
private:
	typedef CPhysicItem inherited;
	friend CRocketLauncher;
public:
	//////////////////////////////////////////////////////////////////////////
	//	Generic
	//////////////////////////////////////////////////////////////////////////

	CCustomRocket(void);
	virtual ~CCustomRocket(void);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void reinit		();
	virtual void reload		(LPCSTR section);

	virtual void OnH_A_Independent();
	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	virtual BOOL UsedAI_Locations	()			{return	(FALSE);}
	virtual bool Useful				() const	{return false;	}

	virtual void renderable_Render() {inherited::renderable_Render();}

	//создание физической оболочки
	virtual void			activate_physic_shell	();
	virtual void			create_physic_shell		();

	virtual void			PhDataUpdate			(float step);
	virtual void			PhTune					(float step);

	//////////////////////////////////////////////////////////////////////////
	//	Rocket Properties
	//////////////////////////////////////////////////////////////////////////
public:
	virtual	void			StartEngine				();
	virtual	void			StopEngine				();
	virtual	void			UpdateEngine			();
	virtual	void			UpdateEnginePh			();

	virtual	void			StartFlying				();
	virtual	void			StopFlying				();

	virtual	void			SetLaunchParams			(const Fmatrix& xform, 
													 const Fvector& vel,
													 const Fvector& angular_vel);

	virtual void			OnEvent					(NET_Packet& P, u16 type);
protected:
	//указатель на владельца RocketLauncher - который стреляет ракету
	CGameObject*			m_pOwner;


	//параметры которые задаются RocketLauncher-ом перед пуском
	Fmatrix					m_LaunchXForm;
	Fvector					m_vLaunchVelocity;
	Fvector					m_vLaunchAngularVelocity;

	enum ERocketState{
		eInactive,		//ракета неактивна и находиться в инвентаре
		eEngine,		//включен двигатель
		eFlying,		//просто летим
		eCollide		//произошло столкновение с препятствием
	};

	//текущее состояние ракеты
	ERocketState m_eState;

	//двигатель присутствует
	bool		m_bEnginePresent;
	//время работы двигателя с момента старта
	int			m_dwEngineWorkTime;
	//сила работы двигателя (размер импульса в секунду)
	float		m_fEngineImpulse;
	float		m_fEngineImpulseUp;
	//текущее время работы двигателя
	int			m_dwEngineTime;

	//обработка столкновения
	virtual void			Contact(const Fvector &pos, const Fvector &normal);
	static	void __stdcall	ObjectContactCallback(bool& do_colide,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/);


	//////////////////////////////////////////////////////////////////////////
	//	Lights
	//////////////////////////////////////////////////////////////////////////
protected:
	//флаг, что подсветка может быть включена
	bool			m_bLightsEnabled;
	//флаг, что подсветка будет остановлена
	//вместе с двигателем
	bool			m_bStopLightsWithEngine;
	//подсветка во время полета и работы двигателя
	IRender_Light*	m_pTrailLight;
	Fcolor			m_TrailLightColor;
	float			m_fTrailLightRange;
protected:
	virtual void	StartLights();
	virtual void	StopLights();
	virtual void	UpdateLights();
    
	
	//////////////////////////////////////////////////////////////////////////
	//	Particles
	//////////////////////////////////////////////////////////////////////////
protected:	
	//имя партиклов двигателя
	shared_str			m_sEngineParticles;
	CParticlesObject*	m_pEngineParticles;
	//имя партиклов полета
	shared_str			m_sFlyParticles;
	CParticlesObject*	m_pFlyParticles;

	Fvector				m_vPrevVel;

protected:
	virtual void		StartEngineParticles();
	virtual void		StopEngineParticles();
	virtual void		StartFlyParticles();
	virtual void		StopFlyParticles();

	virtual void		UpdateParticles();
};