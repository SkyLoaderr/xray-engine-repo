#include "stdafx.h"
#pragma hdrstop

#ifdef _EDITOR
	#include "xrServer_Entities.h"
	#include "xr_ini.h"
	#include "clsid_game.h"
	#include "xr_tokens.h"
	#include "game_base.h"
#else
	#include "xrServer.h"
#endif

// EDITOR, NETWORK, SAVE, LOAD, DEMO
void	xrServerEntity::Spawn_Write		(NET_Packet& P, BOOL bLocal)
{
	// generic
	P.w_begin			(M_SPAWN		);
	P.w_string			(s_name			);
	P.w_string			(s_name_replace	);
	P.w_u8				(s_gameid		);
	P.w_u8				(s_RP			);
	P.w_vec3			(o_Position		);
	P.w_vec3			(o_Angle		);
	P.w_u16				(RespawnTime	);
	P.w_u16				(ID				);
	P.w_u16				(ID_Parent		);
	P.w_u16				(ID_Phantom		);
	if (bLocal)			P.w_u16(u16(s_flags|M_SPAWN_OBJECT_LOCAL) );
	else				P.w_u16(u16(s_flags&~(M_SPAWN_OBJECT_LOCAL|M_SPAWN_OBJECT_ASPLAYER)));

	// write specific data
	u32	position		= P.w_tell		();
	P.w_u16				(0				);
	STATE_Write			(P				);
	u16 size			= u16			(P.w_tell()-position);
	P.w_seek			(position,&size,sizeof(u16));
}

void	xrServerEntity::Spawn_Read		(NET_Packet& P)
{
	u16					dummy16;
	// generic
	P.r_begin			(dummy16		);	R_ASSERT(M_SPAWN==dummy16);
	P.r_string			(s_name			);
	P.r_string			(s_name_replace	);
	P.r_u8				(s_gameid		);
	P.r_u8				(s_RP			);
	P.r_vec3			(o_Position		);
	P.r_vec3			(o_Angle		);
	P.r_u16				(RespawnTime	);
	P.r_u16				(ID				);
	P.r_u16				(ID_Parent		);
	P.r_u16				(ID_Phantom		);
	P.r_u16				(s_flags		); 

	// read specific data
	u16					size;
	P.r_u16				(size			);	// size
	STATE_Read			(P,size			);
}

#ifdef _EDITOR
xr_token game_types[]={
	{ "Any game",		GAME_ANY		},
	{ "Single",			GAME_SINGLE		},
	{ "Deathmatch",		GAME_DEATHMATCH },
	{ "CTF",			GAME_CTF		},
	{ "Assault",		GAME_ASSAULT	},
	{ "Counter Strike",	GAME_CS			},
	{ 0,				0				}
};
void	xrServerEntity::FillProp	(LPCSTR pref, PropValueVec& values)
{
	FILL_PROP_EX(values,	pref, "Game Type",		&s_gameid, 		PHelper.CreateToken(game_types,1));
	FILL_PROP_EX(values,	pref, "Active",			&s_flags, 		PHelper.CreateFlag(M_SPAWN_OBJECT_ACTIVE));
	FILL_PROP_EX(values,	pref, "Respawn Time (s)",&RespawnTime,	PHelper.CreateU16(0,43200));
}
#endif

xrSE_Weapon::xrSE_Weapon()
{
	a_current			= 90;
	a_elapsed			= 30;
}
void xrSE_Weapon::UPDATE_Read		(NET_Packet& P)
{
	P.r_u32				(timestamp);
	P.r_u8				(flags);
	P.r_u8				(state);

	P.r_u16				(a_current);
	P.r_u16				(a_elapsed);

	P.r_vec3			(o_Position	);
	P.r_angle8			(o_Angle.x	);
	P.r_angle8			(o_Angle.y	);
	P.r_angle8			(o_Angle.z	);
}
void	xrSE_Weapon::UPDATE_Write	(NET_Packet& P)
{
	P.w_u32				(timestamp);
	P.w_u8				(flags);
	P.w_u8				(state);

	P.w_u16				(a_current);
	P.w_u16				(a_elapsed);

	P.w_vec3			(o_Position	);
	P.w_angle8			(o_Angle.x	);
	P.w_angle8			(o_Angle.y	);
	P.w_angle8			(o_Angle.z	);
}
void	xrSE_Weapon::STATE_Read		(NET_Packet& P, u16 size)
{
	P.r_u16				(a_current);
	P.r_u16				(a_elapsed);
}
void	xrSE_Weapon::STATE_Write	(NET_Packet& P)
{
	P.w_u16				(a_current);
	P.w_u16				(a_elapsed);
}
#ifdef _EDITOR
void	xrSE_Weapon::FillProp		(LPCSTR pref, PropValueVec& values)
    {
    	inherited::FillProp(pref, values);
      	FILL_PROP_EX(values,PHelper.PrepareKey(pref,s_name), "Ammo: total",		&a_current, PHelper.CreateU16(0,1000,1));
        FILL_PROP_EX(values,PHelper.PrepareKey(pref,s_name), "Ammo: in magazine",	&a_elapsed, PHelper.CreateU16(0,30,1));
    }
