////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Object_Base.cpp
//	Created 	: 19.09.2002
//  Modified 	: 16.07.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server base object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects.h"
#include "script_value_container_impl.h"
#include "xrMessages.h"
#include "game_base_space.h"

#ifndef XRSE_FACTORY_EXPORTS
#	include "xrEProps.h"
	
	IPropHelper &PHelper()
	{
		NODEFAULT;
#	ifdef DEBUG
		return(*(IPropHelper*)0);
#	endif
	}
#endif

LPCSTR script_section = "script";
LPCSTR current_version = "current_server_entity_version";

IC	u16	script_server_object_version	()
{
	static bool initialized		= false;
	static u16  script_version	= 0;
	if (!initialized) {
		initialized				= true;
		if (!pSettings->section_exist(script_section) || !pSettings->line_exist(script_section,current_version))
			script_version		= 0;
		script_version			= pSettings->r_u16(script_section,current_version);
	}
	return						(script_version);
}

////////////////////////////////////////////////////////////////////////////
// CPureServerObject
////////////////////////////////////////////////////////////////////////////
void CPureServerObject::save				(IWriter	&tMemoryStream)
{
}

void CPureServerObject::load				(IReader	&tFileStream)
{
}

void CPureServerObject::load				(NET_Packet	&tNetPacket)
{
}

void CPureServerObject::save				(NET_Packet	&tNetPacket)
{
}

////////////////////////////////////////////////////////////////////////////
// CSE_Abstract
////////////////////////////////////////////////////////////////////////////
CSE_Abstract::CSE_Abstract					(LPCSTR caSection)
{
	m_editor_flags.zero			();
	RespawnTime					= 0;
	net_Ready					= FALSE;
	ID							= 0xffff;
	ID_Parent					= 0xffff;
	ID_Phantom					= 0xffff;
	owner						= 0;
	s_gameid					= 0;
	s_RP						= 0xFE;			// Use supplied coords
	s_flags.assign				(0);
	Memory.mem_copy				(s_name,caSection,((u32)xr_strlen(caSection) + 1)*sizeof(char));
	ZeroMemory					(s_name_replace,sizeof(string64));
	o_Angle.set					(0.f,0.f,0.f);
	o_Position.set				(0.f,0.f,0.f);
	m_bALifeControl				= false;
	m_wVersion					= 0;
	m_script_version			= 0;
}

CSE_Abstract::~CSE_Abstract					()
{
}

CSE_Visual* CSE_Abstract::visual			()
{
	return						(0);
}

ISE_Shape*  CSE_Abstract::shape				()
{
	return						(0);
}

CSE_Motion* CSE_Abstract::motion			()
{
	return						(0);
}
	
void CSE_Abstract::Spawn_Write				(NET_Packet	&tNetPacket, BOOL bLocal)
{
	// generic
	tNetPacket.w_begin			(M_SPAWN);
	tNetPacket.w_stringZ		(s_name			);
	tNetPacket.w_stringZ		(s_name_replace	);
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
	
	tNetPacket.w_u16			(script_server_object_version());

	CScriptValueContainer::assign();

	// write specific data
	u32	position				= tNetPacket.w_tell();
	tNetPacket.w_u16			(0);
	STATE_Write					(tNetPacket);
	u16 size					= u16(tNetPacket.w_tell()-position);
	tNetPacket.w_seek			(position,&size,sizeof(u16));
}

BOOL CSE_Abstract::Spawn_Read				(NET_Packet	&tNetPacket)
{
	u16							dummy16;
	// generic
	tNetPacket.r_begin			(dummy16);	
	R_ASSERT					(M_SPAWN==dummy16);
	tNetPacket.r_stringZ		(s_name			);
	tNetPacket.r_stringZ		(s_name_replace	);
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

	if (m_wVersion > 69)
		m_script_version		= tNetPacket.r_u16();

	// read specific data
	u16							size;
	tNetPacket.r_u16			(size			);	// size
	STATE_Read					(tNetPacket,size);
    
	return						TRUE;
}

CSE_Abstract *CSE_Abstract::base	()
{
	return						(this);
}

const CSE_Abstract *CSE_Abstract::base	() const
{
	return						(this);
}

CSE_Abstract *CSE_Abstract::init	()
{
	return						(this);
}

LPSTR		CSE_Abstract::name			()
{
	return	(s_name);
}

LPSTR		CSE_Abstract::name_replace	()
{
	return	(s_name_replace);
}

Fvector&	CSE_Abstract::position		()
{
	return	(o_Position);
}

Fvector&	CSE_Abstract::angle			()
{
	return	(o_Angle);
}

Flags16&	CSE_Abstract::flags			()
{
	return	(s_flags);
}

xr_token game_types[]={
	{ "Any game",		GAME_ANY		},
	{ "Single",			GAME_SINGLE		},
	{ "Deathmatch",		GAME_DEATHMATCH },
//	{ "CTF",			GAME_CTF		},
//	{ "Assault",		GAME_ASSAULT	},
	{ "Counter Strike",	GAME_CS			},
	{ "TeamDeathmatch",	GAME_TEAMDEATHMATCH },
	{ "ArtefactHunt",	GAME_ARTEFACTHUNT },
	{ 0,				0				}
};

void CSE_Abstract::FillProps				(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateToken8		(items,	PrepareKey(pref,"Game Type"),			&s_gameid,		game_types);
    PHelper().CreateU16			(items,	PrepareKey(pref, "Respawn Time (s)"),	&RespawnTime,	0,43200);
}

void CSE_Abstract::FillProp					(LPCSTR pref, PropItemVec &items)
{
	CScriptValueContainer::assign();
	FillProps					(pref,items);
}
