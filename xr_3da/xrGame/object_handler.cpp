////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler.cpp
//	Created 	: 02.10.2001
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_handler.h"
#include "object_state_base.h"
#include "object_state_show.h"
#include "object_state_hide.h"
#include "object_state_fire_primary.h"
#include "object_state_reload.h"
#include "object_state_empty.h"
#include "object_state_switch.h"
#include "object_state_idle.h"

#include "entity_alive.h"
#include "inventoryowner.h"
#include "weapon.h"
#include "missile.h"
#include "enemy_manager.h"
#include "custommonster.h"
#include "weaponmagazined.h"
#include "ef_storage.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "fooditem.h"

CObjectHandler::CObjectHandler		() : inherited("")
{
	init						();
}

CObjectHandler::~CObjectHandler		()
{
}

void CObjectHandler::init			()
{
}

void CObjectHandler::Load			(LPCSTR section)
{
	inherited::Load					(section);
	CInventoryOwner::Load			(section);
	m_dwFireRandomMin  				= pSettings->r_s32(section,"FireRandomMin");
	m_dwFireRandomMax  				= pSettings->r_s32(section,"FireRandomMax");
	m_dwNoFireTimeMin  				= pSettings->r_s32(section,"NoFireTimeMin");
	m_dwNoFireTimeMax  				= pSettings->r_s32(section,"NoFireTimeMax");
	m_fMinMissDistance 				= pSettings->r_float(section,"MinMissDistance");
	m_fMinMissFactor   				= pSettings->r_float(section,"MinMissFactor");
	m_fMaxMissDistance 				= pSettings->r_float(section,"MaxMissDistance");
	m_fMaxMissFactor				= pSettings->r_float(section,"MaxMissFactor");
	m_bCanFire						= true;
	m_bHammerIsClutched				= false;
	m_dwNoFireTime					= 0;
	m_dwStartFireTime				= 0;
}

void CObjectHandler::reinit			(CAI_Stalker *object)
{
	inherited::reinit				(object,true);
	CInventoryOwner::reinit			();
	add_state						(xr_new<CObjectStateBase>((CInventoryItem*)0,CWeapon::EWeaponStates(-1)),u32(eObjectActionNoItems),0);
	state(eObjectActionNoItems).initialize();
	set_current_state				(eObjectActionNoItems);
	set_dest_state					(eObjectActionNoItems);
}

void CObjectHandler::reload			(LPCSTR section)
{
	inherited::reload				(section);
	CInventoryOwner::reload			(section);
}

#ifdef DEBUG
LPCSTR to_string(const u32 id)
{
	static	string4096 S;
	if ((id & 0xffff) != 0xffff)
		if (Level().Objects.net_Find(id & 0xffff))
			strcpy	(S,*Level().Objects.net_Find(id & 0xffff)->cName());
		else
			strcpy	(S,"no_items");
	else
		strcpy	(S,"no_items");
	strcat		(S,":");
	switch (id >> 16) {
		case eObjectActionIdle		 : {strcat(S,"Idle"); break;}
		case eObjectActionShow		 : {strcat(S,"Show"); break;}		
		case eObjectActionHide		 : {strcat(S,"Hide"); break;}		
		case eObjectActionTake		 : {strcat(S,"Take"); break;}		
		case eObjectActionDrop		 : {strcat(S,"Drop"); break;}		
		case eObjectActionStrapped	 : {strcat(S,"Strapped"); break;}		
		case eObjectActionAim1		 : {strcat(S,"Aim1"); break;}		
		case eObjectActionAim2		 : {strcat(S,"Aim2"); break;}		
		case eObjectActionReload1	 : {strcat(S,"Reload1"); break;}	
		case eObjectActionReload2	 : {strcat(S,"Reload2"); break;}	
		case eObjectActionFire1		 : {strcat(S,"Fire1"); break;}		
		case eObjectActionFire2		 : {strcat(S,"Fire2"); break;}		
		case eObjectActionSwitch1	 : {strcat(S,"Switch1"); break;}	
		case eObjectActionSwitch2	 : {strcat(S,"Switch2"); break;}	
		case eObjectActionActivate	 : {strcat(S,"Activate"); break;}
		case eObjectActionDeactivate : {strcat(S,"Deactivate"); break;}
		case eObjectActionMisfire1	 : {strcat(S,"Misfire1"); break;}
		case eObjectActionEmpty1	 : {strcat(S,"Empty1"); break;}
		case eObjectActionUse		 : {strcat(S,"Use"); break;}
		default						 : NODEFAULT;
	}
	return		(S);
}
#endif

