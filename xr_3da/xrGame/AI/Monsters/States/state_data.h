#pragma once

struct SStateDataMoveToPoint {
	Fvector		point;
	u32			vertex;
	EAction		action;
	bool		accelerated;
	bool		braking;
	u8			accel_type;
};

struct SStateDataLookToPoint {
	Fvector		point;
	EAction		action;
};

struct SStateDataCustomAction {
	EAction		action;
};
