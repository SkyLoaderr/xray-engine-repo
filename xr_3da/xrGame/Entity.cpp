// Entity.cpp: implementation of the CEntity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
 
#include "hudmanager.h"
#include "Entity.h"

#define MAX_ARMOR		200
#define MAX_HEALTH		100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntity::CEntity()
{
	fAccuracy			= 1.f;
	iMAX_Health			= MAX_HEALTH;
	iMAX_Armor			= MAX_ARMOR;
	eHealthLost_Enabled = FALSE;
	
	eHealthLost_Begin	= Engine.Event.Handler_Attach	("level.entity.healthlost.begin",	this);
	eHealthLost_End		= Engine.Event.Handler_Attach	("level.entity.healthlost.end",		this);
}

CEntity::~CEntity()
{
	Engine.Event.Handler_Detach	(eHealthLost_Begin,	this);
	Engine.Event.Handler_Detach	(eHealthLost_End,	this);
}

void CEntity::OnEvent	(EVENT E, DWORD P1, DWORD P2)
{
	if (E==eHealthLost_Begin)	
	{
		if (0==P2 || DWORD(this)==P2)	
		{
			eHealthLost_Enabled		= TRUE;
			LPCSTR	param			= LPCSTR(P1);
			sscanf					(param,"%f,%f",&eHealthLost_speed,&eHealthLost_granularity);
			eHealthLost_cumulative	= 0;
		}
	} else
	if (E==eHealthLost_End)
	{
		if (0==P2 || DWORD(this)==P2)	
		{
			eHealthLost_Enabled		= FALSE;
		}
	}
}

BOOL CEntity::Hit(int perc, Fvector &dir, CEntity* who) 
{
	if (who->Local())
	{
		// *** we are REMOTE/LOCAL, attacker is LOCAL
		// send, update
		// *** signal to everyone
		NET_Packet	P;
		P.w_begin	(M_HIT				);
		P.w_u16		(u16(net_ID)		);
		P.w_u16		(u16(who->net_ID)	);
		P.w_u8		(perc				);
		P.w_dir		(dir				);
		Level().Send(P,net_flags(TRUE)	);
	} else {
		// *** we are REMOTE/LOCAL, attacker is REMOTE
		// update
	}
	
	// *** process hit calculations
	// Calc impulse
	Fvector					vLocalDir;
	float					m = dir.magnitude();
	R_ASSERT				(m>EPS);
	
	// convert impulse into local coordinate system
	Fmatrix mInvXForm;
	mInvXForm.invert		(svTransform);
	mInvXForm.transform_dir	(vLocalDir,dir);
	vLocalDir.invert		();

	// hit impulse
	HitImpulse				(dir,vLocalDir,perc);
	
	// Calc HitAmount
	int iHitAmount, iOldHealth=iHealth;
	if (iArmor)
	{
		iHealth		-=	(iMAX_Armor-iArmor)/iMAX_Armor*perc;
		iHitAmount	=	(perc*9)/10;
		iArmor		-=	iHitAmount;
	} else {
		iHitAmount	=	perc;
		iHealth		-=	iHitAmount;
	}
	
	// Update state and play sounds, etc
	if (iHealth<=0 && iOldHealth>0)
	{ 
		if (who!=this)	Level().HUD()->outMessage(0xffffffff,cName(),"Killed by '%s'...",who->cName());
		else			Level().HUD()->outMessage(0xffffffff,cName(),"Crashed...");
		Die				();
		return TRUE;
	}
	else
	{
		HitSignal		(iHitAmount,vLocalDir,who);
		return FALSE;
	}
}

void CEntity::Load		(LPCSTR section)
{
	inherited::Load		(section);

	bVisible			= FALSE;
	CLS_ID				= CLSID_ENTITY;
	iHealth				= iArmor = 100;

	// Team params
	id_Team = -1; if (pSettings->LineExists(section,"team"))	id_Team		= pSettings->ReadINT	(section,"team");
	id_Squad= -1; if (pSettings->LineExists(section,"squad"))	id_Squad	= pSettings->ReadINT	(section,"squad");
	id_Group= -1; if (pSettings->LineExists(section,"group"))	id_Group	= pSettings->ReadINT	(section,"group");
}

