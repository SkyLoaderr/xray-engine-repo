#include "stdafx.h"
#include "WeaponAK74.h"
#include "WeaponHUD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponAK74::CWeaponAK74() : CWeaponMagazinedWGrenade("AK74",SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
	m_weight = 1.5f;
	m_slot = 2;
}

CWeaponAK74::~CWeaponAK74()
{
}

void CWeaponAK74::Load	(LPCSTR section)
{
	CWeapon::Load		(section);
	bFlame				= FALSE;


	// Sounds
	SoundCreate			(sndShow,		"draw"    ,m_eSoundShow);
	SoundCreate			(sndHide,		"holster" ,m_eSoundHide);
	SoundCreate			(sndShot,		"shoot"   ,m_eSoundShot);
	SoundCreate			(sndEmptyClick,	"empty"   ,m_eSoundEmptyClick);
	SoundCreate			(sndReload,		"reload"  ,m_eSoundReload);
	SoundCreate			(sndRicochet[0],"ric1"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[1],"ric2"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[2],"ric3"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[3],"ric4"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[4],"ric5"    ,m_eSoundRicochet);
	SoundCreate			(sndShotG,		"shoot"   ,m_eSoundShot);
	SoundCreate			(sndReloadG,	"reload"  ,m_eSoundReload);

	MediaLOAD	();

	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		"idle");
	animGet				(mhud_reload,	"reload");
	animGet				(mhud_show,		"draw_w_gl");
	animGet				(mhud_hide,		"holster_w_gl");
	animGet				(mhud_shots,	"shoot");
	
	animGet				(mhud_idle_g,	"idle_grenade");
	animGet				(mhud_reload_g,	"reload");
	animGet				(mhud_shots_g,	"shoot_grenade");
	animGet				(mhud_switch_g,	"switch_grenade");
	animGet				(mhud_switch,	"switch_grenade_off");




	// load ammo classes SECOND (grenade_class)
	m_ammoTypes2.clear	(); 
	LPCSTR				S = pSettings->r_string(section,"grenade_class");
	if (S && S[0]) 
	{
		string128		_ammoItem;
		int				count		= _GetItemCount	(S);
		for (int it=0; it<count; it++)	
		{
			_GetItem				(S,it,_ammoItem);
			m_ammoTypes2.push_back	(_ammoItem);
		}
		m_ammoName2 = pSettings->r_string(*m_ammoTypes2[0],"inv_name_short");
	}
	else
		m_ammoName2 = 0;

	iMagazineSize2 = iMagazineSize;

}


BOOL CWeaponAK74::net_Spawn(LPVOID DC) 
{
	m_pGrenadePoint = &vLastFP;
	BOOL l_res = CWeaponMagazined::net_Spawn(DC);
	
	CKinematics* V = PKinematics(m_pHUD->Visual()); R_ASSERT(V);
	V->LL_GetBoneInstance(V->LL_BoneID("wpn_grenade")).set_callback(GrenadeCallback, this);

	CSE_ALifeObject *l_tpALifeObject = (CSE_ALifeObject*)(DC);
	m_bHideGrenade = !iAmmoElapsed;
	
	if(iAmmoElapsed && !m_pGrenade) 
	{
		CSE_Abstract*		D	= F_entity_Create("wpn_fake_missile");
		R_ASSERT			(D);
		CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT					(l_tpALifeDynamicObject);
		l_tpALifeDynamicObject->m_tNodeID	= l_tpALifeObject->m_tNodeID;
		// Fill
		strcpy				(D->s_name,"wpn_fake_missile");
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}

	bPending = false;

	return l_res;
}