#endif

//
xrSE_Teamed::xrSE_Teamed()
{
	s_team = s_squad = s_group = 0;
}

void	xrSE_Teamed::STATE_Read			(NET_Packet& P, u16 size)
{
	P.r_u8				(s_team	);
	P.r_u8				(s_squad);
	P.r_u8				(s_group);
}
void	xrSE_Teamed::STATE_Write		(NET_Packet& P)
{
	P.w_u8				(s_team	);
	P.w_u8				(s_squad);
	P.w_u8				(s_group);
}
void	xrSE_Teamed::UPDATE_Read		(NET_Packet& P)	{};
void	xrSE_Teamed::UPDATE_Write		(NET_Packet& P)	{};
#ifdef _EDITOR
void	xrSE_Teamed::FillProp			(LPCSTR pref, PropValueVec& values)
{
  	inherited::FillProp(pref,values);
   	FILL_PROP_EX(values,PHelper.PrepareKey(pref,s_name), "Team",	&s_team, 	PHelper.CreateU8(0,64,1));
    FILL_PROP_EX(values,PHelper.PrepareKey(pref,s_name), "Squad",	&s_squad, 	PHelper.CreateU8(0,64,1));
    FILL_PROP_EX(values,PHelper.PrepareKey(pref,s_name), "Group",	&s_group, 	PHelper.CreateU8(0,64,1));
}
#endif

//***** Dummy
xrSE_Dummy::xrSE_Dummy()
{
	s_Animation	= 0;
	s_Model		= 0;
	s_Particles	= 0;
	s_Sound		= 0;
}
xrSE_Dummy::~xrSE_Dummy()
{
	_FREE				(s_Animation	);
	_FREE				(s_Model		);
	_FREE				(s_Particles	);
	_FREE				(s_Sound		);
}
void xrSE_Dummy::STATE_Read			(NET_Packet& P, u16 size)
{
	P.r_u8				(s_style);

	if (s_style&esAnimated)		{
		// Load animator
		string256				fn;
		P.r_string				(fn);
		s_Animation				= xr_strdup(fn);
	}
	if (s_style&esModel)			{
		// Load model
		string256				fn;
		P.r_string				(fn);
		s_Model					= xr_strdup(fn);
	}
	if (s_style&esParticles)		{
		// Load model
		string256				fn;
		P.r_string				(fn);
		s_Particles				= xr_strdup(fn);
	}
	if (s_style&esSound)			{
		// Load model
		string256				fn;
		P.r_string				(fn);
		s_Sound					= xr_strdup(fn);
	}
}
void xrSE_Dummy::STATE_Write		(NET_Packet& P)
{
	P.w_u8				(s_style		);
	if (s_style&esAnimated)		P.w_string			(s_Animation	);
	if (s_style&esModel)		P.w_string			(s_Model		);
	if (s_style&esParticles)	P.w_string			(s_Particles	);
	if (s_style&esSound)		P.w_string			(s_Sound		);
}
void xrSE_Dummy::UPDATE_Read		(NET_Packet& P)	{};
void xrSE_Dummy::UPDATE_Write		(NET_Packet& P)	{};
#ifdef _EDITOR
void	xrSE_Dummy::FillProp			(LPCSTR pref, PropValueVec& values)
{
  	inherited::FillProp(pref,values);
}
#endif

//***** MercuryBall
xrSE_MercuryBall::xrSE_MercuryBall()
{
	s_Model[0]	=	0;
}
void	xrSE_MercuryBall::UPDATE_Read	(NET_Packet& P)				{}
void	xrSE_MercuryBall::UPDATE_Write	(NET_Packet& P)				{}
void	xrSE_MercuryBall::STATE_Read	(NET_Packet& P, u16 size)	{/* P.r_string(s_Model); */}
void	xrSE_MercuryBall::STATE_Write	(NET_Packet& P)				{ P.w_string(s_Model); }

