// Entity.cpp: implementation of the CEntity class.

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
 
#include "hudmanager.h"
#include "Entity.h"
#include "inventory.h"

#include "actor.h"

#define MAX_ARMOR		200
#define MAX_HEALTH		100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntity::CEntity()
{
	fAccuracy			= 1.f;
	fMAX_Health			= MAX_HEALTH;
	fMAX_Armor			= MAX_ARMOR;

	m_dwDeathTime = 0;
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
			if (this!=who)	if(bDebug) HUD().outMessage	(0xffffffff,cName(),"Killed by '%s'...",who->cName());
			else			if(bDebug) HUD().outMessage	(0xffffffff,cName(),"Crashed...");
			Die				();
		}
		break;
	}
}

void CEntity::Die() 
{
}

//���������� ���������
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
	if (Local() && g_Alive())
	{
		fEntityHealth		-=	lost_health; 
		fEntityHealth		=	fEntityHealth<-1000?-1000:fEntityHealth;
		fArmor				-=	lost_armor;
	}

	return lost_health;
}

void CEntity::Hit			(float perc, Fvector &dir, CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type) 
{
	inherited::Hit(perc,dir,who,element,position_in_object_space,impulse, hit_type);
	if (bDebug)				Log("Process HIT: ", cName());

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
	//if (Local() && (fEntityHealth>0))
	if (Local() && g_Alive())
	{
		//if (fEntityHealth<=0)
		if(!g_Alive() && !AlreadyDie())
		{
			KillEntity(who);
		}
	}
}

void CEntity::Load		(LPCSTR section)
{
	inherited::Load		(section);

	setVisible			(FALSE);
	CLS_ID				= CLSID_ENTITY;
	
	m_fMaxHealthValue = fEntityHealth = 100;
	//GetEntityHealth()				= fArmor = 100;

	// Team params
	id_Team = -1; if (pSettings->line_exist(section,"team"))	id_Team		= pSettings->r_s32	(section,"team");
	id_Squad= -1; if (pSettings->line_exist(section,"squad"))	id_Squad	= pSettings->r_s32	(section,"squad");
	id_Group= -1; if (pSettings->line_exist(section,"group"))	id_Group	= pSettings->r_s32	(section,"group");
	
#pragma todo("Jim to Dima: no specific figures or comments needed")	
	m_fMorale = 66.f;

	// Msg					("! entity size: %d",sizeof(*this));
}

BOOL CEntity::net_Spawn		(LPVOID DC)
{
	inherited::net_Spawn	(DC);

	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeCreatureAbstract	*E	= dynamic_cast<CSE_ALifeCreatureAbstract*>(e);
	if (!E) {
		// Car or trader only!!!!
		CSE_ALifeItemCar	*C	= dynamic_cast<CSE_ALifeItemCar*>(e);
		CSE_ALifeTrader		*T	= dynamic_cast<CSE_ALifeTrader*>(e);
		R_ASSERT2			(C || T,"Invalid entity (no inheritance from CSE_CreatureAbstract, CSE_ALifeItemCar and CSE_ALifeTrader)!");
		id_Team				= id_Squad = id_Group = 0;
	}
	else {
		id_Team				= E->g_team();
		id_Squad			= E->g_squad();
		id_Group			= E->g_group();
	}

	// Register
	CSquad& S				= Level().get_squad	(id_Team,id_Squad);
	CGroup& G				= Level().get_group	(id_Team,id_Squad,id_Group);
	G.Members.push_back		(this);
	if (S.Leader==0)		S.Leader			=this;
	++G.m_dwAliveCount;
	
	CActor	*pA = dynamic_cast<CActor*>(this);
	if (!pA) {
		Level().SquadMan.RegisterMember(id_Squad, this);
		Level().SquadMan.Dump();
	}

	// Initialize variables
	fEntityHealth			= 100;
	fArmor					= 0;
	
	Engine.Sheduler.Unregister	(this);
	Engine.Sheduler.Register	(this);

	return					TRUE;
}

void CEntity::net_Destroy	()
{
	CSquad& S			= Level().get_squad	(g_Team(),g_Squad());
	CGroup& G			= Level().get_group	(g_Team(),g_Squad(),g_Group());
	G.Member_Remove		(this);
	if (this==S.Leader)	
	{
		S.Leader		= 0;
		if (!G.Members.empty())	S.Leader	= G.Members.back();
	}

	for (u32 T=0; T<Level().Teams.size(); ++T)
	{
		CTeam&	TD		= Level().Teams[T];
		for (u32 S=0; S<TD.Squads.size(); ++S)
		{
			CSquad&	SD	= TD.Squads[S];
			objVisible& VIS	= SD.KnownEnemys;

			VIS.clear		();
			for (u32 G=0; G<SD.Groups.size(); ++G)
			{
				CGroup& GD = SD.Groups[G];
				for (u32 M=0; M<GD.Members.size(); ++M)
				{
					CEntityAlive* E	= dynamic_cast<CEntityAlive*>(GD.Members[M]);
					if (E && E->g_Alive())	E->GetVisible(VIS);
				}
			}
		}
	}
	
	CActor	*pA = dynamic_cast<CActor*>(this);
	if (!pA) {
		Level().SquadMan.RemoveMember(g_Squad(), this);
		Level().SquadMan.Dump();
	}

	inherited::net_Destroy	();
}

