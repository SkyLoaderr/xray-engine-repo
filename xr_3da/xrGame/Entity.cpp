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

#define MAX_ARMOR		200
#define MAX_HEALTH		100

#define BODY_REMOVE_TIME		600000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntity::CEntity()
{
	fMAX_Health			= MAX_HEALTH;
	fMAX_Armor			= MAX_ARMOR;

	m_iTradeIconX		= m_iTradeIconY = 0;
	m_iMapIconX			= m_iMapIconY = 0;
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
			CObject* who	= Level().Objects.net_Find	(id);
			if (who)
			{
				if (this!=who)	if(bDebug) HUD().outMessage	(0xffffffff,cName(),"Killed by '%s'...",who->cName());
				else			if(bDebug) HUD().outMessage	(0xffffffff,cName(),"Crashed...");
			};
			Die				();
		}
		break;
	}
}

void CEntity::Die() 
{
	fEntityHealth = -1.f;
	Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).unregister_member(this);
}

//обновление состояния
float CEntity::CalcCondition(float hit)
{
	// Calc amount (correct only on local player)
	float	lost_health,		lost_armor;

	if (fArmor>0)
	{
		lost_health		=	(fMAX_Armor-fArmor)/fMAX_Armor*hit;
		lost_armor		=	(hit*9.f)/10.f;
	} 
	else 
	{
		lost_health		=	hit;
		lost_armor		=	0;
	}

	// If Local() - perform some logic
	if (Local() && g_Alive()) {
		fEntityHealth		-=	lost_health; 
		fEntityHealth		=	fEntityHealth<-1000?-1000:fEntityHealth;
		fArmor				-=	lost_armor;
	}

	return lost_health;
}

void CEntity::Hit			(float perc, Fvector &dir, CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type) 
{

	if (bDebug)				Log("Process HIT: ", *cName());

	// *** process hit calculations
	// Calc impulse
	Fvector					vLocalDir;
	float					m = dir.magnitude();
	R_ASSERT				(m>EPS);
	
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
	CLS_ID				= CLSID_ENTITY;
	
	m_fMaxHealthValue = fEntityHealth = 100;
	
	// Team params
	id_Team = -1; if (pSettings->line_exist(section,"team"))	id_Team		= pSettings->r_s32	(section,"team");
	id_Squad= -1; if (pSettings->line_exist(section,"squad"))	id_Squad	= pSettings->r_s32	(section,"squad");
	id_Group= -1; if (pSettings->line_exist(section,"group"))	id_Group	= pSettings->r_s32	(section,"group");
	
#pragma todo("Jim to Dima: no specific figures or comments needed")	
	m_fMorale = 66.f;


	//загрузить параметры иконки торговли
	CKinematics* pKinematics=PKinematics(Visual());
	CInifile* ini = NULL;

/*			ref_str	first;
		ref_str	second;
		ref_str	comment;

		Item() : first(0), second(0), comment(0) {};
	};
	typedef xr_vector<Item>			Items;
	typedef Items::iterator			SectIt;
    struct XRCORE_API Sect {
		ref_str			Name;
		Items			Data;

		IC SectIt		begin()		{ return Data.begin();	}
		IC SectIt		end()		{ return Data.end();	}
		IC size_t		size()		{ return Data.size();	}
		IC void			clear()		{ Data.clear();			}
	    BOOL			line_exist	(LPCSTR L, LPCSTR* val=0);
*/
	
	if(pKinematics) ini = pKinematics->LL_UserData();
	if(ini)
	{
		if(ini->section_exist("icon"))
		{
			m_iTradeIconX = ini->r_u32("icon","icon_x");
			m_iTradeIconY = ini->r_u32("icon","icon_y");

			m_iMapIconX = ini->r_u32("icon","map_icon_x");
			m_iMapIconY = ini->r_u32("icon","map_icon_y");
		}
		else
		{
			m_iTradeIconX = m_iTradeIconY = 0;
			m_iMapIconX = 1;
			m_iMapIconY = 4;
		}

		CParticlesPlayer::Load(pKinematics);
	}
	//////////////////////////////////////
	//время убирания тела с уровня
	if(pSettings->line_exist(section,"body_remove_time"))
		m_dwBodyRemoveTime = pSettings->r_u32(section,"body_remove_time");
	else
		m_dwBodyRemoveTime = BODY_REMOVE_TIME;
	//////////////////////////////////////
}

BOOL CEntity::net_Spawn		(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return				(FALSE);

	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeCreatureAbstract	*E	= dynamic_cast<CSE_ALifeCreatureAbstract*>(e);
	if (!E) {
		// Car or trader only!!!!
		CSE_ALifeCar		*C	= dynamic_cast<CSE_ALifeCar*>(e);
		CSE_ALifeTrader		*T	= dynamic_cast<CSE_ALifeTrader*>(e);
		R_ASSERT2			(C || T,"Invalid entity (no inheritance from CSE_CreatureAbstract, CSE_ALifeItemCar and CSE_ALifeTrader)!");
		id_Team				= id_Squad = id_Group = 0;
	}
	else {
		id_Team				= E->g_team();
		id_Squad			= E->g_squad();
		id_Group			= E->g_group();
	}

	// Initialize variables
	if(E)
		fEntityHealth			= E->fHealth;
	else
		fEntityHealth			= 100.f;

	fArmor					= 0;

	// Register
	if (g_Alive()) {
		Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).register_member(this);
		++Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).m_dwAliveCount;
	}
	
	Engine.Sheduler.Unregister	(this);
	Engine.Sheduler.Register	(this);

	return					TRUE;
}

void CEntity::net_Destroy	()
{
	if (g_Alive())
		Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).unregister_member(this);
	inherited::net_Destroy	();
}

void CEntity::renderable_Render()
{
	inherited::renderable_Render		();
}

void CEntity::KillEntity(CObject* who)
{
	NET_Packet		P;
	u_EventGen		(P,GE_DIE,ID());
	P.w_u16			(u16(who->ID()));
	P.w_u32			(0);
	if (OnServer())
		u_EventSend		(P);

	set_death_time		();
}

void CEntity::reinit			()
{
	inherited::reinit			();

	m_level_death_time			= 0;
	m_game_death_time			= 0;
}

void CEntity::set_death_time	()
{
	m_level_death_time	= Level().timeServer();
	m_game_death_time	= Level().GetGameTime();
}