bool CObjectHandler::check_if_can_kill_target(const CEntity *tpEntity, const Fvector &target_pos, float yaw2, float pitch2, float fSafetyAngle) const
{
	Fvector fire_pos = tpEntity->Position();
	fire_pos.sub(target_pos,fire_pos);
	float yaw1,pitch1;
	fire_pos.getHP(yaw1,pitch1);
	yaw1 = angle_normalize_signed(yaw1);
	pitch1 = angle_normalize_signed(pitch1);
	yaw2 = angle_normalize_signed(yaw2);
	pitch2 = angle_normalize_signed(pitch2);
	
	const CInventoryOwner *tpInventoryOwner = dynamic_cast<const CInventoryOwner*>(tpEntity);
	if (tpInventoryOwner) {
		if (!tpInventoryOwner->inventory().ActiveItem() || !dynamic_cast<CWeapon*>(tpInventoryOwner->inventory().ActiveItem()) || !dynamic_cast<CWeapon*>(tpInventoryOwner->inventory().ActiveItem())->GetAmmoElapsed())
			return(false);
		if (!((angle_difference(yaw1,yaw2) <= fSafetyAngle) && (angle_difference(pitch1,pitch2) <= fSafetyAngle)))
			return(false);

#pragma todo("Dima to Dima : Recover check if can kill target function")
//		setEnabled			(false);
//		Collide::ray_query	l_tRayQuery;
//		l_tRayQuery.O		=	NULL;
//		Fvector				l_tFireDirection;
//		l_tFireDirection.setHP(yaw1,pitch1);
//		g_pGameLevel->ObjectSpace.RayPick(tpEntity->Position(), l_tFireDirection, fire_pos.distance_to(target_pos) + .5f, l_tRayQuery);
//		setEnabled			(true);
//		return				(l_tRayQuery.O && (l_tRayQuery.O->Position().distance_to(target_pos) < EPS_L));
		return				(true);
	}
	else
		return((tpEntity->Position().distance_to(target_pos) < 2.f) && (angle_difference(yaw1,yaw2) <= PI_DIV_2));
}

bool CObjectHandler::check_if_can_kill_enemy(const SRotation &orientation) const
{
	const CMemoryManager	*memory_manager = dynamic_cast<const CMemoryManager*>(this);
	const CEntity			*entity	= dynamic_cast<const CEntity *>(this);
	VERIFY					(memory_manager);
	VERIFY					(entity);
	return			(
		memory_manager->enemy() && 
		memory_manager->enemy()->g_Alive() && 
		memory_manager->visible(memory_manager->enemy()) && 
		check_if_can_kill_target(
			entity,
			memory_manager->enemy()->Position(),
			-orientation.yaw,
			-orientation.pitch
		)
	);
}

bool CObjectHandler::check_if_can_kill_member() const
{
	bool bCanKillMember = false;
	const CAI_Stalker	*stalker = dynamic_cast<const CAI_Stalker*>(this);
	VERIFY				(stalker);
	
	for (int i=0, iTeam = (int)stalker->g_Team(); i<(int)stalker->visible_objects().size(); ++i) {
		const CEntityAlive	*entity_alive = dynamic_cast<const CEntityAlive*>(stalker->visible_objects()[i]);
		if (entity_alive && (entity_alive->g_Team() == iTeam))
			if (entity_alive->g_Alive() && check_if_can_kill_target(stalker,entity_alive->Position(),-stalker->head_orientation().current.yaw,-stalker->head_orientation().current.pitch)) {
				bCanKillMember = true;
				break;
			}
	}
	return(bCanKillMember);
}