#ifdef _EDITOR
void	xrSE_MercuryBall::FillProp	(LPCSTR pref, PropValueVec& values)
{
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,s_name), "Model",		s_Model, 		PHelper.CreateLibObject(sizeof(s_Model)));
}
#endif
//


//***** Car
void xrSE_Car::STATE_Read			(NET_Packet& P, u16 size)	{inherited::STATE_Read(P,size); };
void xrSE_Car::STATE_Write			(NET_Packet& P)				{inherited::STATE_Write(P);		};
void xrSE_Car::UPDATE_Read			(NET_Packet& P)	{};
void xrSE_Car::UPDATE_Write			(NET_Packet& P)	{};
#ifdef _EDITOR
void xrSE_Car::FillProp				(LPCSTR pref, PropValueVec& values)
{
  	inherited::FillProp(pref,values);
}
#endif

//***** Crow
void xrSE_Crow::STATE_Read			(NET_Packet& P, u16 size)	{};
void xrSE_Crow::STATE_Write			(NET_Packet& P)				{};
void xrSE_Crow::UPDATE_Read			(NET_Packet& P)				{};
void xrSE_Crow::UPDATE_Write		(NET_Packet& P)				{};
#ifdef _EDITOR
void xrSE_Crow::FillProp			(LPCSTR pref, PropValueVec& values)
{
  	inherited::FillProp(pref,values);
}
#endif

//***** Target
void xrSE_Target::STATE_Read		(NET_Packet& P, u16 size)	{};
void xrSE_Target::STATE_Write		(NET_Packet& P)				{};
void xrSE_Target::UPDATE_Read		(NET_Packet& P)				{};
void xrSE_Target::UPDATE_Write		(NET_Packet& P)				{};
#ifdef _EDITOR
void xrSE_Target::FillProp			(LPCSTR pref, PropValueVec& values)
{
	inherited::FillProp(pref,values);
}
#endif

//***** Target Assault
void xrSE_Target_Assault::STATE_Read		(NET_Packet& P, u16 size)	{};
void xrSE_Target_Assault::STATE_Write		(NET_Packet& P)				{};
void xrSE_Target_Assault::UPDATE_Read		(NET_Packet& P)				{};
void xrSE_Target_Assault::UPDATE_Write		(NET_Packet& P)				{};
#ifdef _EDITOR
void xrSE_Target_Assault::FillProp			(LPCSTR pref, PropValueVec& values)
{
	inherited::FillProp(pref,values);
}
#endif

//***** Health
void xrSE_Health::STATE_Read		(NET_Packet& P, u16 size)	{	P.r_u8(amount); };
void xrSE_Health::STATE_Write		(NET_Packet& P)				{	P.w_u8(amount);	};
void xrSE_Health::UPDATE_Read		(NET_Packet& P)				{};
void xrSE_Health::UPDATE_Write		(NET_Packet& P)				{};
#ifdef _EDITOR
void xrSE_Health::FillProp			(LPCSTR pref, PropValueVec& values)
{
	inherited::FillProp(pref,values);
	FILL_PROP_EX(values,	PHelper.PrepareKey(pref,s_name), "Health amount",	&amount,	PHelper.CreateU8(0,255));
}
#endif

//***** Actor
void xrSE_Actor::STATE_Read			(NET_Packet& P, u16 size)	{inherited::STATE_Read(P,size); };
void xrSE_Actor::STATE_Write		(NET_Packet& P)				{inherited::STATE_Write(P);		};
void xrSE_Actor::UPDATE_Read		(NET_Packet& P)
{
	P.r_u32				(timestamp	);
	P.r_u8				(flags		);
	P.r_vec3			(o_Position	);
	P.r_u16				(mstate		);
	P.r_angle8			(model_yaw	);
	P.r_angle8			(torso.yaw	);
	P.r_angle8			(torso.pitch);
	P.r_sdir			(accel		);
	P.r_sdir			(velocity	);
	P.r_float_q16		(fHealth,	-1000,1000);
	P.r_float_q16		(fArmor,	-1000,1000);
	P.r_u8				(weapon		);
};
void xrSE_Actor::UPDATE_Write		(NET_Packet& P)
{
	P.w_u32				(timestamp	);
	P.w_u8				(flags		);
	P.w_vec3			(o_Position	);
	P.w_u16				(mstate		);
	P.w_angle8			(model_yaw	);
	P.w_angle8			(torso.yaw	);
	P.w_angle8			(torso.pitch);
	P.w_sdir			(accel		);
	P.w_sdir			(velocity	);
	P.w_float_q16		(fHealth,	-1000,1000);
	P.w_float_q16		(fArmor,	-1000,1000);
	P.w_u8				(weapon		);
}
#ifdef _EDITOR
void	xrSE_Actor::FillProp			(LPCSTR pref, PropValueVec& values)
{
  	inherited::FillProp(pref,values);
}
#endif

