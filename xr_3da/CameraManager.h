// CameraManager.h: interface for the CCameraManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "CameraDefs.h"

struct SPPInfo {
	struct SColor{
		float r, g, b;
		SColor					(){}
		SColor					(float _r, float _g, float _b):r(_r),g(_g),b(_b){}
		IC operator u32()										{
			int		_r	= clampr	(iFloor(r*255.f+.5f),0,255);
			int		_g	= clampr	(iFloor(g*255.f+.5f),0,255);
			int		_b	= clampr	(iFloor(b*255.f+.5f),0,255);
			return color_rgba		(_r,_g,_b,0);
		}
		IC SColor& operator +=	(const SColor &ppi)				{
			r += ppi.r; g += ppi.g; b += ppi.b; 
			return *this;
		}
		IC SColor& operator -=	(const SColor &ppi)				{
			r -= ppi.r; g -= ppi.g; b -= ppi.b; 
			return *this;
		}
		IC SColor& set			(float _r, float _g, float _b)	{
			r=_r;g=_g;b=_b;
			return *this;
		}
	};
	float		blur, gray;
	struct SDuality { 
		float h, v; 
		SDuality				(){}
		SDuality				(float _h, float _v):h(_h),v(_v){}
		IC SDuality& set		(float _h, float _v)			{
			h=_h;v=_v;
			return *this;
		}
	} duality;
	struct SNoise	{
		float		intensity, grain;
		float		fps;
		SNoise					(){}
		SNoise					(float _i, float _g, float _f):intensity(_i),grain(_g),fps(_f){}
		IC SNoise& set			(float _i, float _g, float _f){
			intensity=_i;grain=_g;fps=_f;
			return *this;
		}
	} noise;

	SColor		color_base;
	SColor		color_gray;
	SColor		color_add;

	IC SPPInfo& operator += (const SPPInfo &ppi) {
		blur		+= ppi.blur;
		gray		+= ppi.gray;
		duality.h	+= ppi.duality.h; duality.v += ppi.duality.v;
		noise.intensity += ppi.noise.intensity; noise.grain += ppi.noise.grain;
		noise.fps	+= ppi.noise.fps;
		color_base	+= ppi.color_base;
		color_gray	+= ppi.color_gray;
		color_add	+= ppi.color_add;
		return *this;
	}
	IC SPPInfo& operator -= (const SPPInfo &ppi) {
		blur		-= ppi.blur;
		gray		-= ppi.gray;
		duality.h	-= ppi.duality.h; duality.v -= ppi.duality.v;
		noise.intensity -= ppi.noise.intensity; noise.grain -= ppi.noise.grain;
		noise.fps	-= ppi.noise.fps;
		color_base	-= ppi.color_base;
		color_gray	-= ppi.color_gray;
		color_add	-= ppi.color_add;
		return *this;
	}
	void normalize		();
	SPPInfo				()
	{
		blur = gray = duality.h = duality.v = 0;
		noise.intensity=0; noise.grain = 1; noise.fps = 10;
		color_base.set	(.5f,	.5f,	.5f);
		color_gray.set	(.333f, .333f,	.333f);
		color_add.set	(0.f,	0.f,	0.f);
	}
	IC SPPInfo&	lerp(const SPPInfo& def, const SPPInfo& to, float factor)
	{
		SPPInfo& pp = *this;

		pp.duality.h		+= def.duality.h		+ (to.duality.h			- def.duality.h)		* factor; 			
		pp.duality.v		+= def.duality.v		+ (to.duality.v			- def.duality.v)		* factor;
		pp.gray				+= def.gray				+ (to.gray				- def.gray)				* factor;
		pp.blur				+= def.blur				+ (to.blur				- def.blur)				* factor;
		pp.noise.intensity	+= def.noise.intensity	+ (to.noise.intensity	- def.noise.intensity)	* factor;
		pp.noise.grain		+= def.noise.grain		+ (to.noise.grain		- def.noise.grain)		* factor;
		pp.noise.fps		+= def.noise.fps		+ (to.noise.fps			- def.noise.fps)		* factor;	

		pp.color_base.set	(
			def.color_base.r	+ (to.color_base.r - def.color_base.r) * factor, 
			def.color_base.g	+ (to.color_base.g - def.color_base.g) * factor, 
			def.color_base.b	+ (to.color_base.b - def.color_base.b) * factor
			);

		pp.color_gray.set	(
			def.color_gray.r	+ (to.color_gray.r - def.color_gray.r) * factor, 
			def.color_gray.g	+ (to.color_gray.g - def.color_gray.g) * factor, 
			def.color_gray.b	+ (to.color_gray.b - def.color_gray.b) * factor
			);

		pp.color_add.set	(
			def.color_add.r	+ (to.color_add.r - def.color_add.r) * factor, 
			def.color_add.g	+ (to.color_add.g - def.color_add.g) * factor, 
			def.color_add.b	+ (to.color_add.b - def.color_add.b) * factor
			);
		return *this;
	}
};

