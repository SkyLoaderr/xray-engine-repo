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
#include "ModelPool.h"

// definition (Renderer)
class IRender_Target{
public:	
	virtual u32			get_width			()				{ return Device.dwWidth;	}
	virtual u32			get_height			()				{ return Device.dwHeight;	}
};

class	CRender{
    IRender_Target			Target;
    Fmatrix					current_matrix;
public:
	// Data
	CFrustum				ViewBase;
	CPSLibrary				PSLibrary;

    CModelPool*				Models;
public:
	// Occlusion culling
	virtual BOOL			occ_visible		(Fbox&	B);
	virtual BOOL			occ_visible		(sPoly& P);
	virtual BOOL			occ_visible		(vis_data& P);

	// Constructor/destructor
							CRender			();
	virtual 				~CRender		();

	void 					Initialize		();
	void 					ShutDown		();

	void					OnDeviceCreate	();
	void					OnDeviceDestroy	();

    void					Calculate		();
    void					Render	 		();

	void					set_Transform	(Fmatrix* M);
	void					add_Visual   	(IRender_Visual* visual);

	virtual ref_shader		getShader		(int id);
	IRender_Target*			getTarget		(){return &Target;}

	virtual IRender_Visual*	model_Create			(LPCSTR name);
	virtual IRender_Visual*	model_Create			(LPCSTR name, IReader* data);
	virtual IRender_Visual*	model_CreatePE			(LPCSTR name);
	virtual IRender_Visual*	model_CreateParticles	(LPCSTR name);
    
    virtual IRender_DetailModel*	model_CreateDM	(IReader* R);
	virtual IRender_Visual*	model_Duplicate			(IRender_Visual* V);
	virtual void			model_Delete			(IRender_Visual* &	V, BOOL bDiscard=TRUE);
    virtual void			model_Delete			(IRender_DetailModel* & F)
    {
        if (F)
        {
            CDetail*	D	= (CDetail*)F;
            D->Unload		();
            xr_delete		(D);
            F				= NULL;
        }
    }
	void 					model_Render			(IRender_Visual* m_pVisual, const Fmatrix& mTransform, int priority, bool strictB2F, float m_fLOD);
	void 					model_RenderSingle		(IRender_Visual* m_pVisual, const Fmatrix& mTransform, float m_fLOD);

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
