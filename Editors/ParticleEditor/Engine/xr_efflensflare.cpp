#include "stdafx.h"
#pragma hdrstop

#include "xr_efflensflare.h"
#include "xr_trims.h"
#include "xr_tokens.h"
#include "xr_ini.h"

#ifdef _LEVEL_EDITOR
	#include "ELight.h"
	#include "ui_main.h"
	#include "scene.h"
#else
	#include "xr_creator.h"
#endif


#ifdef _LEVEL_EDITOR
	#define FAR_DIST UI.ZFar()
#else
	#define FAR_DIST pCreator->Environment.Current.Far
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
#ifndef _EDITOR
	Device.seqDevDestroy.Add	(this);
	Device.seqDevCreate.Add		(this);
	if (Device.bReady) OnDeviceCreate();
#endif

	bInit				= false;

    m_Flags.bFlare		= FALSE;
    m_Flags.bSource		= FALSE;
    m_Flags.bGradient	= FALSE;
	fBlend				= 0.f;

    LightColor.set		( 0xFFFFFFFF );
	fGradientValue		= 0.f;
}


CLensFlare::~CLensFlare()
{
	if (Device.bReady) OnDeviceDestroy();
#ifndef _EDITOR
	Device.seqDevDestroy.Remove	(this);
	Device.seqDevCreate.Remove	(this);
#endif
}

void CLensFlare::OnDeviceCreate()
{
	// pre-build indices
	WORD	Indices[MAX_Flares*2*3];
	int		Cnt = 0;
	int		ICnt= 0;
	for (int i=0; i<MAX_Flares; i++){
		Indices[ICnt++]=Cnt;
		Indices[ICnt++]=Cnt+1;
		Indices[ICnt++]=Cnt+2;

		Indices[ICnt++]=Cnt+2;
		Indices[ICnt++]=Cnt+3;
		Indices[ICnt++]=Cnt+0;

		Cnt+=4;
	}
	P.VB_Create(FVF::F_LIT, MAX_Flares*4, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC);
	P.IB_Create(0, MAX_Flares*2*3, D3DUSAGE_WRITEONLY, Indices);
	// shaders
	m_Gradient.hShader	= CreateFlareShader(m_Gradient.texture);
	m_Source.hShader	= CreateSourceShader(m_Source.texture);
    for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++) it->hShader = CreateFlareShader(it->texture);
}

void CLensFlare::OnDeviceDestroy()
{
	P.IB_Destroy();
	P.VB_Destroy();
	// shaders
	if (m_Gradient.hShader) Device.Shader.Delete(m_Gradient.hShader);
	if (m_Source.hShader)	Device.Shader.Delete(m_Source.hShader);
    for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++) if (it->hShader) Device.Shader.Delete(it->hShader);
}

Shader* CLensFlare::CreateSourceShader(const char* tex_name)
{
	return (tex_name&&tex_name[0])?Device.Shader.Create("effects\\sun",tex_name):0;
}

Shader* CLensFlare::CreateFlareShader(const char* tex_name)
{
	return (tex_name&&tex_name[0])?Device.Shader.Create("effects\\flare",tex_name):0;
}

void CLensFlare::SetSource(float fRadius, const char* tex_name)
{
	m_Source.fRadius	= fRadius;
	m_Source.hShader	= CreateSourceShader(tex_name);
    strcpy(m_Source.texture,tex_name);
}

void CLensFlare::SetGradient(float fMaxRadius, float fOpacity, const char* tex_name)
{
	m_Gradient.fRadius	= fMaxRadius;
	m_Gradient.hShader	= CreateFlareShader(tex_name);
	m_Gradient.fOpacity	= fOpacity;
    strcpy(m_Gradient.texture,tex_name);
}

void CLensFlare::AddFlare(float fRadius, float fOpacity, float fPosition, const char* tex_name)
{
	SFlare F;
	F.fRadius	= fRadius;
	F.fOpacity	= fOpacity;
    F.fPosition	= fPosition;
	F.hShader	= CreateFlareShader(tex_name);
    strcpy(F.texture,tex_name);
	m_Flares.push_back(F);
}

