#ifndef _RENDER_H_
#define _RENDER_H_

#include "frustum.h"
#include "vis_common.h"
#include "IRenderDetailModel.h"

// refs
class ENGINE_API	IRenderable;
class ENGINE_API	IRender_Visual;
class ENGINE_API	IBlender;
class ENGINE_API	CSkeletonWallmark;
class ENGINE_API	CKinematics;

const	float		fLightSmoothFactor = 4.f;

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Light)
class	ENGINE_API	IRender_Light		{
public:
	enum LT
	{
		DIRECT		= 0,
		POINT		= 1,
		SPOT		= 2,
		OMNIPART	= 3,
		REFLECTED	= 4,
	};
public:
	virtual void					set_type			(LT type)							= 0;
	virtual void					set_active			(bool)								= 0;
	virtual bool					get_active			()									= 0;
	virtual void					set_shadow			(bool)								= 0;
	virtual void					set_indirect		(bool)								{};
	virtual void					set_position		(const Fvector& P)					= 0;
	virtual void					set_rotation		(const Fvector& D, const Fvector& R)= 0;
	virtual void					set_cone			(float angle)						= 0;
	virtual void					set_range			(float R)							= 0;
	virtual void					set_virtual_size	(float R)							= 0;
	virtual void					set_texture			(LPCSTR name)						= 0;
	virtual void					set_color			(const Fcolor& C)					= 0;
	virtual void					set_color			(float r, float g, float b)			= 0;
	virtual ~IRender_Light()		{};
};

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Glow)
class	ENGINE_API	IRender_Glow		{
public:
	virtual void					set_active			(bool)								= 0;
	virtual bool					get_active			()									= 0;
	virtual void					set_position		(const Fvector& P)					= 0;
	virtual void					set_direction		(const Fvector& P)					= 0;
	virtual void					set_radius			(float R)							= 0;
	virtual void					set_texture			(LPCSTR name)						= 0;
	virtual void					set_color			(const Fcolor& C)					= 0;
	virtual void					set_color			(float r, float g, float b)			= 0;
	virtual ~IRender_Glow()			{};
};
//////////////////////////////////////////////////////////////////////////
// definition (Per-object render-specific data)
class	ENGINE_API	IRender_ObjectSpecific		{
public:
	virtual ~IRender_ObjectSpecific()	{};
};

//////////////////////////////////////////////////////////////////////////
// definition (Portal)
class	ENGINE_API	IRender_Portal				{
public:
	virtual ~IRender_Portal()			{};
};

//////////////////////////////////////////////////////////////////////////
// definition (Sector)
class	ENGINE_API	IRender_Sector				{
public:
	virtual ~IRender_Sector()			{};
};

//////////////////////////////////////////////////////////////////////////
// definition (Target)
class	ENGINE_API	IRender_Target				{
public:
	virtual	void					set_blur			(float	f)							= 0;
	virtual	void					set_gray			(float	f)							= 0;
	virtual void					set_duality_h		(float	f)							= 0;
	virtual void					set_duality_v		(float	f)							= 0;
	virtual void					set_noise			(float	f)							= 0;
	virtual void					set_noise_scale		(float	f)							= 0;
	virtual void					set_noise_fps		(float	f)							= 0;
	virtual void					set_color_base		(u32	f)							= 0;
	virtual void					set_color_gray		(u32	f)							= 0;
	virtual void					set_color_add		(u32	f)							= 0;
	virtual u32						get_width			()									= 0;
	virtual u32						get_height			()									= 0;
	virtual ~IRender_Target()		{};
};

//////////////////////////////////////////////////////////////////////////
// definition (Renderer)
class	ENGINE_API	IRender_interface
{
public:
	// options
	s32								m_skinning;

	// data
	CFrustum						ViewBase;
	CFrustum*						View;
public:
	// Loading / Unloading
	virtual	void					create					()											= 0;
	virtual	void					destroy					()											= 0;
	virtual	void					reset_begin				()											= 0;
	virtual	void					reset_end				()											= 0;

	virtual	void					level_Load				()											= 0;
	virtual void					level_Unload			()											= 0;