//***** Enemy
void xrSE_Enemy::STATE_Read			(NET_Packet& P, u16 size)	{inherited::STATE_Read(P,size); };
void xrSE_Enemy::STATE_Write		(NET_Packet& P)				{inherited::STATE_Write(P);		};
void xrSE_Enemy::UPDATE_Read		(NET_Packet& P)
{
	P.r_u32				(timestamp		);
	P.r_u8				(flags			);
	P.r_vec3			(o_Position		);
	P.r_angle8			(o_model		);
	P.r_angle8			(o_torso.yaw	);
	P.r_angle8			(o_torso.pitch	);
}
void xrSE_Enemy::UPDATE_Write		(NET_Packet& P)
{
	P.w_u32				(timestamp		);
	P.w_u8				(flags			);
	P.w_vec3			(o_Position		);
	P.w_angle8			(o_model		);
	P.w_angle8			(o_torso.yaw	);
	P.w_angle8			(o_torso.pitch	);
}
#ifdef _EDITOR
void	xrSE_Enemy::FillProp			(LPCSTR pref, PropValueVec& values)
{
  	inherited::FillProp(pref,values);
}
#endif

//***** CFormed (Base)
class xrSE_CFormed
{
public:
	union shape_data
	{
		Fsphere		sphere;
		Fmatrix		box;
	};
	struct shape_def
	{
		u8			type;
		shape_data	data;
	};
	vector<shape_def>	shapes;
public:
	void					cform_read			(NET_Packet& P)
	{
		shapes.clear();
		u8	count;	P.r_u8(count);
		while (count) {
			shape_def	S;
			P.r_u8	(S.type);
			switch	(S.type)
			{
			case 0:	P.r			(&S.data.sphere,sizeof(S.data.sphere));	break;
			case 1:	P.r_matrix	(S.data.box);							break;
			}
			shapes.push_back	(S);
			count--;
		}
	}
	void					cform_write			(NET_Packet& P)
	{
		P.w_u8	(u8(shapes.size()));
		for (u32 i=0; i<shapes.size(); i++)
		{
			shape_def&	S	= shapes[i];
			P.w_u8	(S.type);
			switch (S.type)
			{
			case 0:	P.w			(&S.data.sphere,sizeof(S.data.sphere));	break;
			case 1:	P.w_matrix	(S.data.box);							break;
			}
		}
	}
};

class xrSE_Event : public xrSE_CFormed, public xrServerEntity
{
	typedef	xrServerEntity		inherited;
public:	// actions
	struct tAction
	{
		u8		type;
		u16		count;
		u64		cls;
		LPSTR	event;
	};
	vector<tAction>			Actions;

