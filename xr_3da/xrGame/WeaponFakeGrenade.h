// WeaponFakeGrenade.h: объект для эмулирования подствольной гранаты
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include "gameobject.h"
#include "explosive.h"

class CWeaponMagazinedWGrenade;


class CWeaponFakeGrenade :	public CExplosive
{
private:
	//основное наследование от CGameObject
	typedef CGameObject inherited;
public:
	friend CWeaponMagazinedWGrenade;

	CWeaponFakeGrenade(void);
	virtual ~CWeaponFakeGrenade(void);


	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	virtual BOOL UsedAI_Locations	()	{return	(FALSE);}

	void Explode(const Fvector &pos, const Fvector &normal);
	void Destroy();

	virtual void renderable_Render() {inherited::renderable_Render();}
	
protected:
	Fvector				m_pos, m_vel;
	CGameObject*		m_pOwner;

	enum EState{
		stInactive,
		stEngine,
		stFlying,
		stExplode,
		stDestroying
	};

	EState	m_state;
	
	int	m_engineTime, m_explodeTime, m_flashTime;
	float m_mass, m_engine_f, m_engine_u;
	

	//высота подлета гранаты при взрыве
	float				m_fJumpHeight;

	static void __stdcall ObjectContactCallback(bool& do_colide,dContact& c);
};