DEFINE_VECTOR				(CEffector*,EffectorVec,EffectorIt);
DEFINE_VECTOR				(CEffectorPP*,EffectorPPVec,EffectorPPIt);

class ENGINE_API CCameraManager
{
	Fvector					vPosition;
	Fvector					vDirection;
	Fvector					vNormal;
	Fvector					vRight;

	EffectorVec				m_Effectors;
	EffectorPPVec			m_EffectorsPP;

	float					fFov;
	float					fFar;
	float					fAspect;
	
	Fmatrix					unaffected_mView;
	Fvector					unaffected_vPosition;
	Fvector					unaffected_vDirection;
	Fvector					unaffected_vNormal;
	Fvector					unaffected_vRight;
	
	Fvector					affected_vPosition;
	Fvector					affected_vDirection;
	Fvector					affected_vNormal;
	Fvector					affected_vRight;

	SPPInfo					pp_affected;
public:
	void					Dump				(void);
	CEffector*				AddEffector			(CEffector*			ef);
	CEffector*				GetEffector			(EEffectorType		type);
	void					RemoveEffector		(EEffectorType		type);

	CEffectorPP*			GetEffector			(EEffectorPPType	type);
	CEffectorPP*			AddEffector			(CEffectorPP*		ef);
	void					RemoveEffector		(EEffectorPPType	type);

	IC Fmatrix&				unaffected_View		()	{ return unaffected_mView;		}
	IC Fvector&				unaffected_Pos		()	{ return unaffected_vPosition;	}
	IC Fvector&				unaffected_Dir		()	{ return unaffected_vDirection;	}
	IC Fvector&				unaffected_Up		()	{ return unaffected_vNormal;	}
	IC Fvector&				unaffected_Right	()	{ return unaffected_vRight;		}
	IC void					unaffected_Matrix	(Fmatrix& M)	
	{	M.set(unaffected_vRight,unaffected_vNormal,unaffected_vDirection,unaffected_vPosition);	}

	IC Fvector&				affected_Pos		()	{ return affected_vPosition;	}
	IC Fvector&				affected_Dir		()	{ return affected_vDirection;	}
	IC Fvector&				affected_Up			()	{ return affected_vNormal;		}
	IC Fvector&				affected_Right		()	{ return affected_vRight;		}
	IC void					affected_Matrix		(Fmatrix& M)	
	{	M.set(affected_vRight,affected_vNormal,affected_vDirection,affected_vPosition);	}

	IC Fmatrix&				render_View			()	{ return Device.mView; }
	IC Fvector&				render_Pos			()	{ return vPosition;	}
	IC Fvector&				render_Dir			()	{ return vDirection;}
	IC Fvector&				render_Up			()	{ return vNormal;	}
	IC Fvector&				render_Right		()	{ return vRight;	}
	IC void					render_Matrix		(Fmatrix& M)	
	{	M.set(vRight,vNormal,vDirection,vPosition);	}

	void					Update				(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, u32 flags=0);
	void					Update				(const CCameraBase* C);
	void					ResetPP				();
	CCameraManager();
	~CCameraManager();
};
ENGINE_API extern SPPInfo					pp_identity;
ENGINE_API extern SPPInfo					pp_zero;

ENGINE_API extern float psCamInert;
ENGINE_API extern float psCamSlideInert;

#endif // !defined(AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_)