	void					Actions_clear()
	{
		for (u32 a=0; a<Actions.size(); a++)
			_FREE(Actions[a].event);
		Actions.clear	();
	}
public:	
	virtual void			UPDATE_Read			(NET_Packet& P)
	{
	}
	virtual void			UPDATE_Write		(NET_Packet& P)
	{
	}
	virtual void			STATE_Read			(NET_Packet& P, u16 size)
	{
		// CForm
		cform_read			(P);

		// Actions
		Actions_clear		();
		u8 count;	P.r_u8	(count);
		while (count)	{
			tAction		A;
			string512	str;
			P.r_u8		(A.type);
			P.r_u16		(A.count);
			P.r_u64		(A.cls);
			P.r_string	(str);
			A.event		= xr_strdup(str);
			Actions.push_back(A);
			count--;
		}
	}
	virtual void			STATE_Write			(NET_Packet& P)
	{
		// CForm
		cform_write			(P);

		// Actions
		P.w_u8				(u8(Actions.size()));
		for (u32 i=0; i<Actions.size(); i++)
		{
			tAction&	A = Actions[i];
			P.w_u8		(A.type	);
			P.w_u16		(A.count);
			P.w_u64		(A.cls	);
			P.w_string	(A.event);
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// Rat
//////////////////////////////////////////////////////////////////////////

xrSE_Rat::xrSE_Rat()
{
	caModel[0]						= 0;
	// personal charactersitics
	fEyeFov							= 120;
	fEyeRange						= 10;
	fHealth							= 5;
	fMinSpeed						= .5;
	fMaxSpeed						= 1.5;
	fAttackSpeed					= 4.0;
	fMaxPursuitRadius				= 100;
	fMaxHomeRadius					= 10;
	// morale
	fMoraleSuccessAttackQuant		= 20;
	fMoraleDeathQuant				= -10;
	fMoraleFearQuant				= -20;
	fMoraleRestoreQuant				=  10;
	u16MoraleRestoreTimeInterval	= 3000;
	fMoraleMinValue					= 0;
	fMoraleMaxValue					= 100;
	fMoraleNormalValue				= 66;
	// attack
	fHitPower						= 10.0;
	u16HitInterval					= 1500;
	fAttackDistance					= 0.7f;
	fAttackAngle					= 45;
	fAttackSuccessProbability		= 0.5f;
}

void xrSE_Rat::STATE_Read(NET_Packet& P, u16 size)
{
	// inherited properties
	inherited::STATE_Read(P,size);
	// model
	P.r_string(caModel);
	// personal characteristics
	P.r_float (fEyeFov);
	P.r_float (fEyeRange);
	P.r_float (fHealth);
	P.r_float (fMinSpeed);
	P.r_float (fMaxSpeed);
	P.r_float (fAttackSpeed);
	P.r_float (fMaxPursuitRadius);
	P.r_float (fMaxHomeRadius);
	// morale
	P.r_float (fMoraleSuccessAttackQuant);
	P.r_float (fMoraleDeathQuant);
	P.r_float (fMoraleFearQuant);
	P.r_float (fMoraleRestoreQuant);
	P.r_u16   (u16MoraleRestoreTimeInterval);
	P.r_float (fMoraleMinValue);
	P.r_float (fMoraleMaxValue);
	P.r_float (fMoraleNormalValue);
	// attack
	P.r_float (fHitPower);
	P.r_u16   (u16HitInterval);
	P.r_float (fAttackDistance);
	P.r_float (fAttackAngle);
	P.r_float (fAttackSuccessProbability);
}

void xrSE_Rat::STATE_Write(NET_Packet& P)
{
	// inherited properties
	inherited::STATE_Write(P);
	// model
	P.w_string(caModel);
	// personal characteristics
	P.w_float (fEyeFov);
	P.w_float (fEyeRange);
	P.w_float (fHealth);
	P.w_float (fMinSpeed);
	P.w_float (fMaxSpeed);
	P.w_float (fAttackSpeed);
	P.w_float (fMaxPursuitRadius);
	P.w_float (fMaxHomeRadius);
	// morale
	P.w_float (fMoraleSuccessAttackQuant);
	P.w_float (fMoraleDeathQuant);
	P.w_float (fMoraleFearQuant);
	P.w_float (fMoraleRestoreQuant);
	P.w_u16   (u16MoraleRestoreTimeInterval);
	P.w_float (fMoraleMinValue);
	P.w_float (fMoraleMaxValue);
	P.w_float (fMoraleNormalValue);
	// attack
	P.w_float (fHitPower);
	P.w_u16   (u16HitInterval);
	P.w_float (fAttackDistance);
	P.w_float (fAttackAngle);
	P.w_float (fAttackSuccessProbability);
}

void xrSE_Rat::UPDATE_Read(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
}

void xrSE_Rat::UPDATE_Write(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
}

#ifdef _EDITOR
void xrSE_Rat::FillProp(LPCSTR pref, PropValueVec& values)
{
   	inherited::FillProp(pref, values);
	// model
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,s_name),								"Model",				&caModel,						PHelper.CreateLibObject(sizeof(s_Model)));
	// personal characteristics
   	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Personal")),"Field of view",		&fEyeFov,						PHelper.CreateFloat(0,170,10));
   	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Personal")),"Eye range",			&fEyeRange,						PHelper.CreateFloat(0,300,10));
   	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Personal")),"Health",				&fHealth,						PHelper.CreateFloat(0,200,5));
   	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Personal")),"Minimum speed",		&fMinSpeed,						PHelper.CreateFloat(0,10,0.1));
   	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Personal")),"Maximum speed",		&fMaxSpeed,						PHelper.CreateFloat(0,10,0.1));
   	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Personal")),"Attack speed",			&fAttackSpeed,					PHelper.CreateFloat(0,10,0.1));
   	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Personal")),"Pursuit distance",		&fMaxPursuitRadius,				PHelper.CreateFloat(0,300,10));
   	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Personal")),"Home distance",		&fMaxHomeRadius,				PHelper.CreateFloat(0,300,10));
	// morale
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Morale")),	"Success attack quant",	&fMoraleSuccessAttackQuant,		PHelper.CreateFloat(-100,100,5));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Morale")),	"Death quant",			&fMoraleDeathQuant,				PHelper.CreateFloat(-100,100,5));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Morale")),	"Fear quant",			&fMoraleFearQuant,				PHelper.CreateFloat(-100,100,5));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Morale")),	"Restore quant",		&fMoraleRestoreQuant,			PHelper.CreateFloat(-100,100,5));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Morale")),	"Restore time interval",&u16MoraleRestoreTimeInterval,	PHelper.CreateU16  (0,65535,500));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Morale")),	"Minimum value",		&fMoraleMinValue,				PHelper.CreateFloat(-100,100,5));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Morale")),	"Maximum value",		&fMoraleMaxValue,				PHelper.CreateFloat(-100,100,5));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Morale")),	"Normal value",			&fMoraleNormalValue,			PHelper.CreateFloat(-100,100,5));
	// attack
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Attack")),	"Hit power",			&fHitPower,						PHelper.CreateFloat(0,200,5));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Attack")),	"Hit interval",			&u16HitInterval,				PHelper.CreateU16  (0,65535,500));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Attack")),	"Distance",				&fAttackDistance,				PHelper.CreateFloat(0,300,10));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Attack")),	"Maximum angle",		&fAttackAngle,					PHelper.CreateFloat(0,180,10));
	FILL_PROP_EX(values, PHelper.PrepareKey(pref,PHelper.PrepareKey(s_name,"Attack")),	"Success probability",	&fAttackSuccessProbability,		PHelper.CreateFloat(0,100,1));
}
#endif

