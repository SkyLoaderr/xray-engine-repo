// Entity.cpp: implementation of the CEntity class.

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hudmanager.h"
#include "Entity.h"
#include "actor.h"
#include "xrserver_objects_alife_monsters.h"
#include "entity.h"
#include "level.h"
#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"
#include "clsid_game.h"
#include "../skeletoncustom.h"
#include "monster_community.h"
#include "ai_space.h"

#define MAX_ARMOR		200
#define MAX_HEALTH		100

#define BODY_REMOVE_TIME		600000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntity::CEntity()
{
	fMAX_Health			= MAX_HEALTH;
}

CEntity::~CEntity()
{	
}

void CEntity::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	switch (type)
	{

	case GE_DIE:
		{
			u16				id;
			u32				cl;
			P.r_u16			(id);
			P.r_u32			(cl);
			CObject			*who = Level().Objects.net_Find	(id);
			if (who) {
				if (this!=who)	if(bDebug) HUD().outMessage	(0xffffffff,cName(),"Killed by '%s'...",who->cName());
				else			if(bDebug) HUD().outMessage	(0xffffffff,cName(),"Crashed...");
			};
			Die				(who);
		}
		break;
	}
}

void CEntity::Die(CObject* who)
{
	set_ready_to_save	();
	SetfHealth			(-1.f);
	Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).unregister_member(this,false);
}

//���������� ���������
float CEntity::CalcCondition(float hit)
{

	// If Local() - perform some logic
	if (Local() && g_Alive()) {
		SetfHealth			(GetfHealth()-hit);
		SetfHealth			((GetfHealth()<-1000)?-1000:GetfHealth());
	}
	return hit;
}




void CEntity::Hit			(float perc, Fvector &dir, CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type) 
{

	if (bDebug)				Log("Process HIT: ", *cName());

	// *** process hit calculations
	// Calc impulse
	Fvector					vLocalDir;
	float					m = dir.magnitude();
	VERIFY					(m>EPS);
	
	// convert impulse into local coordinate system
	Fmatrix					mInvXForm;
	mInvXForm.invert		(XFORM());
	mInvXForm.transform_dir	(vLocalDir,dir);
	vLocalDir.invert		();

	// hit impulse
	if(impulse) HitImpulse				(impulse,dir,vLocalDir); // @@@: WT
	
	// Calc amount (correct only on local player)
	float lost_health = CalcCondition(perc);

	// Signal hit
	if(-1!=element)	HitSignal(lost_health,vLocalDir,who,element);

	// If Local() - perform some logic
	if (Local() && !g_Alive() && !AlreadyDie())
		KillEntity	(who);
	//must be last!!! @slipch
	inherited::Hit(perc,dir,who,element,position_in_object_space,impulse, hit_type);
}

void CEntity::Load		(LPCSTR section)
{
	inherited::Load		(section);

	setVisible			(FALSE);
	m_fMaxHealthValue /*= fEntityHealth*/ = 100;
	SetfHealth			(100);
	
	// Team params
	id_Team				= READ_IF_EXISTS(pSettings,r_s32,section,"team",-1);
	id_Squad			= READ_IF_EXISTS(pSettings,r_s32,section,"squad",-1);
	id_Group			= READ_IF_EXISTS(pSettings,r_s32,section,"group",-1);
	
#pragma todo("Jim to Dima: no specific figures or comments needed")	
	m_fMorale			= 66.f;


	//��������� ��������� ������ ��������

/*			shared_str	first;
		shared_str	second;
		shared_str	comment;

		Item() : first(0), second(0), comment(0) {};
	};
	typedef xr_vector<Item>			Items;
	typedef Items::iterator			SectIt;
    struct XRCORE_API Sect {
		shared_str			Name;
		Items			Data;

		IC SectIt		begin()		{ return Data.begin();	}
		IC SectIt		end()		{ return Data.end();	}
		IC size_t		size()		{ return Data.size();	}
		IC void			clear()		{ Data.clear();			}
	    BOOL			line_exist	(LPCSTR L, LPCSTR* val=0);
*/

	//////////////////////////////////////
	//����� �������� ���� � ������
	m_dwBodyRemoveTime	= READ_IF_EXISTS(pSettings,r_u32,section,"body_remove_time",BODY_REMOVE_TIME);
	//////////////////////////////////////
}

