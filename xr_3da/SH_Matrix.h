#ifndef SH_MATRIX_H
#define SH_MATRIX_H
#pragma once

#include "WaveForm.h"

class	ENGINE_API	CStream;
class	ENGINE_API	CFS_Base;

class	ENGINE_API	CMatrix
{
public:
	enum { modeProgrammable=0, modeTCM, modeS_refl, modeC_refl, modeDetail	};
	enum 
	{
		tcmScale		= (1<<0),
		tcmRotate		= (1<<1),
		tcmScroll		= (1<<2),
		tcmFORCE32		= u32(-1)
	};
public:
	Fmatrix			xform;

	u32			dwReference;
	u32			dwFrame;
	u32			dwMode;
    union{
		u32		tcm;				// mask for tc-modifiers
        Flags32	tcm_flags;
    };
	WaveForm		scaleU, scaleV;
	WaveForm		rotate;
	WaveForm		scrollU,scrollV;

	CMatrix			(){	ZeroMemory(this,sizeof(CMatrix));}

	IC void			tc_trans	(Fmatrix& T, float u, float v)
	{
		T.identity	();
		T.m[2][0] = u;
		T.m[2][1] = v;
	}
	IC void			Calculate	()
	{
		if (dwFrame==Device.dwFrame)	return;
		dwFrame		= Device.dwFrame;

		// Switch on mode
		switch (dwMode) {
		case modeProgrammable:			
		case modeDetail:
			return;
		case modeTCM: 
			{
				Fmatrix		T;
				float		sU=1,sV=1,t=Device.fTimeGlobal;
				tc_trans	(xform,.5f,.5f);
				if (tcm&tcmRotate) {
					T.rotateZ		(rotate.Calculate(t)*t);
					xform.mulA_43	(T);
				}
				if (tcm&tcmScale) {
					sU				= scaleU.Calculate(t);
					sV				= scaleV.Calculate(t);
					T.scale			(sU,sV,1);
					xform.mulA_43	(T);
				}
				if (tcm&tcmScroll) {
					float u = scrollU.Calculate(t)*t; 
					float v = scrollV.Calculate(t)*t;
                    u*=sU;
                    v*=sV;
					tc_trans	(T, u, v );
					xform.mulA_43	(T);
				}
				tc_trans		(T, -0.5f, -0.5f );
				xform.mulB_43	(T);
			}
			return;
		case modeS_refl:
			{
				float Ux= .5f*Device.mView._11, Uy= .5f*Device.mView._21, Uz= .5f*Device.mView._31, Uw = .5f;
				float Vx=-.5f*Device.mView._12, Vy=-.5f*Device.mView._22, Vz=-.5f*Device.mView._32, Vw = .5f;
				
				xform._11=Ux; xform._12=Vx; xform._13=0; xform._14=0;
				xform._21=Uy; xform._22=Vy; xform._23=0; xform._24=0;
				xform._31=Uz; xform._32=Vz; xform._33=0; xform._34=0;
				xform._41=Uw; xform._42=Vw; xform._43=0; xform._44=0;
			}
			return;
		case modeC_refl:
			{
				Fmatrix	M	= Device.mView;
				M._41		= 0.f;
				M._42		= 0.f;
				M._43		= 0.f;
				xform.invert(M);	
			}
			return;
		default:
			return;
		}
	}

	IC	BOOL		Similar		(CMatrix& M)		// comare by modes and params
	{
		if (dwMode!=M.dwMode)				return FALSE;
		if (tcm!=M.tcm)						return FALSE;
		if (!scaleU.Similar(M.scaleU))		return FALSE;
		if (!scaleV.Similar(M.scaleV))		return FALSE;
		if (!rotate.Similar(M.rotate))		return FALSE;
		if (!scrollU.Similar(M.scrollU))	return FALSE;
		if (!scrollV.Similar(M.scrollV))	return FALSE;
		return TRUE;
	}

	void			Load		(CStream* fs);
	void			Save		(CFS_Base* fs);
};
#endif