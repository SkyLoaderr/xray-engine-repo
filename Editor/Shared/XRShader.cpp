// XRShader.cpp: implementation of the CXRShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "XRShader.h"

#define DUMMY

//////////////////////////////////////////////////////////////////////
// Calculations //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IC void tc_trans(Fmatrix& T, float u, float v)
{
   	T.identity	();
    T.m[2][0] = u;
    T.m[2][1] = v;
}
void SH_Stage::Calculate(float t)
{
	if (!bNeedXFORM) return;
	// Process TC source
	switch (tcs) {
	case SH_StageDef::tcsGeometry:		XForm.identity();	break;
	case SH_StageDef::tcsReflection:	XForm.identity();	break;
	case SH_StageDef::tcsEnvironment:
		{
			float Ux= .5f*Device.mView._11, Uy= .5f*Device.mView._21, Uz= .5f*Device.mView._31, Uw = .5f;
			float Vx=-.5f*Device.mView._12, Vy=-.5f*Device.mView._22, Vz=-.5f*Device.mView._32, Vw = .5f;
			
			XForm._11=Ux; XForm._12=Vx; XForm._13=0; XForm._14=0;
			XForm._21=Uy; XForm._22=Vy; XForm._23=0; XForm._24=0;
			XForm._31=Uz; XForm._32=Vz; XForm._33=0; XForm._34=0;
			XForm._41=Uw; XForm._42=Vw; XForm._43=0; XForm._44=0;
		}
		break;
	case SH_StageDef::tcsVector:
		{
			Fmatrix T;
			T.invert	(Device.mView);
			XForm.mul_43(mPlanar,T);
		}
		break;
	default:			XForm.identity();	break;
	}
	
	// Process TC mod
	if (tcm) {
		Fmatrix T;
        tc_trans	(T, 0.5f, 0.5f );
		XForm.mul_43(T);
		if (tcm&SH_StageDef::tcmScale) {
			T.scale		(tcm_scaleU,tcm_scaleV,1);
			XForm.mul2_43(T);
		}
		if (tcm&SH_StageDef::tcmRotate) {
			T.rotateZ	(tcm_rotate*t);
			XForm.mul2_43(T);
		}
		if (tcm&SH_StageDef::tcmScroll) {
        	float u = tcm_scrollU*t; 
            float v = tcm_scrollV*t; 
			if (tcm&SH_StageDef::tcmScale) {
    	    	u-=floorf(u)/tcm_scaleU;
	        	v-=floorf(v)/tcm_scaleV;
    	    } else {
    	    	u-=floorf(u);
	        	v-=floorf(v);
            }
            tc_trans	(T, u, v );
			XForm.mul2_43(T);
		}
		if (tcm&SH_StageDef::tcmStretch) {
			float v = tcm_stretch.Calculate(t);
			T.scale		(v,v,1);
			XForm.mul2_43(T);
		}
        tc_trans		(T, -0.5f, -0.5f );
		XForm.mul_43	(T);
	}
}

void SH_Pass::Calculate(float t)
{
	if (bCalcFactor)
	{
		int		v	= iFloor(Factor.Calculate(t)*255.f);
		clamp	(v,0,255);
		DWORD	u	= v;
		Factor_Calculated = D3DCOLOR_RGBA(u,u,u,u);
	}
	
	for (DWORD I=0; I<Stages_Count; I++)
		Stages[I].Calculate(t);
}

//////////////////////////////////////////////////////////////////////
// Texture creation and assotiation //////////////////////////////////
//////////////////////////////////////////////////////////////////////
int ParseName(const char* N, int iOffset)
{
	if (0==strcmp(N,"$lmap"))	{ R_ASSERT(iOffset); return 0; }
	if (0==strcmp(N,"$null"))	return -1;
	if (0==strcmp(N,"$base0"))	return 0 + iOffset;
	if (0==strcmp(N,"$base1"))	return 1 + iOffset;
	if (0==strcmp(N,"$base2"))	return 2 + iOffset;
	if (0==strcmp(N,"$base3"))	return 3 + iOffset;
	if (0==strcmp(N,"$base4"))	return 4 + iOffset;
	if (0==strcmp(N,"$base5"))	return 5 + iOffset;
	if (0==strcmp(N,"$base6"))	return 6 + iOffset;
	if (0==strcmp(N,"$base7"))	{ R_ASSERT(0==iOffset); return 7 + iOffset; }
	return -1;
}
void CXRShader::CompileTextures(tex_names &T, tex_handles &H)
{
	H.resize(Passes_Count);
	for (DWORD i=0; i<Passes_Count; i++)
		Passes[i].CompileTextures(this, i, T, H);
}
void SH_Pass::CompileTextures(CXRShader* S, DWORD ID, tex_names& T, tex_handles& H)
{
	// Compile stages
	for (DWORD I=0; I<Stages_Count; I++)
		Stages[I].CompileTextures(S,I,T,H[ID]);
}
void SH_Stage::CompileTextures(CXRShader* S, DWORD ID, tex_names& T, tex_vector& H )
{
	// Parse name
	int	iOffset	= S->Flags.bLightmaps?1:0;
	int			tid		= ParseName(Tname,iOffset);
	char*		Name	= Tname;
	if (tid>=0)	{
		if (tid<int(T.size()))	Name	= T[tid];
		else	{
			Name = "$null";
			Msg("! ERROR: Not enought textures for shader. Try level rebuild.");
		}
	}
	H.push_back(Device.Shader._CreateTexture(Name));
}