BOOL CEntity::net_Spawn		(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC))
		return				(FALSE);

	// load damage params
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeCreatureAbstract	*E	= smart_cast<CSE_ALifeCreatureAbstract*>(e);
	if (!E) {
		// Car or trader only!!!!
		CSE_ALifeCar		*C	= smart_cast<CSE_ALifeCar*>(e);
		CSE_ALifeTrader		*T	= smart_cast<CSE_ALifeTrader*>(e);
		R_ASSERT2			(C || T,"Invalid entity (no inheritance from CSE_CreatureAbstract, CSE_ALifeItemCar and CSE_ALifeTrader)!");
		id_Team				= id_Squad = id_Group = 0;
	}
	else {
		id_Team				= E->g_team();
		id_Squad			= E->g_squad();
		id_Group			= E->g_group();


#ifdef DEBUG
		if (!ai().get_alife()) {
			
			CSE_ALifeMonsterRat		*rat		= smart_cast<CSE_ALifeMonsterRat*>(E);
			CSE_ALifeMonsterBase	*monster	= smart_cast<CSE_ALifeMonsterBase*>(E);

			if (monster || rat) {
				MONSTER_COMMUNITY		monster_community;
				monster_community.set	(pSettings->r_string(*cNameSect(), "species"));

				if(monster_community.team() != 255)
					id_Team = monster_community.team();
			}
		}
#endif
	}


	// Initialize variables
	if(E) {
		SetfHealth			(E->fHealth);
//		fEntityHealth		= E->fHealth;
		m_killer_id			= E->m_killer_id;
		if (m_killer_id == ID())
			m_killer_id		= ALife::_OBJECT_ID(-1);
	}
	else
		SetfHealth				(100.f);
//		fEntityHealth		= 100.f;

//	fArmor					= 0;

	// Register
	if (g_Alive()) {
		Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).register_member(this);
		++Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).m_dwAliveCount;
	}
	else {
		m_level_death_time		= Device.dwTimeGlobal;
		m_game_death_time		= Level().GetGameTime();
	}
	
	CKinematics* pKinematics=smart_cast<CKinematics*>(Visual());
	CInifile* ini = NULL;

	if(pKinematics) ini = pKinematics->LL_UserData();
	if(ini)
	{
		if(ini->section_exist("damage_section"))	
			CDamageManager::load_section(ini->r_string("damage_section","damage"),pSettings);

		CParticlesPlayer::LoadParticles(pKinematics);
	}
	return					TRUE;
}

void CEntity::net_Destroy	()
{
	if (g_Alive())
		Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).unregister_member(this,true);
	inherited::net_Destroy	();
	set_ready_to_save		();
}

void CEntity::renderable_Render()
{
	inherited::renderable_Render		();
}

void CEntity::KillEntity(u16 whoID)
{
	if (whoID && (whoID != ID())) {
		VERIFY			(m_killer_id == ALife::_OBJECT_ID(-1));
		m_killer_id		= whoID;
	}

	if (!getDestroy()){
		NET_Packet		P;
		u_EventGen		(P,GE_DIE,ID());
		P.w_u16			(u16(whoID));
		P.w_u32			(0);
		if (OnServer())
			u_EventSend	(P);
		set_death_time	();
	}
};

void CEntity::KillEntity(CObject* who)
{
	VERIFY			(who);
	if (who) KillEntity(who->ID());	
}

void CEntity::reinit			()
{
	inherited::reinit			();

	m_level_death_time			= 0;
	m_game_death_time			= 0;
	m_killer_id					= ALife::_OBJECT_ID(-1);
}


void CEntity::reload			(LPCSTR section)
{
	inherited::reload			(section);
	CDamageManager::reload		(section,pSettings);
}

void CEntity::set_death_time	()
{
	m_level_death_time	= Device.dwTimeGlobal;
	m_game_death_time	= Level().GetGameTime();
}

bool CEntity::IsFocused			()const	{ return (smart_cast<const CEntity*>(g_pGameLevel->CurrentEntity())==this);		}
bool CEntity::IsMyCamera		()const	{ return (smart_cast<const CEntity*>(g_pGameLevel->CurrentViewEntity())==this);	}

void CEntity::set_ready_to_save	()
{
}

DLL_Pure *CEntity::_construct	()
{
	inherited::_construct		();
	CDamageManager::_construct	();
	return						(this);
}

const u32 FORGET_KILLER_TIME = 180000;

void CEntity::shedule_Update	(u32 dt)
{
	inherited::shedule_Update	(dt);
	if (!g_Alive() && (m_killer_id != u16(-1))) {
		if (Device.dwTimeGlobal > m_level_death_time + FORGET_KILLER_TIME) {
			m_killer_id			= u16(-1);
			NET_Packet			P;
			u_EventGen			(P,GE_ASSIGN_KILLER,ID());
			P.w_u16				(u16(-1));
			if (GameID() == GAME_SINGLE)	u_EventSend			(P);
		}
	}
}
void CEntity::ChangeTeam(int team, int squad, int group)
{
	if ((team == g_Team()) && (squad == g_Squad()) && (group == g_Group())) return;
	VERIFY2(g_Alive(), "Try to change team of a dead object");
	
	// remove from current team
	Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).unregister_member	(this,false);

	id_Team					= team;
	id_Squad				= squad;
	id_Group				= group;

	// add to new team
	Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).register_member		(this);
}