	virtual IDirect3DBaseTexture9*	texture_load			(LPCSTR	fname)								= 0;
			void					shader_option_skinning	(s32 mode)									{ m_skinning=mode;	}
	virtual HRESULT					shader_compile			(
		LPCSTR							name,
		LPCSTR                          pSrcData,
		UINT                            SrcDataLen,
		void*							pDefines,
		void*							pInclude,
		LPCSTR                          pFunctionName,
		LPCSTR                          pTarget,
		DWORD                           Flags,
		void*							ppShader,
		void*							ppErrorMsgs,
		void*							ppConstantTable)												= 0;

	// Information
	virtual	void					Statistics				(CGameFont* F	)							{};

	virtual LPCSTR					getShaderPath			()											= 0;
	virtual ref_shader				getShader				(int id)									= 0;
	virtual IRender_Sector*			getSector				(int id)									= 0;
	virtual IRender_Visual*			getVisual				(int id)									= 0;
	virtual D3DVERTEXELEMENT9*		getVB_Format			(int id)									= 0;
	virtual IDirect3DVertexBuffer9*	getVB					(int id)									= 0;
	virtual IDirect3DIndexBuffer9*	getIB					(int id)									= 0;
	virtual IRender_Sector*			detectSector			(const Fvector& P)								= 0;
	virtual IRender_Target*			getTarget				()											= 0;

	// Main 
	IC		void					set_Frustum				(CFrustum*	O	)							{ VERIFY(O);	View = O;			}
	virtual void					set_Transform			(Fmatrix*	M	)							= 0;
	virtual void					set_HUD					(BOOL 		V	)							= 0;
	virtual void					set_Invisible			(BOOL 		V	)							= 0;
	virtual void					flush					()											= 0;	
	virtual void					set_Object				(IRenderable*		O	)					= 0;
	virtual	void					add_Occluder			(Fbox2&	bb_screenspace	)					= 0;	// mask screen region as oclluded (-1..1, -1..1)
	virtual void					add_Visual				(IRender_Visual*	V	)					= 0;	// add visual leaf	(no culling performed at all)
	virtual void					add_Geometry			(IRender_Visual*	V	)					= 0;	// add visual(s)	(all culling performed)
	virtual void					add_StaticWallmark		(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V)=0;
	virtual void					add_SkeletonWallmark	(CSkeletonWallmark* wm)						= 0;
	virtual void					add_SkeletonWallmark	(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)=0;

	virtual IBlender*				blender_create			(CLASS_ID cls)								= 0;
	virtual void					blender_destroy			(IBlender* &)								= 0;

	virtual IRender_ObjectSpecific*	ros_create				(IRenderable* parent)						= 0;
	virtual void					ros_destroy				(IRender_ObjectSpecific* &)					= 0;

	// Lighting/glowing
	virtual IRender_Light*			light_create			()											= 0;
	virtual void					light_destroy			(IRender_Light* &)							= 0;
	virtual IRender_Glow*			glow_create				()											= 0;
	virtual void					glow_destroy			(IRender_Glow* &)							= 0;

	// Models
	virtual IRender_Visual*			model_CreateParticles	(LPCSTR name)								= 0;
	virtual IRender_DetailModel*	model_CreateDM			(IReader*	F)								= 0;
	virtual IRender_Visual*			model_Create			(LPCSTR name, IReader*	data=0)				= 0;
	virtual IRender_Visual*			model_CreateChild		(LPCSTR name, IReader*	data)				= 0;
	virtual IRender_Visual*			model_Duplicate			(IRender_Visual*	V)						= 0;
	virtual void					model_Delete			(IRender_Visual* &	V, BOOL bDiscard=FALSE)	= 0;
	virtual void 					model_Delete			(IRender_DetailModel* & F)					= 0;
	virtual void					model_Logging			(BOOL bEnable)								= 0;

	// Occlusion culling
	virtual BOOL					occ_visible				(vis_data&	V)								= 0;
	virtual BOOL					occ_visible				(Fbox&		B)								= 0;
	virtual BOOL					occ_visible				(sPoly&		P)								= 0;

	// Main
	virtual void					Calculate				()											= 0;
	virtual void					Render					()											= 0;
	virtual void					Screenshot				(LPCSTR postfix=0, BOOL bSquare=FALSE)		= 0;

	// Render mode
	virtual void					rmNear					()											= 0;
	virtual void					rmFar					()											= 0;
	virtual void					rmNormal				()											= 0;

	// Constructor/destructor
	virtual ~IRender_interface();
};

extern ENGINE_API	IRender_interface*	Render;

#endif