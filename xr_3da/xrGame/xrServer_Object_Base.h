////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Object_Base.h
//	Created 	: 19.09.2002
//  Modified 	: 16.07.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server base object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Objects_Abstract.h"
#include "object_interfaces.h"
#include "script_value_container.h"
#include "alife_space.h"
#include "client_id.h"

class	NET_Packet;

#pragma warning(push)
#pragma warning(disable:4005)

SERVER_ENTITY_DECLARE_BEGIN(CPureServerObject,IPureServerObject)
	virtual							~CPureServerObject(){}
	virtual void					load(IReader	&tFileStream);
	virtual void					save(IWriter	&tMemoryStream);
	virtual void					load(NET_Packet	&tNetPacket);
	virtual void					save(NET_Packet	&tNetPacket);
};
add_to_type_list(CPureServerObject)
#define script_type_list save_type_list(CPureServerObject)

class xrClientData;

SERVER_ENTITY_DECLARE_BEGIN3(CSE_Abstract,ISE_Abstract,CPureServerObject,CScriptValueContainer)
public:
	enum ESpawnFlags {
		flSpawnActive			= u32(1 << 0),
		flSpawnOnSurgeOnly		= u32(1 << 1),
		flSpawnSingleItemOnly	= u32(1 << 2),
		flSpawnIfDestroyedOnly	= u32(1 << 3),
		flSpawnInfiniteCount	= u32(1 << 4),
		flSpawnDestroyOnSpawn	= u32(1 << 5),
	};

private:
	LPSTR							s_name_replace;

public:
	BOOL							net_Ready;
	BOOL							net_Processed;	// Internal flag for connectivity-graph
	
	u16								m_wVersion;
	u16								m_script_version;
	u16								RespawnTime;

	u16								ID;				// internal ID
	u16								ID_Parent;		// internal ParentID, 0xffff means no parent
	u16								ID_Phantom;		// internal PhantomID, 0xffff means no phantom
	xrClientData*					owner;

	// spawn data
	shared_str						s_name;
	u8								s_gameid;
	u8								s_RP;
	Flags16							s_flags;		// state flags
	xr_vector<u16>					children;

	// update data
	Fvector							o_Position;
	Fvector							o_Angle;
	CLASS_ID						m_tClassID;
	int								m_script_clsid;
	shared_str						m_ini_string;

	// for ALife control
	bool							m_bALifeControl;
	ALife::_SPAWN_ID				m_tSpawnID;

	// ALife spawn params
	float							m_spawn_probability;
	Flags32							m_spawn_flags;
	shared_str						m_spawn_control;
	u32								m_max_spawn_count;
	u32								m_spawn_count;
	u64								m_last_spawn_time;
	u64								m_min_spawn_interval;
	u64								m_max_spawn_interval;

	//client object custom data serialization
	xr_vector<u8>					client_data;
	virtual void					load			(NET_Packet	&tNetPacket);

	//////////////////////////////////////////////////////////////////////////
	
									CSE_Abstract	(LPCSTR caSection);
	virtual							~CSE_Abstract	();
	virtual void					OnEvent			(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender ){};
	virtual void					FillProps		(LPCSTR pref, PropItemVec &items);
	//
	virtual void		__stdcall	Spawn_Write		(NET_Packet &tNetPacket, BOOL bLocal);
	virtual BOOL		__stdcall	Spawn_Read		(NET_Packet &tNetPacket);
	virtual void		__stdcall	FillProp		(LPCSTR pref, PropItemVec &items);
	virtual LPCSTR		__stdcall	name			() const;
	virtual LPCSTR		__stdcall	name_replace	() const;
	virtual void		__stdcall	set_name		(LPCSTR s) {s_name = s;};
	virtual void		__stdcall	set_name_replace(LPCSTR s) {xr_free(s_name_replace); s_name_replace = xr_strdup(s);};
	virtual Fvector&	__stdcall	position		();
	virtual Fvector&	__stdcall	angle			();
	virtual Flags16&	__stdcall	flags			();
	virtual CSE_Visual* __stdcall	visual			();
	virtual ISE_Shape*  __stdcall	shape			();
	virtual CSE_Motion* __stdcall	motion			();
	//

	IC		const Fvector			&Position		() const					{return o_Position;};
	// we need this to prevent virtual inheritance :-(
	virtual CSE_Abstract			*base			();
	virtual const CSE_Abstract		*base			() const;
	virtual CSE_Abstract			*init			();
	// end of the virtual inheritance dependant code
	IC		int						script_clsid	() const					{VERIFY(m_script_clsid >= 0); return (m_script_clsid);}
};
add_to_type_list(CSE_Abstract)
#define script_type_list save_type_list(CSE_Abstract)

#pragma warning(pop)
