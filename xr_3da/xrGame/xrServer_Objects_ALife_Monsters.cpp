////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects monsters for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"

////////////////////////////////////////////////////////////////////////////
// CALifeTraderAbstract
////////////////////////////////////////////////////////////////////////////
void CALifeTraderAbstract::STATE_Write		(NET_Packet &tNetPacket)
{
	save_vector					(m_tpEvents,tNetPacket);
	save_base_vector			(m_tpTaskIDs,tNetPacket);
}

void CALifeTraderAbstract::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	if (m_wVersion > 19) {
		load_vector				(m_tpEvents,tNetPacket);
		load_base_vector		(m_tpTaskIDs,tNetPacket);
	}
}

void CALifeTraderAbstract::UPDATE_Write		(NET_Packet &tNetPacket)
{
	tNetPacket.w_float			(m_fCumulativeItemMass);
	tNetPacket.w_u32			(m_dwMoney);
	tNetPacket.w_u32			(m_tRank);
};

void CALifeTraderAbstract::UPDATE_Read		(NET_Packet &tNetPacket)
{
	tNetPacket.r_float			(m_fCumulativeItemMass);
	tNetPacket.r_u32			(m_dwMoney);
	u32							dwDummy;
	tNetPacket.r_u32			(dwDummy);
	m_tRank						= EStalkerRank(m_tRank);
};

////////////////////////////////////////////////////////////////////////////
// CALifeTrader
////////////////////////////////////////////////////////////////////////////
void CALifeTrader::STATE_Write				(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}

void CALifeTrader::STATE_Read				(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	inherited2::STATE_Read		(tNetPacket, size);
}

void CALifeTrader::UPDATE_Write				(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
};

void CALifeTrader::UPDATE_Read				(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
};

////////////////////////////////////////////////////////////////////////////
// CALifeCreatureAbstract
////////////////////////////////////////////////////////////////////////////
void CALifeCreatureAbstract::STATE_Write	(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u8				(s_team	);
	tNetPacket.w_u8				(s_squad);
	tNetPacket.w_u8				(s_group);
	tNetPacket.w_float			(fHealth);
	visual_write				(tNetPacket);
}

void CALifeCreatureAbstract::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
	tNetPacket.r_u8				(s_team	);
	tNetPacket.r_u8				(s_squad);
	tNetPacket.r_u8				(s_group);
	if (m_wVersion > 18)
		tNetPacket.r_float		(fHealth);
	visual_read					(tNetPacket);
}

void CALifeCreatureAbstract::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	
	tNetPacket.w_float_q16		(fHealth,	-1000,1000);
	
	tNetPacket.w_u32			(timestamp		);
	tNetPacket.w_u8				(flags			);
	tNetPacket.w_vec3			(o_Position		);
	tNetPacket.w_angle8			(o_model		);
	tNetPacket.w_angle8			(o_torso.yaw	);
	tNetPacket.w_angle8			(o_torso.pitch	);
};

void CALifeCreatureAbstract::UPDATE_Read	(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	
	tNetPacket.r_float_q16		(fHealth,	-1000,1000);
	
	tNetPacket.r_u32			(timestamp		);
	tNetPacket.r_u8				(flags			);
	tNetPacket.r_vec3			(o_Position		);
	tNetPacket.r_angle8			(o_model		);
	tNetPacket.r_angle8			(o_torso.yaw	);
	tNetPacket.r_angle8			(o_torso.pitch	);
};

#ifdef _EDITOR
void CALifeCreatureAbstract::FillProp		(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp			(pref,items);
    PHelper.CreateU8			(items,PHelper.PrepareKey(pref,s_name, "Team"),		&s_team, 	0,64,1);
    PHelper.CreateU8			(items,PHelper.PrepareKey(pref,s_name, "Squad"),	&s_squad, 	0,64,1);
    PHelper.CreateU8			(items,PHelper.PrepareKey(pref,s_name, "Group"),	&s_group, 	0,64,1);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Health" 				),&fHealth,							0,200,5);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeMonsterAbstract
////////////////////////////////////////////////////////////////////////////
void CALifeMonsterAbstract::STATE_Write		(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeMonsterAbstract::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CALifeMonsterAbstract::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
	tNetPacket.w				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
	tNetPacket.w				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	tNetPacket.w				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
	tNetPacket.w				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
	tNetPacket.w				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
};

void CALifeMonsterAbstract::UPDATE_Read		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
	tNetPacket.r				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
	tNetPacket.r				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	tNetPacket.r				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
	tNetPacket.r				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
	tNetPacket.r				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
};

