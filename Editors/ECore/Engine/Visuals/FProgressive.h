// FProgressive.h: interface for the FProgressive class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FProgressiveH
#define FProgressiveH
#pragma once

#include "FVisual.h"
struct	FSlideWindowItem;

class	FProgressive	: public Fvisual
{
protected:
	FSlideWindowItem*	pSWI		;
	u32					last_lod	;
public:
    					FProgressive();
	virtual 			~FProgressive();
	virtual void 		Render		(float LOD);		// LOD - Level Of Detail  [0.0f - min, 1.0f - max], -1 = Ignored
	virtual void 		Load		(const char* N, IReader *data,u32 dwFlags);
	virtual void 		Copy		(IRender_Visual *pFrom);
	virtual void 		Release		();

};

#endif
