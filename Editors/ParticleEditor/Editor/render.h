//---------------------------------------------------------------------------
#ifndef renderH
#define renderH

#include "frustum.h"
#include "vis_common.h"
#include "blender.h"
#include "blender_clsid.h"
#include "xrRender_console.h"
#include "PSLibrary.h"

// definition (Renderer)
class IRender_Target{
public:	
	virtual u32			get_width			()				{ return Device.dwWidth;	}
	virtual u32			get_height			()				{ return Device.dwHeight;	}
};


class	CRender{
    IRender_Target		Target;
public:
	// Data
	CFrustum			ViewBase;
	CPSLibrary			PSystems;
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

	IRender_Target*		getTarget		(){return &Target;}
};

IC  float   CalcSSA(Fvector& C, float R)
{
    float distSQ  = Device.m_Camera.GetPosition().distance_to_sqr(C);
    return  R*R/distSQ;
}
extern CRender  RImplementation;
extern CRender*	Render;

#endif
