////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_space.h
//	Created 	: 08.05.2004
//  Modified 	: 08.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ObjectHandlerSpace {
	enum EWorldProperties {
		eWorldPropertyItemID		= u32(0),
		eWorldPropertyHidden,
		eWorldPropertyStrapping,
		eWorldPropertyStrapped,
		eWorldPropertyUnstrapping,
		eWorldPropertySwitch1,
		eWorldPropertySwitch2,
		eWorldPropertyAimed1,
		eWorldPropertyAimed2,
		eWorldPropertyAiming1,
		eWorldPropertyAiming2,
		eWorldPropertyEmpty1,
		eWorldPropertyEmpty2,
		eWorldPropertyReady1, // (magazine1 is full of ammo1) && !missfire1
		eWorldPropertyReady2, // (magazine2 is full of ammo2) && !missfire2
		eWorldPropertyFiring1,
		eWorldPropertyFiring2,
		eWorldPropertyAmmo1,
		eWorldPropertyAmmo2,
		eWorldPropertyIdle,
		eWorldPropertyIdleStrap,
		eWorldPropertyDropped,
		eWorldPropertyQueueWait1,
		eWorldPropertyQueueWait2,
		eWorldPropertyAimingReady1,
		eWorldPropertyAimingReady2,
		
		eWorldPropertyThrowStarted,
		eWorldPropertyThrowIdle,
		eWorldPropertyThrow,
		eWorldPropertyThreaten,

		eWorldPropertyPrepared,
		eWorldPropertyUsed,
		eWorldPropertyUseEnough,

		eWorldPropertyNoItems				= u32((u16(-1) << 16) | eWorldPropertyItemID),
		eWorldPropertyNoItemsIdle			= u32((u16(-1) << 16) | eWorldPropertyIdle),
		eWorldPropertyDummy					= u32(-1),
	};

	enum EWorldOperators {
		eWorldOperatorShow			= u32(0),
		eWorldOperatorHide,
		eWorldOperatorDrop,
		eWorldOperatorStrapping,
		eWorldOperatorStrapped,
		eWorldOperatorUnstrapping,
		eWorldOperatorIdle,
		eWorldOperatorAim1,
		eWorldOperatorAim2,
		eWorldOperatorReload1,
		eWorldOperatorReload2,
		eWorldOperatorFire1,
		eWorldOperatorFire2,
		eWorldOperatorSwitch1,
		eWorldOperatorSwitch2,
		eWorldOperatorQueueWait1,
		eWorldOperatorQueueWait2,
		eWorldOperatorAimingReady1,
		eWorldOperatorAimingReady2,

		eWorldOperatorThrowStart,
		eWorldOperatorThrowIdle,
		eWorldOperatorThrow,
		eWorldOperatorThreaten,

		eWorldOperatorPrepare,
		eWorldOperatorUse,

		eWorldOperatorNoItemsIdle	= u32((u16(-1) << 16) | eWorldOperatorIdle),
		eWorldOperatorDummy			= u32(-1),
	};
};