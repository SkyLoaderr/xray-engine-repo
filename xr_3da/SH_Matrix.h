#ifndef SH_MATRIX_H
#define SH_MATRIX_H
#pragma once

#include "WaveForm.h"

class	ENGINE_API	CStream;
class	ENGINE_API	CFS_Base;

class	ENGINE_API	CMatrix
{
public:
	enum { modeProgrammable=0, modeTCM, modeS_refl, modeC_refl	};
	enum 
	{
		tcmScale		= (1<<0),
		tcmRotate		= (1<<1),
		tcmScroll		= (1<<2),
		tcmFORCE32		= DWORD(-1)
	};
public:
	Fmatrix			xform;

	DWORD			dwReference;
	DWORD			dwFrame;
	DWORD			dwMode;
	DWORD			tcm;				// mask for tc-modifiers
	WaveForm		scaleU, scaleV;
	WaveForm		rotate;
	WaveForm		scrollU,scrollV;

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
		case modeProgrammable:			return;
		case modeTCM: 
			{
				Fmatrix		T;
				float		sU=1,sV=1,t=Device.fTimeGlobal;
				tc_trans	(xform,.5f,.5f);
				if (tcm&tcmScale) {
					sU				= scaleU.Calculate(t);
					sV				= scaleV.Calculate(t);
					T.scale			(sU,sV,1);
					xform.mul2_43	(T);
				}
				if (tcm&tcmRotate) {
					T.rotateZ		(rotate.Calculate(t)*t);
					xform.mul2_43	(T);
				}
				if (tcm&tcmScroll) {
					float u = scrollU.Calculate(t)*t; 
					float v = scrollV.Calculate(t)*t; 
					u-=floorf(u)/sU;
					v-=floorf(v)/sV;
					tc_trans	(T, u, v );
					xform.mul2_43(T);
				}
				tc_trans		(T, -0.5f, -0.5f );
				xform.mul_43	(T);
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
				xform.invert(Device.mView);	
			}
			return;
		default:
			return;
		}
	}

	void			Load		(CStream* fs);
	void			Save		(CFS_Base* fs);
};
#endif