#ifdef _EDITOR
void CALifeMonsterAbstract::FillProp		(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp			(pref,items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeCreatureActor
////////////////////////////////////////////////////////////////////////////
CALifeCreatureActor::CALifeCreatureActor	(LPCSTR caSection) : CALifeCreatureAbstract(caSection), CALifeTraderAbstract(caSection), CAbstractServerObject(caSection)
{
	set_visual					("actors\\Different_stalkers\\stalker_hood_multiplayer.ogf");
}

void CALifeCreatureActor::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion < 21) {
		CALifeDynamicObject::STATE_Read(tNetPacket,size);
		tNetPacket.r_u8			(s_team	);
		tNetPacket.r_u8			(s_squad);
		tNetPacket.r_u8			(s_group);
		if (m_wVersion > 18)
			tNetPacket.r_float	(fHealth);
		if (m_wVersion >= 3)
			visual_read			(tNetPacket);
	}
	else {
		inherited1::STATE_Read	(tNetPacket,size);
		inherited2::STATE_Read	(tNetPacket,size);
		visual_read				(tNetPacket);
	}
};

void CALifeCreatureActor::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
	visual_write				(tNetPacket);
};

void CALifeCreatureActor::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
	tNetPacket.r_u16			(mstate		);
	tNetPacket.r_sdir			(accel		);
	tNetPacket.r_sdir			(velocity	);
	tNetPacket.r_float_q16		(fArmor,	-1000,1000);
	tNetPacket.r_u8				(weapon		);
};
void CALifeCreatureActor::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
	tNetPacket.w_u16			(mstate		);
	tNetPacket.w_sdir			(accel		);
	tNetPacket.w_sdir			(velocity	);
	tNetPacket.w_float_q16		(fArmor,	-1000,1000);
	tNetPacket.w_u8				(weapon		);
}

#ifdef _EDITOR
void CALifeCreatureActor::FillProp			(LPCSTR pref, PropItemVec& items)
{
  	inherited1::FillProp		(pref,items);
  	inherited2::FillProp		(pref,items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeMonsterCrow
////////////////////////////////////////////////////////////////////////////
void CALifeMonsterCrow::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20) {
		inherited::STATE_Read	(tNetPacket,size);
		visual_read				(tNetPacket);
	}
}

void CALifeMonsterCrow::STATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	visual_write				(tNetPacket);
}

