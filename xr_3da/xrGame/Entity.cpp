// Entity.cpp: implementation of the CEntity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\fstaticrender.h"
 
#include "hudmanager.h"
#include "Entity.h"

#define MAX_ARMOR		200
#define MAX_HEALTH		100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntity::CEntity()
{
	iMAX_Health	= MAX_HEALTH;
	iMAX_Armor	= MAX_ARMOR;
}

CEntity::~CEntity()
{
}

BOOL CEntity::Hit(int perc, Fvector &dir, CEntity* who) 
{
	if (who->Local())
	{
		// *** we are REMOTE/LOCAL, attacker is LOCAL
		// send, update
		// *** signal to everyone
		NET_Packet	P;
		P.w_begin	(M_FIRE_HIT);
		P.w_u8		(u8(net_ID));
		P.w_u8		(u8(who->net_ID));
		P.w_u8		(perc	);
		P.w_dir		(dir	);
		Level().Send(P,net_flags(TRUE));
	} else {
		// *** we are REMOTE/LOCAL, attacker is REMOTE
		// update
	}
	
	// *** process hit calculations
	// Calc impulse
	Fvector vLocalDir;
	float m = dir.magnitude();
	R_ASSERT(m>EPS);
	
	float amount			=	2*float(perc)/Movement.GetMass();
	dir.y					+=	0.1f;
	Fvector I; I.direct		(Movement.vExternalImpulse,dir,amount/m);
	Movement.vExternalImpulse.add(I);
	
	// convert impulse into local coordinate system
	Fmatrix mInvXForm;
	mInvXForm.invert		(svTransform);
	mInvXForm.transform_dir	(vLocalDir,dir);
	vLocalDir.invert		();
	
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

void CEntity::Load(CInifile* ini, const char* section)
{
	CObject::Load(ini,section);
	bVisible	= FALSE;

	CLS_ID	= CLSID_ENTITY;
	iHealth = iArmor = 100;

	// Team params
	id_Team = -1; if (ini->LineExists(section,"team"))	id_Team		= ini->ReadINT	(section,"team");
	id_Squad= -1; if (ini->LineExists(section,"squad"))	id_Squad	= ini->ReadINT	(section,"squad");
	id_Group= -1; if (ini->LineExists(section,"group"))	id_Group	= ini->ReadINT	(section,"group");

	// Movement: General
	Movement.SetParent		(this);
	Fbox	bb;
	
	// Movement: BOX
	Fvector	vBOX0_center= ini->ReadVECTOR	(section,"ph_box0_center"	);
	Fvector	vBOX0_size	= ini->ReadVECTOR	(section,"ph_box0_size"		);
	bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	Movement.SetBox		(0,bb);
	
	// Movement: BOX
	Fvector	vBOX1_center= ini->ReadVECTOR	(section,"ph_box1_center"	);
	Fvector	vBOX1_size	= ini->ReadVECTOR	(section,"ph_box1_size"		);
	bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	Movement.SetBox		(1,bb);
	
	// Movement: Foots
	Fvector	vFOOT_center= ini->ReadVECTOR	(section,"ph_foot_center"	);
	Fvector	vFOOT_size	= ini->ReadVECTOR	(section,"ph_foot_size"		);
	bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	Movement.SetFoots	(vFOOT_center,vFOOT_size);

	// Movement: Crash speed and mass
	float	cs_min		= ini->ReadFLOAT	(section,"ph_crash_speed_min"	);
	float	cs_max		= ini->ReadFLOAT	(section,"ph_crash_speed_max"	);
	float	mass		= ini->ReadFLOAT	(section,"ph_mass"				);
	Movement.SetCrashSpeeds	(cs_min,cs_max);
	Movement.SetMass		(mass);
	
	// Movement: Frictions
	float af, gf, wf;
	af					= ini->ReadFLOAT	(section,"ph_friction_air"	);
	gf					= ini->ReadFLOAT	(section,"ph_friction_ground");
	wf					= ini->ReadFLOAT	(section,"ph_friction_wall"	);
	Movement.SetFriction	(af,wf,gf);

	// BOX activate
	Movement.ActivateBox	(0);
}

BOOL CEntity::Spawn		(BOOL bLocal, int server_id, int team, int squad, int group, Fvector4& o_pos)
{
	CObject::Spawn		(bLocal,server_id,o_pos);

	// Initialize variables
	id_Team				= team;
	id_Squad			= squad;
	id_Group			= group;
	iHealth				= 100;
	iArmor				= 0;
	Movement.SetPosition(o_pos.x,o_pos.y,o_pos.z);
	Movement.SetVelocity(0,0,0);
	pCreator->Objects.sheduled.Unregister	(this);
	pCreator->Objects.sheduled.Register		(this);

	return				TRUE;
}

void CEntity::OnMoveVisible()
{
	::Render.Wallmarks.AddShadow(this);
}
