//---------------------------------------------------------------------------
#ifndef renderH
#define renderH

#include "frustum.h"

// definition (Renderer)
class	CRender{
public:
	// Data
	CFrustum			ViewBase;
public:
	// Occlusion culling
	virtual BOOL		occ_visible		(Fbox&	B);
	virtual BOOL		occ_visible		(sPoly& P);
	
	// Constructor/destructor
						CRender			();
	virtual 			~CRender		();
};                                                    

extern CRender*	Render;
	
#endif
