////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma hdrstop

#include "xrServer_Objects.h"
#include "game_base.h"

#ifdef _EDITOR
	#include "ui_main.h"
	#include "BodyInstance.h"
#endif

////////////////////////////////////////////////////////////////////////////
// CAbstractServerObject
////////////////////////////////////////////////////////////////////////////
void CAbstractServerObject::Spawn_Write		(NET_Packet	&tNetPacket, BOOL bLocal)
{
	// generic
	tNetPacket.w_begin			(M_SPAWN);
	tNetPacket.w_string			(s_name			);
	tNetPacket.w_string			(s_name_replace	);
	tNetPacket.w_u8				(s_gameid		);
	tNetPacket.w_u8				(s_RP			);
	tNetPacket.w_vec3			(o_Position		);
	tNetPacket.w_vec3			(o_Angle		);
	tNetPacket.w_u16			(RespawnTime	);
	tNetPacket.w_u16			(ID				);
	tNetPacket.w_u16			(ID_Parent		);
	tNetPacket.w_u16			(ID_Phantom		);
	
	s_flags.set					(M_SPAWN_VERSION,TRUE);
	if (bLocal)
		tNetPacket.w_u16		(u16(s_flags.flags|M_SPAWN_OBJECT_LOCAL) );
	else
		tNetPacket.w_u16		(u16(s_flags.flags&~(M_SPAWN_OBJECT_LOCAL|M_SPAWN_OBJECT_ASPLAYER)));
	
	tNetPacket.w_u16			(SPAWN_VERSION);

	// write specific data
	u32	position				= tNetPacket.w_tell();
	tNetPacket.w_u16			(0);
	STATE_Write					(tNetPacket);
	u16 size					= u16(tNetPacket.w_tell()-position);
	tNetPacket.w_seek			(position,&size,sizeof(u16));
}

