#ifndef SH_CONSTANT_H
#define SH_CONSTANT_H
#pragma once

#include "WaveForm.h"

class	ENGINE_API	CStream;
class	ENGINE_API	CFS_Base;

class	ENGINE_API	CConstant
{
public:
	enum { modeProgrammable=0, modeWaveForm	};
public:
	Fcolor			const_float;
	u32			const_dword;

	u32			dwReference;
	u32			dwFrame;
	u32			dwMode;
	WaveForm		_R;
	WaveForm		_G;
	WaveForm		_B;
	WaveForm		_A;

	CConstant		(){	ZeroMemory(this,sizeof(CConstant));}
	IC void			set_float	(float r, float g, float b, float a)
	{
		const_float.set	(r,g,b,a);
		const_dword		= const_float.get();
	}
	IC void			set_float	(Fcolor& c)
	{
		const_float.set	(c);
		const_dword		= const_float.get();
	}
	IC void			set_dword	(u32 c)
	{
		const_float.set(c);
		const_dword		= c;
	}
	IC void			Calculate	()
	{
		if (dwFrame==Device.dwFrame)	return;
		dwFrame		= Device.dwFrame;
		if (modeProgrammable==dwMode)	return;

		float	t	= Device.fTimeGlobal;
		set_float	(_R.Calculate(t),_G.Calculate(t),_B.Calculate(t),_A.Calculate(t));
	}
	IC	BOOL		Similar		(CConstant& C)		// comare by modes and params
	{
		if (dwMode!=C.dwMode)	return FALSE;
		if (!_R.Similar(C._R))	return FALSE;
		if (!_G.Similar(C._G))	return FALSE;
		if (!_B.Similar(C._B))	return FALSE;
		if (!_A.Similar(C._A))	return FALSE;
		return TRUE;
	}
	void			Load		(CStream* fs);
	void			Save		(CFS_Base* fs);
};
#endif