void CALifeMonsterCrow::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CALifeMonsterCrow::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CALifeMonsterCrow::FillProp			(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeMonsterRat
////////////////////////////////////////////////////////////////////////////
CALifeMonsterRat::CALifeMonsterRat			(LPCSTR caSection) : CALifeMonsterAbstract(caSection), CAbstractServerObject(caSection)
{
	set_visual					("monsters\\rat\\rat_1");
	// personal charactersitics
	fEyeFov						= 120;
	fEyeRange					= 10;
	fHealth						= 5;
	fMinSpeed					= .5;
	fMaxSpeed					= 1.5;
	fAttackSpeed				= 4.0;
	fMaxPursuitRadius			= 100;
	fMaxHomeRadius				= 10;
	// morale
	fMoraleSuccessAttackQuant	= 20;
	fMoraleDeathQuant			= -10;
	fMoraleFearQuant			= -20;
	fMoraleRestoreQuant			=  10;
	u16MoraleRestoreTimeInterval= 3000;
	fMoraleMinValue				= 0;
	fMoraleMaxValue				= 100;
	fMoraleNormalValue			= 66;
	// attack
	fHitPower					= 10.0;
	u16HitInterval				= 1500;
	fAttackDistance				= 0.7f;
	fAttackAngle				= 45;
	fAttackSuccessProbability	= 0.5f;
}

void CALifeMonsterRat::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	// inherited properties
	inherited::STATE_Read		(tNetPacket,size);
	// personal characteristics
	tNetPacket.r_float			(fEyeFov);
	tNetPacket.r_float			(fEyeRange);
	if (m_wVersion <= 5)
		tNetPacket.r_float		(fHealth);
	tNetPacket.r_float			(fMinSpeed);
	tNetPacket.r_float			(fMaxSpeed);
	tNetPacket.r_float			(fAttackSpeed);
	tNetPacket.r_float			(fMaxPursuitRadius);
	tNetPacket.r_float			(fMaxHomeRadius);
	// morale
	tNetPacket.r_float			(fMoraleSuccessAttackQuant);
	tNetPacket.r_float			(fMoraleDeathQuant);
	tNetPacket.r_float			(fMoraleFearQuant);
	tNetPacket.r_float			(fMoraleRestoreQuant);
	tNetPacket.r_u16			(u16MoraleRestoreTimeInterval);
	tNetPacket.r_float			(fMoraleMinValue);
	tNetPacket.r_float			(fMoraleMaxValue);
	tNetPacket.r_float			(fMoraleNormalValue);
	// attack
	tNetPacket.r_float			(fHitPower);
	tNetPacket.r_u16			(u16HitInterval);
	tNetPacket.r_float			(fAttackDistance);
	tNetPacket.r_float			(fAttackAngle);
	tNetPacket.r_float			(fAttackSuccessProbability);
}

void CALifeMonsterRat::STATE_Write			(NET_Packet	&tNetPacket)
{
	// inherited properties
	inherited::STATE_Write		(tNetPacket);
	// personal characteristics
	tNetPacket.w_float			(fEyeFov);
	tNetPacket.w_float			(fEyeRange);
	tNetPacket.w_float			(fMinSpeed);
	tNetPacket.w_float			(fMaxSpeed);
	tNetPacket.w_float			(fAttackSpeed);
	tNetPacket.w_float			(fMaxPursuitRadius);
	tNetPacket.w_float			(fMaxHomeRadius);
	// morale
	tNetPacket.w_float			(fMoraleSuccessAttackQuant);
	tNetPacket.w_float			(fMoraleDeathQuant);
	tNetPacket.w_float			(fMoraleFearQuant);
	tNetPacket.w_float			(fMoraleRestoreQuant);
	tNetPacket.w_u16			(u16MoraleRestoreTimeInterval);
	tNetPacket.w_float			(fMoraleMinValue);
	tNetPacket.w_float			(fMoraleMaxValue);
	tNetPacket.w_float			(fMoraleNormalValue);
	// attack
	tNetPacket.w_float			(fHitPower);
	tNetPacket.w_u16			(u16HitInterval);
	tNetPacket.w_float			(fAttackDistance);
	tNetPacket.w_float			(fAttackAngle);
	tNetPacket.w_float			(fAttackSuccessProbability);
}