void CLensFlare::Load( CInifile* pIni, LPSTR section )
{
	LPCSTR		T,R,O,P;
	FILE_NAME	name;
	float r, o, p;
	m_Flags.bSource = pIni->ReadBOOL ( section,"source" );
	if (m_Flags.bSource){
		T = pIni->ReadSTRING ( section,"source_texture" );
		r = pIni->ReadFLOAT	 ( section,"source_radius" );
		SetSource(r,T);
	}
	m_Flags.bFlare = pIni->ReadBOOL ( section,"flares" );
	if (m_Flags.bFlare){
		T = pIni->ReadSTRING ( section,"flare_textures" );
		R = pIni->ReadSTRING ( section,"flare_radius" );
		O = pIni->ReadSTRING ( section,"flare_opacity");
		P = pIni->ReadSTRING ( section,"flare_position");
		DWORD tcnt = _GetItemCount(T);
		for (DWORD i=0; i<tcnt; i++){
			_GetItem(R,i,name); r=(float)atof(name);
			_GetItem(O,i,name); o=(float)atof(name);
			_GetItem(P,i,name); p=(float)atof(name);
			_GetItem(T,i,name);
			AddFlare(r,o,p,name);
		}
	}
	m_Flags.bGradient = pIni->ReadTOKEN( section, "gradient", BOOL_token );
	if (m_Flags.bGradient){
		T = pIni->ReadSTRING( section,"gradient_texture" );
		r = pIni->ReadFLOAT	( section,"gradient_radius"  );
		o = pIni->ReadFLOAT	( section,"gradient_opacity" );
		SetGradient(r,o,T);
	}
	bInit			= false;
}

void CLensFlare::OnMove()
{
	VERIFY(bInit);
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
	if ( pCreator->ObjectSpace.RayTest( Device.vCameraPosition, vSunDir) )
#endif
	{
		fBlend = fBlend - BLEND_DEC_SPEED * Device.fTimeDelta;
	}else{
		fBlend = fBlend + BLEND_INC_SPEED * Device.fTimeDelta;
	}
	clamp( fBlend, 0.0f, 1.0f );
	
	// gradient
	if (m_Flags.bGradient){
		Fvector				scr_pos;
		Device.mFullTransform.transform	( scr_pos, vecLight );
		float kx = 1, ky = 1;
		float sun_blend		= 0.5f;
		float sun_max		= 1.75f;
		scr_pos.y			*= -1;
		
		if (fabsf(scr_pos.x) > sun_blend)	kx = ((sun_max - (float)fabsf(scr_pos.x))) / (sun_max - sun_blend);
		if (fabsf(scr_pos.y) > sun_blend)	ky = ((sun_max - (float)fabsf(scr_pos.y))) / (sun_max - sun_blend);
		
		if (!((fabsf(scr_pos.x) > sun_max) || (fabsf(scr_pos.y) > sun_max)))
			fGradientValue	= kx * ky * m_Gradient.fOpacity * fBlend;
		else
			fGradientValue	= 0;
	}
}

