#ifndef _LIGHT_CONTROL_H
#define _LIGHT_CONTROL_H
#pragma once

#define L_FOR_STATIC	1
#define L_FOR_DYNAMIC	2

typedef vector<int>		vecI;
typedef vecI::iterator	vecIIT;

class CStream;

class FLightsController {
	friend BOOL __forceinline lights_compare(int a, int b);

	vector<Flight>	Lights;			// -- Lights itself
	vector<Flight>	LightsDynamic;

	vector<BOOL>	Enabled;		// -- is Enabled
	vecI			Distance;		// -- Only selected are valid!!!

	svector<int,32>	Selected;		// Selected (static lights only) in one frame
	svector<int,32>	SelectedDynamic;// Selected (dynamic lights only)

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
	DWORD	dwLastLights;

	int		Add				(Flight &L);	// Adds dynamic light - return handle
	void	Remove			(int handle);	// Removes dynamic Light
	Flight&	Get				(int handle)	{ return LightsDynamic[handle]; }


	void	UnselectAll		(void);			// Disables all lights
	void	BuildSelection	(void);			// Select relevant lights

	// warning all static lights must be disabled
	// for static - selects from dynamic lights
	void	Select			(Fvector &pos, float fRadius);
	void	SelectDynamic	(Fvector &pos, float fRadius);

	IC	void	BeginStatic(void) {
		for (int i=0; i<Selected.size(); i++) {
			Disable(Selected[i]);
		}
	}

	void	Load	(CStream *fs);
	void	Unload	(void);
	void	Render	(void);
};

#endif
