#include "stdafx.h"
#pragma hdrstop

#include "xr_efflensflare.h"

#ifdef _LEVEL_EDITOR
	#include "ELight.h"
	#include "scene.h"
	#include "ui_main.h"
#else
	#include "xr_object.h"
	#include "igame_level.h"
	#include "Environment.h"
#endif


#ifdef _LEVEL_EDITOR
	#define FAR_DIST UI.ZFar()
#else
	#define FAR_DIST g_pGameLevel->Environment->Current.Far
#endif

#define MAX_Flares	12
//////////////////////////////////////////////////////////////////////////////
// Globals ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define BLEND_INC_SPEED 8.0f
#define BLEND_DEC_SPEED 4.0f

CLensFlare::CLensFlare()
{
	// Device
	bInit						= false;
	dwFrame						= 0xfffffffe;

    m_Flags.zero();
	fBlend						= 0.f;

    LightColor.set				( 0xFFFFFFFF );
	fGradientValue				= 0.f;

    hGeom						= 0;
}


CLensFlare::~CLensFlare()
{
	DDUnload			();

	// shaders
	m_Gradient.hShader.destroy	();
	m_Source.hShader.destroy	();
	for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++) it->hShader.destroy();

	// VS
	hGeom.destroy				();
}

ref_shader CLensFlare::CreateSourceShader(const char* tex_name)
{
	ref_shader	R;
	if			(tex_name&&tex_name[0])	R.create("effects\\sun",tex_name);
	return		R;
}

ref_shader CLensFlare::CreateFlareShader(const char* tex_name)
{
	ref_shader	R;
	if			(tex_name&&tex_name[0])	R.create("effects\\flare",tex_name);
	return		R;
}

void CLensFlare::SetSource(float fRadius, const char* tex_name)
{
	m_Source.fRadius	= fRadius;
    strcpy				(m_Source.texture,tex_name);
}

void CLensFlare::SetGradient(float fMaxRadius, float fOpacity, const char* tex_name)
{
	m_Gradient.fRadius	= fMaxRadius;
	m_Gradient.fOpacity	= fOpacity;
    strcpy				(m_Gradient.texture,tex_name);
}

void CLensFlare::AddFlare(float fRadius, float fOpacity, float fPosition, const char* tex_name)
{
	SFlare F;
	F.fRadius	= fRadius;
	F.fOpacity	= fOpacity;
    F.fPosition	= fPosition;
    strcpy				(F.texture,tex_name);
	m_Flares.push_back	(F);
}

void CLensFlare::Load( CInifile* pIni, LPCSTR section )
{
	LPCSTR		T,R,O,P;
	string256	name;
	float r, o, p;
	m_Flags.set	(flSource,pIni->r_bool(section,"source" ));
	if (m_Flags.is(flSource)){
		T = pIni->r_string 	( section,"source_texture" );
		r = pIni->r_float	( section,"source_radius" );
		SetSource(r,T);
	}
	m_Flags.set	(flFlare,pIni->r_bool ( section,"flares" ));
	if (m_Flags.is(flFlare)){
		T = pIni->r_string 	( section,"flare_textures" );
		R = pIni->r_string 	( section,"flare_radius" );
		O = pIni->r_string 	( section,"flare_opacity");
		P = pIni->r_string 	( section,"flare_position");
		u32 tcnt = _GetItemCount(T);
		for (u32 i=0; i<tcnt; i++){
			_GetItem(R,i,name); r=(float)atof(name);
			_GetItem(O,i,name); o=(float)atof(name);
			_GetItem(P,i,name); p=(float)atof(name);
			_GetItem(T,i,name);
			AddFlare(r,o,p,name);
		}
	}
	m_Flags.set	(flGradient,CInifile::IsBOOL(pIni->r_string( section, "gradient")));
	if (m_Flags.is(flGradient)){
		T = pIni->r_string( section,"gradient_texture" );
		r = pIni->r_float	( section,"gradient_radius"  );
		o = pIni->r_float	( section,"gradient_opacity" );
		SetGradient(r,o,T);
	}

	DDLoad();

  	bInit			= false;
}

