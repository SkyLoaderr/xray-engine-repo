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
	IC void			set_RS	(DWORD a, DWORD b)
	{
		State		st;
		st.set_RS	(a,b);
		States.push_back(st);
	}
	IC void			set_TSS	(DWORD a, DWORD b, DWORD c)
	{
		State		st;
		st.set_TSS	(a,b,c);
		States.push_back(st);
	}
	IC BOOL			equal	(SimulatorStates& S)
	{
		if (States.size()!=S.States.size())	return FALSE;
		if (0!=memcmp(States.begin(),S.States.begin(),States.size()*sizeof(State))) return FALSE;
		return TRUE;
	}
	IC void			clear	()
	{
		States.clear();
	}

	DWORD			record	();
};
#endif