//////////////////////////////////////////////////////////////////////
// Compilation startup (general definitions and constants ) //////////
//////////////////////////////////////////////////////////////////////
void CXRShader::CompileInit(SH_ShaderDef& DEF)
{
	strcpy(cName,DEF.cName);

	// general flags
	dwFrame				= 0;
	Flags.bStrictB2F	= DEF.R.bStrictB2F;
	Flags.bLighting		= DEF.NeedLighting();
	Flags.bLightmaps	= (DEF.CL==SH_ShaderDef::clLightmap);
	Flags.iPriority		= DEF.R.iPriority;
	Passes_Count		= DEF.Passes_Count;

	// passes
	for (DWORD i=0; i<Passes_Count; i++)
		Passes[i].CompileInit(DEF.Passes[i]);
}
void SH_Pass::CompileInit(SH_PassDef& DEF)
{
	//*** General flags
	bCalcFactor		= (DEF.CS==SH_PassDef::csFactor);
	Factor			= DEF.Factor;
	Stages_Count	= DEF.Stages_Count;

	//*** Compile stages
	for (DWORD I=0; I<Stages_Count; I++)
		Stages[I].CompileInit(DEF.Stages[I]);

}
void SH_Stage::CompileInit(SH_StageDef& DEF)
{
	// Detect if XForm is needed at all
	bNeedXFORM = FALSE;
	if (DEF.tcs != SH_StageDef::tcsGeometry)	bNeedXFORM = TRUE;
	if (DEF.tcm != 0)							bNeedXFORM = TRUE;

	// General
	XForm.identity		();
	mPlanar.identity	();

	// XForm params
	tcs			= DEF.tcs;
	tcm			= DEF.tcm;
	tcm_scaleU	= DEF.tcm_scaleU;
	tcm_scaleV	= DEF.tcm_scaleV;
	tcm_rotate	= DEF.tcm_rotate;
	tcm_scrollU	= DEF.tcm_scrollU;
	tcm_scrollV = DEF.tcm_scrollV;
	tcm_stretch	= DEF.tcm_stretch;

	// Parse geom channel
	strcpy(Tname,DEF.Tname);
}
//////////////////////////////////////////////////////////////////////
// Compilation ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CXRShader::Compile(SH_ShaderDef& DEF)
{
	CompileInit(DEF);

	// info
	char* S = ctime( (long *)&DEF.cTime );
	S[strlen(S)-1] = 0;
	Msg("* Compiling shader \"%s\", created on \"%s\", %s",
		cName,DEF.cComputer, S);

	// passes
	CSimulatorRS		RS;
	for (DWORD i=0; i<Passes_Count; i++)
		Passes[i].Compile(DEF,i,RS);
}

