// WeaponRPG7Grenade.h: объект для эмулирования гранаты РПГ7
//
//////////////////////////////////////////////////////////////////////


#include "WeaponFakeGrenade.h"

class CWeaponRPG7;

class CWeaponRPG7Grenade : public CWeaponFakeGrenade
{
private:
	typedef CWeaponFakeGrenade inherited;
public:
	friend CWeaponRPG7;

	CWeaponRPG7Grenade(void);
	virtual ~CWeaponRPG7Grenade(void);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	
	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	virtual void Explode(const Fvector &pos, const Fvector &normal);
	
	virtual BOOL UsedAI_Locations	() {return	(FALSE);}
	
	virtual void renderable_Render() {inherited::renderable_Render();}

	static void __stdcall ObjectContactCallback(bool& do_colide,dContact& c);

protected:
	void	 StopEngine			();

	Fvector					m_pos, m_vel;
	CGameObject*			m_pOwner;
	
	xr_vector<ref_str>			m_effects;
	xr_vector<ref_str>			m_trailEffects;
	xr_list<CParticlesObject*>	m_trailEffectsPSs;

	//подсветка во время работы двигателя
	IRender_Light*	m_pTrailLight;
	Fcolor			m_TrailLightColor;
	float			m_fTrailLightRange;
public:
	virtual void activate_physic_shell	();
};