#include "stdafx.h"
#include "xr_efflensflare.h"
#include "xr_trims.h"
#include "xr_tokens.h"
#include "xr_area.h"
#include "environment.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "xr_ini.h"
#include "xr_object.h"

// font
#include "xr_smallfont.h"

struct FlareVertex {
	Fvector p;
	DWORD	color;
	float	tu,tv;

	IC void	set(float x, float y, float z, DWORD c, float u, float v)
	{
		p.set(x,y,z);
		color = c;
		tu=u; tv=v;
	};
};

#define FVF_FlareVertex		( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define MAX_Flares	12
//////////////////////////////////////////////////////////////////////////////
// Globals ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define BLEND_SPEED 3.0f

CLensFlare::CLensFlare()
{
	// Device
	Device.seqDevDestroy.Add	(this);
	Device.seqDevCreate.Add		(this);
	if (Device.bReady) OnDeviceCreate();

	bInit				= false;

	bSourcePresent		= FALSE;
	bFlaresPresent		= FALSE;
	bGradientPresent	= FALSE;
	fBlend				= 0;

    LightColor.set		( 0xFFFFFFFF );
	fGradientDensity	= 0.85f;
}


CLensFlare::~CLensFlare()
{
	if (Device.bReady) OnDeviceDestroy();
	Device.seqDevDestroy.Remove	(this);
	Device.seqDevCreate.Remove	(this);

	for(DWORD i=0; i<Flares.size(); i++) Device.Shader.Delete(Flares[i].hShader);
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
	P.VB_Create(FVF_FlareVertex, MAX_Flares*4, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC);
	P.IB_Create(0, MAX_Flares*2*3, D3DUSAGE_WRITEONLY, Indices);
}

void CLensFlare::OnDeviceDestroy()
{
	P.IB_Destroy();
	P.VB_Destroy();
}

void CLensFlare::SetSource(float fRadius, const char* tex_name){
	CFlare F;
	F.fRadius	= fRadius;
	F.hShader	= Device.Shader.Create("flares",tex_name,FALSE);
	Flares.insert(Flares.begin(),F);
}

void CLensFlare::AddFlare(float fRadius, float fOpacity, float fPosition, const char* tex_name){
	CFlare F;
	F.fRadius	= fRadius;
	F.fOpacity	= fOpacity;
    F.fPosition	= fPosition;
	F.hShader	= Device.Shader.Create("flares",tex_name,FALSE);
	Flares.push_back(F);
}

void CLensFlare::Load( CInifile* pIni, LPSTR section )
{
	LPCSTR		T,R,O,P;
	FILE_NAME	name;
	float r, o, p;
	bSourcePresent = pIni->ReadBOOL ( section,"source" );
	if (bSourcePresent){
		T = pIni->ReadSTRING ( section,"source_texture" );
		r = pIni->ReadFLOAT	 ( section,"source_radius" );
		SetSource(r,T);
	}
	bFlaresPresent = pIni->ReadBOOL ( section,"flares" );
	if (bFlaresPresent){
		T = pIni->ReadSTRING ( section,"flare_textures" );
		R = pIni->ReadSTRING ( section,"flare_radius" );
		O = pIni->ReadSTRING ( section,"flare_opacity");
		P = pIni->ReadSTRING ( section,"flare_position");
		DWORD tcnt = _GetItemCount(T);
		for (DWORD i=0; i<tcnt; i++){
			_GetItem(R,i,name,""); r=(float)atof(name);
			_GetItem(O,i,name,""); o=(float)atof(name);
			_GetItem(P,i,name,""); p=(float)atof(name);
			_GetItem(T,i,name,""); 
			AddFlare(r,o,p,name);
		}
	}
	bGradientPresent= pIni->ReadTOKEN( section, "gradient", BOOL_token );
	if (bGradientPresent)
		fGradientDensity = pIni->ReadFLOAT( section, "gradient_density");
	bInit			= false;
}

