////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife.h"

////////////////////////////////////////////////////////////////////////////
// CALifeEvent
////////////////////////////////////////////////////////////////////////////
void CALifeEvent::STATE_Write				(NET_Packet &tNetPacket)
{
}

void CALifeEvent::STATE_Read				(NET_Packet &tNetPacket, u16 size)
{
}

void CALifeEvent::UPDATE_Write				(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.w				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.w				(&m_tGraphID,		sizeof(m_tGraphID));
	tNetPacket.w				(&m_tBattleResult,	sizeof(m_tBattleResult));
	m_tpMonsterGroup1->UPDATE_Write(tNetPacket);
	m_tpMonsterGroup2->UPDATE_Write(tNetPacket);
};

void CALifeEvent::UPDATE_Read				(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.r				(&m_tGraphID,		sizeof(m_tGraphID));
	tNetPacket.r				(&m_tBattleResult,	sizeof(m_tBattleResult));
	m_tpMonsterGroup1->UPDATE_Read(tNetPacket);
	m_tpMonsterGroup2->UPDATE_Read(tNetPacket);
};

////////////////////////////////////////////////////////////////////////////
// CALifePersonalEvent
////////////////////////////////////////////////////////////////////////////
void CALifePersonalEvent::STATE_Write		(NET_Packet &tNetPacket)
{
	save_base_vector			(m_tpItemIDs,		tNetPacket);
}

void CALifePersonalEvent::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	load_base_vector			(m_tpItemIDs,		tNetPacket);
}

void CALifePersonalEvent::UPDATE_Write		(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.w				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.w				(&m_tTaskID,		sizeof(m_tTaskID));
	tNetPacket.w_u32			(m_iHealth);
	tNetPacket.w				(&m_tRelation,		sizeof(m_tRelation));
};

void CALifePersonalEvent::UPDATE_Read		(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.r				(&m_tTaskID,		sizeof(m_tTaskID));
	tNetPacket.r_s32			(m_iHealth);
	tNetPacket.r				(&m_tRelation,		sizeof(m_tRelation));
};

////////////////////////////////////////////////////////////////////////////
// CALifeTask
////////////////////////////////////////////////////////////////////////////
void CALifeTask::STATE_Write				(NET_Packet &tNetPacket)
{
}

void CALifeTask::STATE_Read					(NET_Packet &tNetPacket, u16 size)
{
}

