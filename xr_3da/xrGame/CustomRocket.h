//////////////////////////////////////////////////////////////////////
// CustomRocket.h:	������, ������� �������� RocketLauncher 
//					(����� ������, ��������� � ���������� ��������)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "physic_item.h"
#include "PHObject.h"

class CRocketLauncher;

struct SRoketContact
{
	bool contact;
	Fvector pos;
	Fvector up;
	SRoketContact()
	{
		contact = false;
	}
};

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
	virtual BOOL net_Spawn(CSE_Abstract* DC);
	virtual void net_Destroy();

	virtual void reinit		();
	virtual void reload		(LPCSTR section);

	virtual void OnH_A_Independent();
	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	virtual BOOL UsedAI_Locations	()			{return	(FALSE);}
	virtual bool Useful				() const	{return false;	}

	virtual void renderable_Render() {inherited::renderable_Render();}

	//�������� ���������� ��������
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
	//��������� �� ��������� RocketLauncher - ������� �������� ������
	CGameObject*			m_pOwner;

	SRoketContact			m_contact;
	//��������� ������� �������� RocketLauncher-�� ����� ������
	Fmatrix					m_LaunchXForm;
	Fvector					m_vLaunchVelocity;
	Fvector					m_vLaunchAngularVelocity;

	enum ERocketState{
		eInactive,		//������ ��������� � ���������� � ���������
		eEngine,		//������� ���������
		eFlying,		//������ �����
		eCollide		//��������� ������������ � ������������
	};

	//������� ��������� ������
	ERocketState m_eState;

	//��������� ������������
	bool		m_bEnginePresent;
	//����� ������ ��������� � ������� ������
	int			m_dwEngineWorkTime;
	//���� ������ ��������� (������ �������� � �������)
	float		m_fEngineImpulse;
	float		m_fEngineImpulseUp;
	//������� ����� ������ ���������
	int			m_dwEngineTime;

	//��������� ������������
	virtual void			Contact(const Fvector &pos, const Fvector &normal);
			 void			PlayContact();
	static	void __stdcall	ObjectContactCallback(bool& do_colide,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/);


	//////////////////////////////////////////////////////////////////////////
	//	Lights
	//////////////////////////////////////////////////////////////////////////
protected:
	//����, ��� ��������� ����� ���� ��������
	bool			m_bLightsEnabled;
	//����, ��� ��������� ����� �����������
	//������ � ����������
	bool			m_bStopLightsWithEngine;
	//��������� �� ����� ������ � ������ ���������
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
	//��� ��������� ���������
	shared_str			m_sEngineParticles;
	CParticlesObject*	m_pEngineParticles;
	//��� ��������� ������
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