void CLensFlare::OnMove(){
	VERIFY(bInit);
	//
	// Compute center and axis of flares
	//
	float fDot;

	Fvector vecPos;

	Fmatrix	matEffCamPos;
	matEffCamPos.identity();
	matEffCamPos.i.set(Device.vCameraRight);
	matEffCamPos.j.set(Device.vCameraTop);
	matEffCamPos.k.set(Device.vCameraDirection);

	// Calculate our position and direction
	vecPos.set(Device.vCameraPosition);
	vecDir.set(0.0f, 0.0f, 1.0f);
	matEffCamPos.transform_dir(vecDir);
	vecDir.normalize();

	// Figure out of light (or flare) might be visible
	vecLight.set(vSunDir); 
	vecLight.normalize();

	fDot = vecLight.dotproduct(vecDir);

	if(fDot <= 0.01f){	bRender = false; return;} else bRender = true;

	// Calculate the point directly in front of us, on the far clip plane
	float 	fDistance	= pCreator->Environment.Current.Far*0.75f;
	vecCenter.mul(vecDir, fDistance);
	vecCenter.add(vecPos);
	// Calculate position of light on the far clip plane
	vecLight.mul(fDistance / fDot);
	vecLight.add(vecPos);
	// Compute axis which goes from light through the center of the screen
	vecAxis.sub(vecLight, vecCenter);

	//
	// Figure out if light is behind something else
	//
	//------------------------------------------------
	//------------------------------------------------
	//
	// Draw light source
	//
	// Figure out screen X and Y axes in model coordinates
	vecX.set(1.0f, 0.0f, 0.0f);
	matEffCamPos.transform_dir(vecX);
	vecX.normalize();
	vecY.crossproduct(vecX, vecDir);

	pCreator->CurrentEntity()->CFORM()->Enable( pCreator->CurrentEntity()->bVisible );
	if ( pCreator->ObjectSpace.RayTest( Device.vCameraPosition, vSunDir) )
	{
		fBlend = fBlend - BLEND_SPEED * Device.fTimeDelta;
	}else{
		fBlend = fBlend + BLEND_SPEED * Device.fTimeDelta;
	}
	pCreator->CurrentEntity()->CFORM()->EnableRollback( );
	clamp( fBlend, 0.0f, 1.0f );

// gradient
	if (bGradientPresent){
		Fvector				scr_pos;
		float w =	vecLight.x*Device.mFullTransform._14 + 
					vecLight.y*Device.mFullTransform._24 + 
					vecLight.z*Device.mFullTransform._34 + Device.mFullTransform._44;
		w = fabsf(w);
		Device.mFullTransform.transform	( scr_pos, vecLight );
		float kx = 1, ky = 1;
		float sun_blend		= 0.5f;
		float sun_max		= 1.5f;
		scr_pos.y			*= -1;

		if (fabsf(scr_pos.x) > sun_blend)	kx = ((sun_max - (float)fabsf(scr_pos.x))) / (sun_max - sun_blend);
		if (fabsf(scr_pos.y) > sun_blend)	ky = ((sun_max - (float)fabsf(scr_pos.y))) / (sun_max - sun_blend);
		float    cl			= kx * ky * fGradientDensity * 0.3f * fBlend;

		if (!((fabsf(scr_pos.x) > sun_max) || (fabsf(scr_pos.y) > sun_max)))
			pCreator->Environment.SetGradient(cl);
	}
}

void CLensFlare::Render(BOOL bSun, BOOL bFlares){
	VERIFY(bInit);
	if (!bRender) return;

	Fcolor dwLight;
	Fcolor color;
	Fvector vec, vecSx, vecSy;
	Fvector vecDx, vecDy;

	dwLight.set		( LightColor );
	vector<CFlare*>	rlist;

	FlareVertex *pv = (FlareVertex*) P.VB_Lock(D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);

	float 	fDistance	= pCreator->Environment.Current.Far*0.75f;

	if (bSourcePresent){
		vecSx.mul(vecX, Flares[0].fRadius*fDistance);
		vecSy.mul(vecY, Flares[0].fRadius*fDistance);

		color.set		( dwLight );

		if (bSun){
			DWORD c = color.get();
			pv->set(vecLight.x+vecSx.x-vecSy.x, vecLight.y+vecSx.y-vecSy.y, vecLight.z+vecSx.z-vecSy.z, c, 1, 1); pv++;
			pv->set(vecLight.x+vecSx.x+vecSy.x, vecLight.y+vecSx.y+vecSy.y, vecLight.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
			pv->set(vecLight.x-vecSx.x+vecSy.x, vecLight.y-vecSx.y+vecSy.y, vecLight.z-vecSx.z+vecSy.z, c, 0, 0); pv++;
			pv->set(vecLight.x-vecSx.x-vecSy.x, vecLight.y-vecSx.y-vecSy.y, vecLight.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
			rlist.push_back(&Flares[0]);
		}
	}

	if ((fBlend>=EPS_L)&&bFlares&&bFlaresPresent){
		vecDx.normalize(vecAxis);
		vecDy.crossproduct(vecDx, vecDir);
		DWORD st = bSourcePresent?1:0;
		for (DWORD i=st; i<Flares.size(); i++){
			CFlare&	F		= Flares[i];
			vec.mul(vecAxis, F.fPosition);
			vec.add(vecCenter);
			vecSx.mul(vecDx, F.fRadius*fDistance);
			vecSy.mul(vecDy, F.fRadius*fDistance);
			float    cl		= F.fOpacity * fBlend;
			color.set		( dwLight );
			color.mul_rgb	( cl );
			DWORD c=color.get();
			pv->set(vec.x+vecSx.x-vecSy.x, vec.y+vecSx.y-vecSy.y, vec.z+vecSx.z-vecSy.z, c, 1, 1); pv++;
			pv->set(vec.x+vecSx.x+vecSy.x, vec.y+vecSx.y+vecSy.y, vec.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
			pv->set(vec.x-vecSx.x+vecSy.x, vec.y-vecSx.y+vecSy.y, vec.z-vecSx.z+vecSy.z, c, 0, 0); pv++;
			pv->set(vec.x-vecSx.x-vecSy.x, vec.y-vecSx.y-vecSy.y, vec.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
			rlist.push_back(&Flares[i]);
		}
	}

	P.VB_Unlock();

	HW.pDevice->SetTransform	( D3DTS_WORLD, precalc_identity.d3d());

	for (DWORD i=0; i<rlist.size(); i++){
		Device.Shader.Set(rlist[i]->hShader);
		Device.Primitive.DrawSubset(P,i*4,4,i*6,2);
	}
}

void CLensFlare::Update( Fvector& sun_dir, float view_dist, Fcolor& color )
{
	vSunDir.mul	(sun_dir,-1);

	// color
	Fcolor		a; a.set(color); a.a = 0;
	LightColor.set(a.get_maximized());
	LightColor.a = 0;

	bInit = true;
}
