#include "stdafx.h"
#pragma hdrstop

#include "resourcemanager.h"
#include "blenders\blender.h"
#include "render.h"

#include "Environment.h"
#include "xr_efflensflare.h"
#include "xr_trims.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ENGINE_API float	psGravity	= 20.f;

//////////////////////////////////////////////////////////////////////////
// shader/blender
class CBlender_skybox		: public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: combiner";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C)
	{
		C.r_Pass			("null",			"r1_sky2",		FALSE,	TRUE, FALSE);
		C.r_Sampler			("s_sky0",			"$null",		D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r_Sampler			("s_sky1",			"$null",		D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r_End				();
	}
};
static CBlender_skybox		b_skybox;

//////////////////////////////////////////////////////////////////////////
// vertex
#pragma pack(push,1)
struct v_skybox				{
	Fvector3	p;
	u32			color;
	Fvector3	uv	[2];

	void		set			(Fvector3& _p, u32 _c, Fvector3& _tc)
	{
		p		= _p;
		color	= _c;
		uv[0]	= _tc;
		uv[1]	= _tc;
	}
};
const	u32 v_skybox_fvf	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1);
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
// half box def
static	Fvector3	hbox_verts[24]	=
{
	{-1.f,	-1.f,	-1.f}, {-1.f,	-1.01f,	-1.f},	// down
	{ 1.f,	-1.f,	-1.f}, { 1.f,	-1.01f,	-1.f},	// down
	{-1.f,	-1.f,	 1.f}, {-1.f,	-1.01f,	 1.f},	// down
	{ 1.f,	-1.f,	 1.f}, { 1.f,	-1.01f,	 1.f},	// down
	{-1.f,	 1.f,	-1.f}, {-1.f,	 1.f,	-1.f},
	{ 1.f,	 1.f,	-1.f}, { 1.f,	 1.f,	-1.f},
	{-1.f,	 1.f,	 1.f}, {-1.f,	 1.f,	 1.f},
	{ 1.f,	 1.f,	 1.f}, { 1.f,	 1.f,	 1.f},
	{-1.f,	 0.f,	-1.f}, {-1.f,	-1.f,	-1.f},	// half
	{ 1.f,	 0.f,	-1.f}, { 1.f,	-1.f,	-1.f},	// half
	{ 1.f,	 0.f,	 1.f}, { 1.f,	-1.f,	 1.f},	// half
	{-1.f,	 0.f,	 1.f}, {-1.f,	-1.f,	 1.f}	// half
};
static	u16			hbox_faces[18*3]	=
{
	4,	 5,	 7,
	7,	 6,	 4,
	0,	 1,	 9,
	9,	 8,	 0,
	8,	 9,	 5,
	5,	 4,	 8,
	1,	 3,	10,
	10,	 9,	 1,
	9,	10,	 7,
	7,	 5,	 9,
	3,	 2,	11,
	11,	10,	 3,
	10,	11,	 6,
	6,	 7,	10,
	2,	 0,	 8,
	8,	11,	 2,
	11,	 8,	 4,
	4,	 6,	11
};

//////////////////////////////////////////////////////////////////////////
// environment
CEnvironment::CEnvironment	()
{
	eff_Rain				= 0;
    eff_LensFlare			= 0;
	OnDeviceCreate			();
}
CEnvironment::~CEnvironment	()
{
	OnDeviceDestroy			();
}

void CEnvDescriptor::load	(LPCSTR S)
{
	sky_texture				= Device.Resources->_CreateTexture(pSettings->r_string(S,"sky_texture"));
	sky_color				= pSettings->r_fvector3	(S,"sky_color");
	far_plane				= pSettings->r_float	(S,"far_plane");
	fog_color				= pSettings->r_fvector3	(S,"fog_color");
	fog_density				= pSettings->r_float	(S,"fog_density");
	rain_density			= pSettings->r_float	(S,"rain_density");
	rain_color				= pSettings->r_fvector3	(S,"rain_color");
	ambient					= pSettings->r_fvector3	(S,"ambient");
	lmap_color				= pSettings->r_fvector3	(S,"lmap_color");
	hemi_color				= pSettings->r_fvector3	(S,"hemi_color");
	sun_color				= pSettings->r_fvector3	(S,"sun_color");
	Fvector2 sund			= pSettings->r_fvector2	(S,"sun_dir");	sun_dir.setHP	(deg2rad(sund.y),deg2rad(sund.x));
}
void CEnvDescriptor::unload	()
{
	sky_r_textures.clear	();
	sky_r_textures.push_back(0);
	sky_r_textures.push_back(0);
}
void CEnvDescriptor::lerp	(CEnvDescriptor& A, CEnvDescriptor& B, float f)
{
	float	fi				= 1-f;
	sky_r_textures.clear	();
	sky_r_textures.push_back(A.sky_texture);
	sky_r_textures.push_back(B.sky_texture);
	sky_factor				= f;
	sky_color.lerp			(A.sky_color,B.sky_color,f);
	far_plane				= fi*A.far_plane + f*B.far_plane;
	fog_color.lerp			(A.fog_color,B.fog_color,f);
	fog_density				= fi*A.fog_density + f*B.fog_density;
	fog_near				= (1.0f - fog_density)*0.85f * far_plane;
	fog_far					= 0.95f * far_plane;
	rain_density			= fi*A.rain_density + f*B.rain_density;
	rain_color.lerp			(A.rain_color,B.rain_color,f);
	ambient.lerp			(A.ambient,B.ambient,f);
	lmap_color.lerp			(A.lmap_color,B.lmap_color,f);
	hemi_color.lerp			(A.hemi_color,B.hemi_color,f);
	sun_color.lerp			(A.sun_color,B.sun_color,f);
	sun_dir.lerp			(A.sun_dir,B.sun_dir,f).normalize();
}