void CALifeTask::UPDATE_Write				(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tTaskID,		sizeof(m_tTaskID));
	tNetPacket.w				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.w				(&m_tCustomerID,	sizeof(m_tCustomerID));
	tNetPacket.w_float			(m_fCost);
	tNetPacket.w				(&m_tTaskType,		sizeof(m_tTaskType));
	switch (m_tTaskType) {
		case eTaskTypeSearchForItemCL : {
			tNetPacket.w		(&m_tClassID,		sizeof(m_tClassID));
			tNetPacket.w		(&m_tLocationID,	sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemCG : {
			tNetPacket.w		(&m_tClassID,		sizeof(m_tClassID));
			tNetPacket.w		(&m_tGraphID,		sizeof(m_tGraphID));
			break;
		}
		case eTaskTypeSearchForItemOL : {
			tNetPacket.w		(&m_tObjectID,		sizeof(m_tObjectID));
			tNetPacket.w		(&m_tLocationID,	sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemOG : {
			tNetPacket.w		(&m_tObjectID,		sizeof(m_tObjectID));
			tNetPacket.w		(&m_tGraphID,		sizeof(m_tGraphID));
			break;
		}
		default : {
			break;
		}
	}
}

void CALifeTask::UPDATE_Read				(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tTaskID,	sizeof(m_tTaskID));
	tNetPacket.r				(&m_tTimeID,	sizeof(m_tTimeID));
	tNetPacket.r				(&m_tCustomerID,sizeof(m_tCustomerID));
	tNetPacket.r_float			(m_fCost);
	tNetPacket.r				(&m_tTaskType,	sizeof(m_tTaskType));
	switch (m_tTaskType) {
		case eTaskTypeSearchForItemCL : {
			tNetPacket.r		(&m_tClassID,	sizeof(m_tClassID));
			tNetPacket.r		(&m_tLocationID,sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemCG : {
			tNetPacket.r		(&m_tClassID,	sizeof(m_tClassID));
			tNetPacket.r		(&m_tGraphID,	sizeof(m_tGraphID));
			break;
		}
		case eTaskTypeSearchForItemOL : {
			tNetPacket.r		(&m_tObjectID,	sizeof(m_tObjectID));
			tNetPacket.r		(&m_tLocationID,sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemOG : {
			tNetPacket.r		(&m_tObjectID,	sizeof(m_tObjectID));
			tNetPacket.r		(&m_tGraphID,	sizeof(m_tGraphID));
			break;
		}
		default : {
			break;
		}
	};
}

////////////////////////////////////////////////////////////////////////////
// CALifePersonalTask
////////////////////////////////////////////////////////////////////////////
void CALifePersonalTask::STATE_Write		(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifePersonalTask::STATE_Read			(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CALifePersonalTask::UPDATE_Write		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w_u32			(m_dwTryCount);
};

void CALifePersonalTask::UPDATE_Read		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r_u32			(m_dwTryCount);
};

////////////////////////////////////////////////////////////////////////////
// CALifeObject
////////////////////////////////////////////////////////////////////////////
void CALifeObject::STATE_Write				(NET_Packet &tNetPacket)
{
	tNetPacket.w_u8				(m_ucProbability);
	tNetPacket.w_u32			(m_dwSpawnGroup);
	tNetPacket.w				(&m_tGraphID,	sizeof(m_tGraphID));
	tNetPacket.w_float			(m_fDistance);
	tNetPacket.w_u32			(m_bDirectControl);
	tNetPacket.w_u32			(m_tNodeID);
}

void CALifeObject::STATE_Read				(NET_Packet &tNetPacket, u16 size)
{
	if (m_wVersion >= 1) {
		tNetPacket.r_u8			(m_ucProbability);
		tNetPacket.r_u32		(m_dwSpawnGroup);
		if (m_wVersion < 4) {
			u16					wDummy;
			tNetPacket.r_u16	(wDummy);
		}
		tNetPacket.r			(&m_tGraphID,	sizeof(m_tGraphID));
		tNetPacket.r_float		(m_fDistance);
		m_tObjectID				= ID;
	}
	if (m_wVersion >= 4) {
		u32						dwDummy;
		tNetPacket.r_u32		(dwDummy);
		m_bDirectControl		= !!dwDummy;
	}
	if (m_wVersion >= 8)
		tNetPacket.r_u32		(m_tNodeID);
}

void CALifeObject::UPDATE_Write				(NET_Packet &tNetPacket)
{
}

void CALifeObject::UPDATE_Read				(NET_Packet &tNetPacket)
{
};

#ifdef _EDITOR
#include "PropertiesListHelper.h"
void CALifeObject::FillProp					(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref, items);
	PHelper.CreateU8			(items,	PHelper.PrepareKey(pref, "ALife\\Probability"),		&m_ucProbability,	0,100);
	PHelper.CreateU32			(items,	PHelper.PrepareKey(pref, "ALife\\Spawn group ID"),	&m_dwSpawnGroup,	0,0xffffffff);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeEventGroup
////////////////////////////////////////////////////////////////////////////
void CALifeEventGroup::STATE_Write			(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeEventGroup::STATE_Read			(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CALifeEventGroup::UPDATE_Write			(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w				(&m_wCountAfter,sizeof(m_wCountAfter));
};

void CALifeEventGroup::UPDATE_Read			(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r				(&m_wCountAfter,sizeof(m_wCountAfter));
};

////////////////////////////////////////////////////////////////////////////
// CALifeDynamicObject
////////////////////////////////////////////////////////////////////////////
void CALifeDynamicObject::STATE_Write		(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeDynamicObject::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CALifeDynamicObject::UPDATE_Write		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
};

void CALifeDynamicObject::UPDATE_Read		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
};

////////////////////////////////////////////////////////////////////////////
// CALifeDynamicObjectVisual
////////////////////////////////////////////////////////////////////////////
void CALifeDynamicObjectVisual::STATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
}

void CALifeDynamicObjectVisual::STATE_Read	(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
}

void CALifeDynamicObjectVisual::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
};

void CALifeDynamicObjectVisual::UPDATE_Read	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
};

#ifdef _EDITOR
void CALifeDynamicObjectVisual::FillProp	(LPCSTR pref, PropItemVec& items)
{
	inherited1::FillProp		(pref,items);
	inherited2::FillProp		(PHelper.PrepareKey(pref,s_name),values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeAnomalousZone
////////////////////////////////////////////////////////////////////////////
CALifeAnomalousZone::CALifeAnomalousZone	(LPCSTR caSection) : CALifeDynamicObject(caSection), CAbstractServerObject(caSection)
{
	m_maxPower					= 100.f;
	m_attn						= 1.f;
	m_period					= 1000;
}

void CALifeAnomalousZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)	{
	// CForm
	if (m_wVersion >= 15)
		inherited1::STATE_Read	(tNetPacket,size);
	cform_read					(tNetPacket);

	tNetPacket.r_float			(m_maxPower);
	tNetPacket.r_float			(m_attn);
	tNetPacket.r_u32			(m_period);
};
void CALifeAnomalousZone::STATE_Write		(NET_Packet	&tNetPacket)				{
	inherited1::STATE_Write		(tNetPacket);
	// CForm
	cform_write					(tNetPacket);

	tNetPacket.w_float			(m_maxPower);
	tNetPacket.w_float			(m_attn);
	tNetPacket.w_u32			(m_period);
};
void CALifeAnomalousZone::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CALifeAnomalousZone::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
}

#ifdef _EDITOR
void CALifeAnomalousZone::FillProp			(LPCSTR pref, PropItemVec& items)
{
	inherited1::FillProp		(pref,items);
	inherited2::FillProp		(pref,items);
    PHelper.CreateFloat			(items,PHelper.PrepareKey(pref,s_name,"Power"),	&m_maxPower,0.f,1000.f);
    PHelper.CreateFloat			(items,PHelper.PrepareKey(pref,s_name,"Attenuation"),	&m_attn,0.f,100.f);
    PHelper.CreateU32			(items,PHelper.PrepareKey(pref,s_name,"Period"), &m_period,20,10000);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeObjectPhysic
////////////////////////////////////////////////////////////////////////////
CALifeObjectPhysic::CALifeObjectPhysic		(LPCSTR caSection) : CALifeDynamicObjectVisual(caSection) , CAbstractServerObject(caSection)
{
	type 						= epotBox;
	mass 						= 10.f;
    fixed_bone[0]				=0;
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	set_visual				(pSettings->r_string(caSection,"visual"));
}

CALifeObjectPhysic::~CALifeObjectPhysic		() 
{
}

void CALifeObjectPhysic::STATE_Read			(NET_Packet	&tNetPacket, u16 size) 
{
	if (m_wVersion >= 14)
		if (m_wVersion >= 16) {
			inherited::STATE_Read(tNetPacket,size);
			visual_read			(tNetPacket);
		}
		else {
			CALifeObject::STATE_Read(tNetPacket,size);
			visual_read			(tNetPacket);
		}
	tNetPacket.r_u32			(type);
	tNetPacket.r_float			(mass);
    
	if (m_wVersion > 9)
		tNetPacket.r_string		(fixed_bone);
	// internal
	strlwr						(fixed_bone);
}

void CALifeObjectPhysic::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	visual_write				(tNetPacket);
	tNetPacket.w_u32			(type);
	tNetPacket.w_float			(mass);
	tNetPacket.w_string			(fixed_bone);
}

void CALifeObjectPhysic::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CALifeObjectPhysic::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
xr_token po_types[]={
	{ "Box",			epotBox			},
	{ "Fixed chain",	epotFixedChain	},
	{ "Free chain",		epotFreeChain	},
	{ "Skeleton",		epotSkeleton	},
	{ 0,				0				}
};

void CALifeObjectPhysic::FillProp			(LPCSTR pref, PropItemVec& values) {
	inherited::FillProp			(pref,	 values);
	PHelper.CreateToken			(values, PHelper.PrepareKey(pref,s_name,"Type"), &type,	po_types, 1);
	PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Mass"), &mass, 0.1f, 10000.f);
	PHelper.CreateText			(values, PHelper.PrepareKey(pref,s_name,"Fixed bone"),	fixed_bone,	sizeof(fixed_bone));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeObjectHangingLamp
////////////////////////////////////////////////////////////////////////////
CALifeObjectHangingLamp::CALifeObjectHangingLamp(LPCSTR caSection) : CALifeDynamicObjectVisual(caSection), CAbstractServerObject(caSection)
{
	flags.set					(flPhysic,TRUE);
    mass						= 10.f;
    startup_animation[0]		= 0;
	spot_texture[0]				= 0;
	color_animator[0]			= 0;
	spot_bone[0]				= 0;
	spot_range					= 10.f;
	spot_cone_angle				= PI_DIV_3;
	color						= 0xffffffff;
    spot_brightness				= 1.f;
}

CALifeObjectHangingLamp::~CALifeObjectHangingLamp()
{
}

void CALifeObjectHangingLamp::STATE_Read	(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);

	visual_read					(tNetPacket);
	// model
	tNetPacket.r_u32			(color);
	tNetPacket.r_string			(color_animator);
	tNetPacket.r_string			(spot_texture);
	tNetPacket.r_string			(spot_bone);
	tNetPacket.r_float			(spot_range);
	tNetPacket.r_angle8			(spot_cone_angle);
    if (m_wVersion>10)
		tNetPacket.r_float		(spot_brightness);
    if (m_wVersion>11)
    	tNetPacket.r_u16		(flags.flags);
    if (m_wVersion>12)
    	tNetPacket.r_float		(mass);
    if (m_wVersion>17)
		tNetPacket.r_string		(startup_animation);

#ifdef _EDITOR    
	PlayAnimation				(startup_animation[0]?startup_animation:"$editor");
#endif

	// internal
	strlwr						(spot_bone);
}

void CALifeObjectHangingLamp::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	visual_write				(tNetPacket);
	// model
	tNetPacket.w_u32			(color);
	tNetPacket.w_string			(color_animator);
	tNetPacket.w_string			(spot_texture);
	tNetPacket.w_string			(spot_bone);
	tNetPacket.w_float			(spot_range);
	tNetPacket.w_angle8			(spot_cone_angle);
	tNetPacket.w_float			(spot_brightness);
   	tNetPacket.w_u16			(flags.flags);
	tNetPacket.w_float			(mass);
	tNetPacket.w_string			(startup_animation);
}

void CALifeObjectHangingLamp::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CALifeObjectHangingLamp::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void __fastcall	CALifeObjectHangingLamp::OnChangeAnim(PropValue* sender)
{
	PlayAnimation				(startup_animation);
}

void CALifeObjectHangingLamp::FillProp		(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
	PHelper.CreateColor			(values, PHelper.PrepareKey(pref,s_name,"Color"),			&color);
	PHelper.CreateFlag16		(values, PHelper.PrepareKey(pref,s_name,"Physic"),			&flags,				flPhysic);
	PHelper.CreateLightAnim		(values, PHelper.PrepareKey(pref,s_name,"Color animator"),	color_animator,		sizeof(color_animator));
	PHelper.CreateTexture		(values, PHelper.PrepareKey(pref,s_name,"Texture"),			spot_texture,		sizeof(spot_texture));
	PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Range"),			&spot_range,		0.1f, 1000.f);
	PHelper.CreateAngle			(values, PHelper.PrepareKey(pref,s_name,"Angle"),			&spot_cone_angle,	0, deg2rad(120.f));
    PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Brightness"),		&spot_brightness,	0.1f, 5.f);
    PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Mass"),			&mass,				1.f, 1000.f);

    if (visual && PKinematics(visual))
    {
    	CKinematics::accel		*ll_bones	= PKinematics(visual)->LL_Bones();
    	CKinematics::accel		*ll_motions	= PKinematics(visual)->LL_Motions();
        CKinematics::accel::iterator _I, _E;
        AStringVec				vec;
        bool					bFound;
        // bones
        _I						= ll_motions->begin();
        _E						= ll_motions->end();
        bFound					= false;
        
		for (; _I!=_E; _I++) {
			vec.push_back(_I->first);
			if (!bFound && startup_animation[0] && (vec.back() == startup_animation))
				bFound=true;
		}
        
		if (!bFound)
			startup_animation[0]= 0;
        PropValue				*tNetPacket = PHelper.CreateList	(values,	PHelper.PrepareKey(pref,s_name,"Startup animation"), startup_animation, &vec);
        tNetPacket->SetEvents	(0,0,OnChangeAnim);
		// motions
        vec.clear				();
        _I						= ll_bones->begin();
        _E						= ll_bones->end();
        bFound					= false;
        for (; _I!=_E; _I++) {
			vec.push_back(_I->first);
			if (!bFound && spot_bone[0] && (vec.back() == spot_bone))
				bFound=true;
		}
        
		if (!bFound)
			spot_bone[0]		= 0;
		PHelper.CreateList		(values, PHelper.PrepareKey(pref,s_name,"Guide bone"),		spot_bone,			&vec);
    }
}
#endif
