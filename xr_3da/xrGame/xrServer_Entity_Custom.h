#ifndef xrServer_Entity_CustomH
#define xrServer_Entity_CustomH

#include "xrMessages.h"
#include "ai_alife_interfaces.h"

#define SPAWN_VERSION	u16(14)
//------------------------------------------------------------------------------
// Version history
//------------------------------------------------------------------------------
// 10 - xrSE_PhysicObject append 	'fixed_bone'
// 11 - xrSE_HangingLamp append 	'spot_brightness'
// 12 - xrSE_HangingLamp append 	'flags'
// 13 - xrSE_HangingLamp append 	'mass'
// 14 - xrSE_PhysicObject append 	inherited from CALifeObject
//------------------------------------------------------------------------------

class xrClientData;

class xrServerEntity : public IPureServerObject {
public:
	BOOL							net_Ready;
	BOOL							net_Processed;	// Internal flag for connectivity-graph
	
	u16								m_wVersion;
	u16								RespawnTime;

	u16								ID;				// internal ID
	u16								ID_Parent;		// internal ParentID, 0xffff means no parent
	u16								ID_Phantom;		// internal PhantomID, 0xffff means no phantom
	xrClientData*					owner;
	vector<u16>						children;

	// spawn data
	string64						s_name;
	string64						s_name_replace;
	u8								s_gameid;
	u8								s_RP;
	Flags16							s_flags;		// state flags

	// update data
	Fvector							o_Position;
	Fvector							o_Angle;

	// for ALife control
	bool							m_bALifeControl;

									xrServerEntity(LPCSTR caSection)
	{
		RespawnTime					= 0;
		net_Ready					= FALSE;
		ID							= 0xffff;
        ID_Parent					= 0xffff;
		ID_Phantom					= 0xffff;
		owner						= 0;
		s_gameid					= 0;
		s_RP						= 0xFE;			// Use supplied coords
        s_flags.set					(M_SPAWN_OBJECT_ACTIVE);
		Memory.mem_copy				(s_name,caSection,(strlen(caSection) + 1)*sizeof(char));
		ZeroMemory					(s_name_replace,sizeof(string64));
        o_Angle.set					(0.f,0.f,0.f);
        o_Position.set				(0.f,0.f,0.f);
		m_bALifeControl				= false;
		m_wVersion					= 0;
	}
	
	virtual							~xrServerEntity()
	{
	}
	
	virtual void					OnEvent			(NET_Packet &tNetPacket, u16 type, u32 time, u32 sender ){};
	virtual u8						g_team			(){return 0;};
	virtual u8						g_squad			(){return 0;};
	virtual u8						g_group			(){return 0;};

	virtual void					Init			(LPCSTR	caSection){};
	void							Spawn_Write		(NET_Packet &tNetPacket, BOOL bLocal);
	BOOL							Spawn_Read		(NET_Packet &tNetPacket);
	// editor integration
#ifdef _EDITOR
    virtual void					FillProp		(LPCSTR pref, PropItemVec &items);
#endif
};

#endif