void CEnvironment::load		()
{
	if (Palette.empty()){
        for(int env=0; env<24; env++) 
        {
            LPCSTR	sect		= "environment";
            string32 name;
            sprintf				(name,"%d",env);
            if (!pSettings->line_exist	(sect,name))	continue;
            CEnvDescriptor		D;
            D.load				(pSettings->r_string(sect,name));
            Palette.push_back	(D);
        }
    }
    // rain
    if (!eff_Rain)			eff_Rain = xr_new<CEffect_Rain>();
}

void CEnvironment::unload	()
{
    xr_delete				(eff_Rain);
	Palette.clear			();
    Current.unload			();
}

void CEnvironment::OnFrame()
{
	// ******************** Environment params (interpolation)
	float		t_pos	= Device.fTimeGlobal/10;
	float		t_ip;
	float		t_fact	= modff		(t_pos, &t_ip);
	int			f_1		= iFloor	(t_ip)	% Palette.size();
	int			f_2		= (f_1 + 1)			% Palette.size();
	CEnvDescriptor&	_A	= Palette	[f_1];
	CEnvDescriptor&	_B	= Palette	[f_2];
	Current.lerp		(_A,_B,t_fact);

	// ******************** Environment params (setting)
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,	color_rgba_f(Current.fog_color.x,Current.fog_color.y,Current.fog_color.z,0) )); 
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGSTART,	*(u32 *)(&Current.fog_near)	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGEND,	*(u32 *)(&Current.fog_far)	));
}

extern float psHUD_FOV;
void CEnvironment::RenderFirst	()
{
	// Render skybox/plane
	{
		::Render->rmFar				();
		Fmatrix						mSky;
		mSky.translate				(Device.vCameraPosition);
		RCache.set_xform_world		(mSky);

		u32		i_offset,v_offset;
		Fcolor	clr					= { Current.sky_color.x, Current.sky_color.y, Current.sky_color.z, Current.sky_factor };
		u32		C					= clr.get	();

		// Fill index buffer
		u16*	pib					= RCache.Index.Lock	(18*3,i_offset);
		CopyMemory					(pib,hbox_faces,18*3*2);
		RCache.Index.Unlock			(18*3);

		// Fill vertex buffer
		v_skybox* pv				= (v_skybox*)	RCache.Vertex.Lock	(12,sh_2geom.stride(),v_offset);
		for (u32 v=0; v<12; v++)
			pv[v].set				(hbox_verts[v*2],C,hbox_verts[v*2+1]);
		RCache.Vertex.Unlock		(12,sh_2geom.stride());

		// Render
		RCache.set_Geometry			(sh_2geom);
		RCache.set_Shader			(sh_2sky);
		RCache.set_Textures			(&Current.sky_r_textures);
		RCache.Render				(D3DPT_TRIANGLELIST,v_offset,0,12,i_offset,18);

		::Render->rmNormal			();
	}

	// Sun sources
	/* *********************** interfere with R2
	if (psEnvFlags.test(effSunGlare))
		for(u32 i=0; i<Suns.size(); i++) Suns[i]->RenderSource();
	*/
}

void CEnvironment::RenderLast		()
{
//	if (psEnvFlags.test(effSunGlare))
//		for(u32 i=0; i<Suns.size(); i++) Suns[i]->RenderFlares();

	eff_Rain->Render				();
}

void CEnvironment::OnDeviceCreate()
{
	sh_2sky.create			(&b_skybox,"skybox_2t");
	sh_2geom.create			(v_skybox_fvf,RCache.Vertex.Buffer(), RCache.Index.Buffer());
    load					();
}

void CEnvironment::OnDeviceDestroy()
{
	unload					();
	sh_2sky.destroy			();
	sh_2geom.destroy		();
}

