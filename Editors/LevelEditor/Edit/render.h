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

    void				Calculate		();
    void				Render			();
};                                                    

IC  float   CalcSSA(Fvector& C, float R)
{
    float distSQ  = Device.m_Camera.GetPosition().distance_to_sqr(C);
    return  R*R/distSQ;
}
extern CRender*	Render;
	
#endif