void CObjectHandler::update(u32 time_delta)
{
//	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
//	VERIFY					(stalker);
//
//	CWeapon					*tpWeapon = dynamic_cast<CWeapon*>(stalker->inventory().ActiveItem());
//
//	if (!stalker->inventory().ActiveItem() || !tpWeapon)
//		return;
//
//	if (stalker->mental_state() == MonsterSpace::eMentalStatePanic) {
//		stalker->inventory().Activate(u32(-1));
//		return;
//	}
//
//	bool bSafeFire = m_bFiring;
//
//	u32						dwStartFireAmmo, dwFireDelayMin = 0, dwFireDelayMax = 0;
//	CWeaponMagazined		*tpWeaponMagazined = dynamic_cast<CWeaponMagazined*>(tpWeapon);
//	if (tpWeaponMagazined && stalker->enemy()) {
//		float fDistance = stalker->Position().distance_to(stalker->enemy()->Position());
//		float fDistance1 = tpWeaponMagazined->m_fMaxRadius, fDistance2 = (tpWeaponMagazined->m_fMaxRadius + 0*tpWeaponMagazined->m_fMinRadius)/2,fDistance3 = 0*tpWeaponMagazined->m_fMinRadius;
//		u32 dwFuzzyDistance;
//		if (_abs(fDistance - fDistance1) <	_abs(fDistance - fDistance2))
//			dwFuzzyDistance = 0;
//		else
//			if (_abs(fDistance - fDistance2) <	_abs(fDistance - fDistance3))
//				dwFuzzyDistance = 1;
//			else
//				dwFuzzyDistance = 2;
//		
//		u32	dwMinQueueSize = 0, dwMaxQueueSize = 0;
//		switch (stalker->movement_type()) {
//			case eMovementTypeRun : {
//				switch (dwFuzzyDistance) {
//					case 0 : {
//						dwMinQueueSize	= 1;
//						dwMaxQueueSize	= 3;
//						dwFireDelayMin	= 2000;
//						dwFireDelayMax	= 4000;
//						break;
//					}
//					case 1 : {
//						dwMinQueueSize	= 2;
//						dwMaxQueueSize	= 3;
//						dwFireDelayMin	= 1000;
//						dwFireDelayMax	= 2000;
//						break;
//					}
//					case 2 : {
//						dwMinQueueSize	= 3;
//						dwMaxQueueSize	= 7;
//						dwFireDelayMin	= 700;
//						dwFireDelayMax	= 1500;
//						break;
//					}
//					default : NODEFAULT;
//				}
//				break;
//			}
//			case eMovementTypeWalk : {
//				switch (dwFuzzyDistance) {
//					case 0 : {
//						dwMinQueueSize	= 1;
//						dwMaxQueueSize	= 3;
//						dwFireDelayMin	= 1000;
//						dwFireDelayMax	= 3000;
//						break;
//					}
//					case 1 : {
//						dwMinQueueSize	= 2;
//						dwMaxQueueSize	= 4;
//						dwFireDelayMin	= 900;
//						dwFireDelayMax	= 2000;
//						break;
//					}
//					case 2 : {
//						dwMinQueueSize	= 5;
//						dwMaxQueueSize	= 10;
//						dwFireDelayMin	= 700;
//						dwFireDelayMax	= 1200;
//						break;
//					}
//					default : NODEFAULT;
//				}
//				break;
//			}
//			case eMovementTypeStand : {
//				switch (dwFuzzyDistance) {
//					case 0 : {
//						dwMinQueueSize	= 1;
//						dwMaxQueueSize	= 3;
//						dwFireDelayMin	= 1500;
//						dwFireDelayMax	= 2500;
//						break;
//					}
//					case 1 : {
//						dwMinQueueSize	= 3;
//						dwMaxQueueSize	= 5;
//						dwFireDelayMin	= 1000;
//						dwFireDelayMax	= 1800;
//						break;
//					}
//					case 2 : {
//						dwMinQueueSize	= 10;
//						dwMaxQueueSize	= 15;
//						dwFireDelayMin	= 500;
//						dwFireDelayMax	= 1000;
//						break;
//					}
//					default : NODEFAULT;
//				}
//				break;
//			}
//			default : {
//				stalker->inventory().Action(kWPN_FIRE, CMD_STOP);
//				stalker->inventory().ActiveItem()->Deactivate();
//				dwMaxQueueSize			= 0;
//				break;
//			}
//		}
//		if (dwMaxQueueSize)
//			if (tpWeaponMagazined->GetAmmoMagSize() > 1)
//				tpWeaponMagazined->SetQueueSize(::Random.randI(iFloor(float(dwMinQueueSize)/_min(30,tpWeaponMagazined->GetAmmoMagSize())*float(tpWeaponMagazined->GetAmmoMagSize())),iFloor(float(dwMaxQueueSize)/_min(30,tpWeaponMagazined->GetAmmoMagSize())*float(tpWeaponMagazined->GetAmmoMagSize()))));
//			else
//				tpWeaponMagazined->SetQueueSize(1);
//	}
//
////	Msg	("Time : %d\nDESIRED : %s\nREAL : %s\nFIRING : %d",Level().timeServer(),(weapon_state == eObjectActionIdle) ? "IDLE" : "FIRE",(tpWeapon->STATE == CWeapon::eFire) ? "FIRING" : ((tpWeapon->STATE == CWeapon::eIdle) ? "IDLE" : ((tpWeapon->STATE == CWeapon::eReload) ? "RELOAD" : "UNKNOWN")),int(m_bFiring));
//	if (weapon_state == eObjectActionIdle) {
//		m_bFiring = false;
//		if (tpWeapon->STATE == CWeapon::eFire)
//			stalker->inventory().Action(kWPN_FIRE, CMD_STOP);
//		xr_vector<CInventorySlot>::iterator I = stalker->inventory().m_slots.begin(), B = I;
//		xr_vector<CInventorySlot>::iterator E = stalker->inventory().m_slots.end() - 2;
//		s32 best_slot = -1;
//		for ( ; I != E; ++I)
//			if ((*I).m_pIItem && ((I - B) != (int)stalker->inventory().GetActiveSlot()) && (!dynamic_cast<CWeaponMagazined*>((*I).m_pIItem) || dynamic_cast<CWeaponMagazined*>((*I).m_pIItem)->IsAmmoAvailable()))
//				best_slot = u32(I - B);
//		if (best_slot > (int)stalker->inventory().GetActiveSlot())
//			stalker->inventory().Activate(best_slot);
//	}
//	else
//		if (weapon_state == eObjectActionFire1)
//			if (tpWeapon->STATE == CWeapon::eFire)
//				if (check_if_can_kill_enemy(stalker->head_orientation().current) && !check_if_can_kill_member()) {
//					u32 dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
//					if (dwStartFireAmmo) {
//						m_bFiring = true;
//						m_dwNoFireTime = Level().timeServer() + ::Random.randI(dwFireDelayMin,dwFireDelayMax);
//					}
//					else {
//						stalker->inventory().Action(kWPN_FIRE, CMD_STOP);
//						m_bFiring = false;
//					}
//				}
//				else {
//					stalker->inventory().Action(kWPN_FIRE, CMD_STOP);
//					m_bFiring = false;
//					m_dwNoFireTime = Level().timeServer();
//				}
//			else
//				if (tpWeapon->STATE == CWeapon::eIdle)
//					if (check_if_can_kill_enemy(stalker->head_orientation().current) && !check_if_can_kill_member()) {
//						dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
//						if (dwStartFireAmmo && (Level().timeServer() > m_dwNoFireTime)) {
//							if (m_bFiring && (Level().timeServer() - m_dwStartFireTime > TIME_FIRE_DELTA)) {
//								stalker->inventory().Action(kWPN_FIRE, CMD_START);
////								Msg				("Weapon could fire (%d)",Level().timeServer());
//							}
//							else {
//								m_dwNoFireTime = Level().timeServer();
//								stalker->inventory().Action(kWPN_FIRE, CMD_STOP);
//							}
//							if (!m_bFiring) {
//								m_dwStartFireTime	= Level().timeServer();
////								Msg				("Weapon shouldn't fire (%d)",Level().timeServer());
//							}
//							m_bFiring = true;
//						}
//					}
//					else
//						m_bFiring = false;
//				else
//					m_bFiring = false;
//		else {
//			if (tpWeapon->STATE == CWeapon::eFire)
//				stalker->inventory().Action(kWPN_FIRE,	CMD_STOP);
//			m_bFiring = false;
//		}
//			
//	if (tpWeaponMagazined) {
//		dwStartFireAmmo = tpWeaponMagazined->GetAmmoElapsed();
//		if ((!dwStartFireAmmo) && (CWeapon::eReload != tpWeapon->STATE))
//			if (tpWeaponMagazined->IsAmmoAvailable())
//				stalker->inventory().Action(kWPN_RELOAD,	CMD_START);
//			else
//				if ((CWeapon::eHidden != tpWeapon->STATE) && (CWeapon::eHiding != tpWeapon->STATE)) {
//					if (tpWeaponMagazined->STATE == CWeapon::eFire)
//						stalker->inventory().Action(kWPN_FIRE,	CMD_STOP);
//					xr_vector<CInventorySlot>::iterator I = stalker->inventory().m_slots.begin(), B = I;
//					xr_vector<CInventorySlot>::iterator E = stalker->inventory().m_slots.end() - 2;
//					u32 best_slot = u32(-1);
//					for ( ; I != E; ++I)
//						if ((*I).m_pIItem && ((I - B) != (int)stalker->inventory().GetActiveSlot()) && (!dynamic_cast<CWeaponMagazined*>((*I).m_pIItem) || dynamic_cast<CWeaponMagazined*>((*I).m_pIItem)->IsAmmoAvailable()))
//							best_slot = u32(I - B);
//					if (-1 != best_slot)														   
//						stalker->inventory().Activate(best_slot);
////					else {
////						stalker->inventory().Ruck(tpWeaponMagazined);
////						tpWeaponMagazined->Drop();
////					}
//				}
//	}
//
//	bool bFiring = (tpWeapon->STATE == CWeapon::eFire) || (tpWeapon->STATE == CWeapon::eFire2);
//
//	if (tpWeapon && (tpWeapon->STATE != CWeapon::eIdle)) {
//		CStalkerMovementManager	*stalker_movement_manager = dynamic_cast<CStalkerMovementManager*>(this);
//		if (stalker_movement_manager && (stalker_movement_manager->mental_state() == eMentalStateFree))
//			stalker_movement_manager->set_mental_state(eMentalStateDanger);
//	}
//
//	CGroup &Group = *stalker->getGroup();
//	if ((bSafeFire) && (!bFiring))
//		--(Group.m_dwFiring);
//	else
//		if ((!bSafeFire) && (bFiring))
//			++Group.m_dwFiring;
	
//		dwStartFireAmmo = 
//		if ((!dwStartFireAmmo) && (CWeapon::eReload != tpWeapon->STATE))
//			if (tpWeaponMagazined->IsAmmoAvailable())
//				stalker->inventory().Action(kWPN_RELOAD,	CMD_START);
	set_current_state	(object_state());
	inherited::update	(time_delta);

#ifdef _DEBUG
//	if (!path().empty()) {
//		Msg				("Path : ");
//		xr_vector<u32>::const_iterator	I = path().begin();
//		xr_vector<u32>::const_iterator	E = path().end();
//		for ( ; I != E; ++I)
//			Msg			("%s",to_string(*I));
//	}
//	else
//		Msg			("Path : %s",to_string(current_state_id()));
#endif
}

