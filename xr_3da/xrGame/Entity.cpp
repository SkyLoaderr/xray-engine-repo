// Entity.cpp: implementation of the CEntity class.

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
 
#include "hudmanager.h"
#include "Entity.h"
#include "ai_funcs.h"

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
}

CEntity::~CEntity()
{
}

void CEntity::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	switch (type)
	{
	case GE_HIT:
		{
			u16				id;
			Fvector			dir;
			float			power;
			P.r_u16			(id);
			P.r_dir			(dir);
			P.r_float		(power);
			Hit				(power,dir,Level().Objects.net_Find(id));
		}
		break;
	case GE_DIE:
		{
			u16				id;
			u32				cl;
			P.r_u16			(id);
			P.r_u32			(cl);
			CObject* who	= Level().Objects.net_Find	(id);
			if (who!=this)	Level().HUD()->outMessage	(0xffffffff,cName(),"Killed by '%s'...",who->cName());
			else			Level().HUD()->outMessage	(0xffffffff,cName(),"Crashed...");
			Die				();
		}
		break;
	}
}

void CEntity::Hit			(float perc, Fvector &dir, CObject* who) 
{
	// *** process hit calculations
	// Calc impulse
	Fvector					vLocalDir;
	float					m = dir.magnitude();
	R_ASSERT				(m>EPS);
	
	// convert impulse into local coordinate system
	Fmatrix					mInvXForm;
	mInvXForm.invert		(clTransform);
	mInvXForm.transform_dir	(vLocalDir,dir);
	vLocalDir.invert		();

	// hit impulse
	HitImpulse				(perc,dir,vLocalDir);
	
	// Calc amount (correct only on local player)
	float	lost_health,		lost_armor;
	if (fArmor>0)
	{
		lost_health		=	(fMAX_Armor-fArmor)/fMAX_Armor*perc;
		lost_armor		=	(perc*9.f)/10.f;
	} else {
		lost_health		=	perc;
		lost_armor		=	0;
	}

	// Signal hit
	HitSignal				(lost_health,vLocalDir,who);

	// If Local() - perform some logic
	if (Local() && (fHealth>0))
	{
		fHealth				-=	lost_health;
		fArmor				-=	lost_armor;

		if (fHealth<=0)
		{
			// die
			NET_Packet		P;
			u_EventGen		(P,GE_DIE,ID()	);
			P.w_u16			(u16(who->ID())	);
			P.w_u32			(0);
			u_EventSend		(P);
		}
	}
}

void CEntity::Load		(LPCSTR section)
{
	inherited::Load		(section);

	setVisible			(FALSE);
	CLS_ID				= CLSID_ENTITY;
	
	m_fMaxHealthValue = fHealth = 100;
	//fHealth				= fArmor = 100;

	// Team params
	id_Team = -1; if (pSettings->LineExists(section,"team"))	id_Team		= pSettings->ReadINT	(section,"team");
	id_Squad= -1; if (pSettings->LineExists(section,"squad"))	id_Squad	= pSettings->ReadINT	(section,"squad");
	id_Group= -1; if (pSettings->LineExists(section,"group"))	id_Group	= pSettings->ReadINT	(section,"group");
	
	m_fMorale = 66.f;
}

BOOL CEntity::net_Spawn		(LPVOID DC)
{
	inherited::net_Spawn	(DC);

	xrSE_Teamed*		E	= (xrSE_Teamed*)DC;

	// 
	id_Team					= E->g_team();
	id_Squad				= E->g_squad();
	id_Group				= E->g_group();

	// Register
	CSquad& S				= Level().get_squad	(id_Team,id_Squad);
	CGroup& G				= Level().get_group	(id_Team,id_Squad,id_Group);
	if (S.Leader==0)		S.Leader			=this;
	else					G.Members.push_back	(this);
	G.m_dwAliveCount++;
	
	// Initialize variables
	//fHealth					= 100;
	fArmor					= 0;
	
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

BOOL CEntityAlive::net_Spawn	(LPVOID DC)
{
	inherited::net_Spawn	(DC);

	Movement.SetPosition	(vPosition);
	Movement.SetVelocity	(0,0,0);
	return					TRUE;
}
void CEntityAlive::HitImpulse	(float amount, Fvector& vWorldDir, Fvector& vLocalDir)
{
	float Q					= 2*float(amount)/Movement.GetMass();
	Movement.vExternalImpulse.mad	(vWorldDir,Q);
}

CEntityAlive::CEntityAlive()
{
}
CEntityAlive::~CEntityAlive()
{
}