void CEntity::renderable_Render()
{
	inherited::renderable_Render		();
}

void CEntity::shedule_Update	(u32 dt)
{
	inherited::shedule_Update	(dt);
}

void CEntity::KillEntity(CObject* who)
{
	NET_Packet		P;
	u_EventGen		(P,GE_DIE,ID());
	P.w_u16			(u16(who->ID()));
	P.w_u32			(0);
	u_EventSend		(P);

	m_dwDeathTime = Level().GetGameTime();
}



/////////////////////////////////////////////
// CEntityAlive
/////////////////////////////////////////////
void CEntityAlive::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);

	//condition update with the game time pass
	UpdateCondition();

	//����� ��������
	if(Local() && !g_Alive() && !AlreadyDie())
	{
		if(GetWhoHitLastTime())
			KillEntity(GetWhoHitLastTime());
		else
			KillEntity(this);
	}
}

void CEntityAlive::Load		(LPCSTR section)
{
	inherited::Load			(section);
	CEntityCondition::Load	(section);

	m_fFood					= 100*pSettings->r_float	(section,"ph_mass");

	/*
	// m_PhysicMovementControl: General
	m_PhysicMovementControl.SetParent		(this);
	Fbox	bb;

	// m_PhysicMovementControl: BOX
	Fvector	vBOX0_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	Fvector	vBOX0_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	m_PhysicMovementControl.SetBox		(0,bb);

	// m_PhysicMovementControl: BOX
	Fvector	vBOX1_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	Fvector	vBOX1_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	m_PhysicMovementControl.SetBox		(1,bb);

	// m_PhysicMovementControl: Foots
	Fvector	vFOOT_center= pSettings->r_fvector3	(section,"ph_foot_center"	);
	Fvector	vFOOT_size	= pSettings->r_fvector3	(section,"ph_foot_size"		);
	bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	m_PhysicMovementControl.SetFoots	(vFOOT_center,vFOOT_size);

	// m_PhysicMovementControl: Crash speed and mass
	float	cs_min		= pSettings->r_float	(section,"ph_crash_speed_min"	);
	float	cs_max		= pSettings->r_float	(section,"ph_crash_speed_max"	);
	float	mass		= pSettings->r_float	(section,"ph_mass"				);
	m_PhysicMovementControl.SetCrashSpeeds	(cs_min,cs_max);
	m_PhysicMovementControl.SetMass		(mass);
	*/

	// m_PhysicMovementControl: Frictions
	/*
	float af, gf, wf;
	af					= pSettings->r_float	(section,"ph_friction_air"	);
	gf					= pSettings->r_float	(section,"ph_friction_ground");
	wf					= pSettings->r_float	(section,"ph_friction_wall"	);
	m_PhysicMovementControl.SetFriction	(af,wf,gf);

	// BOX activate
	m_PhysicMovementControl.ActivateBox	(0);
	*/
}

BOOL CEntityAlive::net_Spawn	(LPVOID DC)
{
	inherited::net_Spawn	(DC);

	//m_PhysicMovementControl.SetPosition	(Position());
	//m_PhysicMovementControl.SetVelocity	(0,0,0);
	return					TRUE;
}

void CEntityAlive::net_Destroy	()
{
	CInventoryOwner	*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(this);
	if (l_tpInventoryOwner) {
		l_tpInventoryOwner->m_inventory.ClearAll();
		l_tpInventoryOwner->m_trade_storage.ClearAll();
	}

	inherited::net_Destroy		();
}

void CEntityAlive::HitImpulse	(float /**amount/**/, Fvector& /**vWorldDir/**/, Fvector& /**vLocalDir/**/)
{
//	float Q					= 2*float(amount)/m_PhysicMovementControl.GetMass();
//	m_PhysicMovementControl.vExternalImpulse.mad	(vWorldDir,Q);
}

void CEntityAlive::Hit(float P, Fvector &dir,CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type)
{
	//�������� ���������, ����� ��� ��� ������������ ����� ��������� ���
	ConditionHit(who, P, hit_type, element);


	inherited::Hit(P,dir,who,element,position_in_object_space,impulse, hit_type);
}

CEntityAlive::CEntityAlive()
{
	m_dwDeathTime	= 0;
	m_fAccuracy		= 25.f;
	m_fIntelligence	= 25.f;
}

CEntityAlive::~CEntityAlive()
{
}

void CEntityAlive::BuyItem(LPCSTR buf)
{
	NET_Packet P;
	u_EventGen	(P,GE_BUY,ID());
	P.w_string	(buf);
	u_EventSend	(P);
}

//��������� ��� �������� ����
float CEntityAlive::CalcCondition(float /**hit/**/)
{	
	UpdateCondition();

	//dont call inherited::CalcCondition it will be meaning less
	return GetHealthLost()*100.f;
}