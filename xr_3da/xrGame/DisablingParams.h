#ifndef DISABLING_PARAMS_H
#define DISABLING_PARAMS_H
struct	SOneDDOParams
{
	float				velocity				;
	float				acceleration			;
};

struct	SAllDDOParams
{
	SOneDDOParams		translational			;
	SOneDDOParams		rotational				;
	u16					L2frames				;
};

struct SAllDDWParams
{
	SAllDDOParams		objects_params			;
	float				reanable_factor			;
};



extern SAllDDWParams	worldDisablingParams									;

#endif