IC DWORD BC(DWORD V) { return V?1:0; }
void SH_Pass::Compile(SH_ShaderDef& SHADER, DWORD ID, CSimulatorRS& RS)
{
	SH_PassDef& DEF = SHADER.Passes[ID];

	//*** Create StateBlock
	R_ASSERT(Device.bReady);
	R_CHK	(HW.pDevice->BeginStateBlock());
	RS.SetRS(D3DRS_ZFUNC, SHADER.R.bZTest?D3DCMP_LESSEQUAL:D3DCMP_ALWAYS);
	if (ID==0)	RS.SetRS(D3DRS_ZWRITEENABLE, BC(SHADER.R.bZWrite));
	else		RS.SetRS(D3DRS_ZWRITEENABLE, BC(0));

	// Blend mode
	RS.SetRS(D3DRS_ALPHABLENDENABLE,	BC(DEF.Flags.bABlend));
#ifndef DUMMY    
	if (DEF.Flags.bABlend) {
#endif    
		RS.SetRS(D3DRS_SRCBLEND,		DEF.Flags.bABlend?DEF.Bsrc:D3DBLEND_ONE);
		RS.SetRS(D3DRS_DESTBLEND,		DEF.Flags.bABlend?DEF.Bdst:D3DBLEND_ZERO);
		RS.SetRS(D3DRS_ALPHATESTENABLE, BC(DEF.Flags.bATest));
		if (DEF.Flags.bATest)
			RS.SetRS(D3DRS_ALPHAREF,	DWORD(DEF.Flags.bARef));
#ifndef DUMMY            
	}
#endif    

	// Lighting
	BOOL bLight = (DEF.CS==SH_PassDef::csDiffuse)?DEF.Flags.bLighting:FALSE;
	RS.SetRS(D3DRS_LIGHTING,			BC(bLight));
#ifndef DUMMY            
	if (bLight) {
#endif    
		RS.SetRS(D3DRS_SPECULARENABLE,	BC(bLight?DEF.Flags.bSpecular:FALSE));
#ifndef DUMMY
	}
#endif    

	// Fog
	RS.SetRS(D3DRS_FOGENABLE,			BC(DEF.Flags.bFog));

	// Compile stages
	for (DWORD I=0; I<Stages_Count; I++)
		Stages[I].Compile(SHADER,DEF.Stages[I],I,RS);

	RS.SetTSS(Stages_Count,D3DTSS_COLOROP,D3DTOP_DISABLE);
#ifdef DUMMY
	RS.SetTSS(Stages_Count,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
#endif

	R_CHK	(HW.pDevice->EndStateBlock(&SB));
}
void SH_Stage::Compile(SH_ShaderDef& SHADER, SH_StageDef& DEF, DWORD ID, CSimulatorRS& RS)
{
	// Parse geom channel
	int	iOffset	= (SHADER.CL==SH_ShaderDef::clLightmap)?1:0;
	int iChannel = ParseName(DEF.Gname,iOffset);

	// Record wrap mode
	switch (DEF.tca)
	{
	case SH_StageDef::tcaWarp:	
		RS.SetTSS(ID,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
		RS.SetTSS(ID,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
		break;
	case SH_StageDef::tcaMirror:
		RS.SetTSS(ID,D3DTSS_ADDRESSU,D3DTADDRESS_MIRROR);
		RS.SetTSS(ID,D3DTSS_ADDRESSV,D3DTADDRESS_MIRROR);
		break;
	case SH_StageDef::tcaClamp:
		RS.SetTSS(ID,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
		RS.SetTSS(ID,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
		break;
	default:
		R_ASSERT(0=="Unknown addressing");
	}

	// Record channel
	switch (DEF.tcs)
	{
	case SH_StageDef::tcsGeometry: 
		R_ASSERT(iChannel>=0);
		RS.SetTSS(ID,D3DTSS_TEXTURETRANSFORMFLAGS,bNeedXFORM?D3DTTFF_COUNT2:D3DTTFF_DISABLE);
		RS.SetTSS(ID,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU|iChannel);
		break;
	case SH_StageDef::tcsReflection:
		RS.SetTSS(ID,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
		RS.SetTSS(ID,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR|ID);
		break;
	case SH_StageDef::tcsEnvironment:
		RS.SetTSS(ID,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
		RS.SetTSS(ID,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACENORMAL|ID);
		break;
	case SH_StageDef::tcsVector:
		{
			RS.SetTSS(ID,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
			RS.SetTSS(ID,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION|ID);
			
			// create planar xform matrix
			Fvector		up,right,y;
			y.set		(0,1,0);
			if (fabsf(DEF.tcs_vec_dir.y)>.99f) y.set(1,0,0);
			right.crossproduct		(y,DEF.tcs_vec_dir);
			up.crossproduct			(DEF.tcs_vec_dir,right);
			mPlanar.build_camera_dir(DEF.tcs_vec_center,DEF.tcs_vec_dir,up);
		}
		break;
	default:
		R_ASSERT(0=="Unknown UV channel");
	}

	// Record Operations
	RS.TSS.SetColor	(ID,DEF.ca1,DEF.cop,DEF.ca2);
	RS.TSS.SetAlpha	(ID,DEF.aa1,DEF.aop,DEF.aa2);
}

