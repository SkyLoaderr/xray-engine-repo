#pragma once

struct SStateDataMoveToPoint {
	Fvector		point;
	u32			vertex;
	EAction		action;
	bool		accelerated;
	bool		braking;
	u8			accel_type;
	float		completion_dist;
};

struct SStateHideFromPoint {
	Fvector		point;
	EAction		action;
	bool		accelerated;
	bool		braking;
	u8			accel_type;
	float		distance;
	float		cover_min_dist;
	float		cover_max_dist;
	float		cover_search_radius;
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




