////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_space.h"

using namespace luabind;

#define wrap_function(a,b) \
	virtual void a(NET_Packet &net_packet) \
	{ \
		call_member<void>(b,net_packet); \
	} \
	static  void static_##a(T *t, NET_Packet &net_packet) \
	{ \
		t->T::a(net_packet); \
	}

#define wrap_function1(a,b,c) \
	virtual void a(NET_Packet &net_packet, c _c) \
	{ \
		call_member<void>(b,net_packet,_c); \
	} \
	static  void static_##a(T *t, NET_Packet &net_packet, c _c) \
	{ \
		t->T::a(net_packet,_c); \
	}

template <typename T>
struct CWrapperAbstract : public T, public wrap_base {
	CWrapperAbstract(LPCSTR section) : T(section){}
	wrap_function(STATE_Write,	"state_write")
	wrap_function1(STATE_Read,	"state_read", u16)
	wrap_function(UPDATE_Write,	"update_write")
	wrap_function(UPDATE_Read,	"update_read")
};

#define declare_virtual(T1,a,b) \
	.def(b, &T1::a, &CWrapperAbstract<T1>::static_##a)

#define export_standard(T1) \
	.def(	constructor<LPCSTR>()) \
	declare_virtual(T1,STATE_Write,	"state_write") \
	declare_virtual(T1,STATE_Read,	"state_read") \
	declare_virtual(T1,UPDATE_Write,"update_write") \
	declare_virtual(T1,UPDATE_Read,	"update_read")


#define wrap_class(T1,T2,T3) \
	class_<T1,T2,CWrapperAbstract<T1> >(T3) \
		export_standard(T1)

void CSE_ALifeTraderAbstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeTraderAbstract>
			("cse_alife_trader_abstract")
//			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeTrader::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeTrader,CSE_ALifeDynamicObjectVisual,CSE_ALifeTraderAbstract>
			("cse_alife_trader")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeAnomalousZone::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeAnomalousZone,CSE_ALifeDynamicObject,CSE_ALifeSchedulable,CSE_Shape>
			("cse_alife_anomalous_zone")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeCreatureAbstract::script_register(lua_State *L)
{
	module(L)[
//		class_<CSE_ALifeCreatureAbstract,CSE_ALifeDynamicObjectVisual>
//			("cse_alife_creature_abstract")
//			.def(		constructor<LPCSTR>())
		wrap_class(CSE_ALifeCreatureAbstract,CSE_ALifeDynamicObjectVisual,"cse_alife_creature_abstract")
	];
}

void CSE_ALifeMonsterAbstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeMonsterAbstract,CSE_ALifeCreatureAbstract,CSE_ALifeSchedulable>
			("cse_alife_monster_abstract")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeCreatureActor::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeCreatureActor,CSE_ALifeCreatureAbstract,CSE_ALifeTraderAbstract>
			("cse_alife_creature_actor")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeCreatureCrow::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeCreatureCrow,CSE_ALifeCreatureAbstract>
			("cse_alife_creature_crow")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeMonsterRat::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeMonsterRat,CSE_ALifeMonsterAbstract,CSE_ALifeInventoryItem>
			("cse_alife_monster_rat")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeMonsterZombie::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeMonsterZombie,CSE_ALifeMonsterAbstract>
			("cse_alife_monster_zombie")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeMonsterBiting::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeMonsterBiting,CSE_ALifeMonsterAbstract>
			("cse_alife_monster_biting")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeHumanAbstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeHumanAbstract,CSE_ALifeTraderAbstract,CSE_ALifeMonsterAbstract>
			("cse_alife_human_abstract")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeHumanStalker::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeHumanStalker,CSE_ALifeHumanAbstract>
			("cse_alife_human_stalker")
			.def(		constructor<LPCSTR>())
	];
}

void CSE_ALifeObjectIdol::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeObjectIdol,CSE_ALifeHumanAbstract>
			("cse_alife_object_idol")
			.def(		constructor<LPCSTR>())
	];
}
