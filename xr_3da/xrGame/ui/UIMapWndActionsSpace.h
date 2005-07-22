#pragma once

#define GLOBAL_MAP_IDX				(u16(65000))

namespace UIMapWndActionsSpace {
	inline	u32 make_map_id(u16 map_idx, u16 prop_idx)	{
		u32 res = (map_idx<<16) + prop_idx;
		return res;
	}

	enum EWorldProperties {
		ePropFoo					= u32(0),
		ePropMapOpened,
		ePropMapOpenedIdle,
		ePropGlobalMapCentered,		
		ePropLocalMapCentered,		
		ePropLocalMapZoom,
		ePropDummy					= u16(-1),
	};

	enum EWorldOperators {
		eOperatorFoo				= u32(0),
		eOperatorMapOpen,
		eOperatorMapClose,
		eOperatorGlobalMapCenter,
		eOperatorLocalMapCenter,
		eOperatorMapOpenIdle,
		eOperatorLocalMapZoom,
		eWorldOperatorDummy			= u16(-1),
	};
};