void CLensFlare::Render(BOOL bSun, BOOL bFlares, BOOL bGradient)
{
	VERIFY(bInit);
	if (!bRender) return;
	
	Fcolor dwLight;
	Fcolor color;
	Fvector vec, vecSx, vecSy;
	Fvector vecDx, vecDy;
	
	dwLight.set		( LightColor );
	vector<SFlare*>	rlist;
	
	FVF::LIT *pv = (FVF::LIT*) P.VB_Lock(D3DLOCK_DISCARD);
	
	float 	fDistance	= FAR_DIST*0.75f;
	
	if (m_Flags.bSource&&bSun){
		vecSx.mul(vecX, m_Source.fRadius*fDistance);
		vecSy.mul(vecY, m_Source.fRadius*fDistance);

		color.set		( dwLight );

		DWORD c = color.get();
		pv->set(vecLight.x+vecSx.x-vecSy.x, vecLight.y+vecSx.y-vecSy.y, vecLight.z+vecSx.z-vecSy.z, c, 1, 0); pv++;
		pv->set(vecLight.x+vecSx.x+vecSy.x, vecLight.y+vecSx.y+vecSy.y, vecLight.z+vecSx.z+vecSy.z, c, 1, 1); pv++;
		pv->set(vecLight.x-vecSx.x+vecSy.x, vecLight.y-vecSx.y+vecSy.y, vecLight.z-vecSx.z+vecSy.z, c, 0, 1); pv++;
		pv->set(vecLight.x-vecSx.x-vecSy.x, vecLight.y-vecSx.y-vecSy.y, vecLight.z-vecSx.z-vecSy.z, c, 0, 0); pv++;
		rlist.push_back(&m_Source);
	}
	
	if (fBlend>=EPS_L){
		if(bFlares&&m_Flags.bFlare){
			vecDx.normalize(vecAxis);
			vecDy.crossproduct(vecDx, vecDir);
			for (FlareIt it=m_Flares.begin(); it!=m_Flares.end(); it++){
				SFlare&	F		= *it;
				vec.mul(vecAxis, F.fPosition);
				vec.add(vecCenter);
				vecSx.mul(vecDx, F.fRadius*fDistance);
				vecSy.mul(vecDy, F.fRadius*fDistance);
				float    cl		= F.fOpacity * fBlend;
				color.set		( dwLight );
				color.mul_rgb	( cl );
				DWORD c=color.get();
				pv->set(vec.x+vecSx.x-vecSy.x, vec.y+vecSx.y-vecSy.y, vec.z+vecSx.z-vecSy.z, c, 1, 0); pv++;
				pv->set(vec.x+vecSx.x+vecSy.x, vec.y+vecSx.y+vecSy.y, vec.z+vecSx.z+vecSy.z, c, 1, 1); pv++;
				pv->set(vec.x-vecSx.x+vecSy.x, vec.y-vecSx.y+vecSy.y, vec.z-vecSx.z+vecSy.z, c, 0, 1); pv++;
				pv->set(vec.x-vecSx.x-vecSy.x, vec.y-vecSx.y-vecSy.y, vec.z-vecSx.z-vecSy.z, c, 0, 0); pv++;
				rlist.push_back(it);
			}
		}

		// gradient
		if (bGradient&&m_Flags.bGradient&&(fGradientValue>=EPS_L)){
			vecSx.mul(vecX, m_Gradient.fRadius*fGradientValue*fDistance);
			vecSy.mul(vecY, m_Gradient.fRadius*fGradientValue*fDistance);

			color.set		( dwLight );
			color.mul_rgba	( fGradientValue );

			DWORD c = color.get();
			pv->set(vecLight.x+vecSx.x-vecSy.x, vecLight.y+vecSx.y-vecSy.y, vecLight.z+vecSx.z-vecSy.z, c, 1, 0); pv++;
			pv->set(vecLight.x+vecSx.x+vecSy.x, vecLight.y+vecSx.y+vecSy.y, vecLight.z+vecSx.z+vecSy.z, c, 1, 1); pv++;
			pv->set(vecLight.x-vecSx.x+vecSy.x, vecLight.y-vecSx.y+vecSy.y, vecLight.z-vecSx.z+vecSy.z, c, 0, 1); pv++;
			pv->set(vecLight.x-vecSx.x-vecSy.x, vecLight.y-vecSx.y-vecSy.y, vecLight.z-vecSx.z-vecSy.z, c, 0, 0); pv++;
			rlist.push_back(&m_Gradient);
		}
	}
	
	P.VB_Unlock	();
	Device.set_xform_world	(Fidentity);
	
	for (DWORD i=0; i<rlist.size(); i++)
	{
    	if (rlist[i]->hShader){
			Device.Shader.set_Shader	(rlist[i]->hShader);
			Device.Primitive.DrawSubset	(P,i*4,4,i*6,2);
        }
	}
}

void CLensFlare::Update( Fvector& sun_dir, Fcolor& color )
{
	vSunDir.mul	(sun_dir,-1);
	
	// color
	Fcolor		a; a.set(color); a.a = 0;
	LightColor.set	(a.get());
	LightColor.a	= 0;
	
	bInit = true;
}
