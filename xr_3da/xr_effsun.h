#ifndef __XR_EFFSUN_H__
#define __XR_EFFSUN_H__

#include "xr_efflensflare.h"

class							CTexture;
class							CEffectLensFlare;
//-----------------------------------------------------------------------------------------------------------
//Sun Effect
//-----------------------------------------------------------------------------------------------------------
class							CSun
{
	Fcolor						sun_color;
	Fvector						sun_dir;
	CLensFlare					LensFlare;
	void						LoadSunData				( CInifile* pIni, LPSTR section );
public:
								CSun					( CInifile* pIni, LPSTR section );
								~CSun					( );

	void __fastcall				RenderSource			( );
	void __fastcall				RenderFlares			( );
	Fvector&					Direction				( ) { return sun_dir;	}
	void						Update					( );
	void						OnMove					( );
};

#endif //__XR_EFFSUN_H__
