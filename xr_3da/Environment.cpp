// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "resourcemanager.h"
#include "render.h"

#include "Environment.h"
#include "xr_effsun.h"
#include "xr_trims.h"
#include "customhud.h"

// font
#include "x_ray.h"
#include "gamefont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ENGINE_API float	psGravity	= 20.f;
ENGINE_API Flags32	psEnvFlags	= {effSunGlare};

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
		C.r_Sampler			("s_sky0",			"null",			D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r_Sampler			("s_sky1",			"null",			D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r_End				();
	}
};
static CBlender_skybox		b_skybox;

//////////////////////////////////////////////////////////////////////////
// vertex
struct v_skybox				{
	Fvector4	p;
	u32			color;
	Fvector3	uv	[2];
};
const	u32 v_skybox_fvf	= D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1);

//////////////////////////////////////////////////////////////////////////
// environment
CEnvironment::CEnvironment	()
{
	sh_2sky.create			(&b_skybox,"skybox_2t");
	sh_2geom.create			(v_skybox_fvf,RCache.Vertex.Buffer(), RCache.QuadIB);
}
CEnvironment::~CEnvironment	()
{
	for(u32 i=0; i<Suns.size(); i++) xr_delete(Suns[i]);
}

void CEnvDescriptor::load	(LPCSTR S)
{
	sky_texture				= Device.Resources->_CreateTexture(pSettings->r_string(S,"sky_texture"));
	sky_color				= pSettings->r_fvector3	(S,"sky_color");
	far_plane				= pSettings->r_float	(S,"far_plane");
	fog_color				= pSettings->r_fvector3	(S,"fog_color");
	fog_density				= pSettings->r_float	(S,"fog_density");
	ambient					= pSettings->r_fvector3	(S,"ambient");
	lmap_color				= pSettings->r_fvector3	(S,"lmap_color");
	hemi_color				= pSettings->r_fvector3	(S,"hemi_color");
	sun_color				= pSettings->r_fvector3	(S,"sun_color");
	Fvector2 sund			= pSettings->r_fvector2	(S,"sun_dir");	sun_dir.setHP	(deg2rad(sund.y),deg2rad(sund.x));
}
void CEnvDescriptor::lerp	(CEnvDescriptor& A, CEnvDescriptor& B, float f)
{
	float	fi	= 1-f;
	sky_0					= A.sky_texture;
	sky_1					= B.sky_texture;
	sky_factor				= f;
	sky_color.lerp			(A.sky_color,B.sky_color,f);
	far_plane				= fi*A.far_plane + f*B.far_plane;
	fog_color.lerp			(A.fog_color,B.fog_color,f);
	fog_density				= fi*A.fog_density + f*B.fog_density;
	fog_near				= (1.0f - fog_density)*0.85f * far_plane;
	fog_far					= 0.95f * far_plane;
	ambient.lerp			(A.ambient,B.ambient,f);
	lmap_color.lerp			(A.lmap_color,B.lmap_color,f);
	hemi_color.lerp			(A.hemi_color,B.hemi_color,f);
	sun_color.lerp			(A.sun_color,B.sun_color,f);
	sun_dir.lerp			(A.sun_dir,B.sun_dir,f).normalize();
}

void CEnvironment::Load		(CInifile *pIni, char *section)
{
	for(int env=0; env<24; env++) 
	{
		LPCSTR	sect		= "environment";
		char	name		[32];
		sprintf				(name,"%d",env);
		if (!pSettings->line_exist	(sect,name))	continue;
		CEnvDescriptor		D;
		D.load				(pSettings->r_string(sect,name));
		Palette.push_back	(D);
	}
	
	if (pIni->line_exist(section,"suns"))
	{
		LPCSTR		S;
		string256	name;
		CSun*		pSun;
		S = pIni->r_string(section,"suns");
		u32 scnt = _GetItemCount(S);
		for (u32 i=0; i<scnt; i++){
			_GetItem(S,i,name);
			pSun	= xr_new<CSun> (pIni, name);
			Suns.push_back(pSun);
		}
		R_ASSERT2	(Suns.size(), "Should be at least one 'sun' source in level");
	}

	// update suns
	for(u32 i=0; i<Suns.size(); i++) Suns[i]->Update();
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
	Fcolor c_fog;	c_fog.set	(Current.fog_color.x,Current.fog_color.y,Current.fog_color.z,0);
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,	c_fog.get	( ) )); 
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGSTART,	*(u32 *)(&Current.fog_near)	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGEND,	*(u32 *)(&Current.fog_far)	));

	for(u32 i=0; i<Suns.size(); i++) Suns[i]->Update();
}

extern float psHUD_FOV;
void CEnvironment::RenderFirst	()
{
	// Sun sources
	/* *********************** interfere with R2
	if (psEnvFlags.test(effSunGlare))
		for(u32 i=0; i<Suns.size(); i++) Suns[i]->RenderSource();
	*/
}

void CEnvironment::RenderLast	()
{
	if (psEnvFlags.test(effSunGlare))
		for(u32 i=0; i<Suns.size(); i++) Suns[i]->RenderFlares();
}
