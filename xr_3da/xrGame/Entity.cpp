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
	
	// Movement: BOX
	Fbox	bb;	
	Fvector	vBOX_center	= ini->ReadVECTOR	(section,"ph_box_center"	);
	Fvector	vBOX_size	= ini->ReadVECTOR	(section,"ph_box_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow	(vBOX_size);
	Movement.SetBox		(bb);
	
	// Movement: Foots
	Fvector	vFOOT_center= ini->ReadVECTOR	(section,"ph_foot_center"	);
	Fvector	vFOOT_size	= ini->ReadVECTOR	(section,"ph_foot_size"		);
	bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	Movement.SetFoots	(bb);

	// Movement: Crash speed and mass
	float	cs_min		= ini->ReadFLOAT	(section,"ph_crash_min"		);
	float	cs_max		= ini->ReadFLOAT	(section,"ph_crash_max"		);
	float	mass		= ini->ReadFLOAT	(section,"ph_mass"			);
	Movement.SetParent		(this);
	Movement.SetCrashSpeeds	(cs_min,cs_max);
	Movement.SetMass		(mass);
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

	return				TRUE;
}

void CEntity::OnMoveVisible()
{
	::Render.Wallmarks.AddShadow(this);
}