BOOL CAbstractServerObject::Spawn_Read		(NET_Packet	&tNetPacket)
{
	u16							dummy16;
	// generic
	tNetPacket.r_begin			(dummy16);	
	R_ASSERT					(M_SPAWN==dummy16);
	tNetPacket.r_string			(s_name			);
	tNetPacket.r_string			(s_name_replace	);
	tNetPacket.r_u8				(s_gameid		);
	tNetPacket.r_u8				(s_RP			);
	tNetPacket.r_vec3			(o_Position		);
	tNetPacket.r_vec3			(o_Angle		);
	tNetPacket.r_u16			(RespawnTime	);
	tNetPacket.r_u16			(ID				);
	tNetPacket.r_u16			(ID_Parent		);
	tNetPacket.r_u16			(ID_Phantom		);
	tNetPacket.r_u16			(s_flags.flags	); 
	
	// dangerous!!!!!!!!!
	if (s_flags.is(M_SPAWN_VERSION))
		tNetPacket.r_u16		(m_wVersion);
	
	if (0==m_wVersion) {
		tNetPacket.r_pos		-= sizeof(u16);
		m_wVersion				= 0;
        return					FALSE;
	}
	// this is "zaplatka"

	// read specific data
	u16							size;
	tNetPacket.r_u16			(size			);	// size
	STATE_Read					(tNetPacket,size);
    
	return						TRUE;
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

void CAbstractServerObject::FillProp		(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateToken			(items,	PHelper.PrepareKey(pref,"Game Type"),			&s_gameid,		game_types, 1);
    PHelper.CreateFlag16		(items,	PHelper.PrepareKey(pref, "Active"),				&s_flags, 		M_SPAWN_OBJECT_ACTIVE);
    PHelper.CreateU16			(items,	PHelper.PrepareKey(pref, "Respawn Time (s)"),	&RespawnTime,	0,43200);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectShape
////////////////////////////////////////////////////////////////////////////
void CServerObjectShape::cform_read			(NET_Packet	&tNetPacket)
{
	shapes.clear				();
	u8							count;
	tNetPacket.r_u8				(count);
	
	while (count) {
		shape_def				S;
		tNetPacket.r_u8			(S.type);
		switch (S.type) {
			case 0 :	
				tNetPacket.r	(&S.data.sphere,sizeof(S.data.sphere));	
				break;
			case 1 :	
				tNetPacket.r_matrix(S.data.box);
				break;
		}
		shapes.push_back		(S);
		count--;
	}
}

void CServerObjectShape::cform_write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_u8				(u8(shapes.size()));
	for (u32 i=0; i<shapes.size(); i++) {
		shape_def				&S = shapes[i];
		tNetPacket.w_u8			(S.type);
		switch (S.type) {
			case 0:	
				tNetPacket.w	(&S.data.sphere,sizeof(S.data.sphere));
				break;
			case 1:	
				tNetPacket.w_matrix	(S.data.box);
				break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
// CServerObjectVisual
////////////////////////////////////////////////////////////////////////////
void CServerObjectVisual::set_visual		(LPCSTR name)
{
	strcpy						(visual_name,name);
#ifdef _EDITOR
	OnChangeVisual				(0);
#endif
}

void CServerObjectVisual::visual_read		(NET_Packet	&tNetPacket)
{
	tNetPacket.r_string			(visual_name);
#ifdef _EDITOR
	OnChangeVisual				(0);
#endif
}

void CServerObjectVisual::visual_write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(visual_name);
}

#ifdef _EDITOR
void CServerObjectVisual::PlayAnimation		(LPCSTR name)
{
    // play motion if skeleton
    if (PKinematics(visual)){ 
	    play_animation			= name;
        CMotionDef				*M = PKinematics(visual)->ID_Cycle_Safe(play_animation.c_str());
        if (M)
			PKinematics(visual)->PlayCycle(M); 
        PKinematics(visual)->Calculate();
    }
}

void __fastcall	CServerObjectVisual::OnChangeVisual(PropValue* sender)
{
	Device.Models.Delete		(visual);
    if (visual_name[0]) {
        visual					= Device.Models.Create(visual_name);
        PlayAnimation			(play_animation.c_str());
    }
	UI.Command					(COMMAND_UPDATE_PROPERTIES);
}

void CServerObjectVisual::FillProp			(LPCSTR pref, PropItemVec& values)
{
    PropValue					*V = PHelper.CreateGameObject(values, PHelper.PrepareKey(pref,"Model"),visual_name,sizeof(visual_name));
    V->SetEvents				(0,0,OnChangeVisual);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectEvent
////////////////////////////////////////////////////////////////////////////
void CServerObjectEvent::UPDATE_Read		(NET_Packet	&tNetPacket)
{
}

void CServerObjectEvent::UPDATE_Write		(NET_Packet	&tNetPacket)
{
}

void CServerObjectEvent::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	// CForm
	cform_read					(tNetPacket);

	// Actions
	Actions_clear				();
	u8							count;
	tNetPacket.r_u8				(count);
	while (count) {
		tAction					A;
		string512				str;
		tNetPacket.r_u8			(A.type);
		tNetPacket.r_u16		(A.count);
		tNetPacket.r_u64		(A.cls);
		tNetPacket.r_string		(str);
		A.event					= xr_strdup(str);
		Actions.push_back		(A);
		count--;
	}
}

void CServerObjectEvent::STATE_Write		(NET_Packet	&tNetPacket)
{
	// CForm
	cform_write					(tNetPacket);

	// Actions
	tNetPacket.w_u8				(u8(Actions.size()));
	for (u32 i=0; i<Actions.size(); i++) {
		tAction&				A = Actions[i];
		tNetPacket.w_u8			(A.type	);
		tNetPacket.w_u16		(A.count);
		tNetPacket.w_u64		(A.cls	);
		tNetPacket.w_string		(A.event);
	}
}

////////////////////////////////////////////////////////////////////////////
// CALifeGraphPoint
////////////////////////////////////////////////////////////////////////////
#ifdef _EDITOR
	static TokenValue4::ItemVec locations[4];
	static TokenValue4::ItemVec	level_ids;
#endif

CALifeGraphPoint::CALifeGraphPoint(LPCSTR caSection) : CAbstractServerObject(caSection)
{
	s_gameid					= GAME_DUMMY;
	m_caConnectionPointName[0]	= 0;
	m_tLevelID					= 0;
	m_tLocations[0]				= 0;
	m_tLocations[1]				= 0;
	m_tLocations[2]				= 0;
	m_tLocations[3]				= 0;
}

void CALifeGraphPoint::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_string			(m_caConnectionPointName);
	tNetPacket.r_u32			(m_tLevelID);
	tNetPacket.r_u8				(m_tLocations[0]);
	tNetPacket.r_u8				(m_tLocations[1]);
	tNetPacket.r_u8				(m_tLocations[2]);
	tNetPacket.r_u8				(m_tLocations[3]);
	if (!tNetPacket.r_eof()) {
		tNetPacket.r_pos		-= 4;
		u32						dwTemp;
		tNetPacket.r_u32		(dwTemp);
		m_tLocations[0]			= (u8)dwTemp;
		tNetPacket.r_u32		(dwTemp);
		m_tLocations[1]			= (u8)dwTemp;
	}
};

void CALifeGraphPoint::STATE_Write			(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(m_caConnectionPointName);
	tNetPacket.w_u32			(m_tLevelID);
	tNetPacket.w_u8				(m_tLocations[0]);
	tNetPacket.w_u8				(m_tLocations[1]);
	tNetPacket.w_u8				(m_tLocations[2]);
	tNetPacket.w_u8				(m_tLocations[3]);
};
void CALifeGraphPoint::UPDATE_Read			(NET_Packet	&tNetPacket)
{
}

void CALifeGraphPoint::UPDATE_Write			(NET_Packet	&tNetPacket)
{
}

#ifdef _EDITOR              
void CALifeGraphPoint::FillProp				(LPCSTR pref, PropItemVec& items)
{
    CInifile					*Ini = 0;
    
	if(locations[0].empty()||locations[1].empty()||locations[2].empty()||locations[3].empty()||level_ids.empty()){
	    string256				gm_name;
        FS.update_path			(gm_name,_game_data_,"game.ltx");
    	R_ASSERT2				(FS.exist(gm_name),"Couldn't find file 'game.ltx'");
		Ini						= xr_new<CInifile>(gm_name);
    }
    
	for (int i=0; i<LOCATION_TYPE_COUNT; i++)
		if(locations[i].empty()) {
			string256			caSection, T;
			strconcat			(caSection,SECTION_HEADER,itoa(i,T,10));
			R_ASSERT			(Ini->section_exist(caSection));
			LPCSTR				N,V;
			for (u32 k = 0; Ini->r_line(caSection,k,&N,&V); k++) {
   				locations[i].push_back	(TokenValue4::Item());
				TokenValue4::Item		&val = locations[i].back();
				val.str					= V;
				val.ID					= atoi(N);
			}
		}
	
	if(level_ids.empty()) {
		R_ASSERT				(Ini->section_exist("levels"));
        LPCSTR					N,V;
        for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++) {
			R_ASSERT			(Ini->section_exist(N));
   			level_ids.push_back	(TokenValue4::Item());
            TokenValue4::Item	&val = level_ids.back();
			LPCSTR				S = Ini->r_string(N,"caption");
			val.str				= S;
            val.ID				= Ini->r_u32(N,"id");
        }
    }
    if (Ini)
		xr_delete				(Ini);
	
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Location\\1"),				(u32*)&m_tLocations[0],			&locations[0],					1);
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Location\\2"),				(u32*)&m_tLocations[1],			&locations[1],					1);
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Location\\3"),				(u32*)&m_tLocations[2],			&locations[2],					1);
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Location\\4"),				(u32*)&m_tLocations[3],			&locations[3],					1);
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Connection\\Level name"),	(u32*)&m_tLevelID,				&level_ids,						1);
	PHelper.CreateText			(items,	PHelper.PrepareKey(pref,s_name,"Connection\\Point name"),	m_caConnectionPointName,		sizeof(m_caConnectionPointName));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectDummy
////////////////////////////////////////////////////////////////////////////
CServerObjectDummy::CServerObjectDummy(LPCSTR caSection) : CAbstractServerObject(caSection)
{
	s_Animation					= 0;
	s_Model						= 0;
	s_Particles					= 0;
	s_Sound						= 0;
}
CServerObjectDummy::~CServerObjectDummy()
{
	xr_free						(s_Animation	);
	xr_free						(s_Model		);
	xr_free						(s_Particles	);
	xr_free						(s_Sound		);
}
void CServerObjectDummy::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_u8				(s_style);

	if (s_style&esAnimated)		{
		// Load animator
		string256				fn;
		tNetPacket.r_string		(fn);
		s_Animation				= xr_strdup(fn);
	}
	if (s_style & esModel) {
		// Load model
		string256				fn;
		tNetPacket.r_string		(fn);
		s_Model					= xr_strdup(fn);
	}
	if (s_style & esParticles) {
		// Load model
		string256				fn;
		tNetPacket.r_string		(fn);
		s_Particles				= xr_strdup(fn);
	}
	if (s_style & esSound) {
		// Load model
		string256				fn;
		tNetPacket.r_string		(fn);
		s_Sound					= xr_strdup(fn);
	}
}
void CServerObjectDummy::STATE_Write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_u8				(s_style		);
	if (s_style&esAnimated)		tNetPacket.w_string			(s_Animation	);
	if (s_style&esModel)		tNetPacket.w_string			(s_Model		);
	if (s_style&esParticles)	tNetPacket.w_string			(s_Particles	);
	if (s_style&esSound)		tNetPacket.w_string			(s_Sound		);
}
void CServerObjectDummy::UPDATE_Read		(NET_Packet	&tNetPacket)
{
}

