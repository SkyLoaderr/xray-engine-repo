////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_Abstract.h
//	Created 	: 19.09.2002
//  Modified 	: 18.06.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_AbstractH
#define xrServer_Objects_AbstractH

#include "alife_interfaces.h"
#include "xrServer_Space.h"
#include "script_export_space.h"

class CPureServerObject : public IPureServerObject {
public:
	virtual							~CPureServerObject(){}
	virtual void					load(IReader	&tFileStream);
	virtual void					save(IWriter	&tMemoryStream);
	virtual void					load(NET_Packet	&tNetPacket);
	virtual void					save(NET_Packet	&tNetPacket);
//	static  void					script_register(lua_State *L);
};
add_to_type_list(CPureServerObject)
#undef script_type_list
#define script_type_list save_type_list(CPureServerObject)

class xrClientData;

class CSE_Abstract : public CPureServerObject {
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
#ifdef _EDITOR
    virtual void					FillProp		(LPCSTR pref, PropItemVec &items);
#endif
};
add_to_type_list(CSE_Abstract)
#undef script_type_list
#define script_type_list save_type_list(CSE_Abstract)

class CSE_Shape
{
public:
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
#undef script_type_list
#define script_type_list save_type_list(CSE_Shape)

class CSE_Visual
{
	string64						visual_name;
public:
#ifdef _EDITOR
	AnsiString						play_animation;
	IRender_Visual*		   			visual;
    void __fastcall					OnChangeVisual	(PropValue* sender);
    void 							PlayAnimation	(LPCSTR name);
#endif
public:
									CSE_Visual		(LPCSTR name=0);
	virtual							~CSE_Visual		();

	void							visual_read		(NET_Packet& P);
	void							visual_write	(NET_Packet& P);

    void							set_visual		(LPCSTR name, bool load=true);
	LPCSTR							get_visual		() const {return visual_name;};
    
#ifdef _EDITOR
    void 							FillProp		(LPCSTR pref, PropItemVec& values);
#endif
};
add_to_type_list(CSE_Visual)
#undef script_type_list
#define script_type_list save_type_list(CSE_Visual)

#ifdef _EDITOR
	class CObjectAnimator;
#endif

class CSE_Motion
{
	ref_str							motion_name;
public:
#ifdef _EDITOR
	CObjectAnimator*	   			animator;
    void __fastcall					OnChangeMotion	(PropValue* sender);
    void 							PlayMotion		(LPCSTR name=0);
#endif
public:
									CSE_Motion 		(LPCSTR name=0);
	virtual							~CSE_Motion		();

	void							motion_read		(NET_Packet& P);
	void							motion_write	(NET_Packet& P);

    void							set_motion		(LPCSTR name);
	LPCSTR							get_motion		() const {return *motion_name;};
    
#ifdef _EDITOR
    void 							FillProp		(LPCSTR pref, PropItemVec& values);
#endif
};
add_to_type_list(CSE_Motion)
#undef script_type_list
#define script_type_list save_type_list(CSE_Motion)

#endif // xrServer_Objects_AbstractH