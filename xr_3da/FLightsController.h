#ifndef _LIGHT_CONTROL_H
#define _LIGHT_CONTROL_H
#pragma once

#include "light.h"

const float fLightSmoothFactor = 4.f;

DEF_VECTOR(vecI,int);

class ENGINE_API CStream;

class ENGINE_API CLightDB_Static {
	friend BOOL __forceinline lights_compare(int a, int b);

	vector<xrLIGHT>	Lights;			// -- Lights itself
	vector<BYTE>	Enabled;		// -- is Enabled
	vecI			Distance;		// -- Only selected are valid!!!

	vecI			Selected;		// Selected (static only) in one frame

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

	void	UnselectAll		(void);			// Disables all lights

	void	SelectDynamic	(Fvector &pos, float fRadius);

	void	Load			(CStream *fs);
	void	Unload			(void);
	void	Render			(void);
};

#endif
