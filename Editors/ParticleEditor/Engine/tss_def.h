#ifndef TSS_DEF_H
#define TSS_DEF_H

#pragma once

class	ENGINE_API SimulatorStates
{
private:
	struct State
	{
		DWORD	type;		// 0=RS, 1=TSS
		DWORD	v1,v2,v3;
		
		IC void	set_RS	(DWORD a, DWORD b)
		{
			type	= 0;
			v1		= a;
			v2		= b;
			v3		= 0;
		}
		IC void	set_TSS	(DWORD a, DWORD b, DWORD c)
		{
			type	= 1;
			v1		= a;
			v2		= b;
			v3		= c;
		}
	};
private:
	vector<State>	States;
public:
	void			set_RS	(DWORD a, DWORD b);
	void			set_TSS	(DWORD a, DWORD b, DWORD c);
	BOOL			equal	(SimulatorStates& S);
	void			clear	();
	DWORD			record	();
};
#endif