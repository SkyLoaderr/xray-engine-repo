#pragma once

struct SStateDataMoveToPoint {
	Fvector		point;
	u32			vertex;
	EAction		action;
	bool		accelerated;
	bool		braking;
	u8			accel_type;
};

struct SStateHideFromPoint {
	Fvector		point;
	EAction		action;
	bool		accelerated;
	bool		braking;
	u8			accel_type;
	float		distance;
};


struct SStateDataLookToPoint {
	Fvector		point;
	EAction		action;
};

struct SStateDataCustomAction {
	EAction		action;
	u32			spec_params;
	u32			time_out;
};




