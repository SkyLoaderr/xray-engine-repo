////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_Abstract.h
//	Created 	: 19.09.2002
//  Modified 	: 18.06.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_AbstractH
#define xrServer_Objects_AbstractH

#include "object_interfaces.h"
#include "xrServer_Space.h"
#include "xrCDB.h"
#ifdef XRSE_FACTORY_EXPORTS
#	include "Sound.h"
#endif
#include "xrEProps.h"
#include "PropertiesListHelper.h"

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

SERVER_ENTITY_DECLARE_BEGIN(CSE_Abstract,CPureServerObject)
public:
	BOOL							net_Ready;
	BOOL							net_Processed;	// Internal flag for connectivity-graph
	
	u16								m_wVersion;
	u16								RespawnTime;

	u16								ID;				// internal ID
	u16								ID_Parent;		// internal ParentID, 0xffff means no parent
	u16								ID_Phantom;		// internal PhantomID, 0xffff means no phantom
	xrClientData*					owner;
	xr_vector<u16>					children;

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
	ref_str							m_ini_string;

									CSE_Abstract	(LPCSTR caSection);
	virtual							~CSE_Abstract	();
	virtual void					OnEvent			(NET_Packet &tNetPacket, u16 type, u32 time, u32 sender ){};
	void							Spawn_Write		(NET_Packet &tNetPacket, BOOL bLocal);
	BOOL							Spawn_Read		(NET_Packet &tNetPacket);
	IC		const Fvector			&Position		() const					{return o_Position;};
	// we need this to prevent virtual inheritance :-(
	virtual CSE_Abstract			*base			();
	virtual const CSE_Abstract		*base			() const;
	virtual CSE_Abstract			*init			();
	// end of the virtual inheritance dependant code
	// editor integration
    virtual void					FillProp		(LPCSTR pref, PropItemVec &items);
};
add_to_type_list(CSE_Abstract)
#define script_type_list save_type_list(CSE_Abstract)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Shape)
	enum{
    	cfSphere=0,
        cfBox
    };
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
    DEFINE_VECTOR					(shape_def,ShapeVec,ShapeIt);
	ShapeVec						shapes;
public:
	void							cform_read		(NET_Packet& P);
	void							cform_write		(NET_Packet& P);
									CSE_Shape		();
	virtual							~CSE_Shape		();
};
add_to_type_list(CSE_Shape)
#define script_type_list save_type_list(CSE_Shape)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Visual)
private:
	ref_str							visual_name;
public:

#ifdef _EDITOR
	AnsiString						play_animation;
	IRender_Visual*		   			visual;
    void __stdcall					OnChangeVisual	(PropValue* sender);
    void 							PlayAnimation	(LPCSTR name);
#endif

public:
									CSE_Visual		(LPCSTR name=0);
	virtual							~CSE_Visual		();

	void							visual_read		(NET_Packet& P);
	void							visual_write	(NET_Packet& P);

    void							set_visual		(LPCSTR name, bool load=true);
	LPCSTR							get_visual		() const {return *visual_name;};
    
    void 							FillProp		(LPCSTR pref, PropItemVec& values);
};
add_to_type_list(CSE_Visual)
#define script_type_list save_type_list(CSE_Visual)

#ifdef _EDITOR
	class CObjectAnimator;
#endif

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Motion)
private:
	ref_str							motion_name;
public:

#ifdef _EDITOR
	CObjectAnimator*	   			animator;
    void __stdcall 					OnChangeMotion	(PropValue* sender);
    void 							PlayMotion		(LPCSTR name=0);
#endif

public:
									CSE_Motion 		(LPCSTR name=0);
	virtual							~CSE_Motion		();

	void							motion_read		(NET_Packet& P);
	void							motion_write	(NET_Packet& P);

    void							set_motion		(LPCSTR name);
	LPCSTR							get_motion		() const {return *motion_name;};
    
    void 							FillProp		(LPCSTR pref, PropItemVec& values);
};
add_to_type_list(CSE_Motion)
#define script_type_list save_type_list(CSE_Motion)

#pragma warning(pop)

#endif // xrServer_Objects_AbstractH