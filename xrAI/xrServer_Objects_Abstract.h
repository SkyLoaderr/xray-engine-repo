////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_Abstract.h
//	Created 	: 19.09.2002
//  Modified 	: 18.06.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_AbstractH
#define xrServer_Objects_AbstractH

#include "xrServer_Space.h"
#include "xrCDB.h"
#include "ShapeData.h"
#ifndef XRGAME_EXPORTS
#	include "Sound.h"
#	include "net_utils.h"
#endif
#include "xrEProps.h"
#include "PropertiesListHelper.h"

#pragma warning(push)
#pragma warning(disable:4005)

SERVER_ENTITY_DECLARE_BEGIN(CSE_Shape,CShapeData)
public:
	void							cform_read		(NET_Packet& P);
	void							cform_write		(NET_Packet& P);
									CSE_Shape		();
	virtual							~CSE_Shape		();
	virtual CSE_Shape*  __stdcall	shape			() = 0;
};
add_to_type_list(CSE_Shape)
#define script_type_list save_type_list(CSE_Shape)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Visual)
public:
	enum {
		flPlayAnimation				= u32(1 << 0),
	};
public:
	ref_str							visual_name;
	ref_str							startup_animation;
	Flags32							m_visual_flags;
public:
									CSE_Visual		(LPCSTR name=0);
	virtual							~CSE_Visual		();

	void							visual_read		(NET_Packet& P);
	void							visual_write	(NET_Packet& P);

    void							set_visual		(LPCSTR name, bool load=true);
	LPCSTR							get_visual		() const {return *visual_name;};
	IC	void						play_animation	()
	{
		m_visual_flags.set			(flPlayAnimation,TRUE);
	}

	virtual CSE_Visual* __stdcall	visual			() = 0;
};
add_to_type_list(CSE_Visual)
#define script_type_list save_type_list(CSE_Visual)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Motion)
public:
	ref_str							motion_name;
public:
									CSE_Motion 		(LPCSTR name=0);
	virtual							~CSE_Motion		();

	void							motion_read		(NET_Packet& P);
	void							motion_write	(NET_Packet& P);

    void							set_motion		(LPCSTR name);
	LPCSTR							get_motion		() const {return *motion_name;};

	virtual CSE_Motion* __stdcall	motion			() = 0;
};
add_to_type_list(CSE_Motion)
#define script_type_list save_type_list(CSE_Motion)

struct ISE_Abstract {
	virtual void		__stdcall	Spawn_Write		(NET_Packet &tNetPacket, BOOL bLocal) = 0;
	virtual BOOL		__stdcall	Spawn_Read		(NET_Packet &tNetPacket) = 0;
	virtual void		__stdcall	FillProp		(LPCSTR pref, PropItemVec &items) = 0;
	virtual LPSTR		__stdcall	name			() = 0;
	virtual LPSTR		__stdcall	name_replace	() = 0;
	virtual Fvector&	__stdcall	position		() = 0;
	virtual Fvector&	__stdcall	angle			() = 0;
	virtual Flags16&	__stdcall	flags			() = 0;
	virtual CSE_Visual* __stdcall	visual			() = 0;
	virtual CSE_Shape*  __stdcall	shape			() = 0;
	virtual CSE_Motion* __stdcall	motion			() = 0;
};

#pragma warning(pop)

#endif // xrServer_Objects_AbstractH