void CLensFlare::OnFrame()
{
	VERIFY(bInit);
	if (dwFrame==Device.dwFrame) return;
	dwFrame			= Device.dwFrame;
	//
	// Compute center and axis of flares
	//
	float fDot;

	Fvector vecPos;

	Fmatrix	matEffCamPos;
	matEffCamPos.identity();
	// Calculate our position and direction

#ifdef _LEVEL_EDITOR
	matEffCamPos.i.set(Device.m_Camera.GetRight());
	matEffCamPos.j.set(Device.m_Camera.GetNormal());
	matEffCamPos.k.set(Device.m_Camera.GetDirection());
	vecPos.set(Device.m_Camera.GetPosition());
#else
	matEffCamPos.i.set(Device.vCameraRight);
	matEffCamPos.j.set(Device.vCameraTop);
	matEffCamPos.k.set(Device.vCameraDirection);
	vecPos.set(Device.vCameraPosition);
#endif
	vecDir.set(0.0f, 0.0f, 1.0f);
	matEffCamPos.transform_dir(vecDir);
	vecDir.normalize();

	// Figure out of light (or flare) might be visible
	vecLight.set(vSunDir);
	vecLight.normalize();

	fDot = vecLight.dotproduct(vecDir);

	if(fDot <= 0.01f){	bRender = false; return;} else bRender = true;

	// Calculate the point directly in front of us, on the far clip plane
	float 	fDistance	= FAR_DIST*0.75f;
	vecCenter.mul(vecDir, fDistance);
	vecCenter.add(vecPos);
	// Calculate position of light on the far clip plane
	vecLight.mul(fDistance / fDot);
	vecLight.add(vecPos);
	// Compute axis which goes from light through the center of the screen
	vecAxis.sub(vecLight, vecCenter);

	//
	// Figure out if light is behind something else
	vecX.set(1.0f, 0.0f, 0.0f);
	matEffCamPos.transform_dir(vecX);
	vecX.normalize();
	vecY.crossproduct(vecX, vecDir);

#ifdef _LEVEL_EDITOR
	if ( Scene.RayPick(Device.m_Camera.GetPosition(), vSunDir, OBJCLASS_SCENEOBJECT, 0, false, 0))
#else
	CObject*	o_main		= g_pGameLevel->CurrentViewEntity();
	BOOL		o_enable	= FALSE;
	if (o_main)
	{
		o_enable		=	o_main->getEnabled();
		o_main->setEnabled	(FALSE);
	}
	if ( g_pGameLevel->ObjectSpace.RayTest( Device.vCameraPosition, vSunDir, 1000.f) )
#endif
	{
		fBlend = fBlend - BLEND_DEC_SPEED * Device.fTimeDelta;
	}else{
		fBlend = fBlend + BLEND_INC_SPEED * Device.fTimeDelta;
	}
	clamp( fBlend, 0.0f, 1.0f );

#ifdef _LEVEL_EDITOR
#else
	if (o_main)				o_main->setEnabled	(o_enable);
#endif


	// gradient
	if (m_Flags.is(flGradient)){
		Fvector				scr_pos;
		Device.mFullTransform.transform	( scr_pos, vecLight );
		float kx = 1, ky = 1;
		float sun_blend		= 0.5f;
		float sun_max		= 2.5f;
		scr_pos.y			*= -1;

		if (_abs(scr_pos.x) > sun_blend)	kx = ((sun_max - (float)_abs(scr_pos.x))) / (sun_max - sun_blend);
		if (_abs(scr_pos.y) > sun_blend)	ky = ((sun_max - (float)_abs(scr_pos.y))) / (sun_max - sun_blend);

		if (!((_abs(scr_pos.x) > sun_max) || (_abs(scr_pos.y) > sun_max)))
			fGradientValue	= kx * ky * m_Gradient.fOpacity * fBlend;
		else
			fGradientValue	= 0;
	}
}