void CALifeMonsterRat::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CALifeMonsterRat::UPDATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CALifeMonsterRat::FillProp				(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProp			(pref, items);
	// personal characteristics
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Field of view" 		),&fEyeFov,							0,170,10);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Eye range" 			),&fEyeRange,						0,300,10);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Minimum speed" 		),&fMinSpeed,						0,10,0.1);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Maximum speed" 		),&fMaxSpeed,						0,10,0.1);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Attack speed" 			),&fAttackSpeed,					0,10,0.1);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Pursuit distance" 		),&fMaxPursuitRadius,				0,300,10);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Home distance" 		),&fMaxHomeRadius,					0,300,10);
	// morale																			
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Morale",		"Success attack quant" 	),&fMoraleSuccessAttackQuant,		-100,100,5);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Morale",		"Death quant" 			),&fMoraleDeathQuant,				-100,100,5);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Morale",		"Fear quant" 			),&fMoraleFearQuant,				-100,100,5);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Morale",		"Restore quant" 		),&fMoraleRestoreQuant,				-100,100,5);
	PHelper.CreateU16  			(items, PHelper.PrepareKey(pref,s_name,"Morale",		"Restore time interval" ),&u16MoraleRestoreTimeInterval,	0,65535,500);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Morale",		"Minimum value" 		),&fMoraleMinValue,					-100,100,5);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Morale",		"Maximum value" 		),&fMoraleMaxValue,					-100,100,5);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Morale",		"Normal value" 			),&fMoraleNormalValue,				-100,100,5);
	// attack																			 	
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit power" 			),&fHitPower,						0,200,5);
	PHelper.CreateU16  			(items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit interval" 			),&u16HitInterval,					0,65535,500);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Attack",		"Distance" 				),&fAttackDistance,					0,300,10);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Attack",		"Maximum angle" 		),&fAttackAngle,					0,180,10);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Attack",		"Success probability" 	),&fAttackSuccessProbability,		0,100,1);
}	
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeMonsterZombie
////////////////////////////////////////////////////////////////////////////
CALifeMonsterZombie::CALifeMonsterZombie	(LPCSTR caSection) : CALifeMonsterAbstract(caSection), CAbstractServerObject(caSection)
{
	set_visual					("monsters\\zombie\\zombie_1");
	// personal charactersitics
	fEyeFov						= 120;
	fEyeRange					= 30;
	fHealth						= 200;
	fMinSpeed					= 1.5;
	fMaxSpeed					= 1.75;
	fAttackSpeed				= 2.0;
	fMaxPursuitRadius			= 100;
	fMaxHomeRadius				= 30;
	// attack
	fHitPower					= 20.0;
	u16HitInterval				= 1000;
	fAttackDistance				= 1.0f;
	fAttackAngle				= 15;
}

void CALifeMonsterZombie::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	// inherited properties
	inherited::STATE_Read		(tNetPacket,size);
	// personal characteristics
	tNetPacket.r_float			(fEyeFov);
	tNetPacket.r_float			(fEyeRange);
	if (m_wVersion <= 5)
		tNetPacket.r_float		(fHealth);
	tNetPacket.r_float			(fMinSpeed);
	tNetPacket.r_float			(fMaxSpeed);
	tNetPacket.r_float			(fAttackSpeed);
	tNetPacket.r_float			(fMaxPursuitRadius);
	tNetPacket.r_float			(fMaxHomeRadius);
	// attack
	tNetPacket.r_float			(fHitPower);
	tNetPacket.r_u16			(u16HitInterval);
	tNetPacket.r_float			(fAttackDistance);
	tNetPacket.r_float			(fAttackAngle);
}

void CALifeMonsterZombie::STATE_Write		(NET_Packet	&tNetPacket)
{
	// inherited properties
	inherited::STATE_Write		(tNetPacket);
	// personal characteristics
	tNetPacket.w_float			(fEyeFov);
	tNetPacket.w_float			(fEyeRange);
	tNetPacket.w_float			(fMinSpeed);
	tNetPacket.w_float			(fMaxSpeed);
	tNetPacket.w_float			(fAttackSpeed);
	tNetPacket.w_float			(fMaxPursuitRadius);
	tNetPacket.w_float			(fMaxHomeRadius);
	// attack
	tNetPacket.w_float			(fHitPower);
	tNetPacket.w_u16			(u16HitInterval);
	tNetPacket.w_float			(fAttackDistance);
	tNetPacket.w_float			(fAttackAngle);
}

void CALifeMonsterZombie::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CALifeMonsterZombie::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CALifeMonsterZombie::FillProp			(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProp			(pref, items);
	// personal characteristics
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Field of view" 		),&fEyeFov,							0,170,10);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Eye range" 			),&fEyeRange,						0,300,10);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Minimum speed" 		),&fMinSpeed,						0,10,0.1);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Maximum speed" 		),&fMaxSpeed,						0,10,0.1);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Attack speed" 			),&fAttackSpeed,					0,10,0.1);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Pursuit distance" 		),&fMaxPursuitRadius,				0,300,10);
   	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Personal",	"Home distance" 		),&fMaxHomeRadius,					0,300,10);
	// attack																			 	
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit power" 			),&fHitPower,						0,200,5);
	PHelper.CreateU16  			(items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit interval" 			),&u16HitInterval,					0,65535,500);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Attack",		"Distance" 				),&fAttackDistance,					0,300,10);
	PHelper.CreateFloat			(items, PHelper.PrepareKey(pref,s_name,"Attack",		"Maximum angle" 		),&fAttackAngle,					0,100,1);
}
#endif

