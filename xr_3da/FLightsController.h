#pragma once

#include "..\light.h"

// refs
class ENGINE_API IReader;
class ENGINE_API CObject;
class CLightTrack;

// t-defs
class CLightDB_Static 
{
	DEF_VECTOR(vecI,int);

	xr_vector<xrLIGHT>			Lights;			// -- Lights itself
	xr_vector<BYTE>				Enabled;		// -- is Enabled
	vecI						Distance;		// -- Only selected are valid!!!

	vecI						Selected;		// Selected (static only) in one frame
private:
	IC	void	Disable		(int num) {
		if (Enabled[num]) {
			Enabled[num]=false;
			CHK_DX(HW.pDevice->LightEnable(num,FALSE));
		}
	}

	IC	void	Enable		(int num) {
		if (!Enabled[num]) {
			Enabled[num]=true;
			CHK_DX(HW.pDevice->LightEnable(num,TRUE));
		}
	}
public:
	void	add_light			(WORD L);

	void	UnselectAll			(void);			// Disables all lights

	void	Select				(Fvector &pos, float fRadius, xr_vector<xrLIGHT*>&	dest);
	void	Select				(Fvector &pos, float fRadius);
	void	Track				(IRenderable* O);

	void	Load				(IReader* fs);
	void	Unload				(void);
	void	Render				(void);
};