u32 CObjectHandler::weapon_state(const CWeapon *weapon) const
{
	if (!weapon)
		return		(u32(-1));

	switch (weapon->STATE) {
		case CWeapon::eIdle		:  {
			const CWeaponMagazined	*weapon_magazined = dynamic_cast<const CWeaponMagazined*>(weapon);
			if (!weapon_magazined || (weapon_magazined->GetAmmoElapsed() && !weapon_magazined->IsMisfire()))
				return(CWeapon::eIdle);
			if (!weapon_magazined->GetAmmoElapsed() && (current_state_state_id() != eObjectActionReload1))
				return(CWeapon::eMagEmpty);
			if (weapon_magazined->IsMisfire() && (current_state_state_id() != eObjectActionReload1))
				return(CWeapon::eMisfire);
			
			switch (current_state_state_id()) {
				case eObjectActionIdle		: return(CWeapon::eIdle		);
				case eObjectActionFire1		: return(CWeapon::eFire		);
				case eObjectActionFire2		: return(CWeapon::eFire2	);
				case eObjectActionReload1	: return(CWeapon::eReload	);
				case eObjectActionShow		: return(CWeapon::eShowing	);
				case eObjectActionHide		: return(CWeapon::eHiding	);
			}
		}
		default : 
			return(weapon->STATE);
	}
}

