#pragma once

//////////////////////////////////////////////////////////////////////////
// SStateDataAction
//////////////////////////////////////////////////////////////////////////
struct SStateDataAction {
	EAction		action;
	u32			spec_params;
	u32			time_out;

	SStateDataAction() {
		action		= ACT_STAND_IDLE;
		spec_params	= 0;
		time_out	= 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// SStateDataMoveToPoint
//////////////////////////////////////////////////////////////////////////
struct SStateDataMoveToPoint {
	Fvector				point;
	u32					vertex;
	
	bool				accelerated;
	bool				braking;
	u8					accel_type;
	
	float				completion_dist;
	
	SStateDataAction	action;

	SStateDataMoveToPoint() {
		point.set			(0.f,0.f,0.f);
		vertex				= u32(-1);
		accelerated			= false;
		completion_dist		= 0.f;
	}
};

//////////////////////////////////////////////////////////////////////////
// SStateHideFromPoint
//////////////////////////////////////////////////////////////////////////
struct SStateHideFromPoint {
	Fvector				point;
	
	bool				accelerated;
	bool				braking;
	u8					accel_type;
	
	float				distance;
	
	float				cover_min_dist;
	float				cover_max_dist;
	float				cover_search_radius;
	
	SStateDataAction	action;

	SStateHideFromPoint() {
		point.set			(0.f,0.f,0.f);
		
		accelerated			= false;
		
		distance			= 1.f;
		
		cover_min_dist		= 10.f;
		cover_max_dist		= 30.f;
		cover_search_radius = 20.f;
	}
};		

//////////////////////////////////////////////////////////////////////////
// SStateDataLookToPoint
//////////////////////////////////////////////////////////////////////////
struct SStateDataLookToPoint {
	Fvector				point;
	SStateDataAction	action;

	SStateDataLookToPoint() {
		point.set		(0.f,0.f,0.f);
	}
};