void CServerObjectDummy::UPDATE_Write		(NET_Packet	&tNetPacket)
{
}

#ifdef _EDITOR
void CServerObjectDummy::FillProp			(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectSpectator
////////////////////////////////////////////////////////////////////////////
void CServerObjectSpectator::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
}

void CServerObjectSpectator::STATE_Write	(NET_Packet	&tNetPacket)
{
}

void CServerObjectSpectator::UPDATE_Read	(NET_Packet	&tNetPacket)
{
}

void CServerObjectSpectator::UPDATE_Write	(NET_Packet	&tNetPacket)
{
}

#ifdef _EDITOR
void CServerObjectSpectator::FillProp		(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp			(pref,items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectTarget
////////////////////////////////////////////////////////////////////////////
void CServerObjectTarget::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
};

void CServerObjectTarget::STATE_Write		(NET_Packet	&tNetPacket)
{
};

void CServerObjectTarget::UPDATE_Read		(NET_Packet	&tNetPacket)
{
};

void CServerObjectTarget::UPDATE_Write		(NET_Packet	&tNetPacket)
{
};

#ifdef _EDITOR
void CServerObjectTarget::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectTargetAssault
////////////////////////////////////////////////////////////////////////////
void CServerObjectTargetAssault::STATE_Read	(NET_Packet	&tNetPacket, u16 size)	
{
};

void CServerObjectTargetAssault::STATE_Write(NET_Packet	&tNetPacket)
{
};

void CServerObjectTargetAssault::UPDATE_Read(NET_Packet	&tNetPacket)
{
};

void CServerObjectTargetAssault::UPDATE_Write(NET_Packet &tNetPacket)
{
};

#ifdef _EDITOR
void CServerObjectTargetAssault::FillProp	(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectTarget_CS_Base
////////////////////////////////////////////////////////////////////////////
CServerObjectTarget_CS_Base::CServerObjectTarget_CS_Base(LPCSTR caSection) : CServerObjectTarget(caSection)
{
	s_team						= 0;
	radius						= 10.f;
    s_gameid					= GAME_CS;    
};

void CServerObjectTarget_CS_Base::STATE_Read(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_float			(radius);
	tNetPacket.r_u8				(s_team);
}

void CServerObjectTarget_CS_Base::STATE_Write(NET_Packet	&tNetPacket)
{
	tNetPacket.w_float			(radius);
	tNetPacket.w_u8				(s_team);
}

void CServerObjectTarget_CS_Base::UPDATE_Read(NET_Packet	&tNetPacket)
{
}

void CServerObjectTarget_CS_Base::UPDATE_Write(NET_Packet	&tNetPacket)
{
}

#ifdef _EDITOR
void CServerObjectTarget_CS_Base::FillProp	(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
    PHelper.CreateFloat			(items,PHelper.PrepareKey(pref,s_name,"Radius"),	&radius,1.f,100.f);
    PHelper.CreateU8			(items,PHelper.PrepareKey(pref,s_name,"Team"),		&s_team,0,1);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectTarget_CS_Cask
////////////////////////////////////////////////////////////////////////////
CServerObjectTarget_CS_Cask::CServerObjectTarget_CS_Cask(LPCSTR caSection) : CServerObjectTarget(caSection)
{
	s_Model[0]					= 0;
}
void CServerObjectTarget_CS_Cask::UPDATE_Read(NET_Packet	&tNetPacket)
{
}

void CServerObjectTarget_CS_Cask::UPDATE_Write(NET_Packet	&tNetPacket)
{
}

void CServerObjectTarget_CS_Cask::STATE_Read(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_string			(s_Model);
}

void CServerObjectTarget_CS_Cask::STATE_Write(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(s_Model);
}

#ifdef _EDITOR
void CServerObjectTarget_CS_Cask::FillProp	(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateGameObject	(items, PHelper.PrepareKey(pref,s_name,"Model"),	s_Model,	sizeof(s_Model));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectTarget_CS
////////////////////////////////////////////////////////////////////////////
CServerObjectTarget_CS::CServerObjectTarget_CS(LPCSTR caSection) : CServerObjectTarget(caSection)
{
	s_Model[0]					= 0;
}

void CServerObjectTarget_CS::UPDATE_Read	(NET_Packet	&tNetPacket)
{
}

void CServerObjectTarget_CS::UPDATE_Write	(NET_Packet	&tNetPacket)
{
}

void CServerObjectTarget_CS::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_string			(s_Model);
}

void CServerObjectTarget_CS::STATE_Write	(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(s_Model);
}

#ifdef _EDITOR
void CServerObjectTarget_CS::FillProp		(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateGameObject	(items, PHelper.PrepareKey(pref,s_name,"Model"),	s_Model,	sizeof(s_Model));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CServerObjectTemporary
////////////////////////////////////////////////////////////////////////////
CServerObjectTemporary::CServerObjectTemporary(LPCSTR caSection) : CAbstractServerObject(caSection)
{
};

void CServerObjectTemporary::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
};

void CServerObjectTemporary::STATE_Write	(NET_Packet	&tNetPacket)
{
};

void CServerObjectTemporary::UPDATE_Read	(NET_Packet	&tNetPacket)
{
};

void CServerObjectTemporary::UPDATE_Write	(NET_Packet	&tNetPacket)
{
};

#ifdef _EDITOR
void CServerObjectTemporary::FillProp		(LPCSTR pref, PropItemVec& values)
{
};

#endif