//--------------------------------------------------------------------
xrServerEntity*	F_entity_Create		(LPCSTR name)
{
	if (!pSettings->SectionExists(name)) return 0;
    
	CLASS_ID cls = pSettings->ReadCLSID(name,"class");

	switch (cls){
	case CLSID_OBJECT_ACTOR:		return new	xrSE_Actor;
	case CLSID_OBJECT_DUMMY:		return new  xrSE_Dummy;
	case CLSID_AI_RAT:				return new	xrSE_Rat;
	case CLSID_AI_SOLDIER:			return new	xrSE_Enemy;
	case CLSID_AI_ZOMBIE:			return new	xrSE_Enemy;
	case CLSID_AI_CROW:				return new	xrSE_Crow;
	case CLSID_EVENT:				return new  xrSE_Event;
	case CLSID_CAR_NIVA:			return new  xrSE_Car;

	// Artifacts
	case CLSID_AF_MERCURY_BALL:		return new  xrSE_MercuryBall;

	case CLSID_OBJECT_W_M134:		return new	xrSE_Weapon;
	case CLSID_OBJECT_W_FN2000:		return new	xrSE_Weapon;
	case CLSID_OBJECT_W_AK74:		return new	xrSE_Weapon;
	case CLSID_OBJECT_W_LR300:		return new	xrSE_Weapon;
	case CLSID_OBJECT_W_HPSA:		return new	xrSE_Weapon;
	case CLSID_OBJECT_W_PM:			return new	xrSE_Weapon;
	case CLSID_OBJECT_W_FORT:		return new	xrSE_Weapon;
	case CLSID_OBJECT_W_BINOCULAR:	return new	xrSE_Weapon;
	case CLSID_OBJECT_W_SHOTGUN:	return new	xrSE_Weapon;
	case CLSID_OBJECT_HEALTH:		return new	xrSE_Health;
	case CLSID_TARGET_ASSAULT:		return new	xrSE_Target_Assault;
	}
	return 0;
}

void			F_entity_Destroy	(xrServerEntity* P)
{
	_DELETE		(P);
}
