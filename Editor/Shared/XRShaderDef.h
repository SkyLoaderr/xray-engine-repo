#ifndef _XRShaderDef_H_
#define _XRShaderDef_H_
#pragma once
#pragma pack(push,4)

#include "xrShaderTypes.h"

struct ENGINE_API SH_Function
{
	enum EFunction
	{
		fCONSTANT		= 0,
		fSIN,
		fTRIANGLE,
		fSQUARE,
		fSAWTOOTH,
		fINVSAWTOOTH,
		fFORCE32		= DWORD(-1)
	};
	IC float signf		(float t) { return t/fabsf(t); }
	IC float Func		(float t)
	{
		switch (F)
		{
			case fCONSTANT:		return 0;
			case fSIN:			return sinf(t*PI_MUL_2); 
			case fTRIANGLE:		return asinf(sinf((t-0.25f)*PI_MUL_2))/PI_DIV_2;
			case fSQUARE:		return signf(cosf(t*PI));
			case fSAWTOOTH:		return atanf(tanf((t+0.5f)*PI))/PI_DIV_2;
			case fINVSAWTOOTH:	return -(atanf(tanf((t+0.5f)*PI))/PI_DIV_2);
			default:			NODEFAULT; return 0;
		}
	}
public:
	EFunction	F;
	float		arg[4];

	IC float Calculate	(float t)
	{
		// y = arg0 + arg1*func( (time+arg2)*arg3 )
		float x = (t+arg[2])*arg[3];
		return arg[0] + arg[1]*Func(x-floorf(x));
	}
};

struct ENGINE_API SH_StageDef
{
	enum ETCSource
	{
		tcsGeometry		= 0,
		tcsReflection	= 1,
		tcsEnvironment	= 2,
		tcsVector		= 3,
		tcsFORCE32		= DWORD(-1)
	};
	enum ETCAddress
	{
		tcaWarp			= 0,
		tcaMirror		= 1,
		tcaClamp		= 2,
		tcaFORCE32		= DWORD(-1)
	};
public:
	sh_name				Tname;			// Texture Name, without extension, lowercase
	sh_name				Gname;			// GeomChannel name
	ETCSource			tcs;
	Fvector				tcs_vec_center;
	Fvector				tcs_vec_dir;
	DWORD				tcm;			// mask for tc-modifiers
	float				tcm_scaleU,tcm_scaleV;
	float				tcm_rotate;
	float				tcm_scrollU,tcm_scrollV;
	SH_Function			tcm_stretch;
	ETCAddress			tca;
	DWORD				ca1,cop,ca2;	// DX8 defines used
	DWORD				aa1,aop,aa2;

	//---------------------------------------------------------------------------
	void				Init(){ZeroMemory(this,sizeof(SH_StageDef));}
	void				SetMapName		(const char* N)
	{
		VERIFY(strlen(N)<64);
		VERIFY(0==strchr(N,'.'));
		strcpy(Tname,N);
		strlwr(Tname);
	}
	void				SetChannelName	(const char* N)
	{
		VERIFY(strlen(N)<64);
		VERIFY(0==strchr(N,'.'));
		strcpy(Gname,N);
		strlwr(Gname);
	}
};

struct SH_PassDef
{
public:
	struct PFlags 
	{
		unsigned bARef		: 8;
		unsigned bABlend	: 1;
		unsigned bATest		: 1;
		unsigned bFog		: 1;
		unsigned bLighting	: 1;
		unsigned bSpecular	: 1;
	};
	enum	EColorSource
	{
		csNone			= 0,
		csDiffuse		= 1,
		csFactor		= 2,
		csFORCE32		= DWORD(-1)
	};
public:
	PFlags				Flags;
	SH_Function			Factor;
	EColorSource		CS;
	DWORD				Stages_Count;
	SH_StageDef			Stages[sh_STAGE_MAX];
	DWORD				Bsrc,Bdst;		// DX8 defines used

	void				Init(){ZeroMemory(this,sizeof(SH_PassDef));}
	BOOL				NeedLighting()
	{	return Flags.bLighting; }
};

struct SH_ShaderDef
{
public:
	struct	RFlags			// render
	{
		WORD	bZTest		: 1;
		WORD	bZWrite		: 1;
		WORD	bStrictB2F	: 1;
		WORD	iPriority	: 2;
	};
	struct	CFlags			// game & compiler
	{
		WORD	bCollision	: 1;
		WORD	bCastShadow	: 1;
	};
	enum	CLighting
	{
		clNone				= 0,
		clVertex			= 1,
		clLightmap			= 2,
		clFORCE32			= DWORD(-1)
	};
public:
	sh_name					cName;
	sh_name					cComputer;
	DWORD					cTime;
	RFlags					R;
	CFlags					C;
	CLighting				CL;
	float					cl_LM_Density;
	float					cl_VC_Ambient;
	DWORD					Passes_Count;
	SH_PassDef				Passes[sh_PASS_MAX];

    void 					Init(){ 
		ZeroMemory(this,sizeof(SH_ShaderDef)); 
		R.iPriority			= 1;  
	}
	//---------------------------------------------------------------------------
	void					SetName(const char* N)
	{
		// Name
		VERIFY(strlen(N)<64);
		VERIFY(0==strchr(N,'.'));
		strcpy(cName,N);
		strlwr(cName);

		// Computer
		const DWORD comp = MAX_COMPUTERNAME_LENGTH + 1;
		char	buf	[comp];
		DWORD	sz = comp;
		GetComputerName(buf,&sz);
		if (sz > 63) sz=63;
		buf[sz] = 0;
		strcpy(cComputer,buf);

		// Time
		_tzset(); time( (long*)&cTime );
	};

	IC BOOL					NeedLighting	()
	{
		BOOL v = false;
		for (DWORD i=0; i<Passes_Count; i++)
			v |= Passes[i].NeedLighting	();
		return v;
	};
};

#pragma pack(pop)
#endif

