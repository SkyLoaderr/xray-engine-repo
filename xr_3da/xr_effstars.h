#ifndef __XR_EFFSTARS_H__
#define __XR_EFFSTARS_H__

#include "xr_effects.h"

//-----------------------------------------------------------------------------------------------------------
//Stars Effect
//-----------------------------------------------------------------------------------------------------------
class							CEffectStars
{
	friend struct				sxr_star;

	Fmatrix						matTranslate;

	DWORD						stars_count;
	FLvertex*					stars;
	float*						rnd_value;
	float*						opacity;

	float						vis_radius;

	void						GenerateStars			( );
public:
								CEffectStars			( );
								~CEffectStars		( );

	BOOL						Load					( LPSTR section, void* data = NULL );

	void						OnMove					( );
	void _fastcall				RenderAlpha				( );

	void						Update					( );
};

#endif //__XR_EFFSTARS_H__