u32 CObjectHandler::object_state() const
{
	if (!inventory().ActiveItem() || !inventory().ActiveItem()->H_Parent() || inventory().ActiveItem()->getDestroy())
		return			(eObjectActionNoItems);

	CWeapon				*weapon = dynamic_cast<CWeapon*>(inventory().ActiveItem());
	if (weapon) {
		if (weapon->ID() != current_object_state_id()) {
			switch (weapon_state(weapon)) {
				case CWeapon::eIdle		: return(uid(eObjectActionIdle,weapon->ID()));
				case CWeapon::eFire		: return(uid(eObjectActionFire1,weapon->ID()));
				case CWeapon::eFire2	: return(uid(eObjectActionFire2,weapon->ID()));
				case CWeapon::eReload	: return(uid(eObjectActionReload1,weapon->ID()));
				case CWeapon::eShowing	: return(uid(eObjectActionShow,weapon->ID()));
				case CWeapon::eHiding	: return(uid(eObjectActionHide,weapon->ID()));
				case CWeapon::eMisfire	: return(uid(eObjectActionMisfire1,weapon->ID()));
				case CWeapon::eMagEmpty : return(uid(eObjectActionEmpty1,weapon->ID()));
				case CWeapon::eHidden	: return(eObjectActionNoItems);
				default			: NODEFAULT;
			}
		}
		switch (weapon_state(weapon)) {
			case CWeapon::eIdle : {
				switch (current_state_state_id()) {
					case eObjectActionIdle :
					case eObjectActionAim1 :
					case eObjectActionAim2 :
					case eObjectActionSwitch1 :
					case eObjectActionSwitch2 : 
						return	(current_state_id());
					default :
						if (!weapon->IsWorking())
							return	(uid(eObjectActionIdle,weapon->ID()));
						else
							return	(current_state_id());
				}
			}
			case CWeapon::eFire : {
				switch (current_state_state_id()) {
					case eObjectActionFire1 :
						return	(current_state_id());
					default : 
						return	(uid(eObjectActionFire1,weapon->ID()));
				}
			}
			case CWeapon::eFire2 : {
				switch (current_state_state_id()) {
					case eObjectActionFire2 :
						return	(current_state_id());
					default : 
						return	(uid(eObjectActionFire2,weapon->ID()));
				}
			}
			case CWeapon::eReload : {
				switch (current_state_state_id()) {
					case eObjectActionReload1 :
					case eObjectActionReload2 :
						return	(current_state_id());
					default : 
						return	(uid(eObjectActionReload1,weapon->ID()));
				}
			}
			case CWeapon::eShowing : {
				switch (current_state_state_id()) {
					case eObjectActionShow :
						return	(current_state_id());
					default : 
						return	(uid(eObjectActionShow,weapon->ID()));
				}
			}
			case CWeapon::eHiding : {
				switch (current_state_state_id()) {
					case eObjectActionHide :
						return	(current_state_id());
					default : 
						return	(uid(eObjectActionHide,weapon->ID()));
				}
			}
			case CWeapon::eMisfire : {
				switch (current_state_state_id()) {
					case eObjectActionMisfire1 :
						return	(current_state_id());
					default : 
						return	(uid(eObjectActionMisfire1,weapon->ID()));
				}
			}
			case CWeapon::eMagEmpty : {
				switch (current_state_state_id()) {
					case eObjectActionEmpty1 :
						return	(current_state_id());
					default : 
						return	(uid(eObjectActionEmpty1,weapon->ID()));
				}
			}
			case CWeapon::eHidden :
				return	(current_state_id());
//				return			(eObjectActionNoItems);
			default : NODEFAULT;
		}
	}

	CMissile		*missile = dynamic_cast<CMissile*>(inventory().ActiveItem());
	if (missile) {
//		if (missile->ID() != current_object_state_id()) {
			switch (missile->State()) {
				case MS_HIDDEN	 : return(eObjectActionNoItems);
				case MS_SHOWING	 : return(uid(eObjectActionShow,missile->ID()));
				case MS_HIDING	 : return(uid(eObjectActionHide,missile->ID()));
				case MS_IDLE	 : return(uid(eObjectActionIdle,missile->ID()));
				case MS_EMPTY	 : return(uid(eObjectActionEmpty1,missile->ID()));
				case MS_THREATEN : return(uid(eObjectActionFire2,missile->ID()));
				case MS_READY	 : return(uid(eObjectActionSwitch1,missile->ID()));
				case MS_THROW	 : return(uid(eObjectActionFire1,missile->ID()));
				case MS_END		 : return(uid(eObjectActionFire1,missile->ID()));
				case MS_PLAYING	 : return(uid(eObjectActionIdle,missile->ID()));
				default			 : NODEFAULT;
			}
//		}
//		switch (missile->State()) {
//			case MS_HIDDEN	 : {
//				switch (current_state_state_id()) {
//					case eObjectActionIdle :
//					case eObjectActionAim1 :
//					case eObjectActionAim2 :
//					case eObjectActionSwitch1 :
//					case eObjectActionSwitch2 : 
//						return	(current_state_id());
//					default :
//						if (!weapon->IsWorking())
//							return	(uid(eObjectActionIdle,weapon->ID()));
//						else
//							return	(current_state_id());
//				}
//				break;
//			}
//			case MS_SHOWING	 : return(uid(eObjectActionShow,missile->ID()));
//			case MS_HIDING	 : return(uid(eObjectActionHide,missile->ID()));
//			case MS_IDLE	 : return(uid(eObjectActionIdle,missile->ID()));
//			case MS_EMPTY	 : return(uid(eObjectActionEmpty1,missile->ID()));
//			case MS_THREATEN : return(uid(eObjectActionFire2,missile->ID()));
//			case MS_READY	 : return(uid(eObjectActionSwitch1,missile->ID()));
//			case MS_THROW	 : return(uid(eObjectActionFire1,missile->ID()));
//			case MS_END		 : return(uid(eObjectActionFire1,missile->ID()));
//			case MS_PLAYING	 : return(uid(eObjectActionIdle,missile->ID()));
//			default			 : NODEFAULT;
//		}
	}

	CFoodItem		*food_item = dynamic_cast<CFoodItem*>(inventory().ActiveItem());
	if (food_item) {
		switch (food_item->STATE) {
			case FOOD_HIDDEN : return(eObjectActionNoItems);
			case FOOD_SHOWING: return(uid(eObjectActionShow,food_item->ID()));
			case FOOD_HIDING : return(uid(eObjectActionHide,food_item->ID()));
			case FOOD_IDLE	 : return(uid(eObjectActionIdle,food_item->ID()));
			case FOOD_PREPARE: return(uid(eObjectActionSwitch1,food_item->ID()));
			case FOOD_EATING : return(uid(eObjectActionFire1,food_item->ID()));
			default			 : NODEFAULT;
		}
	}
	
	NODEFAULT;

#ifdef DEBUG
	return			(u32(eObjectActionDummy));
#endif
}