//////////////////////////////////////////////////////////////////////////
// CALifeMonsterBiting
//////////////////////////////////////////////////////////////////////////
CALifeMonsterBiting::CALifeMonsterBiting	(LPCSTR caSection) : CALifeMonsterAbstract(caSection), CAbstractServerObject(caSection)
{
    set_visual					(pSettings->r_string(caSection,"visual"));
}

void CALifeMonsterBiting::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CALifeMonsterBiting::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeMonsterBiting::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CALifeMonsterBiting::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

//////////////////////////////////////////////////////////////////////////
// CALifeHumanAbstract
//////////////////////////////////////////////////////////////////////////
void CALifeHumanAbstract::STATE_Write		(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
	save_base_vector			(m_tpaVertices,tNetPacket);
	save_bool_vector			(m_baVisitedVertices,tNetPacket);
	save_vector					(m_tpTasks,tNetPacket);
}

void CALifeHumanAbstract::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	inherited2::STATE_Read		(tNetPacket, size);
	if (m_wVersion > 19) {
		load_base_vector		(m_tpaVertices,tNetPacket);
		load_bool_vector		(m_baVisitedVertices,tNetPacket);
		load_vector				(m_tpTasks,tNetPacket);
	}
}

void CALifeHumanAbstract::UPDATE_Write		(NET_Packet &tNetPacket)
{
	// calling inherited
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
	tNetPacket.w				(&m_tTaskState,sizeof(m_tTaskState));
	tNetPacket.w_u32			(m_dwCurTaskLocation);
	tNetPacket.w_u32			(m_dwCurTask);
};

void CALifeHumanAbstract::UPDATE_Read		(NET_Packet &tNetPacket)
{
	// calling inherited
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
	tNetPacket.r				(&m_tTaskState,sizeof(m_tTaskState));
	tNetPacket.r_u32			(m_dwCurTaskLocation);
	tNetPacket.r_u32			(m_dwCurTask);
};

#ifdef _EDITOR
void CALifeHumanAbstract::FillProp			(LPCSTR pref, PropItemVec& items)
{
  	inherited1::FillProp		(pref,items);
  	inherited2::FillProp		(pref,items);
}
#endif

//////////////////////////////////////////////////////////////////////////
// CALifeHumanStalker
//////////////////////////////////////////////////////////////////////////
void CALifeHumanStalker::STATE_Write		(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeHumanStalker::STATE_Read			(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CALifeHumanStalker::UPDATE_Write		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
};

void CALifeHumanStalker::UPDATE_Read		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
};

//////////////////////////////////////////////////////////////////////////
// CALifeObjectIdol
//////////////////////////////////////////////////////////////////////////
CALifeObjectIdol::CALifeObjectIdol			(LPCSTR caSection) : CALifeHumanAbstract(caSection), CAbstractServerObject(caSection)
{
	m_dwAniPlayType				= 0;
	m_caAnimations[0]			= 0;
}

void CALifeObjectIdol::STATE_Read			(NET_Packet& tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_string			(m_caAnimations);
	tNetPacket.r_u32			(m_dwAniPlayType);
}

void CALifeObjectIdol::STATE_Write			(NET_Packet& tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_string			(m_caAnimations);
	tNetPacket.w_u32			(m_dwAniPlayType);
}

void CALifeObjectIdol::UPDATE_Read			(NET_Packet& tNetPacket)
{
}

void CALifeObjectIdol::UPDATE_Write			(NET_Packet& tNetPacket)
{
}

#ifdef _EDITOR
void CALifeObjectIdol::FillProp(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProp			(pref, items);
    PHelper.CreateText			(items, PHelper.PrepareKey(pref,s_name,"Idol", "Animations"),m_caAnimations,sizeof(m_caAnimations));
   	PHelper.CreateU32			(items, PHelper.PrepareKey(pref,s_name,"Idol", "Animation playing type"),&m_dwAniPlayType,0,2,1);
}	
#endif
