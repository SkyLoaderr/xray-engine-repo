#ifndef _LIGHT_CONTROL_H
#define _LIGHT_CONTROL_H
#pragma once

#include "light.h"

const float fLightSmoothFactor = 4.f;

DEF_VECTOR(vecI,int);

class ENGINE_API CStream;

class ENGINE_API CLightsController {
	friend BOOL __forceinline lights_compare(int a, int b);

	vector<xrLIGHT>	Lights;			// -- Lights itself
	vector<xrLIGHT>	LightsDynamic;
	vector<Flight>	Keyframes;

	vector<BYTE>	Enabled;		// -- is Enabled
	vecI			Distance;		// -- Only selected are valid!!!

	vecI			Selected;			// Selected (static only) in one frame
	vecI			SelectedDynamic;	// Selected (dynamic only)
	vecI			SelectedProcedural;	// Selected (both) - applyed to both types

	// Depth enhance
	Fvector			DE_Data[4];
	float			DE_Time;

	IC	void	Disable(int num) {
		if (Enabled[num]) {
			Enabled[num]=false;
			CHK_DX(HW.pDevice->LightEnable(num,FALSE));
		}
	}

	IC	void	Enable(int num) {
		if (!Enabled[num]) {
			Enabled[num]=true;
			CHK_DX(HW.pDevice->LightEnable(num,TRUE));
		}
	}
public:
	void	add_sector_lights(vector<WORD> &L);

	int		Add				(xrLIGHT &L);	// Adds dynamic light - return handle
	void	Remove			(int handle);	// Removes dynamic Light
	xrLIGHT&	Get			(int handle)	{ return LightsDynamic[handle]; }

	void	UnselectAll		(void);			// Disables all lights
	void	BuildSelection	(void);			// Select relevant lights

	// warning all static lights must be disabled
	// for static - selects from dynamic lights
	void	Select			(Fvector &pos, float fRadius);
	void	SelectDynamic	(Fvector &pos, float fRadius);

	IC	void	BeginStatic	(void) {
		for (DWORD i=0; i<Selected.size(); i++) {
			Disable(Selected[i]);
		}
	}

	void	LoadKeyframes	(CStream *fs);
	void	Load			(CStream *fs);
	void	Unload			(void);
	void	Render			(void);
};

#endif