void CObjectHandler::add_item			(CInventoryItem *inventory_item)
{
	u32					id = inventory_item->ID();
	add_state			(xr_new<CObjectStateBase>(inventory_item,CWeapon::eHiding),	uid(eObjectActionDrop,id),		0);
	
	CWeapon				*weapon = dynamic_cast<CWeapon*>(inventory_item);
	CMissile			*missile = dynamic_cast<CMissile*>(inventory_item);
	CEatableItem		*eatable = dynamic_cast<CEatableItem*>(inventory_item);

	if (weapon) {
		add_state		(xr_new<CObjectStateBase>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionIdle,id),		0);
		add_state		(xr_new<CObjectStateShow>(inventory_item,CWeapon::eShowing),	uid(eObjectActionShow,id),		0);
		add_state		(xr_new<CObjectStateHide>(inventory_item,CWeapon::eHiding),		uid(eObjectActionHide,id),		0);
		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionStrapped,id),	0);
		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionAim1,id),		0);
		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionAim2,id),		0);
		add_state		(xr_new<CObjectStateReload>(inventory_item,CWeapon::eReload),	uid(eObjectActionReload1,id),	0);
		add_state		(xr_new<CObjectStateReload>(inventory_item,CWeapon::eReload),	uid(eObjectActionReload2,id),	0);
		add_state		(xr_new<CObjectStateFirePrimary>(inventory_item,CWeapon::eFire),uid(eObjectActionFire1,id),		0);
		add_state		(xr_new<CObjectStateBase>(inventory_item,CWeapon::eFire2),		uid(eObjectActionFire2,id),		0);
		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionSwitch1,id),	0);
		add_state		(xr_new<CObjectStateIdle>(inventory_item,CWeapon::eIdle,true),	uid(eObjectActionSwitch2,id),	0);
		add_state		(xr_new<CObjectStateEmpty>(inventory_item,CWeapon::eFire),		uid(eObjectActionMisfire1,id),	0);
		add_state		(xr_new<CObjectStateEmpty>(inventory_item,CWeapon::eFire),		uid(eObjectActionEmpty1,id),	0);

		add_transition	(uid(eObjectActionShow,id),			uid(eObjectActionIdle,id),		1,		1);
		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionHide,id),		1,		1);
		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionDrop,id),		1);
		add_transition	(uid(eObjectActionStrapped,id),		uid(eObjectActionIdle,id),		1,		1);
		add_transition	(uid(eObjectActionAim1,id),			uid(eObjectActionIdle,id),		1,		1);
		add_transition	(uid(eObjectActionAim2,id),			uid(eObjectActionIdle,id),		1,		1);
		add_transition	(uid(eObjectActionReload1,id),		uid(eObjectActionIdle,id),		1,		1);
		add_transition	(uid(eObjectActionReload2,id),		uid(eObjectActionIdle,id),		1,		1);
		add_transition	(uid(eObjectActionFire1,id),		uid(eObjectActionIdle,id),		1);
		add_transition	(uid(eObjectActionFire2,id),		uid(eObjectActionIdle,id),		1);
		add_transition	(uid(eObjectActionSwitch1,id),		uid(eObjectActionIdle,id),		1);
		add_transition	(uid(eObjectActionSwitch2,id),		uid(eObjectActionIdle,id),		1);
		add_transition	(uid(eObjectActionReload1,id),		uid(eObjectActionAim1,id),		1,		1);
		add_transition	(uid(eObjectActionReload2,id),		uid(eObjectActionAim1,id),		1,		1);
		add_transition	(uid(eObjectActionReload1,id),		uid(eObjectActionAim2,id),		1,		1);
		add_transition	(uid(eObjectActionReload2,id),		uid(eObjectActionAim2,id),		1,		1);
		add_transition	(uid(eObjectActionAim1,id),			uid(eObjectActionAim2,id),		1,		1);
		add_transition	(uid(eObjectActionFire1,id),		uid(eObjectActionAim1,id),		1,		1);
		add_transition	(uid(eObjectActionSwitch1,id),		uid(eObjectActionAim1,id),		1);
		add_transition	(uid(eObjectActionAim1,id),			uid(eObjectActionSwitch2,id),	1);
		add_transition	(uid(eObjectActionFire2,id),		uid(eObjectActionAim2,id),		1,		1);
		add_transition	(uid(eObjectActionSwitch2,id),		uid(eObjectActionAim2,id),		1);
		add_transition	(uid(eObjectActionAim2,id),			uid(eObjectActionSwitch1,id),	1);
		add_transition	(uid(eObjectActionMisfire1,id),		uid(eObjectActionReload1,id),	1);
		add_transition	(uid(eObjectActionMisfire1,id),		uid(eObjectActionHide,id),		10);
		add_transition	(uid(eObjectActionEmpty1,id),		uid(eObjectActionReload1,id),	1);
		add_transition	(uid(eObjectActionEmpty1,id),		uid(eObjectActionHide,id),		10);

		state(uid(eObjectActionIdle,id)).set_inertia_time(0);
		state(uid(eObjectActionAim1,id)).set_inertia_time(1000);
		state(uid(eObjectActionAim2,id)).set_inertia_time(1000);
	}
	else if (missile) {
		add_state		(xr_new<CObjectStateIdle>(inventory_item,MS_IDLE,true),			uid(eObjectActionIdle,id),		0);
//		add_state		(xr_new<CObjectStateShow>(inventory_item,MS_SHOWING),			uid(eObjectActionShow,id),		0);
		add_state		(xr_new<CObjectStateShow>(inventory_item,MS_IDLE),				uid(eObjectActionShow,id),		0);
		add_state		(xr_new<CObjectStateHide>(inventory_item,MS_HIDING),			uid(eObjectActionHide,id),		0);
		add_state		(xr_new<CObjectStateFirePrimary>(inventory_item,MS_THREATEN),	uid(eObjectActionFire2,id),		0);
		add_state		(xr_new<CObjectStateSwitch>(inventory_item,MS_READY,true),		uid(eObjectActionSwitch1,id),	0);
		add_state		(xr_new<CObjectStateBase>(inventory_item,MS_END),				uid(eObjectActionFire1,id),		0);

		add_transition	(uid(eObjectActionShow,id),			uid(eObjectActionIdle,id),		1,		1);
		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionHide,id),		1,		1);
		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionDrop,id),		1);
		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionFire2,id),		1);
		add_transition	(uid(eObjectActionFire2,id),		uid(eObjectActionSwitch1,id),	1);
		add_transition	(uid(eObjectActionSwitch1,id),		uid(eObjectActionFire1,id),		1);
		add_transition	(uid(eObjectActionFire1,id),		u32(eObjectActionNoItems),		1);

		state(uid(eObjectActionSwitch1,id)).set_inertia_time(100);
		state(uid(eObjectActionFire1,id)).set_inertia_time(1000);
	}
	else if (eatable) {
		add_state		(xr_new<CObjectStateIdle>(inventory_item,FOOD_IDLE,true),		uid(eObjectActionIdle,id),		0);
		add_state		(xr_new<CObjectStateShow>(inventory_item,FOOD_IDLE),			uid(eObjectActionShow,id),		0);
		add_state		(xr_new<CObjectStateHide>(inventory_item,FOOD_HIDING),			uid(eObjectActionHide,id),		0);
		add_state		(xr_new<CObjectStateSwitch>(inventory_item,FOOD_PREPARE,true),	uid(eObjectActionSwitch1,id),	0);
		add_state		(xr_new<CObjectStateBase>(inventory_item,FOOD_EATING),			uid(eObjectActionFire1,id),		0);

		add_transition	(uid(eObjectActionShow,id),			uid(eObjectActionIdle,id),		1,		1);
		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionHide,id),		1,		1);
		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionDrop,id),		1);
		add_transition	(uid(eObjectActionIdle,id),			uid(eObjectActionSwitch1,id),	1,		1);
		add_transition	(uid(eObjectActionSwitch1,id),		uid(eObjectActionFire1,id),		1);
		add_transition	(uid(eObjectActionFire1,id),		u32(eObjectActionNoItems),		1);
	}

	// нож, приборы

	//
	if (graph().vertex(uid(eObjectActionHide,inventory_item->ID())))
		add_transition		(uid(eObjectActionHide,inventory_item->ID()),u32(eObjectActionNoItems),1.f);
	if (graph().vertex(uid(eObjectActionShow,inventory_item->ID())))
		add_transition		(u32(eObjectActionNoItems),uid(eObjectActionShow,inventory_item->ID()),1.f);
}

