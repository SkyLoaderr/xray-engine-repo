//---------------------------------------------------------------------------
#ifndef renderH
#define renderH

#include "frustum.h"
#include "vis_common.h"
#include "blender.h"
#include "blender_clsid.h"
#include "xrRender_console.h"
#include "PSLibrary.h"
#include "IRenderDetailModel.H"
#include "DetailModel.H"

// definition (Renderer)
class IRender_Target{
public:	
	virtual u32			get_width			()				{ return Device.dwWidth;	}
	virtual u32			get_height			()				{ return Device.dwHeight;	}
};


class	CRender{
    IRender_Target			Target;
public:
	// Data
	CFrustum				ViewBase;
	CPSLibrary				PSLibrary;
public:
	// Occlusion culling
	virtual BOOL			occ_visible		(Fbox&	B);
	virtual BOOL			occ_visible		(sPoly& P);
	virtual BOOL			occ_visible		(vis_data& P);

	// Constructor/destructor
							CRender			();
	virtual 				~CRender		();

    void					Calculate		();
    void					Render	 		();

	IRender_Target*			getTarget		(){return &Target;}

	virtual IRender_Visual*	model_CreatePE			(LPCSTR name);
	virtual IRender_Visual*	model_CreateParticles	(LPCSTR name);
    
    virtual IRender_DetailModel*	model_CreateDM	(IReader* R);
    void					model_Delete	(IRender_DetailModel* & F)
    {
        if (F)
        {
            CDetail*	D	= (CDetail*)F;
            D->Unload		();
            xr_delete		(D);
            F				= NULL;
        }
    }

	// Render mode
	virtual void			rmNear					();
	virtual void			rmFar					();
	virtual void			rmNormal				();
};

IC  float   CalcSSA(Fvector& C, float R)
{
    float distSQ  = Device.m_Camera.GetPosition().distance_to_sqr(C);
    return  R*R/distSQ;
}
extern CRender  RImplementation;
extern CRender*	Render;

#endif
