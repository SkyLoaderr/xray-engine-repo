#ifndef _RENDER_H_
#define _RENDER_H_

#include "frustum.h"
#include "vis_common.h"

// refs
class ENGINE_API	CVisual;
class ENGINE_API	CObject;
class ENGINE_API	xrLIGHT;
class ENGINE_API	CLightPPA;
namespace PS	{ 
	struct ENGINE_API SDef;
	struct ENGINE_API SEmitter; 
};

const float							fLightSmoothFactor = 4.f;

// definition (Portal)
class	ENGINE_API	IRender_Portal
{
	virtual ~IRender_Portal()		{};
};

// definition (Sector)
class	ENGINE_API	IRender_Sector
{
	virtual ~IRender_Sector()		{};
};

// definition (Target)
class	ENGINE_API	IRender_target
{
public:
	virtual void					eff_load			(LPCSTR n)	= 0;
	virtual	void					set_blur			(float f)	= 0;
	virtual	void					set_gray			(float f)	= 0;
	virtual void					set_duality_h		(float f)	= 0;
	virtual void					set_duality_v		(float f)	= 0;
	virtual void					set_noise			(float f)	= 0;
	virtual void					set_noise_scale		(float f)	= 0;
	virtual void					set_noise_color		(u32 f)		= 0;
	virtual void					set_noise_fps		(float f)	= 0;
	virtual u32						get_width			()			= 0;
	virtual u32						get_height			()			= 0;
};

// definition (Renderer)
class	ENGINE_API	IRender_interface	:
	public pureDeviceCreate,
	public pureDeviceDestroy
{
public:
	// Data
	CObject*						val_pObject;
	Fmatrix*						val_pTransform;
	BOOL							val_bHUD;
	CFrustum						ViewBase;
	CFrustum*						View;
public:
	// Loading / Unloading
	virtual	void					level_Load				()								= 0;
	virtual void					level_Unload			()								= 0;

	// Information
	virtual int						getVisualsCount			()								= 0;
	virtual IRender_Portal*			getPortal				(int id)						= 0;
	virtual IRender_Sector*			getSector				(int id)						= 0;
	virtual IRender_Sector*			getSectorActive			()								= 0;
	virtual CVisual*				getVisual				(int id)						= 0;
	virtual D3DVERTEXELEMENT9*		getVB_Format			(int id)						= 0;
	virtual IDirect3DVertexBuffer9*	getVB					(int id)						= 0;
	virtual IDirect3DIndexBuffer9*	getIB					(int id)						= 0;
	virtual IRender_Sector*			detectSector			(Fvector& P)					= 0;
	virtual CRender_target*			getTarget				()								= 0;

	// Main 
	IC		void					set_Frustum				(CFrustum*	O	)				{ VERIFY(O);	View = O;			}
	IC		void					set_Transform			(Fmatrix*	M	)				{ VERIFY(M);	val_pTransform = M;	}
	IC		void					set_HUD					(BOOL 		V	)				{ val_bHUD		= V;				}
	IC		BOOL					get_HUD					()								{ return val_bHUD;					}
	virtual void					flush					()								= 0;	
	virtual void					set_Object				(CObject*	O	)				= 0;
	virtual void					add_Visual				(CVisual*	V	)				= 0;	// add visual leaf	(no culling performed at all)
	virtual void					add_Geometry			(CVisual*	V	)				= 0;	// add visual(s)	(all culling performed)
	virtual void					add_Lights				(vector<WORD> &V)				= 0;
	virtual void					add_Glows				(vector<WORD> &V)				= 0;
	virtual void					add_Patch				(Shader* S, const Fvector& P, float s, float a, BOOL bNearer)	= 0;
	virtual void					add_Wallmark			(Shader* S, const Fvector& P, float s, CDB::TRI* T)				= 0;

	// Lighting
	virtual void					L_add					(CLightPPA* L	)				= 0;
	virtual void					L_select				(Fvector &pos, float fRadius, vector<xrLIGHT*>&	dest)	= 0;

	// Models
	virtual CVisual*				model_CreatePS			(LPCSTR name, PS::SEmitter* E)			= 0;
	virtual CVisual*				model_Create			(LPCSTR name)							= 0;
	virtual CVisual*				model_Create			(LPCSTR name, IReader* data)			= 0;
	virtual CVisual*				model_Duplicate			(CVisual* V)							= 0;
	virtual void					model_Delete			(CVisual* &V)							= 0;

	// Occlusion culling
	virtual BOOL					occ_visible				(vis_data&	V)							= 0;
	virtual BOOL					occ_visible				(Fbox&		B)							= 0;
	virtual BOOL					occ_visible				(sPoly&		P)							= 0;

	// Main
	virtual void					Calculate				()										= 0;
	virtual void					Render					()										= 0;
	virtual void					RenderBox				(IRender_Sector* S, Fbox& BB, int sh)	= 0;
	virtual void					Screenshot				(BOOL bSquare=FALSE)					= 0;

	// Render mode
	virtual void					rmNear					()										= 0;
	virtual void					rmFar					()										= 0;
	virtual void					rmNormal				()										= 0;

	// Constructor/destructor
	CRender_interface();
	virtual ~CRender_interface();
};

extern ENGINE_API	CRender_interface*	Render;

#endif