void CLensFlare::Render(BOOL bSun, BOOL bFlares, BOOL bGradient)
{
	VERIFY				(bInit);
	OnFrame				();
	if (!bRender)		return;

	Fcolor				dwLight;
	Fcolor				color;
	Fvector				vec, vecSx, vecSy;
	Fvector				vecDx, vecDy;

	dwLight.set							( LightColor );
	svector<ref_shader,MAX_Flares>		_2render;

	u32									VS_Offset;
	FVF::LIT *pv						= (FVF::LIT*) RCache.Vertex.Lock(2*MAX_Flares*4,hGeom.stride(),VS_Offset);

	float 	fDistance					= FAR_DIST*0.75f;

	if (m_Flags.is(flSource)&&bSun)
	{
		vecSx.mul			(vecX, m_Source.fRadius*fDistance);
		vecSy.mul			(vecY, m_Source.fRadius*fDistance);
		color.set			( dwLight );

		u32 c				= color.get();
		pv->set				(vecLight.x+vecSx.x-vecSy.x, vecLight.y+vecSx.y-vecSy.y, vecLight.z+vecSx.z-vecSy.z, c, 0, 0); pv++;
		pv->set				(vecLight.x+vecSx.x+vecSy.x, vecLight.y+vecSx.y+vecSy.y, vecLight.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
		pv->set				(vecLight.x-vecSx.x-vecSy.x, vecLight.y-vecSx.y-vecSy.y, vecLight.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
		pv->set				(vecLight.x-vecSx.x+vecSy.x, vecLight.y-vecSx.y+vecSy.y, vecLight.z-vecSx.z+vecSy.z, c, 1, 1); pv++;
		_2render.push_back	(m_Source.hShader);
	}

	if (fBlend>=EPS_L)
	{
		if(bFlares&&(m_Flags.is(flFlare)))
		{
			vecDx.normalize		(vecAxis);
			vecDy.crossproduct	(vecDx, vecDir);
			for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++)
			{
				SFlare&	F			= *it;
				vec.mul				(vecAxis, F.fPosition);
				vec.add				(vecCenter);
				vecSx.mul			(vecDx, F.fRadius*fDistance);
				vecSy.mul			(vecDy, F.fRadius*fDistance);
				float    cl			= F.fOpacity * fBlend;
				color.set			( dwLight );
				color.mul_rgba		( cl );
				u32 c				= color.get();
				pv->set				(vec.x+vecSx.x-vecSy.x, vec.y+vecSx.y-vecSy.y, vec.z+vecSx.z-vecSy.z, c, 0, 0); pv++;
				pv->set				(vec.x+vecSx.x+vecSy.x, vec.y+vecSx.y+vecSy.y, vec.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
				pv->set				(vec.x-vecSx.x-vecSy.x, vec.y-vecSx.y-vecSy.y, vec.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
				pv->set				(vec.x-vecSx.x+vecSy.x, vec.y-vecSx.y+vecSy.y, vec.z-vecSx.z+vecSy.z, c, 1, 1); pv++;
				_2render.push_back	(it->hShader);
			}
		}

		// gradient
		if (bGradient && (m_Flags.is(flGradient)) && (fGradientValue>=EPS_L))
		{
			vecSx.mul				(vecX, m_Gradient.fRadius*fGradientValue*fDistance);
			vecSy.mul				(vecY, m_Gradient.fRadius*fGradientValue*fDistance);

			color.set				( dwLight );
			color.mul_rgba			( fGradientValue );

			u32 c					= color.get	();
			pv->set					(vecLight.x+vecSx.x-vecSy.x, vecLight.y+vecSx.y-vecSy.y, vecLight.z+vecSx.z-vecSy.z, c, 0, 0); pv++;
			pv->set					(vecLight.x+vecSx.x+vecSy.x, vecLight.y+vecSx.y+vecSy.y, vecLight.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
			pv->set					(vecLight.x-vecSx.x-vecSy.x, vecLight.y-vecSx.y-vecSy.y, vecLight.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
			pv->set					(vecLight.x-vecSx.x+vecSy.x, vecLight.y-vecSx.y+vecSy.y, vecLight.z-vecSx.z+vecSy.z, c, 1, 1); pv++;
			_2render.push_back		(m_Gradient.hShader);
		}
	}

	RCache.Vertex.Unlock	(_2render.size()*4,hGeom.stride());

	RCache.set_xform_world	(Fidentity);
	RCache.set_Geometry		(hGeom);
	for (u32 i=0; i<_2render.size(); i++)
	{
    	if (_2render[i])
		{
			u32						vBase	= i*4+VS_Offset;
			RCache.set_Shader		(_2render[i]);
			/*
			CHK_DX					(HW.pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE));
			CHK_DX					(HW.pDevice->SetRenderState(D3DRS_SRCBLEND,		D3DBLEND_ONE));
			CHK_DX					(HW.pDevice->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_ONE));
			*/
			RCache.Render			(D3DPT_TRIANGLELIST,vBase, 0,4,0,2);
	    }
	}
}

void CLensFlare::Update( Fvector& sun_dir, Fcolor& color )
{
	vSunDir.mul		(sun_dir,-1);

	// color
	LightColor.set	(color);
	LightColor.a	= 1;

	bInit			= true;
}

void CLensFlare::DDLoad	()
{
	// VS
	hGeom.create		(FVF::F_LIT,RCache.Vertex.Buffer(),RCache.QuadIB);

	// shaders
	m_Gradient.hShader	= CreateFlareShader		(m_Gradient.texture);
	m_Source.hShader	= CreateSourceShader	(m_Source.texture);
    for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++) it->hShader = CreateFlareShader(it->texture);
}

void CLensFlare::DDUnload()
{
	// shaders
	m_Gradient.hShader.destroy	();
	m_Source.hShader.destroy	();
    for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++) it->hShader.destroy();

	// VS
	hGeom.destroy();
}