BOOL CEntity::net_Spawn		(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	inherited::net_Spawn	(bLocal,server_id,o_pos,o_angle,P,flags);

	// Read team & squad & group
	u8					s_team,s_squad,s_group;
	P.r_u8				(s_team);
	P.r_u8				(s_squad);
	P.r_u8				(s_group);
	
	// Register
	CSquad& S			= Level().get_squad	(s_team,s_squad);
	CGroup& G			= Level().get_group	(s_team,s_squad,s_group);
	if (S.Leader==0)	S.Leader			=this;
	else				G.Members.push_back	(this);
	
	// Initialize variables
	id_Team				= s_team;
	id_Squad			= s_squad;
	id_Group			= s_group;
	iHealth				= 100;
	iArmor				= 0;
	
	Engine.Sheduler.Unregister	(this);
	Engine.Sheduler.Register	(this);

	return				TRUE;
}

void CEntity::net_Destroy	()
{
	CSquad& S			= Level().get_squad	(g_Team(),g_Squad());
	CGroup& G			= Level().get_group	(g_Team(),g_Squad(),g_Group());
	if (this==S.Leader)	
	{
		S.Leader		= 0;
		if (!G.Members.empty())	{
			S.Leader	= G.Members.back();
			G.Member_Remove(S.Leader);
		}
	}
	else {
		G.Member_Remove	(this);
	}

	inherited::net_Destroy	();
}

void CEntity::OnVisible()
{
	inherited::OnVisible		();
}

void CEntity::Update	(DWORD dt)
{
	inherited::Update	(dt);

	if (eHealthLost_Enabled)
	{
		eHealthLost_cumulative	+= (float(dt)/1000.f)*eHealthLost_speed;
		if (eHealthLost_cumulative > eHealthLost_granularity)
		{
			Fvector vdir; vdir.set	(0,1,0);
			eHealthLost_cumulative	-= eHealthLost_granularity;
			Hit						(iFloor(eHealthLost_granularity),vdir,this);
		}
	}
}

void CEntityAlive::Load		(LPCSTR section)
{
	inherited::Load			(section);

	// Movement: General
	Movement.SetParent		(this);
	Fbox	bb;

	// Movement: BOX
	Fvector	vBOX0_center= pSettings->ReadVECTOR	(section,"ph_box0_center"	);
	Fvector	vBOX0_size	= pSettings->ReadVECTOR	(section,"ph_box0_size"		);
	bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	Movement.SetBox		(0,bb);

	// Movement: BOX
	Fvector	vBOX1_center= pSettings->ReadVECTOR	(section,"ph_box1_center"	);
	Fvector	vBOX1_size	= pSettings->ReadVECTOR	(section,"ph_box1_size"		);
	bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	Movement.SetBox		(1,bb);

	// Movement: Foots
	Fvector	vFOOT_center= pSettings->ReadVECTOR	(section,"ph_foot_center"	);
	Fvector	vFOOT_size	= pSettings->ReadVECTOR	(section,"ph_foot_size"		);
	bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	Movement.SetFoots	(vFOOT_center,vFOOT_size);

	// Movement: Crash speed and mass
	float	cs_min		= pSettings->ReadFLOAT	(section,"ph_crash_speed_min"	);
	float	cs_max		= pSettings->ReadFLOAT	(section,"ph_crash_speed_max"	);
	float	mass		= pSettings->ReadFLOAT	(section,"ph_mass"				);
	Movement.SetCrashSpeeds	(cs_min,cs_max);
	Movement.SetMass		(mass);

	// Movement: Frictions
	float af, gf, wf;
	af					= pSettings->ReadFLOAT	(section,"ph_friction_air"	);
	gf					= pSettings->ReadFLOAT	(section,"ph_friction_ground");
	wf					= pSettings->ReadFLOAT	(section,"ph_friction_wall"	);
	Movement.SetFriction	(af,wf,gf);

	// BOX activate
	Movement.ActivateBox	(0);
}

BOOL CEntityAlive::net_Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	inherited::net_Spawn	(bLocal,server_id,o_pos,o_angle,P,flags);
	Movement.SetPosition	(o_pos.x,o_pos.y,o_pos.z);
	Movement.SetVelocity	(0,0,0);
	return					TRUE;
}
void CEntityAlive::HitImpulse	(Fvector& vWorldDir, Fvector& vLocalDir, float amount)
{
	float Q						=	2*float(amount)/Movement.GetMass();
	Movement.vExternalImpulse.mad	(vWorldDir,Q);
}

CEntityAlive::CEntityAlive()
{
}
CEntityAlive::~CEntityAlive()
{
}