void CObjectHandler::remove_item		(CInventoryItem *inventory_item)
{
	u32				id = inventory_item->ID();
	remove_state	(uid(eObjectActionDrop,			id));

	CWeapon			*weapon = dynamic_cast<CWeapon*>(inventory_item);
	CMissile		*missile = dynamic_cast<CMissile*>(inventory_item);

	if (weapon) {
		remove_state	(uid(eObjectActionIdle,id));
		remove_state	(uid(eObjectActionShow,id));
		remove_state	(uid(eObjectActionHide,id));
		remove_state	(uid(eObjectActionStrapped,id));
		remove_state	(uid(eObjectActionAim1,id));
		remove_state	(uid(eObjectActionAim2,id));
		remove_state	(uid(eObjectActionReload1,id));
		remove_state	(uid(eObjectActionReload2,id));
		remove_state	(uid(eObjectActionFire1,id));
		remove_state	(uid(eObjectActionFire2,id));
		remove_state	(uid(eObjectActionSwitch1,id));
		remove_state	(uid(eObjectActionSwitch2,id));
		return;
	}

	if (missile) {
		remove_state	(uid(eObjectActionIdle,id));
		remove_state	(uid(eObjectActionShow,id));
		remove_state	(uid(eObjectActionHide,id));
		remove_state	(uid(eObjectActionFire1,id));
		remove_state	(uid(eObjectActionSwitch1,id));
		remove_state	(uid(eObjectActionFire2,id));
		return;
	}
}

