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
    void __stdcall	OnChangeVisual	(PropValue* sender);  
public:
	ref_str							visual_name;
	ref_str							startup_animation;
public:
									CSE_Visual		(LPCSTR name=0);
	virtual							~CSE_Visual		();

	void							visual_read		(NET_Packet& P);
	void							visual_write	(NET_Packet& P);

    void							set_visual		(LPCSTR name, bool load=true);
	LPCSTR							get_visual		() const {return *visual_name;};

	virtual void		__stdcall	FillProps		(LPCSTR pref, PropItemVec &items);

	virtual CSE_Visual* __stdcall	visual			() = 0;
};
add_to_type_list(CSE_Visual)
#define script_type_list save_type_list(CSE_Visual)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Motion)
	void __stdcall	OnChangeMotion	(PropValue* sender);  
public:
	ref_str							motion_name;
public:
									CSE_Motion 		(LPCSTR name=0);
	virtual							~CSE_Motion		();

	void							motion_read		(NET_Packet& P);
	void							motion_write	(NET_Packet& P);

    void							set_motion		(LPCSTR name);
	LPCSTR							get_motion		() const {return *motion_name;};

	virtual void		__stdcall	FillProps		(LPCSTR pref, PropItemVec &items);

	virtual CSE_Motion* __stdcall	motion			() = 0;
};
add_to_type_list(CSE_Motion)
#define script_type_list save_type_list(CSE_Motion)

struct ISE_Abstract {
public:
	enum {
		flUpdateProperties			= u32(1 << 0),
		flVisualChange				= u32(1 << 1),
		flVisualAnimationChange		= u32(1 << 2),
		flMotionChange				= u32(1 << 3),
	};
	Flags32							m_editor_flags;
	IC	void						set_editor_flag	(u32 mask)	{m_editor_flags.set	(mask,TRUE);}

public:
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