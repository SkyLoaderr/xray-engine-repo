//---------------------------------------------------------------------------
#ifndef renderH
#define renderH

#include "frustum.h"
#include "vis_common.h"
#include "blender.h"
#include "blender_clsid.h"
#include "xrRender_console.h"

// definition (Renderer)
class	CRender{
public:
	// Data
	CFrustum			ViewBase;
public:
	// Occlusion culling
	virtual BOOL		occ_visible		(Fbox&	B);
	virtual BOOL		occ_visible		(sPoly& P);
	virtual BOOL		occ_visible		(vis_data& P);

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
extern CRender  RImplementation;
extern CRender*	Render;

#endif
