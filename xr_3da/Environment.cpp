// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "resourcemanager.h"
#include "blenders\blender.h"
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
	Fvector4	p;
	u32			color;
	Fvector3	uv	[2];

	void		set			(float _x, float _y, float _z, float _w, u32 _c, Fvector3& _tc)
	{
		p.set	(_x,_y,_z,_w);
		color	= _c;
		uv[0]	= _tc;
		uv[1]	= _tc;
	}
};
const	u32 v_skybox_fvf	= D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1);
#pragma pack(pop)

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
	Fvector			P	= Device.vCameraPosition;
	Fvector			D	= Device.vCameraDirection;
	Fvector			U	= Device.vCameraTop;

	// Calc vectors
	float YFov			= deg2rad(Device.fFOV*Device.fASPECT);	float wT=tanf(YFov*0.5f);	float wB=-wT;
	float XFov			= deg2rad(Device.fFOV);					float wR=tanf(XFov*0.5f);	float wL=-wR;

	// calc x-axis (viewhoriz) and store cop
	// here we are assuring that vectors are perpendicular & normalized
	Fvector				R,COP;
	R.crossproduct		(D,U);
	R.normalize			();
	COP.set				(P);

	// calculate the corner vertices of the window
	Fvector				sPts	[4];  // silhouette points (corners of window)
	Fvector				dirs	[4];
	Fvector				Offset,	T;
	Offset.add			(D,COP);

	// find projector direction vectors (from cop through silhouette pts)
	T.mad(Offset,U,wT);	sPts[0].mad(T,R,wR); dirs[0].sub(sPts[0],COP).normalize();// 0=TR
	T.mad(Offset,U,wT);	sPts[1].mad(T,R,wL); dirs[1].sub(sPts[1],COP).normalize();// 1=TL
	T.mad(Offset,U,wB);	sPts[2].mad(T,R,wL); dirs[2].sub(sPts[2],COP).normalize();// 2=BL
	T.mad(Offset,U,wB);	sPts[3].mad(T,R,wR); dirs[3].sub(sPts[3],COP).normalize();// 3=BR

	// Render skybox/plane
	{
		u32		Offset;
		Fcolor	clr					= { Current.sky_color.x, Current.sky_color.y, Current.sky_color.z, Current.sky_factor };
		u32		C					= clr.get	();
		float	_w					= float		(::Render->getTarget()->get_width());
		float	_h					= float		(::Render->getTarget()->get_height());

		// Analyze depth
		float						d_Z	= .99f, d_W = 1.f;

		// Fill vertex buffer
		v_skybox* pv				= (v_skybox*)	RCache.Vertex.Lock	(4,sh_2geom.stride(),Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, dirs[3]);	pv++; // BL
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, dirs[0]);	pv++; // TL
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, dirs[2]);	pv++; // BR
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, dirs[1]);	pv++; // TR
		RCache.Vertex.Unlock		(4,sh_2geom.stride());
		RCache.set_Geometry			(sh_2geom);
		RCache.set_Shader			(sh_2sky);
		RCache.set_Textures			(&Current.sky_r_textures);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

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