#ifdef DEBUG
void CObjectHandler::show_graph()
{
//	Msg						("\nGraph dump (%d vertices, %d edges)",graph().vertices().size(),graph().edge_count());
//	state_iterator			I = graph().vertices().begin(), B = I;
//	state_iterator			E = graph().vertices().end();
//	for ( ;I != E; ++I) {
//		string4096			S;
//		char				*S1 = S;
//		if ((*I).edges().empty())
//			S1				+= sprintf(S1,"%32s -> %32s",to_string((*I).vertex_id()),"(no edges)");
//		else
//			for (u32 i=0; i<(*I).edges().size(); ++i) {
//				S1				+= sprintf(S1,"%32s -> ",to_string((*I).vertex_id()));
//				S1				+= sprintf(S1,"%32s%s",to_string(graph().vertices()[(*I).edges()[i].vertex_index()].vertex_id()),i != ((*I).edges().size() - 1) ? "\n" : "");
//			}
//			Msg					("%s",S);
//	}
}
#endif

void CObjectHandler::OnItemTake			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake	(inventory_item);
//	Msg						("Adding item %s (%d)",*inventory_item->cName(),inventory_item->ID());
	add_item				(inventory_item);
#ifdef DEBUG
	show_graph				();
#endif
}

void CObjectHandler::OnItemDrop			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemDrop	(inventory_item);
//	Msg						("Removing item %s (%d)",*inventory_item->cName(),inventory_item->ID());
	if (object_state(current_state_id(),inventory_item)) {
		set_current_state	(eObjectActionNoItems);
		set_dest_state		(eObjectActionNoItems);
	}
	remove_item				(inventory_item);
#ifdef DEBUG
	show_graph				();
#endif
}

void CObjectHandler::OnItemDropUpdate	()
{
}

CInventoryItem *CObjectHandler::best_weapon() const
{
	CInventoryItem	*best_weapon = 0;
	u32				best_weapon_type = 0;
	ai().ef_storage().m_tpCurrentMember = m_object;
	TIItemSet::const_iterator	I = inventory().m_all.begin();
	TIItemSet::const_iterator	E = inventory().m_all.end();
	for ( ; I != E; ++I) {
		if ((*I)->getDestroy())
			continue;
		CWeapon		*weapon = dynamic_cast<CWeapon*>(*I);
		if (weapon && (weapon->GetAmmoCurrent() > weapon->GetAmmoMagSize()/10)) {
			ai().ef_storage().m_tpGameObject	= weapon;
			u32	current_weapon_type = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
			if (current_weapon_type > best_weapon_type) {
				best_weapon_type = current_weapon_type;
				best_weapon		 = *I;
			}
			ai().ef_storage().m_tpGameObject	= 0;
			continue;
		}
		CMissile	*missile = dynamic_cast<CMissile*>(*I);
		if (missile) {
			ai().ef_storage().m_tpGameObject	= missile;
			u32	current_weapon_type = ai().ef_storage().m_pfPersonalWeaponType->dwfGetWeaponType();
			if (current_weapon_type > best_weapon_type) {
				best_weapon_type = current_weapon_type;
				best_weapon		 = *I;
			}
			ai().ef_storage().m_tpGameObject	= 0;
		}
	}
	return			(best_weapon);
}

void CObjectHandler::set_dest_state	(MonsterSpace::EObjectAction object_action, CGameObject *game_object)
{
	if (object_action == MonsterSpace::eObjectActionActivate)
		object_action					= MonsterSpace::eObjectActionIdle;

	if (object_action == MonsterSpace::eObjectActionDeactivate)
		object_action					= MonsterSpace::eObjectActionNoItems;

	if (object_action == MonsterSpace::eObjectActionNoItems) {
		inherited::set_dest_state		(object_action);
		return;
	}

	if (game_object)
		inherited::set_dest_state		(uid(object_action, game_object->ID()));
	else
		if (inventory().ActiveItem())
			inherited::set_dest_state	(uid(object_action, inventory().ActiveItem()->ID()));
		else
			inherited::set_dest_state	(u32(MonsterSpace::eObjectActionNoItems));
}

