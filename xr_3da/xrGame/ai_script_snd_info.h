#pragma once

class CLuaSoundInfo {
public:
	CLuaGameObject	*who;
	Fvector					position;
	float					power;
	int						time;		
	int						dangerous;


	CLuaSoundInfo				()
	{
		who				= 0;
		time			= 0;
		dangerous		= 0;
	}

	void set(CLuaGameObject *p_who, bool p_danger, Fvector p_position, float p_power, int p_time) {
		who			= p_who;
		position	= p_position;
		power		= p_power;
		time		= p_time;
		dangerous	= int(p_danger);
	}
};